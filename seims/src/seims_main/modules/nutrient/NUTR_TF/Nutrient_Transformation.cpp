#include "seims.h"
#include "Nutrient_Transformation.h"

using namespace std;

Nutrient_Transformation::Nutrient_Transformation(void) :
//input
    m_nCells(-1), CELLWIDTH(-1.f), soilLayers(-1), cmn(-1.f), cdn(-1.f), sdnco(-1.f), nactfr(-1.f),
    psp(-1.f), ssp(-1.f),
    soillayers(NULL), soilDepth(NULL), soilthick(NULL), CLAY(NULL), density(NULL),
    landcover(NULL), rsdco_pl(NULL), sol_cbn(NULL), a_days(NULL), b_days(NULL),
    sol_wpmm(NULL), sol_awc(NULL), sol_ul(NULL), Porosity(NULL), sand(NULL),
    sol_solp(NULL), sol_orgp(NULL), sol_actp(NULL), sol_stap(NULL), sol_fop(NULL),
    sol_no3(NULL), sol_nh4(NULL), sol_orgn(NULL), sol_aorgn(NULL), sol_fon(NULL),
    sol_cov(NULL), sol_rsd(NULL),
    /// from other modules
    solst(NULL), SOTE(NULL),
    /// cell scale output
    hmntl(NULL), hmptl(NULL), rmn2tl(NULL),
    rmptl(NULL), rwntl(NULL), wdntl(NULL), rmp1tl(NULL), roctl(NULL),
    /// watershed scale statistics
    wshd_dnit(-1.f), wshd_hmn(-1.f), wshd_hmp(-1.f), wshd_rmn(-1.f), wshd_rmp(-1.f),
    wshd_rwn(-1.f), wshd_nitn(-1.f), wshd_voln(-1.f), wshd_pal(-1.f), wshd_pas(-1.f),
    solP_model(0), cswat(0),
    /// tillage factor on SOM decomposition, used by CENTURY model
    tillage_switch(NULL), tillage_depth(NULL), tillage_days(NULL), tillage_factor(NULL),
    /// CENTURY model related variables
    sol_WOC(NULL), sol_WON(NULL), sol_BM(NULL), sol_BMC(NULL), sol_BMN(NULL),
    sol_HP(NULL), sol_HS(NULL), sol_HSC(NULL), sol_HSN(NULL), sol_HPC(NULL),
    sol_HPN(NULL), sol_LM(NULL), sol_LMC(NULL), sol_LMN(NULL), sol_LSC(NULL),
    sol_LSN(NULL), sol_LS(NULL), sol_LSL(NULL), sol_LSLC(NULL), sol_LSLNC(NULL),
    sol_RNMN(NULL), sol_RSPC(NULL),
    conv_wt(NULL) {
}

Nutrient_Transformation::~Nutrient_Transformation(void) {
    if (hmntl != NULL) Release1DArray(hmntl);
    if (hmptl != NULL) Release1DArray(hmptl);
    if (rmn2tl != NULL) Release1DArray(rmn2tl);
    if (rmptl != NULL) Release1DArray(rmptl);
    if (rwntl != NULL) Release1DArray(rwntl);
    if (wdntl != NULL) Release1DArray(wdntl);
    if (rmp1tl != NULL) Release1DArray(rmp1tl);
    if (roctl != NULL) Release1DArray(roctl);
    if (a_days != NULL) Release1DArray(a_days);
    if (b_days != NULL) Release1DArray(b_days);
    /// release CENTURY related variables
    if (sol_WOC != NULL) Release2DArray(m_nCells, sol_WOC);
    if (sol_WON != NULL) Release2DArray(m_nCells, sol_WON);
    if (sol_BM != NULL) Release2DArray(m_nCells, sol_BM);
    if (sol_BMC != NULL) Release2DArray(m_nCells, sol_BMC);
    if (sol_BMN != NULL) Release2DArray(m_nCells, sol_BMN);
    if (sol_HP != NULL) Release2DArray(m_nCells, sol_HP);
    if (sol_HS != NULL) Release2DArray(m_nCells, sol_HS);
    if (sol_HSC != NULL) Release2DArray(m_nCells, sol_HSC);
    if (sol_HSN != NULL) Release2DArray(m_nCells, sol_HSN);
    if (sol_HPC != NULL) Release2DArray(m_nCells, sol_HPC);
    if (sol_HPN != NULL) Release2DArray(m_nCells, sol_HPN);
    if (sol_LM != NULL) Release2DArray(m_nCells, sol_LM);
    if (sol_LMC != NULL) Release2DArray(m_nCells, sol_LMC);
    if (sol_LMN != NULL) Release2DArray(m_nCells, sol_LMN);
    if (sol_LSC != NULL) Release2DArray(m_nCells, sol_LSC);
    if (sol_LSN != NULL) Release2DArray(m_nCells, sol_LSN);
    if (sol_LS != NULL) Release2DArray(m_nCells, sol_LS);
    if (sol_LSL != NULL) Release2DArray(m_nCells, sol_LSL);
    if (sol_LSLC != NULL) Release2DArray(m_nCells, sol_LSLC);
    if (sol_LSLNC != NULL) Release2DArray(m_nCells, sol_LSLNC);
    if (sol_RNMN != NULL) Release2DArray(m_nCells, sol_RNMN);
    if (sol_RSPC != NULL) Release2DArray(m_nCells, sol_RSPC);

    if (conv_wt != NULL) Release2DArray(m_nCells, conv_wt);
}

bool Nutrient_Transformation::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_NUTR_TF, "CheckInputSize",
                                 "Input data for " + string(key) + " is invalid with size: " + ValueToString(n) +
                                     ". The origin size is " + ValueToString(m_nCells) + ".\n");
        }
    }
    return true;
}

bool Nutrient_Transformation::CheckInputData() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The input data can not be less than zero.");
    }
    if (this->soilLayers < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The maximum soil layers number can not be NULL.");
    }
    if (this->CELLWIDTH < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The cell width can not be less than 0.");
    }
    if (this->soillayers == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The soil layers can not be NULL.");
    }
    if (this->cmn < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The cmn can not be less than 0.");
    }
    if (this->cdn < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The cdn can not be less than 0.");
    }
    if (this->landcover == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The landcover can not be NULL.");
    }
    if (this->nactfr < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The nactfr can not be less than 0.");
    }
    if (this->sdnco < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sdnco can not be less than 0.");
    }
    if (this->psp < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The psp can not be less than 0.");
    }
    if (this->ssp < 0) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The ssp can not be less than 0.");
    }
    if (this->CLAY == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The CLAY can not be NULL.");
    }
    if (this->soilDepth == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The soilDepth can not be NULL.");
    }
    if (this->rsdin == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The rsdin can not be NULL.");
    }
    //if (sol_cov == NULL)
    //	throw ModelException(MID_NUTR_TF, "CheckInputData", "The residue on soil surface can not be NULL.");
    //if (sol_rsd == NULL)
    //	throw ModelException(MID_NUTR_TF, "CheckInputData", "The organic matter in soil classified as residue can not be NULL.");
    //if (this->a_days == NULL) {
    //    throw ModelException(MID_NUTR_TF, "CheckInputData", "The a_days can not be NULL.");
    //}
    //if (this->b_days == NULL) {
    //    throw ModelException(MID_NUTR_TF, "CheckInputData", "The b_days can not be NULL.");
    //}
    if (this->soilthick == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The soilthick can not be NULL.");
    }
    if (this->density == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The density can not be NULL.");
    }
    if (this->rsdco_pl == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The rsdco_pl can not be NULL.");
    }
    if (this->sol_cbn == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_cbn can not be NULL.");
    }
    if (this->sol_awc == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_awc can not be NULL.");
    }
    if (this->sol_wpmm == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_wpmm can not be NULL.");
    }
    if (this->sol_no3 == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_no3 can not be NULL.");
    }
    if (this->sol_nh4 == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_nh4 can not be NULL.");
    }
    if (this->sol_orgn == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_orgn can not be NULL.");
    }
    if (this->sol_orgp == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_orgp can not be NULL.");
    }
    if (this->sol_solp == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The sol_solp can not be NULL.");
    }
    if (this->solst == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The solst can not be NULL.");
    }
    if (this->SOTE == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData", "The SOTE can not be NULL.");
    }
    if (this->sol_ul == NULL) {
        throw ModelException(MID_NUTR_TF, "CheckInputData",
                             "The amount of water held in the soil layer at saturation data can not be NULL.");
    }
    return true;
}

void Nutrient_Transformation::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, Tag_CellWidth)) { this->CELLWIDTH = value; }
    else if (StringMatch(sk, VAR_NACTFR)) { this->nactfr = value; }
    else if (StringMatch(sk, VAR_SDNCO)) { this->sdnco = value; }
    else if (StringMatch(sk, VAR_CMN)) { this->cmn = value; }
    else if (StringMatch(sk, VAR_CDN)) { this->cdn = value; }
    else if (StringMatch(sk, VAR_PSP)) { this->psp = value; }
    else if (StringMatch(sk, VAR_SSP)) { this->ssp = value; }
    else if (StringMatch(sk, VAR_CSWAT)) { this->cswat = (int) value; }
    else {
        throw ModelException(MID_NUTR_TF, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void Nutrient_Transformation::Set1DData(const char *key, int n, float *data) {
    if (!this->CheckInputSize(key, n)) return;
    string sk(key);
    if (StringMatch(sk, VAR_LCC)) { this->landcover = data; }
    else if (StringMatch(sk, VAR_PL_RSDCO)) { this->rsdco_pl = data; }
    else if (StringMatch(sk, VAR_SOL_RSDIN)) { this->rsdin = data; }
    else if (StringMatch(sk, VAR_SOL_COV)) { this->sol_cov = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) { this->soillayers = data; }
    else if (StringMatch(sk, VAR_SOTE)) { this->SOTE = data; }
    else if (StringMatch(sk, VAR_A_DAYS)) { this->a_days = data; }
    else if (StringMatch(sk, VAR_B_DAYS)) { this->b_days = data; }
        /// tillage related variables of CENTURY model
    else if (StringMatch(sk, VAR_TILLAGE_DAYS)) { tillage_days = data; }
    else if (StringMatch(sk, VAR_TILLAGE_DEPTH)) { tillage_depth = data; }
    else if (StringMatch(sk, VAR_TILLAGE_FACTOR)) { tillage_factor = data; }
    else if (StringMatch(sk, VAR_TILLAGE_SWITCH)) { tillage_switch = data; }
    else {
        throw ModelException(MID_NUTR_TF, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void Nutrient_Transformation::Set2DData(const char *key, int nRows, int nCols, float **data) {
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);
    soilLayers = nCols;
    if (StringMatch(sk, VAR_SOL_CBN)) { this->sol_cbn = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { this->density = data; }
    else if (StringMatch(sk, VAR_CLAY)) { this->CLAY = data; }
    else if (StringMatch(sk, VAR_ROCK)) { this->rock = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { this->solst = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { this->sol_awc = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { this->sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_NH4)) { this->sol_nh4 = data; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { this->sol_orgn = data; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { this->sol_orgp = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { this->sol_solp = data; }
    else if (StringMatch(sk, VAR_SOL_WPMM)) { this->sol_wpmm = data; }
    else if (StringMatch(sk, VAR_SOILDEPTH)) { this->soilDepth = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { this->soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_RSD)) { this->sol_rsd = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else if (StringMatch(sk, VAR_POROST)) { Porosity = data; }
    else if (StringMatch(sk, VAR_SAND)) { sand = data; }
    else {
        throw ModelException(MID_NUTR_TF, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void Nutrient_Transformation::initialOutputs() {
    if (hmntl == NULL) Initialize1DArray(m_nCells, hmntl, 0.f);
    if (hmptl == NULL) Initialize1DArray(m_nCells, hmptl, 0.f);
    if (rmn2tl == NULL) Initialize1DArray(m_nCells, rmn2tl, 0.f);
    if (rmptl == NULL) Initialize1DArray(m_nCells, rmptl, 0.f);
    if (rwntl == NULL) Initialize1DArray(m_nCells, rwntl, 0.f);
    if (wdntl == NULL) Initialize1DArray(m_nCells, wdntl, 0.f);
    if (rmp1tl == NULL) Initialize1DArray(m_nCells, rmp1tl, 0.f);
    if (roctl == NULL) Initialize1DArray(m_nCells, roctl, 0.f);
    if (a_days == NULL) Initialize1DArray(m_nCells, a_days, 0.f);
    if (b_days == NULL) Initialize1DArray(m_nCells, b_days, 0.f);
    if (sol_cov == NULL || sol_rsd == NULL) {
        Initialize1DArray(m_nCells, sol_cov, rsdin);
        Initialize2DArray(m_nCells, soilLayers, sol_rsd, 0.f);
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            sol_rsd[i][0] = sol_cov[i];
        }
    }
    /// initialize conv_wt
    if (conv_wt == NULL) {
        Initialize2DArray(m_nCells, soilLayers, conv_wt, 0.f);
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            for (int k = 0; k < (int) soillayers[i]; k++) {
                float wt1 = 0.f;
                float conv_wt1 = 0.f;
                // mg/kg => kg/ha
                wt1 = density[i][k] * soilthick[i][k] / 100.f;
                // kg/kg => kg/ha
                conv_wt1 = 1.e6f * wt1;
                conv_wt[i][k] = conv_wt1;
            }
        }
    }
    // initial input soil chemical in first run
    if (sol_no3 == NULL) Initialize2DArray(m_nCells, soilLayers, sol_no3, 0.f);
    if (sol_fon == NULL || sol_fop == NULL || sol_aorgn == NULL ||
        sol_actp == NULL || sol_stap == NULL) {
        Initialize2DArray(m_nCells, soilLayers, sol_fon, 0.f);
        Initialize2DArray(m_nCells, soilLayers, sol_fop, 0.f);
        Initialize2DArray(m_nCells, soilLayers, sol_aorgn, 0.f);
        Initialize2DArray(m_nCells, soilLayers, sol_actp, 0.f);
        Initialize2DArray(m_nCells, soilLayers, sol_stap, 0.f);

#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            // fresh organic P / N
            sol_fop[i][0] = sol_cov[i] * .0010f;
            sol_fon[i][0] = sol_cov[i] * .0055f;

            for (int k = 0; k < (int) soillayers[i]; k++) {
                //float wt1 = 0.f;
                //float conv_wt = 0.f;
                //// mg/kg => kg/ha
                //wt1 = density[i][k] * soilthick[i][k] / 100.f;
                //// kg/kg => kg/ha
                //conv_wt = 1.e6f * wt1;
                float wt1 = conv_wt[i][k] * 1.e-6f;
                /// if sol_no3 is not provided, { initialize it.
                if (sol_no3[i][k] <= 0.f) {
                    sol_no3[i][k] = 0.f;
                    float zdst = 0.f;
                    zdst = exp(-soilDepth[i][k] / 1000.f);
                    sol_no3[i][k] = 10.f * zdst * 0.7f;
                    sol_no3[i][k] *= wt1;    // mg/kg => kg/ha
                    //if(k == 0) outfile << sol_no3[i][k];
                }
                /// if sol_orgn is not provided, { initialize it.
                if (sol_orgn[i][k] <= 0.f) {
                    // CN ratio changed back to 14
                    sol_orgn[i][k] = 10000.f * (sol_cbn[i][k] / 14.f) * wt1;
                }
                // assume C:N ratio of 10:1
                // nitrogen active pool fraction (nactfr)
                float nactfr = .02f;
                sol_aorgn[i][k] = sol_orgn[i][k] * nactfr;
                sol_orgn[i][k] *= (1.f - nactfr);

                // currently not used
                //sumorgn = sumorgn + sol_aorgn[i][k] + sol_orgn[i][k] + sol_fon[i][k];

                if (sol_orgp[i][k] <= 0.f) {
                    // assume N:P ratio of 8:1
                    sol_orgp[i][k] = 0.125f * sol_orgn[i][k];
                }

                if (sol_solp[i][k] <= 0.f) {
                    // assume initial concentration of 5 mg/kg
                    sol_solp[i][k] = 5.f * wt1;
                }

                float solp = 0.f;
                float actp = 0.f;
                float psp = psp;
                if (solP_model == 0)// Set active pool based on dynamic PSP MJW
                {
                    // Allow Dynamic PSP Ratio
                    if (conv_wt[i][k] != 0) solp = (sol_solp[i][k] / conv_wt[i][k]) * 1000000.f;
                } else {
                    throw ModelException(MID_NUTR_TF, "initialOutputs",
                                         "Please check the bulk density and soil thickness data.");
                    if (CLAY[i][k] > 0.f) {
                        psp = -0.045f * log(CLAY[i][k]) + (0.001f * solp);
                        psp = psp - (0.035f * sol_cbn[i][k]) + 0.43f;
                    } else {
                        psp = 0.4f;
                    }
                    // Limit PSP range
                    if (psp < .05f) { psp = 0.05f; }
                    else if (psp > 0.9f) psp = 0.9f;
                }

                sol_actp[i][k] = sol_solp[i][k] * (1.f - psp) / psp;

                if (solP_model == 0) // Set Stable pool based on dynamic coefficient
                {                      // From White et al 2009
                    // convert to concentration for ssp calculation
                    actp = sol_actp[i][k] / conv_wt[i][k] * 1000000.f;
                    solp = sol_solp[i][k] / conv_wt[i][k] * 1000000.f;
                    // estimate Total Mineral P in this soil based on data from sharpley 2004
                    float ssp = 0.;
                    ssp = 25.044f * pow((actp + solp), -0.3833f);
                    // limit SSP Range
                    if (ssp > 7.f) ssp = 7.f;
                    if (ssp < 1.f) ssp = 1.f;
                    // define stableP
                    sol_stap[i][k] = ssp * (sol_actp[i][k] + sol_solp[i][k]);
                } else {
                    // The original code
                    sol_stap[i][k] = 4.f * sol_actp[i][k];
                }
                //m_sol_hum[i][k] = sol_cbn[i][k] * wt1 * 17200.f;
                //summinp = summinp + sol_solp[i][k] + sol_actp[i][k] + sol_stap[i][k];
                //sumorgp = sumorgp + sol_orgp[i][k] + sol_fop[i][k];
            }
        }
    }
    /**** initialization of CENTURY model related variables *****/
    /****                soil_chem.f of SWAT                *****/
    if (cswat == 2) {
        /// definition of temporary parameters
        float sol_mass = 0.f, FBM = 0.f, FHP = 0.f;
        float x1 = 0.f, RTO = 0.f; //, FHS = 0.f, sol_min_n = 0.f;
        if (sol_WOC == NULL) {
            Initialize2DArray(m_nCells, soilLayers, sol_WOC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_WON, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_BM, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_BMC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_BMN, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HP, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HS, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HSC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HSN, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HPC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_HPN, 0.f);

            Initialize2DArray(m_nCells, soilLayers, sol_LM, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LMC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LMN, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LSC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LSN, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LS, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LSL, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LSLC, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_LSLNC, 0.f);

            Initialize2DArray(m_nCells, soilLayers, sol_RNMN, 0.f);
            Initialize2DArray(m_nCells, soilLayers, sol_RSPC, 0.f);

#pragma omp parallel for
            for (int i = 0; i < m_nCells; i++) {
                for (int k = 0; k < (int) soillayers[i]; k++) {
                    /// soil mass in each layer, kg/ha
                    sol_mass = 10000.f * soilthick[i][k] * density[i][k] * (1.f - rock[i][k] / 100.f);
                    /// mineral nitrogen, kg/ha
                    //sol_min_n = sol_no3[i][k] + sol_nh4[i][k];
                    sol_WOC[i][k] = sol_mass * sol_cbn[i][k] / 100.f;
                    sol_WON[i][k] = sol_aorgn[i][k] + sol_orgn[i][k];
                    /// fraction of Mirobial biomass, humus passive C pools
                    if (FBM < 1.e-10f) FBM = 0.04f;
                    if (FHP < 1.e-10f) FHP = 0.7f - 0.4f * exp(-0.277f * 100.f);
                    //FHS = 1.f - FBM - FHP;
                    sol_BM[i][k] = FBM * sol_WOC[i][k];
                    sol_BMC[i][k] = sol_BM[i][k];
                    RTO = sol_WON[i][k] / sol_WOC[i][k];
                    sol_BMN[i][k] = RTO * sol_BMC[i][k];
                    sol_HP[i][k] = FHP * (sol_WOC[i][k] - sol_BM[i][k]);
                    sol_HS[i][k] = sol_WOC[i][k] - sol_BM[i][k] - sol_HP[i][k];
                    sol_HSC[i][k] = sol_HS[i][k];
                    sol_HSN[i][k] = RTO * sol_HSC[i][k];
                    sol_HPC[i][k] = sol_HP[i][k]; /// same as sol_aorgn
                    sol_HPN[i][k] = RTO * sol_HPC[i][k]; // same as sol_orgn
                    x1 = sol_rsd[i][k] / 1000.f;
                    sol_LM[i][k] = 500.f * x1;
                    sol_LS[i][k] = sol_LM[i][k];
                    sol_LSL[i][k] = 0.8f * sol_LS[i][k];
                    sol_LMC[i][k] = 0.42f * sol_LM[i][k];

                    sol_LMN[i][k] = 0.1f * sol_LMC[i][k];
                    sol_LSC[i][k] = 0.42f * sol_LS[i][k];
                    sol_LSLC[i][k] = 0.8f * sol_LSC[i][k];
                    sol_LSLNC[i][k] = 0.2f * sol_LSC[i][k];
                    sol_LSN[i][k] = sol_LSC[i][k] / 150.f;

                    sol_WOC[i][k] += sol_LSC[i][k] + sol_LMC[i][k];
                    sol_WON[i][k] += sol_LSN[i][k] + sol_LMN[i][k];

                    sol_orgn[i][k] = sol_HPN[i][k];
                    sol_aorgn[i][k] = sol_HSN[i][k];
                    sol_fon[i][k] = sol_LMN[i][k] + sol_LSN[i][k];
                }
            }
        }
    }
    // allocate the output variables
    if (!FloatEqual(wshd_dnit, 0.f)) {
        wshd_dnit = 0.f;
        wshd_hmn = 0.f;
        wshd_hmp = 0.f;
        wshd_rmn = 0.f;
        wshd_rmp = 0.f;
        wshd_rwn = 0.f;
        wshd_nitn = 0.f;
        wshd_voln = 0.f;
        wshd_pal = 0.f;
        wshd_pas = 0.f;
    }
}

int Nutrient_Transformation::Execute() {
    CheckInputData();
    initialOutputs();
    //int cellid = 3406;
    //cout<<"NutrTF, pre solno3: ";
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	for (int j = 0; j < (int)soillayers[i]; j++){
    //		if (sol_no3[i][j] != sol_no3[i][j])
    //			cout<<"cellid: "<<i<<"lyr: "<<j<<", "<<sol_no3[i][j]<<endl;
    //	}
    //}
    //for (int j = 0; j < (int)soillayers[cellid]; j++)
    //	cout<<j<<", "<<sol_no3[cellid][j]<<", ";
    //cout<<endl;
    //cout<<"NUTR_TF, cell id: "<<cellid<<", sol_no3[0]: "<<sol_no3[cellid][0]<<endl;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        // compute nitrogen and phosphorus mineralization
        if (cswat == 0) {
            Mineralization_StaticCarbonMethod(i);
        } else if (cswat == 1) {
            Mineralization_CFARMOneCarbonModel(i);
        } else if (cswat == 2) {
            Mineralization_CENTURYModel(i);
        } else { /// throw exception if Carbon method invalid
            throw ModelException(MID_NUTR_TF, "ComputeCNPCycling", "Carbon modeling method must be 0, 1, or 2.");
        }
        //if (i == cellid){
        //	cout<<"NutrTF, after mineralization solno3: ";
        //	for (int j = 0; j < (int)soillayers[cellid]; j++)
        //		cout<<j<<", "<<sol_no3[cellid][j]<<", ";
        //	cout<<endl;
        //}
        //Calculate daily mineralization (NH3 to NO3) and volatilization of NH3
        Volatilization(i);
        //if (i == cellid){
        //	cout<<"NutrTF, after Volatilization solno3: ";
        //	for (int j = 0; j < (int)soillayers[cellid]; j++)
        //		cout<<j<<", "<<sol_no3[cellid][j]<<", ";
        //	cout<<endl;
        //}
        //Calculate P flux between the labile, active mineral and stable mineral P pools
        CalculatePflux(i);
    }
    //cout<<"NUTR_TF after, ";
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	for (int j = 0; j < (int)soillayers[i]; j++){
    //		if (sol_no3[i][j] != sol_no3[i][j])
    //			cout<<"cellid: "<<i<<"lyr: "<<j<<", "<<sol_no3[i][j]<<endl;
    //	}
    //}
    return 0;
}

void Nutrient_Transformation::Mineralization_StaticCarbonMethod(int i) {
    //soil layer (k)
    for (int k = 0; k < (int) soillayers[i]; k++) {
        //soil layer used to compute soil water and soil temperature factors
        int kk = 0;
        if (k == 0) {
            kk = 1;
        } else {
            kk = k;
        }
        float sut = 0.f;
        // mineralization can occur only if temp above 0 deg
        if (SOTE[i] > 0) {
            // compute soil water factor (sut)
            sut = 0.f;
            if (solst[i][kk] < 0) {
                solst[i][kk] = 0.0000001f;
            }
            sut = 0.1f + 0.9f * sqrt(solst[i][kk] / sol_awc[i][kk]);
            sut = max(0.05f, sut);

            //compute soil temperature factor
            //variable to hold intermediate calculation result (xx)
            float xx = 0.f;
            //soil temperature factor (cdg)
            float cdg = 0.f;
            xx = SOTE[i];
            cdg = 0.9f * xx / (xx + exp(9.93f - 0.312f * xx)) + 0.1f;
            cdg = max(0.1f, cdg);

            // compute combined factor
            xx = 0.f;
            //combined temperature/soil water factor (csf)
            float csf = 0.f;
            xx = cdg * sut;
            if (xx < 0) {
                xx = 0.f;
            }
            if (xx > 1000000) {
                xx = 1000000.f;
            }
            csf = sqrt(xx);

            // compute flow from active to stable pools
            //amount of nitrogen moving from active organic to stable organic pool in layer (rwn)
            float rwn = 0.f;
            rwn = 0.1e-4f * (sol_aorgn[i][k] * (1.f / nactfr - 1.f) - sol_orgn[i][k]);
            if (rwn > 0.f) {
                rwn = min(rwn, sol_aorgn[i][k]);
            } else {
                rwn = -(min(abs(rwn), sol_orgn[i][k]));
            }
            sol_orgn[i][k] = max(1.e-6f, sol_orgn[i][k] + rwn);
            sol_aorgn[i][k] = max(1.e-6f, sol_aorgn[i][k] - rwn);

            // compute humus mineralization on active organic n
            //amount of nitrogen moving from active organic nitrogen pool to nitrate pool in layer (hmn)
            float hmn = 0.f;
            hmn = cmn * csf * sol_aorgn[i][k];
            hmn = min(hmn, sol_aorgn[i][k]);
            // compute humus mineralization on active organic p
            xx = 0.f;
            //amount of phosphorus moving from the organic pool to the labile pool in layer (hmp)
            float hmp = 0.f;
            xx = sol_orgn[i][k] + sol_aorgn[i][k];
            if (xx > 1.e-6f) {
                hmp = 1.4f * hmn * sol_orgp[i][k] / xx;
            } else {
                hmp = 0.f;
            }
            hmp = min(hmp, sol_orgp[i][k]);

            // move mineralized nutrients between pools;
            sol_aorgn[i][k] = max(1.e-6f, sol_aorgn[i][k] - hmn);
            sol_no3[i][k] = sol_no3[i][k] + hmn;
            sol_orgp[i][k] = sol_orgp[i][k] - hmp;
            sol_solp[i][k] = sol_solp[i][k] + hmp;

            // compute residue decomp and mineralization of
            // fresh organic n and p (upper two layers only)
            //amount of nitrogen moving from fresh organic to nitrate(80%) and active organic(20%) pools in layer (rmn1)
            float rmn1 = 0.f;
            //amount of phosphorus moving from fresh organic to labile(80%) and organic(20%) pools in layer (rmp)
            float rmp = 0.f;
            if (k <= 2) {
                //the C:N ratio (cnr)
                float cnr = 0.f;
                //the C:P ratio (cnr)
                float cpr = 0.f;
                //the nutrient cycling water factor for layer (ca)
                float ca = 0.f;
                float cnrf = 0.f;
                float cprf = 0.f;
                if (sol_fon[i][k] + sol_no3[i][k] > 1e-4f) {
                    //Calculate cnr, equation 3:1.2.5 in SWAT Theory 2009, p189
                    cnr = 0.58f * sol_rsd[i][k] / (sol_fon[i][k] + sol_no3[i][k]);
                    if (cnr > 500) {
                        cnr = 500.f;
                    }
                    cnrf = exp(-0.693f * (cnr - 25.f) / 25.f);
                } else {
                    cnrf = 1.f;
                }
                if (sol_fop[i][k] + sol_solp[i][k] > 1e-4f) {
                    cpr = 0.58f * sol_rsd[i][k] / (sol_fop[i][k] + sol_solp[i][k]);
                    if (cpr > 5000) {
                        cpr = 5000.f;
                    }
                    cprf = 0.f;
                    cprf = exp(-0.693f * (cpr - 200.f) / 200.f);
                } else {
                    cprf = 1.f;
                }
                //decay rate constant (decr)
                float decr = 0.f;

                float rdc = 0.f;
                //Calculate ca, equation 3:1.2.8 in SWAT Theory 2009, p190
                ca = min(min(cnrf, cprf), 1.f);
                //if (landcover[i] > 0)
                //{
                //    decr = rsdco_pl[(int) landcover[i]] * ca * csf;
                //} }else{
                //{
                //    decr = 0.05f;
                //}
                if (rsdco_pl[i] < 0.f) {
                    decr = 0.05f;
                } else {
                    decr = rsdco_pl[i] * ca * csf;
                }
                decr = min(decr, 1.f);
                sol_rsd[i][k] = max(1.e-6f, sol_rsd[i][k]);
                rdc = decr * sol_rsd[i][k];
                sol_rsd[i][k] = sol_rsd[i][k] - rdc;
                if (sol_rsd[i][k] < 0)sol_rsd[i][k] = 0.f;
                rmn1 = decr * sol_fon[i][k];
                sol_fop[i][k] = max(1.e-6f, sol_fop[i][k]);
                rmp = decr * sol_fop[i][k];

                sol_fop[i][k] = sol_fop[i][k] - rmp;
                sol_fon[i][k] = max(1.e-6f, sol_fon[i][k]) - rmn1;;
                //debug
                //if (rmn1 != rmn1)
                //	cout<<"cellid: "<<i<<", lyr: "<<k<<", rsdco_pl: "<<
                //	rsdco_pl[i]<<", ca: "<<ca<<", csf: "<<csf<<endl;
                //Calculate no3, aorgn, solp, orgp, equation 3:1.2.9 in SWAT Theory 2009, p190
                sol_no3[i][k] = sol_no3[i][k] + 0.8f * rmn1;
                sol_aorgn[i][k] = sol_aorgn[i][k] + 0.2f * rmn1;
                sol_solp[i][k] = sol_solp[i][k] + 0.8f * rmp;
                sol_orgp[i][k] = sol_orgp[i][k] + 0.2f * rmp;
            }
            //  compute denitrification
            //amount of nitrogen lost from nitrate pool in layer due to denitrification
            float wdn = 0.f;
            //Calculate wdn, equation 3:1.4.1 and 3:1.4.2 in SWAT Theory 2009, p194
            if (sut >= sdnco) {
                wdn = sol_no3[i][k] * (1.f - exp(-cdn * cdg * sol_cbn[i][k]));
            } else {
                wdn = 0.f;
            }
            sol_no3[i][k] = sol_no3[i][k] - wdn;

            // Summary calculations
            wshd_hmn = wshd_hmn + hmn * (1.f / m_nCells);
            wshd_rwn = wshd_rwn + rwn * (1.f / m_nCells);
            wshd_hmp = wshd_hmp + hmp * (1.f / m_nCells);
            wshd_rmn = wshd_rmn + rmn1 * (1.f / m_nCells);
            wshd_rmp = wshd_rmp + rmp * (1.f / m_nCells);
            wshd_dnit = wshd_dnit + wdn * (1.f / m_nCells);
            //hmntl = hmntl + hmn;
            //rwntl = rwntl + rwn;
            //hmptl = hmptl + hmp;
            //rmn2tl = rmn2tl + rmn1;
            //rmptl = rmptl + rmp;
            //wdntl = wdntl + wdn;
            hmntl[i] = hmn;
            rwntl[i] = rwn;
            hmptl[i] = hmp;
            rmn2tl[i] = rmn1;
            rmptl[i] = rmp;
            wdntl[i] = wdn;
        }
    }
}

void Nutrient_Transformation::Mineralization_CFARMOneCarbonModel(int i) {
    /// TODO
}

void Nutrient_Transformation::Volatilization(int i) {
    //soil layer (k)
    float kk = 0.f;
    for (int k = 0; k < (int) soillayers[i]; k++) {
        //nitrification/volatilization temperature factor (nvtf)
        float nvtf = 0.f;
        //Calculate nvtf, equation 3:1.3.1 in SWAT Theory 2009, p192
        nvtf = 0.41f * (SOTE[i] - 5.f) / 10.f;
        if (sol_nh4[i][k] > 0.f && nvtf >= 0.001f) {
            float sw25 = 0.f;
            float swwp = 0.f;
            //nitrification soil water factor (swf)
            float swf = 0.f;
            //Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
            sw25 = sol_wpmm[i][k] + 0.25f * sol_awc[i][k];
            swwp = sol_wpmm[i][k] + solst[i][k];
            if (swwp < sw25) {
                swf = (swwp - sol_wpmm[i][k]) / (sw25 - sol_wpmm[i][k]);
            } else {
                swf = 1.f;
            }

            if (k == 0) {
                kk = 0.f;
            } else {
                kk = soilDepth[i][k - 1];
            }
            //depth from the soil surface to the middle of the layer (dmidl/mm)
            float dmidl = 0.f;
            //volatilization depth factor (dpf)
            float dpf = 0.f;
            //nitrification regulator (akn)
            float akn = 0.f;
            //volatilization regulator (akn)
            float akv = 0.f;
            //amount of ammonium converted via nitrification and volatilization in layer (rnv)
            float rnv = 0.f;
            //amount of nitrogen moving from the NH3 to the NO3 pool (nitrification) in the layer (rnit)
            float rnit = 0.f;
            //amount of nitrogen lost from the NH3 pool due to volatilization (rvol)
            float rvol = 0.f;
            //volatilization CEC factor (cecf)
            float cecf = 0.15f;
            //Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
            dmidl = (soilDepth[i][k] + kk) / 2.f;
            dpf = 1.f - dmidl / (dmidl + exp(4.706f - 0.0305f * dmidl));
            //Calculate rnv, equation 3:1.3.6, 3:1.3.7 and 3:1.3.8 in SWAT Theory 2009, p193
            akn = nvtf * swf;
            akv = nvtf * dpf * cecf;
            rnv = sol_nh4[i][k] * (1.f - exp(-akn - akv));
            //Calculate rnit, equation 3:1.3.9 in SWAT Theory 2009, p193
            rnit = 1.f - exp(-akn);
            //Calculate rvol, equation 3:1.3.10 in SWAT Theory 2009, p193
            rvol = 1.f - exp(-akv);

            //calculate nitrification (NH3 => NO3)
            if ((rvol + rnit) > 1.e-6f) {
                //Calculate the amount of nitrogen removed from the ammonium pool by nitrification,
                //equation 3:1.3.11 in SWAT Theory 2009, p193
                rvol = rnv * rvol / (rvol + rnit);
                //Calculate the amount of nitrogen removed from the ammonium pool by volatilization,
                //equation 3:1.3.12 in SWAT Theory 2009, p194
                rnit = rnv - rvol;
                if (rnit < 0)rnit = 0.f;
                sol_nh4[i][k] = max(1e-6f, sol_nh4[i][k] - rnit);
            }
            if (sol_nh4[i][k] < 0.f) {
                rnit = rnit + sol_nh4[i][k];
                sol_nh4[i][k] = 0.f;
            }
            //debug
            //if (rnit != rnit)
            //	cout<<"cellid: "<<i<<", lyr: "<<k<<", akn: "<<akn<<", nvtf:"<<
            //	nvtf<<", swf: "<<swf<<", dpf: "<<dpf<<", cecf: "<<cecf<<endl;
            sol_no3[i][k] = sol_no3[i][k] + rnit;
            //calculate ammonia volatilization
            sol_nh4[i][k] = max(1e-6f, sol_nh4[i][k] - rvol);
            if (sol_nh4[i][k] < 0.f) {
                rvol = rvol + sol_nh4[i][k];
                sol_nh4[i][k] = 0.f;
            }
            //summary calculations
            wshd_voln += rvol * (1.f / m_nCells);
            wshd_nitn += rnit * (1.f / m_nCells);
        }
    }
}

void Nutrient_Transformation::CalculatePflux(int i) {
    // float wt1 = 0.f;
    // float conv_wt = 0.f;
    for (int k = 0; k < (int) soillayers[i]; k++) {
        // mg/kg => kg/ha
        // wt1 = conv_wt[i][k] * 1.e-6f;
        // wt1 = density[i][k] * soilthick[i][k] / 100.f;
        // kg/kg => kg/ha
        // conv_wt = 1.e6f * wt1;

        // make sure that no zero or negative pool values come in
        if (sol_solp[i][k] <= 1.e-6) sol_solp[i][k] = 1.e-6f;
        if (sol_actp[i][k] <= 1.e-6) sol_actp[i][k] = 1.e-6f;
        if (sol_stap[i][k] <= 1.e-6) sol_stap[i][k] = 1.e-6f;

        // Convert kg/ha to ppm so that it is more meaningful to compare between soil layers
        float solp = 0.f;
        float actp = 0.f;
        float stap = 0.f;
        solp = sol_solp[i][k] / conv_wt[i][k] * 1000000.f;
        actp = sol_actp[i][k] / conv_wt[i][k] * 1000000.f;
        stap = sol_stap[i][k] / conv_wt[i][k] * 1000000.f;

        // ***************Soluble - Active Transformations***************
        // Dynamic PSP Ratio
        float psp = 0.f;
        //cout << psp << endl;
        //PSP = -0.045*log (% clay) + 0.001*(Solution P, mg kg-1) - 0.035*(% Organic C) + 0.43
        if (CLAY[i][k] > 0.f) {
            psp = -0.045f * log(CLAY[i][k]) + (0.001f * solp);
            psp = psp - (0.035f * sol_cbn[i][k]) + 0.43f;
        } else {
            psp = 0.4f;
        }
        // Limit PSP range
        if (psp < .1f) psp = 0.1f;
        if (psp > 0.7f) psp = 0.7f;

        // Calculate smoothed PSP average
        if (psp > 0.f) psp = (psp * 29.f + psp * 1.f) / 30;
        // Store PSP for tomarrows smoothing calculation
        psp = psp;

        //***************Dynamic Active/Soluble Transformation Coeff******************
        // Calculate P balance
        float rto = psp / (1.f - psp);
        float rmn1 = 0.f;
        rmn1 = sol_solp[i][k] - sol_actp[i][k] * rto; // P imbalance
        // Move P between the soluble and active pools based on vadas et al., 2006
        if (rmn1 >= 0.f) // Net movement from soluble to active
        {
            rmn1 = max(rmn1, (-1 * sol_solp[i][k]));
            // Calculate dynamic coefficient
            float vara = 0.918f * (exp(-4.603f * psp));
            float varb = (-0.238f * log(vara)) - 1.126f;
            float arate = 0.f;
            if (a_days[i] > 0) {
                arate = vara * pow(a_days[i], varb);
            } else {
                arate = vara; // ? arate = vara * (1) ** varb; ?
            }
            // limit rate coefficient from 0.05 to .5 helps on day 1 when a_days is zero
            if (arate > 0.5f) arate = 0.5f;
            if (arate < 0.1f) arate = 0.1f;
            rmn1 = arate * rmn1;
            a_days[i] = a_days[i] + 1.f; // add a day to the imbalance counter
            b_days[i] = 0;
        }

        if (rmn1 < 0.f) // Net movement from Active to Soluble
        {
            rmn1 = min(rmn1, sol_actp[i][k]);
            // Calculate dynamic coefficient
            float base = 0.f;
            float varc = 0.f;
            base = (-1.08f * psp) + 0.79f;
            varc = base * (exp(-0.29f));
            // limit varc from 0.1 to 1
            if (varc > 1.f) varc = 1.f;
            if (varc < .1f) varc = .1f;
            rmn1 = rmn1 * varc;
            a_days[i] = 0.f;
            b_days[i] = b_days[i] + 1.f; // add a day to the imbalance counter
        }

        //*************** Active - Stable Transformations ******************
        // Estimate active stable transformation rate coeff
        // original value was .0006
        // based on linear regression rate coeff = 0.005 @ 0% CaCo3 0.05 @ 20% CaCo3
        float as_p_coeff = 0.f;
        float sol_cal = 2.8f;
        as_p_coeff = 0.0023f * sol_cal + 0.005f;
        if (as_p_coeff > 0.05f) as_p_coeff = 0.05f;
        if (as_p_coeff < 0.002f) as_p_coeff = 0.002f;
        // Estimate active/stable pool ratio
        // Generated from sharpley 2003
        float xx = 0.f;
        float ssp = 0.f;
        xx = actp + (actp * rto);
        if (xx > 1.e-6f) ssp = 25.044f * pow(xx, -0.3833f);
        // limit ssp to range in measured data
        if (ssp > 10.f) ssp = 10.f;
        if (ssp < 0.7f) ssp = 0.7f;
        // Smooth ssp, no rapid changes
        if (ssp > 0.f) ssp = (ssp + ssp * 99.f) / 100.f;
        float roc = 0.f;
        roc = ssp * (sol_actp[i][k] + sol_actp[i][k] * rto);
        roc = roc - sol_stap[i][k];
        roc = as_p_coeff * roc;
        // Store todays ssp for tomarrow's calculation
        ssp = ssp;

        // **************** Account for Soil Water content, do not allow movement in dry soil************
        float wetness = 0.f;
        wetness = (solst[i][k] / sol_awc[i][k]); // range from 0-1 1 = field cap
        if (wetness > 1.f) wetness = 1.f;
        if (wetness < 0.25f) wetness = 0.25f;
        rmn1 = rmn1 * wetness;
        roc = roc * wetness;

        // If total P is greater than 10,000 mg/kg do not allow transformations at all
        if ((solp + actp + stap) < 10000.f) {
            // Allow P Transformations
            sol_stap[i][k] = sol_stap[i][k] + roc;
            if (sol_stap[i][k] < 0.f) sol_stap[i][k] = 0.f;
            sol_actp[i][k] = sol_actp[i][k] - roc + rmn1;
            if (sol_actp[i][k] < 0.f) sol_actp[i][k] = 0.f;
            sol_solp[i][k] = sol_solp[i][k] - rmn1;
            if (sol_solp[i][k] < 0.f) sol_solp[i][k] = 0.f;

            // Add water soluble P pool assume 1:5 ratio based on sharpley 2005 et al
            wshd_pas += roc * (1.f / m_nCells);
            wshd_pal += rmn1 * (1.f / m_nCells);
            roctl[i] += roc;
            rmp1tl[i] += rmn1;
        }

        //////////////////////// pminrl.f ///////////////////////////
// 		float bk = .0006f;
//         float rmn1 = 0.f;
//         rmn1 = (sol_solp[i][k] - sol_actp[i][k] * rto);
//         if (rmn1 > 0.f) rmn1 *= 0.1f;
//         if (rmn1 < 0.f)rmn1 *= 0.6f;
//         rmn1 = min(rmn1, sol_solp[i][k]);
//         //amount of phosphorus moving from the active mineral to the stable mineral pool in the soil layer (roc)
//         float roc = 0;
//         //Calculate roc, equation 3:2.3.4 and 3:2.3.5 in SWAT Theory 2009, p215
//         roc = bk * (4.f * sol_actp[i][k] - sol_stap[i][k]);
//         if (roc < 0)roc *= 0.1f;
//         roc = min(roc, sol_actp[i][k]);
//         sol_stap[i][k] = sol_stap[i][k] + roc;
//         if (sol_stap[i][k] < 0)sol_stap[i][k] = 0.f;
//         sol_actp[i][k] = sol_actp[i][k] - roc + rmn1;
//         if (sol_actp[i][k] < 0)sol_actp[i][k] = 0.f;
//         sol_solp[i][k] = sol_solp[i][k] - rmn1;
//         if (sol_solp[i][k] < 0)sol_solp[i][k] = 0.f;
//         wshd_pas += roc * (1.f / m_nCells);
//         wshd_pal += rmn1 * (1.f / m_nCells);
//         roctl[i] += roc;
//         rmp1tl[i] += rmn1;
    }
}

void Nutrient_Transformation::Mineralization_CENTURYModel(int i) {
    /// update tillage related variables if stated. Code from subbasin.f of SWAT, line 153-164
    /// if CENTURY model, and tillage operation has been operated
    if (tillage_days != NULL && tillage_days[i] > 0.f) {
        if (tillage_days[i] >= 30.f) {
            tillage_switch[i] = 0.f;
            tillage_days[i] = 0.f;
        } else {
            tillage_days[i] += 1.f;
        }
    }

    // ABCO2   : allocation from biomass to CO2; 0.6 (surface litter), 0.85?.68*(CLAF + SILF) (all other layers) (Parton et al., 1993, 1994)
    // ABL     : carbon allocation from biomass to leaching; ABL = (1-exp(-f/(0.01* SW+ 0.1*(KdBM)*DB)) (Williams, 1995)
    // ABP     : allocation from biomass to passive humus; 0 (surface litter), 0.003 + 0.032*CLAF (all other layers) (Parton et al., 1993, 1994)
    // ALMCO2  : allocation from metabolic litter to CO2; 0.6 (surface litter), 0.55 (all other layers) (Parton et al., 1993, 1994)
    // ALSLCO2 : allocation from lignin of structural litter to CO2; 0.3 (Parton et al., 1993, 1994)
    // ALSLNCO2: allocation from non-lignin of structural litter to CO2; 0.6 (surface litter), 0.55 (all other layers) (Parton et al., 1993, 1994)
    // APCO2   : allocation from passive humus to CO2; 0.55 (Parton et al., 1993, 1994)
    // ASCO2   : allocation from slow humus to CO2; 0.55 (Parton et al., 1993, 1994)
    // ASP     : allocation from slow humus to passive; 0 (surface litter), 0.003-0.009*CLAF (all other layers) (Parton et al., 1993, 1994)
    // BMC     : mass of C in soil microbial biomass and associated products (kg ha-1)
    // BMCTP   : potential transformation of C in microbial biomass (kg ha-1 day-1)
    // BMN     : mass of N in soil microbial biomass and associated products (kg ha-1)
    // BMNTP   : potential transformation of N in microbial biomass (kg ha-1 day-1)
    // CDG     : soil temperature control on biological processes
    // CNR     : C/N ratio of standing dead
    // CS      : combined factor controlling biological processes [CS = sqrt(CDG�SUT)* 0.8*OX*X1), CS < 10; CS = 10, CS>=10 (Williams, 1995)]
    // DBp     : soil bulk density of plow layer (Mg m-3) (Not used)
    // HSCTP   : potential transformation of C in slow humus (kg ha-1 day-1)
    // HSNTP   : potential transformation of N in slow humus (kg ha-1 day-1)
    // HPCTP   : potential transformation of C in passive humus (kg ha-1 day-1)
    // HPNTP   : potential transformation of N in passive humus (kg ha-1 day-1)
    // LMF     : fraction of the litter that is metabolic
    // LMNF    : fraction of metabolic litter that is N (kg kg-1)
    // LMCTP   : potential transformation of C in metabolic litter (kg ha-1 day-1)
    // LMNTP   : potential transformation of N in metabolic litter (kg ha-1 day-1)
    // LSCTP   : potential transformation of C in structural litter (kg ha-1 day-1)
    // LSF     : fraction of the litter that is structural
    // LSLF    : fraction of structural litter that is lignin (kg kg-1)
    // LSNF    : fraction of structural litter that is N (kg kg-1)
    // LSLCTP  : potential transformation of C in lignin of structural litter (kg ha-1 day-1)
    // LSLNCTP : potential transformation of C in nonlignin structural litter (kg ha-1 day-1)
    // LSNTP   : potential transformation of N in structural litter (kg ha-1 day-1)
    // NCBM    : N/C ratio of biomass
    // NCHP    : N/C ratio passive humus
    // NCHS    : N/C ratio of the slow humus
    // OX      : oxygen control on biological processes with soil depth
    // SUT     : soil water control on biological processes
    // X1      : tillage control on residue decomposition (Not used)
    // XBMT    : control on transformation of microbial biomass by soil texture and structure.
    //           Its values: surface litter layer = 1; all other layers = 1-0.75*(SILF + CLAF) (Parton et al., 1993, 1994)
    // XLSLF   : control on potential transformation of structural litter by lignin fraction of structural litter [XLSLF = exp(-3* LSLF) (Parton et al., 1993, 1994)]
    float x1 = 0.f, xx = 0.f;
    float LSR = 0.f, BMR = 0.f, HSR = 0.f, HPR = 0.f, RLR = 0.f;
    float LSCTA = 0.f, LSLCTA = 0.f, LSLNCTA = 0.f, LSNTA = 0.f, XBM = .0f;
    float LMCTA = 0.f, LMNTA = 0.f, BMCTA = 0.f, BMNTA = 0.f, HSCTA = 0.f, HSNTA = 0.f, HPCTA = 0.f, HPNTA = 0.f;
    float LSCTP = 0.f, LSLCTP = 0.f, LSLNCTP, LSNTP = 0.f, LMR = 0.f, LMCTP = 0.f;
    float LMNTP = 0.f, BMCTP = 0.f, BMNTP = 0.f, HSCTP = 0.f, HSNTP = 0.f, HPCTP = 0.f, HPNTP = 0.f;
    float NCHP = 0.f, NCBM = 0.f, NCHS = 0.f;
    float ABCO2 = 0.f, A1CO2 = 0.f, APCO2 = 0.f, ASCO2 = 0.f, ABP = 0.f, ASP = 0.f, A1 = 0.f, ASX = 0.f, APX = 0.f;
    float PRMT_51 = 0.f, PRMT_45 = 0.f;
    float DF1 = 0.f, DF2 = 0.f, DF3 = 0.f, DF4 = 0.f, DF5 = 0.f, DF6 = 0.f;
    float ADD = 0.f, ADF1 = 0.f, ADF2 = 0.f, ADF3 = 0.f, ADF4 = 0.f, ADF5 = 0.f;
    float TOT = 0.f, PN1 = 0.f, PN2 = 0.f, PN3 = 0.f, PN5 = 0.f, PN6 = 0.f, PN7 = 0.f, PN8 = 0.f, PN9 = 0.f;
    float SUM = 0.f, CPN1 = 0.f, CPN2 = 0.f, CPN3 = 0.f, CPN4 = 0.f, CPN5 = 0.f, RTO = 0.f;
    float wdn = 0.f;
    // SNMN = 0.f, PN4 = 0.f, ALMCO2 = 0.f, ALSLCO2 = 0.f, ALSLNCO2 = 0.f, LSLNCAT = 0.f . NOT used in this module
    // XBMT = 0.f, XLSLF = 0.f, LSLF = 0.f, LSF = 0.f, LMF = 0.f, x3 = 0.f
    /// calculate tillage factor using DSSAT
    if (tillage_switch[i] == 1 && tillage_days[i] <= 30.f) {
        tillage_factor[i] = 1.6f;
    } else {
        tillage_factor[i] = 1.f;
    }

    /// calculate C/N dynamics for each soil layer
    for (int k = 0; k < (int) soillayers[i]; k++) {
        float sol_mass = 0.f;
        sol_mass = soilthick[i][k] / 1000.f * 10000.f * density[i][k] * 1000.f * (1 - rock[i][k] / 100.f);
        //if (k == 0)
        //{
        //	//10 cm / 1000 = 0.01m; 1 ha = 10000 m2; ton/m3; * 1000 --> final unit is kg/ha; rock fraction is considered
        //	sol_mass = 10.f / 1000.f * 10000.f * density[i][k]* 1000. *(1- rock[i][k] / 100.f);
        //} else
        //	sol_mass = (soilDepth[i][k] - soilDepth[i][k - 1]) / 1000.f * 10000.f * density[i][k]* 1000.f *(1- rock[i][k] / 100.f);

        //If k = 1, using temperature, soil moisture in layer 2 to calculate decomposition factor
        int kk = 0;
        if (k == 0) { kk = 1; }
        else { kk = k; }
        // mineralization can occur only if temp above 0 deg
        //check sol_st soil water content in each soil ayer mm H2O
        if (SOTE[i] > 0.f && solst[i][k] > 0.f) {
            //from Armen
            //compute soil water factor - sut
            // float fc = sol_awc[i][k] + sol_wpmm[i][k];            // units mm
            float wc = solst[i][k] + sol_wpmm[i][k];        // units mm
            float sat = sol_ul[i][k] + sol_wpmm[i][k];        // units mm
            float voidfr = Porosity[i][k] * (1.f - wc / sat);        // fraction

            float sut = 0.f;
            x1 = wc - sol_wpmm[i][k];
            if (x1 < 0.f) { sut = .1f * pow(solst[i][kk] / sol_wpmm[i][k], 2.f); }
            else { sut = .1f + .9f * sqrt(solst[i][kk] / sol_awc[i][k]); }
            sut = min(1.f, sut);
            sut = max(.05f, sut);

            //compute tillage factor (x1)
            x1 = 1.f;
            // calculate tillage factor using DSSAT
            if (tillage_switch[i] == 1 && tillage_days[i] <= 30.f) {
                if (k == 1) { x1 = 1.6f; }
                else {
                    if (soilDepth[i][k] >= tillage_depth[i]) {
                        x1 = 1.6f;
                    } else if (soilDepth[i][k - 1] > tillage_depth[i]) {
                        x1 = 1.f + 0.6f * (tillage_depth[i] - soilDepth[i][k - 1]) / soilthick[i][k];
                    }
                }
            } else {
                x1 = 1.f;
            }
            //compute soil temperature factor
            float cdg = 0.f;
            /// cdg = SOTE[i] / (SOTE[i] + exp(5.058459f - 0.2503591f * SOTE[i]));
            /* cdg is calculated by function fcgd in carbon_new.f of SWAT, by Armen
             * i.e., Function fcgd(xx)
             */
            cdg = pow((SOTE[i]) + 5.f, 8.f / 3.f) * (50.f - SOTE[i]) / (pow(40.f, 8.f / 3.f) * 15.f);
            if (cdg < 0.f) cdg = 0.f;

            //compute oxygen (OX)
            float ox = 0.f;
            // ox = 1 - (0.9* sol_z[i][k]/1000.) / (sol_z[i][k]/1000.+ exp(1.50-3.99*sol_z[i][k]/1000.))
            // ox = 1 - (0.8* sol_z[i][k]) / (sol_z[i][k]+ exp(1.50-3.99*sol_z[i][k]))
            ox = 1.f -
                0.8f * ((soilDepth[i][kk] + soilDepth[i][kk - 1]) / 2.f) / (((soilDepth[i][kk] + soilDepth[i][kk - 1]) / 2.f) +
                    exp(18.40961f - 0.023683632f *
                        ((soilDepth[i][kk] +
                            soilDepth[i][kk - 1]) / 2.f)));
            // compute combined factor
            float cs = 0.f;
            cs = min(10.f, sqrt(cdg * sut) * 0.9f * ox * x1);
            // call denitrification (to use void fraction and cdg factor)
            // repetitive computation, commented by LJ
            //cdg = pow((SOTE[i]) + 5.f, 8.f / 3.f) * (50.f - SOTE[i]) / (pow(40.f, 8.f / 3.f) * 15.f);
            if (cdg > 0.f && voidfr <= 0.1f) {
                // call ndenit(k,j,cdg,wdn,void);
                // rewrite from ndenit.f of SWAT
                float vof = 1.f / (1.f + pow(voidfr / 0.04f, 5.f));
                wdn = sol_no3[i][k] * (1.f - exp(-cdn * cdg * vof * sol_cbn[i][k]));
                sol_no3[i][k] -= wdn;
            }
            wshd_dnit += wdn * (1.f / m_nCells);
            wdntl[i] += wdn;

            float sol_min_n = sol_no3[i][k] + sol_nh4[i][k];
            //if(i == 100 && k == 0) cout << sol_LSL[i][k] << sol_LS[i][k] << endl;
            // lignin content in structural litter (fraction)
            RLR = min(0.8f, sol_LSL[i][k] / (sol_LS[i][k] + 1.e-5f));
            // HSR=PRMT(47) !CENTURY SLOW HUMUS TRANSFORMATION RATE D^-1(0.00041_0.00068) ORIGINAL VALUE = 0.000548,
            HSR = 5.4799998e-4f;
            // HPR=PRMT(48) !CENTURY PASSIVE HUMUS TRANSFORMATION RATE D^-1(0.0000082_0.000015) ORIGINAL VALUE = 0.000012
            HPR = 1.2e-5f;
            APCO2 = .55f;
            ASCO2 = .60f;
            PRMT_51 = 0.f;   // COEF ADJUSTS MICROBIAL ACTIVITY FUNCTION IN TOP SOIL LAYER (0.1_1.),
            PRMT_51 = 1.f;
            //The following codes are calculating of the N:C ration in the newly formed SOM for each pool
            //please note that in the surface layer, no new materials enter Passive pool, therefore, no NCHP is
            //calculated for the first layer.
            if (k == 0) {
                cs = cs * PRMT_51;
                ABCO2 = .55f;
                A1CO2 = .55f;
                BMR = .0164f;
                LMR = .0405f;
                LSR = .0107f;
                NCHP = .1f;
                XBM = 1.f;
                // COMPUTE N/C RATIOS
                // relative nitrogen content in residue (%)
                x1 = 0.1f * (sol_LSN[i][k] + sol_LMN[i][k]) / (sol_rsd[i][k] / 1000.f + 1.e-5f);
                if (x1 > 2.f) {
                    NCBM = .1f;
                } else if (x1 > .01f) {
                    NCBM = 1.f / (20.05f - 5.0251f * x1);
                } else {
                    NCBM = .05f;
                }
                NCHS = NCBM / (5.f * NCBM + 1.f);
            } else {
                ABCO2 = 0.17f + 0.0068f * sand[i][k];
                A1CO2 = .55f;
                BMR = .02f;
                LMR = .0507f;
                LSR = .0132f;
                XBM = .25f + .0075f * sand[i][k];

                x1 = 1000.f * sol_min_n / (sol_mass / 1000.f);
                if (x1 > 7.15f) {
                    NCBM = .33f;
                    NCHS = .083f;
                    NCHP = .143f;
                } else {
                    NCBM = 1.f / (15.f - 1.678f * x1);
                    NCHS = 1.f / (20.f - 1.119f * x1);
                    NCHP = 1.f / (10.f - .42f * x1);
                }
            }
            ABP = .003f + .00032f * CLAY[i][k];

            PRMT_45 = 0.f;  // COEF IN CENTURY EQ ALLOCATING SLOW TO PASSIVE HUMUS(0.001_0.05) ORIGINAL VALUE = 0.003,
            PRMT_45 = 5.0000001e-2f;
            ASP = max(.001f, PRMT_45 - .00009f * CLAY[i][k]);
            // POTENTIAL TRANSFORMATIONS STRUCTURAL LITTER
            x1 = LSR * cs * exp(-3.f * RLR);
            LSCTP = x1 * sol_LSC[i][k];
            LSLCTP = LSCTP * RLR;
            LSLNCTP = LSCTP * (1.f - RLR);
            LSNTP = x1 * sol_LSN[i][k];
            // POTENTIAL TRANSFORMATIONS METABOLIC LITTER
            x1 = LMR * cs;
            LMCTP = sol_LMC[i][k] * x1;
            LMNTP = sol_LMN[i][k] * x1;
            // POTENTIAL TRANSFORMATIONS MICROBIAL BIOMASS
            x1 = BMR * cs * XBM;
            BMCTP = sol_BMC[i][k] * x1;
            BMNTP = sol_BMN[i][k] * x1;
            // POTENTIAL TRANSFORMATIONS SLOW HUMUS
            x1 = HSR * cs;
            HSCTP = sol_HSC[i][k] * x1;
            HSNTP = sol_HSN[i][k] * x1;
            // POTENTIAL TRANSFORMATIONS PASSIVE HUMUS
            x1 = cs * HPR;
            HPCTP = sol_HPC[i][k] * x1;
            HPNTP = sol_HPN[i][k] * x1;
            // ESTIMATE N DEMAND
            A1 = 1.f - A1CO2;
            ASX = 1.f - ASCO2 - ASP;
            APX = 1.f - APCO2;

            PN1 = LSLNCTP * A1 * NCBM;               // Structural Litter to Biomass
            PN2 = .7f * LSLCTP * NCHS;               // Structural Litter to Slow
            PN3 = LMCTP * A1 * NCBM;                 // Metabolic Litter to Biomass
            // PN4 = BMCTP*ABL*NCBM;                // Biomass to Leaching (calculated in NCsed_leach)
            PN5 = BMCTP * ABP * NCHP;                // Biomass to Passive
            PN6 = BMCTP * (1.f - ABP - ABCO2) * NCHS;     // Biomass to Slow
            PN7 = HSCTP * ASX * NCBM;               // Slow to Biomass
            PN8 = HSCTP * ASP * NCHP;                // Slow to Passive
            PN9 = HPCTP * APX * NCBM;                // Passive to Biomass

            // COMPARE SUPPLY AND DEMAND FOR N
            SUM = 0.f;
            x1 = PN1 + PN2;
            if (LSNTP < x1) { CPN1 = x1 - LSNTP; }
            else { SUM = SUM + LSNTP - x1; }
            if (LMNTP < PN3) { CPN2 = PN3 - LMNTP; }
            else { SUM = SUM + LMNTP - PN3; }
            x1 = PN5 + PN6;
            if (BMNTP < x1) { CPN3 = x1 - BMNTP; }
            else { SUM = SUM + BMNTP - x1; }
            x1 = PN7 + PN8;
            if (HSNTP < x1) { CPN4 = x1 - HSNTP; }
            else { SUM = SUM + HSNTP - x1; }
            if (HPNTP < PN9) { CPN5 = PN9 - HPNTP; }
            else { SUM = SUM + HPNTP - PN9; }

            // total available N
            float Wmin = max(1.e-5f, sol_no3[i][k] + sol_nh4[i][k] + SUM);
            // total demand for potential tranformaiton of SOM;
            float DMDN = CPN1 + CPN2 + CPN3 + CPN4 + CPN5;
            float x3 = 1.f;
            // REDUCE DEMAND if SUPPLY LIMITS
            if (Wmin < DMDN) x3 = Wmin / DMDN;

            // ACTUAL TRANSFORMATIONS

            if (CPN1 > 0.f) {
                LSCTA = LSCTP * x3;
                LSNTA = LSNTP * x3;
                LSLCTA = LSLCTP * x3;
                LSLNCTA = LSLNCTP * x3;
            } else {
                LSCTA = LSCTP;
                LSNTA = LSNTP;
                LSLCTA = LSLCTP;
                //LSLNCAT = LSLNCTP;
            }
            if (CPN2 > 0.f) {
                LMCTA = LMCTP * x3;
                LMNTA = LMNTP * x3;
            } else {
                LMCTA = LMCTP;
                LMNTA = LMNTP;
            }
            if (CPN3 > 0.f) {
                BMCTA = BMCTP * x3;
                BMNTA = BMNTP * x3;
            } else {
                BMCTA = BMCTP;
                BMNTA = BMNTP;
            }
            if (CPN4 > 0.f) {
                HSCTA = HSCTP * x3;
                HSNTA = HSNTP * x3;
            } else {
                HSCTA = HSCTP;
                HSNTA = HSNTP;
            }
            if (CPN5 > 0.f) {
                HPCTA = HPCTP * x3;
                HPNTA = HPNTP * x3;
            } else {
                HPCTA = HPCTP;
                HPNTA = HPNTP;
            }

            // Recalculate demand using actual transformations revised from EPIC code by Zhang
            PN1 = LSLNCTA * A1 * NCBM;               // Structural Litter to Biomass
            PN2 = .7f * LSLCTA * NCHS;               // Structural Litter to Slow
            PN3 = LMCTA * A1 * NCBM;                 // Metabolic Litter to Biomass
            //PN4 = BMCTP * ABL * NCBM;                // Biomass to Leaching (calculated in NCsed_leach)
            PN5 = BMCTA * ABP * NCHP;                // Biomass to Passive
            PN6 = BMCTA * (1.f - ABP - ABCO2) * NCHS;     // Biomass to Slow
            PN7 = HSCTA * ASX * NCBM;                // Slow to Biomass
            PN8 = HSCTA * ASP * NCHP;                // Slow to Passive
            PN9 = HPCTA * APX * NCBM;                // Passive to Biomass
            // COMPARE SUPPLY AND DEMAND FOR N
            SUM = 0.f;
            CPN1 = 0.f;
            CPN2 = 0.f;
            CPN3 = 0.f;
            CPN4 = 0.f;
            CPN5 = 0.f;
            x1 = PN1 + PN2;
            if (LSNTA < x1) { CPN1 = x1 - LSNTA; }
            else { SUM = SUM + LSNTA - x1; }
            if (LMNTA < PN3) { CPN2 = PN3 - LMNTA; }
            else { SUM = SUM + LMNTA - PN3; }
            x1 = PN5 + PN6;
            if (BMNTA < x1) { CPN3 = x1 - BMNTA; }
            else { SUM = SUM + BMNTA - x1; }
            x1 = PN7 + PN8;
            if (HSNTA < x1) { CPN4 = x1 - HSNTA; }
            else { SUM = SUM + HSNTA - x1; }
            if (HPNTA < PN9) { CPN5 = PN9 - HPNTA; }
            else { SUM = SUM + HPNTA - PN9; }
            // total available N
            Wmin = max(1.e-5f, sol_no3[i][k] + sol_nh4[i][k] + SUM);
            // total demand for potential transformation of SOM
            DMDN = CPN1 + CPN2 + CPN3 + CPN4 + CPN5;

            // supply - demand
            sol_RNMN[i][k] = SUM - DMDN;
            // UPDATE
            if (sol_RNMN[i][k] > 0.f) {
                sol_nh4[i][k] += sol_RNMN[i][k];
            } else {
                x1 = sol_no3[i][k] + sol_RNMN[i][k];
                if (x1 < 0.f) {
                    sol_RNMN[i][k] = -sol_no3[i][k];
                    sol_no3[i][k] = 1.e-6f;
                } else {
                    sol_no3[i][k] = x1;
                }
            }

            DF1 = LSNTA;
            DF2 = LMNTA;
            // DF represents Demand from
            // SNMN=SNMN+sol_RNMN[i][k]

            // calculate P flows
            // compute humus mineralization on active organic p
            float hmp = 0.f;
            float hmp_rate = 0.f;
            hmp_rate = 1.4f * (HSNTA + HPNTA) / (sol_HSN[i][k] + sol_HPN[i][k] + 1.e-6f);
            // hmp_rate = 1.4f * (HSNTA ) / (sol_HSN[i][k] + sol_HPN[i][k] + 1.e-6f);
            hmp = hmp_rate * sol_orgp[i][k];
            hmp = min(hmp, sol_orgp[i][k]);
            sol_orgp[i][k] = sol_orgp[i][k] - hmp;
            sol_solp[i][k] = sol_solp[i][k] + hmp;

            // compute residue decomposition and mineralization of
            // fresh organic n and p (upper two layers only)
            float rmp = 0.f;
            float decr = 0.f;
            decr = (LSCTA + LMCTA) / (sol_LSC[i][k] + sol_LMC[i][k] + 1.e-6f);
            decr = min(1.f, decr);
            rmp = decr * sol_fop[i][k];

            sol_fop[i][k] = sol_fop[i][k] - rmp;
            sol_solp[i][k] = sol_solp[i][k] + .8f * rmp;
            sol_orgp[i][k] = sol_orgp[i][k] + .2f * rmp;
            // end of calculate P flows

            LSCTA = min(sol_LSC[i][k], LSCTA);
            sol_LSC[i][k] = max(1.e-10f, sol_LSC[i][k] - LSCTA);
            LSLCTA = min(sol_LSLC[i][k], LSLCTA);
            sol_LSLC[i][k] = max(1.e-10f, sol_LSLC[i][k] - LSLCTA);
            sol_LSLNC[i][k] = max(1.e-10f, sol_LSLNC[i][k] - LSLNCTA);
            LMCTA = min(sol_LMC[i][k], LMCTA);
            if (sol_LM[i][k] > 0.f) {
                RTO = max(0.42f, sol_LMC[i][k] / sol_LM[i][k]);
                sol_LM[i][k] = sol_LM[i][k] - LMCTA / RTO;
                sol_LMC[i][k] = sol_LMC[i][k] - LMCTA;
            }
            sol_LSL[i][k] = max(1.e-10f, sol_LSL[i][k] - LSLCTA / .42f);
            sol_LS[i][k] = max(1.e-10f, sol_LS[i][k] - LSCTA / .42f);

            x3 = APX * HPCTA + ASX * HSCTA + A1 * (LMCTA + LSLNCTA);
            sol_BMC[i][k] = sol_BMC[i][k] - BMCTA + x3;
            DF3 = BMNTA - NCBM * x3;
            // DF3 is the supply of BMNTA - demand of N to meet the Passive, Slow, Metabolic, and Non-lignin Structural
            // C pools transformations into microbiomass pool
            x1 = .7f * LSLCTA + BMCTA * (1.f - ABP - ABCO2);
            sol_HSC[i][k] = sol_HSC[i][k] - HSCTA + x1;
            DF4 = HSNTA - NCHS * x1;
            // DF4 Slow pool supply of N - N demand for microbiomass C transformed into slow pool
            x1 = HSCTA * ASP + BMCTA * ABP;
            sol_HPC[i][k] = sol_HPC[i][k] - HPCTA + x1;
            DF5 = HPNTA - NCHP * x1;
            // DF5 Passive pool demand of N - N demand for microbiomass C transformed into passive pool
            DF6 = sol_min_n - sol_no3[i][k] - sol_nh4[i][k];
            // DF6 Supply of mineral N - available mineral N = N demanded from mineral pool

            ADD = DF1 + DF2 + DF3 + DF4 + DF5 + DF6;
            ADF1 = abs(DF1);
            ADF2 = abs(DF2);
            ADF3 = abs(DF3);
            ADF4 = abs(DF4);
            ADF5 = abs(DF5);
            TOT = ADF1 + ADF2 + ADF3 + ADF4 + ADF5;
            xx = ADD / (TOT + 1.e-10f);
            sol_LSN[i][k] = max(.001f, sol_LSN[i][k] - DF1 + xx * ADF1);
            sol_LMN[i][k] = max(.001f, sol_LMN[i][k] - DF2 + xx * ADF2);
            sol_BMN[i][k] = sol_BMN[i][k] - DF3 + xx * ADF3;
            sol_HSN[i][k] = sol_HSN[i][k] - DF4 + xx * ADF4;
            sol_HPN[i][k] = sol_HPN[i][k] - DF5 + xx * ADF5;
            sol_RSPC[i][k] = .3f * LSLCTA + A1CO2 * (LSLNCTA + LMCTA) +
                ABCO2 * BMCTA + ASCO2 * HSCTA + APCO2 * HPCTA;

            sol_rsd[i][k] = sol_LS[i][k] + sol_LM[i][k];
            sol_orgn[i][k] = sol_HPN[i][k];
            sol_aorgn[i][k] = sol_HSN[i][k];
            sol_fon[i][k] = sol_LMN[i][k] + sol_LSN[i][k];
            sol_cbn[i][k] = 100.f * (sol_LSC[i][k] + sol_LMC[i][k] + sol_HSC[i][k] +
                sol_HPC[i][k] + sol_BMC[i][k]) / sol_mass;

            // summary calculations
            float hmn = 0.f;
            float cell_dafr = 1.f / m_nCells;
            hmn = sol_RNMN[i][k];
            wshd_hmn = wshd_hmn + hmn * cell_dafr;
            float rwn = 0.f;
            rwn = HSNTA;
            wshd_rwn = wshd_rwn + rwn * cell_dafr;

            wshd_hmp = wshd_hmp + hmp * cell_dafr;
            float rmn1 = 0.f;
            rmn1 = (LSNTA + LMNTA);
            wshd_rmn = wshd_rmn + rmn1 * cell_dafr;
            wshd_rmp = wshd_rmp + rmp * cell_dafr;
            wshd_dnit = wshd_dnit + wdn * cell_dafr;
            hmntl[i] = hmntl[i] + hmn;
            rwntl[i] = rwntl[i] + rwn;
            hmptl[i] = hmptl[i] + hmp;
            rmn2tl[i] = rmn2tl[i] + rmn1;
            rmptl[i] = rmptl[i] + rmp;
            wdntl[i] = wdntl[i] + wdn;
        }
    }
}

void Nutrient_Transformation::GetValue(const char *key, float *value) {
    string sk(key);
    if (StringMatch(sk, VAR_WSHD_DNIT)) { *value = this->wshd_dnit; }
    else if (StringMatch(sk, VAR_WSHD_HMN)) { *value = this->wshd_hmn; }
    else if (StringMatch(sk, VAR_WSHD_HMP)) { *value = this->wshd_hmp; }
    else if (StringMatch(sk, VAR_WSHD_RMN)) { *value = this->wshd_rmn; }
    else if (StringMatch(sk, VAR_WSHD_RMP)) { *value = this->wshd_rmp; }
    else if (StringMatch(sk, VAR_WSHD_RWN)) { *value = this->wshd_rwn; }
    else if (StringMatch(sk, VAR_WSHD_NITN)) { *value = this->wshd_nitn; }
    else if (StringMatch(sk, VAR_WSHD_VOLN)) { *value = this->wshd_voln; }
    else if (StringMatch(sk, VAR_WSHD_PAL)) { *value = this->wshd_pal; }
    else if (StringMatch(sk, VAR_WSHD_PAS)) { *value = this->wshd_pas; }
    else {
        throw ModelException(MID_NUTR_TF, "GetValue", "Parameter " + sk + " does not exist.");
    }
}

void Nutrient_Transformation::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_HMNTL)) { *data = this->hmntl; }
    else if (StringMatch(sk, VAR_HMPTL)) { *data = this->hmptl; }
    else if (StringMatch(sk, VAR_RMN2TL)) { *data = this->rmn2tl; }
    else if (StringMatch(sk, VAR_RMPTL)) { *data = this->rmptl; }
    else if (StringMatch(sk, VAR_RWNTL)) { *data = this->rwntl; }
    else if (StringMatch(sk, VAR_WDNTL)) { *data = this->wdntl; }
    else if (StringMatch(sk, VAR_RMP1TL)) { *data = this->rmp1tl; }
    else if (StringMatch(sk, VAR_ROCTL)) { *data = this->roctl; }
    else if (StringMatch(sk, VAR_SOL_COV)) { *data = this->sol_cov; }
    else if (StringMatch(sk, VAR_A_DAYS)) { *data = this->a_days; }
    else if (StringMatch(sk, VAR_B_DAYS)) { *data = this->b_days; }
    else {
        throw ModelException(MID_NUTR_TF, "Get1DData", "Parameter " + sk + " does not exist.");
    }
    *n = this->m_nCells;
}

void Nutrient_Transformation::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = soilLayers;
    if (StringMatch(sk, VAR_SOL_AORGN)) { *data = this->sol_aorgn; }
    else if (StringMatch(sk, VAR_SOL_FORGN)) { *data = this->sol_fon; }
    else if (StringMatch(sk, VAR_SOL_FORGP)) { *data = this->sol_fop; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { *data = this->sol_no3; }
    else if (StringMatch(sk, VAR_SOL_NH4)) { *data = this->sol_nh4; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { *data = this->sol_orgn; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { *data = this->sol_orgp; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { *data = this->sol_solp; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { *data = this->sol_actp; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { *data = this->sol_stap; }
    else if (StringMatch(sk, VAR_SOL_RSD)) { *data = this->sol_rsd; }
        /// 2-CENTURY C/N cycling related variables
    else if (StringMatch(sk, VAR_SOL_WOC)) { *data = this->sol_WOC; }
    else if (StringMatch(sk, VAR_SOL_WON)) { *data = this->sol_WON; }
    else if (StringMatch(sk, VAR_SOL_BM)) { *data = this->sol_BM; }
    else if (StringMatch(sk, VAR_SOL_BMC)) { *data = this->sol_BMC; }
    else if (StringMatch(sk, VAR_SOL_BMN)) { *data = this->sol_BMN; }
    else if (StringMatch(sk, VAR_SOL_HP)) { *data = this->sol_HP; }
    else if (StringMatch(sk, VAR_SOL_HS)) { *data = this->sol_HS; }
    else if (StringMatch(sk, VAR_SOL_HSC)) { *data = this->sol_HSC; }
    else if (StringMatch(sk, VAR_SOL_HSN)) { *data = this->sol_HSN; }
    else if (StringMatch(sk, VAR_SOL_HPC)) { *data = this->sol_HPC; }
    else if (StringMatch(sk, VAR_SOL_HPN)) { *data = this->sol_HPN; }
    else if (StringMatch(sk, VAR_SOL_LM)) { *data = this->sol_LM; }
    else if (StringMatch(sk, VAR_SOL_LMC)) { *data = this->sol_LMC; }
    else if (StringMatch(sk, VAR_SOL_LMN)) { *data = this->sol_LMN; }
    else if (StringMatch(sk, VAR_SOL_LSC)) { *data = this->sol_LSC; }
    else if (StringMatch(sk, VAR_SOL_LSN)) { *data = this->sol_LSN; }
    else if (StringMatch(sk, VAR_SOL_LS)) { *data = this->sol_LS; }
    else if (StringMatch(sk, VAR_SOL_LSL)) { *data = this->sol_LSL; }
    else if (StringMatch(sk, VAR_SOL_LSLC)) { *data = this->sol_LSLC; }
    else if (StringMatch(sk, VAR_SOL_LSLNC)) { *data = this->sol_LSLNC; }
    else if (StringMatch(sk, VAR_SOL_RNMN)) { *data = this->sol_RNMN; }
    else if (StringMatch(sk, VAR_SOL_RSPC)) { *data = this->sol_RSPC; }
    else if (StringMatch(sk, VAR_CONV_WT)) { *data = this->conv_wt; }
    else {
        throw ModelException(MID_NUTR_TF, "Get2DData", "Parameter " + sk + " does not exist.");
    }
}
