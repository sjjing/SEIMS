/*!
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 * \author Liang-Jun Zhu
 * \date June 2016
 *           1. Source code of SWAT include: readmgt.f, operatn.f, sched_mgt.f, plantop.f, harvkillop.f, harvestop.f, killop.f, newtillmix.f, etc.
 *           2. Preliminary implemented version, not include grazing, auto fertilizer, etc. See detail please find the TODOs.
 * \date 2016-9-29
 * \description: 1. Add the CENTURY model related code, mainly include fert.f, newtillmix.f, and harvestop.f
 *               2. Update fertilizer operation for paddy rice, i.e., ExecuteFertilizerOperation()
 */
#pragma once

#include <visit_struct/visit_struct_intrusive.hpp>
#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "Scenario.h"
#include "ClimateParams.h"

using namespace std;
using namespace MainBMP;
/** \defgroup PLTMGT_SWAT
 * \ingroup Management
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 */
/*!
 * \class MGTOpt_SWAT
 * \ingroup PLTMGT_SWAT
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 */
class MGTOpt_SWAT : public SimulationModule {


    /*
    * Plant management factory derived from BMPs Scenario
    * Key is  uniqueBMPID, which is calculated by Landuse_ID * 100 + subScenario;
    * Value is a series of plant management operations
    */
    map<int, BMPPlantMgtFactory *> m_mgtFactory;

    /// valid cell numbers in each subbasin
    map<int, int> m_nCellsSubbsn;

    /// subbasin area, key is subbasinID, value is area (ha)
    map<int, float> m_nAreaSubbsn;

    /// map from LanduseLookup
    map<int, float *> m_landuseLookupMap;

    /// map for CropLookup
    map<int, float *> m_cropLookupMap;

    /// map for FertilizerLookup
    map<int, float *> m_fertilizerLookupMap;

    /// map for TillageLookup
    map<int, float *> m_tillageLookupMap;

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description cell width (m)
    float CELLWIDTH;

    // @In
    // @Description cell area (ha)
    float cellArea;

    // @In
    // @Description the total number of subbasins
    int nSub;   

    /**Parameters from MongoDB**/

    // @In
    // @Description subbasin ID of each cell
    float *subbasin;

    // @In
    // @Description land use
    float *landuse;

    // @In
    // @Description land cover
    float *landcover;

    // @In
    // @Description management unit, e.g., fields
    float *mgt_fields;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description maximum soil layers
    int soilLayers;

    // @In
    // @Description depth to bottom of soil layer, sol_z, mm
    float **soilDepth;

    // @In
    // @Description soil thick
    float **soilthick;

    // @In
    // @Description maximum root depth of soil
    float *SOL_ZMX;

    // @In
    // @Description bulk density of top soil layer in cell, sol_bd, Mg/m^3, i.e., g/cm3        |
    float **density;

    // @In
    // @Description sol_sumfc(:)   |mm H2O        |amount of water held in the soil profile at field capacity (FC-WP)
    float *sol_sumAWC;

    // @In
    // @Description sol_n, initialized by sol_cbn / 11.0 according to readsol.f in SWAT
    float **sol_N;

    // @In
    // @Description soil carbon, sol_cbn
    float **sol_cbn;

    // @In
    // @Description soil rock (%)
    float **rock;

    // @In
    // @Description soil clay (%)
    float **CLAY;

    // @In
    // @Description soil sand (%)
    float **sand;

    // @In
    // @Description soil silt (%)
    float **silt;

    /**soil properties, both as input and ouput**/

    // @In
    // @Description sol_aorgn(:,:)|kg N/ha       |amount of nitrogen stored in the active organic (humic) nitrogen pool
    float **sol_aorgn;

    // @In
    // @Description sol_fon(:,:)  |kg N/ha       |amount of nitrogen stored in the fresh organic (residue) pool
    float **sol_fon;

    // @In
    // @Description sol_fop(:,:)  |kg P/ha       |amount of phosphorus stored in the fresh organic (residue) pool
    float **sol_fop;

    // @In
    // @Description sol_nh4(:,:)  |kg N/ha       |amount of nitrogen stored in the ammonium pool in soil layer
    float **sol_nh4;

    // @In
    // @Description sol_no3(:,:)  |kg N/ha       |amount of nitrogen stored in the nitrate pool in soil layer
    float **sol_no3;

    // @In
    // @Description sol_orgn(:,:) |kg N/ha       |amount of nitrogen stored in the stable organic N pool
    float **sol_orgn;

    // @In
    // @Description sol_orgp(:,:) |kg P/ha       |amount of phosphorus stored in the organic P pool
    float **sol_orgp;

    // @In
    // @Description sol_solp(:,:) |kg P/ha       |amount of inorganic phosphorus stored in solution
    float **sol_solp;

    // @In
    // @Description minimum temperature for plant growth
    float *T_BASE;

    /** Temporary parameters**/

    // @In
    // @Description Sequence number of management operations done in the previous time step run
    int *doneOpSequence;

    /** plant operation related parameters **/

    // @In
    // @Description landuse lookup table
    float **LanduseLookup;

    // @In
    // @Description landuse number
    int landuseNum;

    // @In
    // @Description CN2 values
    float *CN2;

    // @In
    // @Description plant growth code, 0 or 1
    float *IGRO;

    // @In
    // @Description land cover/crop  classification:1-7, i.e., IDC
    float *IDC;

    // @Out
    // @Description Harvest index target, defined in plant operation and used in harvest/kill operation
    float *hi_targ;

    // @Out
    // @Description Biomass target
    float *biotarg;

    // @In
    // @Description current year in rotation to maturity
    float *CURYR_INIT;

    // @In
    // @Description wsyf(:)|(kg/ha)/(kg/ha)|Value of harvest index between 0 and HVSTI, which represents the lowest value expected due to water stress
    float *wsyf;

    // @In
    // @Description the leaf area indices for day i
    float *LAIDAY;

    // @In
    // @Description phu base
    float *PHUBASE;

    // @In
    // @Description phu accumulated
    float *frPHUacc;

    // @In
    // @Description total number of heat units to bring plant to maturity
    float *PHU_PLT;

    // @In
    // @Description dormancy flat, idorm in SWAT, 1 is growing and 0 is dormancy
    float *dormi;

    // @In
    // @Description hvsti(:)|(kg/ha)/(kg/ha)|harvest index: crop yield/aboveground biomass
    float *hvsti;

    // @In
    // @Description
    /// hvstiadj(:) |(kg/ha)/(kg/ha)|optimal harvest index for current time during growing season
    float *hvsti_adj;

    // @In
    // @Description DO NOT KNOW NOW, initialized as 0.
    float *laimaxfr;

    // @In
    // @Description DO NOT KNOW NOW
    float *olai;

    // @In
    // @Description fraction of plant biomass that is nitrogen, pltfr_n in SWAT
    float *frPlantN;

    // @In
    // @Description fraction of plant biomass that is phosphorous, pltfr_p in SWAT
    float *frPlantP;

    // @In
    // @Description amount of nitrogen in plant biomass (kg/ha), plantn in SWAT
    float *plant_N;

    // @In
    // @Description amount of phosphorus in plant biomass (kg/ha), plantp in SWAT
    float *plant_P;

    // @In
    // @Description actual ET simulated during life of plant, plt_et in SWAT
    float *plt_et_tot;

    // @In
    // @Description potential ET simulated during life of plant, plt_pet in SWAT
    float *plt_pet_tot;

    // @In
    // @Description fraction of total plant biomass that is in roots, rwt in SWAT
    float *frRoot;

    // @In
    // @Description land cover/crop biomass (dry weight), bio_ms in SWAT
    float *BIOMASS;

    // @In
    // @Description amount of organic matter in the soil layer classified as residue,sol_rsd(:,:)|kg/ha
    float **sol_rsd;

    // @In
    // @Description fraction of potential plant growth achieved where the reduction is caused by water stress, strsw in SWAT
    float *frStrsWtr;

    /** Crop related parameters **/

    // @In
    // @Description crop lookup table
    float **CropLookup;

    // @In
    // @Description crop number
    int cropNum;
    
    /** Fertilizer related parameters **/

    // @In
    // @Description fertilizer lookup table
    float **FertilizerLookup;

    // @In
    // @Description fertilizer number
    int fertilizerNum;   

    // @In
    // @Description carbon modeling method, 0 Static soil carbon (old mineralization routines), 1 C-FARM one carbon pool model, 2 Century model
    int cswat;

    // @Out
    // @Description C-FARM model, manure organic carbon in soil, kg/ha
    float **sol_mc;

    // @Out
    // @Description C-FARM model, manure organic nitrogen in soil, kg/ha
    float **sol_mn;

    BEGIN_VISITABLES(MGTOpt_SWAT);

    // @Out
    // @Description C-FARM model, manure organic phosphorus in soil, kg/ha
    //float **sol_mp;
    VISITABLE(float **, sol_mp);

    // @In
    // @Description CENTURY model, slow Nitrogen pool in soil, equals to soil active organic n pool in SWAT
    //float **sol_HSN; 
    VISITABLE(float **, sol_HSN);

    // @In
    // @Description CENTURY model, metabolic litter SOM pool
    //float **sol_LM; 
    VISITABLE(float **, sol_LM);

    // @In
    // @Description CENTURY model, metabolic litter C pool
    //float **sol_LMC; 
    VISITABLE(float **, sol_LMC);

    // @In
    // @Description CENTURY model, metabolic litter N pool
    //float **sol_LMN; 
    VISITABLE(float **, sol_LMN);

    // @In
    // @Description CENTURY model, structural litter C pool
    //float **sol_LSC; 
    VISITABLE(float **, sol_LSC);

    // @In
    // @Description CENTURY model, structural litter N pool
    //float **sol_LSN;  
    VISITABLE(float **, sol_LSN);

    // @In
    // @Description CENTURY model, structural litter SOM pool
    //float **sol_LS; 
    VISITABLE(float **, sol_LS);

    // @In
    // @Description CENTURY model, lignin weight in structural litter
    //float **sol_LSL; 
    VISITABLE(float **, sol_LSL);

    // @In
    // @Description CENTURY model, lignin amount in structural litter pool
    //float **sol_LSLC; 
    VISITABLE(float **, sol_LSLC);

    // @In
    // @Description CENTURY model, non-lignin part of the structural litter C
    //float **sol_LSLNC; 
    VISITABLE(float **, sol_LSLNC);

    // @Out
    // @Description tillage factor on SOM decomposition, used by CENTURY model
    //float *tillage_switch;
    VISITABLE(float *, tillage_switch);

    // @Out
    // @Description tillage factor on SOM decomposition, used by CENTURY model
    //float *tillage_depth;
    VISITABLE(float *, tillage_depth);

    // @Out
    // @Description tillage factor on SOM decomposition, used by CENTURY model
    //float *tillage_days;
    VISITABLE(float *, tillage_days);

    // @Out
    // @Description tillage factor on SOM decomposition, used by CENTURY model
    //float *tillage_factor;
    VISITABLE(float *, tillage_factor);

    // @In
    // @Description NEED to figure out
    //float **sol_BMN; 
    VISITABLE(float **, sol_BMN);

    // @In
    // @Description mass of N present in passive humus
    //float **sol_HPN; 
    VISITABLE(float **, sol_HPN);

    /** Irrigation operation related **/

    // @Out
    // @Description irrigation flag
    //float *irr_flag;
    VISITABLE(float *, irr_flag);

    // @Out
    // @Description aird(:) |mm H2O |amount of water applied to cell on current day
    //float *irr_water;
    VISITABLE(float *, irr_water);

    // @Out
    // @Description qird(:) |mm H2O |amount of water from irrigation to become surface runoff
    //float *irr_surfq;
    VISITABLE(float *, irr_surfq);

    // @In
    // @Description potsa(:) |ha |surface area of impounded water body
    //float *pot_sa;
    VISITABLE(float *, pot_sa);

    // @In
    // @Description deepirr(:) |mm H2O |amount of water removed from deep aquifer for irrigation
    //float *m_deepIrrWater;
    VISITABLE(float *, m_deepIrrWater);

    // @In
    // @Description shallirr(:) |mm H2O |amount of water removed from shallow aquifer for irrigation
    //float *m_shallowIrrWater;
    VISITABLE(float *, m_shallowIrrWater);

    /** auto irrigation operation related**/

    // @Out
    // @Description Water stress identifier, 1 plant water demand, 2 soil water content
    //float *awtr_strsID;
    VISITABLE(float *, awtr_strsID);

    // @Out
    // @Description Water stress threshold that triggers irrigation, if m_wtrStresID is 1, the value usually 0.90 ~ 0.95
    //float *awtr_strsTrig;
    VISITABLE(float *, awtr_strsTrig);

    // @Out
    // @Description irrigation source
    //float *airr_source;
    VISITABLE(float *, airr_source);

    // @Out
    // @Description irrigation source location code
    //float *airr_location;
    VISITABLE(float *, airr_location);

    // @Out
    // @Description auto irrigation efficiency, 0 ~ 100, IRR_EFF
    //float *airr_eff;
    VISITABLE(float *, airr_eff);

    // @Out
    // @Description amount of irrigation water applied each time auto irrigation is triggered (mm), 0 ~ 100, IRR_MX
    //float *airrwtr_depth;
    VISITABLE(float *, airrwtr_depth);

    // @Out
    // @Description surface runoff ratio (0-1) (0.1 is 10% surface runoff), IRR_ASQ
    //float *airrsurf_ratio;
    VISITABLE(float *, airrsurf_ratio);

    ///**Bacteria related**/
    //////// TODO, bacteria modeling will be implemented in the future. (bacteria.f)
    ///// fraction of manure containing active colony forming units (cfu)
    //float m_bactSwf;
    /////  bactlp_plt(:) |# cfu/m^2     |less persistent bacteria on foliage
    //float* m_bactLessPersistPlt;
    /////  bactlpq(:)    |# cfu/m^2     |less persistent bacteria in soil solution
    //float* m_bactLessPersistSol;
    /////  bactlps(:)    |# cfu/m^2     |less persistent bacteria attached to soil particles
    //float* m_bactLessPersistParticle;
    /////  bactp_plt(:)  |# cfu/m^2     |persistent bacteria on foliage
    //float* m_bactPersistPlt;
    /////  bactpq(:)     |# cfu/m^2     |persistent bacteria in soil solution
    //float* m_bactPersistSol;
    /////  bactps(:)     |# cfu/m^2     |persistent bacteria attached to soil particles
    //float* m_bactPersistParticle;

    /** HarvestKill, Harvest, and Kill operation related  **/

    // @In
    // @Description stsol_rd(:) |mm  |storing last soil root depth for use in harvestkillop/killop /// defined in swu.f
    //float *lastSoilRD;
    VISITABLE(float *, lastSoilRD);

    // @In
    // @Description Daily carbon change by different means (entire soil profile for each cell), initialized as 0, 1 harvest, 2 harvestkill, 3 harvgrain op, 1,2,3 
    //float *grainc_d; 
    VISITABLE(float *, grainc_d);

    // @In
    // @Description Daily carbon change by different means (entire soil profile for each cell), initialized as 0, 1 harvest, 2 harvestkill, 3 harvgrain op, 1,2
    //float *m_rsdc_d; 
    VISITABLE(float *, m_rsdc_d);

    // @In
    // @Description Daily carbon change by different means (entire soil profile for each cell), initialized as 0, 1 harvest, 2 harvestkill, 3 harvgrain op, 2
    //float *m_stoverc_d;
    VISITABLE(float *, m_stoverc_d);

    //float *m_sedc_d;	
    //float *m_surfqc_d;	
    //float *m_latc_d;	
    //float *m_percc_d;	
    //float *m_foc_d;
    //float *m_NPPC_d;
    //float *m_soc_d;
    //float *m_rspc_d;
    //float *m_emitc_d; // include biomass_c eaten by grazing, burnt

    /** tillage operation related **/

    // @In
    // @Description tillage lookup table
    //float **TillageLookup;
    VISITABLE(float **, TillageLookup);

    // @In
    // @Description tillage number
    //int tillageNum;
    VISITABLE(int, tillageNum);
    
    // @In
    // @Description sol_actp(:,:) |kg P/ha |amount of phosphorus stored in the active mineral phosphorus pool
    //float **sol_actp;
    VISITABLE(float **, sol_actp);

    // @In
    // @Description sol_stap(:,:) |kg P/ha       |amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool
    //float **sol_stap;
    VISITABLE(float **, sol_stap);

    /**auto fertilizer operation**/


    // @Out
    // @Description fertilizer ID from fertilizer database
    //float *afert_id;
    VISITABLE(float *, afert_id);

    // @Out
    // @Description Code for approach used to determine amount of nitrogen to Cell, 0 Nitrogen target approach, 1 annual max approach
    //float *afert_nstrsID;
    VISITABLE(float *, afert_nstrsID);

    // @Out
    // @Description Nitrogen stress factor of cover/plant that triggers fertilization, usually set 0.90 to 0.95
    //float *afert_nstrs;
    VISITABLE(float *, afert_nstrs);

    // @Out
    // @Description Maximum amount of mineral N allowed in any one application (kg N/ha), auto_napp
    //float *afert_maxN;
    VISITABLE(float *, afert_maxN);

    // @Out
    // @Description Maximum amount of mineral N allowed to be applied in any one year (kg N/ha), auto_nyr
    //float *afert_AmaxN;
    VISITABLE(float *, afert_AmaxN);

    // @Out
    // @Description modifier for auto fertilization target nitrogen content, tnylda
    //float *afert_nyldTarg;
    VISITABLE(float *, afert_nyldTarg);

    // @Out
    // @Description auto_eff(:) |none |fertilizer application efficiency calculated as the amount of N applied divided by the amount of N removed at harvest
    //float *afert_frteff;
    VISITABLE(float *, afert_frteff);

    // @Out
    // @Description Fraction of fertilizer applied to top 10mm of soil, the default is 0.2
    //float *afert_frtsurf;
    VISITABLE(float *, afert_frtsurf);

    /** Grazing operation **/

    // @Out
    // @Description ndeat(:) |days |number of days cell has been grazed
    //float *grz_days;
    VISITABLE(float *, grz_days);

    // @Out
    // @Description igrz(:) |none |grazing flag for cell: 0 cell currently not grazed, 1 cell currently grazed
    //float *grz_flag;
    VISITABLE(float *, grz_flag);

    /** Release or Impound Operation **/

    // @Out
    // @Description release/impound action code: 0 begin impounding water, 1 release impounded water
    //float *impound_trig;
    VISITABLE(float *, impound_trig);

    // @In
    // @Description volume of water stored in the depression/impounded area, mm
    //float *pot_vol;
    VISITABLE(float *, pot_vol);

    // @Out
    // @Description maximum volume of water stored in the depression/impounded area, mm
    //float *pot_volmaxmm;
    VISITABLE(float *, pot_volmaxmm);

    // @Out
    // @Description low depth ...., mm
    //float *pot_vollowmm;
    VISITABLE(float *, pot_vollowmm);

    // @In
    // @Description no3 amount kg
    //float *pot_no3;
    VISITABLE(float *, pot_no3);

    // @In
    // @Description nh4 amount kg
    //float *pot_nh4;
    VISITABLE(float *, pot_nh4);

    // @In
    // @Description soluble phosphorus amount, kg
    //float *pot_solp;
    VISITABLE(float *, pot_solp);

    // @In
    // @Description field capacity (FC-WP), mm
    //float **sol_awc;
    VISITABLE(float **, sol_awc);

    // @In
    // @Description amount of water held in the soil layer at saturation (sat - wp water), mm
    //float **sol_ul;
    VISITABLE(float **, sol_ul);

    // @In
    // @Description soil water storage (mm)
    //float **solst;
    VISITABLE(float **, solst);

    // @In
    // @Description soil water storage in soil profile (mm)
    //float *solsw;
    VISITABLE(float *, solsw);

    // @In
    // @Description flag to identify the initialization
    //bool initialized;
    VISITABLE(bool, initialized);

    END_VISITABLES;

private:
    // @In
    // @Description deep and shallow aquifer are not distinguished in SEIMS, both are set to m_SBGS, deepst(:) |mm H2O |depth of water in deep aquifer
    float *m_deepWaterDepth;

    // @In
    // @Description deep and shallow aquifer are not distinguished in SEIMS, both are set to m_SBGS, shallst | mm H2O |depth of water in shallow aquifer
    float *m_shallowWaterDepth;

public:
    //! Constructor
    MGTOpt_SWAT(void);

    //! Destructor
    ~MGTOpt_SWAT(void);

    int Execute(void);

    void SetValue(const char *key, float data);

    void Set1DData(const char *key, int n, float *data);

    void Get1DData(const char *key, int *n, float **data);

    void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    void Set2DData(const char *key, int n, int col, float **data);

    void SetScenario(Scenario *sce);

    void SetSubbasins(clsSubbasins *subbasins);

private:
    /*!
     * \brief Get operation parameters according to operation sequence number
     * \param[in] cellIdx current cell index
     * \param[out] factoryID Index of Plant BMPs factory
     * \param[out] nOps Operation sequence number, and there might be several operation occurred on one day
     */
    bool GetOperationCode(int cellIdx, int &factoryID, vector<int> &nOps);

    /*!
     * \brief Manager all operations on schedule
     * \param[in] cellIdx Index of valid cell
     * \param[in] factoryID Index of Plant BMPs factory
     * \param[in] nOp Operation sequence
     */
    void ScheduledManagement(int cellIdx, int &factoryID, int nOp);

    void ExecutePlantOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteIrrigationOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteFertilizerOperation(int cellIdx, int &factoryID, int nOp);

    void ExecutePesticideOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteHarvestKillOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteTillageOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteHarvestOnlyOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteKillOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteGrazingOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteAutoIrrigationOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteAutoFertilizerOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteReleaseImpoundOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteContinuousFertilizerOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteContinuousPesticideOperation(int cellIdx, int &factoryID, int nOp);

    void ExecuteBurningOperation(int cellIdx, int &factoryID, int nOp);

    /*!
     * \brief check the input data. Make sure all the input data is available.
     * \return bool The validity of the input data.
     */
    bool CheckInputData(void);

    /*!
     * \brief check the input size. Make sure all the input data have same dimension.
     *
     *
     * \param[in] key The key of the input data
     * \param[in] n The input data dimension
     * \return bool The validity of the dimension
     */
    bool CheckInputSize(const char *, int);

    /*!
     * \brief check the input size of 2D data. Make sure all the input data have same dimension.
     *
     *
     * \param[in] key The key of the input data
     * \param[in] n The first dimension input data 
     * \param[in] col The second dimension of input data
     * \return bool The validity of the dimension
     */
    bool CheckInputSize2D(const char *key, int n, int col);

    /// initialize all possible outputs
    void initialOutputs(void);

    /// Handle lookup tables ///

    /// landuse lookup table
    void initializeLanduseLookup(void);

    /// crop lookup table
    void initializeCropLookup(void);

    /// fertilizer lookup table
    void initializeFertilizerLookup(void);

    /// tillage lookup table
    void initializeTillageLookup(void);

    /// the complementary error function
    float Erfc(float xx);

    /// distributes dead root mass through the soil profile
    void rootFraction(int i, float *&root_fr);
};

VISITABLE_STRUCT(MGTOpt_SWAT, m_nCells, m_deepWaterDepth, m_shallowWaterDepth, CELLWIDTH, cellArea, nSub, subbasin, landuse, landcover, 
	mgt_fields, soillayers, soilLayers, soilDepth, soilthick, SOL_ZMX, density, sol_sumAWC, sol_N, sol_cbn, rock, CLAY, sand, silt,
	sol_aorgn, sol_fon, sol_fop, sol_nh4, sol_no3, sol_orgn, sol_orgp, sol_solp, T_BASE, doneOpSequence, LanduseLookup, landuseNum, 
	CN2, IGRO, IDC, hi_targ, biotarg, CURYR_INIT, wsyf, LAIDAY, PHUBASE, frPHUacc, PHU_PLT, dormi, hvsti, hvsti_adj, laimaxfr, olai,
	frPlantN, frPlantP, plant_N, plant_P, plt_et_tot, plt_pet_tot, frRoot, BIOMASS, sol_rsd, frStrsWtr, CropLookup, cropNum, FertilizerLookup,
	fertilizerNum, cswat, sol_mc, sol_mn);