#include "seims.h"
#include "ClimateParams.h"
#include "PlantGrowthCommon.h"

#include "SNO_SP.h"

SNO_SP::SNO_SP(void) : m_nCells(-1),
                       T0(NODATA_VALUE),
                       K_blow(NODATA_VALUE),
                       T_snow(NODATA_VALUE),
                       lag_snow(NODATA_VALUE),
                       c_snow6(NODATA_VALUE),
                       c_snow12(NODATA_VALUE),
                       SNOCOVMX(NODATA_VALUE),
                       SNO50COV(NODATA_VALUE),
                       snowCoverCoef1(NODATA_VALUE),
                       snowCoverCoef2(NODATA_VALUE),
                       NEPR(NULL),
                       TMEAN(NULL),
                       TMAX(NULL),
                       SE(NULL),
                       SNAC(NULL),
                       SNME(NULL),
                       SA(NULL),
                       packT(NULL) {//m_swe(NODATA_VALUE), m_lastSWE(NODATA_VALUE), m_swe0(NODATA_VALUE),
}

SNO_SP::~SNO_SP(void) {
    if (this->SNME != NULL) Release1DArray(this->SNME);
    if (this->SA != NULL) Release1DArray(this->SA);
    if (this->packT != NULL) Release1DArray(this->packT);
}

bool SNO_SP::CheckInputData(void) {
    if (this->m_date <= 0) throw ModelException(MID_SNO_SP, "CheckInputData", "You have not set the time.");
    if (this->m_nCells <= 0) {
        throw ModelException(MID_SNO_SP, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (this->T0 == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The Snow melt temperature can not be NODATA.");
    }
    if (this->K_blow == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData",
                             "The fraction coefficient of precipitation as snow can not be NODATA.");
    }
    //if (this->m_swe == NODATA_VALUE) throw ModelException(MID_SNO_SP, "CheckInputData", "The average snow accumulation of watershed can not be NODATA.");
    //if (this->m_swe0 == NODATA_VALUE) throw ModelException(MID_SNO_SP, "CheckInputData", "The swe0 can not be NODATA.");
    if (this->T_snow == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The snow fall temperature can not be NODATA.");
    }
    if (this->lag_snow == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The lag snow can not be NODATA.");
    }
    if (this->c_snow6 == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The csnow6 can not be NODATA.");
    }
    if (this->c_snow12 == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The csnow12 can not be NODATA.");
    }
    if (this->SNOCOVMX == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The snowCoverMax can not be NODATA.");
    }
    if (this->SNO50COV == NODATA_VALUE) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The snowCover50 can not be NODATA.");
    }
    if (this->TMEAN == NULL) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The mean temperature data can not be NULL.");
    }
    if (this->TMAX == NULL) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The max temperature data can not be NULL.");
    }
    if (this->NEPR == NULL) {
        throw ModelException(MID_SNO_SP, "CheckInputData", "The net precipitation data can not be NULL.");
    }
    //if (this->SE == NULL) throw ModelException(MID_SNO_SP, "CheckInputData", "The snow sublimation data can not be NULL.");
    return true;
}

void SNO_SP::initialOutputs() {
    if (m_nCells <= 0) {
        throw ModelException(MID_SNO_SP, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (SNME == NULL) Initialize1DArray(m_nCells, SNME, 0.f);
    if (SA == NULL) Initialize1DArray(m_nCells, SA, 0.f);
    if (packT == NULL) Initialize1DArray(m_nCells, packT, 0.f);
    //if (this->m_swe == NODATA_VALUE)
    //	this->m_swe = this->m_swe0;
    //if (this->m_lastSWE == NODATA_VALUE)
    //	this->m_lastSWE = this->m_swe;
    if (SNAC == NULL) {  /// the initialization should be removed when snow redistribution module is accomplished. LJ
        Initialize1DArray(m_nCells, SNAC, 0.f);
    }
    if (SE == NULL) { /// Snow sublimation will be considered in AET_PTH
        Initialize1DArray(m_nCells, SE, 0.f);
    }
}

int SNO_SP::Execute() {
    this->CheckInputData();
    this->initialOutputs();
    /// determine the shape parameters for the equation which describes area of
    /// snow cover as a function of amount of snow
    if (snowCoverCoef1 == NODATA_VALUE || snowCoverCoef2 == NODATA_VALUE) {
        PGCommon::getScurveShapeParameter(0.5f, 0.95f, SNO50COV, 0.95f, &snowCoverCoef1, &snowCoverCoef2);
    }
    //  if (this->m_lastSWE == NODATA_VALUE) /// moved to initialOutputs()
    //this->m_lastSWE = this->m_swe;
    //If all cells have no snow, snow melt is set to zero.
    //if (m_swe < 0.01)
    //{
    //    if (this->m_lastSWE >= 0.01)
    //    {
    //        for (int rw = 0; rw < this->m_nCells; rw++)
    //            SNME[rw] = 0.f;
    //    }
    //    this->m_lastSWE = this->m_swe;

    //    return 0;
    //}

    /// adjust melt factor for time of year, i.e., smfac in snom.f
    // which only need to computed once.
    int d = JulianDay(this->m_date);
    float sinv = float(sin(2.f * PI / 365.f * (d - 81.f)));
    float cmelt = (c_snow6 + c_snow12) / 2.f + (c_snow6 - c_snow12) / 2.f * sinv;
#pragma omp parallel for
    for (int rw = 0; rw < m_nCells; rw++) {
        /// estimate snow pack temperature
        packT[rw] = packT[rw] * (1 - lag_snow) + TMEAN[rw] * lag_snow;
        /// calculate snow fall
        SA[rw] = SA[rw] + SNAC[rw] - SE[rw];
        if (TMEAN[rw] < T_snow) /// precipitation will be snow
        {
            SA[rw] += K_blow * NEPR[rw];
            NEPR[rw] *= (1.f - K_blow);
        }

        if (SA[rw] < 0.01) {
            SNME[rw] = 0.f;
        } else {
            float dt = TMAX[rw] - T0;
            if (dt < 0) {
                SNME[rw] = 0.f;  //if temperature is lower than t0, the snowmelt is 0.
            } else {
                //calculate using eq. 1:2.5.2 SWAT p58
                SNME[rw] = cmelt * ((packT[rw] + TMAX[rw]) / 2.f - T0);
                // adjust for areal extent of snow cover
                float snowCoverFrac = 0.f; //fraction of HRU area covered with snow
                if (SA[rw] < SNOCOVMX) {
                    float xx = SA[rw] / SNOCOVMX;
                    snowCoverFrac = xx / (xx + exp(snowCoverCoef1 = snowCoverCoef2 * xx));
                } else {
                    snowCoverFrac = 1.f;
                }
                SNME[rw] *= snowCoverFrac;
                if (SNME[rw] < 0.f) SNME[rw] = 0.f;
                if (SNME[rw] > SA[rw]) SNME[rw] = SA[rw];
                SA[rw] -= SNME[rw];
                NEPR[rw] += SNME[rw];
                if (NEPR[rw] < 0.f) NEPR[rw] = 0.f;
            }
        }
    }
    //this->m_lastSWE = this->m_swe;
    return 0;
}

bool SNO_SP::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_SNO_SP, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_SNO_SP, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

void SNO_SP::SetValue(const char *key, float data) {
    string s(key);
    if (StringMatch(s, VAR_K_BLOW)) { this->K_blow = data; }
    else if (StringMatch(s, VAR_T0)) { this->T0 = data; }
    else if (StringMatch(s, VAR_T_SNOW)) {
        this->T_snow = data;
        //else if (StringMatch(s, VAR_SWE)) this->m_swe = data;
        //else if (StringMatch(s, VAR_SWE0)) this->m_swe0 = data;
    } else if (StringMatch(s, VAR_LAG_SNOW)) { this->lag_snow = data; }
    else if (StringMatch(s, VAR_C_SNOW6)) { this->c_snow6 = data; }
    else if (StringMatch(s, VAR_C_SNOW12)) { this->c_snow12 = data; }
    else if (StringMatch(s, VAR_SNOCOVMX)) { this->SNOCOVMX = data; }
    else if (StringMatch(s, VAR_SNO50COV)) { this->SNO50COV = data; }
    else if (StringMatch(s, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
    else {
        throw ModelException(MID_SNO_SP, "SetValue", "Parameter " + s
            +
                " does not exist in current module. Please contact the module developer.");
    }
}

void SNO_SP::Set1DData(const char *key, int n, float *data) {
    //check the input data
    string s(key);
    this->CheckInputSize(key, n);
    if (StringMatch(s, VAR_TMEAN)) { this->TMEAN = data; }
    else if (StringMatch(s, VAR_TMAX)) { this->TMAX = data; }
    else if (StringMatch(s, VAR_NEPR)) {
        this->NEPR = data;
        //else if (StringMatch(s, VAR_SNRD)) this->SNAC = data;
        //else if (StringMatch(s, VAR_SNSB)) this->SE = data;
    } else {
        throw ModelException(MID_SNO_SP, "Set1DData", "Parameter " + s +
            " does not exist in current module. Please contact the module developer.");
    }
}

void SNO_SP::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string s(key);
    if (StringMatch(s, VAR_SNME)) { *data = this->SNME; }
    else if (StringMatch(s, VAR_SNAC)) { *data = this->SNAC; }
    else {
        throw ModelException(MID_SNO_SP, "Get1DData", "Result " + s
            +
                " does not exist in current module. Please contact the module developer.");
    }
    *n = this->m_nCells;
}
