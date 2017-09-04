#include "seims.h"
#include "Biomass_EPIC.h"

using namespace std;

Biomass_EPIC::Biomass_EPIC(void) : m_nCells(-1), m_nClimDataYrs(-1), Co2(NODATA_VALUE), TMEAN(NULL), TMIN(NULL),
                                   SR(NULL), daylenmin(NULL), dormhr(NULL),
                                   soilLayers(-1), n_updis(NODATA_VALUE), p_updis(NODATA_VALUE),
                                   nfixco(NODATA_VALUE),
                                   nfixmx(NODATA_VALUE), m_soilRD(NULL), TMEAN0(NULL),
                                   soillayers(NULL), SOL_ZMX(NULL), sol_alb(NULL), soilDepth(NULL),
                                   sol_awc(NULL), sol_sumAWC(NULL), sol_sumul(NULL),
                                   solst(NULL), solsw(NULL), rsdin(NULL), sol_cov(NULL),
                                   sol_rsd(NULL),
                                   IGRO(NULL), IDC(NULL), alai_min(NULL), BIOE(NULL), BIOEHI(NULL),
                                   BIO_LEAF(NULL), BLAI(NULL), BMX_TREES(NULL),
                                   BN1(NULL), BN2(NULL), BN3(NULL), BP1(NULL),
                                   BP2(NULL), BP3(NULL),
                                   CHTMX(NULL), CO2HI(NULL), DLAI(NULL), epco(NULL), EXT_COEF(NULL),
                                   FRGRW1(NULL), FRGRW2(NULL),
                                   hvsti(NULL), LAIMX1(NULL), LAIMX2(NULL), MAT_YRS(NULL), T_BASE(NULL),
                                   T_OPT(NULL), WAVP(NULL),
                                   CHT(NULL), CURYR_INIT(NULL), BIO_INIT(NULL), LAI_INIT(NULL),
                                   PHU_PLT(NULL), dormi(NULL), plt_et_tot(NULL), plt_pet_tot(NULL),
                                   daylength(NULL), VPD(NULL), PET(NULL), PPT(NULL), SOET(NULL),
                                   sol_no3(NULL), sol_solp(NULL), SNAC(NULL),
                                   LAIDAY(NULL), frPHUacc(NULL), laiyrmax(NULL), hvsti_adj(NULL),
                                   laimaxfr(NULL), olai(NULL), lastSoilRD(NULL),
                                   AET_PLT(NULL), frRoot(NULL), m_fixN(NULL),
                                   m_plantUpTkN(NULL), plant_N(NULL), frPlantN(NULL),
                                   m_plantUpTkP(NULL), plant_P(NULL), frPlantP(NULL),
                                   m_NO3Defic(NULL), m_frStrsAe(NULL), m_frStrsN(NULL), m_frStrsP(NULL),
                                   m_frStrsTmp(NULL), frStrsWtr(NULL),
                                   m_biomassDelta(NULL), BIOMASS(NULL), ALBDAY(NULL),
    // parameters not allowed to modify
                                   ubw(10.f), uobw(NODATA_VALUE) {
}

Biomass_EPIC::~Biomass_EPIC(void) {
    if (sol_cov != NULL) Release1DArray(sol_cov);
    if (sol_rsd != NULL) Release2DArray(m_nCells, sol_rsd);
    if (LAIDAY != NULL) Release1DArray(LAIDAY);
    if (laiyrmax != NULL) Release1DArray(laiyrmax);
    if (frPHUacc != NULL) Release1DArray(frPHUacc);
    if (plt_et_tot != NULL) Release1DArray(plt_et_tot);
    if (plt_pet_tot != NULL) Release1DArray(plt_pet_tot);
    if (hvsti_adj != NULL) Release1DArray(hvsti_adj);
    if (laimaxfr != NULL) Release1DArray(laimaxfr);
    if (olai != NULL) Release1DArray(olai);
    if (lastSoilRD != NULL) Release1DArray(lastSoilRD);
    if (AET_PLT != NULL) Release1DArray(AET_PLT);
    if (frRoot != NULL) Release1DArray(frRoot);
    if (m_fixN != NULL) Release1DArray(m_fixN);
    if (m_plantUpTkN != NULL) Release1DArray(m_plantUpTkN);
    if (m_plantUpTkP != NULL) Release1DArray(m_plantUpTkP);
    if (plant_N != NULL) Release1DArray(plant_N);
    if (plant_P != NULL) Release1DArray(plant_P);
    if (frPlantN != NULL) Release1DArray(frPlantN);
    if (frPlantP != NULL) Release1DArray(frPlantP);
    if (m_NO3Defic != NULL) Release1DArray(m_NO3Defic);
    if (m_frStrsAe != NULL) Release1DArray(m_frStrsAe);
    if (m_frStrsN != NULL) Release1DArray(m_frStrsN);
    if (m_frStrsP != NULL) Release1DArray(m_frStrsP);
    if (m_frStrsTmp != NULL) Release1DArray(m_frStrsTmp);
    if (frStrsWtr != NULL) Release1DArray(frStrsWtr);
    if (m_biomassDelta != NULL) Release1DArray(m_biomassDelta);
    if (BIOMASS != NULL) Release1DArray(BIOMASS);
}

void Biomass_EPIC::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_CO2)) { Co2 = value; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, VAR_NUPDIS)) { n_updis = value; }
    else if (StringMatch(sk, VAR_PUPDIS)) { p_updis = value; }
    else if (StringMatch(sk, VAR_NFIXCO)) { nfixco = value; }
    else if (StringMatch(sk, VAR_NFIXMX)) { nfixmx = value; }
    else {
        throw ModelException(MID_BIO_EPIC, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

bool Biomass_EPIC::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_BIO_EPIC, "CheckInputSize", "Input data for " + string(key) +
            " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_BIO_EPIC, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input raster data should have same size.");
        }
    }
    return true;
}

void Biomass_EPIC::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    CheckInputSize(key, n);
    //// climate
    if (StringMatch(sk, DataType_MeanTemperature)) { TMEAN = data; }
    else if (StringMatch(sk, DataType_MinimumTemperature)) { TMIN = data; }
    else if (StringMatch(sk, DataType_SolarRadiation)) { SR = data; }
    else if (StringMatch(sk, VAR_DAYLEN_MIN)) { daylenmin = data; }
    else if (StringMatch(sk, VAR_TMEAN_ANN)) { TMEAN0 = data; }
    else if (StringMatch(sk, VAR_DORMHR)) { dormhr = data; }
    else if (StringMatch(sk, VAR_DAYLEN)) {
        daylength = data;
        //// soil properties and water related
    } else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, VAR_SOL_ZMX)) { SOL_ZMX = data; }
    else if (StringMatch(sk, VAR_SOL_ALB)) { sol_alb = data; }
    else if (StringMatch(sk, VAR_SOL_SW)) { solsw = data; }
    else if (StringMatch(sk, VAR_SOL_SUMAWC)) { sol_sumAWC = data; }
    else if (StringMatch(sk, VAR_SOL_SUMSAT)) { sol_sumul = data; }
    else if (StringMatch(sk, VAR_PET)) { PET = data; }
    else if (StringMatch(sk, VAR_VPD)) { VPD = data; }
    else if (StringMatch(sk, VAR_PPT)) { PPT = data; }
    else if (StringMatch(sk, VAR_SOET)) { SOET = data; }
    else if (StringMatch(sk, VAR_SOL_COV)) { sol_cov = data; }
    else if (StringMatch(sk, VAR_SNAC)) {
        SNAC = data;
        //// land cover
    } else if (StringMatch(sk, VAR_SOL_RSDIN)) { rsdin = data; }
    else if (StringMatch(sk, VAR_IGRO)) { IGRO = data; }
    else if (StringMatch(sk, VAR_IDC)) { IDC = data; }
    else if (StringMatch(sk, VAR_ALAIMIN)) { alai_min = data; }
    else if (StringMatch(sk, VAR_BIO_E)) { BIOE = data; }
    else if (StringMatch(sk, VAR_BIOEHI)) { BIOEHI = data; }
    else if (StringMatch(sk, VAR_BIOLEAF)) { BIO_LEAF = data; }
    else if (StringMatch(sk, VAR_BLAI)) { BLAI = data; }
    else if (StringMatch(sk, VAR_BMX_TREES)) { BMX_TREES = data; }
    else if (StringMatch(sk, VAR_BN1)) { BN1 = data; }
    else if (StringMatch(sk, VAR_BN2)) { BN2 = data; }
    else if (StringMatch(sk, VAR_BN3)) { BN3 = data; }
    else if (StringMatch(sk, VAR_BP1)) { BP1 = data; }
    else if (StringMatch(sk, VAR_BP2)) { BP2 = data; }
    else if (StringMatch(sk, VAR_BP3)) { BP3 = data; }
    else if (StringMatch(sk, VAR_CHTMX)) { CHTMX = data; }
    else if (StringMatch(sk, VAR_CO2HI)) { CO2HI = data; }
    else if (StringMatch(sk, VAR_DLAI)) { DLAI = data; }
    else if (StringMatch(sk, VAR_EXT_COEF)) { EXT_COEF = data; }
    else if (StringMatch(sk, VAR_FRGRW1)) { FRGRW1 = data; }
    else if (StringMatch(sk, VAR_FRGRW2)) { FRGRW2 = data; }
    else if (StringMatch(sk, VAR_HVSTI)) { hvsti = data; }
    else if (StringMatch(sk, VAR_LAIMX1)) { LAIMX1 = data; }
    else if (StringMatch(sk, VAR_LAIMX2)) { LAIMX2 = data; }
    else if (StringMatch(sk, VAR_MAT_YRS)) { MAT_YRS = data; }
    else if (StringMatch(sk, VAR_T_BASE)) { T_BASE = data; }
    else if (StringMatch(sk, VAR_T_OPT)) { T_OPT = data; }
    else if (StringMatch(sk, VAR_WAVP)) {
        WAVP = data;
        //// management
    } else if (StringMatch(sk, VAR_EPCO)) { epco = data; }
    else if (StringMatch(sk, VAR_TREEYRS)) { CURYR_INIT = data; }
    else if (StringMatch(sk, VAR_LAIINIT)) { LAI_INIT = data; }
    else if (StringMatch(sk, VAR_BIOINIT)) { BIO_INIT = data; }
    else if (StringMatch(sk, VAR_PHUPLT)) { PHU_PLT = data; }
    else if (StringMatch(sk, VAR_CHT)) { CHT = data; }
    else if (StringMatch(sk, VAR_DORMI)) { dormi = data; }
    else {
        throw ModelException(MID_BIO_EPIC, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

bool Biomass_EPIC::CheckInputSize2D(const char *key, int n, int col) {
    CheckInputSize(key, n);
    if (col <= 0) {
        throw ModelException(MID_BIO_EPIC, "CheckInputSize2D", "Input data for " + string(key) +
            " is invalid. The layer number could not be less than zero.");
    }
    if (soilLayers != col) {
        if (soilLayers <= 0) {
            soilLayers = col;
        } else {
            throw ModelException(MID_BIO_EPIC, "CheckInputSize2D", "Input data for " + string(key) +
                " is invalid. All the layers of input 2D raster data should have same size.");
        }
    }
    return true;
}

void Biomass_EPIC::Set2DData(const char *key, int nRows, int nCols, float **data) {
    string sk(key);
    CheckInputSize2D(key, nRows, nCols);
    if (StringMatch(sk, VAR_SOILDEPTH)) { soilDepth = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_RSD)) { sol_rsd = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { sol_awc = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { solst = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { sol_solp = data; }
    else {
        throw ModelException(MID_BIO_EPIC, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

bool Biomass_EPIC::CheckInputData(void) {
    /// DT_Single
    if (m_date <= 0) throw ModelException(MID_BIO_EPIC, "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (soilLayers <= 0) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (FloatEqual(Co2, NODATA_VALUE)) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The ambient atmospheric CO2 concentration must be provided.");
    }
    if (FloatEqual(n_updis, NODATA_VALUE)) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The nitrogen uptake distribution parameter must be provided.");
    }
    if (FloatEqual(p_updis, NODATA_VALUE)) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The phosphorus uptake distribution parameter must be provided.");
    }
    if (FloatEqual(nfixco, NODATA_VALUE)) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The nitrogen fixation coefficient must be provided.");
    }
    if (FloatEqual(nfixmx, NODATA_VALUE)) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The maximum daily-N fixation must be provided.");
    }
    if (TMEAN0 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The annual mean air temperature can not be NULL.");
    }
    /// DT_Raster1D
    if (TMIN == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The min temperature data can not be NULL.");
    }
    if (TMEAN == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The mean temperature data can not be NULL.");
    }
    if (SR == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The solar radiation data can not be NULL.");
    if (daylength == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The day length can not be NULL.");
    if (daylenmin == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The minimum day length can not be NULL.");
    }
    if (dormhr == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The dormancy threshold time can not be NULL.");
    }

    if (soillayers == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The soil layers data can not be NULL.");
    }
    if (SOL_ZMX == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The maximum rooting depth in soil profile can not be NULL.");
    }
    if (sol_alb == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The albedo when soil is moist can not be NULL.");
    }
    if (solsw == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The amount of water stored in the soil profile can not be NULL.");
    }
    if (sol_sumAWC == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The amount of water held in soil profile at field capacity can not be NULL.");
    }
    if (sol_sumul == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The amount of water held in soil profile at saturation can not be NULL.");
    }
    if (PET == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The PET can not be NULL.");
    if (VPD == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The Vapor pressure deficit can not be NULL.");
    }
    if (PPT == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The potential plant et can not be NULL.");
    if (SOET == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The actual soil et can not be NULL.");
    }
    if (this->rsdin == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The rsdin can not be NULL.");
    }
    if (IGRO == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The land cover status code can not be NULL.");
    }
    if (IDC == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The land cover/plant classification can not be NULL.");
    }
    if (alai_min == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The minimum LAI during winter dormant period can not be NULL.");
    }
    if (BIOE == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The RUE at ambient atmospheric CO2 concentration data can not be NULL.");
    }
    if (BIOEHI == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The  RUE at elevated atmospheric CO2 concentration data can not be NULL.");
    }
    if (BIO_LEAF == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The fraction of biomass that drops during dormancy data can not be NULL.");
    }
    if (BLAI == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The max LAI data can not be NULL.");
    if (BMX_TREES == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The maximum biomass for a forest can not be NULL.");
    }
    if (BN1 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at emergence data can not be NULL.");
    }
    if (BN2 == NULL) {
        throw ModelException(MID_BIO_EPIC,
                             "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at 50% maturity data can not be NULL.");
    }
    if (BN3 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at maturity data can not be NULL.");
    }
    if (BP1 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at emergence data can not be NULL.");
    }
    if (BP2 == NULL) {
        throw ModelException(MID_BIO_EPIC,
                             "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at 50% maturity data can not be NULL.");
    }
    if (BP3 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at maturity data can not be NULL.");
    }
    if (CHTMX == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The maximum canopy height can not be NULL.");
    }
    if (CO2HI == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The elevated atmospheric CO2 concentration data can not be NULL.");
    }
    if (DLAI == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The PHU fraction when senescence becomes dominant data can not be NULL.");
    }
    if (EXT_COEF == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The  light extinction coefficient data can not be NULL.");
    }
    if (FRGRW1 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The PHU fraction for the 1st point data can not be NULL.");
    }
    if (FRGRW2 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The PHU fraction for the 2nd point data can not be NULL.");
    }
    if (hvsti == NULL) throw ModelException(MID_BIO_EPIC, "CheckInputData", "The harvest index can not be NULL.");
    if (LAIMX1 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The LAI fraction for the 1st point data can not be NULL.");
    }
    if (LAIMX2 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "TheLAI fraction for the 2nd point  data can not be NULL.");
    }
    if (MAT_YRS == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The full development year data can not be NULL.");
    }
    if (T_BASE == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The base temperature data can not be NULL.");
    }
    if (T_OPT == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The optimal temperature for plant growth can not be NULL.");
    }
    if (WAVP == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The rate of decline in rue per unit increase in VPD data can not be NULL.");
    }

    if (epco == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The plant water uptake compensation factor can not be NULL.");
    }
    if (CURYR_INIT == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The initial age of tress can not be NULL.");
    }
    if (LAI_INIT == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The initial leaf area index of transplants can not be NULL.");
    }
    if (BIO_INIT == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The initial dry weight biomass can not be NULL.");
    }
    if (PHU_PLT == NULL) {
        throw ModelException(MID_BIO_EPIC,
                             "CheckInputData",
                             "The total number of heat unites (hours) needed to bring plant to maturity can not be NULL.");
    }
    if (CHT == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The canopy height for the day can not be NULL.");
    }
    if (dormi == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The dormancy status code can not be NULL.");
    }

    /// DT_Raster2D
    if (soilDepth == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The soil depth data can not be NULL.");
    }
    if (soilthick == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The soil thickness data can not be NULL.");
    }
    if (sol_awc == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The water available to plants in soil layer at field capacity can not be NULL.");
    }
    if (solst == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The soil moisture in soil layers can not be NULL.");
    }
    if (sol_no3 == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData",
                             "The nitrogen stored in the nitrate pool in soil layer can not be NULL.");
    }
    if (sol_solp == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The phosphorus stored in solution can not be NULL.");
    }
    return true;
}

void Biomass_EPIC::initialOutputs() {
    if (FloatEqual(uobw, NODATA_VALUE)) {
        ubw = 10.f; /// the uptake distribution for water is hardwired, users are not allowed to modify
        uobw = 0.f;
        uobw = PGCommon::getNormalization(ubw);
    }
    if (ALBDAY == NULL) Initialize1DArray(m_nCells, ALBDAY, 0.f);
    if (sol_cov == NULL || sol_rsd == NULL) {
        Initialize1DArray(m_nCells, sol_cov, rsdin);
        Initialize2DArray(m_nCells, soilLayers, sol_rsd, 0.f);
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            sol_rsd[i][0] = sol_cov[i];
        }
    }
    if (LAIDAY == NULL) {
        if (LAI_INIT != NULL) {
            Initialize1DArray(m_nCells, LAIDAY, LAI_INIT);
        } else {
            Initialize1DArray(m_nCells, LAIDAY, 0.f);
        }
    }
    if (laiyrmax == NULL) {
        laiyrmax = new float[m_nCells];
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            if (PGCommon::IsTree((int) (IDC[i]))) {
                laiyrmax[i] *= CURYR_INIT[i] / MAT_YRS[i];
            } else {
                laiyrmax[i] = LAIDAY[i];
            }
        }
    }
    if (frPHUacc == NULL) {
        Initialize1DArray(m_nCells, frPHUacc, 0.f);
    }
    if (plt_et_tot == NULL) {
        Initialize1DArray(m_nCells, plt_et_tot, 0.f);
    }
    if (plt_pet_tot == NULL) {
        Initialize1DArray(m_nCells, plt_pet_tot, 0.f);
    }
    if (hvsti_adj == NULL) {
        Initialize1DArray(m_nCells, hvsti_adj, 0.f);
    }
    if (laimaxfr == NULL) {
        Initialize1DArray(m_nCells, laimaxfr, 0.f);
    }
    if (olai == NULL) {
        Initialize1DArray(m_nCells, olai, 0.f);
    }
    if (lastSoilRD == NULL) {
        Initialize1DArray(m_nCells, lastSoilRD, 10.f);
    }
    if (m_soilRD == NULL) {
        Initialize1DArray(m_nCells, m_soilRD, 10.f);
    }
    if (AET_PLT == NULL) {
        Initialize1DArray(m_nCells, AET_PLT, 0.f);
    }
    if (frRoot == NULL) {
        Initialize1DArray(m_nCells, frRoot, 0.f);
    }
    if (m_fixN == NULL) {
        Initialize1DArray(m_nCells, m_fixN, 0.f);
    }
    if (m_plantUpTkN == NULL) {
        Initialize1DArray(m_nCells, m_plantUpTkN, 0.f);
    }
    if (m_plantUpTkP == NULL) {
        Initialize1DArray(m_nCells, m_plantUpTkP, 0.f);
    }
    if (plant_N == NULL) {
        Initialize1DArray(m_nCells, plant_N, 0.f);
    }
    if (plant_P == NULL) {
        Initialize1DArray(m_nCells, plant_P, 0.f);
    }
    if (frPlantN == NULL) {
        Initialize1DArray(m_nCells, frPlantN, 0.f);
    }
    if (frPlantP == NULL) {
        Initialize1DArray(m_nCells, frPlantP, 0.f);
    }

    if (m_NO3Defic == NULL) {
        Initialize1DArray(m_nCells, m_NO3Defic, 0.f);
    }
    // initialize these stress factors according to sim_iniday.f of SWAT
    if (m_frStrsAe == NULL) {
        Initialize1DArray(m_nCells, m_frStrsAe, 1.f);
    }
    if (m_frStrsN == NULL) {
        Initialize1DArray(m_nCells, m_frStrsN, 1.f);
    }
    if (m_frStrsP == NULL) {
        Initialize1DArray(m_nCells, m_frStrsP, 1.f);
    }
    if (m_frStrsTmp == NULL) {
        Initialize1DArray(m_nCells, m_frStrsTmp, 1.f);
    }
    // according to zero.f of SWAT
    if (frStrsWtr == NULL) {
        Initialize1DArray(m_nCells, frStrsWtr, 1.f);
    }
    if (m_biomassDelta == NULL) {
        Initialize1DArray(m_nCells, m_biomassDelta, 0.f);
    }
    if (BIOMASS == NULL) {
        if (BIO_INIT != NULL) {
            Initialize1DArray(m_nCells, BIOMASS, BIO_INIT);
        } else {
            Initialize1DArray(m_nCells, BIOMASS, 0.f);
        }
    }
}

void Biomass_EPIC::DistributePlantET(int i) {    /// swu.f of SWAT
    float sum = 0.f, sump = 0.f, gx = 0.f;
    /// fraction of water uptake by plants achieved
    /// where the reduction is caused by low water content
    float reduc = 0.f;
    /// water uptake by plants in each soil layer
    /*
     * Initialize1DArray should not be used inside a OMP codeblock
     * In VS this would be fine, but in linux, may be problematic.*/
    float *wuse(NULL);
    Initialize1DArray((int) soillayers[i], wuse, 0.f);

    //float *wuse = new float[(int) soillayers[i]];
    //for (int j = 0; j < (int) soillayers[i]; j++)
    //	wuse[j] = 0.f;
    /// water uptake by plants from all layers
    float xx = 0.f;
    int ir = -1;
    int idc = int(IDC[i]);
    if (idc == 1 || idc == 2 || idc == 4 || idc == 5) {
        m_soilRD[i] = 2.5f * frPHUacc[i] * SOL_ZMX[i];
        if (m_soilRD[i] > SOL_ZMX[i]) m_soilRD[i] = SOL_ZMX[i];
        if (m_soilRD[i] < 10.f) m_soilRD[i] = 10.f;   /// minimum root depth is 10mm
    } else {
        m_soilRD[i] = SOL_ZMX[i];
    }
    lastSoilRD[i] = m_soilRD[i];
    if (PPT[i] <= 0.01f) {
        frStrsWtr[i] = 1.f;
    } else {
        /// initialize variables
        gx = 0.f;
        ir = 0;
        sump = 0.f;
        xx = 0.f;
        // update soil storage profile just in case
        solsw[i] = 0.f;
        for (int ly = 0; ly < (int) soillayers[i]; ly++) {
            solsw[i] += solst[i][ly];
        }
        /// compute aeration stress
        if (solsw[i] >= sol_sumAWC[i]) // mm
        {
            float satco = (solsw[i] - sol_sumAWC[i]) / (sol_sumul[i] - sol_sumAWC[i]);
            float pl_aerfac = 0.85f;
            float scparm = 100.f * (satco - pl_aerfac) / (1.0001f - pl_aerfac);
            if (scparm > 0.f) {
                m_frStrsAe[i] = 1.f - (scparm / (scparm + exp(2.9014f - 0.03867f * scparm)));
            } else {
                m_frStrsAe[i] = 1.f;
            }
        }
        for (int j = 0; j < (int) soillayers[i]; j++) {
            if (ir > 0) break;
            if (m_soilRD[i] <= soilDepth[i][j]) {
                gx = m_soilRD[i];
                ir = j;
            } else {
                gx = soilDepth[i][j];
            }
            sum = 0.f;
            if (m_soilRD[i] <= 0.01f) {
                sum = PPT[i] / uobw;
            } else {
                sum = PPT[i] * (1.f - exp(-ubw * gx / m_soilRD[i])) / uobw;
            }
            wuse[j] = sum - sump + 1.f * epco[i];
            wuse[j] = sum - sump + (sump - xx) * epco[i];
            sump = sum;
            /// adjust uptake if sw is less than 25% of plant available water
            reduc = 0.f;
            if (solst[i][j] < sol_awc[i][j] / 4.f) {
                reduc = exp(5.f * (4.f * solst[i][j] / sol_awc[i][j] - 1.f));
            } else {
                reduc = 1.f;
            }
            // reduc = 1.f;  /// TODO, Is SWAT wrong here? by LJ
            wuse[j] *= reduc;
            if (solst[i][j] < wuse[j]) {
                wuse[j] = solst[i][j];
            }
            solst[i][j] = max(UTIL_ZERO, solst[i][j] - wuse[j]);
            xx += wuse[j];
        }
        /// update total soil water in profile
        solsw[i] = 0.f;
        for (int ly = 0; ly < (int) soillayers[i]; ly++) {
            solsw[i] += solst[i][ly];
        }
        frStrsWtr[i] = xx / PPT[i];
        AET_PLT[i] = xx;
    }
    Release1DArray(wuse);
}

void Biomass_EPIC::CalTempStress(int i) {
    float tgx = 0.f, rto = 0.f;
    tgx = TMEAN[i] - T_BASE[i];
    if (tgx <= 0.f) {
        m_frStrsTmp[i] = 0.f;
    } else if (TMEAN[i] > T_OPT[i]) {
        tgx = 2.f * T_OPT[i] - T_BASE[i] - TMEAN[i];
    }
    rto = (T_OPT[i] - T_BASE[i]) / pow((tgx + UTIL_ZERO), 2.f);
    if (rto <= 200.f && tgx > 0.f) {
        m_frStrsTmp[i] = exp(-0.1054f * rto);
    } else {
        m_frStrsTmp[i] = 0.f;
    }
    if (TMIN[i] <= TMEAN0[i] - 15.f) {
        m_frStrsTmp[i] = 0.f;
    }
}

void Biomass_EPIC::AdjustPlantGrowth(int i) {
    /// Update accumulated heat units for the plant
    float delg = 0.f;
    if (PHU_PLT[i] > 0.1) {
        delg = (TMEAN[i] - T_BASE[i]) / PHU_PLT[i];
    }
    if (delg < 0.f) {
        delg = 0.f;
    }
    frPHUacc[i] += delg;
    //if(i == 5) cout << m_biomassDelta[i] << ", \n";
    //if(i == 0) cout << frPHUacc[i] << ", \n";
    /// If plant hasn't reached maturity
    if (frPHUacc[i] <= 1.f) {
        ///compute temperature stress - strstmp(j) , tstr.f in SWAT
        CalTempStress(i);
        /// Calculate optimal biomass
        /// 1. calculate photosynthetically active radiation
        float activeRadiation = 0.f;
        activeRadiation = 0.5f * SR[i] * (1.f - exp(-EXT_COEF[i] * (LAIDAY[i] + 0.05f)));
        /// 2. Adjust radiation-use efficiency for CO2
        ////  determine shape parameters for the radiation use efficiency equation, readplant.f in SWAT
        if (FloatEqual(CO2HI[i], 330.0f)) CO2HI[i] = 660.f;
        float m_RadUseEffiShpCoef1 = 0.f;
        float m_RadUseEffiShpCoef2 = 0.f;
        PGCommon::getScurveShapeParameter(BIOE[i] * 0.01f, BIOEHI[i] * 0.01f, Co2, CO2HI[i],
                                          &m_RadUseEffiShpCoef1, &m_RadUseEffiShpCoef2);

        float beadj = 0.f;
        if (Co2 > 330.f) {
            beadj = 100.f * Co2 / (Co2 + exp(m_RadUseEffiShpCoef1 - Co2 * m_RadUseEffiShpCoef2));
        } else {
            beadj = BIOE[i];
        }
        /// 3. adjust radiation-use efficiency for vapor pressure deficit
        ///     assumes vapor pressure threshold of 1.0 kPa
        float ruedecl = 0.f;
        if (VPD[i] > 1.0) {
            ruedecl = VPD[i] - 1.f;
            beadj -= WAVP[i] * ruedecl;
            beadj = max(beadj, 0.27f * BIOE[i]);
        }
        m_biomassDelta[i] = max(0.f, beadj * activeRadiation);
        /// 4. Calculate plant uptake of N and P to make sure no plant N and P uptake under temperature, water and aeration stress   
        /// frStrsWtr and m_frStrsAe are derived from DistributePlantET()
        //if (i == 2000 && (frStrsWtr[i] > 0.f || m_frStrsTmp[i] > 0.f || m_frStrsAe[i] > 0.f))
        //	cout<<"water stress frac: "<<frStrsWtr[i]<<", tmp: "<<m_frStrsTmp[i]<<", Ae: "<<m_frStrsAe[i]<<endl;
        float reg = min(min(frStrsWtr[i], m_frStrsTmp[i]), m_frStrsAe[i]);
        if (reg < 0.f) reg = 0.f;
        if (reg > 0.) {
            //cout<<"Begin Uptake N and P"<<endl;
            /// call nup to calculates plant nitrogen uptake
            PlantNitrogenUptake(i);
            /// call npup to calculates plant phosphorus uptake
            PlantPhosphorusUptake(i);
        } else {
            m_frStrsN[i] = 1.f;
            m_frStrsP[i] = 1.f;
        }
        /// 5. auto fertilization-nitrogen demand (non-legumes only)
        int idc = int(IDC[i]);
        //if((idc == 4 || idc == 5 || idc == 6 || idc == 7) && auto_nstrs[i] > 0.)
        /// call anfert
        ///////// TODO: Finish auto fertilization-nitrogen later. by LJ
        /// 6. reduce predicted biomass due to stress on plant
        reg = min(frStrsWtr[i], min(m_frStrsTmp[i], min(m_frStrsN[i], m_frStrsP[i])));
        if (reg < 0.f) reg = 0.f;
        if (reg > 1.f) reg = 1.f;
        //// TODO bio_targ in SWAT is not incorporated in SEIMS.
        BIOMASS[i] += m_biomassDelta[i] * reg;
        float rto = 1.f;
        if (idc == CROP_IDC_TREES) {
            if (MAT_YRS[i] > 0.) {
                float curYrMat = CURYR_INIT[i] + m_yearIdx;
                rto = curYrMat / MAT_YRS[i];
                BIOMASS[i] = min(BIOMASS[i], rto * BMX_TREES[i] * 1000.f);  /// convert tons/ha -> kg/ha
            } else {
                rto = 1.f;
            }
        }
        BIOMASS[i] = max(BIOMASS[i], 0.f);
        //if(i == 5) cout << BIOMASS[i] << ", \n";
        /// 7. calculate fraction of total biomass that is in the roots
        float m_rootShootRatio1 = 0.4f;
        float m_rootShootRatio2 = 0.2f;
        frRoot[i] = m_rootShootRatio1 * (m_rootShootRatio1 - m_rootShootRatio2) * frPHUacc[i];
        float LAIShpCoef1 = 0.f, LAIShpCoef2 = 0.f;
        PGCommon::getScurveShapeParameter(LAIMX1[i], LAIMX2[i], FRGRW1[i], FRGRW2[i],
                                          &LAIShpCoef1, &LAIShpCoef2);
        float f = frPHUacc[i] / (frPHUacc[i] + exp(LAIShpCoef1 - LAIShpCoef2 * frPHUacc[i]));
        float ff = f - laimaxfr[i];
        laimaxfr[i] = f;
        /// 8. calculate new canopy height
        if (idc == CROP_IDC_TREES) {
            CHT[i] = rto * CHTMX[i];
        } else {
            CHT[i] = CHTMX[i] * sqrt(f);
        }
        /// 9. calculate new leaf area index (LAI)
        float laiMax = 0.f;
        float laiDelta = 0.f;
        if (frPHUacc[i] <= DLAI[i]) {
            laiMax = 0.f;
            laiDelta = 0.f;
            if (idc == CROP_IDC_TREES) {
                laiMax = rto * BLAI[i];
            } else {
                laiMax = BLAI[i];
            }
            if (LAIDAY[i] > laiMax) LAIDAY[i] = laiMax;
            laiDelta = ff * laiMax * (1.f - exp(5.f * (LAIDAY[i] - laiMax))) * sqrt(reg);
            LAIDAY[i] += laiDelta;
            if (LAIDAY[i] > laiMax) LAIDAY[i] = laiMax;
            olai[i] = LAIDAY[i];
            if (LAIDAY[i] > laiyrmax[i]) laiyrmax[i] = LAIDAY[i];
        } else {
            LAIDAY[i] = olai[i] * (1.f - frPHUacc[i]) / (1.f - DLAI[i]);
        }
        if (LAIDAY[i] < alai_min[i]) {
            LAIDAY[i] = alai_min[i];
        }
        /// 10. calculate plant ET values
        if (frPHUacc[i] > 0.5f && frPHUacc[i] < DLAI[i]) {
            plt_et_tot[i] += (AET_PLT[i] + SOET[i]);
            plt_pet_tot[i] += PET[i];
        }
        hvsti_adj[i] = hvsti[i] * 100.f * frPHUacc[i] / (100.f * frPHUacc[i] +
            exp(11.1f - 10.f * frPHUacc[i]));
    } else {
        if (DLAI[i] > 1.f) {
            if (frPHUacc[i] > DLAI[i]) {
                LAIDAY[i] = olai[i] * (1.f - (frPHUacc[i] - DLAI[i]) / (1.2f - DLAI[i]));
            }
        }
        if (LAIDAY[i] < 0.f) LAIDAY[i] = 0.f;
    }
}

void Biomass_EPIC::PlantNitrogenUptake(int i) {
    float uobn = PGCommon::getNormalization(n_updis);
    float n_reduc = 300.f; /// nitrogen uptake reduction factor (not currently used; defaulted 300.)
    float tno3 = 0.f;
    for (int l = 0; l < (int) soillayers[i]; l++) {
        tno3 += sol_no3[i][l];
    }
    tno3 /= n_reduc;
    // float up_reduc = tno3 / (tno3 + exp(1.56f - 4.5f * tno3)); /// However, up_reduc is not used hereafter.
    /// icrop is land cover code in SWAT.
    /// in SEIMS, it is no need to use it.
    //// determine shape parameters for plant nitrogen uptake equation, from readplant.f
    frPlantN[i] = PGCommon::NPBiomassFraction(BN1[i], BN2[i], BN3[i], frPHUacc[i]);
    float un2 = 0.f; /// ideal (or optimal) plant nitrogen content (kg/ha)
    un2 = frPlantN[i] * BIOMASS[i];
    if (un2 < plant_N[i]) un2 = plant_N[i];
    m_NO3Defic[i] = un2 - plant_N[i];
    m_NO3Defic[i] = min(4.f * BN3[i] * m_biomassDelta[i], m_NO3Defic[i]);
    m_frStrsN[i] = 1.f;
    int ir = 0;
    if (m_NO3Defic[i] < UTIL_ZERO) {
        return;
    }
    for (int l = 0; l < (int) soillayers[i]; l++) {
        if (ir > 0) {
            break;
        }
        float gx = 0.f;
        if (m_soilRD[i] <= soilDepth[i][l]) {
            gx = m_soilRD[i];
            ir = 1;
        } else {
            gx = soilDepth[i][l];
        }
        float unmx = 0.f;
        float uno3l = 0.f; /// plant nitrogen demand (kg/ha)
        unmx = m_NO3Defic[i] * (1.f - exp(-n_updis * gx / m_soilRD[i])) / uobn;
        uno3l = min(unmx - m_plantUpTkN[i], sol_no3[i][l]);
        //if (uno3l != uno3l)
        //	cout<<"cellid: "<<i<<", lyr: "<<l<<",m_NO3Defic: "<<m_NO3Defic[i]<<
        //	", UpDis: "<<n_updis<<", gx: "<<gx<<", msoilrd:"<<m_soilRD[i]<<
        //	", uobn:"<<uobn<<", plantUpTkN: "<<m_plantUpTkN[i]<<
        //	", soilNo3: "<<sol_no3[i][l]<<", unmx: "<<unmx<<
        //	", uno3l: "<<uno3l<<endl;
        m_plantUpTkN[i] += uno3l;
        sol_no3[i][l] -= uno3l;
    }
    if (m_plantUpTkN[i] < 0.f) m_plantUpTkN[i] = 0.f;
    /// If crop is a legume, call nitrogen fixation routine
    if (FloatEqual(IDC[i], 1.f) || FloatEqual(IDC[i], 2.f) ||
        FloatEqual(IDC[i], 3.f)) {
            PlantNitrogenFixed(i);
    }
    m_plantUpTkN[i] += m_fixN[i];
    plant_N[i] += m_plantUpTkN[i];
    //if (plant_N[i] > 0.f)
    //	cout<<"cell ID: "<<i<<", plantN: "<<plant_N[i]<<endl;
    /// compute nitrogen stress
    if (FloatEqual(IDC[i], 1.f) || FloatEqual(IDC[i], 2.f) ||
        FloatEqual(IDC[i], 3.f)) {
            m_frStrsN[i] = 1.f;
    } else {
        PGCommon::calPlantStressByLimitedNP(plant_N[i], un2, &m_frStrsN[i]);
        float xx = 0.f;
        if (m_NO3Defic[i] > 1.e-5f) {
            xx = m_plantUpTkN[i] / m_NO3Defic[i];
        } else {
            xx = 1.f;
        }
        m_frStrsN[i] = max(m_frStrsN[i], xx);
        m_frStrsN[i] = min(m_frStrsN[i], 1.f);
    }
}

void Biomass_EPIC::PlantNitrogenFixed(int i) {
    /// compute the difference between supply and demand
    float uno3l = 0.f;
    if (m_NO3Defic[i] > m_plantUpTkN[i]) {
        uno3l = m_NO3Defic[i] - m_plantUpTkN[i];
    } else /// if supply is being met, fixation = 0 and return
    {
        m_fixN[i] = 0.f;
        return;
    }
    /// compute fixation as a function of no3, soil water, and growth stage
    //// 1. compute soil water factor
    float fxw = 0.f;
    fxw = solsw[i] / (0.85f * sol_sumAWC[i]);
    //// 2. compute no3 factor
    float sumn = 0.f; /// total amount of nitrate stored in soil profile (kg/ha)
    float fxn = 0.f;
    for (int l = 0; l < soillayers[i]; l++) {
        sumn += sol_no3[i][l];
    }
    if (sumn > 300.f) fxn = 0.f;
    if (sumn > 100.f && sumn <= 300.f) fxn = 1.5f - 0.0005f * sumn;
    if (sumn <= 100.f) fxn = 1.f;
    //// 3. compute growth stage factor
    float fxg = 0.f;
    if (frPHUacc[i] > 0.15f && frPHUacc[i] <= 0.30f) {
        fxg = 6.67f * frPHUacc[i] - 1.f;
    }
    if (frPHUacc[i] > 0.30f && frPHUacc[i] <= 0.55f) {
        fxg = 1.f;
    }
    if (frPHUacc[i] > 0.55f && frPHUacc[i] <= 0.75f) {
        fxg = 3.75f - 5.f * frPHUacc[i];
    }
    float fxr = min(1.f, min(fxw, fxn)) * fxg;
    fxr = max(0.f, fxr);
    if (nfixco <= 0.f) nfixco = 0.5f;
    if (nfixmx <= 0.f) nfixmx = 20.f;
    m_fixN[i] = min(6.f, fxr * m_NO3Defic[i]);
    m_fixN[i] = nfixco * m_fixN[i] + (1.f - nfixco) * uno3l;
    m_fixN[i] = min(m_fixN[i], uno3l);
    m_fixN[i] = min(nfixmx, m_fixN[i]);
}

void Biomass_EPIC::PlantPhosphorusUptake(int i) {
    float uobp = PGCommon::getNormalization(p_updis);
    //// determine shape parameters for plant phosphorus uptake equation, from readplant.f
    frPlantP[i] = PGCommon::NPBiomassFraction(BP1[i], BP2[i], BP3[i], frPHUacc[i]);
    float up2 = 0.f; /// optimal plant phosphorus content
    float uapd = 0.f; /// plant demand of phosphorus
    float upmx = 0.f; /// maximum amount of phosphorus that can be removed from the soil layer
    float uapl = 0.f; /// amount of phosphorus removed from layer
    float gx = 0.f; /// lowest depth in layer from which phosphorus may be removed
    up2 = frPlantP[i] * BIOMASS[i];
    if (up2 < plant_P[i]) up2 = plant_P[i];
    uapd = up2 - plant_P[i];
    uapd *= 1.5f;   /// luxury p uptake
    m_frStrsP[i] = 1.f;
    int ir = 0;
    if (uapd < UTIL_ZERO) return;
    for (int l = 0; l < soillayers[i]; l++) {
        if (ir > 0) break;
        if (m_soilRD[i] <= soilDepth[i][l]) {
            gx = m_soilRD[i];
            ir = 1;
        } else {
            gx = soilDepth[i][l];
        }
        upmx = uapd * (1.f - exp(-p_updis * gx / m_soilRD[i])) / uobp;
        uapl = min(upmx - m_plantUpTkP[i], sol_solp[i][l]);
        m_plantUpTkP[i] += uapl;
        sol_solp[i][l] -= uapl;
    }
    if (m_plantUpTkP[i] < 0.f) m_plantUpTkP[i] = 0.f;
    plant_P[i] += m_plantUpTkP[i];
    /// compute phosphorus stress
    PGCommon::calPlantStressByLimitedNP(plant_P[i], up2, &m_frStrsP[i]);
}

void Biomass_EPIC::CheckDormantStatus(int i) {
    /// TODO
    return;
}

int Biomass_EPIC::Execute() {
    CheckInputData();
    initialOutputs();
    //cout<<"BIOEPIC, pre solno3: ";
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	for (int j = 0; j < (int)soillayers[i]; j++){
    //		if (sol_no3[i][j] != sol_no3[i][j])
    //			cout<<"cellid: "<<i<<"lyr: "<<j<<", "<<sol_no3[i][j]<<endl;
    //	}
    //}
    //cout<<endl;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        /// calculate albedo in current day, albedo.f of SWAT
        float cej = -5.e-5f, eaj = 0.f;
        eaj = exp(cej * (sol_cov[i] + 0.1f));
        if (SNAC[i] < 0.5f) {
            ALBDAY[i] = sol_alb[i];
            if (LAIDAY[i] > 0.f) {
                ALBDAY[i] = 0.23f * (1.f - eaj) + sol_alb[i] * eaj;
            }
        } else {
            ALBDAY[i] = 0.8f;
        }
        /// reWrite from plantmod.f of SWAT
        /// calculate residue on soil surface for current day
        sol_cov[i] = max(0.8f * BIOMASS[i] + sol_rsd[i][0], 0.f);
        if (FloatEqual(IGRO[i], 1.f))            /// land cover growing
        {
            DistributePlantET(i);                  /// swu.f
            if (FloatEqual(dormi[i], 0.f)) {    /// plant will not undergo stress if dormant
                AdjustPlantGrowth(i);
            }              /// plantmod.f
            CheckDormantStatus(i);                 /// dormant.f
        }
    }
    //cout<<"BIOEPIC, after solno3: ";
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	for (int j = 0; j < (int)soillayers[i]; j++){
    //		if (sol_no3[i][j] != sol_no3[i][j])
    //			cout<<"cellid: "<<i<<"lyr: "<<j<<", "<<sol_no3[i][j]<<endl;
    //	}
    //}
    //cout<<endl;
    return 0;
}

void Biomass_EPIC::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = m_nCells;
    if (StringMatch(sk, VAR_BIOMASS)) { *data = BIOMASS; }
    else if (StringMatch(sk, VAR_LAST_SOILRD)) { *data = lastSoilRD; }
    else if (StringMatch(sk, VAR_PLANT_P)) { *data = plant_P; }
    else if (StringMatch(sk, VAR_PLANT_N)) { *data = plant_N; }
    else if (StringMatch(sk, VAR_FR_PLANT_N)) { *data = frPlantN; }
    else if (StringMatch(sk, VAR_FR_PLANT_P)) { *data = frPlantP; }
    else if (StringMatch(sk, VAR_AET_PLT)) { *data = AET_PLT; }
    else if (StringMatch(sk, VAR_PLTPET_TOT)) { *data = plt_pet_tot; }
    else if (StringMatch(sk, VAR_PLTET_TOT)) { *data = plt_et_tot; }
    else if (StringMatch(sk, VAR_FR_PHU_ACC)) { *data = frPHUacc; }
    else if (StringMatch(sk, VAR_LAIDAY)) { *data = LAIDAY; }
    else if (StringMatch(sk, VAR_LAIYRMAX)) { *data = laiyrmax; }
    else if (StringMatch(sk, VAR_LAIMAXFR)) { *data = laimaxfr; }
    else if (StringMatch(sk, VAR_OLAI)) { *data = olai; }
    else if (StringMatch(sk, VAR_ALBDAY)) { *data = ALBDAY; }
    else if (StringMatch(sk, VAR_DORMI)) { *data = dormi; }
    else if (StringMatch(sk, VAR_IGRO)) { *data = IGRO; }
    else if (StringMatch(sk, VAR_HVSTI_ADJ)) { *data = hvsti_adj; }
    else if (StringMatch(sk, VAR_CHT)) { *data = CHT; }
    else if (StringMatch(sk, VAR_FR_ROOT)) { *data = frRoot; }
    else if (StringMatch(sk, VAR_FR_STRSWTR)) { *data = frStrsWtr; }
    else if (StringMatch(sk, VAR_SOL_COV)) { *data = sol_cov; }
    else if (StringMatch(sk, VAR_SOL_SW)) { *data = solsw; }
    else {
        throw ModelException(MID_BIO_EPIC, "Get1DData", "Result " + sk +
            " does not exist in current module. Please contact the module developer.");
    }
}

void Biomass_EPIC::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = soilLayers;
    if (StringMatch(sk, VAR_SOL_RSD)) { *data = sol_rsd; }
    else {
        throw ModelException(MID_BIO_EPIC, "Get2DData", "Result " + sk + " does not exist.");
    }
}