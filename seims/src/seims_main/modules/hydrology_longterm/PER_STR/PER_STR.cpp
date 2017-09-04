#include "seims.h"
#include "PER_STR.h"

PER_STR::PER_STR(void) : nSoilLayers(-1), TIMESTEP(-1), m_nCells(-1), t_soil(NODATA_VALUE),
                         Conductivity(NULL), sol_ul(NULL), sol_awc(NULL),
                         soilthick(NULL), soillayers(NULL),
                         INFIL(NULL), SOTE(NULL), solst(NULL), solsw(NULL),
                         pot_vol(NULL), SURU(NULL),
                         Perco(NULL) {
}

PER_STR::~PER_STR(void) {
    if (Perco == NULL) Release2DArray(m_nCells, Perco);
}

void PER_STR::initialOutputs() {
    if (Perco == NULL) {
        Initialize2DArray(m_nCells, nSoilLayers, Perco, NODATA_VALUE);
    }
}

int PER_STR::Execute() {
    CheckInputData();
    initialOutputs();
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        // Note that, infiltration, pothole seepage, irrigation etc. have been added to
        // the first soil layer in other modules. By LJ

        float excessWater = 0.f, maxSoilWater = 0.f, fcSoilWater = 0.f;
        for (int j = 0; j < (int) soillayers[i]; j++) {
            excessWater = 0.f;
            maxSoilWater = sol_ul[i][j];
            fcSoilWater = sol_awc[i][j];
            // determine gravity drained water in layer
            excessWater += solst[i][j] - fcSoilWater;
            //if (i == 100)
            //	cout<<"lyr: "<<j<<", soil storage: "<<solst[i][j]<<", fc: "<<fcSoilWater<<", excess: "<<excessWater<<endl;
            // for the upper two layers, soil may be frozen
            if (j == 0 && SOTE[i] <= t_soil) {
                continue;
            }
            Perco[i][j] = 0.f;
            // No movement if soil moisture is below field capacity
            if (excessWater > 1.e-5f) {
                float maxPerc = maxSoilWater - fcSoilWater;
                float tt = 3600.f * maxPerc / Conductivity[i][j]; // secs
                Perco[i][j] = excessWater * (1.f - exp(-TIMESTEP / tt)); // secs

                if (Perco[i][j] > maxPerc) {
                    Perco[i][j] = maxPerc;
                }
                //Adjust the moisture content in the current layer, and the layer immediately below it
                solst[i][j] -= Perco[i][j];
                excessWater -= Perco[i][j];
                solst[i][j] = max(UTIL_ZERO, solst[i][j]);
                // redistribute soil water if above field capacity (high water table), rewrite from sat_excess.f of SWAT
                //float qlyr = solst[i][j];
                if (j < (int) soillayers[i] - 1) {
                    solst[i][j + 1] += Perco[i][j];
                    if (solst[i][j] - sol_ul[i][j] > 1.e-4f) {
                        solst[i][j + 1] += solst[i][j] - sol_ul[i][j];
                        solst[i][j] = sol_ul[i][j];
                    }
                } else /// for the last soil layer
                {
                    if (solst[i][j] - sol_ul[i][j] > 1.e-4f) {
                        float ul_excess = solst[i][j] - sol_ul[i][j];
                        solst[i][j] = sol_ul[i][j];
                        for (int ly = (int) soillayers[i] - 2; ly >= 0; ly--) {
                            solst[i][ly] += ul_excess;
                            if (solst[i][ly] > sol_ul[i][ly]) {
                                ul_excess = solst[i][ly] - sol_ul[i][ly];
                                solst[i][ly] = sol_ul[i][ly];
                            } else {
                                ul_excess = 0.f;
                                break;
                            }
                            if (ly == 0 && ul_excess > 0.f) {
                                // add ul_excess to depressional storage and then to surfq
                                if (pot_vol != NULL) {
                                    pot_vol[i] += ul_excess;
                                } else {
                                    SURU[i] += ul_excess;
                                }
                            }
                        }
                    }
                }
            } else {
                Perco[i][j] = 0.f;
            }
        }
        /// update soil profile water
        solsw[i] = 0.f;
        for (int ly = 0; ly < (int) soillayers[i]; ly++) {
            solsw[i] += solst[i][ly];
        }
    }
    return 0;
}

void PER_STR::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = nSoilLayers;
    if (StringMatch(sk, VAR_PERCO)) { *data = Perco; }
    else {
        throw ModelException(MID_PER_STR, "Get2DData", "Output " + sk + " does not exist.");
    }
}

void PER_STR::Set1DData(const char *key, int nRows, float *data) {
    string sk(key);
    CheckInputSize(key, nRows);
    if (StringMatch(sk, VAR_SOTE)) { SOTE = data; }
    else if (StringMatch(sk, VAR_INFIL)) { INFIL = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, VAR_SOL_SW)) { solsw = data; }
    else if (StringMatch(sk, VAR_POT_VOL)) { pot_vol = data; }
    else if (StringMatch(sk, VAR_SURU)) { SURU = data; }
    else {
        throw ModelException(MID_PER_STR, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void PER_STR::Set2DData(const char *key, int nrows, int ncols, float **data) {
    string sk(key);
    CheckInputSize(key, nrows);
    nSoilLayers = ncols;

    if (StringMatch(sk, VAR_CONDUCT)) { Conductivity = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { sol_awc = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { solst = data; }
    else {
        throw ModelException(MID_PER_STR, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void PER_STR::SetValue(const char *key, float data) {
    string s(key);
    if (StringMatch(s, Tag_TimeStep)) { TIMESTEP = int(data); }
    else if (StringMatch(s, VAR_T_SOIL)) { t_soil = data; }
    else if (StringMatch(s, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
    else {
        throw ModelException(MID_PER_STR, "SetValue",
                             "Parameter " + s +
                                 " does not exist in current module. Please contact the module developer.");
    }
}

bool PER_STR::CheckInputData() {
    if (m_date <= 0) {
        throw ModelException(MID_PER_STR, "CheckInputData", "You have not set the time.");
    }
    if (m_nCells <= 0) {
        throw ModelException(MID_PER_STR, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The time step can not be less than zero.");
    }

    if (Conductivity == NULL) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The Conductivity can not be NULL.");
    }
    if (sol_ul == NULL) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The saturated moisture can not be NULL.");
    }
    if (solst == NULL) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The Moisture can not be NULL.");
    }
    if (sol_awc == NULL) {
        throw ModelException(MID_PER_PI, "CheckInputData", "The field capacity can not be NULL.");
    }
    if (this->solst == NULL) {
        throw ModelException(MID_PER_PI, "CheckInputData", "The soil storage can not be NULL.");
    }
    if (this->solsw == NULL) {
        throw ModelException(MID_PER_PI, "CheckInputData", "The soil storage of soil profile can not be NULL.");
    }
    if (soilthick == NULL) {
        throw ModelException(MID_PER_PI, "CheckInputData", "The soil depth can not be NULL.");
    }
    if (SOTE == NULL) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The soil temperature can not be NULL.");
    }
    if (INFIL == NULL) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The infiltration can not be NULL.");
    }
    if (FloatEqual(t_soil, NODATA_VALUE)) {
        throw ModelException(MID_PER_STR, "CheckInputData", "The threshold soil freezing temperature can not be NULL.");
    }
    return true;
}

bool PER_STR::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_PER_STR, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_PER_STR, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}
