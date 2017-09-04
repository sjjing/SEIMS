/*!
 * \brief Simulates the loss of nitrate and phosphorus via surface runoff, 
 *        lateral flow, tile flow, and percolation out of the profile.
 *        Method of SWAT
 * \author Huiran Gao
 * \date May 2016
 */

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "NutrientCommon.h"

using namespace std;

/** \defgroup NutrMV
 * \ingroup Nutrient
 * \brief Simulates the loss of nitrate and phosphorus via surface runoff, lateral flow, tile flow, and percolation out of the profile.
 */

/*!
 * \class NutrientMovementViaWater
 * \ingroup NutrMV
 *
 * \brief Nutrient removed and loss in surface runoff, lateral flow, tile flow, and percolation out of the profile
 *
 */
class NutrientMovementViaWater : public SimulationModule {
public:
    NutrientMovementViaWater(void);

    ~NutrientMovementViaWater(void);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetValue(const char *key, float value);

    virtual int Execute(void);

    virtual void GetValue(const char *key, float *value);

    virtual void Get1DData(const char *key, int *n, float **data);

    //virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);
    virtual void SetSubbasins(clsSubbasins *subbasins);

    //! subbasin IDs
    vector<int> m_subbasinIDs;

    /// subbasins information
    clsSubbasins *m_subbasinsInfo;

    // @In
    // @Description cell width of grid map (m)
    float CELLWIDTH;

    // @In
    // @Description cell area, ha
    float cellArea;

    // @In
    // @Description number of cells
    int m_nCells;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description maximum soil layers
    int nSoiLayers;

    // @In
    // @Description stream link
    float *STREAM_LINK;

    // @In
    // @Description carbon modeling method, 0 Static soil carbon (old mineralization routines), 1 C-FARM one carbon pool model, 2 Century model
    int cswat;

    /// input data

    // @In
    // @Description factor which converts kg/kg soil to kg/ha
    float **conv_wt;

    // @In
    // @Description drainage tile flow in soil profile
    float qtile;

    // @In
    // @Description Phosphorus soil partitioning coefficient
    float phoskd;

    // @In
    // @Description phosphorus percolation coefficient (0-1)
    float pperco;

    // @In
    // @Description nitrate percolation coefficient (0-1)
    float nperco;

    // @In
    // @Description Conversion factor from CBOD to COD
    float cod_n;

    // @In
    // @Description Reaction coefficient from CBOD to COD
    float cod_k;

    // @In
    // @Description distribution of soil loss caused by water erosion
    float *SED_OL;

    // @In
    // @Description fraction of porosity from which anions are excluded
    float *anion_excl;

    // @In
    // @Description distribution of surface runoff generated
    float *OL_Flow;

    // @In
    // @Description initial septic operational condition (active-1, failing-2, non_septic-0)
    float isep_opt;

    // @In
    // @Description soil layer where drainage tile is located
    float *ldrain;

    // @In
    // @Description crack volume potential of soil
    float *sol_crk;

    // @In
    // @Description distance to the stream
    float *dist2stream;

    // @In
    // @Description amount of water held in the soil layer at saturation
    float **sol_ul;

    // @In
    // @Description lateral flow in soil layer
    float **SSRU;

    // @In
    // @Description percolation from soil layer
    float **Perco;

    // @In
    // @Description bulk density of the soil
    float **density;

    // @In
    // @Description depth to bottom of soil layer
    float **soilDepth;

    // @In
    // @Description flow out index
    float *FLOWOUT_INDEX_D8;

    // @In
    // @Description Routing layers according to the flow direction, no flow relationships within each layer, The first element in each layer is the number of cells in the layer
    float **ROUTING_LAYERS;

    // @In
    // @Description number of routing layers
    int nRoutingLayers;

    // @In
    // @Description amount of organic nitrogen in surface runoff
    float *sedorgn;

    // @In
    // @Description average air temperature
    float *TMEAN;

    // @In
    // @Description percent organic carbon in soil layer (%)
    float **sol_cbn;

    // @In
    // @Description soil thick of each layer (mm)
    float **soilthick;

    /// output data

    // @Out
    // @Description amount of nitrate transported with lateral flow, kg/ha
    float *latno3;

    // @Out
    // @Description amount of nitrate percolating past bottom of soil profile
    float *perco_n;

    // @Out
    // @Description amount of solute P percolating past bottom of soil profile
    float *perco_p;

    // @Out
    // @Description amount of nitrate transported with surface runoff, kg/ha
    float *sur_no3;

    // @Out
    // @Description amount of ammonian transported with surface runoff, kg/ha
    float *sur_nh4;

    // @Out
    // @Description amount of soluble phosphorus in surface runoff
    float *sur_solp;

    // @Out
    // @Description carbonaceous oxygen demand of surface runoff
    float *sur_cod;

    // @Out
    // @Description chlorophyll-a concentration in water yield
    float *chl_a;
    
    // @Out
    // @Description N and P to channel amount of nitrate transported with lateral flow to channel, kg
    float *latno3ToCh;  

    // @Out
    // @Description amount of nitrate transported with surface runoff to channel, kg
    float *sur_no3_ToCh; 

    // @Out
    // @Description amount of ammonian transported with surface runoff to channel, kg
    float *sur_nh4ToCh; 

    // @Out
    // @Description amount of soluble phosphorus in surface runoff to channel, kg
    float *sur_solpToCh;

    // @Out
    // @Description amount of nitrate percolating past bottom of soil profile sum by sub-basin, kg
    float *perco_n_gw;  

    // @Out
    // @Description amount of solute P percolating past bottom of soil profile sum by sub-basin, kg
    float *perco_p_gw;  

    // @Out
    // @Description amount of COD to reach in surface runoff (kg)
    float *sur_codToCh;

    // @Out
    // @Description the total number of subbasins
    int nSubbasins;
    
    // @Out
    // @Description subbasin grid (subbasins ID)
    float *subbasin;  

    /// input & output

    // @Out
    // @Description average annual amount of phosphorus leached into second soil layer
    float wshd_plch;

    // @In
    // @Description amount of nitrogen stored in the nitrate pool in soil layer
    float **sol_no3;

    // @In
    // @Description amount of phosphorus stored in solution
    float **sol_solp;

    // @In
    // @Description amount of C lost with sediment, kg/ha, input from NUTRSED module
    float *sedc;

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

    /*!
     * \brief Calculate the loss of nitrate via surface runoff, lateral flow, tile flow, and percolation out of the profile.
     *        mainly rewrited from nlch.f of SWAT
	 * 1. nitrate loss with surface flow
	 * 2. nitrate loss with subsurface flow (routing considered)
	 * 3. nitrate loss with percolation
     */
    void NitrateLoss(void);

    /*!
     * \brief Calculates the amount of phosphorus lost from the soil
     *        profile in runoff and the movement of soluble phosphorus from the first
     *        to the second layer via percolation.
     *		 rewrite from solp.f of SWAT
     */
    void PhosphorusLoss(void);

    /*
     * \brief compute loadings of chlorophyll-a, BOD, and dissolved oxygen to the main channel
     *        rewrite from subwq.f of SWAT
     */
    void SubbasinWaterQuality(void);
    ///*!
    // * \brief Calculate enrichment ratio.
    // * enrsb.f of SWAT
    // * \return void
    // */
    //float *CalculateEnrRatio();

    void initialOutputs(void);

    void SumBySubbasin(void);
};

VISITABLE_STRUCT(NutrientMovementViaWater, m_nCells, CELLWIDTH, cellArea, soillayers, nSoiLayers, STREAM_LINK, cswat, conv_wt, qtile, phoskd, 
	pperco, nperco, cod_n, cod_k, SED_OL, anion_excl, OL_Flow, isep_opt, ldrain, sol_crk, dist2stream, sol_ul, SSRU, Perco, density, soilDepth, 
	FLOWOUT_INDEX_D8, ROUTING_LAYERS, nRoutingLayers, sedorgn, TMEAN, sol_cbn, soilthick, latno3, perco_n, perco_p, sur_no3, sur_nh4, sur_solp, 
	sur_cod, chl_a, latno3ToCh, sur_no3_ToCh, sur_nh4ToCh, sur_solpToCh, perco_n_gw, perco_p_gw, sur_codToCh, nSubbasins, subbasin, wshd_plch, 
	sol_no3, sol_solp, sedc);