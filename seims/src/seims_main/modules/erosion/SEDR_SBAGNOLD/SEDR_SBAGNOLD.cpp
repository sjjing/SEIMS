#include "seims.h"
#include "SEDR_SBAGNOLD.h"

using namespace std;

SEDR_SBAGNOLD::SEDR_SBAGNOLD(void) : TIMESTEP(-1), nreach(-1), SEDTOCH(NULL), Chs0(NODATA_VALUE),
                                     sed_chi(NODATA_VALUE),
                                     ptSub(NULL), CHST(NULL), preCHST(NULL), SEDRECH(NULL), vcd(-1),
                                     reachDownStream(NULL), chOrder(NULL), chWidth(NULL),
                                     chLen(NULL), chDepth(NULL), chVel(NULL), chSlope(NULL), chCover(NULL), chErod(NULL),
                                     QRECH(NULL),
                                     p_rf(NODATA_VALUE), spcon(NODATA_VALUE), spexp(NODATA_VALUE),
                                     vcrit(NODATA_VALUE),
                                     sedStorage(NULL), sedDep(NULL), sedDeg(NULL), SEDRECHConc(NULL),
                                     rchSand(NULL), rchSilt(NULL), rchClay(NULL), rchSag(NULL), rchLag(NULL),
                                     rchGra(NULL),
                                     rch_bank_ero(NULL), rch_deg(NULL), rch_dep(NULL), flplain_dep(NULL) {
}

SEDR_SBAGNOLD::~SEDR_SBAGNOLD(void) {
    if (reachDownStream != NULL) Release1DArray(reachDownStream);
    if (chOrder != NULL) Release1DArray(chOrder);
    if (chWidth != NULL) Release1DArray(chWidth);
    if (chLen != NULL) Release1DArray(chLen);
    if (chDepth != NULL) Release1DArray(chDepth);
    if (chVel != NULL) Release1DArray(chVel);
    if (chCover != NULL) Release1DArray(chCover);
    if (chErod != NULL) Release1DArray(chErod);
    if (ptSub != NULL) Release1DArray(ptSub);
    if (SEDRECH != NULL) Release1DArray(SEDRECH);
    if (SEDRECHConc != NULL) Release1DArray(SEDRECHConc);
    if (sedStorage != NULL) Release1DArray(sedStorage);
    if (sedDeg != NULL) Release1DArray(sedDeg);
    if (sedDep != NULL) Release1DArray(sedDep);

    if (rchSand != NULL) Release1DArray(rchSand);
    if (rchSilt != NULL) Release1DArray(rchSilt);
    if (rchClay != NULL) Release1DArray(rchClay);
    if (rchSag != NULL) Release1DArray(rchSag);
    if (rchLag != NULL) Release1DArray(rchLag);
    if (rchGra != NULL) Release1DArray(rchGra);
    if (rch_bank_ero != NULL) Release1DArray(rch_bank_ero);
    if (rch_deg != NULL) Release1DArray(rch_deg);
    if (rch_dep != NULL) Release1DArray(rch_dep);
    if (flplain_dep != NULL) Release1DArray(flplain_dep);
}

bool SEDR_SBAGNOLD::CheckInputData(void) {
    if (TIMESTEP < 0) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: TIMESTEP has not been set.");
    }

    if (nreach < 0) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: nreach has not been set.");
    }

    if (FloatEqual(Chs0, NODATA_VALUE)) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: Chs0 has not been set.");
    }

    if (FloatEqual(p_rf, NODATA_VALUE)) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: p_rf has not been set.");
    }

    if (FloatEqual(spcon, NODATA_VALUE)) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: spcon has not been set.");
    }

    if (FloatEqual(spexp, NODATA_VALUE)) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: spexp has not been set.");
    }

    if (FloatEqual(vcrit, NODATA_VALUE)) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: vcrit has not been set.");
    }

    if (vcd < 0) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: vcd must be 0 or 1.");
    }

    if (SEDTOCH == NULL) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: SEDTOCH has not been set.");
    }

    if (chWidth == NULL) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: ReachParameter has not been set.");
    }

    if (CHST == NULL) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: CHST has not been set.");
    }

    if (CHWTDEPTH == NULL) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: CHWTDEPTH has not been set.");
    }

    if (QRECH == NULL) {
        throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputData", "The parameter: QRECH has not been set.");
    }

    return true;
}

void SEDR_SBAGNOLD::initialOutputs() {
    if (nreach <= 0) {
        throw ModelException(MID_SEDR_SBAGNOLD, "initialOutputs",
                             "The cell number of the input can not be less than zero.");
    }

    if (m_reachLayers.empty()) {
        CheckInputData();
        for (int i = 1; i <= nreach; i++) {
            int order = (int) chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }

    //initial channel storage
    if (SEDRECH == NULL) {
        Initialize1DArray(nreach + 1, SEDRECH, 0.f);
        Initialize1DArray(nreach + 1, SEDRECHConc, 0.f);
        Initialize1DArray(nreach + 1, sedStorage, 0.f);
        Initialize1DArray(nreach + 1, sedDep, 0.f);
        Initialize1DArray(nreach + 1, sedDeg, 0.f);

        for (int i = 1; i <= nreach; i++) {
            //sedStorage[i] = Chs0 * chLen[i]; // Chs0 is initial channel storage per meter, not sediment! By LJ
            //sedStorage[i] = sed_chi * Chs0 * chLen[i] * 1000.f;
            sedStorage[i] = sed_chi * CHST[i] * 1000.f; /// ton/m3 * m3/m * m * 1000 = kg
        }
        Initialize1DArray(nreach + 1, rchSand, 0.f);
        Initialize1DArray(nreach + 1, rchSilt, 0.f);
        Initialize1DArray(nreach + 1, rchClay, 0.f);
        Initialize1DArray(nreach + 1, rchSag, 0.f);
        Initialize1DArray(nreach + 1, rchLag, 0.f);
        Initialize1DArray(nreach + 1, rchGra, 0.f);
        Initialize1DArray(nreach + 1, rch_bank_ero, 0.f);
        Initialize1DArray(nreach + 1, rch_deg, 0.f);
        Initialize1DArray(nreach + 1, rch_dep, 0.f);
        Initialize1DArray(nreach + 1, flplain_dep, 0.f);
    }
    /// initialize point source loadings
    if (ptSub == NULL) Initialize1DArray(nreach + 1, ptSub, 0.f);
}

void SEDR_SBAGNOLD::PointSourceLoading() {
    /// load point source water discharge (m3/s) on current day from Scenario
    for (map<int, BMPPointSrcFactory *>::iterator it = m_ptSrcFactory.begin(); it != m_ptSrcFactory.end(); it++) {
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
            float per_sed = curPtMgt->GetSedment(); /// g/cm3, or Mg/m3
            // 1.3 Sum up all point sources
            for (vector<int>::iterator locIter = m_ptSrcIDs.begin(); locIter != m_ptSrcIDs.end(); locIter++) {
                if (m_pointSrcLocsMap.find(*locIter) != m_pointSrcLocsMap.end()) {
                    PointSourceLocations *curPtLoc = m_pointSrcLocsMap.at(*locIter);
                    int curSubID = curPtLoc->GetSubbasinID();
                    /// Mg/m3 ==> kg / timestep
                    ptSub[curSubID] += per_sed * curPtLoc->GetSize() * 1000.f * TIMESTEP / 86400.f;
                }
            }
        }
    }
}

int SEDR_SBAGNOLD::Execute() {
    //check the data
    CheckInputData();
    initialOutputs();
    /// load point source water volume from m_ptSrcFactory
    PointSourceLoading();
    map < int, vector < int > > ::iterator
    it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nReaches = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < nReaches; ++i) {
            int reachIndex = it->second[i]; // index in the array
            SedChannelRouting(reachIndex);
            // compute changes in channel dimensions caused by downcutting and widening
            if (vcd) {
                doChannelDowncuttingAndWidening(reachIndex);
            }
        }
    }
    return 0;
}

bool SEDR_SBAGNOLD::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        //this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
        return false;
    }
    if (this->nreach != n) {
        if (this->nreach <= 0) { this->nreach = n; }
        else {
            //this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
                nreach << ".\n";
            throw ModelException(MID_SEDR_SBAGNOLD, "CheckInputSize", oss.str());
        }
    }
    return true;
}

void SEDR_SBAGNOLD::GetValue(const char *key, float *value) {
    string sk(key);
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_SED_OUTLET)) { *value = SEDRECH[iOutlet]; }
    else {
        throw ModelException(MID_SEDR_SBAGNOLD, "GetValue", "Parameter " + sk + " does not exist.");
    }
}

void SEDR_SBAGNOLD::SetValue(const char *key, float value) {
    string sk(key);

    if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) value);
    }
#ifdef STORM_MODE
    else if (StringMatch(sk, Tag_ChannelTimeStep))
    {
        TIMESTEP = (int) value;
    }
#else
    else if (StringMatch(sk, Tag_TimeStep)) {
        TIMESTEP = (int) value;
    }
#endif /* STORM_MODE */
    else if (StringMatch(sk, VAR_P_RF)) {
        p_rf = value;
    } else if (StringMatch(sk, VAR_SPCON)) {
        spcon = value;
    } else if (StringMatch(sk, VAR_SPEXP)) {
        spexp = value;
    } else if (StringMatch(sk, VAR_VCRIT)) {
        vcrit = value;
    } else if (StringMatch(sk, VAR_CHS0)) {
        Chs0 = value;
    } else if (StringMatch(sk, VAR_SED_CHI0)) { sed_chi = value; }
    else if (StringMatch(sk, VAR_VCD)) { vcd = (int) value; }
    else {
        throw ModelException(MID_SEDR_SBAGNOLD, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void SEDR_SBAGNOLD::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    //check the input data
    if (StringMatch(sk, VAR_SED_TO_CH)) {
        SEDTOCH = data;   //for longterm model
    } else if (StringMatch(sk, VAR_SUB_SEDTOCH)) {
        SEDTOCH = data;   //for storm model
    } else if (StringMatch(sk, VAR_QRECH)) {
        QRECH = data;
    } else if (StringMatch(sk, VAR_CHST)) {
        CHST = data;
    } else if (StringMatch(sk, VAR_PRECHST)) {
        preCHST = data;
    } else if (StringMatch(sk, VAR_CHWTDEPTH)) {
        CHWTDEPTH = data;
    } else if (StringMatch(sk, VAR_CHWTWIDTH)) {
        chwtwidth = data;
    } else if (StringMatch(sk, VAR_PRECHWTDEPTH)) {
        prechwtdepth = data;
    } else {
        throw ModelException(MID_SEDR_SBAGNOLD, "Set1DData", "Parameter " + sk + " does not exist");
    }
}

void SEDR_SBAGNOLD::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = nreach + 1;
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_SED_RECH)) {
        SEDRECH[0] = SEDRECH[iOutlet];    // kg
        *data = SEDRECH;
    } else if (StringMatch(sk, VAR_SED_RECHConc)) {
        SEDRECHConc[0] = SEDRECHConc[iOutlet];    // kg/m3, i.e., g/L
        *data = SEDRECHConc;
    } else if (StringMatch(sk, VAR_RCH_DEG)) {
        *data = rch_deg;
    } else if (StringMatch(sk, VAR_RCH_BANKERO)) {
        *data = rch_bank_ero;
    } else if (StringMatch(sk, VAR_RCH_DEP)) {
        *data = rch_dep;
    } else if (StringMatch(sk, VAR_FLPLAIN_DEP)) {
        *data = flplain_dep;
    } else {
        throw ModelException(MID_SEDR_SBAGNOLD, "Get1DData", "Output " + sk + " does not exist.");
    }
}

void SEDR_SBAGNOLD::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    string sk(key);
}

void SEDR_SBAGNOLD::SetScenario(Scenario *sce) {
    if (sce != NULL) {
        map<int, BMPFactory *> tmpBMPFactories = sce->GetBMPFactories();
        for (map<int, BMPFactory *>::iterator it = tmpBMPFactories.begin(); it != tmpBMPFactories.end(); it++) {
            /// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
            if (it->first / 100000 == BMP_TYPE_POINTSOURCE) {
                m_ptSrcFactory[it->first] = (BMPPointSrcFactory *) it->second;
            }
        }
    } else {
        throw ModelException(MID_SEDR_SBAGNOLD, "SetScenario", "The scenario can not to be NULL.");
    }
}

void SEDR_SBAGNOLD::SetReaches(clsReaches *reaches) {
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
            Initialize1DArray(nreach + 1, chSlope, 0.f);
            Initialize1DArray(nreach + 1, chCover, 0.f);
            Initialize1DArray(nreach + 1, chErod, 0.f);
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
            chCover[i] = (float) tmpReach->GetCover();
            chErod[i] = (float) tmpReach->GetErod();
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
        throw ModelException(MID_SEDR_SBAGNOLD, "SetReaches", "The reaches input can not to be NULL.");
    }
}

void SEDR_SBAGNOLD::SedChannelRouting(int i) {
    float qOutV = 0.f; // water volume (m^3) of flow out
    float allWater = 0.f; // // water in reach during time step, qdin in SWAT
    // initialize sediment in reach during time step
    /// sediment from upstream reaches
    float allSediment = 0.f; // all sediment in reach, kg
    float sedUp = 0.f; // sediment from upstream channels, kg
    for (size_t j = 0; j < m_reachUpStream[i].size(); ++j) {
        int upReachId = m_reachUpStream[i][j];
        sedUp += SEDRECH[upReachId];
    }
    allSediment = sedUp + SEDTOCH[i] + sedStorage[i];
    /// add point source loadings
    if (ptSub != NULL && ptSub[i] > 0.f) {
        allSediment += ptSub[i];
    }
    //if (i == 12) cout<<"\tall sed: "<<allSediment<<", sed up: "<<sedUp<<", sed to ch: "<<SEDTOCH[i]<<", strg: "<<sedStorage[i]<<endl;

    // initialize water in reach during time step
    qOutV = QRECH[i] * TIMESTEP; // m^3
    //allWater = CHST[i] + qOutV;
    allWater = preCHST[i];
    if ((QRECH[i] < UTIL_ZERO && CHWTDEPTH[i] < UTIL_ZERO) || allWater < 0.01f) {
        /// do not perform sediment routing when:
        /// 1. whether is no water flow out of channel and water depth is nearly zero
        /// 2. if no water in reach
        sedDeg[i] = 0.f;
        sedDep[i] = 0.f;
        SEDRECH[i] = 0.f;
        SEDRECHConc[i] = 0.f;
        rchSand[i] = 0.f;
        rchSilt[i] = 0.f;
        rchClay[i] = 0.f;
        rchSag[i] = 0.f;
        rchLag[i] = 0.f;
        rchGra[i] = 0.f;
        rch_bank_ero[i] = 0.f;
        rch_deg[i] = 0.f;
        rch_dep[i] = 0.f;
        flplain_dep[i] = 0.f;
        sedStorage[i] = allSediment;
        return;
    }
    // initialize reach peak runoff rate and calculate flow velocity
    float peakFlowRate = QRECH[i] * p_rf;
    float crossarea = allWater / chLen[i]; // SWAT, eq. 7:1.1.7
    float peakVelocity = 0.f;
    if (prechwtdepth[i] < 0.01f) {
        peakVelocity = 0.01f;
    } else {
        peakVelocity = peakFlowRate / crossarea;
    }
    if (peakVelocity > 5.f) peakVelocity = 5.f;

    /// calculate tbase  |none  |flow duration (fraction of 24 hr)
    float tbase = chLen[i] / (TIMESTEP * peakVelocity);
    if (tbase > 1.f) tbase = 1.f;

    /// New imporoved method for sediment transport
    float initCon = 0.f; // cyin
    float maxCon = 0.f; // cych
    float sedDeposition = 0.f; // depnet, and dep
    float sedDegradation = 0.f; // deg
    float sedDegradation1 = 0.f; // deg1
    float sedDegradation2 = 0.f; // deg2

    //deposition and degradation
    initCon = allSediment / allWater; // kg/m^3
    //max concentration
    maxCon = spcon * pow(peakVelocity, spexp) * 1000.f; // kg/m^3
    //if (i == 12) cout<<"iniCon: "<<initCon<<", maxCon: "<<maxCon<<endl;
    //initial concentration,mix sufficiently
    sedDeposition = allWater * (initCon - maxCon); // kg
    if (peakVelocity < vcrit) {
        sedDeposition = 0.f;
    }

    if (sedDeposition < 0.f)    //degradation
    {
        sedDegradation = -sedDeposition * tbase;
        // first the deposited material will be degraded before channel bed
        if (sedDegradation >= sedDep[i]) {
            sedDegradation1 = sedDep[i];
            sedDegradation2 = (sedDegradation - sedDegradation1) * chErod[i] * chCover[i];
        } else {
            sedDegradation1 = sedDegradation;
            sedDegradation2 = 0.f;
        }
        sedDeposition = 0.f;
    } else {
        sedDegradation = 0.f;
        sedDegradation1 = 0.f;
        sedDegradation2 = 0.f;
    }
    //update sed deposition
    sedDep[i] += sedDeposition - sedDegradation1;
    if (sedDep[i] < UTIL_ZERO) sedDep[i] = 0.f;
    sedDeg[i] += sedDegradation1 + sedDegradation2;

    //get sediment after deposition and degradation
    allSediment += sedDegradation1 + sedDegradation2 - sedDeposition;
    if (allSediment < UTIL_ZERO) allSediment = 0.f;
    //get out flow water fraction
    float outFraction = qOutV / allWater;
    if (outFraction > 1.f) outFraction = 1.f;
    SEDRECH[i] = allSediment * outFraction;
    if (SEDRECH[i] < UTIL_ZERO) SEDRECH[i] = 0.f;
    // update sediment storage
    sedStorage[i] = allSediment - SEDRECH[i];
    if (sedStorage[i] < UTIL_ZERO) sedStorage[i] = 0.f;

    // get final sediment in water, cannot large than 0.848 ton/m3
    float maxSedinWt = 0.848f * qOutV * 1000.f; /// kg
    if (SEDRECH[i] > maxSedinWt) {
        sedDep[i] += SEDRECH[i] - maxSedinWt;
        SEDRECH[i] = maxSedinWt;
    }
    /// calculate sediment concentration
    SEDRECHConc[i] = SEDRECH[i] / qOutV; /// kg/m3, i.e., g/L
    /// in this default sediment routing method, sediment is not tracked by particle size
    rchSand[i] = 0.f;
    rchSilt[i] = SEDRECH[i]; // the sediments is assumed to be silt for mass conservation
    rchClay[i] = 0.f;
    rchSag[i] = 0.f;
    rchLag[i] = 0.f;
    rchGra[i] = 0.f;

    rch_bank_ero[i] = 0.f;
    rch_deg[i] = sedDegradation2;
    rch_dep[i] = sedDeposition;
    flplain_dep[i] = 0.f;
    //if (i == 12) cout<<"\tallSediment2: "<<allSediment<<", sedDeg: "<<sedDegradation1<<", sedDeg2: "<<sedDegradation2<<
    //", sedDeposition: "<<sedDeposition<<", sed flow out: "<<SEDRECH[i]<<endl;
}

void SEDR_SBAGNOLD::doChannelDowncuttingAndWidening(int id) {
    /// TODO, lj
    float depdeg = prechwtdepth[id] - CHWTDEPTH[id]; // depth of degradation/deposition from original
    if (depdeg < chSlope[id] * chLen[id]) {
        //float storage = CHST[id];
        //float vout = QRECH[id] * TIMESTEP;
        if (preCHST[id] > 1.4e6f) {
            /// downcutting depth, m
            float cutDepth = 358.6f * CHWTDEPTH[id] * chSlope[id] * chErod[id];
            CHWTDEPTH[id] += cutDepth;
            chwtwidth[id] = CHWTDEPTH[id] * (chWidth[id] / chDepth[id]);

            chSlope[id] -= cutDepth / chLen[id];
            chSlope[id] = max(0.0001f, chSlope[id]);
        }
    }
    // call ttcoef(jrch) // TODO
}
