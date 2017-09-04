#include "seims.h"
#include "SUR_MR.h"

SUR_MR::SUR_MR(void) : m_nCells(-1), DT_HS(-1), nSoilLayers(-1), t_soil(NODATA_VALUE),
                       K_run(NODATA_VALUE), P_max(NODATA_VALUE),
    //T_snow(NODATA_VALUE), m_t0(NODATA_VALUE), m_snowAccu(NULL), m_snowMelt(NULL),
                       s_frozen(NODATA_VALUE), Runoff_co(NULL), Moist_in(NULL), TMEAN(NULL),
    // soilthick(NULL) ,m_fieldCap(NULL),m_wiltingPoint(NULL), m_porosity(NULL),
                       sol_awc(NULL), sol_sumul(NULL), soillayers(NULL),
                       NEPR(NULL), DPST(NULL), SOTE(NULL), pot_vol(NULL), impound_trig(NULL),
                       EXCP(NULL), INFIL(NULL), solst(NULL), solsw(NULL) {
}

SUR_MR::~SUR_MR(void) {
    if (EXCP != NULL) Release1DArray(EXCP);
    if (INFIL != NULL) Release1DArray(INFIL);
    if (solst != NULL) Release2DArray(m_nCells, solst);
    if (solsw != NULL) Release1DArray(solsw);
}

void SUR_MR::CheckInputData(void) {
    if (m_date < 0) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "You have not set the time.");
    }
    if (DT_HS < 0) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "You have not set the time step.");
    }
    if (m_nCells <= 0) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The cell number of the input can not be less than zero.");
    }
    if (Moist_in == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The initial soil moisture can not be NULL.");
    }
    if (Runoff_co == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The potential runoff coefficient can not be NULL.");
    }
    if (FloatEqual(t_soil, NODATA_VALUE)) {
        throw ModelException(MID_SUR_MR, "CheckInputData",
                             "The soil freezing temperature of the input data can not be NODATA.");
    }
    //if (FloatEqual(m_t0, NODATA_VALUE))
    //    throw ModelException(MID_SUR_MR, "CheckInputData",
    //                         "The snow melt threshold temperature of the input data can not be NODATA.");
    //if (FloatEqual(T_snow, NODATA_VALUE))
    //    throw ModelException(MID_SUR_MR, "CheckInputData",
    //                         "The snowfall temperature of the input data can not be NODATA.");
    if (FloatEqual(K_run, NODATA_VALUE)) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The runoff exponent of the input data can not be NODATA.");
    }
    if (FloatEqual(P_max, NODATA_VALUE)) {
        throw ModelException(MID_SUR_MR, "CheckInputData",
                             "The maximum P corresponding to runoffCo of the input data can not be NODATA.");
    }
    if (FloatEqual(s_frozen, NODATA_VALUE)) {
        throw ModelException(MID_SUR_MR, "CheckInputData",
                             "The frozen soil moisture of the input data can not be NODATA.");
    }
    if (sol_awc == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The available water amount can not be NULL.");
    }
    //   if (m_fieldCap == NULL)
    //       throw ModelException(MID_SUR_MR, "CheckInputData",
    //	"The water content of soil at field capacity of the input data can not be NULL.");
    //if (m_wiltingPoint == NULL)
    //	throw ModelException(MID_SUR_MR, "CheckInputData",
    //	"The water content of soil at wilting point of the input data can not be NULL.");
    //if (soilthick == NULL)
    //	throw ModelException(MID_SUR_MR, "CheckInputData", "the input data: Soil thickness can not be NULL.");
    //   if (m_porosity == NULL)
    //       throw ModelException(MID_SUR_MR, "CheckInputData", "The soil porosity of the input data can not be NULL.");
    if (TMEAN == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData",
                             "The mean air temperature of the input data can not be NULL.");
    }
    if (SOTE == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The soil temperature of the input data can not be NULL.");
    }
    if (NEPR == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The net precipitation can not be NULL.");
    }
    if (DPST == NULL) {
        throw ModelException(MID_SUR_MR, "CheckInputData", "The depression capacity can not be NULL.");
    }
    //if (m_snowAccu == NULL)
    //    throw ModelException(MID_SUR_MR, "CheckInputData", "The snow accumulation can not be NULL.");
    //if (m_snowMelt == NULL)
    //    throw ModelException(MID_SUR_MR, "CheckInputData", "The snow melt can not be NULL.");
}

void SUR_MR::initialOutputs() {
    if (m_nCells <= 0) {
        throw ModelException(MID_SUR_MR, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    // allocate the output variables
    if (EXCP == NULL) {
        Initialize1DArray(m_nCells, EXCP, 0.f);
        Initialize1DArray(m_nCells, INFIL, 0.f);
        Initialize1DArray(m_nCells, solsw, 0.f);
        Initialize2DArray(m_nCells, nSoilLayers, solst, NODATA_VALUE);
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            for (int j = 0; j < (int) soillayers[i]; j++) { /// mm
                if (Moist_in[i] >= 0.f && Moist_in[i] <= 1.f && sol_awc[i][j] >= 0.f) {
                    solst[i][j] = Moist_in[i] * sol_awc[i][j];
                } else {
                    solst[i][j] = 0.f;
                }
                solsw[i] += solst[i][j];
            }
        }
    }
}

int SUR_MR::Execute() {
    CheckInputData();
    initialOutputs();
    P_max = P_max * DT_HS / 86400.f;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        /// Snow melt should be considered in SnowMelt module, this may be redundant. By LJ
        ////account for the effects of snow melt and soil temperature
        //float t = 0.f, snowMelt = 0.f, snowAcc = 0.f, sd = 0.f;
        //if (TMEAN != NULL) t = TMEAN[i];
        //if (m_snowMelt != NULL)snowMelt = m_snowMelt[i];
        //if (m_snowAccu != NULL)snowAcc = m_snowAccu[i];
        //if (DPST != NULL)sd = DPST[i];

        //// snow, without snow melt
        //if (TMEAN[i] <= T_snow)
        //    hWater = 0.0f;
        //// rain on snow, no snow melt
        //else if (TMEAN[i] > T_snow && TMEAN[i] <= m_t0 && snowAcc > NEPR[i])
        //    hWater = 0.0f;
        //else
        //    hWater = NEPR[i] + snowMelt + DPST[i];
        float hWater = 0.f;
        hWater = NEPR[i] + DPST[i];
        if (hWater > 0.f) {
            /// in the new version, sm is replaced by solsw. By lj
            /// por is replaced by sol_sumul which is calculated by (sat - wp)
            //float sm = 0.f, por = 0.f;
            //for (int j = 0; j < (int)soillayers[i]; j++)
            //{
            //    sm += solst[i][j]; ///  mm H2O
            //    por += m_porosity[i][j] * soilthick[i][j]; /// unit can be seen as mm H2O 
            //}
            // float smFraction = min(sm / por, 1.f);

            /// update total soil water content
            solsw[i] = 0.f;
            for (int ly = 0; ly < (int) soillayers[i]; ly++) {
                solsw[i] += solst[i][ly];
            }
            float smFraction = min(solsw[i] / sol_sumul[i], 1.f);
            // for frozen soil, no infiltration will occur
            if (SOTE[i] <= t_soil && smFraction >= s_frozen) {
                EXCP[i] = NEPR[i];
                INFIL[i] = 0.f;
            } else {
                float alpha = K_run - (K_run - 1.f) * hWater / P_max;
                if (hWater >= P_max) {
                    alpha = 1.f;
                }

                //runoff percentage
                float runoffPercentage;
                if (Runoff_co[i] > 0.99f) {
                    runoffPercentage = 1.f;
                } else {
                    runoffPercentage = Runoff_co[i] * pow(smFraction, alpha);
                }

                float surfq = hWater * runoffPercentage;
                if (surfq > hWater) surfq = hWater;
                INFIL[i] = hWater - surfq;
                EXCP[i] = surfq;


                /// TODO: Why calculate surfq first, rather than infiltration first?
                ///       I think we should calculate infiltration first, until saturation,
                ///       then surface runoff should be calculated. By LJ.

                // check the output data, In my view, we should avoid this situation to occur. By LJ.
                //if (INFIL[i] != INFIL[i] || INFIL[i] < 0.f)
                //{
                //    //string datestr = getDate(&m_date);
                //    ostringstream oss;
                //    oss << "Cell id:" << i << "\tPrecipitation(mm) = " << NEPR[i] << "\thwater = " << hWater
                //    << "\tpercentage:" << runoffPercentage << "\tmoisture = " << sm
                //    << "\tInfiltration(mm) = " << INFIL[i] << "\n";
                //    throw ModelException(MID_SUR_MR, "Execute",
                //                         "Output data error: infiltration is less than zero. Where:\n"
                //                         + oss.str() + "Please contact the module developer. ");
                //}
            }
        } else {
            EXCP[i] = 0.f;
            INFIL[i] = 0.f;
        }
        /// if INFIL > 0., solst need to be updated here. By LJ, 2016-9-2
        if (INFIL[i] > 0.f) {
            //if (pot_vol != NULL && pot_vol[i] > UTIL_ZERO)
            //{
            //	if (impound_trig != NULL && FloatEqual(impound_trig[i], 0.f))
            //	{
            //		pot_vol[i] += INFIL[i];
            //		/// when impounded, set the maximum infiltration to 2 mm
            //		if (pot_vol[i] > 2.f)
            //			INFIL[i] = 2.f;
            //		else
            //			INFIL[i] = 0.f;
            //		pot_vol[i] -= INFIL[i];
            //	}
            //	//else /// release operation should be considered in IMP_SWAT module
            //	//	INFIL[i] += pot_vol[i];
            //}
            solst[i][0] += INFIL[i];
        }
        //if (i == 200)
        //{
        //	cout<<"netRain: "<<NEPR[i]<<", depStrg: "<<DPST[i]<<", infil: "<<INFIL[i]<<", surfq: "<<EXCP[i]<<endl;
        //}
    }
    return 0;
}

bool SUR_MR::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_SUR_MR, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) { m_nCells = n; }
        else {
            throw ModelException(MID_SUR_MR, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void SUR_MR::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(key, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, Tag_HillSlopeTimeStep)) {
        DT_HS = value;
        //else if (StringMatch(sk, VAR_T_SNOW))T_snow = value;
        //else if (StringMatch(sk, VAR_T0))m_t0 = value;
    } else if (StringMatch(sk, VAR_T_SOIL)) { t_soil = value; }
    else if (StringMatch(sk, VAR_K_RUN)) { K_run = value; }
    else if (StringMatch(sk, VAR_P_MAX)) { P_max = value; }
    else if (StringMatch(sk, VAR_S_FROZEN)) { s_frozen = value; }
    else {
        throw ModelException(MID_SUR_MR, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void SUR_MR::Set1DData(const char *key, int n, float *data) {
    CheckInputSize(key, n);
    //set the value
    string sk(key);
    if (StringMatch(sk, VAR_RUNOFF_CO)) { Runoff_co = data; }
    else if (StringMatch(sk, VAR_NEPR)) { NEPR = data; }
    else if (StringMatch(sk, VAR_TMEAN)) { TMEAN = data; }
    else if (StringMatch(sk, VAR_MOIST_IN)) { Moist_in = data; }
    else if (StringMatch(sk, VAR_DPST)) { DPST = data; }
    else if (StringMatch(sk, VAR_SOTE)) { SOTE = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, VAR_SOL_SUMSAT)) { sol_sumul = data; }
    else if (StringMatch(sk, VAR_POT_VOL)) { pot_vol = data; }
    else if (StringMatch(sk, VAR_IMPOUND_TRIG)) {
        impound_trig = data;
        //else if (StringMatch(sk, VAR_SNAC))m_snowAccu = data;
        //else if (StringMatch(sk, VAR_SNME))m_snowMelt = data;
    } else {
        throw ModelException(MID_SUR_MR, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void SUR_MR::Set2DData(const char *key, int nrows, int ncols, float **data) {
    string sk(key);
    CheckInputSize(key, nrows);
    nSoilLayers = ncols;
    if (StringMatch(sk, VAR_SOL_AWC)) {
        sol_awc = data;
        //else if (StringMatch(sk, VAR_FIELDCAP))m_fieldCap = data;
        //else if (StringMatch(sk, VAR_SOILTHICK))soilthick = data;
        //else if (StringMatch(sk, VAR_POROST))m_porosity = data;
    } else {
        throw ModelException(MID_SUR_MR, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void SUR_MR::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_INFIL)) {
        *data = INFIL;     //infiltration
    } else if (StringMatch(sk, VAR_EXCP)) {
        *data = EXCP; // excess precipitation
    } else if (StringMatch(sk, VAR_SOL_SW)) { *data = solsw; }
    else {
        throw ModelException(MID_SUR_MR, "Get1DData", "Result " + sk + " does not exist.");
    }
    *n = m_nCells;
}

void SUR_MR::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = nSoilLayers;
    if (StringMatch(sk, VAR_SOL_ST)) {
        *data = solst;
    } else {
        throw ModelException(MID_SUR_MR, "Get2DData", "Output " + sk + " does not exist.");
    }
}
