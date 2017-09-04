#include "seims.h"
#include "NutrientinGroundwater.h"

using namespace std;

NutrientinGroundwater::NutrientinGroundwater(void) :
//input
    TIMESTEP(-1), m_nCells(-1), CELLWIDTH(-1), gwno3_conc(NULL), gwsolp_conc(NULL), SBQG(NULL),
    nSubbasins(-1), subbasin(NULL), m_subbasinsInfo(NULL), SBGS(NULL),
    perco_n_gw(NULL), perco_p_gw(NULL), soillayers(NULL), nSoilLayers(-1), sol_no3(NULL),
    gwno3(NULL), gwsolp(NULL), GW0(NODATA_VALUE),
    //output
    minpgwToCh(NULL), no3gwToCh(NULL) {

}

NutrientinGroundwater::~NutrientinGroundwater(void) {
    if (no3gwToCh != NULL) Release1DArray(no3gwToCh);
    if (minpgwToCh != NULL) Release1DArray(minpgwToCh);
    if (gwno3 != NULL) Release1DArray(gwno3);
    if (gwsolp != NULL) Release1DArray(gwsolp);
}

void NutrientinGroundwater::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

bool NutrientinGroundwater::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRGW, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
                m_nCells << ".\n";
            throw ModelException(MID_NUTRGW, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrientinGroundwater::CheckInputData() {
    if (m_nCells <= 0) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The cells number can not be less than zero.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }
    if (CELLWIDTH < 0) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (GW0 < 0) {
        throw ModelException(MID_NUTRGW, "CheckInputData",
                             "The initial groundwater storage can not be less than zero.");
    }
    if (SBQG == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData",
                             "The groundwater contribution to stream flow data can not be NULL.");
    }
    if (SBGS == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The groundwater storage can not be NULL.");
    }
    if (perco_n_gw == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The NO3 percolation to groundwater can not be NULL.");
    }
    if (perco_p_gw == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The solute P percolation to groundwater can not be NULL.");
    }
    if (soillayers == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "The soil layers number can not be NULL.");
    }
    if (sol_no3 == NULL) {
        throw ModelException(MID_NUTRGW, "CheckInputData", "sol_no3 can not be NULL.");
    }
    return true;
}

void NutrientinGroundwater::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, Tag_TimeStep)) {
        TIMESTEP = int(value);
    } else if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) value);
    } else if (StringMatch(sk, Tag_CellWidth)) {
        CELLWIDTH = value;
    } else if (StringMatch(sk, VAR_GW0)) {
        GW0 = value;
    } else {
        throw ModelException(MID_NUTRGW, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN)) {
        if (!CheckInputSize(key, n)) return;
        subbasin = data;
    } else if (StringMatch(sk, VAR_SBQG)) {
        SBQG = data;
    } else if (StringMatch(sk, VAR_SBGS)) {
        SBGS = data;
    } else if (StringMatch(sk, VAR_PERCO_N_GW)) {
        perco_n_gw = data;
    } else if (StringMatch(sk, VAR_PERCO_P_GW)) {
        perco_p_gw = data;
    } else if (StringMatch(sk, VAR_SOILLAYERS)) {
        if (!CheckInputSize(key, n)) return;
        soillayers = data;
    } else {
        throw ModelException(MID_NUTRGW, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::Set2DData(const char *key, int nRows, int nCols, float **data) {
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);

    nSoilLayers = nCols;
    if (StringMatch(sk, VAR_SOL_NO3)) {
        sol_no3 = data;
    } else if (StringMatch(sk, VAR_SOL_SOLP)) {
        sol_solp = data;
    } else {
        throw ModelException(MID_NUTRGW, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientinGroundwater::SetReaches(clsReaches *reaches) {
    if (reaches != NULL) {
        nSubbasins = reaches->GetReachNumber();
        vector<int> m_reachId = reaches->GetReachIDs();
        if (gwsolp_conc == NULL) {
            Initialize1DArray(nSubbasins + 1, gwsolp_conc, 0.f);
            Initialize1DArray(nSubbasins + 1, gwno3_conc, 0.f);
        }

        for (vector<int>::iterator it = m_reachId.begin(); it != m_reachId.end(); it++) {
            clsReach *tmpReach = reaches->GetReachByID(*it);
            gwno3_conc[*it] = tmpReach->GetGWNO3();
            gwsolp_conc[*it] = tmpReach->GetGWSolP();
        }
    } else {
        throw ModelException(MID_NUTRGW, "SetReaches", "The reaches input can not to be NULL.");
    }
}

void NutrientinGroundwater::initialOutputs() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTRGW, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    // allocate the output variables
    if (no3gwToCh == NULL) {
        Initialize1DArray(nSubbasins + 1, no3gwToCh, 0.f);
        Initialize1DArray(nSubbasins + 1, minpgwToCh, 0.f);
    }
    if (gwno3 == NULL) {    /// initial nutrient amount stored in groundwater
        gwno3 = new float[nSubbasins + 1];
        gwsolp = new float[nSubbasins + 1];
        for (int i = 1; i <= nSubbasins; i++) {
            Subbasin *subbasin = m_subbasinsInfo->GetSubbasinByID(i);
            float subArea = subbasin->getCellCount() * CELLWIDTH * CELLWIDTH;    //m2
            gwno3[i] = GW0 * gwno3_conc[i] * subArea / 1000000.f; /// mm * mg/L * m2 = 10^-6 kg
            gwsolp[i] = GW0 * gwsolp_conc[i] * subArea / 1000000.f;
        }
    }
}

int NutrientinGroundwater::Execute() {
    CheckInputData();
    initialOutputs();
    //int cellid = 18605;
    //cout<<"NutrGW, pre solno3: ";
    //for (int j = 0; j < (int)soillayers[cellid]; j++)
    //	cout<<j<<", "<<sol_no3[cellid][j]<<", ";
    //cout<<endl;
    for (vector<int>::iterator iter = m_subbasinIDs.begin(); iter != m_subbasinIDs.end(); iter++) {
        int id = *iter;
        Subbasin *subbasin = m_subbasinsInfo->GetSubbasinByID(id);
        int nCells = subbasin->getCellCount();
        float subArea = nCells * CELLWIDTH * CELLWIDTH;    // m^2
        float revap = subbasin->getEG();
        /// 1. firstly, restore the groundwater storage during current day
        ///    since the SBGS has involved percolation water, just need add revap and runoff water
        float gwqVol = SBQG[id] * TIMESTEP;    // m^3, water volume flow out
        float reVapVol = revap * subArea / 1000.f; // m^3
        float tmpGwStorage = SBGS[id] * subArea / 1000.f + gwqVol + reVapVol;
        /// 2. secondly, update nutrient concentration
        gwno3[id] += perco_n_gw[id]; /// nutrient amount, kg
        gwsolp[id] += perco_p_gw[id];
        gwno3_conc[id] = gwno3[id] / tmpGwStorage * 1000.f; // kg / m^3 * 1000. = mg/L
        gwsolp_conc[id] = gwsolp[id] / tmpGwStorage * 1000.f;
        /// 3. thirdly, calculate nutrient in groundwater runoff
        //cout<<"subID: "<<id<<", gwQ: "<<SBQG[id] << ", ";
        no3gwToCh[id] = gwno3_conc[id] * gwqVol / 1000.f;    // g/m3 * m3 / 1000 = kg
        minpgwToCh[id] = gwsolp_conc[id] * gwqVol / 1000.f;
        //cout<<"subID: "<<id<<", gwno3Con: "<<gwno3_conc[id] << ", no3gwToCh: "<<m_no3gwToCh[id] << ", ";
        /// 4. fourthly, calculate nutrient loss loss through revep and update no3 in the bottom soil layer
        float no3ToSoil = revap / 1000.f * gwno3_conc[id] * 10.f;    // kg/ha  (m*10*g/m3=kg/ha)
        float solpToSoil = revap / 1000.f * gwsolp_conc[id] * 10.f;
        float no3ToSoil_kg = no3ToSoil * subArea / 10000.f; /// kg/ha * m^2 / 10000.f = kg
        float solpToSoil_kg = solpToSoil * subArea / 10000.f;
        int *cells = subbasin->getCells();
        int index = 0;
        for (int i = 0; i < nCells; i++) {
            index = cells[i];
            //if (index == cellid)
            //	cout<<"revap: "<<revap<<", gwNo3: "<<gwno3[id]<<", gwStorg: "<<
            //	tmpGwStorage<<", percoNo3Gw: "<<perco_n_gw[id]<<
            //	", gwNo3Con: "<<gwno3_conc[id]<<", no3ToSoil: "<<no3ToSoil<<endl;
            sol_no3[index][(int) soillayers[index] - 1] += no3ToSoil;
            sol_solp[index][(int) soillayers[index] - 1] += solpToSoil;
        }
        /// finally, update nutrient amount
        gwno3[id] -= (no3gwToCh[id] + no3ToSoil_kg);
        gwsolp[id] -= (minpgwToCh[id] + solpToSoil_kg);

        //float gwVol = subArea * SBGS[id] / 1000.f; //m3, memo, this SBGS is the resulted status of the current time step
        //gwno3_conc[id] += (perco_n_gw[id] - no3gwToCh[id]) * 1000.f / gwVol;
        //gwsolp_conc[id] += (perco_p_gw[id] - minpgwToCh[id]) * 1000.f / gwVol;
        //cout<<"subID: "<<id<<", percoNo3: "<<perco_n_gw[id]<<", gwStorage: "<<SBGS[id]<<", new gwno3Con: "<<gwno3_conc[id] << ", ";
    }
    //cout<<"NutrGW, after solno3: ";
    //for (int j = 0; j < (int)soillayers[cellid]; j++)
    //	cout<<j<<", "<<sol_no3[cellid][j]<<", ";
    //cout<<endl;
    return 0;
}

void NutrientinGroundwater::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = nSubbasins + 1;
    if (StringMatch(sk, VAR_NO3GW_TOCH)) {
        *data = no3gwToCh;
    } else if (StringMatch(sk, VAR_MINPGW_TOCH)) {
        *data = minpgwToCh;
    } else if (StringMatch(sk, VAR_GWNO3_CONC)) {
        *data = gwno3_conc;
    } else if (StringMatch(sk, VAR_GWSOLP_CONC)) {
        *data = gwsolp_conc;
    } else if (StringMatch(sk, VAR_GWNO3)) {
        *data = gwno3;
    } else if (StringMatch(sk, VAR_GWSOLP)) {
        *data = gwsolp;
    } else {
        throw ModelException(MID_NUTRGW, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}
