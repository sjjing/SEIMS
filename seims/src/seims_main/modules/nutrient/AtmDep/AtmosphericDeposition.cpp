#include "seims.h"
#include "AtmosphericDeposition.h"

using namespace std;

AtmosphericDeposition::AtmosphericDeposition(void) :
//input
    m_nCells(-1), rcn(-1.f), rca(-1.f), soiLayers(-1),
    D_P(NULL), drydep_no3(-1.f), drydep_nh4(-1.f),
    addrno3(-1.f), addrnh4(-1.f),
    //output
    sol_no3(NULL), sol_nh4(NULL), wshd_rno3(-1.f) {
}

AtmosphericDeposition::~AtmosphericDeposition(void) {
}

bool AtmosphericDeposition::CheckInputData(void) {
    if (m_nCells <= 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData",
                             "The cell number of the input can not be less than zero.");
    }
    if (this->soiLayers < 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The maximum soil layers number can not be less than 0.");
    }
    //if (this->m_cellWidth < 0)
    //    throw ModelException(MID_ATMDEP, "CheckInputData", "The rca can not be less than 0.");
    if (this->rcn < 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The rca can not be less than 0.");
    }
    if (this->rca < 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The rca can not be less than 0.");
    }
    if (this->D_P == NULL) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The precipitation can not be NULL.");
    }
    if (this->drydep_no3 < 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The drydep_no3 can not be less than 0.");
    }
    if (this->drydep_nh4 < 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The drydep_nh4 can not be less than 0.");
    }
    if (this->sol_no3 == NULL) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The sol_no3 can not be NULL.");
    }
    if (this->sol_nh4 == NULL) {
        throw ModelException(MID_ATMDEP, "CheckInputData", "The sol_nh4 can not be NULL.");
    }
    return true;
}

bool AtmosphericDeposition::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_ATMDEP, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_ATMDEP, "CheckInputSize",
                                 "Input data for " + string(key) + " is invalid with size: " + ValueToString(n) +
                                     ". The origin size is "
                                     + ValueToString(m_nCells) + ".\n");
        }
    }
    return true;
}

void AtmosphericDeposition::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_PCP)) { D_P = data; }
    else {
        throw ModelException(MID_ATMDEP, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void AtmosphericDeposition::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) value);
        //else if (StringMatch(sk, Tag_CellSize)) { m_nCells = value; }
        //else if (StringMatch(sk, Tag_CellWidth)) { m_cellWidth = value; }
    } else if (StringMatch(sk, VAR_RCN)) { rcn = value; }
    else if (StringMatch(sk, VAR_RCA)) { rca = value; }
    else if (StringMatch(sk, VAR_DRYDEP_NO3)) { drydep_no3 = value; }
    else if (StringMatch(sk, VAR_DRYDEP_NH4)) { drydep_nh4 = value; }
    else {
        throw ModelException(MID_ATMDEP, "SetValue",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
    }
}

void AtmosphericDeposition::Set2DData(const char *key, int nRows, int nCols, float **data) {
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);
    soiLayers = nCols;
    if (StringMatch(sk, VAR_SOL_NO3)) { this->sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_NH4)) { this->sol_nh4 = data; }
    else {
        throw ModelException(MID_ATMDEP, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void AtmosphericDeposition::initialOutputs() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_ATMDEP, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    // allocate the output variables
    if (addrnh4 < 0.f) {
        addrnh4 = 0.f;
        addrno3 = 0.f;
    }
    /// initialize wshd_rno3 to 0.f at each time step
    if (!FloatEqual(wshd_rno3, 0.f)) wshd_rno3 = 0.f;
}

int AtmosphericDeposition::Execute() {
    //check the data
    this->CheckInputData();
    this->initialOutputs();
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        if (D_P[i] > 0.f) {
            /// Calculate the amount of nitrite and ammonia added to the soil in rainfall
            /// unit conversion: mg/L * mm = 0.01 * kg/ha (CHECKED)
            addrno3 = 0.01f * rcn * D_P[i];
            addrnh4 = 0.01f * rca * D_P[i];
            sol_no3[i][0] += (addrno3 + drydep_no3 / 365.f);
            sol_nh4[i][0] += (addrnh4 + drydep_nh4 / 365.f);
            wshd_rno3 += (addrno3 * (1.f / m_nCells));
        }
    }
    return 0;
}

void AtmosphericDeposition::GetValue(const char *key, float *value) {
    string sk(key);
    if (StringMatch(sk, VAR_WSHD_RNO3)) { *value = wshd_rno3; }
    else {
        throw ModelException(MID_ATMDEP, "GetValue", "Parameter " + sk + " does not exist.");
    }
}