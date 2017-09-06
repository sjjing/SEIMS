/*!
 * \brief channel flow routing using Muskingum method
 * \author Junzhi Liu
 * \version 1.0
 * \date 26-Jul-2012
 * 
 * \revision Liangjun Zhu
 * \date 18-Sep-2016
 * \description: 1. Add point source loadings from Scenario.
 *               2. Assume the channels have a trapezoidal shape
 *               3. Add chbtmwidth as variable intermediate parameter
 *               4. Add chSideSlope (default is 2) as input parameter from MongoDB, which is the ratio of run to rise
 *               5. Add several variables to store values in previous time step, which will be use in QUAL2E etc.
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "Scenario.h"

using namespace std;
using namespace MainBMP;
/** \defgroup MUSK_CH
 * \ingroup Hydrology_longterm
 * \brief channel flow routing using Muskingum method
 */

/* 
 * \struct MuskWeights coefficients
 */
struct MuskWeights {
    float c1;
    float c2;
    float c3;
    float c4;
    float dt;
    int n;  ///< number of division of the origin time step
};

/*!
 * \class MUSK_CH
 * \ingroup MUSK_CH
 * \brief channel flow routing using Muskingum method
 *
 */
class MUSK_CH : public SimulationModule {
public:
    MUSK_CH(void);

    ~MUSK_CH(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void GetValue(const char *key, float *value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    //virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    virtual void SetScenario(Scenario *sce);

    virtual void SetReaches(clsReaches *reaches);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputSizeChannel(const char *key, int n);

    bool CheckInputData(void);

    virtual TimeStepType GetTimeStepType(void) { return TIMESTEP_CHANNEL; };

    // @In
    // @Description flow velocity scaling factor for calibration
    float VelScaleFactor;

    // @In
    // @Description time step (sec)
    int DT_CH;

    // @In
    // @Description hydraulic conductivity of the channel bed (mm/h)
    float K_chb;

    // @In
    // @Description hydraulic conductivity of the channel bank (mm/h)
    float K_bank;

    // @In
    // @Description reach evaporation adjustment factor;
    float Ep_ch;

    // @In
    // @Description initial bank storage per meter of reach length (m3/m)
    float Bnk0;  

    // @In
    // @Description initial percentage of channel volume
    float chs0_perc;

    // @In
    // @Description the initial volume of transmission loss to the deep aquifer over the time interval (m3/s)
    float Vseep0;

    // @In
    // @Description bank flow recession constant
    float a_bnk;

    // @In
    // @Description bank storage loss coefficient
    float b_bnk;

    // @In
    // @Description subbasin grid
    float *subbasin;
 
    // @In
    // @Description Average PET for each subbasin
    float *SBPET;

    // @In
    // @Description overland flow to streams from each subbasin (m3/s)
    float *SBOF;

    // @In
    // @Description interflow to streams from each subbasin (m3/s)
    float *SBIF;

    // @In
    // @Description groundwater flow out of the subbasin (m3/s)
    float *SBQG;

    // @In
    // @Description Groundwater storage (mm) of the subbasin
    float *SBGS;

    // @Out
    // @Description channel outflow
    float *QS;

    // @Out
    // @Description channel outflow
    float *QI;

    // @Out
    // @Description channel outflow
    float *QG;

    // @Out
    // @Description channel water width (m)
    float *chwtwidth;

    // @Out
    // @Description bottom width of channel (m)
    float *chbtmwidth;

    // @Out
    // @Description channel water depth (m)
    float *CHWTDEPTH;

    // @Out
    // @Description channel water depth of previous timestep (m)
    float *prechwtdepth;

    // @Out
    // @Description bank storage (m^3)
    float *BKST;

    // @In
    // @Description groundwater recharge to channel or perennial base flow, m^3/s
    float GWRQ;

    // @Out
    // @Description seepage to deep aquifer
    float *SEEPAGE;

    // @In
    // @Description for muskingum
    float MSK_X;

    // @In
    // @Description for muskingum
    float MSK_co1;

    // @In
    // @Description IS THIS USEFUL? BY LJ
    float QUPREACH;

    /// scenario data

    //temporary at routing time

    // @Out
    // @Description reach storage (m^3) at time, t
    float *CHST;

    // @Out
    // @Description reach storage (m^3) at previous time step, t-1
    float *preCHST;

    // @Out
    // @Description reach outflow (m3/s) at time, t
    float *QRECH;

    
private:

    /// upstream id (The value is -1 if there if no upstream reach)
    vector <vector<int>> m_reachUpStream;

    // the reaches id 
    vector<int> m_reachId;

    /* point source operations
    * key: unique index, BMPID * 100000 + subScenarioID
    * value: point source management factory instance
    */
    map<int, BMPPointSrcFactory *> m_ptSrcFactory;

    /*!
    * reach layers
    * key: stream order
    * value: reach ID
    */
    map<int, vector<int> > m_reachLayers;

    //input

    // reach number (= subbasin number)
    int nreach;

    // outlet ID
    int outletID;

    // The point source discharge (m3/s), ptSub[id], id is the reach id, load from m_Scenario
    float *ptSub;

    // inverse of the channel side slope, by default is 2.
    float *chSideSlope;

    // the subbasin area (m2)  //add to the reach parameters file
    float *area;

    // channel order
    float *chOrder;

    // channel width (m)
    float *chWidth;

    // channel depth (m)
    float *chDepth;

    // channel length (m)
    float *chLen;

    // channel flow velocity (m/s)
    float *chVel;

    // downstream id (The value is 0 if there if no downstream reach)
    float *reachDownStream;

    //  flowin discharge at the last time step
    float *qIn;

    void initialOutputs(void);

    void PointSourceLoading(void);

    void ChannelFlow(int i);

    void GetDt(float timeStep, float fmin, float fmax, float &dt, int &n);

    void GetCoefficients(float reachLength, float v0, MuskWeights &weights);

    void updateWaterWidthDepth(int i);
};

VISITABLE_STRUCT(MUSK_CH, VelScaleFactor, DT_CH, K_chb, K_bank, Ep_ch, Bnk0, chSideSlope, chs0_perc, 
	Vseep0, a_bnk, b_bnk, subbasin, SBPET, SBOF, SBIF, SBQG, SBGS, QS, QI, QG, chwtwidth, chbtmwidth, 
	CHWTDEPTH, prechwtdepth, BKST, GWRQ, SEEPAGE, MSK_X, MSK_co1, QUPREACH, CHST, preCHST, 
	QRECH);