#include "seims.h"
#include "MUSK_CH.h"

using namespace std;

MUSK_CH::MUSK_CH(void) : DT_CH(-1), nreach(-1), outletID(-1), K_chb(NODATA_VALUE),
                         K_bank(NODATA_VALUE), Ep_ch(NODATA_VALUE), Bnk0(NODATA_VALUE), chs0_perc(NODATA_VALUE),
                         a_bnk(NODATA_VALUE), chSideSlope(NULL),
                         b_bnk(NODATA_VALUE), subbasin(NULL), SBOF(NULL),
                         reachDownStream(NULL), chOrder(NULL), chWidth(NULL),
                         chLen(NULL), chDepth(NULL), chVel(NULL), area(NULL),
                         ptSub(NULL), SBIF(NULL), SBQG(NULL), SBPET(NULL), SBGS(NULL), Vseep0(0.f),
                         BKST(NULL), SEEPAGE(NULL),
                         QS(NULL), QI(NULL), QG(NULL),
                         MSK_X(NODATA_VALUE), MSK_co1(NODATA_VALUE), qIn(NULL), CHST(NULL), preCHST(NULL),
                         VelScaleFactor(1.0f),
                         QUPREACH(0.f), GWRQ(0.f), CHWTDEPTH(NULL), prechwtdepth(NULL),
                         chwtwidth(NULL), chbtmwidth(NULL) {
}

MUSK_CH::~MUSK_CH(void) {
    Release1DArray(reachDownStream);
    Release1DArray(chOrder);
    Release1DArray(chWidth);
    Release1DArray(chLen);
    Release1DArray(chDepth);
    Release1DArray(chVel);
    Release1DArray(area);
    Release1DArray(CHST);
    Release1DArray(preCHST);
    Release1DArray(QRECH);
    Release1DArray(BKST);
    Release1DArray(SEEPAGE);
    Release1DArray(QS);
    Release1DArray(QI);
    Release1DArray(QG);
    Release1DArray(CHWTDEPTH);
    Release1DArray(prechwtdepth);
    Release1DArray(chwtwidth);
    Release1DArray(chbtmwidth);
    Release1DArray(ptSub);
    // m_ptSrcFactory will be released by DataCenter->Scenario. lj
}

//! Check input data
bool MUSK_CH::CheckInputData(void) {
    if (DT_CH < 0) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }
    if (nreach < 0) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: nreach has not been set.");
    }
    if (FloatEqual(MSK_X, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: MSK_X has not been set.");
    }
    if (FloatEqual(MSK_co1, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: MSK_co1 has not been set.");
    }
    if (FloatEqual(K_chb, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: K_chb has not been set.");
    }
    if (FloatEqual(K_bank, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: K_bank has not been set.");
    }
    if (FloatEqual(Ep_ch, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: Ep_ch has not been set.");
    }
    if (FloatEqual(Bnk0, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: Bnk0 has not been set.");
    }
    if (FloatEqual(chs0_perc, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: Chs0 has not been set.");
    }
    if (FloatEqual(a_bnk, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: A_bnk has not been set.");
    }
    if (FloatEqual(b_bnk, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: B_bnk has not been set.");
    }
    if (FloatEqual(Vseep0, NODATA_VALUE)) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: Vseep0 has not been set.");
    }
    if (subbasin == NULL) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: subbasin has not been set.");
    }
    if (SBOF == NULL) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: Q_SBOF has not been set.");
    }
    //if (SBIF == NULL)
    //	throw ModelException(MID_MUSK_CH,"CheckInputData","The parameter: Q_SBIF has not been set.");
    //if (SBQG == NULL)
    //	throw ModelException(MID_MUSK_CH,"CheckInputData","The parameter: QG_sub has not been set.");
    //if (SBPET == NULL)
    //	throw ModelException(MID_MUSK_CH,"CheckInputData","The parameter: SBPET has not been set.");
    //if (SBGS == NULL)
    //{
    //	throw ModelException(MID_MUSK_CH,"CheckInputData","The parameter: GW_sub has not been set.");
    //}
    if (chWidth == NULL) {
        throw ModelException(MID_MUSK_CH, "CheckInputData", "The parameter: RchParam has not been set.");
    }
    return true;
}

//! Initial outputs
void MUSK_CH::initialOutputs() {
    if (nreach <= 0) {
        throw ModelException(MID_MUSK_CH, "initialOutputs", "The reach number can not be less than zero.");
    }

    if (m_reachLayers.empty()) {
        for (int i = 1; i <= nreach; i++) {
            if (chOrder == NULL) {
                throw ModelException(MID_MUSK_CH, "initialOutputs",
                                     "Stream order is not loaded successful from Reach table.");
            }
            int order = (int) chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }
    if (outletID < 0) {
        outletID = m_reachLayers.rbegin()->second[0];
    }
    //initial channel storage
    if (CHST == NULL) {
        CHST = new float[nreach + 1];
        preCHST = new float[nreach + 1];
        qIn = new float[nreach + 1];
        QRECH = new float[nreach + 1];
        BKST = new float[nreach + 1];
        SEEPAGE = new float[nreach + 1];
        QS = new float[nreach + 1];
        QI = new float[nreach + 1];
        QG = new float[nreach + 1];
        CHWTDEPTH = new float[nreach + 1];
        prechwtdepth = new float[nreach + 1];
        chwtwidth = new float[nreach + 1];
        chbtmwidth = new float[nreach + 1];

        for (int i = 1; i <= nreach; i++) {
            float qiSub = 0.f;
            float qgSub = 0.f;
            if (SBIF != NULL) { // interflow (subsurface flow)
                qiSub = SBIF[i];
            }
            if (SBQG != NULL) { // groundwater
                qgSub = SBQG[i];
            }
            SEEPAGE[i] = 0.f;
            BKST[i] = Bnk0 * chLen[i];
            chbtmwidth[i] = chWidth[i] - 2.f * chSideSlope[i] * chDepth[i];
            if (chbtmwidth[i] <= UTIL_ZERO) {
                chbtmwidth[i] = 0.5f * chWidth[i];
                chSideSlope[i] = (chWidth[i] - chbtmwidth[i]) / 2.f / chDepth[i];
            }
            CHWTDEPTH[i] = chDepth[i] * chs0_perc;
            chwtwidth[i] = chbtmwidth[i] + 2.f * chSideSlope[i] * CHWTDEPTH[i];
            prechwtdepth[i] = chwtwidth[i];
            // chwtwidth[i] = chWidth[i];
            // CHST[i] = CHWTDEPTH[i] * chwtwidth[i] * chLen[i];
            CHST[i] = chLen[i] * CHWTDEPTH[i] * (chbtmwidth[i] + chSideSlope[i] * CHWTDEPTH[i]);
            preCHST[i] = CHST[i];
            qIn[i] = 0.f;
            QRECH[i] = SBOF[i] + qiSub + qgSub;
            QS[i] = SBOF[i];
            QI[i] = qiSub;
            QG[i] = qgSub;
        }
    }
    /// initialize point source loadings
    if (ptSub == NULL) {
        Initialize1DArray(nreach + 1, ptSub, 0.f);
    }
}

void MUSK_CH::PointSourceLoading() {
    /// load point source water discharge (m3/s) on current day from Scenario
    for (map<int, BMPPointSrcFactory *>::iterator it = m_ptSrcFactory.begin(); it != m_ptSrcFactory.end(); it++) {
        /// reset point source loading water to 0.f
        for (int i = 0; i <= nreach; i++) {
            ptSub[i] = 0.f;
        }
        //cout<<"unique Point Source Factory ID: "<<it->first<<endl;
        vector<int> m_ptSrcMgtSeqs = it->second->GetPointSrcMgtSeqs();
        map < int, PointSourceMgtParams * > m_pointSrcMgtMap = it->second->GetPointSrcMgtMap();
        vector<int> m_ptSrcIDs = it->second->GetPointSrcIDs();
        map < int, PointSourceLocations * > m_pointSrcLocsMap = it->second->GetPointSrcLocsMap();
        // 1. looking for management operations from m_pointSrcMgtMap
        for (vector<int>::iterator seqIter = m_ptSrcMgtSeqs.begin(); seqIter != m_ptSrcMgtSeqs.end(); seqIter++) {
            PointSourceMgtParams *curPtMgt = m_pointSrcMgtMap.at(*seqIter);
            // 1.1 If current day is beyond the date range, then continue to next management
            if (curPtMgt->GetStartDate() != 0 && curPtMgt->GetEndDate() != 0) {
                if (m_date < curPtMgt->GetStartDate() || m_date > curPtMgt->GetEndDate()) {
                    continue;
                }
            }
            // 1.2 Otherwise, get the water volume
            float per_wtrVol = curPtMgt->GetWaterVolume(); /// m3/'size'/day
            // 1.3 Sum up all point sources
            for (vector<int>::iterator locIter = m_ptSrcIDs.begin(); locIter != m_ptSrcIDs.end(); locIter++) {
                if (m_pointSrcLocsMap.find(*locIter) != m_pointSrcLocsMap.end()) {
                    PointSourceLocations *curPtLoc = m_pointSrcLocsMap.at(*locIter);
                    int curSubID = curPtLoc->GetSubbasinID();
                    ptSub[curSubID] += per_wtrVol * curPtLoc->GetSize() / 86400.f; /// m3/'size'/day ==> m3/s
                }
            }
        }
    }
}

//! Execute function
int MUSK_CH::Execute() {
    CheckInputData();
    initialOutputs();
    /// load point source water volume from m_ptSrcFactory
    PointSourceLoading();
    map < int, vector < int > > ::iterator
    it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int reachNum = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < reachNum; ++i) {
            int reachIndex = it->second[i]; // index in the array
            ChannelFlow(reachIndex);
        }
    }
    return 0;
}

//! Check input size
bool MUSK_CH::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_MUSK_CH, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
#ifdef STORM_MODE
    if(nreach != n-1)
    {
        if(nreach <=0)
            nreach = n-1;
        else
        {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << nreach << ".\n";
            throw ModelException(MID_MUSK_CH,"CheckInputSize",oss.str());
        }
    }
#else
    if (nreach != n - 1) {
        if (nreach <= 0) {
            nreach = n - 1;
        } else {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
                nreach << ".\n";
            throw ModelException(MID_MUSK_CH, "CheckInputSize", oss.str());
        }
    }
#endif /* STORM_MODE */
    return true;
}

//! Set value of the module
void MUSK_CH::SetValue(const char *key, float value) {
    string sk(key);

    if (StringMatch(sk, VAR_QUPREACH)) { QUPREACH = value; }
    else if (StringMatch(sk, VAR_VSF)) { VelScaleFactor = value; }
    else if (StringMatch(sk, Tag_ChannelTimeStep)) { DT_CH = (int) value; }
    else if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, VAR_K_CHB)) { K_chb = value; }
    else if (StringMatch(sk, VAR_K_BANK)) { K_bank = value; }
    else if (StringMatch(sk, VAR_EP_CH)) { Ep_ch = value; }
    else if (StringMatch(sk, VAR_BNK0)) { Bnk0 = value; }
    else if (StringMatch(sk, VAR_CHS0_PERC)) { chs0_perc = value; }
    else if (StringMatch(sk, VAR_VSEEP0)) { Vseep0 = value; }
    else if (StringMatch(sk, VAR_A_BNK)) { a_bnk = value; }
    else if (StringMatch(sk, VAR_B_BNK)) { b_bnk = value; }
    else if (StringMatch(sk, VAR_MSK_X)) { MSK_X = value; }
    else if (StringMatch(sk, VAR_MSK_CO1)) { MSK_co1 = value; }
    else if (StringMatch(sk, VAR_GWRQ)) { GWRQ = value; }
    else {
        throw ModelException(MID_MUSK_CH, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

//! Set 1D data
void MUSK_CH::Set1DData(const char *key, int n, float *value) {
    string sk(key);
    //check the input data
    if (StringMatch(sk, VAR_SUBBSN)) { subbasin = value; }
    else if (StringMatch(sk, VAR_SBOF)) {
        CheckInputSize(key, n);
        SBOF = value;
    } else if (StringMatch(sk, VAR_SBIF)) {
        CheckInputSize(key, n);
        SBIF = value;
    } else if (StringMatch(sk, VAR_SBQG)) {
        SBQG = value;
    } else if (StringMatch(sk, VAR_SBPET)) {
        SBPET = value;
    } else if (StringMatch(sk, VAR_SBGS)) {
        SBGS = value;
    } else {
        throw ModelException(MID_MUSK_CH, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

//! Get value
void MUSK_CH::GetValue(const char *key, float *value) {
    string sk(key);
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_QOUTLET)) {
        QRECH[0] = QRECH[iOutlet];
        *value = QRECH[0];
    } else if (StringMatch(sk, VAR_QSOUTLET)) {
        *value = QS[iOutlet];
    }
}

//! Get 1D data
void MUSK_CH::Get1DData(const char *key, int *n, float **data) {
    if (m_reachLayers.empty()) {
        initialOutputs();
    }
    string sk(key);
    *n = nreach + 1;
    //int outletID = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_QRECH)) {
        QRECH[0] = QRECH[outletID];
        *data = QRECH;
    } else if (StringMatch(sk, VAR_QS)) {
        QS[0] = QS[outletID];
        *data = QS;
    } else if (StringMatch(sk, VAR_QI)) {
        QI[0] = QI[outletID];
        *data = QI;
    } else if (StringMatch(sk, VAR_QG)) {
        QG[0] = QG[outletID];
        *data = QG;
    } else if (StringMatch(sk, VAR_BKST)) {
        BKST[0] = BKST[outletID];
        *data = BKST;
    } else if (StringMatch(sk, VAR_CHST)) {
        CHST[0] = CHST[outletID];
        *data = CHST;
    } else if (StringMatch(sk, VAR_PRECHST)) {
        preCHST[0] = preCHST[outletID];
        *data = preCHST;
    } else if (StringMatch(sk, VAR_SEEPAGE)) {
        SEEPAGE[0] = SEEPAGE[outletID];
        *data = SEEPAGE;
    } else if (StringMatch(sk, VAR_CHWTDEPTH)) {
        CHWTDEPTH[0] = CHWTDEPTH[outletID];
        *data = CHWTDEPTH;
    } else if (StringMatch(sk, VAR_PRECHWTDEPTH)) {
        prechwtdepth[0] = prechwtdepth[outletID];
        *data = prechwtdepth;
    } else if (StringMatch(sk, VAR_CHWTWIDTH)) {
        chwtwidth[0] = chwtwidth[outletID];
        *data = chwtwidth;
    } else if (StringMatch(sk, VAR_CHBTMWIDTH)) {
        chbtmwidth[0] = chbtmwidth[outletID];
        *data = chbtmwidth;
    } else {
        throw ModelException(MID_MUSK_CH, "Get1DData", "Output " + sk + " does not exist.");
    }
}

//! Get 2D data
void MUSK_CH::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    string sk(key);
    throw ModelException(MID_MUSK_CH, "Get2DData", "Parameter " + sk + " does not exist.");
}

////! Set 2D data
//void MUSK_CH::Set2DData(const char *key, int nrows, int ncols, float **data)
//{
//    string sk(key);
//    if (StringMatch(sk, Tag_RchParam))
//    {
//        nreach = ncols - 1;
//
//        m_reachId = data[0];
//        reachDownStream = data[1];
//        chOrder = data[2];
//        chWidth = data[3];
//        chLen = data[4];
//        chDepth = data[5];
//        chVel = data[6];
//        area = data[7];
//
//        m_reachUpStream.resize(nreach + 1);
//        if (nreach > 1)
//        {
//            for (int i = 1; i <= nreach; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
//            {
//                int downStreamId = int(reachDownStream[i]);
//                if (downStreamId <= 0)
//                    continue;
//                m_reachUpStream[downStreamId].push_back(i);
//            }
//        }
//    }
//    else
//        throw ModelException(MID_MUSK_CH, "Set2DData", "Parameter " + sk + " does not exist.");
//}
void MUSK_CH::SetScenario(Scenario *sce) {
    if (sce != NULL) {
        map<int, BMPFactory *> tmpBMPFactories = sce->GetBMPFactories();
        for (map<int, BMPFactory *>::iterator it = tmpBMPFactories.begin(); it != tmpBMPFactories.end(); it++) {
            /// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
            if (it->first / 100000 == BMP_TYPE_POINTSOURCE) {
                m_ptSrcFactory[it->first] = (BMPPointSrcFactory *) it->second;
            }
        }
    } else {
        throw ModelException(MID_MUSK_CH, "SetScenario", "The scenario can not to be NULL.");
    }
}

void MUSK_CH::SetReaches(clsReaches *reaches) {
    if (reaches != NULL) {
        nreach = reaches->GetReachNumber();
        m_reachId = reaches->GetReachIDs();
        if (reachDownStream == NULL) {
            Initialize1DArray(nreach + 1, reachDownStream, 0.f);
            Initialize1DArray(nreach + 1, chOrder, 0.f);
            Initialize1DArray(nreach + 1, chWidth, 0.f);
            Initialize1DArray(nreach + 1, chLen, 0.f);
            Initialize1DArray(nreach + 1, chDepth, 0.f);
            Initialize1DArray(nreach + 1, chVel, 0.f);
            Initialize1DArray(nreach + 1, area, 0.f);
            Initialize1DArray(nreach + 1, chSideSlope, 2.f);
        }
        for (vector<int>::iterator it = m_reachId.begin(); it != m_reachId.end(); it++) {
            int i = *it;
            clsReach *tmpReach = reaches->GetReachByID(i);
            reachDownStream[i] = (float) tmpReach->GetDownStream();
            chOrder[i] = (float) tmpReach->GetUpDownOrder();
            chWidth[i] = (float) tmpReach->GetWidth();
            chLen[i] = (float) tmpReach->GetLength();
            chDepth[i] = (float) tmpReach->GetDepth();
            chVel[i] = (float) tmpReach->GetV0();
            area[i] = (float) tmpReach->GetArea();
            chSideSlope[i] = (float) tmpReach->GetSideSlope();
        }

        m_reachUpStream.resize(nreach + 1);
        if (nreach > 1) {
            for (int i = 1; i <= nreach; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
            {
                int downStreamId = int(reachDownStream[i]);
                if (downStreamId <= 0) {
                    continue;
                }
                m_reachUpStream[downStreamId].push_back(i);
            }
        }
    } else {
        throw ModelException(MID_MUSK_CH, "SetReaches", "The reaches input can not to be NULL.");
    }
}

//! Get date time
void MUSK_CH::GetDt(float timeStep, float fmin, float fmax, float &dt, int &n) {
    if (fmax >= timeStep) {
        dt = timeStep;
        n = 1;
        return;
    }

    n = int(timeStep / fmax);
    dt = timeStep / n;

    if (dt > fmax) {
        n++;
        dt = timeStep / n;
    }
}

//! Get coefficients
void MUSK_CH::GetCoefficients(float reachLength, float v0, MuskWeights &weights) {
    v0 = VelScaleFactor * v0;
    float K = (4.64f - 3.64f * MSK_co1) * reachLength / (5.f * v0 / 3.f);

    float min = 2.f * K * MSK_X;
    float max = 2.f * K * (1.f - MSK_X);
    float dt;
    int n;
    GetDt((float) DT_CH, min, max, dt, n);
    weights.dt = dt;

    //get coefficient
    float temp = max + dt;
    weights.c1 = (dt - min) / temp;
    weights.c2 = (dt + min) / temp;
    weights.c3 = (max - dt) / temp;
    weights.c4 = 2 * dt / temp;
    weights.n = n;

    //make sure any coefficient is positive
    if (weights.c1 < 0) {
        weights.c2 += weights.c1;
        weights.c1 = 0.f;
    }
    if (weights.c3 < 0) {
        weights.c2 += weights.c1;
        weights.c3 = 0.f;
    }
}

void MUSK_CH::updateWaterWidthDepth(int i) {
    /// update channel water depth and width according to channel water storage
    float crossArea = CHST[i] / chLen[i];
    CHWTDEPTH[i] =
        (sqrt(chbtmwidth[i] * chbtmwidth[i] + 4.f * chSideSlope[i] * crossArea) - chbtmwidth[i]) / 2.f /
            chSideSlope[i];
    if (CHWTDEPTH[i] < UTIL_ZERO) {
        chwtwidth[i] = chbtmwidth[i];
    } else {
        chwtwidth[i] = chbtmwidth[i] + 2.f * chSideSlope[i] * CHWTDEPTH[i];
    }
}

//! Channel flow
void MUSK_CH::ChannelFlow(int i) {
    float st0 = CHST[i];
    float qiSub = 0.f; /// interflow flow
    if (SBIF != NULL && SBIF[i] >= 0.f) {
        qiSub = SBIF[i];
    }
    float qgSub = 0.f; /// groundwater flow
    if (SBQG != NULL && SBQG[i] >= 0.f) {
        qgSub = SBQG[i];
    }
    float ptSub1 = 0.f; /// point sources flow
    if (ptSub != NULL && ptSub[i] >= 0.f) {
        ptSub1 = ptSub[i];
    }
    //////////////////////////////////////////////////////////////////////////
    // first add all the inflow water
    // 1. water from this subbasin
    float qIn1 = SBOF[i] + qiSub + qgSub + ptSub1 + GWRQ;  /// m^3
    //if (i == outletID) /// this should be added to each channel. By lj
    //	qIn += GWRQ;
    // 2. water from upstream reaches
    float qsUp = 0.f;
    float qiUp = 0.f;
    float qgUp = 0.f;
    for (size_t j = 0; j < m_reachUpStream[i].size(); ++j) {
        int upReachId = m_reachUpStream[i][j];
        qsUp += QS[upReachId];
        qiUp += QI[upReachId];
        qgUp += QG[upReachId];
    }
    qIn1 += qsUp + qiUp + qgUp;
    //qIn is equivalent to the wtrin variable in rtmusk.f of SWAT
    qIn1 += QUPREACH; 
    // QUPREACH is zero for not-parallel program and qsUp, qiUp and qgUp are zero for parallel computing
    //if(i == 12)
    //	cout <<"surfaceQ: "<< SBOF[i] << ", subsurfaceQ: " << qiSub << ", groundQ: " << qgSub << ", pointQ: " << ptSub <<
    //	", UPsurfaceQ: "<<qsUp<<", UPsubsurface: "<<qiUp<<", UPground: "<<qgUp<<", \n";
    // 3. water from bank storage
    float bankOut = BKST[i] * (1.f - exp(-a_bnk));

    BKST[i] -= bankOut;
    qIn1 += bankOut / DT_CH;

    // add inflow water to storage
    //if (i==12) cout<<"initial chStorage: "<<CHST[i]<<", ";
    CHST[i] += qIn1 * DT_CH;
    /// update channel water depth and width according to channel water storage
    updateWaterWidthDepth(i);
    //if (i==12) cout<<"added chStorage: "<<CHST[i]<<endl;
    //if(i == 2) cout <<"qIn:"<< qIn<<", chStorage: "<<CHST[i]<<endl;
    //////////////////////////////////////////////////////////////////////////
    // then subtract all the outflow water
    // 1. transmission losses to deep aquifer, which is lost from the system
    // the unit of kchb is mm/hr
    float seepage = K_chb / 1000.f / 3600.f * chbtmwidth[i] * chLen[i] * DT_CH;
    //if(i == 2) cout << "seepage: " << seepage << endl;
    if (qgSub < UTIL_ZERO) {
        if (CHST[i] > seepage) {
            SEEPAGE[i] = seepage;
            CHST[i] -= seepage;
        } else {
            SEEPAGE[i] = CHST[i];
            CHST[i] = 0.f;
        }
    } else {
        SEEPAGE[i] = 0.f;
    }

    // 2. calculate transmission losses to bank storage
    //float dch = CHST[i] / (chwtwidth[i] * chLen[i]);
    float dch = CHWTDEPTH[i];
    float bankLen = dch * sqrt(1.f + chSideSlope[i] * chSideSlope[i]);
    float bankInLoss = 2.f * K_bank / 1000.f / 3600.f * bankLen * chLen[i] * DT_CH;   // m^3
    bankInLoss = 0.f; //TODO
    if (CHST[i] > bankInLoss) {
        CHST[i] -= bankInLoss;
    } else {
        bankInLoss = CHST[i];
        CHST[i] = 0.f;
    }
    // water balance of the bank storage
    // loss the water from bank storage to the adjacent unsaturated zone and groundwater storage
    float bankOutGw = BKST[i] * (1.f - exp(-b_bnk));
    bankOutGw = 0.f; //TODO
    BKST[i] = BKST[i] + bankInLoss - bankOutGw;
    if (SBGS != NULL) {
        SBGS[i] += bankOutGw / area[i] * 1000.f;
    }   // updated groundwater storage

    // 3. evaporation losses
    float et = 0.f;
    if (SBPET != NULL) {
        et = Ep_ch * SBPET[i] / 1000.0f * chwtwidth[i] * chLen[i];    //m3
        if (CHST[i] > et) {
            CHST[i] -= et;
        } else {
            et = CHST[i];
            CHST[i] = 0.f;
        }
    }
    if (FloatEqual(CHST[i], 0.f)) {
        QRECH[i] = 0.f;
        QS[i] = 0.f;
        QI[i] = 0.f;
        QG[i] = 0.f;
        CHWTDEPTH[i] = 0.f;
        chwtwidth[i] = 0.f;
        return;
    }

    //if(i == 2) cout << "chStorage before routing " << CHST[i] << endl;
    //////////////////////////////////////////////////////////////////////////
    // routing, there are water in the channel after inflow and transmission loss
    float totalLoss = SEEPAGE[i] + bankInLoss + et;
    //if(i == 12) cout << ",  SBPET: " << SBPET[i] << ",  et: " << et << ",  CHST: " << CHST[i] << ", \n";
    //if (CHST[i] >= 0.f)
    //{
//         qIn -= totalLoss / TIMESTEP;// average loss rate during TIMESTEP
// 		if(qIn < 0.f) qIn = 0.f;

    preCHST[i] = CHST[i];
    prechwtdepth[i] = CHWTDEPTH[i];
    CHST[i] = st0;

    // calculate coefficients
    MuskWeights wt;
    GetCoefficients(chLen[i], chVel[i], wt);
    int n = wt.n;
    float q = 0.f;
    for (int j = 0; j < n; j++) {
        QRECH[i] = wt.c1 * qIn1 + wt.c2 * qIn[i] + wt.c3 * QRECH[i];
        qIn[i] = qIn1;
        float tmp = CHST[i] + (qIn1 - totalLoss / DT_CH - QRECH[i]) * wt.dt;
        if (tmp < 0.f) {
            QRECH[i] = CHST[i] / wt.dt + qIn1;
            CHST[i] = 0.f;
        } else {
            CHST[i] = tmp;
        }
        q += QRECH[i];
    }
    QRECH[i] = q / n;
    //}
    //else
    //{
    //    QRECH[i] = 0.f;
    //    CHST[i] = 0.f;
    //    qIn = 0.f;
    //}
    float qInSum = SBOF[i] + qiSub + qgSub + qsUp + qiUp + qgUp;
    QS[i] = QRECH[i] * (SBOF[i] + qsUp) / qInSum;
    QI[i] = QRECH[i] * (qiSub + qiUp) / qInSum;
    QG[i] = QRECH[i] * (qgSub + qgUp) / qInSum;

    // set variables for next time step
    qIn[i] = qIn1;
    //CHWTDEPTH[i] = CHST[i] / (chwtwidth[i] * chLen[i]);
    updateWaterWidthDepth(i);
}
