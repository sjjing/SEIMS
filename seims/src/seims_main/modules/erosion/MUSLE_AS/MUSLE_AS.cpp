#include "seims.h"
#include "MUSLE_AS.h"

MUSLE_AS::MUSLE_AS(void) : m_nCells(-1), CELLWIDTH(-1.f), nsub(-1), nSoilLayers(-1),
                           cellAreaKM(NODATA_VALUE), cellAreaKM1(NODATA_VALUE), cellAreaKM2(NODATA_VALUE),
                           USLE_C(NULL), USLE_P(NULL), USLE_K(NULL), USLE_LS(NULL),
                           acc(NULL), slope(NULL), STREAM_LINK(NULL), slopeForPq(NULL),
                           SNAC(NULL), OL_Flow(NULL),
                           SOER(NULL), sand_yld(NULL), silt_yld(NULL), clay_yld(NULL),
                           sag_yld(NULL), lag_yld(NULL) {
}

MUSLE_AS::~MUSLE_AS(void) {
    if (SOER != NULL) Release1DArray(SOER);
    if (sand_yld != NULL) Release1DArray(sand_yld);
    if (silt_yld != NULL) Release1DArray(silt_yld);
    if (clay_yld != NULL) Release1DArray(clay_yld);
    if (sag_yld != NULL) Release1DArray(sag_yld);
    if (lag_yld != NULL) Release1DArray(lag_yld);
    if (USLE_LS != NULL) Release1DArray(USLE_LS);
    if (slopeForPq != NULL) Release1DArray(slopeForPq);
}

bool MUSLE_AS::CheckInputData(void) {
    if (m_nCells <= 0) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (CELLWIDTH <= 0) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (depRatio == NODATA_VALUE) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The deposition ratio can not be nodata.");
    }
    if (USLE_C == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor C can not be NULL.");
    if (USLE_K == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor K can not be NULL.");
    if (USLE_P == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The factor P can not be NULL.");
    if (acc == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The flow accumulation can not be NULL.");
    }
    if (slope == NULL) throw ModelException(MID_MUSLE_AS, "CheckInputData", "The slope can not be NULL.");
    if (SNAC == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The snow accumulation can not be NULL.");
    }
    if (OL_Flow == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The surface runoff can not be NULL.");
    }
    if (STREAM_LINK == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: STREAM_LINK has not been set.");
    }
    if (det_sand == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: det_sand has not been set.");
    }
    if (det_silt == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: det_silt has not been set.");
    }
    if (det_clay == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: det_clay has not been set.");
    }
    if (det_smagg == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: det_smagg has not been set.");
    }
    if (det_lgagg == NULL) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData", "The parameter: det_lgagg has not been set.");
    }
    return true;
}

void MUSLE_AS::initialOutputs() {
    if (m_nCells <= 0) {
        throw ModelException(MID_MUSLE_AS, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }

    if (SOER == NULL) Initialize1DArray(m_nCells, SOER, 0.f);
    if (sand_yld == NULL) Initialize1DArray(m_nCells, sand_yld, 0.f);
    if (silt_yld == NULL) Initialize1DArray(m_nCells, silt_yld, 0.f);
    if (clay_yld == NULL) Initialize1DArray(m_nCells, clay_yld, 0.f);
    if (sag_yld == NULL) Initialize1DArray(m_nCells, sag_yld, 0.f);
    if (lag_yld == NULL) Initialize1DArray(m_nCells, lag_yld, 0.f);
    if (USLE_LS == NULL) {
        float constant = pow(22.13f, 0.4f);
        USLE_LS = new float[m_nCells];
        slopeForPq = new float[m_nCells];
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++) {
            if (USLE_C[i] < 0.001f) {
                USLE_C[i] = 0.001f;
            }
            if (USLE_C[i] > 1.0f) {
                USLE_C[i] = 1.0f;
            }
            float lambda_i1 = acc[i] * CELLWIDTH;  // m
            float lambda_i = lambda_i1 + CELLWIDTH;
            float L = pow(lambda_i, 1.4f) - pow(lambda_i1, 1.4f); // m^1.4
            L /= CELLWIDTH * constant;  /// m^1.4 / m^0.4 = m

            float S = pow(sin(atan(slope[i])) / 0.0896f, 1.3f);

            USLE_LS[i] = L * S;//equation 3 in memo, LS factor

            slopeForPq[i] = pow(slope[i] * 1000.f, 0.16f);
        }
    }
    if (FloatEqual(cellAreaKM, NODATA_VALUE)) {
        cellAreaKM = pow(CELLWIDTH / 1000.f, 2.f);
        cellAreaKM1 = 3.79f * pow(cellAreaKM, 0.7f);
        cellAreaKM2 = 0.903f * pow(cellAreaKM, 0.017f);
    }
}

float MUSLE_AS::getPeakRunoffRate(int cell) {
    if (OL_Flow[cell] < 0.01f) {
        return 0.f;
    } else {
        return cellAreaKM1 * slopeForPq[cell] *
            pow(OL_Flow[cell] / 25.4f, cellAreaKM2);
    } //equation 2 in memo, peak flow
}

int MUSLE_AS::Execute() {
    CheckInputData();
    initialOutputs();
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        if (OL_Flow[i] < 0.0001f || STREAM_LINK[i] > 0) {
            SOER[i] = 0.f;
        } else {
            float q = getPeakRunoffRate(i); //equation 2 in memo, peak flow
            float Y = 11.8f * pow(OL_Flow[i] * cellAreaKM * 1000.0f * q, 0.56f)
                * USLE_K[i][0] * USLE_LS[i] * USLE_C[i] *
                USLE_P[i];    //equation 1 in memo, sediment yield

            if (SNAC[i] > 0.0001f) {
                Y /= exp(3.f * SNAC[i] / 25.4f);
            }  //equation 4 in memo, the snow pack effect
            SOER[i] = Y * 1000.f; /// kg
        }
        //if(i == 1000) cout << SOER[i] << "," << OL_Flow[i] << endl;
        /// particle size distribution of sediment yield
        sand_yld[i] = SOER[i] * det_sand[i];
        silt_yld[i] = SOER[i] * det_silt[i];
        clay_yld[i] = SOER[i] * det_clay[i];
        sag_yld[i] = SOER[i] * det_smagg[i];
        lag_yld[i] = SOER[i] * det_lgagg[i];
    }
    return 0;
}

bool MUSLE_AS::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_MUSLE_AS, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) { m_nCells = n; }
        else {
            throw ModelException(MID_MUSLE_AS, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void MUSLE_AS::SetValue(const char *key, float data) {
    string sk(key);
    if (StringMatch(sk, Tag_CellWidth)) {
        CELLWIDTH = data;
    } else if (StringMatch(sk, Tag_CellSize)) {
        m_nCells = (int) data;
    } else if (StringMatch(sk, VAR_DEPRATIO)) {
        depRatio = data;
    } else if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) data);
    } else {
        throw ModelException(MID_MUSLE_AS, "SetValue", "Parameter " + sk + " does not exist in current module.");
    }
}

void MUSLE_AS::Set1DData(const char *key, int n, float *data) {

    CheckInputSize(key, n);
    string s(key);
    if (StringMatch(s, VAR_USLE_C)) { USLE_C = data; }
    else if (StringMatch(s, VAR_USLE_P)) { USLE_P = data; }
    else if (StringMatch(s, VAR_ACC)) { acc = data; }
    else if (StringMatch(s, VAR_SLOPE)) {
        slope = data;
        //else if (StringMatch(s, VAR_SUBBSN)) m_subbasin = data;
        //else if (StringMatch(s, VAR_SURU)) OL_Flow = data;
    } else if (StringMatch(s, VAR_OLFLOW)) { OL_Flow = data; }
    else if (StringMatch(s, VAR_SNAC)) { SNAC = data; }
    else if (StringMatch(s, VAR_STREAM_LINK)) { STREAM_LINK = data; }
    else if (StringMatch(s, VAR_DETACH_SAND)) { det_sand = data; }
    else if (StringMatch(s, VAR_DETACH_SILT)) { det_silt = data; }
    else if (StringMatch(s, VAR_DETACH_CLAY)) { det_clay = data; }
    else if (StringMatch(s, VAR_DETACH_SAG)) { det_smagg = data; }
    else if (StringMatch(s, VAR_DETACH_LAG)) { det_lgagg = data; }
    else {
        throw ModelException(MID_MUSLE_AS, "Set1DData", "Parameter " + s + " does not exist.");
    }
}

void MUSLE_AS::Set2DData(const char *key, int nRows, int nCols, float **data) {
    string s(key);
    CheckInputSize(key, nRows);
    nSoilLayers = nCols;
    if (StringMatch(s, VAR_USLE_K)) {
        this->USLE_K = data;
    } else {
        throw ModelException(MID_MUSLE_AS, "Set2DData", "Parameter " + s +
            " does not exist in current module. Please contact the module developer.");
    }
}

void MUSLE_AS::SetSubbasins(clsSubbasins *subbasins) {
    if (nsub < 0) {
        nsub = subbasins->GetSubbasinNumber();
    }
}

void MUSLE_AS::GetValue(const char *key, float *value) {
    string s(key);
    throw ModelException(MID_MUSLE_AS, "GetValue", "Result " + s + " does not exist.");
}

void MUSLE_AS::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SOER)) { *data = SOER; }
    else if (StringMatch(sk, VAR_USLE_LS)) { *data = USLE_LS; }
    else if (StringMatch(sk, VAR_SANDYLD)) { *data = sand_yld; }
    else if (StringMatch(sk, VAR_SILTYLD)) { *data = silt_yld; }
    else if (StringMatch(sk, VAR_CLAYYLD)) { *data = clay_yld; }
    else if (StringMatch(sk, VAR_SAGYLD)) { *data = sag_yld; }
    else if (StringMatch(sk, VAR_LAGYLD)) { *data = lag_yld; }
    else {
        throw ModelException(MID_MUSLE_AS, "Get1DData", "Result " + sk + " does not exist.");
    }
    *n = m_nCells;
}


