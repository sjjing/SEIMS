/*!
 * \brief Sediment routing using simplified version of Bagnold(1997) stream power equation
 *        which is also the original SWAT method.
 *        This module routes sediment from subbasin to basin outlet deposition is based on fall velocity and degradation on stream
 *        reWrite from route.f and rtsed.f of SWAT
 * \author Hui Wu
 * \date Jul. 2012
 * \revised LiangJun Zhu
 * \date May. 2016
 * \description: 1. move m_erodibilityFactor, m_coverFactor, to reach collection of MongoDB as inputs, and is DT_Array1D
 *               2. add point source loadings from Scenario database
 *               3. add SEDRECHConc output with the unit g/cm3 (i.e., Mg/m3)
 * \revised Junzhi Liu
 * \date August. 2016
 * \revised LiangJun Zhu
 * \date Sep. 2016
 * \description: 1. ReCheck and Update code according to route.f and rtsed.f
 *               2. Change the module name from SEDR_VCD to SEDR_SBAGNOLD
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup SEDR_SBAGNOLD
 * \ingroup Erosion
 * \brief Sediment routing using Simplified version of Bagnold(1997) stream power equation
 */
/*!
 * \class SEDR_SBAGNOLD
 * \ingroup SEDR_SBAGNOLD
 *
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 *
 */
class SEDR_SBAGNOLD : public SimulationModule {
public:
    //! Constructor
    SEDR_SBAGNOLD(void);

    //! Destructor
    ~SEDR_SBAGNOLD(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void GetValue(const char *key, float *value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    //virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    virtual void SetReaches(clsReaches *reaches);

    virtual void SetScenario(Scenario *sce);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputSizeChannel(const char *key, int n);

    bool CheckInputData(void);

    virtual TimeStepType GetTimeStepType(void) { return TIMESTEP_CHANNEL; };

	/// upstream id (The value is -1 if there if no upstream reach)
	vector <vector<int>> m_reachUpStream;

	// id the reaches
	vector<int> m_reachId;

	/* point source operations
	* key: unique index, BMPID * 100000 + subScenarioID
	* value: point source management factory instance
	*/
	map<int, BMPPointSrcFactory *> m_ptSrcFactory;

	map<int, vector<int> > m_reachLayers;

	// @In
	// @Description time step (sec)
    int TIMESTEP;

	// @In
	// @Description reach number (= subbasin number)
    int nreach;

	// @In
	// @Description whether change channel dimensions, 0 - do not change, 1 - compute channel degredation
    int vcd;

	// @In
	// @Description the peak rate adjustment factor
    float p_rf;

	// @In
	// @Description Coefficient in sediment transport equation
    float spcon;

	// @In
	// @Description Exponent in sediment transport equation
    float spexp;

	// @In
	// @Description critical velocity for sediment deposition
    float vcrit;
   
	// @In
	// @Description sediment from subbasin (hillslope), kg
    float *SEDTOCH;
    
	// @In
	// @Description initial channel storage per meter of reach length (m^3/m)
    float Chs0;

	// @In
	// @Description Initial channel sediment concentration, ton/m^3, i.e., kg/L
    float sed_chi;

	// @In
	// @Description channel outflow, m^3/s
    float *QRECH;

	// @In
	// @Description Order of reach
    float *chOrder;

	// @In
	// @Description Width of reach
    float *chWidth;

	// @In
	// @Description Depth of reach
    float *chDepth;

	// @In
	// @Description length of reach (m)
    float *chLen;

	// @In
	// @Description Vel of reach
    float *chVel;

	// @In
	// @Description Slope of reach
    float *chSlope;

	// @In
	// @Description Manning of reach
    float *chManning;

	// @In
	// @Description Cover of reach
    float *chCover;

	// @In
	// @Description Erod of reach
    float *chErod;

	// @In
	// @Description downstream id (The value is 0 if there if no downstream reach)
    float *reachDownStream;
    
	// @In
	// @Description The point source loading (kg), ptSub[id], id is the reach id, load from m_Scenario
    float *ptSub;

	// @In
	// @Description reach storage (m^3) at time t
    float *CHST;

	// @In
	// @Description reach storage of previous timestep, m^3
    float *preCHST;

	// @In
	// @Description channel water depth, m
    float *CHWTDEPTH;

	// @In
	// @Description channel water depth of previous timestep, m
    float *prechwtdepth;

	// @In
	// @Description channel bankfull width, m
    float *chwtwidth;

    // OUTPUT

	// @Out
	// @Description initial reach sediment out (kg) at time t
    float *SEDRECH;

	// @Out
	// @Description channel sediment storage (kg)
    float *sedStorage;

	// @Out
	// @Description sediment of deposition
    float *sedDep;

	// @Out
	// @Description sediment of degradation
    float *sedDeg;

	// @Out
	// @Description sediment concentration (g/L, i.e., kg/m3)
    float *SEDRECHConc;

	// @Out
	// @Description sand concentration
    float *rchSand;

	// @Out
	// @Description Silt concentration
    float *rchSilt;

	// @Out
	// @Description Clay concentration
    float *rchClay;

	// @Out
	// @Description small agg concentration
    float *rchSag;

	// @Out
	// @Description large agg concentration
    float *rchLag;

	// @Out
	// @Description gra concentration
    float *rchGra;

	// @Out
	// @Description bank erosion
    float *rch_bank_ero;

	// @Out
	// @Description reach degradation
    float *rch_deg;

	// @Out
	// @Description reach deposition
    float *rch_dep;

	// @Out
	// @Description Floodplain Deposition
    float *flplain_dep;    

    void initialOutputs(void);

    void PointSourceLoading(void);

    void SedChannelRouting(int i);

    void doChannelDowncuttingAndWidening(int id);
};

VISITABLE_STRUCT(SEDR_SBAGNOLD, TIMESTEP, nreach, vcd, p_rf, spcon, spexp, vcrit, SEDTOCH, Chs0, sed_chi, QRECH, chOrder, chWidth, 
	chDepth, chLen, chVel, chSlope, chManning, chCover, chErod, reachDownStream, ptSub, CHST, preCHST, CHWTDEPTH, prechwtdepth, 
	chwtwidth, SEDRECH, sedStorage, sedDep, sedDeg, SEDRECHConc, rchSand, rchSilt, rchClay, rchSag, rchLag, rchGra, rch_bank_ero, 
	rch_deg, rch_dep, flplain_dep);

///// inverse of flood plain side slope of channel, is a fixed number:  1/slope
//float m_sideslopeFloodplain;
///// inverse of side slope of main channel, is a fixed number:  1/slope
//float m_sideslopeMain;
/// reach storage (m3) at time t
//float* CHST;
/// Channel sediment balance in a text format for each reach and at each time step
//float** m_T_CHSB;
/// cross-sectional area of flow in the channel (m^2)
//float *m_CrAreaCh;
