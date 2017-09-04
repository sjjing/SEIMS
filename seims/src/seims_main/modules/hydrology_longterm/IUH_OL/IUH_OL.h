/** 
*	@version	1.0
*	@author    Wu Hui
*	@date	24-January-2011
*
*	@brief	IUH overland method to calculate overland flow routing
*
*	Revision:	Zhiqiang YU	
*   Date:		2011-2-22
*	Description:
*	1.	Add parameter CellWidth.
*	2.	Delete parameter uhminCell and uhmaxCell because the parameter Ol_iuh
*		contains these information. The first and second column of Ol_iuh is 
*		min time and max time.
*	3.	The number of subbasins (m_nsub) should get from subbasin rather than
*		from main program. So does variable m_nCells.
*	4.	Add variable iuhCols to store the number of columns of Ol_iuh. In the 
*		meantime, add one parameter nCols to function SetIUHCell.
*	5.	Add variable cellFlow to store the flow of each cell in each day between
*		min time and max time. Its number of columns equals to the maximum of second
*		column of Ol_iuh add 1.
*	6.  Add function initial to initialize some variables.
*	7.	Modify function Execute.
*	
*	Revision: Liang-Jun Zhu
*	Date:	  2016-7-29
*	Description:
*	1.	Unify the code style.
*	2.	Replace VAR_SUBBASIN with VAR_SUBBASIN_PARAM, which is common used by several modules.
*/
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup IUH_OL
 * \ingroup Hydrology_longterm
 * \brief IUH overland method to calculate overland flow routing
 *
 */

/*!
 * \class IUH_OL
 * \ingroup IUH_OL
 * \brief IUH overland method to calculate overland flow routing
 * 
 */
class IUH_OL : public SimulationModule {
public:
    IUH_OL(void);

    ~IUH_OL(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetSubbasins(clsSubbasins *);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

    //! subbasin IDs
    vector<int> m_subbasinIDs;

    /// subbasins information
    clsSubbasins *m_subbasinsInfo;

    // @In
    // @Description time step (sec)
    int TIMESTEP;

    // @In
    // @Description validate cells number
    int m_nCells;

    // @In
    // @Description cell width of the grid (m)
    float CELLWIDTH;

    // @In
    // @Description cell area, BE CAUTION, the unit is m^2, NOT ha!!!
    float cellArea;

    // @In
    // @Description the total number of subbasins
    int nSubbasins;

    // @In
    // @Description subbasin grid (subbasins ID)
    float *subbasin;


    // @In
    // @Description IUH of each grid cell (1/s)
    float **Ol_iuh;

    // @In
    // @Description the number of columns of Ol_iuh
    int iuhCols;

    // @In
    // @Description surface runoff from depression module
    float *SURU;

    //temporary

    // @In
    // @Description store the flow of each cell in each day between min time and max time
    float **cellFlow;

    // @In
    // @Description the maximum of second column of OL_IUH plus 1.
    int cellFlowCols;

    //output

    // @Out
    // @Description overland flow to streams for each subbasin (m3/s)
    float *SBOF;

    // @Out
    // @Description overland flow in each cell (mm) //added by Gao, as intermediate variable, 29 Jul 2016
    float *OL_Flow;

    //! initial outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(IUH_OL, m_nCells, TIMESTEP, CELLWIDTH, cellArea, nSubbasins, subbasin, Ol_iuh, iuhCols, SURU, cellFlow, cellFlowCols, SBOF, OL_Flow);