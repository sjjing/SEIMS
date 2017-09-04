#include "seims.h"
#include "ClimateParams.h"

#include "Interception_MCS.h"

clsPI_MCS::clsPI_MCS(void) : m_nCells(-1), Pi_b(-1.f), Init_IS(0.f),
                             NEPR(NULL), INLO(NULL), canstor(NULL) {
#ifndef STORM_MODE
    INET = NULL;
#else
    DT_HS = -1.f;
    m_slope = NULL;
#endif
}

clsPI_MCS::~clsPI_MCS(void) {
    if (this->INLO != NULL) Release1DArray(this->INLO);
    if (this->canstor != NULL) Release1DArray(this->canstor);
    if (this->NEPR != NULL) Release1DArray(this->NEPR);
#ifndef STORM_MODE
    if (this->INET != NULL) Release1DArray(this->INET);
#endif
}

void clsPI_MCS::Set1DData(const char *key, int nRows, float *data) {
    this->CheckInputSize(key, nRows);

    string s(key);
    if (StringMatch(s, VAR_PCP)) {
        D_P = data;
    }
    else if (StringMatch(s, VAR_PET)) {
#ifndef STORM_MODE
        PET = data;
#endif
    } else if (StringMatch(s, VAR_INTERC_MAX)) {
        Interc_max = data;
    } else if (StringMatch(s, VAR_INTERC_MIN)) {
        Interc_min = data;
    } else {
        throw ModelException(MID_PI_SVSC, "Set1DData", "Parameter " + s + " does not exist.");
    }
}

void clsPI_MCS::SetValue(const char *key, float data) {
    string s(key);
    if (StringMatch(s, VAR_PI_B)) { this->Pi_b = data; }
    else if (StringMatch(s, VAR_INIT_IS)) { this->Init_IS = data; }
    else if (StringMatch(s, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
#ifdef STORM_MODE
    else if (StringMatch(s, Tag_HillSlopeTimeStep)) { DT_HS = data; }
#endif // STORM_MODE
    else {
        throw ModelException(MID_PI_SVSC, "SetValue", "Parameter " + s + " does not exist.");
    }
}

void clsPI_MCS::Get1DData(const char *key, int *nRows, float **data) {
    initialOutputs();
    string s = key;
    if (StringMatch(s, VAR_INLO)) {
        *data = INLO;
    } else if (StringMatch(s, VAR_INET)) {
#ifndef STORM_MODE
        *data = INET;
#endif
    } else if (StringMatch(s, VAR_CANSTOR)) {
        *data = canstor;
    } else if (StringMatch(s, VAR_NEPR)) {
        *data = NEPR;
    } else {
        throw ModelException(MID_PI_SVSC, "Get1DData", "Result " + s + " does not exist.");
    }
    *nRows = this->m_nCells;
}

void clsPI_MCS::initialOutputs() {
    if (this->canstor == NULL) {
        Initialize1DArray(m_nCells, canstor, Init_IS);
    }
#ifndef STORM_MODE
    if (this->INET == NULL) {
        Initialize1DArray(m_nCells, INET, 0.f);
    }
#endif
    if (this->NEPR == NULL) {
        Initialize1DArray(m_nCells, NEPR, 0.f);
    }
    if (this->INLO == NULL) {
        Initialize1DArray(m_nCells, INLO, 0.f);
    }
}

int clsPI_MCS::Execute() {
    //check input data
    CheckInputData();
    /// initialize outputs
    initialOutputs();

    int julian = JulianDay(m_date);
#pragma omp parallel for
    for (int i = 0; i < this->m_nCells; i++) {
        if (D_P[i] > 0.f) {
#ifdef STORM_MODE
            /// correction for slope gradient, water spreads out over larger area
            D_P[i] = D_P[i] * DT_HS / 3600.f * cos(atan(m_slope[i]));
#endif // STORM_MODE
            //interception storage capacity
            float degree = 2.f * PI * (julian - 87.f) / 365.f;
            /// For water, min and max are both 0, then no need for specific handling.
            float min = Interc_min[i];
            float max = Interc_max[i];
            float capacity = min + (max - min) * pow(0.5f + 0.5f * sin(degree), Pi_b);

            //interception, currently, canstor[i] is storage of (t-1) time step 
            float availableSpace = capacity - canstor[i];
            if (availableSpace < 0) {
                availableSpace = 0.f;
            }

            if (availableSpace < D_P[i]) {
                INLO[i] = availableSpace;
            } else {
                INLO[i] = D_P[i];
            }

            //net precipitation
            NEPR[i] = D_P[i] - INLO[i];
            canstor[i] += INLO[i];
        } else {
            INLO[i] = 0.f;
            NEPR[i] = 0.f;
        }
#ifndef STORM_MODE
        //evaporation
        if (canstor[i] > PET[i]) {
            INET[i] = PET[i];
        } else {
            INET[i] = canstor[i];
        }
        canstor[i] -= INET[i];
#endif    
    }
    return 0;
}

bool clsPI_MCS::CheckInputData() {
    if (this->m_date < 0) {
        throw ModelException(MID_PI_SVSC, "CheckInputData", "You have not set the time.");
    }

    if (m_nCells <= 0) {
        throw ModelException(MID_PI_SVSC, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }

    if (this->D_P == NULL) {
        throw ModelException(MID_PI_SVSC, "CheckInputData", "The precipitation data can not be NULL.");
    }
#ifndef STORM_MODE
    if (this->PET == NULL) {
        throw ModelException(MID_PI_SVSC, "CheckInputData", "The PET data can not be NULL.");
    }
#else
    if (this->m_slope == NULL) {
        throw ModelException(MID_PI_SVSC, "CheckInputData", "The slope gradient can not be NULL.");
    }
    if (this->DT_HS < 0) {
        throw ModelException(MID_PI_SVSC, "CheckInputData", "The Hillslope scale time step must greater than 0.");
    }
#endif
    if (this->Interc_max == NULL) {
        throw ModelException(MID_PI_SVSC, "CheckInputData",
                             "The maximum interception storage capacity can not be NULL.");
    }

    if (this->Interc_min == NULL) {
        throw ModelException(MID_PI_SVSC, "CheckInputData",
                             "The minimum interception storage capacity can not be NULL.");
    }

    if (this->Pi_b > 1.5f || this->Pi_b < 0.5f) {
        throw ModelException(MID_PI_SVSC, "CheckInputData",
                             "The interception storage capacity exponent can not be " + ValueToString(this->Pi_b) +
                                 ". It should between 0.5 and 1.5.");
    }
    if (this->Init_IS > 1.f || this->Init_IS < 0.f) {
        throw ModelException(MID_PI_SVSC, "CheckInputData",
                             "The Initial interception storage can not be " + ValueToString(this->Init_IS) +
                                 ". It should between 0 and 1.");
    }
    return true;
}

bool clsPI_MCS::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_PI_SVSC, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_PI_SVSC, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}
