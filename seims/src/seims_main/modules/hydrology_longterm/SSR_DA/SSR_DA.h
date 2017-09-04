/*!
 * \brief Subsurface runoff using Darcy's law and the kinematic approximation
 *        Water is routed cell-to-cell according to D8 flow direction
 * \author Junzhi Liu
 * \review Liang-Jun Zhu
 * \date 2016-7-24
 * \note: 1. Add support of multi soil layers of each cells.
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/*!
 * \defgroup SSR_DA
 * \ingroup Hydrology_longterm
 * \brief Subsurface runoff using Darcy's law and the kinematic approximation
 *
 */

/*!
 * \class SSR_DA
 * \ingroup SSR_DA
 * 
 */
class SSR_DA : public SimulationModule {

	// @In
	// @Description valid cell numbers
    int m_nCells;

	// @In
	// @Description width of cell (m)
    float CELLWIDTH;

	// @In
	// @Description max number of soil layers
    int nSoilLayers;

	// @In
	// @Description number of soil layers of each cell
    float *soillayers;

	// @In
	// @Description soil thickness
    float **soilthick;

	// @In
	// @Description timestep
    int TIMESTEP;

	// @In
	// @Description Interflow scale factor
    float Ki;

	// @In
	// @Description soil freezing temperature threshold, deg C
    float t_soil;

	// @In
	// @Description slope (tan)
    float *slope;

	// @In
	// @Description  conductivity
    float **Conductivity;

	// @In
	// @Description amount of water held in the soil layer at saturation (sat - wp water), mm
    float **sol_ul;

	// @In
	// @Description pore size distribution index
    float **Poreindex;

	// @In
	// @Description amount of water available to plants in soil layer at field capacity (AWC=FC-WP), mm
    float **sol_awc;

	// @In
	// @Description water content of soil at -1.5 MPa (wilting point) mm H2O
    float **sol_wpmm;

	// @In
	// @Description soil water storage (mm)
    float **solst;

	// @In
	// @Description soil water storage in soil profile (mm)
    float *solsw;

	// @In
	// @Description soil temperature, deg C
    float *SOTE;

	// @In
	// @Description channel width, m
    float *CHWIDTH;

	// @In
	// @Description stream link
    float *STREAM_LINK;

	// @In
	// @Description brief 2d array of flow in cells, The first element in each sub-array is the number of flow in cells in this sub-array
    float **FLOWIN_INDEX_D8;

	// @In
	// @Description brief Routing layers according to the flow direction, no flow relationships within each layer, The first element in each layer is the number of cells in the layer
    float **ROUTING_LAYERS;

	// @In
	// @Description number of routing layers
    int nRoutingLayers;

	// @In
	// @Description number of subbasin
    int nSubbasin;

	// @In
	// @Description subbasin grid (ID of subbasin)
    float *subbasin;

    // outputs

	// @Out
	// @Description subsurface runoff (mm), VAR_SSRU
    float **SSRU;

	// @Out
	// @Description subsurface runoff volume (m3), VAR_SSRUVOL
    float **SSRUVOL;

	// @Out
	// @Description subsurface to streams from each subbasin, the first element is the whole watershed, m3, VAR_SBIF
    float *SBIF;

public:
    /// constructor
    SSR_DA(void);

    /// destructor
    ~SSR_DA(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int nRows, float *data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void SetSubbasins(clsSubbasins *subbasins);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

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

    void initialOutputs(void);

    void FlowInSoil(int id);
};

VISITABLE_STRUCT(SSR_DA, m_nCells, CELLWIDTH, nSoilLayers, soillayers, soilthick, TIMESTEP, Ki, t_soil, slope, Conductivity,
	sol_ul, Poreindex, sol_awc, sol_wpmm, solst, solsw, SOTE, CHWIDTH, STREAM_LINK, FLOWIN_INDEX_D8, ROUTING_LAYERS,
	nRoutingLayers, nSubbasin, subbasin, SSRU, SSRUVOL, SBIF);