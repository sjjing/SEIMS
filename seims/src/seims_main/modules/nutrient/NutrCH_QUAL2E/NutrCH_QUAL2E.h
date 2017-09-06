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

#include <visit_struct/visit_struct_intrusive.hpp>
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

    BEGIN_VISITABLES(NutrCH_QUAL2E);

    // @In
    // @Description cell number
    //int m_nCells;
    VISITABLE(int, m_nCells);

    // @In
    // @Description time step (sec)
    //int DT_CH;
    VISITABLE(int, DT_CH);

    // @In
    // @Description input data
    //float QUPREACH;
    VISITABLE(float, QUPREACH);

    // @In
    // @Description ratio of chlorophyll-a to algal biomass (ug chla/mg alg)
    //float ai0;     
    VISITABLE(float, ai0);

    // @In
    // @Description fraction of algal biomass that is nitrogen (mg N/mg alg)
    //float ai1;
    VISITABLE(float, ai1);

    // @In
    // @Description fraction of algal biomass that is phosphorus (mg P/mg alg)
    //float ai2; 
    VISITABLE(float, ai2);

    // @In
    // @Description the rate of oxygen production per unit of algal photosynthesis (mg O2/mg alg)
    //float ai3;   
    VISITABLE(float, ai3);

    // @In
    // @Description the rate of oxygen uptake per unit of algae respiration (mg O2/mg alg)
    //float ai4;  
    VISITABLE(float, ai4);

    // @In
    // @Description the rate of oxygen uptake per unit of NH3 nitrogen oxidation (mg O2/mg N)
    //float ai5; 
    VISITABLE(float, ai5);

    // @In
    // @Description the rate of oxygen uptake per unit of NO2 nitrogen oxidation (mg O2/mg N)
    //float ai6;  
    VISITABLE(float, ai6);

    // @In
    // @Description non-algal portion of the light extinction coefficient
    //float lambda0;   
    VISITABLE(float, lambda0);

    // @In
    // @Description linear algal self-shading coefficient
    //float lambda1;  
    VISITABLE(float, lambda1);

    // @In
    // @Description nonlinear algal self-shading coefficient
    //float lambda2;   
    VISITABLE(float, lambda2);

    // @In
    // @Description half saturation coefficient for light (MJ/(m2*hr))
    //float k_l;
    VISITABLE(float, k_l);

    // @In
    // @Description half-saturation constant for nitrogen (mg N/L)
    //float k_n;      
    VISITABLE(float, k_n);

    // @In
    // @Description half saturation constant for phosphorus (mg P/L)
    //float k_p;       
    VISITABLE(float, k_p);

    // @In
    // @Description algal preference factor for ammonia
    //float p_n;
    VISITABLE(float, p_n);

    // @In
    // @Description fraction of solar radiation computed in the temperature heat balance that is photo synthetically active
    //float tfact;
    VISITABLE(float, tfact);

    // @In
    // @Description fraction of overland flow
    //float rnum1;
    VISITABLE(float, rnum1);

    // @In
    // @Description option for calculating the local specific growth rate of algae, 1: multiplicative: u = mumax * fll * fnn * fpp, 2: limiting nutrient: u = mumax * fll * Min(fnn, fpp), 3: harmonic mean: u = mumax * fll * 2. / ((1/fnn)+(1/fpp)) 
    //int igropt;
    VISITABLE(int, igropt);

    // @In
    // @Description maximum specific algal growth rate at 20 deg C
    //float mumax;
    VISITABLE(float, mumax);

    // @In
    // @Description algal respiration rate at 20 deg C (1/day)
    //float rhoq;
    VISITABLE(float, rhoq);

    // @In
    // @Description Conversion factor
    //float cod_n;
    VISITABLE(float, cod_n);

    // @In
    // @Description Reaction coefficient
    //float cod_k;
    VISITABLE(float, cod_k);

    // @In
    // @Description stream link
    //float *STREAM_LINK;
    VISITABLE(float *, STREAM_LINK);

    // @In
    // @Description soil temperature (deg C)
    //float *SOTE;
    VISITABLE(float *, SOTE);

    // @In
    // @Description day length for current day (h)
    //float *daylength;
    VISITABLE(float *, daylength);

    // @In
    // @Description solar radiation for the day (MJ/m2)
    //float *SR;
    VISITABLE(float *, SR);

    // @In
    // @Description bank storage
    //float *BKST;
    VISITABLE(float *, BKST);

    // @In
    // @Description channel outflow
    //float *QRECH;
    VISITABLE(float *, QRECH);

    // @In
    // @Description reach storage (m3) at time t
    //float *CHST;
    VISITABLE(float *, CHST);

    // @In
    // @Description reach storage of previous timestep
    //float *preCHST;
    VISITABLE(float *, preCHST);

    // @In
    // @Description channel water depth m
    //float *CHWTDEPTH;
    VISITABLE(float *, CHWTDEPTH);

    // @In
    // @Description channel water depth of previous timestep, m
    //float *prechwtdepth;
    VISITABLE(float *, prechwtdepth);

    // @In
    // @Description temperature of water in reach (deg C)
    //float *wattemp;
    VISITABLE(float *, wattemp);

    END_VISITABLES;

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

    // @Out
    // @Description ammonia storage in reach (kg)
    float *CHSTR_NH4;

    // @Out
    // @Description nitrate storage in reach (kg)
    float *CHSTR_NO3;

    // @Out
    // @Description total nitrogen in reach (kg)
    float *CHSTR_TN;

    // @Out
    // @Description total phosphorus storage in reach (kg)
    float *CHSTR_TP;

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

private:

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

    // downstream id (The value is 0 if there if no downstream reach)
    float *reachDownStream;  

    // reaches number
    int nReaches; 

    // order of channel
    float *chOrder;

    // rate constant for biological oxidation of NH3 to NO2 in reach at 20 deg C
    float *bc1;

    // rate constant for biological oxidation of NO2 to NO3 in reach at 20 deg C
    float *bc2;         

    // rate constant for biological oxidation of organic N to ammonia in reach at 20 deg C
    float *bc3;      

    // rate constant for biological oxidation of organic P to dissolved P in reach at 20 deg C
    float *bc4;     

    // local algal settling rate in reach at 20 deg C (m/day)
    float *rs1;      

    // benthos source rate for dissolved phosphorus in reach at 20 deg C (mg disP-P)/((m**2)*day)
    float *rs2;     

    // benthos source rate for ammonia nitrogen in reach at 20 deg C (mg NH4-N)/((m**2)*day)
    float *rs3;      

    // rate coefficient for organic nitrogen settling in reach at 20 deg C (1/day)
    float *rs4;  

    // organic phosphorus settling rate in reach at 20 deg C (1/day)
    float *rs5;     

    // CBOD deoxygenation rate coefficient in reach at 20 deg C (1/day)
    float *rk1;    

    // reaeration rate in accordance with Fickian diffusion in reach at 20 deg C (1/day)
    float *rk2;       

    // rate of loss of CBOD due to settling in reach at 20 deg C (1/day)
    float *rk3;  

    // sediment oxygen demand rate in reach at 20 deg C (mg O2/ ((m**2)*day))
    float *rk4;

    /// nutrient amount stored in reach

    // algal biomass storage in reach (kg)
    float *chAlgae;

    // organic nitrogen storage in reach (kg)
    float *chOrgN;

    // nitrite storage in reach (kg)
    float *chNO2;

    // organic phosphorus storage in reach (kg)
    float *chOrgP;

    // dissolved phosphorus storage in reach (kg)
    float *chSolP;

    // carbonaceous oxygen demand in reach (kg)
    float *chCOD;

    // dissolved oxygen storage in reach (kg)
    float *chDOx;

    //intermediate variables

    // mean day length of each channel (hr)
    float *chDaylen;

    // mean solar radiation of each channel
    float *chSr;

    // valid cell numbers of each channel
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

VISITABLE_STRUCT(NutrCH_QUAL2E, ch_onco, ch_opco, latno3ToCh, sur_no3_ToCh, SUR_NH4_TOCH, sur_solp_ToCh, sur_cod_ToCH, no3gwToCh, 
    minpgwToCh, sedorgnToCh, sedorgpToCh, sedminpaToCh, sedminpsToCh, nitriteToCh, ptNO3ToCh, ptNH4ToCh, ptOrgNToCh, ptTNToCh, ptSolPToCh, 
    ptOrgPToCh, ptTPToCh, ptCODToCh, rch_deg, CHSTR_NH4, CHSTR_NO3, CHSTR_TN, CHSTR_TP, chChlora, soxy, ch_algae, ch_algaeConc, CH_chlora,
    CH_chloraConc, CH_ORGN, CH_ORGNConc, CH_ORGP, CH_ORGPConc, ch_nh4, ch_nh4Conc, CH_NO2, CH_NO2Conc, CH_NO3, CH_NO3Conc, CH_SOLP, 
    CH_SOLPConc, CH_COD, CH_CODConc, ch_dox, ch_doxConc, CH_TN, CH_TNConc, CH_TP, CH_TPConc);