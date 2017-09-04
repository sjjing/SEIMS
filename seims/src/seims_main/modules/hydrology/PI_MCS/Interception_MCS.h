/*!
 * \brief Precipitation interception based on Maximum Canopy Storage
 *        Equation given by Aston(1979), Code adapted from openLISEM
 *        file: lisInterception.cpp
 *        This module is STORM_MODE and LONGTERM_MODE compatibility.
 * \author Liangjun Zhu
 * \date Apr 2017
 * 
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup PI_MCS
 * \ingroup Hydrology
 * \brief Module for Precipitation Interception module based on Maximum Canopy Storage.
 */

/*!
 * \class clsPI_MCS
 * \ingroup PI_MCS
 * \brief Class for Precipitation Interception module based on Maximum Canopy Storage.
 */
class clsPI_MCS : public SimulationModule {
public:
    //! Constructor
    clsPI_MCS(void);

    //! Destructor
    ~clsPI_MCS(void);

    /* Parameters from database */

    // @In
    // @Description Calibration parameter, the sine-shaped curve controller exponent b, default is 1.35
    float Pi_b;

    // @In
    // @Description Calibration parameter, the initial interception storage for all cells, mm
    float Init_IS;

    // @In
    // @Description Maximum storage capacity, mm
    float *Interc_max;

    // @In
    // @Description Minimum storage capacity, mm
    float *Interc_min;

#ifdef STORM_MODE

    // @In
    // @Description hillslope time step, seconds
    float DT_HS;

    // @In
    // @Description slope for rainfall correction, height/width, i.e. tan(slope)
    float *m_slope;
#endif

    /* Input variables from other module's output */

    // @In
    // @Description Precipitation, For STROM_MODE model, the unit is rainfall intensity mm/h, For LONGTERM_MODE model, the unit is mm
    float *D_P;

#ifndef STORM_MODE

    // @In
    // @Description PET, mm
    float *PET;

#endif

    /* Results */

    // @Out
    // @Description current interception storage, the initial value equal to 0, mm
    float *canstor;

    // @Out
    // @Description Interception loss, mm
    float *INLO;

#ifndef STORM_MODE

    // @Out
    // @Description Evaporation loss from intercepted rainfall, mm
    float *INET;

#endif

    // @Out
    // @Description Net precipitation (after slope correction, of course), mm
    float *NEPR;

    /* Others */

    // @In
    // @Description number of valid cells
    int m_nCells;

public:
    virtual void Set1DData(const char *key, int nRows, float *data);

    virtual void SetValue(const char *key, float data);

    virtual void Get1DData(const char *key, int *nRows, float **data);

    virtual int Execute(void);

private:
    /**
    *	@brief check the input data. Make sure all the input data is available.
    *
    *	@return bool The validity of the input data.
    */
    bool CheckInputData(void);

    /**
    *	@brief check the input size. Make sure all the input data have same dimension.
    *
    *	@param key The key of the input data
    *	@param n The input data dimension
    *	@return bool The validity of the dimension
    */
    bool CheckInputSize(const char *, int);

    /*!
     * \brief Initialize output variables for the first run of the entire simulation
     */
    void initialOutputs(void);
};


VISITABLE_STRUCT(clsPI_MCS, m_nCells, Pi_b, Init_IS, Interc_max, Interc_min, DT_HS, m_slope, D_P, PET, canstor, INLO, INET, NEPR);