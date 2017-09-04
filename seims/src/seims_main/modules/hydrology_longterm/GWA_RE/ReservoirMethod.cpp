#include "seims.h"
#include "ReservoirMethod.h"

ReservoirMethod::ReservoirMethod(void) : TIMESTEP(-1), m_nCells(-1), nSubbasins(-1), nSoilLayers(-1),
                                         soilDepth(NULL), soillayers(NULL), soilthick(NULL), slope(NULL),
                                         GWNEW(NULL), m_subbasinsInfo(NULL),
                                         df_coef(NODATA_VALUE), Kg(NODATA_VALUE), Base_ex(NODATA_VALUE),
                                         Perco(NULL), INET(NULL), DEET(NULL), SOET(NULL), PET(NULL),
                                         AET_PLT(NULL), solst(NULL),
    /// intermediate
                                         SBPET(NULL), SBGS(NULL),
    /// outputs
                                         T_Perco(NULL), T_PerDep(NULL), RG(NULL), SBQG(NULL), Revap(NULL),
                                         T_Revap(NULL), firstRun(true), GWWB(NULL) {
}

ReservoirMethod::~ReservoirMethod(void) {
    if (T_Perco != NULL) Release1DArray(T_Perco);
    if (T_PerDep != NULL) Release1DArray(T_PerDep);
    if (Revap != NULL) Release1DArray(Revap);
    if (T_Revap != NULL) Release1DArray(T_Revap);
    if (RG != NULL) Release1DArray(RG);
    if (SBQG != NULL) Release1DArray(SBQG);
    if (SBPET != NULL) Release1DArray(SBPET);
    if (SBGS != NULL) Release1DArray(SBGS);
    if (GWWB != NULL) Release2DArray(nSubbasins + 1, GWWB);
}

void ReservoirMethod::initialOutputs() {
    int nLen = nSubbasins + 1;
    if (firstRun) {
        setSubbasinInfos();
        firstRun = false;
    }
    if (T_Perco == NULL) Initialize1DArray(nLen, T_Perco, 0.f);
    if (T_Revap == NULL) Initialize1DArray(nLen, T_Revap, 0.f);
    if (T_PerDep == NULL) Initialize1DArray(nLen, T_PerDep, 0.f);
    if (RG == NULL) Initialize1DArray(nLen, RG, 0.f);
    if (SBQG == NULL) Initialize1DArray(nLen, SBQG, 0.f);
    if (SBPET == NULL) Initialize1DArray(nLen, SBPET, 0.f);
    if (SBGS == NULL) Initialize1DArray(nLen, SBGS, GW0);
    if (Revap == NULL) Initialize1DArray(m_nCells, Revap, 0.f);
    if (GWWB == NULL) Initialize2DArray(nLen, 6, GWWB, 0.f);
}

int ReservoirMethod::Execute() {
    if (!CheckInputData()) return -1;
    initialOutputs();
    float QGConvert = 1.f * CELLWIDTH * CELLWIDTH / (TIMESTEP) / 1000.f; // mm ==> m3/s
    for (vector<int>::iterator it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
        int subID = *it;
        Subbasin *curSub = m_subbasinsInfo->GetSubbasinByID(subID);

        // get percolation from the bottom soil layer at the subbasin scale
        int curCellsNum = curSub->getCellCount();
        int *curCells = curSub->getCells();
        float perco = 0.f;
#pragma omp parallel for reduction(+:perco)
        for (int i = 0; i < curCellsNum; i++) {
            int index = 0;
            index = curCells[i];
            perco += Perco[index][(int) soillayers[index] - 1];
        }
        perco /= curCellsNum; // mean mm
        /// percolated water ==> vadose zone ==> shallow aquifer ==> deep aquifer
        /// currently, for convenience, we assume a small portion of the percolated water
        /// will enter groundwater. By LJ. 2016-9-2
        float ratio2gw = 1.f;
        perco *= ratio2gw;
        curSub->setPerco(perco);

        //if (perco > 0.f)
        //{
        //	cout << "subID: "<<subID<<", perco mean: "<<perco << endl;
        //}

        //calculate EG, i.e. Revap
        float revap = 0.f;
        float fPET = 0.f;
        float fEI = 0.f;
        float fED = 0.f;
        float fES = 0.f;
        float plantEP = 0.f;
        fPET = Sum(curCellsNum, curCells, PET) / curCellsNum;
        fEI = Sum(curCellsNum, curCells, INET) / curCellsNum;
        fED = Sum(curCellsNum, curCells, DEET) / curCellsNum;
        fES = Sum(curCellsNum, curCells, SOET) / curCellsNum;
        plantEP = Sum(curCellsNum, curCells, AET_PLT) / curCellsNum;

        curSub->setPET(fPET);

        //if percolation < 0.01, EG will be 0. if percolation >= 0.01, EG will be calculated by equation (why? this is not used currently. Junzhi Liu 2016-08-14).
        //if (perco >= 0.01f)
        //{
        revap = (fPET - fEI - fED - fES - plantEP) * SBGS[subID] / GWMAX;
        if (revap != revap) {
            cout << "fPET: " << fPET << ", fEI: " << fEI << ", fED: " << fED << ", fES: " << fES << ", plantEP: "
                << plantEP << ", " << " subbasin ID: " << subID << ", gwStore: " << SBGS[subID] << endl;
            throw ModelException(MID_GWA_RE, "Execute", "revap calculation wrong!");
        }
        revap = max(revap, 0.f);
        revap = min(revap, perco);
        //}
        //float prevRevap = curSub->getEG();
        //if (prevRevap != revap)
        //{
        //	curSub->setEG(revap);
        //	curSub->setIsRevapChanged(true);
        //}
        //else
        //	curSub->setIsRevapChanged(false);
        curSub->setEG(revap);

        //deep percolation
        float percoDeep = perco * df_coef;
        curSub->setPerde(percoDeep);

        // groundwater runoff (mm)
        float slopeCoef = curSub->getSlopeCoef();
        float kg = Kg * slopeCoef;
        float groundRunoff = kg * pow(SBGS[subID], Base_ex); //mm
        if (groundRunoff != groundRunoff) {
            cout << groundRunoff;
        }

        float groundQ = groundRunoff * curCellsNum * QGConvert; // groundwater discharge (m3/s)

        float groundStorage = SBGS[subID];
        groundStorage += (perco - revap - percoDeep - groundRunoff);

        //add the ground water from bank storage, 2011-3-14
        float gwBank = 0.f;
        // at the first time step GWNEW is NULL
        if (GWNEW != NULL) {
            gwBank = GWNEW[subID];
        }
        groundStorage += gwBank / curSub->getArea() * 1000.f;

        groundStorage = max(groundStorage, 0.f);
        if (groundStorage > GWMAX) {
            groundRunoff += (groundStorage - GWMAX);
            groundQ = groundRunoff * curCellsNum * QGConvert; // groundwater discharge (m3/s)
            groundStorage = GWMAX;
        }
        curSub->setRG(groundRunoff);
        curSub->setGW(groundStorage);
        curSub->setQG(groundQ);
        if (groundStorage != groundStorage) {
            ostringstream oss;
            oss << perco << "\t" << revap << "\t" << percoDeep << "\t" << groundRunoff << "\t" << SBGS[subID]
                << "\t" << Kg << "\t" <<
                Base_ex << "\t" << slopeCoef << endl;
            throw ModelException("Subbasin", "setInputs", oss.str());
        }
        T_Perco[subID] = curSub->getPerco();
        T_Revap[subID] = curSub->getEG();
        T_PerDep[subID] = curSub->getPerde();
        RG[subID] = curSub->getRG();                //get rg of specific subbasin
        SBQG[subID] = curSub->getQG();                //get qg of specific subbasin
        SBPET[subID] = curSub->getPET();
        SBGS[subID] = curSub->getGW();
    }

    T_Perco[0] = m_subbasinsInfo->subbasin2basin(VAR_PERCO);
    T_Revap[0] = m_subbasinsInfo->subbasin2basin(VAR_REVAP);
    T_PerDep[0] = m_subbasinsInfo->subbasin2basin(VAR_PERDE);
    RG[0] = m_subbasinsInfo->subbasin2basin(VAR_RG);  // get rg of entire watershed
    SBGS[0] = m_subbasinsInfo->subbasin2basin(VAR_GW_Q);
    SBQG[0] = m_subbasinsInfo->subbasin2basin(VAR_QG);  // get qg of entire watershed

    // output to GWWB
    for (int i = 0; i <= nSubbasins; i++) {
        GWWB[i][0] = T_Perco[i];
        GWWB[i][1] = T_Revap[i];
        GWWB[i][2] = T_PerDep[i];
        GWWB[i][3] = RG[i];
        GWWB[i][4] = SBGS[i];
        GWWB[i][5] = SBQG[i];
    }

    // update soil moisture
    for (vector<int>::iterator it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
        Subbasin *sub = m_subbasinsInfo->GetSubbasinByID(*it);
        int *cells = sub->getCells();
        int nCells = sub->getCellCount();
        int index = 0;
        for (int i = 0; i < nCells; i++) {
            index = cells[i];
            solst[index][(int) soillayers[index] - 1] += sub->getEG();
            // TODO: Is it need to allocate revap to each soil layers??? By LJ
        }
    }
    return 0;
}

bool ReservoirMethod::CheckInputData() {
    if (m_nCells <= 0) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_nCells has not been set.");
    }
    if (CELLWIDTH < 0) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_NumSub has not been set.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }
    if (nSoilLayers <= 0) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: nSoilLayers has not been set.");
    }
    if (FloatEqual(df_coef, NODATA_VALUE)) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: df_coef has not been set.");
    }
    if (FloatEqual(Kg, NODATA_VALUE)) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Kg has not been set.");
    }
    if (FloatEqual(Base_ex, NODATA_VALUE)) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Base_ex has not been set.");
    }
    if (Perco == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Percolation_2D has not been set.");
    }
    if (INET == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: INET has not been set.");
    }
    if (DEET == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: DEET has not been set.");
    }
    if (SOET == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: SOET has not been set.");
    }
    if (AET_PLT == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Plant EP has not been set.");
    }
    if (PET == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: PET has not been set.");
    }
    if (slope == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Slope has not been set.");
    }
    if (solst == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil moisture has not been set.");
    }
    if (soillayers == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil layers has not been set.");
    }
    if (soilthick == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil thickness has not been set.");
    }

    if (nSubbasins <= 0) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasins information can not be NULL.");
    }
    return true;
}

bool ReservoirMethod::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_GWA_RE, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_GWA_RE, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

// set value
void ReservoirMethod::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, Tag_TimeStep)) {
        TIMESTEP = int(value);
    } else if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) value);
    } else if (StringMatch(sk, Tag_CellWidth)) {
        CELLWIDTH = value;
    } else if (StringMatch(sk, VAR_KG)) {
        Kg = value;
    } else if (StringMatch(sk, VAR_Base_ex)) {
        Base_ex = value;
    } else if (StringMatch(sk, VAR_DF_COEF)) {
        df_coef = value;
    } else if (StringMatch(sk, VAR_GW0)) {
        GW0 = value;
    } else if (StringMatch(sk, VAR_GWMAX)) {
        GWMAX = value;
    } else {
        throw ModelException(MID_GWA_RE, "SetValue", "Parameter " + sk + " does not exist in current module.");
    }
}

void ReservoirMethod::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    if (StringMatch(sk, VAR_GWNEW)) {
        GWNEW = data;
        return;
    }

    //check the input data
    if (!CheckInputSize(key, n)) return;

    //set the value
    if (StringMatch(sk, VAR_INET)) {
        INET = data;
    } else if (StringMatch(sk, VAR_DEET)) {
        DEET = data;
    } else if (StringMatch(sk, VAR_SOET)) {
        SOET = data;
    } else if (StringMatch(sk, VAR_AET_PLT)) {
        AET_PLT = data;
    } else if (StringMatch(sk, VAR_PET)) {
        PET = data;
    } else if (StringMatch(sk, VAR_SLOPE)) {
        slope = data;
    } else if (StringMatch(sk, VAR_SOILLAYERS)) {
        soillayers = data;
    } else {
        throw ModelException(MID_GWA_RE, "Set1DData", "Parameter " + sk + " does not exist in current module.");
    }
}

void ReservoirMethod::Set2DData(const char *key, int nrows, int ncols, float **data) {
    string sk(key);
    CheckInputSize(key, nrows);
    nSoilLayers = ncols;

    if (StringMatch(sk, VAR_PERCO)) {
        Perco = data;
    } else if (StringMatch(sk, VAR_SOL_ST)) {
        solst = data;
    } else if (StringMatch(sk, VAR_SOILDEPTH)) {
        soilDepth = data;
    } else if (StringMatch(sk, VAR_SOILTHICK)) {
        soilthick = data;
    } else {
        throw ModelException(MID_GWA_RE, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void ReservoirMethod::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

void ReservoirMethod::Get1DData(const char *key, int *nRows, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_REVAP)) {
        *data = Revap;
        *nRows = m_nCells;
    } else if (StringMatch(sk, VAR_RG)) {
        *data = RG;
        *nRows = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SBQG)) {
        *data = SBQG;
        *nRows = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SBGS)) {
        *data = SBGS;
        *nRows = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SBPET)) {
        *data = SBPET;
        *nRows = nSubbasins + 1;
    } else {
        throw ModelException(MID_GWA_RE, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}

void ReservoirMethod::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_GWWB)) {
        *data = GWWB;
        *nRows = nSubbasins + 1;
        *nCols = 6;
    } else {
        throw ModelException(MID_GWA_RE, "Get2DData", "Parameter " + sk + " does not exist in current module.");
    }
}

void ReservoirMethod::setSubbasinInfos() {
    for (vector<int>::iterator it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
        Subbasin *curSub = m_subbasinsInfo->GetSubbasinByID(*it);
        if (curSub->getSlope() <= 0.f) {
            curSub->setSlope(slope);
        }
    }
    m_subbasinsInfo->SetSlopeCoefficient();
}
