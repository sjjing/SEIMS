/** 
*	@version	1.3
*	@author    Junzhi Liu
*	@date	19-January-2011
*
*	@brief	Modified Rational Method to calculate infiltration and excess precipitation
*
*	Revision:	Zhiqiang Yu
*   Date:		2011-2-15
*	Description:
*	1.	Parameter S_M_frozen would be s_frozen and DT_Single.
*	2.	Parameter sfrozen would be t_soil and in WaterBalance table.
*	3.  Delete parameter Moist_in.
*	4.  Rename the input and output variables. See metadata rules for names.
*	5.  In function execute, do not change NEPR[i] directly. This will have influence
*		on another modules who will use net precipitation. Use local variable to replace it.
*	6.  Add API function GetValue.
*
*	Revision:	Junzhi Liu
*   Date:		2011-2-19
*	1.	Rename m_excess to EXCP
*	2.	Take snowmelt into consideration when calculating PE, PE=P_NET+Snowmelt-F
*
*	Revision:	Junzhi Liu
*   Date:		2013-10-28
*	1.	Add multi-layers support for soil parameters
*
*   Revision: LiangJun Zhu
*   Date:        2016-5-27
*   1. Update the support for multi-layers soil parameters
*   
*   Revision: LiangJun Zhu
*   Date:       2016-7-14
*   1. Remove snowmelt as AddInput, because snowmelt is considered into net precipitation in SnowMelt moudule,
*      by the meantime, this can avoid runtime error when SnowMelt module is not configured.
*   2. Change the unit of soil moisture from mm H2O/mm Soil to mm H2O, which is more rational.
*   3. Change soil moisture to soil storage which is coincident with SWAT, and do not include wilting point.
*/

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup SUR_MR
 * \ingroup Hydrology_longterm
 * \brief Modified Rational Method to calculate infiltration and excess precipitation
 *
 */

/*!
 * \class SUR_MR
 * \ingroup SUR_MR
 * \brief Modified Rational Method to calculate infiltration and excess precipitation
 * 
 */
class SUR_MR : public SimulationModule {
public:
    //! Constructor
    SUR_MR(void);

    //! Destructor
    ~SUR_MR(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    void CheckInputData(void);

    // @In
    // @Description Hillslope time step (second)
    float DT_HS;

    // @In
    // @Description count of valid cells
    int m_nCells;

    // @In
    // @Description net precipitation of each cell (mm)
    float *NEPR;

    // @In
    // @Description potential runoff coefficient
    float *Runoff_co;

    // @In
    // @Description number of soil layers, i.e., the maximum soil layers of all soil types
    int nSoilLayers;

    // @In
    // @Description soil layers number of each cell
    float *soillayers;
    
    // @In
    // @Description mm H2O: (sol_fc) amount of water available to plants in soil layer at field capacity (fc - wp)
    float **sol_awc;

    // @In
    // @Description amount of water held in the soil layer at saturation (sat - wp water), mm H2O, sol_sumul of SWAT
    float *sol_sumul;

    // @In
    // @Description initial soil water storage fraction related to field capacity (FC-WP)
    float *Moist_in;

    // @In
    // @Description runoff exponent
    float K_run;

    // @In
    // @Description maximum precipitation corresponding to runoffCo
    float P_max;

    // @In
    // @Description depression storage (mm), SD(t-1) from the depression storage module
    float *DPST;    

    // @In
    // @Description mean air temperature (deg C)
    float *TMEAN;
   
    // @In
    // @Description threshold soil freezing temperature (deg C)
    float t_soil;

    // @In
    // @Description frozen soil moisture relative to saturation above which no infiltration occur, (m3/m3 or mm H2O/ mm Soil) 
    float s_frozen;

    // @In
    // @Description soil temperature obtained from the soil temperature module (deg C)
    float *SOTE;

    // @In
    // @Description pothole volume, mm
    float *pot_vol;

    // @In
    // @Description impound trigger
    float *impound_trig;

    // output

    // @Out
    // @Description the excess precipitation (mm) of the total nCells, which could be depressed or generated surface runoff
    float *EXCP;

    // @Out
    // @Description infiltration map of watershed (mm) of the total nCells
    float *INFIL;

    // @Out
    // @Description soil water storage (mm)
    float **solst;

    // @Out
    // @Description soil water storage in soil profile (mm)
    float *solsw;

    /// initial output for the first run
    void initialOutputs(void);
};

VISITABLE_STRUCT(SUR_MR, m_nCells, DT_HS, NEPR, Runoff_co, nSoilLayers, soillayers, sol_awc, sol_sumul, Moist_in, K_run, P_max, DPST, TMEAN, t_soil, 
	s_frozen, SOTE, pot_vol, impound_trig, EXCP, INFIL, solst, solsw);