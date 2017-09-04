/*!
 * \brief Calculates the nitrate and soluble phosphorus loading contributed by groundwater flow.
 * \author Huiran Gao
 * \date Jun 2016
 */

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup NutrGW
 * \ingroup Nutrient
 * \brief Calculates the nitrate and soluble phosphorus loading contributed by groundwater flow.
 */

/*!
 * \class NutrientinGroundwater
 * \ingroup NutrGW
 *
 * \brief Calculates the nitrate and soluble phosphorus loading contributed by groundwater flow.
 *
 */

class NutrientinGroundwater : public SimulationModule {
public:
    NutrientinGroundwater(void);

    ~NutrientinGroundwater(void);

    virtual void SetValue(const char *key, float value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetReaches(clsReaches *reaches);

    //virtual void Set2DData(const char* key, int nRows, int nCols, float** data);
    virtual int Execute(void);

    //virtual void GetValue(const char* key, float* value);
    virtual void Get1DData(const char *key, int *n, float **data);
    //virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);

    virtual void SetSubbasins(clsSubbasins *subbasins);

	//! subbasin IDs
	vector<int> m_subbasinIDs;

	/// subbasins information
	clsSubbasins *m_subbasinsInfo;

	// @In
	// @Description cell width of grid map (m)
    float CELLWIDTH;

	// @In
	// @Description number of cells
    int m_nCells;

	// @In
	// @Description time step (s)
    int TIMESTEP;

    /// input data

	// @In
	// @Description gw0
    float GW0;

	// @Out
	// @Description nitrate N concentration in groundwater loading to reach (mg/L, i.e. g/m3)
    float *gwno3_conc;

	// @Out
	// @Description kg
    float *gwno3;

	// @Out
	// @Description soluble P concentration in groundwater loading to reach (mg/L, i.e. g/m3)
    float *gwsolp_conc;

	// @Out
	// @Description kg
    float *gwsolp;

	// @In
	// @Description groundwater contribution to stream flow (m3/s)
    float *SBQG;

	// @In
	// @Description groundwater storage
    float *SBGS;

	// @In
	// @Description amount of nitrate percolating past bottom of soil profile, kg
    float *perco_n_gw;

	// @In
	// @Description amount of solute P percolating past bottom of soil profile, kg
    float *perco_p_gw;

    // soil related

	// @In
	// @Description amount of nitrogen stored in the nitrate pool in soil layer
    float **sol_no3;

	// @In
	// @Description amount of soluble phosphorus stored in the soil layer
    float **sol_solp;

	// @In
	// @Description max number of soil layers
    int nSoilLayers;

	// @In
	// @Description number of soil layers of each cell
    float *soillayers;

    /// outputs

	// @Out
	// @Description nitrate loading to reach in groundwater to channel
    float *no3gwToCh;

	// @Out
	// @Description soluble P loading to reach in groundwater to channel
    float *minpgwToCh;

	// @In
	// @Description the total number of subbasins
    int nSubbasins;
   
	// @In
	// @Description subbasin grid (subbasins ID)
    float *subbasin;
   

private:

    /*!
     * \brief check the input data. Make sure all the input data is available.
     * \return bool The validity of the input data.
     */
    bool CheckInputData(void);

    /*!
     * \brief check the input size. Make sure all the input data have same dimension.
     *
     * \param[in] key The key of the input data
     * \param[in] n The input data dimension
     * \return bool The validity of the dimension
     */
    bool CheckInputSize(const char *, int);

    /// initial outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(NutrientinGroundwater, m_nCells, CELLWIDTH, TIMESTEP, GW0, gwno3_conc, gwno3, gwsolp_conc, gwsolp, SBQG, SBGS, perco_n_gw, 
	perco_p_gw, sol_no3, sol_solp, nSoilLayers, soillayers, no3gwToCh, minpgwToCh, nSubbasins, subbasin);