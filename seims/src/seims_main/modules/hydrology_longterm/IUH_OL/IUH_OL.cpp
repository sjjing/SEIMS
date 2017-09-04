#include "seims.h"
#include "IUH_OL.h"

using namespace std;

IUH_OL::IUH_OL(void) : TIMESTEP(-1), m_nCells(-1), CELLWIDTH(NODATA_VALUE), cellArea(NODATA_VALUE),
                       nSubbasins(-1), subbasin(NULL), m_subbasinsInfo(NULL),
                       Ol_iuh(NULL), iuhCols(-1), SURU(NULL),
                       cellFlowCols(-1), cellFlow(NULL), OL_Flow(NULL), SBOF(NULL) {
}

IUH_OL::~IUH_OL(void) {
    Release1DArray(SBOF);
    Release2DArray(m_nCells, cellFlow);
    Release1DArray(OL_Flow);
}

bool IUH_OL::CheckInputData(void) {
    if (m_date < 0) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: m_date has not been set.");
    }
    if (m_nCells < 0) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: m_nCells has not been set.");
    }
    if (FloatEqual(CELLWIDTH, NODATA_VALUE)) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: CELLWIDTH has not been set.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }
    if (subbasin == NULL) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: subbasin has not been set.");
    }
    if (nSubbasins <= 0) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) throw ModelException(MID_IUH_OL, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");
    }
    if (Ol_iuh == NULL) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: Ol_iuh has not been set.");
    }
    if (SURU == NULL) {
        throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: surface runoff (SURU) has not been set.");
    }
    //if (m_landcover == NULL)
    //	throw ModelException(MID_IUH_OL, "CheckInputData", "The parameter: landcover has not been set.");
    return true;
}

void IUH_OL::initialOutputs() {
    if (cellArea <= 0.f) cellArea = CELLWIDTH * CELLWIDTH;
    if (this->SBOF == NULL) {
        Initialize1DArray(nSubbasins + 1, SBOF, 0.f);
        for (int i = 0; i < m_nCells; i++) {
            cellFlowCols = max(int(Ol_iuh[i][1] + 1), cellFlowCols);
        }
        //get cellFlowCols, i.e. the maximum of second column of OL_IUH plus 1.
        Initialize2DArray(m_nCells, cellFlowCols, cellFlow, 0.f);
    }
    if (OL_Flow == NULL) {
        Initialize1DArray(m_nCells, OL_Flow, 0.f);
    }
}

int IUH_OL::Execute() {
    this->CheckInputData();
    this->initialOutputs();
    // delete value of last time step
#pragma omp parallel for
    for (int n = 0; n < nSubbasins + 1; n++) {
        SBOF[n] = 0.f;
    }
    //int nt = 0;
    //float qs_cell = 0.f;

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        //forward one time step
        for (int j = 0; j < cellFlowCols; j++) {
            if (j != cellFlowCols - 1) {
                cellFlow[i][j] = cellFlow[i][j + 1];
            } else {
                cellFlow[i][j] = 0.f;
            }
        }

        float v_rs = SURU[i];
        if (v_rs > 0.f) {
            int min = int(this->Ol_iuh[i][0]);
            int max = int(this->Ol_iuh[i][1]);
            int col = 2;
            for (int k = min; k <= max; k++) {
                cellFlow[i][k] += v_rs / 1000.f * Ol_iuh[i][col] * cellArea / TIMESTEP;
                col++;
            }
        }
    }
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        //add today's flow
        int subi = (int) subbasin[i];
        if (nSubbasins == 1) {
            subi = 1;
        } else if (subi >= nSubbasins + 1) {
            throw ModelException(MID_IUH_OL, "Execute", "The subbasin " + ValueToString(subi) + " is invalid.");
        }

        SBOF[subi] += cellFlow[i][0];    //get new value

        OL_Flow[i] = cellFlow[i][0];
        OL_Flow[i] = OL_Flow[i] * TIMESTEP * 1000.f / cellArea;     // m3/s -> mm
        //if(i == 1000) cout << OL_Flow[i] << endl;
    }

    float tmp = 0.f;
#pragma omp parallel for reduction(+:tmp)
    for (int n = 1; n < nSubbasins + 1; n++) {
        tmp += SBOF[n];        //get overland flow routing for entire watershed.
    }
    SBOF[0] = tmp;
    return 0;
}

bool IUH_OL::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_IUH_OL, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_IUH_OL, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void IUH_OL::SetValue(const char *key, float value) {
    string sk(key);

    if (StringMatch(sk, Tag_TimeStep)) { TIMESTEP = (int) value; }
    else if (StringMatch(sk, Tag_CellSize)) { m_nCells = (int) value; }
    else if (StringMatch(sk, Tag_CellWidth)) { CELLWIDTH = value; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else {
        throw ModelException(MID_IUH_OL, "SetValue", "Parameter " + sk + " does not exist in current method.");
    }
}

void IUH_OL::Set1DData(const char *key, int n, float *data) {
    this->CheckInputSize(key, n);
    //set the value
    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN)) { subbasin = data; }
    else if (StringMatch(sk, VAR_SURU)) {
        SURU = data;
        //else if (StringMatch(sk, VAR_LANDCOVER)) m_landcover = data;
    } else {
        throw ModelException(MID_IUH_OL, "Set1DData", "Parameter " + sk + " does not exist in current method.");
    }
}

void IUH_OL::Set2DData(const char *key, int nRows, int nCols, float **data) {

    string sk(key);
    if (StringMatch(sk, VAR_OL_IUH)) {
        this->CheckInputSize(VAR_OL_IUH, nRows);
        Ol_iuh = data;
        iuhCols = nCols;
    } else {
        throw ModelException(MID_IUH_OL, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void IUH_OL::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

void IUH_OL::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SBOF)) {
        *data = this->SBOF;
        *n = this->nSubbasins + 1;
    } else if (StringMatch(sk, VAR_OLFLOW)) {
        *data = this->OL_Flow;
        *n = this->m_nCells;
    } else {
        throw ModelException(MID_IUH_OL, "Get1DData", "Result " + sk + " does not exist.");
    }
}
