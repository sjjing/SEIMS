#include "seims.h"
#include "AET_PriestleyTaylorHargreaves.h"

using namespace std;

AET_PT_H::AET_PT_H(void) : m_nCells(-1), nSoilLayers(-1), esco(NULL), soillayers(NULL), soilDepth(NULL),
                           soilthick(NULL), sol_awc(NULL),
    /// input from other modules
                           TMEAN(NULL), LAIDAY(NULL), PET(NULL), INET(NULL), SNAC(NULL), SNSB(NULL),
                           sol_cov(NULL), sol_no3(NULL), solst(NULL), solsw(NULL),
    /// output
                           PPT(NULL), SOET(NULL), sno3up(0.f) {
}

AET_PT_H::~AET_PT_H(void) {
    /// clean up output variables
    if (PPT != NULL) Release1DArray(PPT);
    if (SOET != NULL) Release1DArray(SOET);
}

void AET_PT_H::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_ESCO)) { esco = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, DataType_MeanTemperature)) { TMEAN = data; }
    else if (StringMatch(sk, VAR_LAIDAY)) { LAIDAY = data; }
    else if (StringMatch(sk, VAR_PET)) { PET = data; }
    else if (StringMatch(sk, VAR_INET)) { INET = data; }
    else if (StringMatch(sk, VAR_SNAC)) { SNAC = data; }
    else if (StringMatch(sk, VAR_SNSB)) { SNSB = data; }
    else if (StringMatch(sk, VAR_SOL_COV)) { sol_cov = data; }
    else if (StringMatch(sk, VAR_SOL_SW)) { solsw = data; }
    else {
        throw ModelException(MID_AET_PTH, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void AET_PT_H::Set2DData(const char *key, int n, int col, float **data) {
    string sk(key);
    CheckInputSize(key, n);
    nSoilLayers = col;
    if (StringMatch(sk, VAR_SOILDEPTH)) { soilDepth = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { sol_awc = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { solst = data; }
    else {
        throw ModelException(MID_AET_PTH, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

bool AET_PT_H::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_AET_PTH, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_AET_PTH, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

bool AET_PT_H::CheckInputData(void) {
    if (this->m_date <= 0) throw ModelException(MID_AET_PTH, "CheckInputData", "You have not set the time.");
    if (this->m_nCells <= 0) {
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (this->esco == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The soil evaporation compensation factor can not be NULL.");
    }
    if (this->soillayers == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The soil layers can not be NULL.");
    }
    if (this->TMEAN == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The mean temperature can not be NULL.");
    }
    if (this->LAIDAY == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The leaf area index can not be NULL.");
    }
    if (this->PET == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The potential evaportranspiration can not be NULL.");
    }
    if (this->SNAC == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The snow accumulation can not be NULL.");
    }
    /// If m_snowSB is not provided, it will be initialized in initialOutputs().
    //if (this->m_snowSB == NULL)
    //    throw ModelException(MID_AET_PTH, "CheckInputData", "The snow sublimation can not be NULL.");
    if (this->sol_cov == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The residue on soil surface can not be NULL.");
    }
    if (this->soilDepth == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The soil depth can not be NULL.");
    }
    if (this->soilthick == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The soil thickness can not be NULL.");
    }
    if (this->sol_awc == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The available water capacity at field capacity can not be NULL.");
    }
    if (this->sol_no3 == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "Nitrogen stored in the nitrate pool can not be NULL.");
    }
    if (this->solst == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The soil storage can not be NULL.");
    }
    if (this->solsw == NULL) {
        throw ModelException(MID_AET_PTH, "CheckInputData", "The soil storage of soil profile can not be NULL.");
    }
    return true;
}

void AET_PT_H::initialOutputs() {
    /// initialize output variables
    if (this->PPT == NULL) Initialize1DArray(m_nCells, PPT, 0.f);
    if (this->SOET == NULL) Initialize1DArray(m_nCells, SOET, 0.f);
    if (this->SNSB == NULL) Initialize1DArray(m_nCells, SNSB, 0.f);
}

int AET_PT_H::Execute() {
    CheckInputData();
    initialOutputs();
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        /// define intermediate variables
        float esd = 0.f, etco = 0.f, effnup = 0.f;
        float no3up = 0.f, es_max = 0.f, eos1 = 0.f, xx = 0.f;
        float cej = 0.f, eaj = 0.f, pet = 0.f, esleft = 0.f;
        float evzp = 0.f, eosl = 0.f, dep = 0.f, evz = 0.f, sev = 0.f;
        pet = PET[i] - INET[i];
        esd = 500.f;
        etco = 0.8f;
        effnup = 0.1f;

        if (pet < UTIL_ZERO) {
            pet = 0.f;
            PPT[i] = 0.f; // i.e., ep_max
            es_max = 0.f;
        } else {
            /// compute potential plant evapotranspiration (PPT) other than Penman-Monteith method
            if (LAIDAY[i] <= 3.f) {
                PPT[i] = LAIDAY[i] * pet / 3.f;
            } else {
                PPT[i] = pet;
            }
            if (PPT[i] < 0.f) PPT[i] = 0.f;
            /// compute potential soil evaporation
            cej = -5.e-5f;
            eaj = 0.f;
            es_max = 0.f;  ///maximum amount of evaporation (soil et)
            eos1 = 0.f;
            if (SNAC[i] >= 0.5f) {
                eaj = 0.5f;
            } else {
                eaj = exp(cej * (sol_cov[i] + 0.1f));
            }
            es_max = pet * eaj;
            eos1 = pet / (es_max + PPT[i] + 1.e-10f);
            eos1 = es_max * eos1;
            es_max = min(es_max, eos1);
            es_max = max(es_max, 0.f);
            /// make sure maximum plant and soil ET doesn't exceed potential ET
            if (pet < es_max + PPT[i] && !FloatEqual(es_max + PPT[i], 0.f)) {
                es_max = pet * es_max / (es_max + PPT[i]);
                PPT[i] = pet * PPT[i] / (es_max + PPT[i]);
            }
            if (pet < es_max + PPT[i]) {
                es_max = pet - PPT[i] - UTIL_ZERO;
            }

            /// initialize soil evaporation variables
            esleft = es_max;
            /// compute sublimation, using the input m_snowSB from snow sublimation module, if not provided, initialized as 0
            if (TMEAN[i] > 0.f) {
                if (SNAC[i] >= esleft) {
                    /// take all soil evap from snow cover
                    SNAC[i] -= esleft;
                    SNSB[i] += esleft;
                    esleft = 0.f;
                } else {
                    /// take all soil evap from snow cover then start taking from soil
                    esleft -= SNAC[i];
                    SNSB[i] += SNAC[i];
                    SNAC[i] = 0.f;
                }
            }
            // take soil evap from each soil layer
            evzp = 0.f;
            eosl = esleft;
            for (int ly = 0; ly < (int) soillayers[i]; ly++) {
                dep = 0.f;
                /// depth exceeds max depth for soil evap (esd, by default 500 mm)
                if (ly == 0) {
                    dep = soilDepth[i][ly];
                } else {
                    dep = soilDepth[i][ly - 1];
                }
                if (dep < esd) {
                    /// calculate evaporation from soil layer
                    evz = 0.f;
                    sev = 0.f;
                    xx = 0.f;
                    evz = eosl * soilDepth[i][ly] /
                        (soilDepth[i][ly] + exp(2.374f - 0.00713f * soilDepth[i][ly]));
                    sev = evz - evzp * esco[i];
                    evzp = evz;
                    if (solst[i][ly] < sol_awc[i][ly]) {
                        xx = 2.5f * (solst[i][ly] - sol_awc[i][ly]) / sol_awc[i][ly]; /// non dimension
                        sev *= Expo(xx);
                    }
                    sev = min(sev, solst[i][ly] * etco);
                    if (sev < 0.f || sev != sev) sev = 0.f;
                    if (sev > esleft) sev = esleft;
                    /// adjust soil storage, potential evap
                    if (solst[i][ly] > sev) {
                        esleft -= sev;
                        solst[i][ly] = max(UTIL_ZERO, solst[i][ly] - sev);
                    } else {
                        esleft -= solst[i][ly];
                        solst[i][ly] = 0.f;
                    }
                }
                /// compute no3 flux from layer 2 to 1 by soil evaporation
                if (ly == 1)  /// index of layer 2 is 1 (soil surface, 10mm)
                {
                    no3up = 0.f;
                    no3up = effnup * sev * sol_no3[i][ly] / (solst[i][ly] + UTIL_ZERO);
                    no3up = min(no3up, sol_no3[i][ly]);
                    sno3up += no3up / m_nCells;
                    sol_no3[i][ly] -= no3up;
                    sol_no3[i][ly - 1] += no3up;
                }
            }
            /// update total soil water content
            solsw[i] = 0.f;
            for (int ly = 0; ly < (int) soillayers[i]; ly++) {
                solsw[i] += solst[i][ly];
            }
            /// calculate actual amount of evaporation from soil
            //if (esleft != esleft || es_max != es_max)
            //	cout<<"esleft: "<<esleft<<", es_max: "<<es_max<<endl;
            if (es_max > esleft) {
                SOET[i] = es_max - esleft;
            } else {
                SOET[i] = 0.f;
            }
        }
    }
    //cout<<"AET_PTH, cell id 5878, sol_no3[0]: "<<m_solNo3[5878][0]<<endl;
    return true;
}

//m_ppt(NULL), m_solAET(NULL), sno3up(NODATA), solsw(NULL)
void AET_PT_H::GetValue(const char *key, float *value) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SNO3UP)) { *value = this->sno3up; }
    else {
        throw ModelException(MID_AET_PTH, "GetValue", "Result " + sk + " does not exist.");
    }
}

void AET_PT_H::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_PPT)) { *data = this->PPT; }
    else if (StringMatch(sk, VAR_SOET)) { *data = this->SOET; }
    else if (StringMatch(sk, VAR_SNAC)) { *data = this->SNAC; }
    else if (StringMatch(sk, VAR_SNSB)) { *data = SNSB; }
    else {
        throw ModelException(MID_AET_PTH, "Get1DData", "Result " + sk + " does not exist.");
    }
    *n = this->m_nCells;
}
