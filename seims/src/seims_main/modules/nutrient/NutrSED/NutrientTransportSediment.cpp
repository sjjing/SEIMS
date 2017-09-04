#include "seims.h"
#include "NutrientTransportSediment.h"

using namespace std;

NutrientTransportSediment::NutrientTransportSediment(void) :
//input
    m_nCells(-1), CELLWIDTH(-1.f), cellArea(-1.f), nSoiLayers(-1), soillayers(NULL),
    nSubbasins(-1), subbasin(NULL), m_subbasinsInfo(NULL),
    density(NULL), soilthick(NULL), enratio(NULL),
    sol_actp(NULL), sol_stap(NULL), sol_fop(NULL), sol_orgp(NULL),
    sol_orgn(NULL), sol_aorgn(NULL), sol_fon(NULL),
    SED_OL(NULL), OL_Flow(NULL),
    /// for CENTURY C/N cycling model inputs
    cswat(0), sol_LSN(NULL), sol_LMN(NULL), sol_HPN(NULL), sol_HSN(NULL),
    sol_HPC(NULL), sol_HSC(NULL), sol_LMC(NULL), sol_LSC(NULL), sol_LS(NULL),
    sol_LM(NULL), sol_LSL(NULL), sol_LSLC(NULL), sol_LSLNC(NULL), sol_BMC(NULL),
    sol_WOC(NULL), Perco(NULL), SSRU(NULL),
    /// for C-FARM one carbon model input
    sol_mp(NULL),
    /// for CENTURY C/N cycling model outputs
    sol_latc(NULL), sol_percoc(NULL), latc(NULL), percoc(NULL), sedc(NULL),
    //outputs
    sedorgn(NULL), sedorgp(NULL), sedminpa(NULL), sedminps(NULL),
    sedorgnToCh(NULL), sedorgpToCh(NULL), sedminpaToCh(NULL), sedminpsToCh(NULL) {
}

NutrientTransportSediment::~NutrientTransportSediment(void) {
    if (enratio != NULL) Release1DArray(enratio);

    if (sedorgp != NULL) Release1DArray(sedorgp);
    if (sedorgn != NULL) Release1DArray(sedorgn);
    if (sedminpa != NULL) Release1DArray(sedminpa);
    if (sedminps != NULL) Release1DArray(sedminps);

    if (sedorgnToCh != NULL) Release1DArray(sedorgnToCh);
    if (sedorgpToCh != NULL) Release1DArray(sedorgpToCh);
    if (sedminpaToCh != NULL) Release1DArray(sedminpaToCh);
    if (sedminpsToCh != NULL) Release1DArray(sedminpsToCh);

    /// for CENTURY C/N cycling model outputs
    if (sol_latc != NULL) Release2DArray(m_nCells, sol_percoc);
    if (sol_percoc != NULL) Release2DArray(m_nCells, sol_percoc);
    if (latc != NULL) Release1DArray(latc);
    if (percoc != NULL) Release1DArray(percoc);
    if (sedc != NULL) Release1DArray(sedc);
}

bool NutrientTransportSediment::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRSED, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.\n");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_NUTRSED, "CheckInputSize",
                                 "Input data for " + string(key) + " is invalid with size: " + ValueToString(n) +
                                     ". The origin size is " + ValueToString(m_nCells) + ".\n");
        }
    }
    return true;
}

bool NutrientTransportSediment::CheckInputData() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The input data can not be less than zero.");
    }
    if (this->CELLWIDTH < 0.f) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (this->nSoiLayers <= 0) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (this->soillayers == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "Soil layers number must not be NULL.");
    }
    if (this->SED_OL == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The distribution of soil loss caused by water erosion can not be NULL.");
    }
    if (this->OL_Flow == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The distribution of surface runoff generated data can not be NULL.");
    }
    if (this->density == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The bulk density of the soil data can not be NULL.");
    }
    if (this->soilthick == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The thickness of soil layer can not be NULL.");
    }
    if (this->sol_actp == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of phosphorus stored in the active mineral phosphorus pool can not be NULL.");
    }
    if (this->sol_orgn == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of nitrogen stored in the stable organic N pool can not be NULL.");
    }
    if (this->sol_orgp == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of phosphorus stored in the organic P pool can not be NULL.");
    }
    if (this->sol_stap == NULL) {
        throw ModelException(MID_NUTRSED,
                             "CheckInputData",
                             "The amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool can not be NULL.");
    }
    if (this->sol_aorgn == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of nitrogen stored in the active organic nitrogen pool data can not be NULL.");
    }
    if (this->sol_fon == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of nitrogen stored in the fresh organic pool can not be NULL.");
    }
    if (this->sol_fop == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The amount of phosphorus stored in the fresh organic pool can not be NULL.");
    }

    if (subbasin == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The parameter: subbasin has not been set.");
    }

    if (nSubbasins <= 0) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    }
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");
    }
    return true;
}

bool NutrientTransportSediment::CheckInputData_CENTURY() {
    if (this->sol_LSN == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LSN can not be NULL.");
    if (this->sol_LMN == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LMN can not be NULL.");
    if (this->sol_HPN == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_HPN can not be NULL.");
    if (this->sol_HSN == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_HSN can not be NULL.");
    if (this->sol_HPC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_HPC can not be NULL.");
    if (this->sol_HSC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_HSC can not be NULL.");
    if (this->sol_LMC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LMC can not be NULL.");
    if (this->sol_LSC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LSC can not be NULL.");
    if (this->sol_LS == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LS can not be NULL.");
    if (this->sol_LM == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LM can not be NULL.");
    if (this->sol_LSL == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LSL can not be NULL.");
    if (this->sol_LSLC == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LSLC can not be NULL.");
    }
    if (this->sol_LSLNC == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_LSLNC can not be NULL.");
    }
    if (this->sol_BMC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_BMC can not be NULL.");
    if (this->sol_WOC == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_WOC can not be NULL.");
    if (this->Perco == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The Perco can not be NULL.");
    }
    if (this->SSRU == NULL) {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The SSRU can not be NULL.");
    }
    return true;
}

bool NutrientTransportSediment::CheckInputData_CFARM() {
    if (this->sol_mp == NULL) throw ModelException(MID_NUTRSED, "CheckInputData", "The sol_mp can not be NULL.");
    return true;
}

void NutrientTransportSediment::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, Tag_CellWidth)) { CELLWIDTH = value; }
    else if (StringMatch(sk, VAR_CSWAT)) { cswat = (int) value; }
    else {
        throw ModelException(MID_NUTRSED, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrientTransportSediment::Set1DData(const char *key, int n, float *data) {
    if (!this->CheckInputSize(key, n)) return;

    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN)) {
        subbasin = data;
    } else if (StringMatch(sk, VAR_SOILLAYERS)) {
        soillayers = data;
    } else if (StringMatch(sk, VAR_SEDYLD)) {
        this->SED_OL = data;
    } else if (StringMatch(sk, VAR_OLFLOW)) {
        this->OL_Flow = data;
    } else {
        throw ModelException(MID_NUTRSED, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientTransportSediment::Set2DData(const char *key, int nRows, int nCols, float **data) {
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);
    nSoiLayers = nCols;
    if (StringMatch(sk, VAR_SOILTHICK)) { this->soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { this->density = data; }
    else if (StringMatch(sk, VAR_SOL_AORGN)) { this->sol_aorgn = data; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { this->sol_orgn = data; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { this->sol_orgp = data; }
    else if (StringMatch(sk, VAR_SOL_FORGP)) { this->sol_fop = data; }
    else if (StringMatch(sk, VAR_SOL_FORGN)) { this->sol_fon = data; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { this->sol_actp = data; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { this->sol_stap = data; }
        /// for CENTURY C/Y cycling model, optional inputs
    else if (StringMatch(sk, VAR_ROCK)) { rock = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else if (StringMatch(sk, VAR_SOL_LSN)) { sol_LSN = data; }
    else if (StringMatch(sk, VAR_SOL_LMN)) { sol_LMN = data; }
    else if (StringMatch(sk, VAR_SOL_HPN)) { sol_HPN = data; }
    else if (StringMatch(sk, VAR_SOL_HSN)) { sol_HSN = data; }
    else if (StringMatch(sk, VAR_SOL_HPC)) { sol_HPC = data; }
    else if (StringMatch(sk, VAR_SOL_HSC)) { sol_HSC = data; }
    else if (StringMatch(sk, VAR_SOL_LMC)) { sol_LMC = data; }
    else if (StringMatch(sk, VAR_SOL_LSC)) { sol_LSC = data; }
    else if (StringMatch(sk, VAR_SOL_LS)) { sol_LS = data; }
    else if (StringMatch(sk, VAR_SOL_LM)) { sol_LM = data; }
    else if (StringMatch(sk, VAR_SOL_LSL)) { sol_LSL = data; }
    else if (StringMatch(sk, VAR_SOL_LSLC)) { sol_LSLC = data; }
    else if (StringMatch(sk, VAR_SOL_LSLNC)) { sol_LSLNC = data; }
    else if (StringMatch(sk, VAR_SOL_BMC)) { sol_BMC = data; }
    else if (StringMatch(sk, VAR_SOL_WOC)) { sol_WOC = data; }
    else if (StringMatch(sk, VAR_PERCO)) { Perco = data; }
    else if (StringMatch(sk, VAR_SSRU)) {
        SSRU = data;
        /// for C-FARM one carbon model
    } else if (StringMatch(sk, VAR_SOL_MP)) { sol_mp = data; }
    else {
        throw ModelException(MID_NUTRSED, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientTransportSediment::initialOutputs() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTRSED, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    // initial enrichment ratio
    if (this->enratio == NULL) {
        Initialize1DArray(m_nCells, enratio, 0.f);
    }
    if (cellArea < 0) {
        cellArea = CELLWIDTH * CELLWIDTH * 0.0001f; //Unit is ha
    }
    // allocate the output variables
    if (sedorgn == NULL) {
        Initialize1DArray(m_nCells, sedorgn, 0.f);
        Initialize1DArray(m_nCells, sedorgp, 0.f);
        Initialize1DArray(m_nCells, sedminpa, 0.f);
        Initialize1DArray(m_nCells, sedminps, 0.f);

        Initialize1DArray(nSubbasins + 1, sedorgnToCh, 0.f);
        Initialize1DArray(nSubbasins + 1, sedorgpToCh, 0.f);
        Initialize1DArray(nSubbasins + 1, sedminpaToCh, 0.f);
        Initialize1DArray(nSubbasins + 1, sedminpsToCh, 0.f);
    }
    /// for CENTURY C/N cycling model outputs
    if (cswat == 2 && sol_latc == NULL) {
        Initialize2DArray(m_nCells, nSoiLayers, sol_latc, 0.f);
        Initialize2DArray(m_nCells, nSoiLayers, sol_percoc, 0.f);
        Initialize1DArray(m_nCells, latc, 0.f);
        Initialize1DArray(m_nCells, percoc, 0.f);
        Initialize1DArray(m_nCells, sedc, 0.f);
    }
}

void NutrientTransportSediment::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

int NutrientTransportSediment::Execute() {
    if (!CheckInputData())return false;
    if (cswat == 1) {
        if (!CheckInputData_CFARM()) {
            return false;
        }
    }
    if (cswat == 2) {
        if (!CheckInputData_CENTURY()) {
            return false;
        }
    }
    this->initialOutputs();
    // initial nutrient to channel for each day
#pragma omp parallel for
    for (int i = 0; i < nSubbasins + 1; i++) {
        sedorgnToCh[i] = 0.f;
        sedorgpToCh[i] = 0.f;
        sedminpaToCh[i] = 0.f;
        sedminpsToCh[i] = 0.f;
    }

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        if (SED_OL[i] < 1.e-4f) SED_OL[i] = 0.f;
        // CREAMS method for calculating enrichment ratio
        enratio[i] = NutrCommon::CalEnrichmentRatio(SED_OL[i], OL_Flow[i], cellArea);
        //if(i == 1000) cout << ""<< SED_OL[i]<<","<< OL_Flow[i] << "," << enratio[i]<<endl;

        //Calculates the amount of organic nitrogen removed in surface runoff
        if (cswat == 0) {
            OrgNRemovedInRunoff_StaticMethod(i);
        } else if (cswat == 1) {
            OrgNRemovedInRunoff_CFARMOneCarbonModel(i);
        } else if (cswat == 2) {
            OrgNRemovedInRunoff_CENTURY(i);
        }
        //Calculates the amount of organic and mineral phosphorus attached to sediment in surface runoff. psed.f of SWAT
        OrgPAttachedtoSed(i);
    }

    //float maxsedorgp = -1.f;
    //int idx = -1;
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	if (sedorgp[i] > maxsedorgp)
    //	{
    //		maxsedorgp = sedorgp[i];
    //		idx = i;
    //	}
    //}
    //cout<<"maximum sedorgp id: "<<idx<< ", surfq: " <<OL_Flow[idx]<< ", sedYld: "<<SED_OL[idx]<<
    //	", eratio: "<<enratio[idx]<<", sedorgp: "<<sedorgp[idx]<<endl;

    // sum by subbasin
    for (int i = 0; i < m_nCells; i++) {
        //add today's flow
        int subi = (int) subbasin[i];
        if (nSubbasins == 1) {
            subi = 1;
        } else if (subi >= nSubbasins + 1) {
            throw ModelException(MID_NUTRSED, "Execute", "The subbasin " + ValueToString(subi) + " is invalid.");
        }
        sedorgnToCh[subi] += sedorgn[i] * cellArea;
        sedorgpToCh[subi] += sedorgp[i] * cellArea;
        sedminpaToCh[subi] += sedminpa[i] * cellArea;
        sedminpsToCh[subi] += sedminps[i] * cellArea;
    }
    //cout << sedorgpToCh[12] << endl;
    // sum all the subbasins and put the sum value in the zero-index of the array
    //for (int i = 1; i < nSubbasins + 1; i++)
    for (vector<int>::iterator it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
        sedorgnToCh[0] += sedorgnToCh[*it];
        sedorgpToCh[0] += sedorgpToCh[*it];
        sedminpaToCh[0] += sedminpaToCh[*it];
        sedminpsToCh[0] += sedminpsToCh[*it];
    }
    return 0;
}

void NutrientTransportSediment::OrgNRemovedInRunoff_StaticMethod(int i) {
    //amount of organic N in first soil layer (orgninfl)
    float orgninfl = 0.f;
    //conversion factor (wt)
    float wt = 0.f;
    orgninfl = sol_orgn[i][0] + sol_aorgn[i][0] + sol_fon[i][0];
    wt = density[i][0] * soilthick[i][0] / 100.f;
    //concentration of organic N in soil (concn)
    float concn = 0.f;
    concn = orgninfl * enratio[i] / wt;
    //Calculate the amount of organic nitrogen transported with sediment to the stream, equation 4:2.2.1 in SWAT Theory 2009, p271
    sedorgn[i] = 0.001f * concn * SED_OL[i] / 1000.f / cellArea;  /// kg/ha
    //update soil nitrogen pools
    if (orgninfl > 1.e-6f) {
        sol_aorgn[i][0] = sol_aorgn[i][0] - sedorgn[i] * (sol_aorgn[i][0] / orgninfl);
        sol_orgn[i][0] = sol_orgn[i][0] - sedorgn[i] * (sol_orgn[i][0] / orgninfl);
        sol_fon[i][0] = sol_fon[i][0] - sedorgn[i] * (sol_fon[i][0] / orgninfl);
        if (sol_aorgn[i][0] < 0.f) {
            sedorgn[i] = sedorgn[i] + sol_aorgn[i][0];
            sol_aorgn[i][0] = 0.f;
        }
        if (sol_orgn[i][0] < 0.f) {
            sedorgn[i] = sedorgn[i] + sol_orgn[i][0];
            sol_orgn[i][0] = 0.f;
        }
        if (sol_fon[i][0] < 0.f) {
            sedorgn[i] = sedorgn[i] + sol_fon[i][0];
            sol_fon[i][0] = 0.f;
        }
    }
}

void NutrientTransportSediment::OrgNRemovedInRunoff_CFARMOneCarbonModel(int i) {
    /// TODO
}

void NutrientTransportSediment::OrgNRemovedInRunoff_CENTURY(int i) {
    float totOrgN_lyr0 = 0.f; /// kg N/ha, amount of organic N in first soil layer, i.e., xx in SWAT src.
    float wt1 = 0.f; /// conversion factor, mg/kg => kg/ha
    float er = 0.f; /// enrichment ratio
    float conc = 0.f; /// concentration of organic N in soil
    float sol_mass = 0.f; /// soil mass, kg
    float QBC = 0.f; /// C loss with runoff or lateral flow
    float VBC = 0.f; /// C loss with vertical flow
    float YBC = 0.f; /// BMC loss with sediment
    float YOC = 0.f; /// Organic C loss with sediment
    float YW = 0.f; /// Wind erosion, kg
    float TOT = 0.f; /// total organic carbon in layer 1
    float YEW = 0.f; /// fraction of soil erosion of total soil mass
    float X1 = 0.f, PRMT_21 = 0.f;
    float PRMT_44 = 0.f; /// ratio of soluble C concentration in runoff to percolate (0.1 - 1.0)
    float XX = 0.f, DK = 0.f, V = 0.f, X3 = 0.f;
    float CO = 0.f; /// the vertical concentration
    float CS = 0.f; /// the horizontal concentration
    float perc_clyr = 0.f, latc_clyr = 0.f;

    totOrgN_lyr0 = sol_LSN[i][0] + sol_LMN[i][0] + sol_HPN[i][0] + sol_HSN[i][0];
    wt1 = density[i][0] * soilthick[i][0] / 100.f;
    er = enratio[i];
    conc = totOrgN_lyr0 * er / wt1;
    sedorgn[i] = 0.001f * conc * SED_OL[i] / 1000.f / cellArea;
    /// update soil nitrogen pools
    if (totOrgN_lyr0 > UTIL_ZERO) {
        float xx1 = (1.f - sedorgn[i] / totOrgN_lyr0);
        sol_LSN[i][0] *= xx1;
        sol_LMN[i][0] *= xx1;
        sol_HPN[i][0] *= xx1;
        sol_HSN[i][0] *= xx1;
    }
    /// Calculate runoff and leached C&N from micro-biomass
    sol_mass = soilthick[i][0] / 1000.f * 10000.f * density[i][0] * 1000.f *
        (1.f - rock[i][0] / 100.f); /// kg/ha
    /// total organic carbon in layer 1
    TOT = sol_HPC[i][0] + sol_HSC[i][0] + sol_LMC[i][0] + sol_LSC[i][0];
    /// fraction of soil erosion of total soil mass
    YEW = min((SED_OL[i] / cellArea + YW / cellArea) / sol_mass, 0.9f);

    X1 = 1.f - YEW;
    YOC = YEW * TOT;
    sol_HSC[i][0] *= X1;
    sol_HPC[i][0] *= X1;
    sol_LS[i][0] *= X1;
    sol_LM[i][0] *= X1;
    sol_LSL[i][0] *= X1;
    sol_LSC[i][0] *= X1;
    sol_LMC[i][0] *= X1;
    sol_LSLC[i][0] *= X1;
    sol_LSLNC[i][0] = sol_LSC[i][0] - sol_LSLC[i][0];

    if (sol_BMC[i][0] > 0.01f) {
        ///KOC FOR CARBON LOSS IN WATER AND SEDIMENT(500._1500.) KD = KOC * C
        PRMT_21 = 1000.f;
        sol_WOC[i][0] = sol_LSC[i][0] + sol_LMC[i][0] + sol_HPC[i][0] + sol_HSC[i][0] + sol_BMC[i][0];
        DK = 0.0001f * PRMT_21 * sol_WOC[i][0];
        X1 = sol_ul[i][0];
        if (X1 <= 0.f) X1 = 0.01f;
        XX = X1 + DK;
        V = OL_Flow[i] + Perco[i][0] + SSRU[i][0];
        if (V > 1.e-10f) {
            X3 = sol_BMC[i][0] * (1.f - exp(-V / XX)); /// loss of biomass C
            PRMT_44 = 0.5;
            CO = X3 / (Perco[i][0] + PRMT_44 * (OL_Flow[i] + SSRU[i][0]));
            CS = PRMT_44 * CO;
            VBC = CO * Perco[i][0];
            sol_BMC[i][0] -= X3;
            QBC = CS * (OL_Flow[i] + SSRU[i][0]);
            /// Compute WBMC loss with sediment
            if (YEW > 0.f) {
                CS = DK * sol_BMC[i][0] / XX;
                YBC = YEW * CS;
            }
        }
    }
    sol_BMC[i][0] -= YBC;
    /// surfqc_d(j) = QBC*(surfq(j)/(surfq(j)+flat(1,j)+1.e-6))  is for print purpose, thus not implemented.
    sol_latc[i][0] = QBC * (SSRU[i][0] / (OL_Flow[i] + SSRU[i][0] + UTIL_ZERO));
    sol_percoc[i][0] = VBC;
    sedc[i] = YOC + YBC;

    latc_clyr += sol_latc[i][0];
    for (int k = 1; k < (int) soillayers[i]; k++) {
        sol_WOC[i][k] = sol_LSC[i][k] + sol_LMC[i][k] + sol_HPC[i][k] + sol_HSC[i][k];
        float Y1 = sol_BMC[i][k] + VBC;
        VBC = 0.f;
        if (Y1 > 0.01f) {
            V = Perco[i][k] + SSRU[i][k];
            if (V > 0.f) {
                VBC = Y1 * (1.f - exp(-V / (sol_ul[i][k] + 0.0001f * PRMT_21 * sol_WOC[i][k])));
            }
        }
        sol_latc[i][k] = VBC * (SSRU[i][k] / (SSRU[i][k] + Perco[i][k] + UTIL_ZERO));
        sol_percoc[i][k] = VBC - sol_latc[i][k];
        sol_BMC[i][k] = Y1 - VBC;

        /// calculate nitrate in percolate and lateral flow
        perc_clyr += sol_percoc[i][k];
        latc_clyr += sol_latc[i][k];
    }
    latc[i] = latc_clyr;
    percoc[i] = perc_clyr;
}

void NutrientTransportSediment::OrgPAttachedtoSed(int i) {
    //amount of phosphorus attached to sediment in soil (sol_attp)
    float sol_attp = 0.f;
    //fraction of active mineral/organic/stable mineral phosphorus in soil (sol_attp_o, sol_attp_a, sol_attp_s)
    float sol_attp_o = 0.f;
    float sol_attp_a = 0.f;
    float sol_attp_s = 0.f;
    //Calculate sediment
    sol_attp = sol_orgp[i][0] + sol_fop[i][0] + sol_actp[i][0] + sol_stap[i][0];
    if (sol_mp != NULL) {
        sol_attp += sol_mp[i][0];
    }
    if (sol_attp > 1.e-3f) {
        sol_attp_o = (sol_orgp[i][0] + sol_fop[i][0]) / sol_attp;
        if (sol_mp != NULL) {
            sol_attp_o += sol_mp[i][0] / sol_attp;
        }
        sol_attp_a = sol_actp[i][0] / sol_attp;
        sol_attp_s = sol_stap[i][0] / sol_attp;
    }
    //conversion factor (mg/kg => kg/ha) (wt)
    float wt = density[i][0] * soilthick[i][0] / 100.f;
    //concentration of organic P in soil (concp)
    float concp = 0.f;
    concp = sol_attp * enratio[i] / wt;  /// mg/kg
    //total amount of P removed in sediment erosion (sedp)
    float sedp = 1.e-6f * concp * SED_OL[i] / cellArea; /// kg/ha
    sedorgp[i] = sedp * sol_attp_o;
    sedminpa[i] = sedp * sol_attp_a;
    sedminps[i] = sedp * sol_attp_s;

    //if(i==100)cout << "sedp: " << sedp<< ",sol_attp_o: "  << sol_attp_o << endl;
    //modify phosphorus pools

    //total amount of P in mineral sediment pools prior to sediment removal (psedd)		// Not used
    //float psedd = 0.f;
    //psedd = sol_actp[i][0] + sol_stap[i][0];

    //total amount of P in organic pools prior to sediment removal (porgg)
    float porgg = 0.f;
    porgg = sol_orgp[i][0] + sol_fop[i][0];
    if (porgg > 1.e-3f) {
        sol_orgp[i][0] = sol_orgp[i][0] - sedorgp[i] * (sol_orgp[i][0] / porgg);
        sol_fop[i][0] = sol_fop[i][0] - sedorgp[i] * (sol_fop[i][0] / porgg);
    }
    sol_actp[i][0] = sol_actp[i][0] - sedminpa[i];
    sol_stap[i][0] = sol_stap[i][0] - sedminps[i];
    if (sol_orgp[i][0] < 0.f) {
        sedorgp[i] = sedorgp[i] + sol_orgp[i][0];
        sol_orgp[i][0] = 0.f;
    }
    if (sol_fop[i][0] < 0.f) {
        sedorgp[i] = sedorgp[i] + sol_fop[i][0];
        sol_fop[i][0] = 0.f;
    }
    if (sol_actp[i][0] < 0.f) {
        sedminpa[i] = sedminpa[i] + sol_actp[i][0];
        sol_actp[i][0] = 0.f;
    }
    if (sol_stap[i][0] < 0.f) {
        sedminps[i] = sedminps[i] + sol_stap[i][0];
        sol_stap[i][0] = 0.f;
    }
    // if (i == 46364) cout << "surfq: " <<OL_Flow[i]<< ", sedYld: "<<SED_OL[i]<<", sedorgp: "<<sedorgp[i]<< endl;
}

void NutrientTransportSediment::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SEDORGN)) {
        *data = this->sedorgn;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SEDORGP)) {
        *data = this->sedorgp;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SEDMINPA)) {
        *data = this->sedminpa;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SEDMINPS)) {
        *data = this->sedminps;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SEDORGN_TOCH)) {
        *data = sedorgnToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SEDORGP_TOCH)) {
        *data = sedorgpToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SEDMINPA_TOCH)) {
        *data = sedminpaToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SEDMINPS_TOCH)) {
        *data = sedminpsToCh;
        *n = nSubbasins + 1;
    }
        /// outputs of CENTURY C/N cycling model
    else if (StringMatch(sk, VAR_LATERAL_C)) {
        *data = this->latc;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_PERCO_C)) {
        *data = this->percoc;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SEDLOSS_C)) {
        *data = this->sedc;
        *n = m_nCells;
    } else {
        throw ModelException(MID_NUTRSED, "Get1DData", "Parameter " + sk + " does not exist");
    }
}

void NutrientTransportSediment::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = nSoiLayers;
    if (StringMatch(sk, VAR_SOL_AORGN)) { *data = this->sol_aorgn; }
    else if (StringMatch(sk, VAR_SOL_FORGN)) { *data = this->sol_fon; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { *data = this->sol_orgn; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { *data = this->sol_orgp; }
    else if (StringMatch(sk, VAR_SOL_FORGP)) { *data = this->sol_fop; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { *data = this->sol_stap; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { *data = this->sol_actp; }
        /// outputs of CENTURY C/N cycling model
    else if (StringMatch(sk, VAR_SOL_LATERAL_C)) { *data = this->sol_latc; }
    else if (StringMatch(sk, VAR_SOL_PERCO_C)) { *data = this->sol_percoc; }
    else {
        throw ModelException(MID_NUTRSED, "Get2DData", "Output " + sk + " does not exist.");
    }
}
