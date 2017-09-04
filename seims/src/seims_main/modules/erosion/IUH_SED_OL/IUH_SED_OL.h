/** 
*	@version	1.0
*	@author    Junzhi Liu
*	@date	2016-08-12
*
*	@brief	IUH overland method to calculate overland sediment routing
*/

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup IUH_SED_OL
 * \ingroup Hydrology_longterm
 * \brief IUH overland method to calculate overland flow routing
 *
 */

/*!
 * \class IUH_SED_OL
 * \ingroup IUH_SED_OL
 * \brief IUH overland method to calculate overland flow routing
 * 
 */
class IUH_SED_OL : public SimulationModule {
public:
    IUH_SED_OL(void);

    ~IUH_SED_OL(void);

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
	// @Description cell area
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
	// @Description sediment yield in each cell
    float *SOER;

    //temporary

	// @In
	// @Description the maximum of second column of OL_IUH plus 1.
    int cellFlowCols;

	// @In
	// @Description store the sediment of each cell in each day between min time and max time
    float **cellSed;

    
    //output

	// @Out
	// @Description sediment to streams
    float *SEDTOCH;

	// @Out
	// @Description sediment to channel at each cell at current time step
    float *SED_OL;

    //! intial outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(IUH_SED_OL, m_nCells, TIMESTEP, CELLWIDTH, cellArea, nSubbasins, subbasin, Ol_iuh, iuhCols, SOER, cellFlowCols, cellSed, SEDTOCH, SED_OL);