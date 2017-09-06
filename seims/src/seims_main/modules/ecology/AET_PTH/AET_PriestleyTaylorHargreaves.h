/*!
 * \brief Potential plant transpiration for Priestley-Taylor and Hargreaves ET methods
 * and potential and actual soil evaporation.
 * Code from SWAT, etact.f
 * \author Liang-Jun Zhu
 * \date May 2016
 *
 * \revision: 1.1
 * \date: 2016-7-15
 * \description: 1. Code reformat with common functions, such as Release1DArray.
 *               2. VAR_SNSB should be output other than input.
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup AET_PTH
 * \ingroup Ecology
 * \brief Potential plant transpiration for Priestley-Taylor and Hargreaves ET methods
 *Actual soil evaporation is also calculated.
 */
/*!
 * \class AET_PT_H
 * \ingroup AET_PTH
 *
 * \brief Potential plant transpiration for Priestley-Taylor and Hargreaves ET methods
 * Actual soil evaporation is also calculated.
 */
class AET_PT_H : public SimulationModule {

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description leaf area index(m^2/m^2)
    float *LAIDAY;

    // @In
    // @Description potential evapotranspiration on current day
    float *PET;

    // @In
    // @Description Evaporation loss from canopy storage
    float *INET;

    // @In
    // @Description soil evaporation compensation factor, if not set or existed, it will be assigned 0.95 as default£¬should be vary from 0.01 to 1.0
    float *esco;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description maximum soil layers, mlyr in SWAT
    int nSoilLayers;

    // @In
    // @Description soil depth
    float **soilDepth;

    // @In
    // @Description soil thickness
    float **soilthick;

    // @In
    // @Description amount of water available to plants in soil layer at field capacity (FC-WP)
    float **sol_awc;

    // @In
    // @Description amount of residue on soil surface (kg/ha)
    float *sol_cov;

    // @In
    // @Description amount of nitrogen stored in the nitrate pool
    float **sol_no3;

    // @In
    // @Description mean air temperature (deg C)
    float *TMEAN;

    // @In
    // @Description amount of water in snow on current day
    float *SNAC;

    // @In
    // @Description snow sublimation on current day
    float *SNSB;

    // @In
    // @Description soil storage of each soil layer, mm H2O
    float **solst;

    // @In
    // @Description soil water storage in soil profile (mm)
    float *solsw;

    /// add output variables

    // @Out
    // @Description maximum amount of transpiration (plant et)  that can occur on current day in HRU, ep_max in SWAT
    float *PPT;

    // @Out
    // @Description actual amount of evaporation (soil et) that occurs on day, es_day in SWAT
    float *SOET;

    // @Out
    // @Description amount of nitrate moving upward in the soil profile in watershed
    float sno3up;    

    // not used
    // depression storage capacity
    // float *m_depSt;

public:
    //! Constructor
    AET_PT_H(void);

    //! Destructor
    ~AET_PT_H(void);

    virtual int Execute(void);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Set2DData(const char *key, int n, int col, float **data);

    virtual void GetValue(const char *key, float *value);

private:
    /*!
     * \brief check the input data. Make sure all the input data is available.
     * \return bool The validity of the input data.
     */
    bool CheckInputData(void);

    /*!
     * \brief check the input size. Make sure all the input data have same dimension.
     *
     *
     * \param[in] key The key of the input data
     * \param[in] n The input data dimension
     * \return bool The validity of the dimension
     */
    bool CheckInputSize(const char *, int);

    //! initialize outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(AET_PT_H, m_nCells, LAIDAY, PET, INET, esco, soillayers, soilLayers, soilDepth, soilthick,
	sol_awc, sol_cov, sol_no3, TMEAN, SNAC, SNSB, solst, solsw, PPT, SOET, sno3up);