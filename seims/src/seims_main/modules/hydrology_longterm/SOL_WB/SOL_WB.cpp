#include "seims.h"
#include "SOL_WB.h"

SOL_WB::SOL_WB(void) : m_nCells(-1), nSoilLayers(-1), soillayers(NULL), soilthick(NULL), SOL_ZMX(NULL),
                       NEPR(NULL), INFIL(NULL), SOET(NULL), Revap(NULL),
                       SSRU(NULL), Perco(NULL), solst(NULL),
                       m_subbasinsInfo(NULL), nSubbasins(-1),
                       D_P(NULL), INLO(NULL), INET(NULL), DPST(NULL), DEET(NULL),
                       SURU(NULL), RG(NULL), SNSB(NULL), TMEAN(NULL), SOTE(NULL),
                       SOWB(NULL) {
}

SOL_WB::~SOL_WB(void) {
    if (SOWB != NULL) Release2DArray(nSubbasins + 1, SOWB);
}

void SOL_WB::initialOutputs() {
    if (SOWB == NULL) Initialize2DArray(nSubbasins + 1, 16, SOWB, 0.f);
}

int SOL_WB::Execute() {
    CheckInputData();
    return 0;
}

void SOL_WB::SetValue(const char *key, float data) {
    string s(key);
    if (StringMatch(s, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
    else {
        throw ModelException(MID_SOL_WB, "SetValue", "Parameter " + s
            + " does not exist. Please contact the module developer.");
    }
}

void SOL_WB::setValueToSubbasins() {
    if (m_subbasinsInfo != NULL) {
        vector<int>::iterator it;
        for (it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++) {
            Subbasin *curSub = m_subbasinsInfo->GetSubbasinByID(*it);
            int *cells = curSub->getCells();
            int cellsNum = curSub->getCellCount();
            float ri = 0.f; // total subsurface runoff of soil profile (mm)
            float sm = 0.f; // total soil moisture of soil profile (mm)
            float pcp = 0.f, netPcp = 0.f, depET = 0.f, infil = 0.f;
            float itpET = 0.f, netPerc = 0.f, r = 0.f, revap = 0.f;
            float rs = 0.f, meanT = 0.f, soilT = 0.f, es = 0.f, totalET = 0.f;

            for (int i = 0; i < cellsNum; i++) // loop cells of current subbasin
            {
                int cell = cells[i];
                ri = 0.f;
                sm = 0.f;
                for (int j = 0; j < (int) soillayers[cell]; j++) {
                    ri += SSRU[cell][j] / float(cellsNum);
                    sm += solst[cell][j] / float(cellsNum);
                }
                pcp += D_P[cell] / float(cellsNum);
                meanT += TMEAN[cell] / float(cellsNum);
                soilT += SOTE[cell] / float(cellsNum);
                netPcp += NEPR[cell] / float(cellsNum);
                itpET += INET[cell] / float(cellsNum);
                depET += DEET[cell] / float(cellsNum);
                infil += INFIL[cell] / float(cellsNum);
                totalET += (INET[cell] + DEET[cell] + SOET[cell]) / float(cellsNum); // add plant et?
                es += SOET[cell] / float(cellsNum);
                netPerc += (Perco[cell][(int) soillayers[cell] - 1] - Revap[cell]) / float(cellsNum);
                revap += Revap[cell] / float(cellsNum);
                rs += SURU[cell] / float(cellsNum);
                r += (SURU[cell] + ri) / float(cellsNum);
            }
            float rg = RG[*it];
            r += rg;

            SOWB[*it][0] = pcp;
            SOWB[*it][1] = meanT;
            SOWB[*it][2] = soilT;
            SOWB[*it][3] = netPcp;
            SOWB[*it][4] = itpET;
            SOWB[*it][5] = depET;
            SOWB[*it][6] = infil;
            SOWB[*it][7] = totalET;
            SOWB[*it][8] = es;
            SOWB[*it][9] = netPerc;
            SOWB[*it][10] = revap;
            SOWB[*it][11] = rs;
            SOWB[*it][12] = ri;
            SOWB[*it][13] = rg;
            SOWB[*it][14] = r;
            SOWB[*it][15] = sm;
        }
    }
}

void SOL_WB::Set1DData(const char *key, int nRows, float *data) {
    string s(key);
    if (StringMatch(s, VAR_RG)) {
        RG = data;
        if (nSubbasins != nRows - 1) {
            throw ModelException(MID_SOL_WB, "Set1DData",
                                 "The size of groundwater runoff should be equal to (subbasin number + 1)!");
        }
        return;
    }
    CheckInputSize(key, nRows);
    if (StringMatch(s, VAR_SOILLAYERS)) {
        soillayers = data;
    } else if (StringMatch(s, VAR_SOL_ZMX)) {
        SOL_ZMX = data;
    } else if (StringMatch(s, VAR_NEPR)) {
        NEPR = data;
    } else if (StringMatch(s, VAR_INFIL)) {
        INFIL = data;
    } else if (StringMatch(s, VAR_SOET)) {
        SOET = data;
    } else if (StringMatch(s, VAR_REVAP)) {
        Revap = data;
    } else if (StringMatch(s, VAR_PCP)) {
        D_P = data;
    } else if (StringMatch(s, VAR_INLO)) {
        INLO = data;
    } else if (StringMatch(s, VAR_INET)) {
        INET = data;
    } else if (StringMatch(s, VAR_DEET)) {
        DEET = data;
    } else if (StringMatch(s, VAR_DPST)) {
        DPST = data;
    } else if (StringMatch(s, VAR_SURU)) {
        SURU = data;
    } else if (StringMatch(s, VAR_SNSB)) {
        SNSB = data;
    } else if (StringMatch(s, VAR_TMEAN)) {
        TMEAN = data;
    } else if (StringMatch(s, VAR_SOTE)) {
        SOTE = data;
    } else {
        throw ModelException(MID_SOL_WB, "Set1DData", "Parameter " + s + " does not exist in current module.");
    }
}

void SOL_WB::Set2DData(const char *key, int nrows, int ncols, float **data) {
    string s(key);
    CheckInputSize(key, nrows);
    nSoilLayers = ncols;
    if (StringMatch(s, VAR_PERCO)) {
        Perco = data;
    } else if (StringMatch(s, VAR_SSRU)) {
        SSRU = data;
    } else if (StringMatch(s, VAR_SOL_ST)) {
        solst = data;
    } else if (StringMatch(s, VAR_SOILTHICK)) {
        soilthick = data;
    } else {
        throw ModelException(MID_SOL_WB, "Set2DData", "Parameter " + s + " does not exist in current module.");
    }
}

void SOL_WB::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

void SOL_WB::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string s(key);
    if (StringMatch(s, VAR_SOWB)) {
        setValueToSubbasins();
        *nRows = nSubbasins + 1;
        *nCols = 16;
        *data = SOWB;
    } else {
        throw ModelException(MID_SOL_WB, "Get2DData", "Result " + s + " does not exist in current module.");
    }
}

void SOL_WB::CheckInputData() {
    if (m_date <= 0) throw ModelException(MID_SOL_WB, "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0) {
        throw ModelException(MID_SOL_WB, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (soillayers == NULL) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The soil layers number can not be NULL.");
    }
    if (SOL_ZMX == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The root depth can not be NULL.");
    if (soilthick == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The soil thickness can not be NULL.");
    if (NEPR == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The net precipitation can not be NULL.");
    if (INFIL == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The infiltration can not be NULL.");
    if (SOET == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The evaporation of soil can not be NULL.");
    if (Revap == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The revaporization can not be NULL.");
    if (SSRU == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The subsurface runoff can not be NULL.");
    if (Perco == NULL) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The percolation data can not be NULL.");
    }
    if (solst == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The soil moisture can not be NULL.");
    //if (m_subbasin == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The subbasion can not be NULL.");
    if (D_P == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The precipitation can not be NULL.");
    if (INLO == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The interception can not be NULL.");
    if (DPST == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The depression data can not be NULL.");
    if (DEET == NULL) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The evaporation of depression can not be NULL.");
    }
    if (INET == NULL) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The evaporation of interception can not be NULL.");
    }
    if (RG == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The runoff of groundwater can not be NULL.");
    if (SURU == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The runoff of surface can not be NULL.");
    //if(SNSB == NULL) throw ModelException(MID_SOL_WB,"CheckInputData","The snow sublimation can not be NULL."); /// OPTIONAL
    if (TMEAN == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The mean temperature can not be NULL.");
    //if (m_tMin == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The min temperature can not be NULL.");
    //if (m_tMax == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The max temperature can not be NULL.");
    if (SOTE == NULL) throw ModelException(MID_SOL_WB, "CheckInputData", "The soil temperature can not be NULL.");

    if (nSubbasins <= 0) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) throw ModelException(MID_SOL_WB, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_SOL_WB, "CheckInputData", "The subbasins information can not be NULL.");
    }
}

bool SOL_WB::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_SOL_WB, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) { m_nCells = n; }
        else {
            throw ModelException(MID_SOL_WB, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}