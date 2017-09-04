/*!
 * Revision:    Liang-Jun Zhu
 * Date:        2016-7-28
 * Description: 1. Move subbasin class to base/data/clsSubbasin, to keep consistent with other modules
 *              2. Code cleanup
 *
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "clsSubbasin.h"

using namespace std;

/*!
 * \defgroup SOL_WB
 * \ingroup Hydrology_longterm
 * \brief Soil water balance calculation
 *
 */

/*!
 * \class SOL_WB
 * \ingroup SOL_WB
 * \brief Soil water balance calculation
 * 
 */

class SOL_WB : public SimulationModule {

    //! subbasin IDs
    vector<int> m_subbasinIDs;
    //! All subbasins information,\sa clsSubbasins, \sa Subbasin
    clsSubbasins *m_subbasinsInfo;

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description maximum soil layers number
    int nSoilLayers;

    // @In
    // @Description soil layers number of each cell
    float *soillayers;

    // @In
    // @Description soil thickness of each layer
    float **soilthick;

    // @In
    // @Description the maximum soil depth
    float *SOL_ZMX;

    // @In
    // @Description Net precipitation (include snow melt if stated) (mm)
    float *NEPR;

    // @In
    // @Description infiltration water (mm)
    float *INFIL;

    // @In
    // @Description evaporation from the soil water storage, es_day in SWAT (mm)
    float *SOET;

    // @In
    // @Description revaporization from groundwater to the last soil layer (mm)
    float *Revap;

    // @In
    // @Description subsurface runoff
    float **SSRU;

    // @In
    // @Description percolation (mm)
    float **Perco;

    // @In
    // @Description soil storage (mm)
    float **solst;
    
    // @In
    // @Description precipitation on the current day (mm)
    float *D_P;

    // @In
    // @Description interception loss (mm)
    float *INLO;

    // @In
    // @Description evaporation from the interception storage (mm)
    float *INET;

    // @In
    // @Description depression (mm)
    float *DPST;

    // @In
    // @Description evaporation from depression storage (mm)
    float *DEET;

    // @In
    // @Description surface runoff generated (mm)
    float *SURU;

    // @In
    // @Description groundwater runoff
    float *RG;

    // @In
    // @Description snow sublimation
    float *SNSB;

    // @In
    // @Description mean temperature
    float *TMEAN;

    // @In
    // @Description soil temperature
    float *SOTE;

    // @In
    // @Description subbasins number
    int nSubbasins;
   
    // @Out
    // @Description soil water balance, time series result, the row index is subbasinID
    float **SOWB;

public:
    SOL_WB(void);

    ~SOL_WB(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int nRows, float *data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void SetSubbasins(clsSubbasins *subbasins);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    virtual int Execute(void);

private:
    /**
    *	@brief check the input data. Make sure all the input data is available.
    */
    void CheckInputData(void);

    /**
    *	@brief check the input size. Make sure all the input data have same dimension.
    *
    *	@param key The key of the input data
    *	@param n The input data dimension
    *	@return bool The validity of the dimension
    */
    bool CheckInputSize(const char *, int);

    //!
    void initialOutputs(void);

    /*!
     * \brief Set parameter values to subbasins
     */
    void setValueToSubbasins(void);
};

VISITABLE_STRUCT(SOL_WB, m_nCells, nSoilLayers, soillayers, soilthick, SOL_ZMX, NEPR, INFIL, SOET, Revap, SSRU, Perco, solst, D_P, 
	INLO, INET, DPST, DEET, SURU, RG, SNSB, TMEAN, SOTE, nSubbasins, SOWB);


// previously, RootDepth (of plant) is confused with the sol_zmx, now change m_RootDepth to SOL_ZMX
//float *m_RootDepth;
//float m_upSoilDepth; /// not needed any more. By LJ.

//time_t m_Date; there is no need to define date here. By LJ.