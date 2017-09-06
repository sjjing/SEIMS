#include "seims.h"
#include "PlantGrowthCommon.h"

#include "managementOperation_SWAT.h"

using namespace std;

MGTOpt_SWAT::MGTOpt_SWAT(void) : m_nCells(-1), nSub(-1), nSoilLayers(-1),
                                 CELLWIDTH(NODATA_VALUE), cellArea(NODATA_VALUE),
    /// add parameters from MongoDB
                                 subbasin(NULL), landuse(NULL), landcover(NULL), mgt_fields(NULL), T_BASE(NULL),
    /// Soil related parameters from MongoDB
                                 soillayers(NULL), soilDepth(NULL), soilthick(NULL), SOL_ZMX(NULL),
                                 density(NULL), sol_sumAWC(NULL), sol_N(NULL), sol_cbn(NULL),
                                 rock(NULL), CLAY(NULL), sand(NULL), silt(NULL),
    /// Soil related parameters
                                 sol_aorgn(NULL), sol_fon(NULL), sol_fop(NULL),
                                 sol_nh4(NULL), sol_no3(NULL), sol_orgn(NULL),
                                 sol_orgp(NULL), sol_solp(NULL),
    /// Plant operation related parameters
                                 LanduseLookup(NULL), landuseNum(-1), CN2(NULL), IGRO(NULL),
                                 IDC(NULL),
                                 hi_targ(NULL), biotarg(NULL), CURYR_INIT(NULL),
                                 wsyf(NULL), LAIDAY(NULL), PHUBASE(NULL), frPHUacc(NULL), PHU_PLT(NULL),
                                 dormi(NULL), hvsti(NULL), hvsti_adj(NULL),
                                 laimaxfr(NULL), olai(NULL), frPlantN(NULL), plant_N(NULL), plant_P(NULL),
                                 plt_et_tot(NULL), plt_pet_tot(NULL), frRoot(NULL), BIOMASS(NULL),
    /// Harvest and Kill, harvest, harvgrain operation
                                 sol_rsd(NULL), frStrsWtr(NULL), CropLookup(NULL), cropNum(-1),
                                 lastSoilRD(NULL),
                                 grainc_d(NULL), m_stoverc_d(NULL), m_rsdc_d(NULL),
    /// Fertilizer operation
                                 FertilizerLookup(NULL), fertilizerNum(-1), cswat(0),
                                 sol_mc(NULL), sol_mn(NULL), sol_mp(NULL),
    /// Irrigation
                                 irr_flag(NULL), irr_water(NULL), irr_surfq(NULL), m_deepWaterDepth(NULL),
                                 m_shallowWaterDepth(NULL),
                                 pot_sa(NULL), deepIrrWater(NULL), shallowIrrWater(NULL),
    /// auto irrigation operation
                                 awtr_strsID(NULL), awtr_strsTrig(NULL), airr_source(NULL), airr_location(NULL),
                                 airr_eff(NULL),
                                 airrwtr_depth(NULL), airrsurf_ratio(NULL),
    /// bacteria related
    //m_bactSwf(NODATA),	m_bactLessPersistPlt(NULL), m_bactLessPersistSol(NULL), m_bactLessPersistParticle(NULL),
    //m_bactPersistPlt(NULL), m_bactPersistSol(NULL), m_bactPersistParticle(NULL),
    /// Tillage operation
                                 TillageLookup(NULL), sol_actp(NULL), sol_stap(NULL),
    /// tillage factor on SOM decomposition, used by CENTURY model
                                 tillage_switch(NULL), tillage_depth(NULL), tillage_days(NULL),
                                 tillage_factor(NULL),
    /// auto fertilizer operation
                                 afert_id(NULL), afert_nstrsID(NULL), afert_nstrs(NULL),
                                 afert_maxN(NULL), afert_AmaxN(NULL),
                                 afert_nyldTarg(NULL), afert_frteff(NULL), afert_frtsurf(NULL),
    /// Grazing operation
                                 grz_days(NULL), grz_flag(NULL),
    /// Release or impound operation
                                 impound_trig(NULL), pot_vol(NULL), pot_volmaxmm(NULL), pot_vollowmm(NULL),
                                 sol_awc(NULL), sol_ul(NULL), solst(NULL), solsw(NULL),
                                 pot_no3(NULL), pot_nh4(NULL), pot_solp(NULL),
    /// CENTURY C/N cycling related variables
                                 sol_HSN(NULL), sol_LM(NULL), sol_LMC(NULL), sol_LMN(NULL), sol_LSC(NULL),
                                 sol_LSN(NULL), sol_LS(NULL), sol_LSL(NULL), sol_LSLC(NULL), sol_LSLNC(NULL),
                                 sol_BMN(NULL), sol_HPN(NULL),
    /// Temporary parameters
                                 doneOpSequence(NULL),
                                 initialized(false) {
}

MGTOpt_SWAT::~MGTOpt_SWAT(void) {
    /// release map containers
    if (!m_mgtFactory.empty()) {
        for (map<int, BMPPlantMgtFactory *>::iterator it = m_mgtFactory.begin(); it != m_mgtFactory.end();) {
            if (it->second != NULL) {
                delete it->second;
                it->second = NULL;
            }
            m_mgtFactory.erase(it++);
        }
        m_mgtFactory.clear();
    }
    if (!m_landuseLookupMap.empty()) {
        for (map<int, float *>::iterator it = m_landuseLookupMap.begin(); it != m_landuseLookupMap.end();) {
            if (it->second != NULL) {
                delete[] it->second;
                it->second = NULL;
            }
            it->second = NULL;
            m_landuseLookupMap.erase(it++);
        }
        m_landuseLookupMap.clear();
    }
    if (!m_cropLookupMap.empty()) {
        for (map<int, float *>::iterator it = m_cropLookupMap.begin(); it != m_cropLookupMap.end();) {
            if (it->second != NULL) {
                delete[] it->second;
                it->second = NULL;
            }
            it->second = NULL;
            m_cropLookupMap.erase(it++);
        }
        m_cropLookupMap.clear();
    }
    if (!m_fertilizerLookupMap.empty()) {
        for (map<int, float *>::iterator it = m_fertilizerLookupMap.begin(); it != m_fertilizerLookupMap.end();) {
            if (it->second != NULL) {
                delete[] it->second;
                it->second = NULL;
            }
            it->second = NULL;
            m_fertilizerLookupMap.erase(it++);
        }
        m_fertilizerLookupMap.clear();
    }
    if (!m_tillageLookupMap.empty()) {
        for (map<int, float *>::iterator it = m_tillageLookupMap.begin(); it != m_tillageLookupMap.end();) {
            if (it->second != NULL) {
                delete[] it->second;
                it->second = NULL;
            }
            it->second = NULL;
            m_tillageLookupMap.erase(it++);
        }
        m_tillageLookupMap.clear();
    }
    /// release output parameters
    /// plant operation
    if (hi_targ != NULL) Release1DArray(hi_targ);
    if (biotarg != NULL) Release1DArray(biotarg);
    /// auto irrigation operation
    if (irr_flag != NULL) Release1DArray(irr_flag);
    if (irr_water != NULL) Release1DArray(irr_water);
    if (irr_surfq != NULL) Release1DArray(irr_surfq);
    if (awtr_strsID != NULL) Release1DArray(awtr_strsID);
    if (awtr_strsTrig != NULL) Release1DArray(awtr_strsTrig);
    if (airr_source != NULL) Release1DArray(airr_source);
    if (airr_location != NULL) Release1DArray(airr_location);
    if (airr_eff != NULL) Release1DArray(airr_eff);
    if (airrwtr_depth != NULL) Release1DArray(airrwtr_depth);
    if (airrsurf_ratio != NULL) Release1DArray(airrsurf_ratio);
    /// fertilizer / auto fertilizer operation
    if (afert_id != NULL) Release1DArray(afert_id);
    if (afert_nstrsID != NULL) Release1DArray(afert_nstrsID);
    if (afert_nstrs != NULL) Release1DArray(afert_nstrs);
    if (afert_maxN != NULL) Release1DArray(afert_maxN);
    if (afert_AmaxN != NULL) Release1DArray(afert_AmaxN);
    if (afert_nyldTarg != NULL) Release1DArray(afert_nyldTarg);
    if (afert_frteff != NULL) Release1DArray(afert_frteff);
    if (afert_frtsurf != NULL) Release1DArray(afert_frtsurf);
    /// Grazing operation
    if (grz_days != NULL) Release1DArray(grz_days);
    if (grz_flag != NULL) Release1DArray(grz_flag);
    /// Impound/Release operation
    if (impound_trig != NULL) Release1DArray(impound_trig);
    if (pot_volmaxmm != NULL) Release1DArray(pot_volmaxmm);
    if (pot_vollowmm != NULL) Release1DArray(pot_vollowmm);
}

void MGTOpt_SWAT::SetValue(const char *key, float data) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) data); }
    else if (StringMatch(sk, VAR_CSWAT)) { cswat = (int) data; }
    else if (StringMatch(sk, Tag_CellWidth)) { CELLWIDTH = data; }
    else {
        throw ModelException(MID_PLTMGT_SWAT, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

bool MGTOpt_SWAT::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputSize", "Input data for " + string(key) +
            " is invalid. The size could not be less than zero.");
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_PLTMGT_SWAT, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input raster data should have same size.");
            return false;
        }
    }
    return true;
}

void MGTOpt_SWAT::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    if (StringMatch(sk, VAR_SBGS))// TODO
    {
        m_deepWaterDepth = data;
        m_shallowWaterDepth = data;
        return;
    }
    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_SUBBSN)) { subbasin = data; }
    else if (StringMatch(sk, VAR_MGT_FIELD)) { mgt_fields = data; }
    else if (StringMatch(sk, VAR_LANDUSE)) { landuse = data; }
    else if (StringMatch(sk, VAR_LANDCOVER)) { landcover = data; }
    else if (StringMatch(sk, VAR_IDC)) {
        IDC = data;
        /// Soil related parameters from MongoDB
    } else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, VAR_SOL_ZMX)) { SOL_ZMX = data; }
    else if (StringMatch(sk, VAR_SOL_SUMAWC)) { sol_sumAWC = data; }
    else if (StringMatch(sk, VAR_T_BASE)) {
        T_BASE = data;
        ///  Plant operation related parameters
    } else if (StringMatch(sk, VAR_CN2)) { CN2 = data; }
    else if (StringMatch(sk, VAR_HVSTI)) { hvsti = data; }
    else if (StringMatch(sk, VAR_WSYF)) { wsyf = data; }
    else if (StringMatch(sk, VAR_PHUPLT)) { PHU_PLT = data; }
    else if (StringMatch(sk, VAR_PHUBASE)) { PHUBASE = data; }
    else if (StringMatch(sk, VAR_IGRO)) { IGRO = data; }
    else if (StringMatch(sk, VAR_FR_PHU_ACC)) { frPHUacc = data; }
    else if (StringMatch(sk, VAR_TREEYRS)) { CURYR_INIT = data; }
    else if (StringMatch(sk, VAR_HVSTI_ADJ)) { hvsti_adj = data; }
    else if (StringMatch(sk, VAR_LAIDAY)) { LAIDAY = data; }
    else if (StringMatch(sk, VAR_DORMI)) { dormi = data; }
    else if (StringMatch(sk, VAR_LAIMAXFR)) { laimaxfr = data; }
    else if (StringMatch(sk, VAR_OLAI)) { olai = data; }
    else if (StringMatch(sk, VAR_PLANT_N)) { plant_N = data; }
    else if (StringMatch(sk, VAR_PLANT_P)) { plant_P = data; }
    else if (StringMatch(sk, VAR_FR_PLANT_N)) { frPlantN = data; }
    else if (StringMatch(sk, VAR_FR_PLANT_P)) { frPlantP = data; }
    else if (StringMatch(sk, VAR_PLTET_TOT)) { plt_et_tot = data; }
    else if (StringMatch(sk, VAR_PLTPET_TOT)) { plt_pet_tot = data; }
    else if (StringMatch(sk, VAR_FR_ROOT)) { frRoot = data; }
    else if (StringMatch(sk, VAR_BIOMASS)) {
        BIOMASS = data;
        //// Harvest and Kill operation
    } else if (StringMatch(sk, VAR_LAST_SOILRD)) {
        lastSoilRD = data;
        /// Irrigation operation
    } else if (StringMatch(sk, VAR_FR_STRSWTR)) {
        frStrsWtr = data;
        //else if (StringMatch(sk, VAR_DEEPST)) m_deepWaterDepth = data;
        //else if (StringMatch(sk, VAR_SHALLST)) m_shallowWaterDepth = data;
        /// impound/release
    } else if (StringMatch(sk, VAR_POT_VOL)) { pot_vol = data; }
    else if (StringMatch(sk, VAR_POT_SA)) { pot_sa = data; }
    else if (StringMatch(sk, VAR_POT_NO3)) { pot_no3 = data; }
    else if (StringMatch(sk, VAR_POT_NH4)) { pot_nh4 = data; }
    else if (StringMatch(sk, VAR_POT_SOLP)) { pot_solp = data; }
    else if (StringMatch(sk, VAR_SOL_SW)) { solsw = data; }
    else {
        throw ModelException(MID_PLTMGT_SWAT, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

bool MGTOpt_SWAT::CheckInputSize2D(const char *key, int n, int col) {
    CheckInputSize(key, n);
    if (col <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
            " is invalid. The layer number could not be less than zero.");
    }
    if (nSoilLayers != col) {
        if (nSoilLayers <= 0) {
            nSoilLayers = col;
        } else {
            throw ModelException(MID_PLTMGT_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
                " is invalid. All the layers of input 2D raster data should have same size of " +
                ValueToString(nSoilLayers) + " instead of " +
                ValueToString(col) + ".");
            return false;
        }
    }
    return true;
}

void MGTOpt_SWAT::Set2DData(const char *key, int n, int col, float **data) {
    string sk(key);
    /// lookup tables
    if (StringMatch(sk, VAR_LANDUSE_LOOKUP)) {
        LanduseLookup = data;
        landuseNum = n;
        initializeLanduseLookup();
        if (col != (int) LANDUSE_PARAM_COUNT) {
            throw ModelException(MID_PLTMGT_SWAT, "ReadLanduseLookup", "The field number " + ValueToString(col) +
                "is not coincident with LANDUSE_PARAM_COUNT: " +
                ValueToString((int) LANDUSE_PARAM_COUNT));
        }
        return;
    } else if (StringMatch(sk, VAR_CROP_LOOKUP)) {
        CropLookup = data;
        cropNum = n;
        initializeCropLookup();
        if (col != (int) CROP_PARAM_COUNT) {
            throw ModelException(MID_PLTMGT_SWAT, "ReadCropLookup", "The field number " + ValueToString(col) +
                "is not coincident with CROP_PARAM_COUNT: " +
                ValueToString((int) CROP_PARAM_COUNT));
        }
        return;
    } else if (StringMatch(sk, VAR_FERTILIZER_LOOKUP)) {
        FertilizerLookup = data;
        fertilizerNum = n;
        initializeFertilizerLookup();
        if (col != (int) FERTILIZER_PARAM_COUNT) {
            throw ModelException(MID_PLTMGT_SWAT, "ReadFertilizerLookup", "The field number " + ValueToString(col) +
                "is not coincident with FERTILIZER_PARAM_COUNT: " +
                ValueToString((int) FERTILIZER_PARAM_COUNT));
        }
        return;
    } else if (StringMatch(sk, VAR_TILLAGE_LOOKUP)) {
        TillageLookup = data;
        tillageNum = n;
        initializeTillageLookup();
        if (col != (int) TILLAGE_PARAM_COUNT) {
            throw ModelException(MID_PLTMGT_SWAT, "ReadTillageLookup", "The field number " + ValueToString(col) +
                "is not coincident with TILLAGE_PARAM_COUNT: " +
                ValueToString((int) TILLAGE_PARAM_COUNT));
        }
        return;
    }
    /// 2D raster data
    CheckInputSize2D(key, n, col);
    /// Soil related parameters from MongoDB
    if (StringMatch(sk, VAR_SOILDEPTH)) { soilDepth = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { density = data; }
    else if (StringMatch(sk, VAR_SOL_CBN)) { sol_cbn = data; }
    else if (StringMatch(sk, VAR_SOL_N)) { sol_N = data; }
    else if (StringMatch(sk, VAR_CLAY)) { CLAY = data; }
    else if (StringMatch(sk, VAR_SILT)) { silt = data; }
    else if (StringMatch(sk, VAR_SAND)) { sand = data; }
    else if (StringMatch(sk, VAR_ROCK)) {
        rock = data;
        /// Soil related parameters --  inputs from other modules
    } else if (StringMatch(sk, VAR_SOL_SORGN)) { sol_orgn = data; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { sol_orgp = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { sol_solp = data; }
    else if (StringMatch(sk, VAR_SOL_NH4)) { sol_nh4 = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_AORGN)) { sol_aorgn = data; }
    else if (StringMatch(sk, VAR_SOL_FORGN)) { sol_fon = data; }
    else if (StringMatch(sk, VAR_SOL_FORGP)) { sol_fop = data; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { sol_actp = data; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { sol_stap = data; }
    else if (StringMatch(sk, VAR_SOL_RSD)) { sol_rsd = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { sol_awc = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) {
        solst = data;
        /// inputs for CENTURY C/N cycling model in stated and necessary
    } else if (StringMatch(sk, VAR_SOL_HSN)) { sol_HSN = data; }
    else if (StringMatch(sk, VAR_SOL_LM)) { sol_LM = data; }
    else if (StringMatch(sk, VAR_SOL_LMC)) { sol_LMC = data; }
    else if (StringMatch(sk, VAR_SOL_LMN)) { sol_LMN = data; }
    else if (StringMatch(sk, VAR_SOL_LSC)) { sol_LSC = data; }
    else if (StringMatch(sk, VAR_SOL_LSN)) { sol_LSN = data; }
    else if (StringMatch(sk, VAR_SOL_LS)) { sol_LS = data; }
    else if (StringMatch(sk, VAR_SOL_LSL)) { sol_LSL = data; }
    else if (StringMatch(sk, VAR_SOL_LSLC)) { sol_LSLC = data; }
    else if (StringMatch(sk, VAR_SOL_LSLNC)) {
        sol_LSLNC = data;
        //else if (StringMatch(sk, VAR_SOL_WON)) m_sol_WON = data;
        //else if (StringMatch(sk, VAR_SOL_BM)) m_sol_BM = data;
        //else if (StringMatch(sk, VAR_SOL_BMC)) m_sol_BMC = data;
    } else if (StringMatch(sk, VAR_SOL_BMN)) {
        sol_BMN = data;
        //else if (StringMatch(sk, VAR_SOL_HP)) m_sol_HP = data;
        //else if (StringMatch(sk, VAR_SOL_HS)) m_sol_HS = data;
        //else if (StringMatch(sk, VAR_SOL_HSC)) m_sol_HSC = data;
        //else if (StringMatch(sk, VAR_SOL_HPC)) m_sol_HPC = data;
    } else if (StringMatch(sk, VAR_SOL_HPN)) {
        sol_HPN = data;
        //else if (StringMatch(sk, VAR_SOL_RNMN)) m_sol_RNMN = data;
        //else if (StringMatch(sk, VAR_SOL_RSPC)) m_sol_RSPC = data;
    } else {
        throw ModelException(MID_PLTMGT_SWAT, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void MGTOpt_SWAT::SetScenario(Scenario *sce) {
    if (NULL == sce) {
        throw ModelException(MID_PLTMGT_SWAT, "SetScenario", "The Scenario data can not to be NULL.");
    }
    map<int, BMPFactory *> tmpBMPFactories = sce->GetBMPFactories();
    if (!m_mgtFactory.empty()) {
        m_mgtFactory.clear();
    }
    for (map<int, BMPFactory *>::iterator it = tmpBMPFactories.begin(); it != tmpBMPFactories.end(); it++) {
        /// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
        if (it->first / 100000 == BMP_TYPE_PLANT_MGT) {
            /// calculate unique index for the key of m_mgtFactory, using Landuse_ID * 100 + subScenario
            int uniqueIdx = ((BMPPlantMgtFactory *) it->second)->GetLUCCID() * 100 + it->second->GetSubScenarioId();
            m_mgtFactory[uniqueIdx] = (BMPPlantMgtFactory *) it->second;
        }
    }
}

void MGTOpt_SWAT::SetSubbasins(clsSubbasins *subbasins) {
    if (NULL == subbasins) {
        throw ModelException(MID_PLTMGT_SWAT, "SetSubbasins", "The Subbasins data can not to be NULL.");
    } else {
        nSub = subbasins->GetSubbasinNumber();
        if (!m_nCellsSubbsn.empty() || !m_nAreaSubbsn.empty()) return;
        vector<int> subIDs = subbasins->GetSubbasinIDs();
        for (vector<int>::iterator it = subIDs.begin(); it != subIDs.end(); it++) {
            Subbasin *tmpSubbsn = subbasins->GetSubbasinByID(*it);
            m_nCellsSubbsn[*it] = tmpSubbsn->getCellCount();
            m_nAreaSubbsn[*it] = tmpSubbsn->getArea();
        }
    }
}

bool MGTOpt_SWAT::CheckInputData(void) {
    // DT_Single
    if (m_date <= 0) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (CELLWIDTH <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "The cell width of the input data can not be less than zero.");
    }
    if (nSoilLayers <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (cswat < 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Carbon modeling method must be 0, 1, or 2");
    } else if (cswat == 2) {
        /// Check for the CENTURY required initialized variables
        if (sol_HSN == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_HSN must not be NULL.");
        }
        if (sol_LM == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LM must not be NULL.");
        }
        if (sol_LMC == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LMC must not be NULL.");
        }
        if (sol_LMN == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LMN must not be NULL.");
        }
        if (sol_LSC == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LSC must not be NULL.");
        }
        if (sol_LSN == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LSN must not be NULL.");
        }
        if (sol_LS == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LS must not be NULL.");
        }
        if (sol_LSL == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LSL must not be NULL.");
        }
        if (sol_LSLC == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LSLC must not be NULL.");
        }
        if (sol_LSLNC == NULL) {
            ModelException(MID_PLTMGT_SWAT, "CheckInputData", "sol_LSLNC must not be NULL.");
        }
    }
    /// DT_Raster
    if (subbasin == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "SubBasin ID must not be NULL");
    if (landuse == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Landuse must not be NULL");
    if (landcover == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Landcover must not be NULL");
    if (mgt_fields == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Management fields must not be NULL");
    }
    if (T_BASE == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "Base or minimum temperature for plant growth must not be NULL");
    }
    if (soillayers == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil layers number must not be NULL");
    }
    if (SOL_ZMX == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Maximum soil root depth must not be NULL");
    }
    if (sol_sumAWC == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "Summary amount water in field capacity must not be NULL");
    }
    if (CN2 == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "CN2 value must not be NULL");
    if (IGRO == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Plant growth code must not be NULL");
    if (IDC == NULL) {
        throw ModelException(MID_BIO_EPIC, "CheckInputData", "The land cover/plant classification can not be NULL.");
    }
    if (CURYR_INIT == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Current growth year must not be NULL");
    }
    if (wsyf == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Value of harvest index must not be NULL");
    }
    if (LAIDAY == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "LAI in current day must not be NULL");
    }
    if (PHUBASE == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Base heat units fraction must not be NULL");
    }
    if (frPHUacc == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Accumulated heat units fraction must not be NULL");
    }
    if (PHU_PLT == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "Heat units needed by plant to maturity must not be NULL");
    }
    if (dormi == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Dormancy flag must not be NULL");
    if (hvsti == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Harvest index must not be NULL");
    if (hvsti_adj == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Adjusted harvest index must not be NULL");
    }
    if (laimaxfr == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "LAI maximum fraction must not be NULL");
    }
    if (olai == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "oLAI must not be NULL");
    if (frPlantN == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Fraction of biomass in nitrogen must not be NULL");
    }
    if (frPlantP == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Fraction of biomass in phosphorus must not be NULL");
    }
    if (plt_et_tot == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "Actual ET simulated during life of plant must not be NULL");
    }
    if (plt_pet_tot == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "Potential ET simulated during life of plant must not be NULL");
    }
    if (frRoot == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Fraction of total biomass in roots must not be NULL");
    }
    if (BIOMASS == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Biomass must not be NULL");
    if (lastSoilRD == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Last root depth in soil must not be NULL");
    }
    if (m_deepWaterDepth == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Depth of water in deep aquifer must not be NULL");
    }
    if (m_shallowWaterDepth == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Depth of water in shallow aquifer must not be NULL");
    }
    /// DT_Raster2D
    if (soilDepth == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil depth must not be NULL");
    if (soilthick == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil thickness must not be NULL");
    if (density == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil bulk density must not be NULL");
    if (sol_N == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil nitrogen must not be NULL");
    if (sol_cbn == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil carbon content must not be NULL");
    }
    if (rock == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil rock content must not be NULL");
    }
    if (CLAY == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil clay content must not be NULL");
    }
    if (sand == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil sand content must not be NULL");
    }
    if (silt == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil silt content must not be NULL");
    }
    if (sol_aorgn == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil active organic N must not be NULL");
    }
    if (sol_fon == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil fresh organic N must not be NULL");
    }
    if (sol_fop == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil fresh organic P must not be NULL");
    }
    if (sol_nh4 == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil NH4 must not be NULL");
    if (sol_no3 == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil NO3 must not be NULL");
    if (sol_orgn == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil stable organic N must not be NULL");
    }
    if (sol_orgp == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil organic P must not be NULL");
    if (sol_solp == NULL) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil soluble P must not be NULL");
    if (sol_actp == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil active mineral P must not be NULL");
    }
    if (sol_stap == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Soil stable mineral P must not be NULL");
    }
    if (sol_rsd == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData",
                             "OM classified as residue in soil layers must not be NULL");
    }
    return true;
}

bool MGTOpt_SWAT::GetOperationCode(int i, int &factoryID, vector<int> &nOps) {
    if (m_mgtFactory.empty()) return false;
    int curLanduseID = int(landuse[i]);
    int curMgtField = int(mgt_fields[i]);
    factoryID = -1;
    /// 1. Is there any plant management operations are suitable to current cell.
    for (map<int, BMPPlantMgtFactory *>::iterator it = m_mgtFactory.begin(); it != m_mgtFactory.end(); it++) {
        /// Key is unique plant management Index, which is calculated by Landuse_ID * 100 + subScenario;
        if (curLanduseID == (it->first) / 100) {
            /// 2. If current cell located in the locations of this BMPPlantMgtFactory
            ///    locations is empty means this operation may be applied to all cells
            if (it->second->GetLocations().empty() || ValueInVector(curMgtField, it->second->GetLocations())) {
                factoryID = it->first;
                break;
            }
        }
    }
    if (factoryID < 0) return false;
    /// 3. Figure out if any management operation should be applied, i.e., find sequence IDs (nOps)
    vector<int> tmpOpSeqences = m_mgtFactory[factoryID]->GetOperationSequence();
    map <int, PlantManagementOperation *> tmpOperations = m_mgtFactory[factoryID]->GetOperations();
    // get the next should be done sequence number
    int curSeq = doneOpSequence[i];
    int nextSeq = -1;
    if (curSeq == -1 || (unsigned) curSeq == tmpOpSeqences.size() - 1) {
        nextSeq = 0;
    } else {
        nextSeq = curSeq + 1;
    }
    int opCode = tmpOpSeqences[nextSeq];
    // figure out the nextSeq is satisfied or not.
    if (tmpOperations.find(opCode) != tmpOperations.end()) {
        PlantManagementOperation *tmpOperation = tmpOperations.at(opCode);
        /// *seqIter is calculated by: seqNo. * 1000 + operationCode
        bool dateDepent = false, huscDepent = false;
        /// If operation applied date (month and day) are defined
        if (tmpOperation->GetMonth() != 0 && tmpOperation->GetDay() != 0) {
            struct tm dateInfo;
            LocalTime(m_date, &dateInfo);
            if (dateInfo.tm_mon == tmpOperation->GetMonth() &&
                dateInfo.tm_mday == tmpOperation->GetDay()) {
                    dateDepent = true;
            }
        }
        /// If husc is defined
        if (tmpOperation->GetHUFraction() >= 0.f) {
            float aphu; /// fraction of total heat units accumulated
            if (FloatEqual(dormi[i], 1.f)) {
                aphu = NODATA_VALUE;
            } else {
                if (tmpOperation->UseBaseHUSC() && FloatEqual(IGRO[i], 0.f)) // use base hu
                {
                    aphu = PHUBASE[i];
                    if (aphu >= tmpOperation->GetHUFraction()) {
                        huscDepent = true;
                    }
                } else { // use accumulated plant hu
                    aphu = frPHUacc[i];
                    if (aphu >= tmpOperation->GetHUFraction()) {
                        huscDepent = true;
                    }
                }
            }
        }
        /// The operation will be applied either date or HUSC are satisfied,
        /// and also in case of repeated run
        if (dateDepent || huscDepent) {
            nOps.push_back(opCode);
            doneOpSequence[i] = nextSeq; /// update value
        }
    }
    if (nOps.empty()) return false;
    return true;
}

void MGTOpt_SWAT::initializeLanduseLookup() {
    /// Check input data
    if (LanduseLookup == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Landuse lookup array must not be NULL");
    }
    if (landuseNum <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Landuse number must be greater than 0");
    }
    if (!m_landuseLookupMap.empty()) {
        return;
    } else {
        for (int i = 0; i < landuseNum; i++) {
            m_landuseLookupMap[(int) LanduseLookup[i][1]] = LanduseLookup[i];
        }
    }
}

void MGTOpt_SWAT::initializeCropLookup() {
    /// Check input data
    if (CropLookup == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Crop lookup array must not be NULL");
    }
    if (cropNum <= 0) throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Crop number must be greater than 0");

    if (!m_cropLookupMap.empty()) {
        return;
    } else {
        for (int i = 0; i < cropNum; i++) {
            m_cropLookupMap[(int) CropLookup[i][1]] = CropLookup[i];
        }
    }
}

void MGTOpt_SWAT::initializeFertilizerLookup() {
    /// Check input data
    if (FertilizerLookup == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Fertilizer lookup array must not be NULL");
    }
    if (fertilizerNum <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Fertilizer number must be greater than 0");
    }

    if (!m_fertilizerLookupMap.empty()) {
        return;
    } else {
        for (int i = 0; i < fertilizerNum; i++) {
            m_fertilizerLookupMap[(int) FertilizerLookup[i][1]] = FertilizerLookup[i];
        }
    }
}

void MGTOpt_SWAT::initializeTillageLookup() {
    /// Check input data
    if (TillageLookup == NULL) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Tillage lookup array must not be NULL");
    }
    if (tillageNum <= 0) {
        throw ModelException(MID_PLTMGT_SWAT, "CheckInputData", "Tillage number must be greater than 0");
    }
    if (!m_tillageLookupMap.empty()) {
        return;
    } else {
        for (int i = 0; i < tillageNum; i++) {
            m_tillageLookupMap[(int) TillageLookup[i][1]] = TillageLookup[i];
        }
    }
}

void MGTOpt_SWAT::ExecutePlantOperation(int i, int &factoryID, int nOp) {
    PlantOperation *curOperation = (PlantOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    IGRO[i] = 1.f;
    hi_targ[i] = curOperation->HITarg();
    biotarg[i] = curOperation->BIOTarg(); /// kg/ha
    CURYR_INIT[i] = curOperation->CurYearMaturity();
    int newPlantID = curOperation->PlantID();
    landcover[i] = newPlantID;
    PHU_PLT[i] = curOperation->HeatUnits();
    dormi[i] = 0.f;
    frPHUacc[i] = 0.f;
    plant_N[i] = 0.f;
    plant_P[i] = 0.f;
    plt_et_tot[i] = 0.f;
    plt_pet_tot[i] = 0.f;
    laimaxfr[i] = 0.f;
    hvsti_adj[i] = 0.f;
    olai[i] = 0.f;
    frRoot[i] = 0.f;
    /// update crop-related parameters in order to calculate phuAcc. by LJ
    if (m_cropLookupMap.find(newPlantID) == m_cropLookupMap.end()) {
        throw ModelException(MID_PLTMGT_SWAT, "ExecutePlantOperation",
                             "The new plant ID: " + ValueToString(newPlantID) +
                                 " is not prepared in cropLookup table!");
    }
    // update IDC
    IDC[i] = m_cropLookupMap.at(newPlantID)[CROP_PARAM_IDX_IDC];
    T_BASE[i] = m_cropLookupMap.at(newPlantID)[CROP_PARAM_IDX_T_BASE];
    /// initialize transplant variables
    if (curOperation->LAIInit() > 0.f) {
        LAIDAY[i] = curOperation->LAIInit();
        BIOMASS[i] = curOperation->BIOInit();
    }
    /// compare maximum rooting depth in soil to maximum rooting depth of plant
    SOL_ZMX[i] = soilDepth[i][(int) soillayers[i] - 1];
    /// if the land cover does existed, throw an exception.
    if (m_landuseLookupMap.find(int(landcover[i])) == m_landuseLookupMap.end()) {
        throw ModelException(MID_PLTMGT_SWAT, "ExecutePlantOperation",
                             "Land use ID: " + ValueToString(int(landcover[i])) +
                                 " does not existed in Landuse lookup table, please check and retry!");
    }
    float pltRootDepth = m_landuseLookupMap[(int) landcover[i]][LANDUSE_PARAM_ROOT_DEPTH_IDX];
    SOL_ZMX[i] = min(SOL_ZMX[i], pltRootDepth);
    //if (i == 5878)
    //	cout<<"new plant: "<<newPlantID<<", IDC: "<<IDC[5878]<<", tbase: "<<T_BASE[5878]<<", solZMX: "<<SOL_ZMX[5878]<<endl;
    /// reset curve number if necessary
    if (curOperation->CNOP() > 0.f)   /// curno.f
    {
        float cnn = curOperation->CNOP();
        CN2[i] = cnn;
    }
}

void MGTOpt_SWAT::ExecuteIrrigationOperation(int i, int &factoryID, int nOp) {
    IrrigationOperation *curOperation = (IrrigationOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    /// irrigation source
    int m_irrSource;
    /// irrigation source location code
    int m_irrNo;
    /// irrigation apply depth (mm)
    float m_irrApplyDepth;
    /// float* m_irrSalt; /// currently not used
    /// irrigation efficiency
    float m_irrEfficiency;
    m_irrSource = curOperation->IRRSource();
    m_irrNo = (curOperation->IRRNo() <= 0) ? (int) subbasin[i] : curOperation->IRRNo();
    m_irrApplyDepth = curOperation->IRRApplyDepth();
    m_irrEfficiency = curOperation->IRREfficiency();
    irr_flag[i] = 1;
    int tmpSubbsnID = (int) subbasin[i];
    if (m_irrSource > IRR_SRC_RES) /// irrigation from reach and reservoir are irr_rch.f and irr_res.f, respectively
    {
        /// call irrsub.f
        /// Performs the irrigation operation when the source is the shallow or deep aquifer or a source outside the watershed
        float vmma = 0.f; /// amount of water in source, mm
        float vmm = 0.f;  ///maximum amount of water to be applied, mm
        float cnv = 0.f;    /// conversion factor (mm/ha => m^3)
        float vmxi = 0.f;   /// amount of water specified in irrigation operation, mm
        float vol = 0.f;      /// volume of water to be applied in irrigation, m^3
        float vmms = 0.f; /// amount of water in shallow aquifer, m^3
        float vmmd = 0.f; /// amount of water in deep aquifer, m^3
        /// Whether m_irrNo is valid
        if (m_nCellsSubbsn.find(m_irrNo) == m_nCellsSubbsn.end()) {
            m_irrNo = int(subbasin[i]);
        }

        cnv = m_nAreaSubbsn[m_irrNo] * 10.f; /// area of current subbasin
        switch (m_irrSource) {
            /// in SEIMS, we hypothesis that shallow aquifer and deep aquifer is consistent within subbasin.
            case IRR_SRC_SHALLOW:
                if (m_shallowWaterDepth[tmpSubbsnID] < UTIL_ZERO) {
                    m_shallowWaterDepth[tmpSubbsnID] = 0.f;
                }
                vmma += m_shallowWaterDepth[tmpSubbsnID] * cnv * m_irrEfficiency;
                vmms = vmma;
                vmma /= m_nCellsSubbsn[m_irrNo];
                vmm = min(sol_sumAWC[i], vmma);
                break;
            case IRR_SRC_DEEP:vmma += m_deepWaterDepth[tmpSubbsnID] * cnv * m_irrEfficiency;
                vmmd = vmma;
                vmma /= m_nCellsSubbsn[m_irrNo];
                vmm = min(sol_sumAWC[i], vmma);
                break;
            case IRR_SRC_OUTWTSD: /// unlimited source
                vmm = sol_sumAWC[i];
                break;
        }
        /// if water available from source, proceed with irrigation
        if (vmm > 0.f) {
            cnv = cellArea * 10.f;
            vmxi = (m_irrApplyDepth < UTIL_ZERO) ? sol_sumAWC[i] : m_irrApplyDepth;
            if (vmm > vmxi) vmm = vmxi;
            vol = vmm * cnv;
            float pot_fr = 0.f;
            if (FloatEqual(impound_trig[i], 0.f) && pot_vol != NULL) {
                /// impound_trig equals to 0 means pot_fr is 1.
                /// and pot_sa is set to cellArea.
                pot_fr = 1.f;
                if (pot_sa != NULL) {
                    pot_vol[i] += vol / (10.f * pot_sa[i]);
                } else {
                    pot_vol[i] += vol / (10.f * cellArea);
                }
                irr_water[i] = vmm;  ///added rice irrigation 11/10/11
            } else {
                pot_fr = 0.f;
                /// Call irrigate(i, vmm) /// irrigate.f
                irr_water[i] = vmm * (1.f - curOperation->IRRSQfrac());
                irr_surfq[i] = vmm * curOperation->IRRSQfrac();
            }
            /// subtract irrigation from shallow or deep aquifer
            if (pot_fr > UTIL_ZERO) {
                vol = irr_water[i] * cnv * m_irrEfficiency;
            }
            switch (m_irrSource) {
                case IRR_SRC_SHALLOW:cnv = m_nAreaSubbsn[m_irrNo] * 10.f;
                    vmma = 0.f;
                    if (vmms > -0.01f) {
                        vmma = vol * m_shallowWaterDepth[tmpSubbsnID] * cnv / vmms;
                    }
                    vmma /= cnv;
                    m_shallowWaterDepth[tmpSubbsnID] -= vmma;
                    if (m_shallowWaterDepth[tmpSubbsnID] < 0.f) {
                        vmma += m_shallowWaterDepth[tmpSubbsnID];
                        m_shallowWaterDepth[tmpSubbsnID] = 0.f;
                    }
                    shallowIrrWater[i] += vmma;
                    break;
                case IRR_SRC_DEEP:cnv = m_nAreaSubbsn[m_irrNo] * 10.f;
                    vmma = 0.f;
                    if (vmmd > 0.01f) {
                        vmma = vol * (m_deepWaterDepth[tmpSubbsnID] * cnv / vmmd);
                    }
                    vmma /= cnv;
                    m_deepWaterDepth[tmpSubbsnID] -= vmma;
                    if (m_deepWaterDepth[tmpSubbsnID] < 0.f) {
                        vmma += m_deepWaterDepth[tmpSubbsnID];
                        m_deepWaterDepth[tmpSubbsnID] = 0.f;
                    }
                    deepIrrWater[i] += vmma;
                    break;
            }
        }
    }
}

void MGTOpt_SWAT::ExecuteFertilizerOperation(int i, int &factoryID, int nOp) {
    /* Briefly change log
	 * 1. Translate from fert.f, remains CSWAT = 1 and 2 to be done!!! by LJ
	 * 2. CSWAT = 1 and 2, were implemented on 2016-9-29, by LJ.
	 * 3. Consider paddy rice field according to Chowdary et al., 2004, 2016-10-9, by LJ.
	 */
    //initializeFertilizerLookup();
    FertilizerOperation *curOperation = (FertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// fertilizer type, ifrt
    int fertilizerID = curOperation->FertilizerID();
    /// kg/ha         |amount of fertilizer applied to HRU
    float fertilizerKgHa = curOperation->FertilizerKg_per_ha();
    /* fraction of fertilizer which is applied to the top 10 mm of soil 
	 * the remaining fraction: 
     *  - is dissolved in the impounded water, if current landcover is paddy rice
	 *  - is applied to first soil layer defined by user, otherwise
	 */
    float fertilizerSurfFrac = curOperation->FertilizerSurfaceFrac();
    /// if the fertilizerID is not existed in lookup table, then throw an exception
    if (m_fertilizerLookupMap.find(fertilizerID) == m_fertilizerLookupMap.end()) {
        throw ModelException(MID_PLTMGT_SWAT, "ExecuteFertilizerOperation", "Fertilizer ID " +
            ValueToString(fertilizerID) +
            " is not existed in Fertilizer Database!");
    }
    /**fertilizer paramters derived from lookup table**/
    //!!    fminn(:)      |kg minN/kg frt|fraction of fertilizer that is mineral N (NO3 + NH4)
    float fertMinN = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FMINN_IDX];
    //!!    fminp(:)      |kg minP/kg frt|fraction of fertilizer that is mineral P
    float fertMinP = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FMINP_IDX];
    //!!    forgn(:)      |kg orgN/kg frt|fraction of fertilizer that is organic N
    float fertOrgN = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FORGN_IDX];
    //!!    forgp(:)      |kg orgP/kg frt|fraction of fertilizer that is organic P
    float fertOrgP = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FORGP_IDX];
    //!!    fnh4n(:)      |kg NH4-N/kgminN|fraction of mineral N in fertilizer that is NH4-N
    float fertNH4N = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_FNH4N_IDX];
    //!!    bactpdb(:)    |# cfu/g   frt |concentration of persistent bacteria in fertilizer
    //float bactPDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BACTPDB_IDX];
    //!!    bactlpdb(:)   |# cfu/g   frt |concentration of less persistent bacteria in fertilizer
    //float bactLPDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BATTLPDB_IDX];
    //!!    bactkddb(:)   |none          |fraction of bacteria in solution (the remaining fraction is sorbed to soil particles)
    //float bactKDDB = m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_BACKTKDDB_IDX];
    // commercial fertilizer (0) or manure (1)
    int fertype = (int) m_fertilizerLookupMap[fertilizerID][FERTILIZER_PARAM_MANURE_IDX];
    /**summary output**/
    //!!    fertn         |kg N/ha       |total amount of nitrogen applied to soil in cell on day
    //float fertN = 0.f;
    //!!    fertp         |kg P/ha       |total amount of phosphorus applied to soil in cell on day
    //float fertP = 0.f;
    //float fertSolP = 0.f;
    /// cfertn       |kg N/ha       |total amount of nitrogen applied to soil during continuous fertilizer operation in cell on day
    //float cFertN = 0.f;
    /// cfertp       |kg P/ha       |total amount of phosphorus applied to soil during continuous fertilizer operation in cell on day
    //float cFertP = 0.f;
    /// weighting factor used to partition the organic N & P content of the fertilizer
    /// between the fresh organic and the active organic pools
    float rtof = 0.5f;
    float xx; /// fraction of fertilizer applied to layer
    float gc = 0.f; //, gc1 = 0.f;
    /// if current landcover is paddy rice, then apply the commercial fertilizer to the top surface and pothole.
    int lyrs = 2;
    if (pot_vol != NULL) {
        if (FloatEqual((int) landcover[i], CROP_PADDYRICE) && fertype == 0 && pot_vol[i] > 0.f) {
            lyrs = 1;
            xx = 1.f - fertilizerSurfFrac;
            pot_no3[i] += xx * fertilizerKgHa * (1.f - fertNH4N) * fertMinN * cellArea; /// kg/ha * ha ==> kg
            pot_nh4[i] += xx * fertilizerKgHa * fertNH4N * fertMinN * cellArea;
            pot_solp[i] += xx * fertilizerKgHa * fertMinP * cellArea;
            // if (i == 46364) cout<<"fert pot no3: "<<pot_no3[46364]<<", nh4: "<<pot_nh4[46364]<<endl;
        }
    }
    for (int l = 0; l < lyrs; l++) /// top surface and first layer
    {
        if (l == 0) xx = fertilizerSurfFrac;
        if (l == 1) xx = 1.f - fertilizerSurfFrac;
        sol_no3[i][l] += xx * fertilizerKgHa * (1.f - fertNH4N) * fertMinN;
        if (cswat == 0) /// Static model
        {
            sol_fon[i][l] += rtof * xx * fertilizerKgHa * fertOrgN;
            sol_aorgn[i][l] += (1.f - rtof) * xx * fertilizerKgHa * fertOrgN;
            sol_fop[i][l] += rtof * xx * fertilizerKgHa * fertOrgP;
            sol_orgp[i][l] += (1.f - rtof) * xx * fertilizerKgHa * fertOrgP;
        } else if (cswat == 1) /// C-FARM one carbon pool model
        {
            sol_mc[i][l] += xx * fertilizerKgHa * fertOrgN * 10.f; /// assume C:N = 10:1
            sol_mn[i][l] += xx * fertilizerKgHa * fertOrgN;
            sol_mp[i][l] += xx * fertilizerKgHa * fertOrgP;
        } else if (cswat == 2) /// CENTURY model for C/N cycling
        {
            float X1 = 0.f, X8 = 0.f, X10 = 0.f, XXX = 0.f, YY = 0.f;
            float ZZ = 0.f, XZ = 0.f, YZ = 0.f, RLN = 0.f;
            /// the fraction of organic carbon in fertilizer, for most fertilizers orgc_f is set to 0.
            float orgc_f = 0.f;
            sol_fop[i][l] += rtof * xx * fertilizerKgHa * fertOrgP;
            sol_orgp[i][l] += (1.f - rtof) * xx * fertilizerKgHa * fertOrgP;
            /// allocate organic fertilizer to slow (SWAT active) N pool, i.e., sol_aorgn
            sol_HSN[i][l] += (1.f - rtof) * xx * fertilizerKgHa * fertOrgN;
            sol_aorgn[i][l] = sol_HSN[i][l];
            /// X1 is fertilizer applied to layer (kg/ha)
            X1 = xx * fertilizerKgHa;
            // X8 is organic carbon applied (kg C/ha)
            X8 = X1 * orgc_f;
            /// RLN is calculated as a function of C:N ration in fertilizer
            RLN = 0.175f * (orgc_f) / (fertMinN + fertOrgN + 1.e-5f);
            /// X10 is the fraction of carbon in fertilizer that is allocated to metabolic litter C pool
            X10 = 0.85f - 0.018f * RLN;
            if (X10 < 0.01f) { X10 = 0.01f; }
            else if (X10 > 0.7f) X10 = 0.7f;

            /// XXX is the amount of organic carbon allocated to metabolic litter C pool
            XXX = X8 * X10;
            sol_LMC[i][l] += XXX;
            /// YY is the amount of fertilizer (including C and N) allocated into metabolic litter SOM pool
            YY = X1 * X10;
            sol_LM[i][l] += YY;
            /// ZZ is amount of organic N allocated to metabolic litter N pool
            ZZ = X1 * rtof * fertOrgN * X10;
            sol_LMN[i][l] += ZZ;

            /// remaining organic N is allocated to structural litter N pool
            sol_LSN[i][l] += X1 * fertOrgN - ZZ;
            /// XZ is the amount of organic carbon allocated to structural litter C pool
            XZ = X1 * orgc_f - XXX;
            sol_LSC[i][l] += XZ;
            /// assuming lignin C fraction of organic carbon to be 0.175;
            float lignin_C_frac = 0.175f;
            /// updating lignin amount in structural litter pool
            sol_LSLC[i][l] += XZ * lignin_C_frac;
            /// non-lignin part of the structural litter C is also updated;
            sol_LSLNC[i][l] += XZ * (1.f - lignin_C_frac);
            /// YZ is the amount of fertilizer (including C and N) allocated into structural litter SOM pool
            YZ = X1 - YY;
            sol_LS[i][l] += YZ;
            /// assuming lignin fraction of the organic fertilizer allocated into structure litter SOM pool to be 0.175
            float lingnin_SOM_frac = 0.175f;
            /// update lignin weight in structural litter.
            sol_LSL[i][l] += YZ * lingnin_SOM_frac;
            sol_fon[i][l] = sol_LMN[i][l] + sol_LSN[i][l];
        }
        sol_nh4[i][l] += xx * fertilizerKgHa * fertNH4N * fertMinN;
        sol_solp[i][l] += xx * fertilizerKgHa * fertMinP;
    }
    /// add bacteria - #cfu/g * t(manure)/ha * 1.e6g/t * ha/10,000m^2 = 100.
    /// calculate ground cover
    gc = (1.99532f - Erfc(1.333f * LAIDAY[i] - 2.f)) / 2.1f;
    if (gc < 0.f) gc = 0.f;
    //gc1 = 1.f - gc;
    /// bact_swf    |none          |fraction of manure containing active colony forming units (cfu)
    //frt_t = m_bactSwf * fertilizerKg / 1000.;
    //m_bactPersistPlt[i] += gc * bactPDB * frt_t * 100.;
    //m_bactLessPersistPlt[i] += gc * bactLPDB * frt_t * 100.;

    //m_bactPersistSol[i] += gc1 * bactPDB * frt_t * 100.;
    //m_bactPersistSol[i] *= bactKDDB;

    //m_bactPersistParticle[i] += gc1 * bactPDB * frt_t * 100.;
    //m_bactPersistParticle[i] *= (1. - bactKDDB);

    //m_bactLessPersistSol[i] += gc1 * bactLPDB *frt_t * 100.;
    //m_bactLessPersistSol[i] *= bactKDDB;

    //m_bactLessPersistParticle[i] += gc1 * bactLPDB * frt_t * 100.;
    //m_bactLessPersistParticle[i] *= (1. - bactKDDB);

    /// summary calculations, currently not used for output. TODO in the future.
    //float fertNO3 = fertilizerKgHa * fertMinN * (1.f - fertNH4N);
    //float fertNH4 = fertilizerKgHa * (fertMinN * fertNH4N);
    //fertOrgN = fertilizerKgHa * fertOrgN;
    //fertOrgP = fertilizerKgHa * fertOrgP;
    //fertSolP = fertilizerKgHa * fertSolP;
    //fertN += (fertilizerKgHa + cFertN) * (fertMinN + fertOrgN); /// should be array, but cureently not useful
    //fertP += (fertilizerKgHa + cFertP) * (fertMinP + fertOrgP);
}

void MGTOpt_SWAT::ExecutePesticideOperation(int i, int &factoryID, int nOp) {
    /// TODO
}

void MGTOpt_SWAT::ExecuteHarvestKillOperation(int i, int &factoryID, int nOp) {
    //// TODO: Yield is not set as outputs yet. by LJ
    /// harvkillop.f
    HarvestKillOperation *curOperation = (HarvestKillOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    float cnop = curOperation->CNOP();
    float wur = 0.f, hiad1 = 0.f;
    if (m_cropLookupMap.find(int(landcover[i])) == m_cropLookupMap.end()) {
        throw ModelException(MID_PLTMGT_SWAT, "ExecuteHarvestKillOperation",
                             "The landcover ID " + ValueToString(landcover[i])
                                 + " is not existed in crop lookup table!");
    }
    /// Get some parameters of current crop / landcover
    float hvsti = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_HVSTI];
    float wsyf = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_WSYF];
    int idc = (int) m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_IDC];
    float bio_leaf = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_BIO_LEAF];
    float cnyld = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_CNYLD];
    float cpyld = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_CPYLD];

    /// calculate modifier for autofertilization target nitrogen content
    // TODO
    //tnyld(j) = 0.
    //tnyld(j) = (1. - rwt(j)) * bio_ms(j) * pltfr_n(j) * auto_eff(j)

    if (hi_targ[i] > 0.f) {
        hiad1 = hi_targ[i];
    } else {
        if (plt_pet_tot[i] < 10.f) {
            wur = 100.f;
        } else {
            wur = 100.f * plt_et_tot[i] / plt_pet_tot[i];
        }
        hiad1 = (hvsti_adj[i] - wsyf) * (wur / (wur + exp(6.13f - 0.0883f * wur))) + wsyf;
        if (hiad1 > hvsti) hiad1 = hvsti;
    }
    /// check if yield is from above or below ground
    float yield = 0.f, resnew = 0.f, rtresnew = 0.f;

    /// stover fraction during harvest and kill operation
    float hi_ovr = curOperation->HarvestIndexOverride();
    float xx = curOperation->StoverFracRemoved();
    if (xx < UTIL_ZERO) {
        xx = hi_ovr;
    }
    if (hi_ovr > UTIL_ZERO) {
        yield = BIOMASS[i] * hi_ovr;
        resnew = BIOMASS[i] - yield;
    } else {
        if (idc == CROP_IDC_TREES) {
            yield = BIOMASS[i] * (1.f - bio_leaf);
            resnew = BIOMASS[i] - yield;
        } else {
            if (hvsti > 1.001f) {
                yield = BIOMASS[i] * (1.f - 1.f / (1.f + hiad1));
                resnew = BIOMASS[i] / (1.f + hiad1);
                resnew *= (1.f - xx);
            } else {
                yield = (1.f - frRoot[i]) * BIOMASS[i] * hiad1;
                resnew = (1.f - frRoot[i]) * (1.f - hiad1) * BIOMASS[i];
                /// remove stover during harvestKill operation
                resnew *= (1.f - xx);
                rtresnew = frRoot[i] * BIOMASS[i];
            }
        }
    }
    if (yield < 0.f) yield = 0.f;
    if (resnew < 0.f) resnew = 0.f;
    if (rtresnew < 0.f) rtresnew = 0.f;

    if (cswat == 2) {
        grainc_d[i] += yield * 0.42f;
        m_stoverc_d[i] += (BIOMASS[i] - yield - rtresnew) * 0.42f * xx;
        m_rsdc_d[i] += resnew * 0.42f;
        m_rsdc_d[i] += rtresnew * 0.42f;
    }
    /// calculate nutrient removed with yield
    float yieldn = 0.f, yieldp = 0.f;
    yieldn = yield * cnyld;
    yieldp = yield * cpyld;
    yieldn = min(yieldn, 0.80f * plant_N[i]);
    yieldp = min(yieldp, 0.80f * plant_P[i]);

    /// call rootfr.f to distributes dead root mass through the soil profile
    /// i.e., derive fraction of roots in each layer
    float *rtfr = new float[(int) soillayers[i]];
    rootFraction(i, rtfr);

    /// fraction of N, P in residue (ff1) or roots (ff2)
    float ff1 = (1.f - hiad1) / (1.f - hiad1 + frRoot[i]);
    float ff2 = 1.f - ff1;
    /// update residue, N, P on soil surface
    sol_rsd[i][0] += resnew;
    sol_fon[i][0] += ff1 * (plant_N[i] - yieldn);
    sol_fop[i][0] += ff1 * (plant_P[i] - yieldp);
    sol_rsd[i][0] = max(sol_rsd[i][0], 0.f);
    sol_fon[i][0] = max(sol_fon[i][0], 0.f);
    sol_fop[i][0] = max(sol_fop[i][0], 0.f);

    /// define variables of CENTURY model
    float BLG1 = 0.f, BLG2 = 0.f, BLG3 = 0.f, CLG = 0.f;
    float sf = 0.f, sol_min_n = 0.f, resnew_n = 0.f, resnew_ne = 0.f;
    float LMF = 0.f, LSF = 0.f;
    float RLN = 0.f, RLR = 0.f;
    /// insert new biomass of CENTURY model
    if (cswat == 2) {
        BLG1 = 0.01f / 0.10f;
        BLG2 = 0.99f;
        BLG3 = 0.10f;
        float XX = log(0.5f / BLG1 - 0.5f);
        BLG2 = (XX - log(1.f / BLG2 - 1.f)) / (1.f - 0.5f);
        BLG1 = XX + 0.5f * BLG2;
        CLG = BLG3 * frPHUacc[i] / (frPHUacc[i] + exp(BLG1 - BLG2 * frPHUacc[i]));
        sf = 0.05f;
        sol_min_n = sol_no3[i][0] + sol_nh4[i][0];
        resnew = resnew;
        resnew_n = ff1 * (plant_N[i] - yieldn);
        resnew_ne = resnew_n + sf * sol_min_n;

        RLN = resnew * CLG / (resnew_n + 1.e-5f);
        RLR = min(0.8f, resnew * CLG / (resnew + 1.e-5f));
        LMF = 0.85f - 0.018f * RLN;
        if (LMF < 0.01f) { LMF = 0.01f; }
        else if (LMF > 0.7f) LMF = 0.7f;
        LSF = 1.f - LMF;
        sol_LM[i][0] += LMF * resnew;
        sol_LS[i][0] += LSF * resnew;

        sol_LSL[i][0] += RLR * resnew;
        sol_LSC[i][0] += 0.42f * LSF * resnew;

        sol_LSLC[i][0] += RLR * 0.42f * resnew;
        sol_LSLNC[i][0] = sol_LSC[i][0] - sol_LSLC[i][0];

        if (resnew_n > (0.42f * LSF * resnew / 150.f)) {
            sol_LSN[i][0] += 0.42f * LSF * resnew / 150.f;
            sol_LMN[i][0] += resnew_n - (0.42f * LSF * resnew / 150.f) + 1.e-25f;
        } else {
            sol_LSN[i][0] += resnew_n;
            sol_LMN[i][0] += 1.e-25f;
        }
        sol_LMC[i][0] += 0.42f * LMF * resnew;
        /// update no3 and nh4 in soil
        sol_no3[i][0] *= (1.f - sf);
        sol_nh4[i][0] *= (1.f - sf);
    }
    /// end insert new biomass of CENTURY model

    /// allocate dead roots, N, P to soil layers
    for (int l = 0; l < soillayers[i]; l++) {
        sol_rsd[i][l] += rtfr[l] * rtresnew;
        sol_fon[i][l] += rtfr[l] * ff2 * (plant_N[i] - yieldn);
        sol_fop[i][l] += rtfr[l] * ff2 * (plant_P[i] - yieldp);

        /// insert new biomass of CENTURY model
        if (cswat == 2) {
            if (l == 1) { sf = 0.05f; }
            else { sf = 0.1f; }

            sol_min_n = sol_no3[i][l] + sol_nh4[i][l]; // kg/ha
            resnew = rtfr[l] * rtresnew;
            resnew_n = rtfr[l] * ff2 * (plant_N[i] - yieldn);
            resnew_ne = resnew_n + sf * sol_min_n;

            RLN = resnew * CLG / (resnew_n + 1.e-5f);
            RLR = min(0.8f, resnew * CLG / 1000.f / (resnew / 1000.f + 1.e-5f));
            LMF = 0.85f - 0.018f * RLN;
            if (LMF < 0.01f) { LMF = 0.01f; }
            else if (LMF > 0.7f) LMF = 0.7f;

            LSF = 1.f - LMF;
            sol_LM[i][l] += LMF * resnew;
            sol_LS[i][l] += LSF * resnew;

            /// here a simplified assumption of 0.5 LSL
            //LSLF = 0.f;
            //LSLF = CLG;

            sol_LSL[i][l] += RLR * LSF * resnew;
            sol_LSC[i][l] += 0.42f * LSF * resnew;

            sol_LSLC[i][l] += RLR * 0.42f * LSF * resnew;
            sol_LSLNC[i][l] = sol_LSC[i][l] - sol_LSLC[i][l];

            if (resnew_ne > (0.42f * LSF * resnew / 150.f)) {
                sol_LSN[i][l] += 0.42f * LSF * resnew / 150.f;
                sol_LMN[i][l] += resnew_ne - (0.42f * LSF * resnew / 150.f) + 1.e-25f;
            } else {
                sol_LSN[i][l] += resnew_ne;
                sol_LMN[i][l] += 1.e-25f;
            }
            sol_LMC[i][l] += 0.42f * LMF * resnew;
            /// update no3 and nh4 in soil
            sol_no3[i][l] *= (1.f - sf);
            sol_nh4[i][l] *= (1.f - sf);
        }
        /// end insert new biomass of CENTURY model
    }
    if (cnop > 0.f) {
        CN2[i] = cnop;
    } /// TODO: Is necessary to isolate the curno.f function for SUR_CN?
    /// reset variables
    IGRO[i] = 0.f;
    dormi[i] = 0.f;
    BIOMASS[i] = 0.f;
    frRoot[i] = 0.f;
    plant_N[i] = 0.f;
    plant_P[i] = 0.f;
    frStrsWtr[i] = 1.f;
    LAIDAY[i] = 0.f;
    hvsti_adj[i] = 0.f;
    Release1DArray(rtfr);
    frPHUacc[i] = 0.f;
    PHU_PLT[i] = 0.f;
}

void MGTOpt_SWAT::rootFraction(int i, float *&root_fr) {
    float cum_rd = 0.f, cum_d = 0.f, cum_rf = 0.f, x1 = 0.f, x2 = 0.f;
    if (lastSoilRD[i] < UTIL_ZERO) {
        root_fr[0] = 1.f;
        return;
    }
    /// Normalized Root Density = 1.15*exp[-11.7*NRD] + 0.022, where NRD = normalized rooting depth
    /// Parameters of Normalized Root Density Function from Dwyer et al 19xx
    float a = 1.15f, b = 11.7f, c = 0.022f,
        d = 0.12029f; /// Integral of Normalized Root Distribution Function  from 0 to 1 (normalized depth) = 0.12029
    int k;/// used as layer identifier
    for (int l = 0; l < (int) soillayers[i]; l++) {
        cum_d += soilthick[i][l];
        if (cum_d >= lastSoilRD[i]) cum_rd = lastSoilRD[i];
        if (cum_d < lastSoilRD[i]) cum_rd = cum_d;
        x1 = (cum_rd - soilthick[i][l]) / lastSoilRD[i];
        x2 = cum_rd / lastSoilRD[i];
        float xx1 = -b * x1;
        if (xx1 > 20.f) xx1 = 20.f;
        float xx2 = -b * x2;
        if (xx2 > 20.f) xx2 = 20.f;
        root_fr[l] = (a / b * (exp(xx1) - exp(xx2)) + c * (x2 - x1)) / d;
        float xx = cum_rf;
        cum_rf += root_fr[l];
        if (cum_rf > 1.f) {
            root_fr[l] = 1.f - xx;
            cum_rf = 1.f;
        }
        k = l;
        if (cum_rd >= lastSoilRD[i]) {
            break;
        }
    }
    /// ensures that cumulative fractional root distribution = 1
    for (int l = 0; l < (int) soillayers[i]; l++) {
        root_fr[l] /= cum_rf;
        if (l == k) { /// exits loop on the same layer as the previous loop
            break;
        }
    }
}

void MGTOpt_SWAT::ExecuteTillageOperation(int i, int &factoryID, int nOp) {
    /// newtillmix.f
    /// Mix residue and nutrients during tillage and biological mixing
    TillageOperation *curOperation = (TillageOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// initialize parameters
    int tillID = curOperation->TillageID();
    float cnop = curOperation->CNOP();
    if (m_tillageLookupMap.find(tillID) == m_tillageLookupMap.end()) {
        throw ModelException(MID_PLTMGT_SWAT, "ExecuteTillageOperation", "The tillage ID " + ValueToString(tillID)
            + " is not existed in tillage lookup table!");
    }
    float deptil = m_tillageLookupMap[tillID][TILLAGE_PARAM_DEPTIL_IDX];
    float effmix = m_tillageLookupMap[tillID][TILLAGE_PARAM_EFFMIX_IDX];
    float bmix = 0.f;
    float emix = 0.f, dtil = 0.f, XX = 0.f, WW1 = 0.f, WW2 = 0.f;
    float WW3 = 0.f, WW4 = 0.f, maxmix = 0.f;
    float *soilMass = new float[(int) soillayers[i]];
    float *soilMixedMass = new float[(int) soillayers[i]];  /// mass of soil mixed for the layer
    float *soilNotMixedMass = new float[(int) soillayers[i]];  /// mass of soil not mixed for the layer

    if (bmix > UTIL_ZERO) {
        /// biological mixing, TODO, in SWAT, this occurs at the end of year process.
        emix = bmix;
        dtil = min(soilDepth[i][(int) soillayers[i] - 1], 50.f);
    } else {
        /// tillage operation
        emix = effmix;
        dtil = deptil;
    }
    if (tillID >= 1) {
        /// TODO, this is associated with erosion modules. Drainmod tile equations
        ///Updated dynamic depressional storage D.Moriasi 4/8/2014
        //cumei(jj)   = 0.
        //cumeira(jj) = 0.
        //cumrt(jj)   = 0.
        //cumrai(jj)  = 0.
        //ranrns_hru(jj) = ranrns(idtill)
    }
    if (cswat == 2) /// DSSAT tillage
    {
        tillage_days[i] = 0;
        tillage_depth[i] = dtil;
        tillage_switch[i] = 1;
    }
    ///  smix(:)     |varies        |amount of substance in soil profile that is being redistributed between mixed layers
    int npmx = 0; /// number of different pesticides, TODO in next version
    //float *smix = new float[22 + npmx + 12];
    //for (int k = 0; k < 22 + npmx + 12; k++)
    //    smix[k] = 0.f;
    float *smix(NULL);
    Initialize1DArray(22 + npmx + 12, smix, 0.f);
    /// incorporate bacteria, no mixing, and lost from transport
    if (dtil > 10.f) {
        //m_bactPersistSol[i] *= (1. - emix);
        //m_bactPersistParticle[i] *= (1. - emix);
        //m_bactLessPersistSol[i] *= (1. - emix);
        //m_bactLessPersistParticle[i] *= (1. - emix);
    }
    /// calculate max mixing to preserve target surface residue
    /// Assume residue in all other layers is negligible to simplify calculation and remove depth dependency
    /// TODO, m_minResidue is defined in OPS files in SWAT as residue management operation, 
    ///       which should be implemented later as BMP operation. By LJ
    float m_minResidue = 10.f; /// currently, set m_minResidue to 10 as default.
    if (m_minResidue > 1.f && bmix < 0.001f) {
        maxmix = 1.f - m_minResidue / sol_rsd[i][0];
        if (maxmix < 0.05f) maxmix = 0.05f;
        if (emix > maxmix) emix = maxmix;
    }
    for (int l = 0; l < (int) soillayers[i]; l++) {
        soilMass[l] = 10000.f * soilthick[i][l] * density[i][l] * (1 - rock[i][l] / 100.f);
        soilMixedMass[l] = 0.f;
        soilNotMixedMass[l] = 0.f;
    }
    if (dtil > 0.f) {
        if (dtil < 10.f) dtil = 11.;
        for (int l = 0; l < (int) soillayers[i]; l++) {
            if (soilDepth[i][l] <= dtil) {
                soilMixedMass[l] = emix * soilMass[l];
                soilNotMixedMass[l] = soilMass[l] - soilMixedMass[l];
            } else if (soilDepth[i][l] > dtil && soilDepth[i][l - 1] < dtil) {
                soilMixedMass[l] = emix * soilMass[l] * (dtil - soilDepth[i][l - 1]) / soilthick[i][l];
                soilNotMixedMass[l] = soilMass[l] - soilMixedMass[l];
            } else {
                soilMixedMass[l] = 0.f;
                soilNotMixedMass[l] = soilMass[l];
            }
            /// calculate the mass or concentration of each mixed element
            /// 1. mass based mixing
            WW1 = soilMixedMass[l] / (soilMixedMass[l] + soilNotMixedMass[l]);
            smix[0] += sol_no3[i][l] * WW1;
            smix[1] += sol_orgn[i][l] * WW1;
            smix[2] += sol_nh4[i][l] * WW1;
            smix[3] += sol_solp[i][l] * WW1;
            smix[4] += sol_orgp[i][l] * WW1;
            smix[5] += sol_aorgn[i][l] * WW1;
            smix[6] += sol_actp[i][l] * WW1;
            smix[7] += sol_fon[i][l] * WW1;
            smix[8] += sol_fop[i][l] * WW1;
            smix[9] += sol_stap[i][l] * WW1;
            smix[10] += sol_rsd[i][l] * WW1;
            if (cswat == 1) /// C-FARM one carbon pool model
            {
                smix[11] += sol_mc[i][l] * WW1;
                smix[12] += sol_mn[i][l] * WW1;
                smix[13] += sol_mp[i][l] * WW1;
            }

            /// 2. concentration based mixing
            WW2 = XX + soilMixedMass[l];
            smix[14] = (XX * smix[14] + sol_cbn[i][l] * soilMixedMass[l]) / WW2;
            smix[15] = (XX * smix[15] + sol_N[i][l] * soilMixedMass[l]) / WW2;
            smix[16] = (XX * smix[16] + CLAY[i][l] * soilMixedMass[l]) / WW2;
            smix[17] = (XX * smix[17] + silt[i][l] * soilMixedMass[l]) / WW2;
            smix[18] = (XX * smix[18] + sand[i][l] * soilMixedMass[l]) / WW2;
            /// 3. mass based distribution
            for (int k = 0; k < npmx; k++) {
                /// TODO
                /// smix[19+k] += sol_pst(k,jj,l) * WW1
            }
            /// 4. For CENTURY model
            if (cswat == 2) {
                smix[19 + npmx + 1] += sol_LSC[i][l] * WW1;
                smix[19 + npmx + 2] += sol_LSLC[i][l] * WW1;
                smix[19 + npmx + 3] += sol_LSLNC[i][l] * WW1;
                smix[19 + npmx + 4] += sol_LMC[i][l] * WW1;
                smix[19 + npmx + 5] += sol_LM[i][l] * WW1;
                smix[19 + npmx + 6] += sol_LSL[i][l] * WW1;
                smix[19 + npmx + 7] += sol_LS[i][l] * WW1;

                smix[19 + npmx + 8] += sol_LSN[i][l] * WW1;
                smix[19 + npmx + 9] += sol_LMN[i][l] * WW1;
                smix[19 + npmx + 10] += sol_BMN[i][l] * WW1;
                smix[19 + npmx + 11] += sol_HSN[i][l] * WW1;
                smix[19 + npmx + 12] += sol_HPN[i][l] * WW1;
            }
            XX += soilMixedMass[l];
        }
        for (int l = 0; l < (int) soillayers[i]; l++) {
            /// reconstitute each soil layer
            WW3 = soilNotMixedMass[l] / soilMass[l];
            WW4 = soilMixedMass[l] / XX;
            sol_no3[i][l] = sol_no3[i][l] * WW3 + smix[0] * WW4;
            sol_orgn[i][l] = sol_orgn[i][l] * WW3 + smix[1] * WW4;
            sol_nh4[i][l] = sol_nh4[i][l] * WW3 + smix[2] * WW4;
            sol_solp[i][l] = sol_solp[i][l] * WW3 + smix[3] * WW4;
            sol_orgp[i][l] = sol_orgp[i][l] * WW3 + smix[4] * WW4;
            sol_aorgn[i][l] = sol_aorgn[i][l] * WW3 + smix[5] * WW4;
            sol_actp[i][l] = sol_actp[i][l] * WW3 + smix[6] * WW4;
            sol_fon[i][l] = sol_fon[i][l] * WW3 + smix[7] * WW4;
            sol_fop[i][l] = sol_fop[i][l] * WW3 + smix[8] * WW4;
            sol_stap[i][l] = sol_stap[i][l] * WW3 + smix[9] * WW4;
            sol_rsd[i][l] = sol_rsd[i][l] * WW3 + smix[10] * WW4;
            if (sol_rsd[i][l] < 1.e-10f) sol_rsd[i][l] = 1.e-10f;
            if (cswat == 1) {
                sol_mc[i][l] = sol_mc[i][l] * WW3 + smix[11] * WW4;
                sol_mn[i][l] = sol_mn[i][l] * WW3 + smix[12] * WW4;
                sol_mp[i][l] = sol_mp[i][l] * WW3 + smix[13] * WW4;
            }
            sol_cbn[i][l] = (sol_cbn[i][l] * soilNotMixedMass[l] + smix[14] * soilMixedMass[l]) / soilMass[l];
            sol_N[i][l] = (sol_N[i][l] * soilNotMixedMass[l] + smix[15] * soilMixedMass[l]) / soilMass[l];
            CLAY[i][l] = (CLAY[i][l] * soilNotMixedMass[l] + smix[16] * soilMixedMass[l]) / soilMass[l];
            silt[i][l] = (silt[i][l] * soilNotMixedMass[l] + smix[17] * soilMixedMass[l]) / soilMass[l];
            sand[i][l] = (sand[i][l] * soilNotMixedMass[l] + smix[18] * soilMixedMass[l]) / soilMass[l];

            for (int k = 0; k < npmx; k++) {
                /// TODO
                /// sol_pst(k,jj,l) = sol_pst(k,jj,l) * WW3 + smix(20+k) * WW4
            }
            if (cswat == 2) {
                sol_LSC[i][l] = sol_LSC[i][l] * WW3 + smix[19 + npmx + 1] * WW4;
                sol_LSLC[i][l] = sol_LSLC[i][l] * WW3 + smix[19 + npmx + 2] * WW4;
                sol_LSLNC[i][l] = sol_LSLNC[i][l] * WW3 + smix[19 + npmx + 3] * WW4;
                sol_LMC[i][l] = sol_LMC[i][l] * WW3 + smix[19 + npmx + 4] * WW4;
                sol_LM[i][l] = sol_LM[i][l] * WW3 + smix[19 + npmx + 5] * WW4;
                sol_LSL[i][l] = sol_LSL[i][l] * WW3 + smix[19 + npmx + 6] * WW4;
                sol_LS[i][l] = sol_LS[i][l] * WW3 + smix[19 + npmx + 7] * WW4;
                sol_LSN[i][l] = sol_LSN[i][l] * WW3 + smix[19 + npmx + 8] * WW4;
                sol_LMN[i][l] = sol_LMN[i][l] * WW3 + smix[19 + npmx + 9] * WW4;
                sol_BMN[i][l] = sol_BMN[i][l] * WW3 + smix[19 + npmx + 10] * WW4;
                sol_HSN[i][l] = sol_HSN[i][l] * WW3 + smix[19 + npmx + 11] * WW4;
                sol_HPN[i][l] = sol_HPN[i][l] * WW3 + smix[19 + npmx + 12] * WW4;
            }
            if (cswat == 1) {
                /// TODO
                /// call tillfactor(jj,bmix,emix,dtil,sol_thick)
            }
        }
    }
    if (cnop > 1.e-4f) CN2[i] = cnop;
    if (soilMass != NULL) Release1DArray(soilMass);
    if (soilMixedMass != NULL) Release1DArray(soilMixedMass);
    if (soilNotMixedMass != NULL) Release1DArray(soilNotMixedMass);
}

void MGTOpt_SWAT::ExecuteHarvestOnlyOperation(int i, int &factoryID, int nOp) {
    /// TODO to be implemented!
    /// harvestop.f
    //  HarvestOnlyOperation *curOperation = (HarvestOnlyOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    //  /// initialize parameters
    //  float hi_bms = curOperation->HarvestIndexBiomass();
    //  float hi_rsd = curOperation->HarvestIndexResidue();
    //  float harveff = curOperation->HarvestEfficiency();
    //  if (m_cropLookupMap.find(int(landcover[i])) == m_cropLookupMap.end())
    //throw ModelException(MID_PLTMGT_SWAT, "ExecuteHarvestOnlyOperation", "The landcover ID " + ValueToString(landcover[i])
    //+ " is not existed in crop lookup table!");
    //  /// Get some parameters of current crop / landcover
    //  float hvsti = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_HVSTI];
    //  float wsyf = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_WSYF];
    //  int idc = (int) m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_IDC];
    //  float bio_leaf = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_BIO_LEAF];
    //  float cnyld = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_CNYLD];
    //  float cpyld = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_CPYLD];
    //  float alai_min = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_ALAI_MIN];

    //  float yieldTbr = 0.f, yieldNtbr = 0.f, yieldPtbr = 0.f;
    //  float clipTbr = 0.f, clipNtbr = 0.f, clipPtbr = 0.f;
    //  float ssb = BIOMASS[i];
    //  float ssabg = BIOMASS[i] * (1.f - frRoot[i]);
    //  float ssr = ssb * frRoot[i];
    //  float ssn = plant_N[i];
    //  float ssp = plant_P[i];
    //  /// calculate modifier for auto fertilization target nitrogen content
    //  afert_nyldTarg[i] = (1.f - frRoot[i] * BIOMASS[i] * frPlantN[i] * afert_frteff[i]);
    //  /// compute grain yield
    //  float hiad1 = 0.f; /// hiad1       |none           |actual harvest index (adj for water/growth)
    //  float wur = 0.f; /// wur         |none           |water deficiency factor
    //  if (plt_pet_tot[i] < 10.f)
    //      wur = 100.f;
    //  else
    //      wur = 100.f * plt_et_tot[i] / plt_pet_tot[i];
    //  hiad1 = (hvsti_adj[i] - wsyf) * (wur / (wur + exp(6.13f - 0.0883f * wur))) + wsyf;
    //  if (hiad1 > hvsti) hiad1 = hvsti;
    //  /// check if yield is from above or below ground
    //  if (hvsti > 1.001f)
    //  {
    //      /// determine clippings (biomass left behind) and update yield
    //      yieldTbr = BIOMASS[i] * (1.f - 1.f / (1.f + hiad1)) * harveff;
    //      clipTbr = BIOMASS[i] * (1.f - 1.f / (1.f + hiad1)) * (1.f - harveff);
    //      BIOMASS[i] -= (yieldTbr + clipTbr);
    //      /// calculate nutrients removed with yield
    //      yieldNtbr = min(yieldTbr * cnyld, 0.80f * plant_N[i]);
    //      yieldPtbr = min(yieldTbr * cpyld, 0.80f * plant_P[i]);
    //      /// calculate nutrients removed with clippings
    //      clipNtbr = min(clipTbr * frPlantN[i], plant_N[i] - yieldNtbr);
    //      clipPtbr = min(clipTbr * frPlantP[i], plant_P[i] - yieldPtbr);
    //      plant_N[i] -= (yieldNtbr + clipNtbr);
    //      plant_P[i] -= (yieldPtbr + clipPtbr);
    //  }
    //  float yieldBms = 0.f, yieldNbms = 0.f, yieldPbms = 0.f;
    //  float clipBms = 0.f, clipNbms = 0.f, clipPbms = 0.f;
    //  float yieldGrn = 0.f, yieldNgrn = 0.f, yieldPgrn = 0.f;
    //  float clipGrn = 0.f, clipNgrn = 0.f, clipPgrn = 0.f;

    //  if (hi_bms > 0.f)
    //  {
    //      /// compute biomass yield
    //      yieldBms = hi_bms * (1.f - frRoot[i]) * BIOMASS[i] * harveff;
    //      clipBms = hi_bms * (1.f - frRoot[i]) * BIOMASS[i] * (1.f - harveff);
    //      BIOMASS[i] -= (yieldBms + clipBms);
    //      /// compute nutrients removed with yield
    //      yieldNbms = min(yieldBms * cnyld, 0.80f * plant_N[i]);
    //      yieldPbms = min(yieldBms * cpyld, 0.80f * plant_P[i]);
    //      /// calculate nutrients removed with clippings
    //      clipNbms = min(clipBms * frPlantN[i], plant_N[i] - yieldNbms);
    //      clipPbms = min(clipBms * frPlantP[i], plant_P[i] - yieldPbms);
    //      plant_N[i] -= (yieldNbms + clipNbms);
    //      plant_P[i] -= (yieldPbms + clipPbms);
    //  } else
    //  {
    //      /// compute grain yields
    //      yieldGrn = (1.f - frRoot[i]) * BIOMASS[i] * hiad1 * harveff;
    //      /// determine clippings (biomass left behind) and update yield
    //      clipGrn = (1.f - frRoot[i]) * BIOMASS[i] * hiad1 * (1.f - harveff);
    //      BIOMASS[i] -= (yieldGrn + clipGrn);
    //      /// calculate nutrients removed with yield
    //      yieldNgrn = min(yieldGrn * cnyld, 0.80f * plant_N[i]);
    //      yieldPgrn = min(yieldGrn * cpyld, 0.80f * plant_P[i]);
    //      /// calculate nutrients removed with clippings
    //      clipNgrn = min(clipGrn * frPlantN[i], plant_N[i] - yieldNgrn);
    //      clipPgrn = min(clipGrn * frPlantP[i], plant_P[i] - yieldPgrn);
    //      plant_N[i] -= (yieldNgrn + clipNgrn);
    //      plant_P[i] -= (yieldPgrn + clipPgrn);
    //  }
    //  /// add clippings to residue and organic N and P
    //  sol_rsd[i][0] += (clipGrn + clipBms + clipTbr);
    //  sol_fon[i][0] += (clipNgrn + clipNbms + clipNtbr);
    //  sol_fop[i][0] += (clipPgrn + clipPbms + clipPtbr);
    //  /// compute residue yield
    //  float yieldRsd = 0.f, yieldNrsd = 0.f, yieldPrsd = 0.f;
    //  if (hi_rsd > 0.)
    //  {
    //      yieldRsd = hi_rsd * sol_rsd[i][0];
    //      yieldNrsd = hi_rsd * sol_fon[i][0];
    //      yieldPrsd = hi_rsd * sol_fop[i][0];
    //      sol_rsd[i][0] -= yieldRsd;
    //      sol_fon[i][0] -= yieldNrsd;
    //      sol_fop[i][0] -= yieldPrsd;
    //  }
    //  float yield = 0.f, yieldN = 0.f, yieldP = 0.f;
    //  yield = yieldGrn + yieldBms + yieldTbr + yieldRsd;
    //  yieldN = yieldNgrn + yieldNbms + yieldNtbr + yieldNrsd;
    //  yieldP = yieldPgrn + yieldPbms + yieldPtbr + yieldPrsd;
    //  float clip = 0.f, clipN = 0.f, clipP = 0.f;
    //  clip = clipGrn + clipBms + clipTbr;
    //  clipN = clipNgrn + clipNbms + clipNtbr;
    //  clipP = clipPgrn + clipPbms + clipNtbr;

    //  /// Calculation for dead roots allocations, resetting phenology, updating other pools
    //  float ff3 = 0.f;
    //  if (ssabg > UTIL_ZERO)
    //      ff3 = (yield + clip) / ssabg;
    //  else
    //      ff3 = 1.f;
    //  if (ff3 > 1.f) ff3 = 1.f;
    //  /// reset leaf area index and fraction of growing season
    //  if (ssb > 0.001f)
    //  {
    //      LAIDAY[i] *= (1.f - ff3);
    //      if (LAIDAY[i] < alai_min)
    //          LAIDAY[i] = alai_min;
    //      frPHUacc[i] *= (1.f - ff3);
    //      frRoot[i] = 0.4f - 0.2f * frPHUacc[i];
    //  } else
    //  {
    //      BIOMASS[i] = 0.f;
    //      LAIDAY[i] = 0.f;
    //      frPHUacc[i] = 0.f;
    //  }
    //  /// allocate roots, N, and P to soil pools
    //  for (int l = 0; l < (int) soillayers[i]; l++)
    //  {
    //      /// TODO, check it out in the near future.
    //  }
}

void MGTOpt_SWAT::ExecuteKillOperation(int i, int &factoryID, int nOp) {
    /// killop.f
    // KillOperation *curOperation = (KillOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    float resnew = 0.f, rtresnew = 0.f;
    resnew = BIOMASS[i] * (1.f - frRoot[i]);
    rtresnew = BIOMASS[i] * frRoot[i];
    /// call rootfr.f to distributes dead root mass through the soil profile
    /// i.e., derive fraction of roots in each layer
    float *rtfr = new float[(int) soillayers[i]];
    rootFraction(i, rtfr);
    /// update residue, N, P on soil surface
    sol_rsd[i][0] += resnew;
    sol_fon[i][0] += plant_N[i] * (1.f - frRoot[i]);
    sol_fop[i][0] += plant_P[i] * (1.f - frRoot[i]);
    sol_rsd[i][0] = max(sol_rsd[i][0], 0.f);
    sol_fon[i][0] = max(sol_fon[i][0], 0.f);
    sol_fop[i][0] = max(sol_fop[i][0], 0.f);

    /// allocate dead roots, N and P to soil layers
    for (int l = 0; l < (int) soillayers[i]; l++) {
        sol_rsd[i][l] += rtfr[l] * rtresnew;
        sol_fon[i][l] += rtfr[l] * plant_N[i] * frRoot[i];
        sol_fop[i][l] += rtfr[l] * plant_P[i] * frRoot[i];
    }
    /// reset variables
    IGRO[i] = 0.f;
    dormi[i] = 0.f;
    BIOMASS[i] = 0.f;
    frRoot[i] = 0.f;
    plant_N[i] = 0.f;
    plant_P[i] = 0.f;
    frStrsWtr[i] = 1.f;
    LAIDAY[i] = 0.f;
    hvsti_adj[i] = 0.f;
    Release1DArray(rtfr);
    frPHUacc[i] = 0.f;
}

void MGTOpt_SWAT::ExecuteGrazingOperation(int i, int &factoryID, int nOp) {
    /// TODO
    /// graze.f, simulate biomass lost to grazing
    //GrazingOperation *curOperation = (GrazingOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    //int manureID = curOperation->ManureID();
    //int grzDays = curOperation->GrazingDays();
    //float bioEat = curOperation->BiomassConsumed();
    //float bioTrmp = curOperation->BiomassTrampled();
    //float manueKg = curOperation->ManureDeposited();
}

void MGTOpt_SWAT::ExecuteAutoIrrigationOperation(int i, int &factoryID, int nOp) {
    AutoIrrigationOperation *curOperation = (AutoIrrigationOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    airr_source[i] = curOperation->AutoIrrSrcCode();
    airr_location[i] = (curOperation->AutoIrrSrcLocs() <= 0) ? (int) subbasin[i] : curOperation->AutoIrrSrcLocs();
    awtr_strsID[i] = curOperation->WaterStrsIdent();
    awtr_strsTrig[i] = curOperation->AutoWtrStrsThrsd();
    airr_eff[i] = curOperation->IrrigationEfficiency();
    airrwtr_depth[i] = curOperation->IrrigationWaterApplied();
    airrsurf_ratio[i] = curOperation->SurfaceRunoffRatio();
    irr_flag[i] = 1;
    /// call autoirr.f
    /// TODO, this will be implemented as an isolated module in the near future.
}

void MGTOpt_SWAT::ExecuteAutoFertilizerOperation(int i, int &factoryID, int nOp) {
    AutoFertilizerOperation *curOperation = (AutoFertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    afert_id[i] = curOperation->FertilizerID();
    afert_nstrsID[i] = curOperation->NitrogenMethod();
    afert_nstrs[i] = curOperation->NitrogenStrsFactor();
    afert_maxN[i] = curOperation->MaxMineralN();
    afert_AmaxN[i] = curOperation->MaxMineralNYearly();
    afert_frteff[i] = curOperation->FertEfficiency();
    afert_frtsurf[i] = curOperation->SurfaceFracApplied();
    if (m_cropLookupMap.find(int(landcover[i])) == m_cropLookupMap.end()) {
        return;
    }
    float cnyld = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_CNYLD];
    float bio_e = m_cropLookupMap[(int) landcover[i]][CROP_PARAM_IDX_BIO_E];
    /// calculate modifier for auto fertilization target nitrogen content'
    if (afert_nyldTarg[i] < UTIL_ZERO) {
        afert_nyldTarg[i] = 150.f * cnyld * bio_e;
    }
    /// call anfert.f
    /// TODO, this will be implemented as an isolated module in the near future.
}

void MGTOpt_SWAT::ExecuteReleaseImpoundOperation(int i, int &factoryID, int nOp) {
    /// No more executable code here.
    ReleaseImpoundOperation *curOperation = (ReleaseImpoundOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    impound_trig[i] = curOperation->ImpoundTriger();
    /// pothole.f and potholehr.f for sub-daily timestep simulation, TODO
    /// IF IMP_SWAT module is not configured, then this operation will be ignored. By LJ
    if (pot_vol == NULL) {
        return;
    }
    /// 1. pothole module has been added by LJ, 2016-9-6, IMP_SWAT
    /// paddy rice module should be added!
    pot_volmaxmm[i] = curOperation->MaxDepth();
    pot_vollowmm[i] = curOperation->LowDepth();
    if (FloatEqual(impound_trig[i], 0.f)) {
        /// Currently, add pothole volume (mm) to the max depth directly (in case of infiltration).
        /// TODO, autoirrigation operations should be triggered. BY lj
        pot_vol[i] = curOperation->MaxDepth();
        /// force the soil water storage to field capacity
        for (int ly = 0; ly < (int) soillayers[i]; ly++) {
            // float dep2cap = sol_ul[i][ly] - solst[i][ly];
            float dep2cap = sol_awc[i][ly] - solst[i][ly];
            if (dep2cap > 0.f) {
                dep2cap = min(dep2cap, pot_vol[i]);
                solst[i][ly] += dep2cap;
                pot_vol[i] -= dep2cap;
            }
        }
        if (pot_vol[i] < curOperation->UpDepth()) {
            pot_vol[i] = curOperation->UpDepth();
        } /// force to reach the up depth.
        /// recompute total soil water storage
        solsw[i] = 0.f;
        for (int ly = 0; ly < (int) soillayers[i]; ly++)
            solsw[i] += solst[i][ly];
    } else {
        pot_volmaxmm[i] = 0.f;
        pot_vollowmm[i] = 0.f;
    }
}

void MGTOpt_SWAT::ExecuteContinuousFertilizerOperation(int i, int &factoryID, int nOp) {
    // TODO
    // ContinuousFertilizerOperation *curOperation = (ContinuousFertilizerOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
}

void MGTOpt_SWAT::ExecuteContinuousPesticideOperation(int i, int &factoryID, int nOp) {
    /// TODO
    // ContinuousPesticideOperation *curOperation = (ContinuousPesticideOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
}

void MGTOpt_SWAT::ExecuteBurningOperation(int i, int &factoryID, int nOp) {
    // BurningOperation *curOperation = (BurningOperation *) m_mgtFactory[factoryID]->GetOperations()[nOp];
    /// TODO
}

void MGTOpt_SWAT::ScheduledManagement(int cellIdx, int &factoryID, int nOp) {
    /// nOp is seqNo. * 1000 + operationCode
    int mgtCode = nOp % 1000;
    switch (mgtCode) {
        case BMP_PLTOP_Plant:ExecutePlantOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Irrigation:ExecuteIrrigationOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Fertilizer:ExecuteFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Pesticide:ExecutePesticideOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_HarvestKill:ExecuteHarvestKillOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Tillage:ExecuteTillageOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Harvest:ExecuteHarvestOnlyOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Kill:ExecuteKillOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Grazing:ExecuteGrazingOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_AutoIrrigation:ExecuteAutoIrrigationOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_AutoFertilizer:ExecuteAutoFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ReleaseImpound:ExecuteReleaseImpoundOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ContinuousFertilizer:ExecuteContinuousFertilizerOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_ContinuousPesticide:ExecuteContinuousPesticideOperation(cellIdx, factoryID, nOp);
            break;
        case BMP_PLTOP_Burning:ExecuteBurningOperation(cellIdx, factoryID, nOp);
            break;
    }
}

int MGTOpt_SWAT::Execute() {
    CheckInputData();  /// essential input data, other inputs for specific management operation will be check separately.
    initialOutputs(); /// all possible outputs will be initialized to avoid NULL pointer problems.
    /// initialize arrays at the beginning of the current day, derived from sim_initday.f of SWAT
    //cout<<"PLTMGT_SWAT, pre: "<<sol_solp[46364][0];
    //int cellid = 918;
    //cout<<"PLTMGT_SWAT, cell id: "<<cellid<<", sol_no3[0]: "<<sol_no3[cellid][0]<<endl;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        if (grainc_d != NULL) grainc_d[i] = 0.f;
        if (m_stoverc_d != NULL) m_stoverc_d[i] = 0.f;
        if (m_rsdc_d != NULL) m_rsdc_d[i] = 0.f;
    }
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        int curFactoryID = -1;
        vector<int> curOps;
        /* Output HUSC to txt files for debugging purpose */
        //if (i == 8144){
        //	ofstream fs;
        //	utils util;
        //	string filename = "e:\\husc.txt";
        //	fs.open(filename.c_str(), ios::out|ios::app);
        //	if (fs.is_open())
        //	{
        //		fs << util.ConvertToString(&this->m_date) <<", IGRO: "<<IGRO[i]<<", phuBase: "<<PHUBASE[i]<<", phuAcc: "<<frPHUacc[i]<<", phuPlt: "<<PHU_PLT[i]<< endl;
        //		fs.close();
        //	}
        //}
        if (GetOperationCode(i, curFactoryID, curOps)) {
            for (vector<int>::iterator it = curOps.begin(); it != curOps.end(); it++) {
                //cout<<curFactoryID<<","<<*it<<endl;
                ScheduledManagement(i, curFactoryID, *it);
                // output for debug, by LJ.
                //if (i == 8144){
                //	ofstream fs;
                //	utils util;
                //	//string filename = "D:\\pltMgt.txt";
                //	string filename = "pltMgt.txt";
                //	fs.open(filename.c_str(), ios::out|ios::app);
                //	if (fs.is_open())
                //	{
                //		fs << util.ConvertToString(&this->m_date) <<", IGRO: "<<IGRO[i]<<", phuBase: "<<PHUBASE[i]<<", phuAcc: "<<frPHUacc[i]<<", phuPlt: "<<PHU_PLT[i]<<", optCode: "<<*it<< endl;
                //		fs.close();
                //	}
                //}
            }
        }
    }
    //cout<<"PLTMGT_SWAT, cell id: "<<cellid<<", sol_no3[0]: "<<sol_no3[cellid][0]<<endl;
    //cout<<", new: "<<sol_solp[46364][0]<<endl;
    return 0;
}

void MGTOpt_SWAT::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = m_nCells;
    /// plant operation
    if (StringMatch(sk, VAR_HITARG)) { *data = hi_targ; }
    else if (StringMatch(sk, VAR_BIOTARG)) {
        *data = biotarg;
        /// auto irrigation operation
    } else if (StringMatch(sk, VAR_IRR_FLAG)) { *data = irr_flag; }
    else if (StringMatch(sk, VAR_IRR_WTR)) { *data = irr_water; }
    else if (StringMatch(sk, VAR_IRR_SURFQ)) { *data = irr_surfq; }
    else if (StringMatch(sk, VAR_AWTR_STRS_ID)) { *data = awtr_strsID; }
    else if (StringMatch(sk, VAR_AWTR_STRS_TRIG)) { *data = awtr_strsTrig; }
    else if (StringMatch(sk, VAR_AIRR_SOURCE)) { *data = airr_source; }
    else if (StringMatch(sk, VAR_AIRR_LOCATION)) { *data = airr_location; }
    else if (StringMatch(sk, VAR_AIRR_EFF)) { *data = airr_eff; }
    else if (StringMatch(sk, VAR_AIRRWTR_DEPTH)) { *data = airrwtr_depth; }
    else if (StringMatch(sk, VAR_AIRRSURF_RATIO)) {
        *data = airrsurf_ratio;
        /// fertilizer / auto fertilizer operation
    } else if (StringMatch(sk, VAR_AFERT_ID)) { *data = afert_id; }
    else if (StringMatch(sk, VAR_AFERT_NSTRSID)) { *data = afert_nstrsID; }
    else if (StringMatch(sk, VAR_AFERT_NSTRS)) { *data = afert_nstrs; }
    else if (StringMatch(sk, VAR_AFERT_MAXN)) { *data = afert_maxN; }
    else if (StringMatch(sk, VAR_AFERT_AMAXN)) { *data = afert_AmaxN; }
    else if (StringMatch(sk, VAR_AFERT_NYLDT)) { *data = afert_nyldTarg; }
    else if (StringMatch(sk, VAR_AFERT_FRTEFF)) { *data = afert_frteff; }
    else if (StringMatch(sk, VAR_AFERT_FRTSURF)) {
        *data = afert_frtsurf;
        /// Grazing operation
    } else if (StringMatch(sk, VAR_GRZ_DAYS)) { *data = grz_days; }
    else if (StringMatch(sk, VAR_GRZ_FLAG)) {
        *data = grz_flag;
        /// Impound/Release operation
    } else if (StringMatch(sk, VAR_IMPOUND_TRIG)) { *data = impound_trig; }
    else if (StringMatch(sk, VAR_POT_VOLMAXMM)) { *data = pot_volmaxmm; }
    else if (StringMatch(sk, VAR_POT_VOLLOWMM)) {
        *data = pot_vollowmm;
        /// tillage operation of CENTURY model
    } else if (StringMatch(sk, VAR_TILLAGE_DAYS)) { *data = tillage_days; }
    else if (StringMatch(sk, VAR_TILLAGE_DEPTH)) { *data = tillage_depth; }
    else if (StringMatch(sk, VAR_TILLAGE_FACTOR)) { *data = tillage_factor; }
    else if (StringMatch(sk, VAR_TILLAGE_SWITCH)) { *data = tillage_switch; }
    else {
        throw ModelException(MID_PLTMGT_SWAT, "Get1DData", "Parameter " + sk + " is not existed!");
    }
}

void MGTOpt_SWAT::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = nSoilLayers;
    /// fertilizer operation
    if (StringMatch(sk, VAR_SOL_MC)) { *data = sol_mc; }
    else if (StringMatch(sk, VAR_SOL_MN)) { *data = sol_mn; }
    else if (StringMatch(sk, VAR_SOL_MP)) { *data = sol_mp; }
    else {
        throw ModelException(MID_PLTMGT_SWAT, "Get1DData", "Parameter " + sk + " is not existed!");
    }
}

void MGTOpt_SWAT::initialOutputs() {
    if (cellArea < 0.f) cellArea = CELLWIDTH * CELLWIDTH / 10000.f; // unit: ha
    /// figure out all the management codes, and initialize the corresponding variables, aimed to save memory. By LJ
    if (initialized) return;
    vector<int> definedMgtCodes;
    for (map<int, BMPPlantMgtFactory *>::iterator it = m_mgtFactory.begin(); it != m_mgtFactory.end(); it++) {
        int factoryID = it->first;
        vector<int> tmpOpSeqences = m_mgtFactory[factoryID]->GetOperationSequence();
        for (vector<int>::iterator seqIter = tmpOpSeqences.begin(); seqIter != tmpOpSeqences.end(); seqIter++) {
            /// *seqIter is calculated by: seqNo. * 1000 + operationCode
            int curMgtCode = *seqIter % 1000;
            if (find(definedMgtCodes.begin(), definedMgtCodes.end(), curMgtCode) == definedMgtCodes.end()) {
                definedMgtCodes.push_back(curMgtCode);
            }
        }
    }
    /// plant operation
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_Plant) != definedMgtCodes.end()) {
        if (hi_targ == NULL) Initialize1DArray(m_nCells, hi_targ, 0.f);
        if (biotarg == NULL) Initialize1DArray(m_nCells, biotarg, 0.f);
    }
    /// irrigation / auto irrigation operations
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_Irrigation) != definedMgtCodes.end() ||
        find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_AutoIrrigation) != definedMgtCodes.end()) {
        if (irr_water == NULL) Initialize1DArray(m_nCells, irr_water, 0.f);
        if (irr_surfq == NULL) Initialize1DArray(m_nCells, irr_surfq, 0.f);
        if (irr_flag == NULL) Initialize1DArray(m_nCells, irr_flag, 0.f);
        if (airr_source == NULL) Initialize1DArray(m_nCells, airr_source, (float) IRR_SRC_OUTWTSD);
        if (airr_location == NULL) Initialize1DArray(m_nCells, airr_location, -1.f);
        if (awtr_strsID == NULL) Initialize1DArray(m_nCells, awtr_strsID, 1.f); /// By default, plant water demand
        if (awtr_strsTrig == NULL) Initialize1DArray(m_nCells, awtr_strsTrig, 0.f);
        if (airr_eff == NULL) Initialize1DArray(m_nCells, airr_eff, 0.f);
        if (airrwtr_depth == NULL) Initialize1DArray(m_nCells, airrwtr_depth, 0.f);
        if (airrsurf_ratio == NULL) Initialize1DArray(m_nCells, airrsurf_ratio, 0.f);
    }
    /// fertilizer / auto fertilizer operations
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_Fertilizer) != definedMgtCodes.end() ||
        find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_AutoFertilizer) != definedMgtCodes.end()) {
        if (afert_id == NULL) Initialize1DArray(m_nCells, afert_id, -1.f);
        if (afert_nstrsID == NULL) Initialize1DArray(m_nCells, afert_nstrsID, 0.f);
        if (afert_nstrs == NULL) Initialize1DArray(m_nCells, afert_nstrs, 0.f);
        if (afert_maxN == NULL) Initialize1DArray(m_nCells, afert_maxN, 0.f);
        if (afert_nyldTarg == NULL) Initialize1DArray(m_nCells, afert_nyldTarg, 0.f);
        if (afert_AmaxN == NULL) Initialize1DArray(m_nCells, afert_AmaxN, 0.f);
        if (afert_frteff == NULL) Initialize1DArray(m_nCells, afert_frteff, 0.f);
        if (afert_frtsurf == NULL) Initialize1DArray(m_nCells, afert_frtsurf, 0.f);

        if (cswat == 1) {
            if (sol_mc == NULL) Initialize2DArray(m_nCells, nSoilLayers, sol_mc, 0.f);
            if (sol_mn == NULL) Initialize2DArray(m_nCells, nSoilLayers, sol_mn, 0.f);
            if (sol_mp == NULL) Initialize2DArray(m_nCells, nSoilLayers, sol_mp, 0.f);
        }
    }
    /// impound/release operation
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_ReleaseImpound) != definedMgtCodes.end()) {
        if (impound_trig == NULL) Initialize1DArray(m_nCells, impound_trig, 1.f);
        if (pot_volmaxmm == NULL) Initialize1DArray(m_nCells, pot_volmaxmm, 0.f);
        if (pot_vollowmm == NULL) Initialize1DArray(m_nCells, pot_vollowmm, 0.f);
    }
    /// tillage
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_Tillage) != definedMgtCodes.end()) {
        if (cswat == 2) {
            if (tillage_days == NULL) Initialize1DArray(m_nCells, tillage_days, 0.f);
            if (tillage_switch == NULL) Initialize1DArray(m_nCells, tillage_switch, 0.f);
            if (tillage_depth == NULL) Initialize1DArray(m_nCells, tillage_depth, 0.f);
            if (tillage_factor == NULL) Initialize1DArray(m_nCells, tillage_factor, 0.f);
        }
    }
    /// harvestkill
    if (find(definedMgtCodes.begin(), definedMgtCodes.end(), BMP_PLTOP_HarvestKill) != definedMgtCodes.end()) {
        if (cswat == 2) {
            if (grainc_d == NULL) Initialize1DArray(m_nCells, grainc_d, 0.f);
            if (m_stoverc_d == NULL) Initialize1DArray(m_nCells, m_stoverc_d, 0.f);
            if (m_rsdc_d == NULL) Initialize1DArray(m_nCells, m_rsdc_d, 0.f);
        }
    }
    if (doneOpSequence == NULL) Initialize1DArray(m_nCells, doneOpSequence, -1);
    initialized = true;
}

float MGTOpt_SWAT::Erfc(float xx) {
    float c1 = .19684f, c2 = .115194f;
    float c3 = .00034f, c4 = .019527f;
    float x = 0.f, erf = 0.f, erfc = 0.f;
    x = abs(sqrt(2.f) * xx);
    erf = 1.f - pow(float(1.f + c1 * x + c2 * x * x + c3 * pow(x, 3.f) + c4 * pow(x, 4.f)), -4.f);
    if (xx < 0.f) erf = -erf;
    erfc = 1.f - erf;
    return erfc;
}
