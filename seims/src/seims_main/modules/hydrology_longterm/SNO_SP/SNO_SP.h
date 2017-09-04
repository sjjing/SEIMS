/*!
 * \brief Snow melt by snowpack daily method from SWAT
 * \author Zhiqiang Yu
 * \date May 2011
 * \revised LiangJun Zhu
 * \date 2016-5-29
 *  1. Remove m_isInitial and add initialOutputs(void)
 *  2. Add SNOCOVMX and SNO50COV to adjust for areal extent of snow cover.
 *  3. ReWrite the execute code according to snom.f of SWAT.
 *  4. In this version, snow melt is added to net precipitation.
 * 
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/*!
 * \defgroup SNO_SP
 * \ingroup Hydrology_longterm
 * \brief Calculate snow melt by snowpack daily method from SWAT
 *
 */

/*!
 * \class SNO_SP
 * \ingroup SNO_SP
 * \brief Calculate snow melt by snowpack daily method from SWAT
 * 
 */
class SNO_SP : public SimulationModule {
public:
    //! Constructor
    SNO_SP(void);

    //! Destructor
    ~SNO_SP(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

    void initialOutputs(void);

    // @In
    // @Description Valid cells number
    int m_nCells;

    // @In
    // @Description Mean air temperature at which snow melt will occur, sub_smtmp
    float T0;

    // @In
    // @Description fraction coefficient of precipitation as snow
    float K_blow;

    // @In
    // @Description Snowfall temperature, i.e., precipitation as snow
    float T_snow;

    // @In
    // @Description Snow pack temperature lag factor (0-1)
    float lag_snow;

    // @In
    // @Description Maximum melt rate for snow during year, sub_smfmx
    float c_snow6;

    // @In
    // @Description Minimum melt rate for snow during year, sub_smfmn
    float c_snow12;

    // @In
    // @Description Minimum snow water content that corresponds to 100% snow cover, mm H2O, SNOCOVMX
    float SNOCOVMX;

    // @In
    // @Description Fraction of SNOCOVMX that corresponds to 50% snow cover, SNO50COV
    float SNO50COV;

    // @In
    // @Description 1st shape parameter for snow cover equation
    float snowCoverCoef1;

    // @In
    // @Description 2nd shape parameter for snow cover equation
    float snowCoverCoef2;

    // @In
    // @Description Mean temperature
    float *TMEAN;

    // @In
    // @Description Max temperature
    float *TMAX;

    // @In
    // @Description Net precipitation
    float *NEPR;

    // @In
    // @Description snow redistribution
    float *SNAC;

    // @In
    // @Description snow sublimation, snoev in SWAT in etact.f
    float *SE;

    // @In
    // @Description temperature of snow pack, snotmp in SWAT
    float *packT;

    /// outputs

    // @Out
    // @Description amount of water in snow melt, snomlt in SWAT
    float *SNME;

    // @Out
    // @Description snow accumulation, sno_hru in SWAT
    float *SA;
};

VISITABLE_STRUCT(SNO_SP, m_nCells, T0, K_blow, T_snow, lag_snow, c_snow6, c_snow12, SNOCOVMX, SNO50COV, snowCoverCoef1, snowCoverCoef2,
	TMEAN, TMAX, NEPR, SNAC, SE, packT, SNME, SA);