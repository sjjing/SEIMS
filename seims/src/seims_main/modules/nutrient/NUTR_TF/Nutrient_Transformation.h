/*!
 * \brief Daily nitrogen and phosphorus mineralization and immobilization.
 *        From nminrl.f, nitvol.f, pminrl.f, and pminrl2.f of SWAT
 * \author Huiran Gao
 * \date April 2016
 *
 * \revised Liang-Jun Zhu
 * \date 2016-7-24
 * \note: 1. 
 *        2. Change wshd_dnit etc. variables to store the statistics values of the current day
 *        3. hmntl etc. variables should be DT_Raster1D rather than DT_Single since they are soil profile values in cell!
 */

/*!
 * \defgroup NUTR_TF
 * \ingroup Nutrient
 * \brief Daily nitrogen and phosphorus mineralization and immobilization.
 * From nminrl.f, nitvol.f, pminrl.f, and pminrl2.f of SWAT
 * \author Huiran Gao
 * \date April 2016
 */

#pragma once

#include <visit_struct/visit_struct_intrusive.hpp>
#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/*!
 * \class Nutrient_Transformation
 * \ingroup NUTR_TF
 *
 * \brief Daily nitrogen and phosphorus mineralization and immobilization.
 *  Considering fresh organic material (plant residue) and active and stable humus material.
 *
 */
class Nutrient_Transformation : public SimulationModule {
public:
    Nutrient_Transformation(void);

    ~Nutrient_Transformation(void);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetValue(const char *key, float value);

    virtual int Execute(void);

    virtual void GetValue(const char *key, float *value);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

	// @In
	// @Description cell width of grid map (m)
    float CELLWIDTH;

	// @In
	// @Description number of cells
    int m_nCells;

	// @In
	// @Description soil layers
    float *soillayers;

	// @In
	// @Description maximum soil layers
    int soilLayers;

	// @In
	// @Description carbon modeling method, 0 Static soil carbon (old mineralization routines), 1 C-FARM one carbon pool model, 2 Century model
    int cswat;

	// @In
	// @Description phosphorus model selection, 0: dynamic coefficient method by White et al., 2009, 1: original method
    int solP_model;

	// @In
	// @Description days since P application
    float *a_days;

	// @In
	// @Description days since P deficit
    float *b_days;

	// @In
	// @Description tillage factor on SOM decomposition, used by CENTURY model
    float *tillage_switch;

	// @In
	// @Description tillage factor on SOM decomposition, used by CENTURY model
    float *tillage_depth;

	// @In
	// @Description tillage factor on SOM decomposition, used by CENTURY model
    float *tillage_days;

	// @In
	// @Description tillage factor on SOM decomposition, used by CENTURY model
    float *tillage_factor;

    ///input data

	// @In
	// @Description rate factor for humus mineralization on active organic N
    float cmn;

	// @In
	// @Description nitrogen active pool fraction. The fraction of organic nitrogen in the active pool.
    float nactfr;

	// @In
	// @Description denitrification threshold: fraction of field capacity
    float sdnco;

	// @In
	// @Description Phosphorus availability index. The fraction of fertilizer P remaining in labile pool after initial rapid phase of P sorption
    float psp;

	// @In
	// @Description Phosphorus availability index. The fraction of fertilizer P remaining in labile pool after initial rapid phase of P sorption
    float ssp;

	// @In
	// @Description rate coefficient for denitrification
    float cdn;

	// @In
	// @Description land cover code from crop.dat
    float *landcover;

	// @In
	// @Description plant residue decomposition coefficient, he fraction of residue which will decompose in a day assuming optimal moisture, temperature, C:N ratio, and C:P ratio
    float *rsdco_pl;

	// @In
	// @Description amount of residue on soil surface (kg/ha)
    float *sol_cov;

	// @In
	// @Description amount of residue on soil surface (kg/ha)
    float *rsdin;

	// @In
	// @Description daily average temperature of soil layer(deg C)
    float *SOTE;

	// @In
	// @Description bulk density of the soil (Mg/m3)
    float **density;

	// @In
	// @Description percent organic carbon in soil layer (%)
    float **sol_cbn;

	// @In
	// @Description amount of water stored in the soil layer on current day(mm H2O)
    float **solst;

	// @In
	// @Description Water content of soil profile at field capacity(mm H2O) (FC-WP)
    float **sol_awc;

	// @In
	// @Description depth to bottom of soil layer
    float **soilDepth;

	// @In
	// @Description Percent of clay content
    float **CLAY;

	// @In
	// @Description percent of rock content
    float **rock;

	// @In
	// @Description thick of each soil layer
    float **soilthick;

	// @Out
	// @Description amount of nitrogen stored in the active organic (humic) nitrogen pool(kg N/ha)
    float **sol_aorgn;

	// @Out
	// @Description amount of nitrogen stored in the fresh organic (residue) pool(kg N/ha)
    float **sol_fon;

	// @Out
	// @Description amount of phosphorus stored in the fresh organic (residue) pool(kg P/ha)
    float **sol_fop;

	// @Out
	// @Description amount of phosphorus stored in the active mineral phosphorus pool
    float **sol_actp;

	// @Out
	// @Description amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool
    float **sol_stap;

	// @In
	// @Description amount of water held in the soil layer at saturation
    float **sol_ul;

	// @In
	// @Description porosity mm/mm
    float **Porosity;

	// @In
	// @Description percent sand content of soil material
    float **sand;

    ///output data

    /************************************************************************/
    /*    CENTURY model related parameters (initialized and output)  20     */
    /************************************************************************/
    
	// @Out
	// @Description CENTURY model related parameters
	float **sol_WOC; 

	// @Out
	// @Description CENTURY model related parameters
    float **sol_WON; 

	// @Out
	// @Description CENTURY model related parameters
    float **sol_BM; 

	// @Out
	// @Description CENTURY model related parameters
    float **sol_BMC; 

	// @Out
	// @Description CENTURY model related parameters
    float **sol_BMN; 

	// @Out
	// @Description CENTURY model related parameters, mass of OM in passive humus
    float **sol_HP; 

	// @Out
    // @Description CENTURY model related parameters, mass of OM in slow humus
    float **sol_HS; 
    
	// @Out
	// @Description CENTURY model related parameters, mass of C present in slow humus
	float **sol_HSC; 

	// @Out
	// @Description CENTURY model related parameters, mass of N present in slow humus
    float **sol_HSN; 

	// @Out
	// @Description CENTURY model related parameters, mass of C present in passive humus
    float **sol_HPC; 

	// @Out
	// @Description CENTURY model related parameters, mass of N present in passive humus
    float **sol_HPN; 

	// @Out
	// @Description CENTURY model related parameters, metabolic litter SOM pool
    float **sol_LM; 

	// @Out
	// @Description CENTURY model related parameters, metabolic litter C pool
    float **sol_LMC; 

	// @Out
	// @Description CENTURY model related parameters, metabolic litter N pool
    float **sol_LMN; 

	// @Out
	// @Description CENTURY model related parameters, structural litter C pool
    float **sol_LSC; 

	// @Out
	// @Description CENTURY model related parameters, structural litter N pool
    float **sol_LSN;

	// @Out
	// @Description CENTURY model related parameters, structural litter SOM pool
    float **sol_LS; 

	// @Out
	// @Description CENTURY model related parameters, lignin weight in structural litter
    float **sol_LSL; 

	// @Out
	// @Description CENTURY model related parameters, lignin amount in structural litter pool
    float **sol_LSLC; 

	// @Out
	// @Description CENTURY model related parameters, non-lignin part of the structural litter C
    float **sol_LSLNC; 

	// @Out
	// @Description CENTURY model related parameters,
    float **sol_RNMN; 

	// @Out
	// @Description CENTURY model related parameters,
    float **sol_RSPC; 

    /************************************************************************/

	// @Out
	// @Description amount of nitrogen moving from active organic to nitrate pool in soil profile on current day in cell(kg N/ha)
    float *hmntl;

	// @Out
	// @Description amount of phosphorus moving from the organic to labile pool in soil profile on current day in cell(kg P/ha)
    float *hmptl;

	// @Out
	// @Description amount of nitrogen moving from the fresh organic (residue) to the nitrate(80%) and active organic(20%) pools in soil profile on current day in cell(kg N/ha)
    float *rmn2tl;

	// @Out
	// @Description amount of phosphorus moving from the fresh organic (residue) to the labile(80%) and organic(20%) pools in soil profile on current day in cell(kg P/ha)
    float *rmptl;

	// @Out
	// @Description amount of nitrogen moving from active organic to stable organic pool in soil profile on current day in cell(kg N/ha)
    float *rwntl;

	// @Out
	// @Description amount of nitrogen lost from nitrate pool by denitrification in soil profile on current day in cell(kg N/ha)
    float *wdntl;

	// @Out
	// @Description amount of phosphorus moving from the labile mineral pool to the active mineral pool in the soil profile on the current day in cell(kg P/ha)
    float *rmp1tl;

	// @Out
	// @Description amount of phosphorus moving from the active mineral pool to the stable mineral pool in the soil profile on the current day in cell(kg P/ha)
    float *roctl;

    ///input & output data

	BEGIN_VISITABLES(Nutrient_Transformation);

	// @In
	// @Description amount of nitrogen stored in the nitrate pool in soil layer(kg N/ha)
    //float **sol_no3;
	VISITABLE(float **, sol_no3);

	// @In
	// @Description amount of nitrogen stored in the stable organic N pool(kg N/ha)
    //float **sol_orgn;
	VISITABLE(float **, sol_orgn);

	// @In
	// @Description amount of phosphorus stored in the organic P pool in soil layer(kg P/ha)
    //float **sol_orgp;
	VISITABLE(float **, sol_orgp);

	// @In
	// @Description
    ///amount of organic matter in the soil classified as residue(kg/ha)
    //float **sol_rsd;
	VISITABLE(float **, sol_rsd);

	// @In
	// @Description amount of phosphorus stored in solution(kg P/ha)
    //float **sol_solp;
	VISITABLE(float **, sol_solp);

	// @In
	// @Description amount of nitrogen stored in the ammonium pool in soil layer(kg/ha)
    //float **sol_nh4;
	VISITABLE(float **, sol_nh4);

	// @In
	// @Description water content of soil at -1.5 MPa (wilting point)
    //float **sol_wpmm;
	VISITABLE(float **, sol_wpmm);

	// @Out
	// @Description nitrogen lost from nitrate pool due to denitrification in watershed(kg N/ha)
    //float wshd_dnit;
	VISITABLE(float, wshd_dnit);

	// @Out
	// @Description nitrogen moving from active organic to nitrate pool in watershed(kg N/ha)
    //float wshd_hmn;
	VISITABLE(float, wshd_hmn);

	// @Out
	// @Description phosphorus moving from organic to labile pool in watershed(kg P/ha)
    //float wshd_hmp;
	VISITABLE(float, wshd_hmp);

	// @Out
	// @Description nitrogen moving from fresh organic (residue) to nitrate and active organic pools in watershed(kg N/ha)
    //float wshd_rmn;
	VISITABLE(float, wshd_rmn);

	// @Out
	// @Description phosphorus moving from fresh organic (residue) to labile and organic pools in watershed(kg P/ha)
    //float wshd_rmp;
	VISITABLE(float, wshd_rmp);

	// @Out
	// @Description nitrogen moving from active organic to stable organic pool in watershed(kg N/ha)
    //float wshd_rwn;
	VISITABLE(float, wshd_rwn);

	// @Out
	// @Description nitrogen moving from active organic to stable organic pool in watershed(kg N/ha)
    //float wshd_nitn;
	VISITABLE(float, wshd_nitn);

	// @Out
	// @Description nitrogen moving from active organic to stable organic pool in watershed(kg N/ha)
    //float wshd_voln;
	VISITABLE(float, wshd_voln);

	// @Out
	// @Description phosphorus moving from labile mineral to active mineral pool in watershed
    //float wshd_pal;
	VISITABLE(float, wshd_pal);

	// @Out
	// @Description phosphorus moving from active mineral to stable mineral pool in watershed
    //float wshd_pas;
	VISITABLE(float, wshd_pas);

	// @Out
	// @Description factor which converts kg/kg soil to kg/ha, could be used in other nutrient modules
    //float **conv_wt;
	VISITABLE(float **, conv_wt);

	END_VISITABLES;

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
     * \brief estimates daily nitrogen and phosphorus mineralization and immobilization.
     *        considering fresh organic material (plant residue) and active and stable humus material
	 *        Execute when CSWAT = 0, rewrite from nminrl.f of SWAT 
     * \return void
     */
    void Mineralization_StaticCarbonMethod(int i);

    /*!
     * \brief simulates organic C, N, and P cycling in soil using C-FARM one carbon model
     *        Execute when CSWAT = 1, rewrite from carbon_new.f and ndenit.f of SWAT
     * \TODO THIS IS ON THE TODO LIST.
     * \return void
     */
    void Mineralization_CFARMOneCarbonModel(int i);

    /*!
     * \brief simulates organic C, N, and P cycling in soil using CENTURY model
     *        Execute when CSWAT = 2, rewrite from carbon_zhang.f90 and ndenit.f of SWAT
     * \return void
     */
    void Mineralization_CENTURYModel(int i);

    /*!
     * \brief estimates daily mineralization (NH3 to NO3) and volatilization of NH3.
     *        rewrite from nitvol.f of SWAT
     * \return void
     */
    void Volatilization(int i);

    /*!
     * \brief Calculate P flux between the labile, active mineral and stable mineral p pools.
     *        rewrite from pminrl, and pminrl2 according to solP_model
	 * \TODO In current version, the solP_model is set to 0, i.e., pminrl2 is used as default
     * \return void
     */
    void CalculatePflux(int i);

    /// initial outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(Nutrient_Transformation, m_nCells, soillayers, soilLayers, cswat, solP_model, a_days, b_days, tillage_switch, tillage_depth, tillage_days, 
	tillage_factor, cmn, nactfr, sdnco, psp, ssp, cdn, landcover, rsdco_pl, sol_cov, rsdin, SOTE, density, sol_cbn, solst, sol_awc, soilDepth, CLAY, rock, 
	soilthick, sol_aorgn, sol_fon, sol_fop, sol_actp, sol_stap, sol_ul, Porosity, sand, sol_WOC, sol_WON, sol_BM, sol_BMC, sol_BMN, sol_HP, sol_HS, sol_HSC, 
	sol_HSN, sol_HPC, sol_HPN, sol_LM, sol_LMC,sol_LMN, sol_LSC, sol_LSN, sol_LS, sol_LSL, sol_LSLC, sol_LSLNC, sol_RNMN, sol_RSPC, hmntl, hmptl, rmn2tl, 
	rmptl, rwntl, wdntl, rmp1tl, roctl);