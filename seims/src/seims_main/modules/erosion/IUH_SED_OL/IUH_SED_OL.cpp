#include "seims.h"
#include "IUH_SED_OL.h"

using namespace std;

IUH_SED_OL::IUH_SED_OL(void) : TIMESTEP(-1), m_nCells(-1), CELLWIDTH(NODATA_VALUE), cellArea(NODATA_VALUE),
                               nSubbasins(-1), subbasin(NULL), m_subbasinsInfo(NULL),
                               Ol_iuh(NULL), iuhCols(-1), cellFlowCols(-1),
                               SOER(NULL), cellSed(NULL), SEDTOCH(NULL), SED_OL(NULL) {
}

IUH_SED_OL::~IUH_SED_OL(void) {
    Release1DArray(SEDTOCH);
    Release2DArray(m_nCells, cellSed);
}

bool IUH_SED_OL::CheckInputData(void) {
    if (m_date < 0) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: m_date has not been set.");
    }
    if (m_nCells < 0) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: m_nCells has not been set.");
    }
    if (FloatEqual(CELLWIDTH, NODATA_VALUE)) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: CELLWIDTH has not been set.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }
    if (subbasin == NULL) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: subbasin has not been set.");
    }
    if (nSubbasins <= 0) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    }
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");
    }
    if (Ol_iuh == NULL) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputData", "The parameter: Ol_iuh has not been set.");
    }
    return true;
}

void IUH_SED_OL::initialOutputs() {
    if (cellArea <= 0.f) cellArea = CELLWIDTH * CELLWIDTH;
    if (SEDTOCH == NULL) {
        Initialize1DArray(nSubbasins + 1, SEDTOCH, 0.f);
        Initialize1DArray(m_nCells, SED_OL, 0.f);

        for (int i = 0; i < m_nCells; i++) {
            cellFlowCols = max(int(Ol_iuh[i][1] + 1), cellFlowCols);
        }
        //get cellFlowCols, i.e. the maximum of second column of OL_IUH plus 1.

        //cellSed = new float *[m_nCells];
        Initialize2DArray(m_nCells, cellFlowCols, cellSed, 0.f);
    }
}

int IUH_SED_OL::Execute() {
    CheckInputData();
    initialOutputs();
    // delete value of last time step
#pragma omp parallel for
    for (int i = 0; i < nSubbasins + 1; i++) {
        SEDTOCH[i] = 0.f;
    }

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        //forward one time step
        for (int j = 0; j < cellFlowCols; j++) {
            if (j != cellFlowCols - 1) {
                cellSed[i][j] = cellSed[i][j + 1];
            } else {
                cellSed[i][j] = 0.f;
            }
        }

        if (SOER[i] > 0.f) {
            int min = int(this->Ol_iuh[i][0]);
            int max = int(this->Ol_iuh[i][1]);
            int col = 2;
            for (int k = min; k <= max; k++) {
                cellSed[i][k] += SOER[i] * Ol_iuh[i][col];
                col++;
            }
        }
    }

    for (int i = 0; i < m_nCells; i++) {
        //add today's flow
        int subi = (int) subbasin[i];
        if (nSubbasins == 1) {
            subi = 1;
        } else if (subi >= nSubbasins + 1) {
            throw ModelException(MID_IUH_SED_OL, "Execute", "The subbasin " + ValueToString(subi) + " is invalid.");
        }
        SEDTOCH[subi] += cellSed[i][0];
        SED_OL[i] = cellSed[i][0];
        //if(i == 1852) cout << cellSed[i][0] << endl;
    }
    //cout << SEDTOCH[1] << endl;
    float tmp = 0.f;
#pragma omp parallel for reduction(+:tmp)
    for (int i = 1; i < nSubbasins + 1; i++) {
        tmp += SEDTOCH[i];        //get overland flow routing for entire watershed.
    }
    SEDTOCH[0] = tmp;

    return 0;
}

bool IUH_SED_OL::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_IUH_SED_OL, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_IUH_SED_OL, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void IUH_SED_OL::SetValue(const char *key, float value) {
    string sk(key);

    if (StringMatch(sk, Tag_TimeStep)) { TIMESTEP = (int) value; }
    else if (StringMatch(sk, Tag_CellSize)) { m_nCells = (int) value; }
    else if (StringMatch(sk, Tag_CellWidth)) { CELLWIDTH = value; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else {
        throw ModelException(MID_IUH_SED_OL, "SetValue", "Parameter " + sk + " does not exist in current method.");
    }
}

void IUH_SED_OL::Set1DData(const char *key, int n, float *data) {
    this->CheckInputSize(key, n);
    //set the value
    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN)) {
        subbasin = data;
    } else if (StringMatch(sk, VAR_SOER)) {
        SOER = data;
    } else {
        throw ModelException(MID_IUH_SED_OL, "Set1DData", "Parameter " + sk + " does not exist in current method.");
    }
}

void IUH_SED_OL::Set2DData(const char *key, int nRows, int nCols, float **data) {
    string sk(key);
    if (StringMatch(sk, VAR_OL_IUH)) {
        this->CheckInputSize(VAR_OL_IUH, nRows);
        Ol_iuh = data;
        iuhCols = nCols;
    } else {
        throw ModelException(MID_IUH_SED_OL, "Set2DData", "Parameter " + sk + " does not exist in current method.");
    }
}

void IUH_SED_OL::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

void IUH_SED_OL::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SED_TO_CH)) {
        *data = SEDTOCH;   // from each subbasin to channel
        *n = nSubbasins + 1;
        return;
    } else if (StringMatch(sk, VAR_SEDYLD)) {
        *data = SED_OL;
        *n = m_nCells;
    } else {
        throw ModelException(MID_IUH_SED_OL, "Get1DData", "Result " + sk + " does not exist.");
    }
}
