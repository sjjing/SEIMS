/*----------------------------------------------------------------------
*	Purpose: 	A simple fill and spill method method to calculate depression storage
*
*	Created:	Junzhi Liu
*	Date:		14-Febrary-2011
*
*	Revision:	Zhiqiang Yu	
*   Date:		2011-2-15
*	Description:
*	1.	Modify the name of some parameters, input and output variables.
*		Please see the metadata rules for the names.
*	2.	Depre_in would be DT_Single. Add function SetSingleData() to 
*		set its value.
*	3.	This module will be called by infiltration module to get the 
*		depression storage. And this module will also use the outputs
*		of infiltration module. The sequence of this two modules is 
*		infiltration->depression. When infiltration first calls the 
*		depression module, the execute function of depression module
*		is not executed before getting the outputs. So, the output 
*		variables should be initial in the Get1DData function. This 
*		initialization is realized by function initalOutputs. 
*	4.	Delete input D_INFIL and add input D_EXCP.
*	
*	Revision:    LiangJunZhu
*	Date    :    2016-7-14
*	Description:
*---------------------------------------------------------------------*/

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup DEP_LINSLEY
 * \ingroup Hydrology_longterm
 * \brief A simple fill and spill method method to calculate depression storage
 *
 */
/*!
 * \class DepressionFSDaily
 * \ingroup DEP_LINSLEY
 * \brief A simple fill and spill method method to calculate depression storage
 * 
 */
class DepressionFSDaily : public SimulationModule {
public:
    //! Constructor
    DepressionFSDaily(void);

    //! Destructor
    ~DepressionFSDaily(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

	// @In
	// @Description valid cells number
    int m_nCells;

	// @In
	// @Description impound/release
    float *impound_trig;

	// @In
	// @Description pothole volume, mm
    float *pot_vol;

	// @In
	// @Description initial depression storage coefficient
    float Depre_in;

	// @In
	// @Description depression storage capacity (mm)
    float *Depression;

	// @In
	// @Description pet
    float *PET;

	// @In
	// @Description evaporation from the interception storage
    float *INET;

	// @In
	// @Description excess precipitation calculated in the infiltration module
    float *EXCP;

    // state variables (output)

	// @Out
	// @Description depression storage
    float *DPST;

	// @Out
	// @Description evaporation from depression storage
    float *DEET;

	// @Out
	// @Description surface runoff
    float *SURU;

    /*!
     * \brief Initialize output variables
     * This module will be called by infiltration module to get the
     *		depression storage. And this module will also use the outputs
     *		of infiltration module. The sequence of this two modules is
     *		infiltration->depression. When infiltration first calls the
     *		depression module, the execute function of depression module
     *		is not executed before getting the outputs. So, the output
     *		variables should be initial in the Get1DData function. This
     *		initialization is realized by function initalOutputs.
     */
    void initialOutputs(void);
};

VISITABLE_STRUCT(DepressionFSDaily, m_nCells, impound_trig, pot_vol, Depre_in, Depression, PET, INET, EXCP, DPST, DEET, SURU);