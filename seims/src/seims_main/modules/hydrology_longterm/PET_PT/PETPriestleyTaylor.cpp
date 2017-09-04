#include "seims.h"
#include "ClimateParams.h"

#include "PETPriestleyTaylor.h"

using namespace std;

PETPriestleyTaylor::PETPriestleyTaylor(void) : TMIN(NULL), TMAX(NULL), SR(NULL), RM(NULL), DEM(NULL),
                                               PHU0(NULL),
                                               daylength(NULL), PHUBASE(NULL), PET(NULL), VPD(NULL),
                                               K_pet(1.f), m_nCells(-1) {
}

PETPriestleyTaylor::~PETPriestleyTaylor(void) {
    if (this->daylength != NULL) Release1DArray(this->daylength);
    if (this->PHUBASE != NULL) Release1DArray(this->PHUBASE);
    if (this->PET != NULL) Release1DArray(this->PET);
    if (this->VPD != NULL) Release1DArray(this->VPD);
}

void PETPriestleyTaylor::Get1DData(const char *key, int *n, float **data) {
    // CheckInputData();// Plz avoid putting CheckInputData() in Get1DData, this may cause Set time error! By LJ
    initialOutputs();
    string sk(key);
    *n = this->m_nCells;
    if (StringMatch(sk, VAR_DAYLEN)) { *data = this->daylength; }
    else if (StringMatch(sk, VAR_VPD)) { *data = this->VPD; }
    else if (StringMatch(sk, VAR_PHUBASE)) { *data = this->PHUBASE; }
    else if (StringMatch(sk, VAR_PET)) { *data = this->PET; }
    else {
        throw ModelException(MID_PET_PT, "Get1DData",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
    }
}

bool PETPriestleyTaylor::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_PET_PT, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_PET_PT, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

bool PETPriestleyTaylor::CheckInputData() {
    if (this->m_date < 0) {
        throw ModelException(MID_PET_PT, "CheckInputData", "You have not set the time.");
    }
    if (m_nCells <= 0) {
        throw ModelException(MID_PET_PT, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (this->DEM == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The elevation can not be NULL.");
    }
    if (this->celllat == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The latitude can not be NULL.");
    }
    if (this->RM == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The relative humidity can not be NULL.");
    }
    if (this->SR == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The solar radiation can not be NULL.");
    }
    if (this->TMIN == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The min temperature can not be NULL.");
    }
    if (this->TMAX == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The max temperature can not be NULL.");
    }
    if (this->TMEAN == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The mean temperature can not be NULL.");
    }
    if (this->PHU0 == NULL) {
        throw ModelException(MID_PET_PT, "CheckInputData", "The PHU0 can not be NULL.");
    }
    return true;
}

void PETPriestleyTaylor::initialOutputs() {
    if (this->PET == NULL) Initialize1DArray(m_nCells, PET, 0.f);
    if (this->VPD == NULL) Initialize1DArray(m_nCells, VPD, 0.f);
    if (this->daylength == NULL) Initialize1DArray(m_nCells, daylength, 0.f);
    if (this->PHUBASE == NULL) Initialize1DArray(m_nCells, PHUBASE, 0.f);
}

int PETPriestleyTaylor::Execute() {
    CheckInputData();
    initialOutputs();
    jday = JulianDay(this->m_date);
#pragma omp parallel for
    for (int i = 0; i < m_nCells; ++i) {
        /// update phubase of the simulation year.
        /* update base zero total heat units, src code from SWAT, subbasin.f
        if (tmpav(j) > 0. .and. phutot(hru_sub(j)) > 0.01) then
            phubase(j) = phubase(j) + tmpav(j) / phutot(hru_sub(j))
        end if*/
        if (jday == 1) PHUBASE[i] = 0.f;
        if (TMEAN[i] > 0.f && PHU0[i] > 0.01f) {
            PHUBASE[i] += TMEAN[i] / PHU0[i];
        }
        /// compute net radiation
        /// net short-wave radiation for PET, etpot.f in SWAT src
        float raShortWave = SR[i] * (1.0f - 0.23f);
        //if the mean T < T_snow, consider the snow depth is larger than 0.5mm.
        if (TMEAN[i] < this->T_snow) {
            raShortWave = SR[i] * (1.0f - 0.8f);
        }

        /// calculate the max solar radiation
        MaxSolarRadiation(jday, this->celllat[i], this->daylength[i], srMax);

        /// calculate net long-wave radiation
        /// net emissivity  equation 2.2.20 in SWAT manual
        float satVaporPressure = SaturationVaporPressure(TMEAN[i]);
        float actualVaporPressure = 0.f;
        if (RM[i] > 1) {   /// IF percent unit.
            RM[i] *= 0.01f;
        } else if (RM[i] < UTIL_ZERO) {
            RM[i] = UTIL_ZERO;
        }
        actualVaporPressure = RM[i] * satVaporPressure;
        if (actualVaporPressure < UTIL_ZERO) {
            actualVaporPressure = UTIL_ZERO;
        }
        VPD[i] = satVaporPressure - actualVaporPressure;
        if (VPD[i] < 0.f) {
            VPD[i] = 0.f;
        }
        float rbo = -(0.34f - 0.139f * sqrt(actualVaporPressure));
        //cloud cover factor
        float rto = 0.0f;
        if (srMax >= 1.0e-4f) {
            rto = 0.9f * (SR[i] / srMax) + 0.1f;
        }
        //net long-wave radiation
        float tk = TMEAN[i] + 273.15f;
        float raLongWave = rbo * rto * 4.9e-9f * pow(tk, 4.f);

        float raNet = raShortWave + raLongWave;

        //////////////////////////////////////////////////////////////////////////
        //calculate the slope of the saturation vapor pressure curve
        float dlt = 4098.f * satVaporPressure / pow((TMEAN[i] + 237.3f), 2.f);

        //calculate latent heat of vaporization(MJ/kg, from swat)
        float latentHeat = LatentHeatVapor(TMEAN[i]);

        //calculate mean barometric pressure(kPa)
        float pb = MeanBarometricPressure(DEM[i]);
        //psychrometric constant(kPa/deg C)
        float gma = 1.013e-3f * pb / (0.622f * latentHeat);
        float pet_alpha = 1.28f;

        float petValue = pet_alpha * (dlt / (dlt + gma)) * raNet / latentHeat;
        PET[i] = K_pet * max(0.f, petValue);
        if (PET[i] != PET[i]) {
            cout << "cell id: " << i << ", pet: " << PET[i] << ", meanT: " << TMEAN[i] <<
                ", rhd: " << RM[i] << ", rbo: " << rbo << ", sr: "<< SR[i] << ", srMax: "
                << srMax << ", rto: " << rto << ", satVaporPressure: " << satVaporPressure << endl;
            throw ModelException(MID_PET_PT, "Execute", "Calculation error occurred!\n");
        }
    }
    return 0;
}

void PETPriestleyTaylor::Set1DData(const char *key, int n, float *value) {
    if (!this->CheckInputSize(key, n)) return;
    string sk(key);
    if (StringMatch(sk, DataType_MeanTemperature)) { this->TMEAN = value; }
    else if (StringMatch(sk, DataType_MinimumTemperature)) { this->TMIN = value; }
    else if (StringMatch(sk, DataType_MaximumTemperature)) { this->TMAX = value; }
    else if (StringMatch(sk, DataType_RelativeAirMoisture)) { this->RM = value; }
    else if (StringMatch(sk, DataType_SolarRadiation)) { this->SR = value; }
    else if (StringMatch(sk, VAR_DEM)) { this->DEM = value; }
    else if (StringMatch(sk, VAR_CELL_LAT)) { this->celllat = value; }
    else if (StringMatch(sk, VAR_PHUTOT)) { this->PHU0 = value; }
    else {
        throw ModelException(MID_PET_PT, "Set1DData", "Parameter " + sk +
            " does not exist in current module. Please contact the module developer.");
    }
}

void PETPriestleyTaylor::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_T_SNOW)) { this->T_snow = value; }
    else if (StringMatch(sk, VAR_K_PET)) { K_pet = value; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else {
        throw ModelException(MID_PET_PT, "SetValue", "Parameter " + sk +
            " does not exist in current module. Please contact the module developer.");
    }
}
