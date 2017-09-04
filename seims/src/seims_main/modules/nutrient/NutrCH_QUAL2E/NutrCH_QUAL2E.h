/*!
 * \brief Calculates in-stream nutrient transformations with QUAL2E method.
 *        watqual2.f of SWAT
 * \author Huiran Gao; Junzhi Liu
 * \date Jun 2016
 *
 * \revision LiangJun Zhu
 * \description 1. Add point source loadings nutrients from Scenario.
 *              2. Add ammonian transported by surface runoff
 */

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup NutrCH_QUAL2E
 * \ingroup Nutrient
 * \brief Calculates in-stream nutrient transformations with QUAL2E method.
 */

/*!
 * \class NutrCH_QUAL2E
 * \ingroup NutrCH_QUAL2E
 *
 * \brief Calculates the concentration of nutrient in reach using QUAL2E method.
 *
 */
class NutrCH_QUAL2E : public SimulationModule {
public:
    NutrCH_QUAL2E(void);

    ~NutrCH_QUAL2E(void);

    virtual void SetValue(const char *key, float value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void SetReaches(clsReaches *reaches);

    virtual void SetScenario(Scenario *sce);

    virtual int Execute(void);

    virtual void GetValue(const char *key, float *value);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual TimeStepType GetTimeStepType(void) { return TIMESTEP_CHANNEL; };

	/// upstream id (The value is -1 if there if no upstream reach)
	vector <vector<int>> m_reachUpStream;

	/// id the reaches
	vector<int> m_reachId;

	/* reach up-down layering
	* key: stream order
	* value: reach ID of current stream order
	*/
	map<int, vector<int> > m_reachLayers;

	/* point source operations
	* key: unique index, BMPID * 100000 + subScenarioID
	* value: point source management factory instance
	*/
	map<int, BMPPointSrcFactory *> m_ptSrcFactory;

    // @In
    // @Description cell number
    int m_nCells;

    // @In
    // @Description time step (sec)
    int DT_CH;

	// @In
	// @Description downstream id (The value is 0 if there if no downstream reach)
    float *reachDownStream;
   
	// @In
	// @Description reaches number
    int nReaches;
   
	// @In
	// @Description input data
    float QUPREACH;

	// @In
    // @Description ratio of chlorophyll-a to algal biomass (ug chla/mg alg)
    float ai0;     
    
	// @In
    // @Description fraction of algal biomass that is nitrogen (mg N/mg alg)
	float ai1;

	// @In
	// @Description fraction of algal biomass that is phosphorus (mg P/mg alg)
    float ai2; 

	// @In
	// @Description the rate of oxygen production per unit of algal photosynthesis (mg O2/mg alg)
    float ai3;   

	// @In
	// @Description the rate of oxygen uptake per unit of algae respiration (mg O2/mg alg)
    float ai4;  

	// @In
	// @Description the rate of oxygen uptake per unit of NH3 nitrogen oxidation (mg O2/mg N)
    float ai5; 

	// @In
	// @Description the rate of oxygen uptake per unit of NO2 nitrogen oxidation (mg O2/mg N)
    float ai6;  

	// @In
	// @Description non-algal portion of the light extinction coefficient
    float lambda0;   

	// @In
	// @Description linear algal self-shading coefficient
    float lambda1;  

	// @In
	// @Description nonlinear algal self-shading coefficient
    float lambda2;   

	// @In
	// @Description half saturation coefficient for light (MJ/(m2*hr))
    float k_l;

	// @In
	// @Description half-saturation constant for nitrogen (mg N/L)
    float k_n;      

	// @In
	// @Description half saturation constant for phosphorus (mg P/L)
    float k_p;       

	// @In
	// @Description algal preference factor for ammonia
	float p_n;

	// @In
	// @Description fraction of solar radiation computed in the temperature heat balance that is photo synthetically active
    float tfact;

	// @In
	// @Description fraction of overland flow
    float rnum1;

	// @In
	// @Description option for calculating the local specific growth rate of algae, 1: multiplicative: u = mumax * fll * fnn * fpp, 2: limiting nutrient: u = mumax * fll * Min(fnn, fpp), 3: harmonic mean: u = mumax * fll * 2. / ((1/fnn)+(1/fpp)) 
    int igropt;

	// @In
	// @Description maximum specific algal growth rate at 20 deg C
    float mumax;

	// @In
	// @Description algal respiration rate at 20 deg C (1/day)
    float rhoq;

	// @In
	// @Description Conversion factor
    float cod_n;

	// @In
	// @Description Reaction coefficient
    float cod_k;

	// @In
	// @Description stream link
    float *STREAM_LINK;

	// @In
	// @Description soil temperature (deg C)
    float *SOTE;

	// @In
	// @Description day length for current day (h)
    float *daylength;

	// @In
	// @Description solar radiation for the day (MJ/m2)
    float *SR;

	// @In
	// @Description bank storage
    float *BKST;

	// @In
	// @Description order of channel
    float *chOrder;

	// @In
	// @Description channel outflow
    float *QRECH;

	// @In
	// @Description reach storage (m3) at time t
    float *CHST;

	// @In
	// @Description reach storage of previous timestep
    float *preCHST;

	// @In
	// @Description channel water depth m
    float *CHWTDEPTH;

	// @In
	// @Description channel water depth of previous timestep, m
    float *prechwtdepth;

	// @In
	// @Description temperature of water in reach (deg C)
    float *wattemp;

	// @In
	// @Description rate constant for biological oxidation of NH3 to NO2 in reach at 20 deg C
    float *bc1;   

	// @In
	// @Description rate constant for biological oxidation of NO2 to NO3 in reach at 20 deg C
    float *bc2;       

	// @In
	// @Description rate constant for biological oxidation of organic N to ammonia in reach at 20 deg C
    float *bc3;      

	// @In
	// @Description rate constant for biological oxidation of organic P to dissolved P in reach at 20 deg C
    float *bc4;     

	// @In
	// @Description local algal settling rate in reach at 20 deg C (m/day)
    float *rs1;      

	// @In
	// @Description benthos source rate for dissolved phosphorus in reach at 20 deg C (mg disP-P)/((m**2)*day)
    float *rs2;     

	// @In
	// @Description benthos source rate for ammonia nitrogen in reach at 20 deg C (mg NH4-N)/((m**2)*day)
    float *rs3;       

	// @In
	// @Description rate coefficient for organic nitrogen settling in reach at 20 deg C (1/day)
    float *rs4;  

	// @In
	// @Description organic phosphorus settling rate in reach at 20 deg C (1/day)
    float *rs5;  

	// @In
	// @Description CBOD deoxygenation rate coefficient in reach at 20 deg C (1/day)
    float *rk1;      

	// @In
	// @Description reaeration rate in accordance with Fickian diffusion in reach at 20 deg C (1/day)
    float *rk2;    

	// @In
	// @Description rate of loss of CBOD due to settling in reach at 20 deg C (1/day)
    float *rk3;  

	// @In
	// @Description sediment oxygen demand rate in reach at 20 deg C (mg O2/ ((m**2)*day))
    float *rk4;    

	// @In
	// @Description Channel organic nitrogen concentration in basin, ppm
    float ch_onco;

	// @In
	// @Description Channel organic phosphorus concentration in basin, ppm
    float ch_opco;

	// @In
	// @Description amount of nitrate transported with lateral flow
    float *latno3ToCh;

	// @In
	// @Description amount of nitrate transported with surface runoff
    float *sur_no3_ToCh;

	// @In
	// @Description amount of ammonian transported with surface runoff
    float *SUR_NH4_TOCH;

	// @In
	// @Description amount of soluble phosphorus in surface runoff
    float *sur_solp_ToCh;

	// @In
	// @Description cod to reach in surface runoff (kg)
    float *sur_cod_ToCH;

	// @In
	// @Description nitrate loading to reach in groundwater
    float *no3gwToCh;

	// @In
	// @Description soluble P loading to reach in groundwater
    float *minpgwToCh;

	// @In
	// @Description amount of organic nitrogen in surface runoff
    float *sedorgnToCh;

	// @In
	// @Description amount of organic phosphorus in surface runoff
    float *sedorgpToCh;

	// @In
	// @Description amount of active mineral phosphorus absorbed to sediment in surface runoff
    float *sedminpaToCh;

	// @In
	// @Description amount of stable mineral phosphorus absorbed to sediment in surface runoff
    float *sedminpsToCh;

	// @In
	// @Description amount of nitrite transported with lateral flow
    float *nitriteToCh;

    /// point source loadings (kg) to channel of each timestep

	// @Out
	// @Description nitrate
    float *ptNO3ToCh;

	// @In
	// @Description ammonia nitrogen
    float *ptNH4ToCh;

	// @In
	// @Description Organic nitrogen
    float *ptOrgNToCh;

	// @Out
	// @Description total nitrogen
    float *ptTNToCh;

	// @In
	// @Description soluble (dissolved) phosphorus
    float *ptSolPToCh;

	// @In
	// @Description Organic phosphorus
    float *ptOrgPToCh;

	// @Out
	// @Description total phosphorus
    float *ptTPToCh;

	// @Out
	// @Description COD
    float *ptCODToCh;

	// @In
	// @Description channel erosion
    float *rch_deg;

    /// nutrient amount stored in reach

	// @In
	// @Description algal biomass storage in reach (kg)
    float *chAlgae;

	// @In
	// @Description organic nitrogen storage in reach (kg)
    float *chOrgN;

	// @Out
	// @Description ammonia storage in reach (kg)
    float *CHSTR_NH4;

	// @In
	// @Description nitrite storage in reach (kg)
    float *chNO2;

	// @Out
	// @Description nitrate storage in reach (kg)
    float *CHSTR_NO3;

	// @Out
	// @Description total nitrogen in reach (kg)
    float *CHSTR_TN;

	// @In
	// @Description organic phosphorus storage in reach (kg)
    float *chOrgP;

	// @In
	// @Description dissolved phosphorus storage in reach (kg)
    float *chSolP;

	// @Out
	// @Description total phosphorus storage in reach (kg)
    float *CHSTR_TP;

	// @In
	// @Description carbonaceous oxygen demand in reach (kg)
    float *chCOD;

	// @In
	// @Description dissolved oxygen storage in reach (kg)
    float *chDOx;

	// @In
	// @Description chlorophyll-a storage in reach (kg)
    float *chChlora;

	// @In
	// @Description saturation storage of dissolved oxygen (kg)
    float soxy;

    /// Outputs, both amount (kg) and concentration (mg/L)

	// @Out
	// @Description algal biomass amount in reach (kg)
    float *ch_algae;

	// @Out
	// @Description algal biomass concentration in reach (mg/L)
    float *ch_algaeConc;

	// @Out
	// @Description chlorophyll-a biomass amount in reach (kg)
    float *CH_chlora;

	// @Out
	// @Description chlorophyll-a biomass concentration in reach (mg/L)
    float *CH_chloraConc;

	// @Out
	// @Description organic nitrogen amount in reach (kg)
    float *CH_ORGN;

	// @Out
	// @Description organic nitrogen concentration in reach (mg/L)
    float *CH_ORGNConc;

	// @Out
	// @Description organic phosphorus amount in reach (kg)
    float *CH_ORGP;

	// @Out
	// @Description organic phosphorus concentration in reach (mg/L)
    float *CH_ORGPConc;

	// @Out
	// @Description ammonia amount in reach (kg)
    float *ch_nh4;

	// @Out
	// @Description ammonia concentration in reach (mg/L)
    float *ch_nh4Conc;

	// @Out
	// @Description nitrite amount in reach (kg)
    float *CH_NO2;

	// @Out
	// @Description nitrite concentration in reach (mg/L)
    float *CH_NO2Conc;

	// @Out
	// @Description nitrate amount in reach (kg)
    float *CH_NO3;

	// @Out
	// @Description nitrate concentration in reach (mg/L)
    float *CH_NO3Conc;

	// @Out
	// @Description dissolved phosphorus amount in reach (kg)
    float *CH_SOLP;

	// @Out
	// @Description dissolved phosphorus concentration in reach (mg/L)
    float *CH_SOLPConc;

	// @Out
	// @Description carbonaceous oxygen demand in reach (kg)
    float *CH_COD;

	// @Out
	// @Description carbonaceous oxygen demand concentration in reach (mg/L)
    float *CH_CODConc;

	// @Out
	// @Description dissolved oxygen amount in reach (kg)
    float *ch_dox;

	// @Out
	// @Description dissolved oxygen concentration in reach (mg/L)
    float *ch_doxConc;

	// @Out
	// @Descriptiontotal N amount in reach (kg)
    float *CH_TN;

	// @Out
	// @Description total N concentration in reach (mg/L)
    float *CH_TNConc;

	// @Out
	// @Description total P amount in reach (kg)
    float *CH_TP;

	// @Out
	// @Description total P concentration in reach (mg/L)
    float *CH_TPConc;

    //intermediate variables

	// @In
	// @Description mean day length of each channel (hr)
    float *chDaylen;

	// @In
	// @Description mean solar radiation of each channel
    float *chSr;

	// @In
	// @Description valid cell numbers of each channel
    int *chCellCount;

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

    bool CheckInputCellSize(const char *key, int n);

    void AddInputNutrient(int);

    void RouteOut(int i);

    void NutrientTransform(int i);

    /*!
    * \brief Corrects rate constants for temperature.
     *
     *    r20         1/day         value of the reaction rate coefficient at the standard temperature (20 degrees C)
     *    thk         none          temperature adjustment factor (empirical constant for each reaction coefficient)
     *    tmp         deg C         temperature on current day
     *
     * \return float
     */
    float corTempc(float r20, float thk, float tmp);

    /// Calculate average day length, solar radiation, and temperature for each channel
    void ParametersSubbasinForChannel(void);

    void initialOutputs(void);

    void PointSourceLoading(void);
};

VISITABLE_STRUCT();