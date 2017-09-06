/** 
*	@version	1.0
*	@author    Wu Hui
*	@date	24-January-2011
*
*	@brief	Reservoir Method to calculate groundwater balance and baseflow
*
*	Revision: Zhiqiang Yu
*   Date:	  2011-2-11
*
*	Revision: Zhiqiang Yu
*	Date:	  2011-2-18
*	Description:
*	1.  Add judgment to calculation of EG (Revap). The average percolation of 
*		one subbasin is first calculated. If the percolation is less than 0.01,
*		EG is set to 0 directly. (in function setInputs of class subbasin)
*	2.	Add member variable m_isRevapChanged to class subbasin. This variable 
*		is the flag whether the Revap is changed by current time step. This flag
*		can avoid repeating setting values when converting subbasin average Revap
*		to cell Revap.(in function Execute of class ReservoirMethod)
*
*	Revision:	Zhiqiang Yu
*	Date:		2011-3-14
*	Description:
*	1.	Add codes to process the groundwater which comes from bank storage in 
*		channel routing module. The water volume of this part of groundwater is
*		added to the groundwater storage. The input variable "T_GWNEW" is used
*		for this purpose. One additional parameter is added to function setInputs 
*		of class subbasin. See equation 8 in memo "Channel water balance" for detailed
*		reference.
*	
*	Revision:	Liang-Jun Zhu
*	Date:		2016-7-27
*	Description:
*	1.	Move subbasin class to base/data module for sharing with other modules
*/
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "clsSubbasin.h"

using namespace std;
/** \defgroup GWA_RE
 * \ingroup Hydrology_longterm
 * \brief Reservoir Method to calculate groundwater balance and baseflow of longterm model
 *
 */

/*!
 * \class ReservoirMethod
 * \ingroup GWA_RE
 * \brief Reservoir Method to calculate groundwater balance and baseflow of longterm model
 * 
 */
class ReservoirMethod : public SimulationModule {
public:
    ReservoirMethod(void);

    ~ReservoirMethod(void);

    virtual void SetValue(const char *key, float value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void SetSubbasins(clsSubbasins *);

    virtual int Execute(void);

    virtual void Get1DData(const char *key, int *nRows, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    //virtual TimeStepType GetTimeStepType()
    //{
    //	return TIMESTEP_CHANNEL;
    //};

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
    *	@param key: The key of the input data
    *	@param n: The input data dimension
    *	@return bool The validity of the dimension
    */
    bool CheckInputSize(const char *, int n);

    /*
     * \brief initial outputs as default values
     */
    void initialOutputs(void);

    // @In
    // @Description time step, second
    int TIMESTEP;

    // @In
    // @Description Valid cells number
    int m_nCells;

    // @In
    // @Description cell size of the grid (m)
    float CELLWIDTH;

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
    // @Description groundwater Revap coefficient
    float df_coef;

    // @In
    // @Description baseflow recession coefficient
    float Kg;

    // @In
    // @Description baseflow recession exponent
    float Base_ex;

    // @In
    // @Description the amount of water percolated from the soil water reservoir and input to the groundwater reservoir from the percolation module(mm)
    float **Perco;

    // @In
    // @Description evaporation from interception storage (mm) from the interception module
    float *INET;

    // @In
    // @Description evaporation from the depression storage (mm) from the depression module
    float *DEET;

    // @In
    // @Description evaporation from the soil water storage (mm) from the soil ET module
    float *SOET;

    // @In
    // @Description actual amount of transpiration (mm H2O)
    float *AET_PLT;

    // @In
    // @Description PET(mm) from the PET modules
    float *PET;

    // @In
    // @Description initial ground water storage (or at time t-1)
    float GW0;

    // @In
    // @Description maximum ground water storage
    float GWMAX;

    // @Out
    // @Description the potential evapotranspiration rate of the subbasin
    float *SBPET;

    // @Out
    // @Description Groundwater storage of the subbasin
    float *SBGS;

    // @In
    // @Description slope (percent, or drop/distance, or tan) of each cell
    float *slope;

    // @In
    // @Description soil storage
    float **solst;

    // @In
    // @Description soil depth of each layer, the maximum soil depth is used here, i.e., soilDepth[i][(int)soillayers[i]]
    float **soilDepth;

    // @In
    // @Description ground water from bank storage, passed from channel routing module
    float *GWNEW;

    //output

    // @Out
    // @Description groundwater runoff
    float *RG;

    // @Out
    // @Description groundwater flow out of the subbasin
    float *SBQG;

    // @Out
    // @Description revaporization from groundwater to the last soil layer
    float *Revap;

    // @Out
    // @Description groundwater water balance statistics
    float **GWWB;

private:

    //! subbasin IDs
    vector<int> m_subbasinIDs;

    //! All subbasins information,\sa clsSubbasins, \sa Subbasin
    clsSubbasins *m_subbasinsInfo;

    // subbasin number
    int nSubbasins;

    // first Run
    bool firstRun;

    //output

    // revaporization from groundwater to the last soil layer
    float *T_Revap;

    // Perco
    float *T_Perco;

    // PerDep
    float *T_PerDep;


    /*
     * \brief Set groundwater related subbasin parameters
     * \sa Subbasin
     * \sa clsSubbasins
     */
    void setSubbasinInfos(void);
};

VISITABLE_STRUCT(ReservoirMethod, m_nCells, TIMESTEP, CELLWIDTH, nSoilLayers, soillayers, soilthick, df_coef, Kg, Base_ex, Perco, INET, 
	DEET, SOET, AET_PLT, PET, GW0, GWMAX, SBPET, SBGS, slope, solst, soilDepth, GWNEW, RG, SBQG, Revap, GWWB);