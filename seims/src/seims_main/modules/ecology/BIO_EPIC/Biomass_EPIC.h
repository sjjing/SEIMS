/*!
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637, plantmod.f
 * \author LiangJun Zhu
 * \date June 2016
 *
 * \review LiangJun Zhu
 * \date 2016-10-7
 * \description:  1. add some code of CENTURY calculation
 */

#pragma once

#include <visit_struct/visit_struct_intrusive.hpp>
#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "PlantGrowthCommon.h"
#include "ClimateParams.h"

using namespace std;
/** \defgroup BIO_EPIC
 * \ingroup Ecology
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637
 */
/*!
 * \class Biomass_EPIC
 * \ingroup BIO_EPIC
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637
 */
class Biomass_EPIC : public SimulationModule {
public:
    //! Constructor
    Biomass_EPIC(void);

    //! Destructor
    ~Biomass_EPIC(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputSize2D(const char *key, int n, int col);

    bool CheckInputData(void);

	BEGIN_VISITABLES(Biomass_EPIC);
	

	// @In
	// @Description valid cells number
    //int m_nCells;
	VISITABLE(int, m_nCells);	

	// @In
	// @Description years of climate data
    //int m_nClimDataYrs;
	VISITABLE(int, m_nClimDataYrs);

    /**  climate inputs  **/

	// @In
	// @Description CO2 concentration
    //float Co2;
	VISITABLE(float, Co2);

	// @In
	// @Description mean air temperature
    //float *TMEAN;
	VISITABLE(float *, TMEAN);

	// @In
	// @Description min air temperature
    //float *TMIN;
	VISITABLE(float *, TMIN);

	// @In
	// @Description solar radiation
    //float *SR;
	VISITABLE(float *, SR);

	// @In
	// @Description average annual air temperature
    //float *TMEAN0;
	VISITABLE(float *, TMEAN0);

	// @In
	// @Description minimum day length
    //float *daylenmin;
	VISITABLE(float *, daylenmin);

	// @In
	// @Description dormancy threshold
    //float *dormhr;
	VISITABLE(float *, dormhr);

    /**  soil properties  **/

	// @In
	// @Description soil layers
    //float *soillayers;
	VISITABLE(float *, soillayers);

	// @In
	// @Description maximum soil layers
    //int soilLayers;
	VISITABLE(int, soilLayers);

	// @In
	// @Description maximum root depth
    //float *SOL_ZMX;
	VISITABLE(float *, SOL_ZMX);

	// @In
	// @Description albedo when soil is moist
    //float *sol_alb;
	VISITABLE(float *, sol_alb);

	// @In
	// @Description soil depth of all layers
    //float **soilDepth;
	VISITABLE(float **, soilDepth);

	// @In
	// @Description soil thickness of all layers
    //float **soilthick;
	VISITABLE(float **, soilthick);

	// @In
	// @Description amount of water available to plants in soil layer at field capacity (fc - wp water), sol_fc in SWAT
    //float **sol_awc;
	VISITABLE(float **, sol_awc);

	// @In
	// @Description total sol_awc in soil profile, sol_sumfc in SWAT
    //float *sol_sumAWC;
	VISITABLE(float *, sol_sumAWC);

	// @In
	// @Description amount of water held in soil profile at saturation, sol_sumul in SWAT
    //float *sol_sumul;
	VISITABLE(float *, sol_sumul);

	// @In
	// @Description amount of water stored in soil layers on current day, sol_st in SWAT
    //float **solst;
	VISITABLE(float **, solst);

	// @In
	// @Description amount of water stored in soil profile on current day, sol_sw in SWAT
    //float *solsw;
	VISITABLE(float *, solsw);

    /**  crop or land cover related parameters  **/

	// @In
	// @Description amount of residue on soil surface (kg/ha)
    //float *rsdin;
	VISITABLE(float *, rsdin);

	// @In
	// @Description amount of residue on soil surface (10 mm surface)
    //float *sol_cov;
	VISITABLE(float *, sol_cov);

	// @In
	// @Description amount of organic matter in the soil layer classified as residue, sol_rsd |kg/ha in SWAT
    //float **sol_rsd;
	VISITABLE(float **, sol_rsd);

	// @In
	// @Description land cover status code, 0 means no crop while 1 means land cover growing
    //float *IGRO;
	VISITABLE(float *, IGRO);

	// @In
	// @Description land cover/crop  classification:1-7, i.e., IDC
    //float *IDC;
	VISITABLE(float *, IDC);

	// @In
	// @Description minimum LAI during winter dormant period, alai_min
    //float *alai_min;
	VISITABLE(float *, alai_min);

	// @In
	// @Description Radiation-use efficicency or biomass-energy ratio ((kg/ha)/(MJ/m**2)), BIO_E in SWAT
    //float *BIOE;
	VISITABLE(float *, BIOE);

	// @In
	// @Description Biomass-energy ratio corresponding to the 2nd point on the radiation use efficiency curve
    //float *BIOEHI;
	VISITABLE(float *, BIOEHI);

	// @In
	// @Description fraction of biomass that drops during dormancy (for tree only), bio_leaf
    //float *BIO_LEAF;
	VISITABLE(float *, BIO_LEAF);

	// @In
	// @Description maximum (potential) leaf area index (BLAI in cropLookup db)
    //float *BLAI;
	VISITABLE(float *, BLAI);

	// @In
	// @Description Maximum biomass for a forest (metric tons/ha), BMX_TREES in SWAT
    //float *BMX_TREES;
	VISITABLE(float *, BMX_TREES);

	// @In
	// @Description nitrogen uptake parameter #1: normal fraction of N in crop biomass at emergence
    //float *BN1;
	VISITABLE(float *, BN1);

	// @In
	// @Description nitrogen uptake parameter #2: normal fraction of N in crop biomass at 50% maturity
    //float *BN2;
	VISITABLE(float *, BN2);

	// @In
	// @Description nitrogen uptake parameter #3: normal fraction of N in crop biomass at maturity
    //float *BN3;
	VISITABLE(float *, BN3);

	END_VISITABLES;

	// @In
	// @Description phosphorus uptake parameter #1: normal fraction of P in crop biomass at emergence
    float *BP1;

	// @In
	// @Description phosphorus uptake parameter #2: normal fraction of P in crop biomass at 50% maturity
    float *BP2;

	// @In
	// @Description phosphorus uptake parameter #3: normal fraction of P in crop biomass at maturity
    float *BP3;

	// @In
	// @Description maximum canopy height (m)
    float *CHTMX;

	// @In
	// @Description elevated CO2 atmospheric concentration corresponding the 2nd point on the radiation use efficiency curve
    float *CO2HI;

	// @In
	// @Description fraction of growing season(PHU) when senescence becomes dominant
    float *DLAI;

	// @In
	// @Description plant water uptake compensation factor
    float *epco;

	// @In
	// @Description light extinction coefficient, ext_coef
    float *EXT_COEF;

	// @In
	// @Description fraction of the growing season corresponding to the 1st point on optimal leaf area development curve
    float *FRGRW1;

	// @In
	// @Description fraction of the growing season corresponding to the 2nd point on optimal leaf area development curve
    float *FRGRW2;

	// @In
	// @Description harvest index: crop yield/aboveground biomass (kg/ha)/(kg/ha)
    float *hvsti;

	// @In
	// @Description fraction of maximum leaf area index corresponding to the 1st point on optimal leaf area development curve
    float *LAIMX1;

	// @In
	// @Description fraction of maximum leaf area index corresponding to the 2nd point on optimal leaf area development curve
    float *LAIMX2;

	// @In
	// @Description the number of years for the tree species to reach full development (years), MAT_YRS in SWAT
    float *MAT_YRS;

	// @In
	// @Description minimum temperature for plant growth
    float *T_BASE;

	// @In
	// @Description optional temperature for plant growth
    float *T_OPT;

	// @In
	// @Description Rate of decline in radiation use efficiency per unit increase in vapor pressure deficit, wavp in SWAT
    float *WAVP;

    /**  parameters need to be initialized in this module if they are NULL, i.e., in initialOutputs(void)  **/

	// @In
	// @Description canopy height (m)
    float *CHT;

	// @In
	// @Description albedo in the current day
    float *ALBDAY;

	// @In
	// @Description initial age of trees (yrs) at the beginning of simulation
    float *CURYR_INIT;

	// @In
	// @Description initial  dry weight biomass
    float *BIO_INIT;

	// @In
	// @Description initial LAI
    float *LAI_INIT;

	// @In
	// @Description total heat units needed to bring plant to maturity
    float *PHU_PLT;

	// @In
	// @Description dominant code, 0 is land cover growing (not dormant), 1 is land cover dormant, by default dormi is 0.
    float *dormi;

	// @In
	// @Description actual ET simulated during life of plant, plt_et in SWAT
    float *plt_et_tot;

	// @In
	// @Description potential ET simulated during life of plant, pltPET in SWAT
    float *plt_pet_tot;

    /**  Nutrient related parameters  **/

	// @In
	// @Description Nitrogen uptake distribution parameter
    float n_updis;

	// @In
	// @Description Phosphorus uptake distribution parameter
    float p_updis;

	// @In
	// @Description Nitrogen fixation coefficient, FIXCO in SWAT
    float nfixco;

	// @In
	// @Description Maximum daily-n fixation (kg/ha), NFIXMX in SWAT
    float nfixmx;

    /**  input from other modules  **/

	// @In
	// @Description day length
    float *daylength;

	// @In
	// @Description vapor pressure deficit (kPa)
    float *VPD;

	// @In
	// @Description potential evapotranspiration, pet_day in SWAT
    float *PET;

	// @In
	// @Description maximum plant et (mm H2O), ep_max in SWAT
    float *PPT;

	// @In
	// @Description actual amount of evaporation (soil et), es_day in SWAT
    float *SOET;

	// @In
	// @Description amount of nitrogen stored in the nitrate pool
    float **sol_no3;

	// @In
	// @Description amount of phosphorus stored in solution
    float **sol_solp;

	// @In
	// @Description amount of water in snow on current day
    float *SNAC;

    /**  set output variables  **/

	// @Out
	// @Description the leaf area indices for day i
    float *LAIDAY;

	// @Out
	// @Description fraction of plant heat units (PHU) accumulated, also as output, phuacc in SWAT
    float *frPHUacc;

	// @Out
	// @Description maximum leaf area index for the current year (m_yearIdx), lai_yrmx in SWAT
    float *laiyrmax;

	// @Out
	// @Description harvest index adjusted for water stress, hvstiadj in SWAT
    float *hvsti_adj;

	// @Out
	// @Description DO NOT KNOW NOW, initialized as 0.
    float *laimaxfr;

	// @Out
	// @Description DO NOT KNOW NOW
    float *olai;

	// @Out
	// @Description last soil root depth for use in harvest-kill-op/kill-op,
    float *lastSoilRD;

	// @Out
	// @Description actual amount of transpiration (mm H2O), ep_day in SWAT
    float *AET_PLT;

	// @Out
	// @Description fraction of total plant biomass that is in roots, rwt in SWAT
    float *frRoot;

	// @Out
	// @Description amount of nitrogen in plant biomass (kg/ha), plantn in SWAT
    float *plant_N;

	// @Out
	// @Description amount of phosphorus in plant biomass (kg/ha), plantp in SWAT
    float *plant_P;

	// @Out
	// @Description fraction of plant biomass that is nitrogen, pltfr_n in SWAT
    float *frPlantN;

	// @Out
	// @Description fraction of plant biomass that is phosphorus, pltfr_p in SWAT
    float *frPlantP;	

	// @Out
	// @Description fraction of potential plant growth achieved where the reduction is caused by water stress, strsw in SWAT
    float *frStrsWtr;	

	// @Out
	// @Description land cover/crop biomass (dry weight), bio_ms in SWAT
    float *BIOMASS;

private:
	/**  intermediate variables  **/

	// @In
	// @Description current rooting depth
	float *m_soilRD;

	// @In
	// @Description water uptake distribution parameter, NOT ALLOWED TO MODIFIED BY USERS
	float ubw;

	// @In
	// @Description water uptake normalization parameter, NOT ALLOWED TO MODIFIED BY USERS
	float uobw;

	// @Out
	// @Description amount of nitrogen added to soil via fixation on the day
	float *m_fixN;

	// @Out
	// @Description plant uptake of nitrogen, nplnt in SWAT
	float *m_plantUpTkN;

	// @Out
	// @Description plant uptake of phosphorus, pplnt in SWAT
	float *m_plantUpTkP;

	// @Out
	// @Description plant nitrogen deficiency (kg/ha), uno3d in SWAT
	float *m_NO3Defic;

	// @Out
	// @Description soil aeration stress
	float *m_frStrsAe;

	// @Out
	// @Description fraction of potential plant growth achieved where the reduction is caused by nitrogen stress, strsn in SWAT
	float *m_frStrsN;

	// @Out
	// @Description fraction of potential plant growth achieved where the reduction is caused by phosphorus stress, strsp in SWAT
	float *m_frStrsP;

	// @Out
	// @Description fraction of potential plant growth achieved where the reduction is caused by temperature stress, strstmp in SWAT
	float *m_frStrsTmp;

	// @Out
	// @Description biomass generated on current day, bioday in SWAT
	float *m_biomassDelta;

	


    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from swu.f of SWAT rev. 637
    //  Distribute potential plant evaporation through
    //	the root zone and calculates actual plant water use based on soil
    //	water availability. Also estimates water stress factor.
    //////////////////////////////////////////////////////////////////////////
    void DistributePlantET(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from grow.f of SWAT rev. 637
    //  Adjust plant biomass, leaf area index, and canopy height
    //	taking into account the effect of water, temperature and nutrient stresses  on the plant
    //////////////////////////////////////////////////////////////////////////
    void AdjustPlantGrowth(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from tstr.f of SWAT rev. 637
    //  Compute temperature stress for crop growth - strstmp
    //////////////////////////////////////////////////////////////////////////
    void CalTempStress(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from nup.f of SWAT rev. 637
    //  Calculates plant nitrogen uptake
    //////////////////////////////////////////////////////////////////////////
    void PlantNitrogenUptake(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from nfix.f of SWAT rev. 637
    //  Estimate nitrogen fixation by legumes
    //  wshd_fixn is NOT INCLUDED, average annual amount of nitrogen added to plant biomass via fixation
    //////////////////////////////////////////////////////////////////////////
    void PlantNitrogenFixed(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from npup.f of SWAT rev. 637
    //  Calculates plant phosphorus uptake
    //////////////////////////////////////////////////////////////////////////
    void PlantPhosphorusUptake(int i);

    //////////////////////////////////////////////////////////////////////////
    //  The following code is transferred from dormant.f of SWAT rev. 637
    //  Check the dormant status of the different plant types
    //////////////////////////////////////////////////////////////////////////
    void CheckDormantStatus(int i);

    /// initialize output variables
    void initialOutputs(void);
};



VISITABLE_STRUCT(Biomass_EPIC, m_soilRD, ubw, uobw, BP1, BP2, BP3, CHTMX, CO2HI, DLAI, epco, EXT_COEF, FRGRW1, FRGRW2, hvsti,
	LAIMX1, LAIMX2, MAT_YRS, T_BASE, T_OPT, WAVP, CHT, ALBDAY, CURYR_INIT, BIO_INIT, LAI_INIT, PHU_PLT, dormi, plt_et_tot,
	plt_pet_tot, n_updis, p_updis, nfixco, nfixmx, daylength, VPD, PET, PPT, SOET, sol_no3, sol_solp, SNAC, LAIDAY, frPHUacc,
	laiyrmax, hvsti_adj, laimaxfr, olai, lastSoilRD, AET_PLT, frRoot, plant_N, plant_P, frPlantN, frPlantP, frStrsWtr, BIOMASS,
	m_fixN, m_plantUpTkN, m_plantUpTkP, m_NO3Defic, m_frStrsAe, m_frStrsN, m_frStrsP, m_frStrsTmp, m_biomassDelta);



/// the following two variables can be temporary variables to save memory.
///// 1st shape parameter for leaf area development equation
//float* m_LAIShpCoef1;
///// 2nd shape parameter for leaf area development equation
//float* m_LAIShpCoef2;
///// 1st shape parameter for radiation use efficiency equation, wac21 in SWAT
//float* m_RadUseEffiShpCoef1;
///// 2nd shape parameter for radiation use efficiency equation, wac22 in SWAT
//float* m_RadUseEffiShpCoef2;

/// currently, the following two variables are assigned the default value.
///// initial root to shoot ratio at beg of growing season, rsr1c in SWAT
//float* m_rootShootRatio1;
///// root to shoot ratio at end of growing season, rsr2c in SWAT
//float* m_rootShootRatio2;