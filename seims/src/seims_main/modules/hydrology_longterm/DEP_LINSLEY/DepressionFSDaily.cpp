#include "seims.h"
#include "DepressionFSDaily.h"

DepressionFSDaily::DepressionFSDaily(void) : m_nCells(-1), Depre_in(NODATA_VALUE),
                                             Depression(NULL),
                                             PET(NULL), INET(NULL), EXCP(NULL),
                                             DPST(NULL), DEET(NULL), SURU(NULL),
                                             impound_trig(NULL), pot_vol(NULL) {
}

DepressionFSDaily::~DepressionFSDaily(void) {
    if (DPST != NULL) Release1DArray(DPST);
    if (DEET != NULL) Release1DArray(DEET);
    if (SURU != NULL) Release1DArray(SURU);
}

bool DepressionFSDaily::CheckInputData(void) {
    if (this->m_date < 0) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData", "You have not set the time.");
    }
    if (this->m_nCells <= 0) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData",
                             "The cell number of the input can not be less than zero.");
    }
    if (Depre_in == NODATA_VALUE) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData",
                             "The parameter: initial depression storage coefficient has not been set.");
    }
    if (Depression == NULL) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData",
                             "The parameter: depression storage capacity has not been set.");
    }
    if (PET == NULL) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData", "The parameter: PET has not been set.");
    }
    if (INET == NULL) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData",
                             "The parameter: evaporation from the interception storage has not been set.");
    }
    if (EXCP == NULL) {
        throw ModelException(MID_DEP_LINSLEY, "CheckInputData",
                             "The parameter: excess precipitation has not been set.");
    }
    return true;
}

void DepressionFSDaily::initialOutputs() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_DEP_LINSLEY, "initialOutputs",
                             "The cell number of the input can not be less than zero.");
    }

    if (DPST == NULL && Depression != NULL) {
        DPST = new float[m_nCells];
        DEET = new float[m_nCells];
        SURU = new float[m_nCells];
#pragma omp parallel for
        for (int i = 0; i < m_nCells; ++i) {
            DPST[i] = Depre_in * Depression[i];
            DEET[i] = 0.f;
            SURU[i] = 0.f;
        }
    }
}

int DepressionFSDaily::Execute() {
    //check the data
    CheckInputData();
    initialOutputs();

#pragma omp parallel for
    for (int i = 0; i < m_nCells; ++i) {
        //////////////////////////////////////////////////////////////////////////
        // runoff
        if (Depression[i] < 0.001f) {
            SURU[i] = EXCP[i];
            DPST[i] = 0.f;
        } else if (EXCP[i] > 0.f) {
            float pc = EXCP[i] - Depression[i] * log(1.f - DPST[i] / Depression[i]);
            float deltaSd = EXCP[i] * exp(-pc / Depression[i]);
            if (deltaSd > Depression[i] - DPST[i]) {
                deltaSd = Depression[i] - DPST[i];
            }
            DPST[i] += deltaSd;
            SURU[i] = EXCP[i] - deltaSd;
        } else {
            DPST[i] += EXCP[i];
            SURU[i] = 0.f;
        }

        //////////////////////////////////////////////////////////////////////////
        // evaporation
        if (DPST[i] > 0) {
            /// TODO: Is this logically right? PET is just potential, which include
            ///       not only ET from surface water, but also from plant and soil.
            ///       Please Check the corresponding theory. By LJ.
            // evaporation from depression storage
            if (PET[i] - INET[i] < DPST[i]) {
                DEET[i] = PET[i] - INET[i];
            } else {
                DEET[i] = DPST[i];
            }
            DPST[i] -= DEET[i];
        } else {
            DEET[i] = 0.f;
            DPST[i] = 0.f;
        }
        if (impound_trig != NULL && FloatEqual(impound_trig[i], 0.f)) {
            if (pot_vol != NULL) {
                pot_vol[i] += SURU[i];
                pot_vol[i] += DPST[i];
                SURU[i] = 0.f;
                DPST[i] = 0.f;
            }
        }
    }
    return true;
}

bool DepressionFSDaily::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        return false;
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_DEP_LINSLEY, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void DepressionFSDaily::SetValue(const char *key, float data) {
    string sk(key);
    if (StringMatch(sk, VAR_DEPREIN)) { Depre_in = data; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
    else {
        throw ModelException(MID_DEP_LINSLEY, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void DepressionFSDaily::Set1DData(const char *key, int n, float *data) {
    //check the input data
    CheckInputSize(key, n);
    string sk(key);
    if (StringMatch(sk, VAR_DEPRESSION)) {
        Depression = data;
    } else if (StringMatch(sk, VAR_INET)) {
        INET = data;
    } else if (StringMatch(sk, VAR_PET)) {
        PET = data;
    } else if (StringMatch(sk, VAR_EXCP)) {
        EXCP = data;
    } else if (StringMatch(sk, VAR_IMPOUND_TRIG)) { impound_trig = data; }
    else if (StringMatch(sk, VAR_POT_VOL)) { pot_vol = data; }
    else {
        throw ModelException(MID_DEP_LINSLEY, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void DepressionFSDaily::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = m_nCells;
    if (StringMatch(sk, VAR_DPST)) {
        *data = DPST;
    } else if (StringMatch(sk, VAR_DEET)) {
        *data = DEET;
    } else if (StringMatch(sk, VAR_SURU)) {
        *data = SURU;
    } else {
        throw ModelException(MID_DEP_LINSLEY, "Get1DData", "Output " + sk + " does not exist.");
    }
}