#include "seims.h"
#include "pothole_SWAT.h"

using namespace std;

IMP_SWAT::IMP_SWAT(void) : cnv(NODATA_VALUE), m_nCells(-1), CELLWIDTH(NODATA_VALUE), cellArea(NODATA_VALUE),
                           soillayers(NULL), nSoilLayers(-1), ROUTING_LAYERS(NULL), nRoutingLayers(-1),
                           subbasin(NULL),
                           slope(NULL), Conductivity(NULL), sol_ul(NULL), sol_sumAWC(NULL), soilthick(NULL),
                           Porosity(NULL),
                           evlai(NODATA_VALUE), pot_tilemm(NODATA_VALUE), pot_no3l(NODATA_VALUE),
                           pot_solpl(NODATA_VALUE),
                           impound_trig(NULL),
                           SED_OL(NULL), sand_yld(NULL), silt_yld(NULL), clay_yld(NULL),
                           sag_yld(NULL), lag_yld(NULL),
                           DEET(NULL), DPST(NULL), LAIDAY(NULL), PET(NULL), solst(NULL),
                           solsw(NULL),
                           OL_Flow(NULL), sur_no3(NULL), sur_nh4(NULL), sur_solp(NULL), sur_cod(NULL),
                           sedorgn(NULL), sedorgp(NULL), sedminpa(NULL), sedminps(NULL),
                           pot_no3(NULL), pot_nh4(NULL), pot_orgn(NULL), pot_solp(NULL), potOrgP(NULL),
                           potActMinP(NULL),
                           potStaMinP(NULL), potSed(NULL), potSand(NULL), potSilt(NULL), potClay(NULL),
                           potSag(NULL), potLag(NULL),
                           pot_vol(NULL), pot_volmaxmm(NULL), pot_vollowmm(NULL), potSeep(NULL), potEvap(NULL),
                           pot_sa(NULL),
                           kv_paddy(NODATA_VALUE), kn_paddy(NODATA_VALUE), pot_k(NODATA_VALUE),
    /// overland to channel
                           SBOF(NULL), SEDTOCH(NULL), sur_no3_ToCh(NULL), SUR_NH4_TOCH(NULL),
                           sur_solp_ToCh(NULL), sur_cod_ToCH(NULL),
                           sedorgnToCh(NULL), sedorgpToCh(NULL), sedminpaToCh(NULL), sedminpsToCh(NULL) {
    //m_potSedIn(NULL), m_potSandIn(NULL), m_potSiltIn(NULL), m_potClayIn(NULL), m_potSagIn(NULL), m_potLagIn(NULL),
}

IMP_SWAT::~IMP_SWAT(void) {
    if (pot_sa != NULL) Release1DArray(pot_sa);
    if (pot_no3 != NULL) Release1DArray(pot_no3);
    if (pot_nh4 != NULL) Release1DArray(pot_nh4);
    if (pot_orgn != NULL) Release1DArray(pot_orgn);
    if (pot_solp != NULL) Release1DArray(pot_solp);
    if (potOrgP != NULL) Release1DArray(potOrgP);
    if (potActMinP != NULL) Release1DArray(potActMinP);
    if (potStaMinP != NULL) Release1DArray(potStaMinP);
    if (potSed != NULL) Release1DArray(potSed);
    if (potSand != NULL) Release1DArray(potSand);
    if (potSilt != NULL) Release1DArray(potSilt);
    if (potClay != NULL) Release1DArray(potClay);
    if (potSag != NULL) Release1DArray(potSag);
    if (potLag != NULL) Release1DArray(potLag);
    if (pot_vol != NULL) Release1DArray(pot_vol);
    if (potSeep != NULL) Release1DArray(potSeep);
    if (potEvap != NULL) Release1DArray(potEvap);
}

bool IMP_SWAT::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_IMP_SWAT, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}

bool IMP_SWAT::CheckInputSize2D(const char *key, int n, int col) {
    CheckInputSize(key, n);
    if (col <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
            " is invalid. The layer number could not be less than zero.");
    }
    if (nSoilLayers != col) {
        if (nSoilLayers <= 0) {
            nSoilLayers = col;
        } else {
            throw ModelException(MID_IMP_SWAT, "CheckInputSize2D", "Input data for " + string(key) +
                " is invalid. All the layers of input 2D raster data should have same size of " +
                ValueToString(nSoilLayers) + " instead of " +
                ValueToString(col) + ".");
            return false;
        }
    }
    return true;
}

bool IMP_SWAT::CheckInputData() {
    if (CELLWIDTH <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (m_nCells <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (nSoilLayers <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The soil layers number can not be less than zero.");
    }
    if (nRoutingLayers <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The routing layers number can not be less than zero.");
    }
    if (evlai <= 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The evLAI can not be less than zero.");
    }
    if (pot_tilemm < 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The pot_tilemm number can not be less than zero.");
    }
    if (pot_no3l < 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The pot_no3l can not be less than zero.");
    }
    if (pot_solpl < 0) {
        throw ModelException(MID_IMP_SWAT, "CheckInputData", "The pot_solpl can not be less than zero.");
    }

    return true;
}

void IMP_SWAT::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, Tag_CellWidth)) {
        CELLWIDTH = value;
        cellArea = CELLWIDTH * CELLWIDTH * 1.e-4f; // m2 ==> ha
        cnv = 10.f * cellArea; // mm/ha => m^3
    } else if (StringMatch(sk, Tag_TimeStep)) { TIMESTEP = value; }
    else if (StringMatch(sk, VAR_EVLAI)) { evlai = value; }
    else if (StringMatch(sk, VAR_POT_TILEMM)) { pot_tilemm = value; }
    else if (StringMatch(sk, VAR_POT_NO3DECAY)) { pot_no3l = value; }
    else if (StringMatch(sk, VAR_POT_SOLPDECAY)) { pot_solpl = value; }
    else if (StringMatch(sk, VAR_KV_PADDY)) { kv_paddy = value; }
    else if (StringMatch(sk, VAR_KN_PADDY)) { kn_paddy = value; }
    else if (StringMatch(sk, VAR_POT_K)) { pot_k = value; }
    else {
        throw ModelException(MID_IMP_SWAT, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void IMP_SWAT::Set1DData(const char *key, int n, float *data) {
    string sk(key);

    if (StringMatch(sk, VAR_SBOF)) {
        SBOF = data;
        subbasinNum = n - 1; /// TODO, add a checkInputSize2 function
        return;
    } else if (StringMatch(sk, VAR_SED_TO_CH)) {
        SEDTOCH = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SUR_NO3_TOCH)) {
        sur_no3_ToCh = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SUR_NH4_TOCH)) {
        SUR_NH4_TOCH = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SUR_SOLP_TOCH)) {
        sur_solp_ToCh = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SUR_COD_TOCH)) {
        sur_cod_ToCH = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SEDORGN_TOCH)) {
        sedorgnToCh = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SEDORGP_TOCH)) {
        sedorgpToCh = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SEDMINPA_TOCH)) {
        sedminpaToCh = data;
        subbasinNum = n - 1;
        return;
    } else if (StringMatch(sk, VAR_SEDMINPS_TOCH)) {
        sedminpsToCh = data;
        subbasinNum = n - 1;
        return;
    }
    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_SLOPE)) { slope = data; }
    else if (StringMatch(sk, VAR_SOILLAYERS)) { soillayers = data; }
    else if (StringMatch(sk, VAR_SUBBSN)) { subbasin = data; }
    else if (StringMatch(sk, VAR_SOL_SUMAWC)) { sol_sumAWC = data; }
    else if (StringMatch(sk, VAR_IMPOUND_TRIG)) { impound_trig = data; }
    else if (StringMatch(sk, VAR_POT_VOLMAXMM)) { pot_volmaxmm = data; }
    else if (StringMatch(sk, VAR_POT_VOLLOWMM)) { pot_vollowmm = data; }
    else if (StringMatch(sk, VAR_SEDYLD)) { SED_OL = data; }
    else if (StringMatch(sk, VAR_SANDYLD)) { sand_yld = data; }
    else if (StringMatch(sk, VAR_SILTYLD)) { silt_yld = data; }
    else if (StringMatch(sk, VAR_CLAYYLD)) { clay_yld = data; }
    else if (StringMatch(sk, VAR_SAGYLD)) { sag_yld = data; }
    else if (StringMatch(sk, VAR_LAGYLD)) { lag_yld = data; }
    else if (StringMatch(sk, VAR_LAIDAY)) { LAIDAY = data; }
    else if (StringMatch(sk, VAR_PET)) { PET = data; }
    else if (StringMatch(sk, VAR_SOL_SW)) { solsw = data; }
    else if (StringMatch(sk, VAR_DEET)) { DEET = data; }
    else if (StringMatch(sk, VAR_DPST)) { DPST = data; }
    else if (StringMatch(sk, VAR_OLFLOW)) { OL_Flow = data; }
    else if (StringMatch(sk, VAR_SUR_NO3)) { sur_no3 = data; }
    else if (StringMatch(sk, VAR_SUR_NH4)) { sur_nh4 = data; }
    else if (StringMatch(sk, VAR_SUR_SOLP)) { sur_solp = data; }
    else if (StringMatch(sk, VAR_SUR_COD)) { sur_cod = data; }
    else if (StringMatch(sk, VAR_SEDORGN)) { sedorgn = data; }
    else if (StringMatch(sk, VAR_SEDORGP)) { sedorgp = data; }
    else if (StringMatch(sk, VAR_SEDMINPA)) { sedminpa = data; }
    else if (StringMatch(sk, VAR_SEDMINPS)) { sedminps = data; }
    else {
        throw ModelException(MID_IMP_SWAT, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void IMP_SWAT::Set2DData(const char *key, int n, int col, float **data) {
    string sk(key);
    if (StringMatch(sk, Tag_ROUTING_LAYERS)) {
        nRoutingLayers = n;
        ROUTING_LAYERS = data;
        return;
    }
    CheckInputSize2D(key, n, col);
    if (StringMatch(sk, VAR_CONDUCT)) { Conductivity = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_POROST)) { Porosity = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { solst = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else {
        throw ModelException(MID_IMP_SWAT, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void IMP_SWAT::initialOutputs() {
    if (pot_sa == NULL) Initialize1DArray(m_nCells, pot_sa, 0.f);
    if (pot_vol == NULL) Initialize1DArray(m_nCells, pot_vol, 0.f);
    if (pot_no3 == NULL) Initialize1DArray(m_nCells, pot_no3, 0.f);
    if (pot_nh4 == NULL) Initialize1DArray(m_nCells, pot_nh4, 0.f);
    if (pot_orgn == NULL) Initialize1DArray(m_nCells, pot_orgn, 0.f);
    if (pot_solp == NULL) Initialize1DArray(m_nCells, pot_solp, 0.f);
    if (potOrgP == NULL) Initialize1DArray(m_nCells, potOrgP, 0.f);
    if (potActMinP == NULL) Initialize1DArray(m_nCells, potActMinP, 0.f);
    if (potStaMinP == NULL) Initialize1DArray(m_nCells, potStaMinP, 0.f);
    if (potSed == NULL) Initialize1DArray(m_nCells, potSed, 0.f);
    if (potSand == NULL) Initialize1DArray(m_nCells, potSand, 0.f);
    if (potSilt == NULL) Initialize1DArray(m_nCells, potSilt, 0.f);
    if (potClay == NULL) Initialize1DArray(m_nCells, potClay, 0.f);
    if (potSag == NULL) Initialize1DArray(m_nCells, potSag, 0.f);
    if (potLag == NULL) Initialize1DArray(m_nCells, potLag, 0.f);
    /// water loss
    if (potSeep == NULL) Initialize1DArray(m_nCells, potSeep, 0.f);
    if (potEvap == NULL) Initialize1DArray(m_nCells, potEvap, 0.f);
}

int IMP_SWAT::Execute() {
    CheckInputData();
    initialOutputs();

    for (int iLayer = 0; iLayer < nRoutingLayers; ++iLayer) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nCells = (int) ROUTING_LAYERS[iLayer][0];
#pragma omp parallel for
        for (int iCell = 1; iCell <= nCells; ++iCell) {
            int id = (int) ROUTING_LAYERS[iLayer][iCell]; // cell index
            if (FloatEqual(impound_trig[id], 0.f)) { /// if impounding trigger on
                potholeSimulate(id);
            } else {
                releaseWater(id);
            }
        }
    }
    /// reCalculate the surface runoff, sediment, nutrient etc. that into the channel
    // cout<<"pre surq no3 to ch: "<<sur_no3_ToCh[12]<<endl;
    // cout<<"pre surfq to ch: "<<SBOF[12]<<", orgp to ch: "<<sedorgpToCh[12]<<endl;
#pragma omp parallel for
    for (int i = 0; i < subbasinNum + 1; i++) {
        SBOF[i] = 0.f;
        SEDTOCH[i] = 0.f;
        sur_no3_ToCh[i] = 0.f;
        SUR_NH4_TOCH[i] = 0.f;
        sur_solp_ToCh[i] = 0.f;
        sur_cod_ToCH[i] = 0.f;
        sedorgnToCh[i] = 0.f;
        sedorgpToCh[i] = 0.f;
        sedminpaToCh[i] = 0.f;
        sedminpsToCh[i] = 0.f;
    }
    // cout<<"final orgp: "<<sedorgp[46364]<<endl;
    // cout<<"final surq no3: "<<sur_no3[46364]<<endl;
    //float maxno3 = -1.f;
    //int idx = -1;
    //for (int i = 0; i < m_nCells; i++)
    //{
    //	if (sur_no3[i] > maxno3)
    //	{
    //		maxno3 = sur_no3[i];
    //		idx = i;
    //	}
    //}
    //cout<<"maximum no3 id: "<<idx<<endl;
    float maxsedorgp = -1.f;
    int idx = -1;
    for (int i = 0; i < m_nCells; i++) {
        if (sedorgp[i] > maxsedorgp) {
            maxsedorgp = sedorgp[i];
            idx = i;
        }
    }
    // cout<<"maximum sedorgp id: "<<idx<< ", surfq: " <<OL_Flow[idx]<<", sedorgp: "<<sedorgp[idx]<<endl;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        int subi = (int) subbasin[i];
        SBOF[subi] += OL_Flow[i] * cellArea * 10.f / TIMESTEP; /// mm -> m3/s
        SEDTOCH[subi] += SED_OL[i];
        sur_no3_ToCh[subi] += sur_no3[i] * cellArea;
        SUR_NH4_TOCH[subi] += sur_nh4[i] * cellArea;
        sur_solp_ToCh[subi] += sur_solp[i] * cellArea;
        sur_cod_ToCH[subi] += sur_cod[i] * cellArea;
        sedorgnToCh[subi] += sedorgn[i] * cellArea;
        sedorgpToCh[subi] += sedorgp[i] * cellArea;
        sedminpaToCh[subi] += sedminpa[i] * cellArea;
        sedminpsToCh[subi] += sedminps[i] * cellArea;
    }
#pragma omp parallel for
    for (int i = 1; i < subbasinNum + 1; i++) {
        SBOF[0] += SBOF[i];
        SEDTOCH[0] += SEDTOCH[i];
        sur_no3_ToCh[0] += sur_no3_ToCh[i];
        SUR_NH4_TOCH[0] += SUR_NH4_TOCH[i];
        sur_solp_ToCh[0] += sur_solp_ToCh[i];
        sur_cod_ToCH[0] += sur_cod_ToCH[i];
        sedorgnToCh[0] += sedorgnToCh[i];
        sedorgpToCh[0] += sedorgpToCh[i];
        sedminpaToCh[0] += sedminpaToCh[i];
        sedminpsToCh[0] += sedminpsToCh[i];
    }
    // cout<<", new: "<<sedorgpToCh[12]<<endl;
    return true;
}

void IMP_SWAT::potholeSimulate(int id) {
/// initialize temporary variables
    float tileo = 0.f; /// m^3, amount of water released to the main channel from the water body by drainage tiles
    //float potevmm = 0.f; /// mm, volume of water evaporated from pothole expressed as depth
    float potev = 0.f; /// m^3, evaporation from impounded water body
    float
        spillo = 0.f; /// m^3, amount of water released to the main channel from impounded water body due to spill-over

    /// potpcpmm and potpcp should be implicitly included in (DPST + DEET) if stated
    //float potpcpmm = 0.f; /// mm, precipitation falling on pothole water body expressed as depth
    //float potpcp = 0.f; /// m^3, precipitation falling on water body

    //float potsepmm = 0.f; // mm, seepage from impounded water body expressed as depth
    float potsep = 0.f; /// m^3, seepage from impounded water body
    //float sumo = 0.f; /// m^3, sum of all releases from water body on current day
    //float potflwo = 0.f; /// mm, discharge from pothole expressed as depth
    float potsedo = 0.f; /// kg, sediment leaving pothole on day
    float potsano = 0.f; /// kg, sand content in sediment leaving pothole on day
    float potsilo = 0.f; /// kg, silt content
    float potclao = 0.f; /// kg, clay content
    float potsago = 0.f; /// kg, small aggregate
    float potlago = 0.f; /// kg, large aggregate
    float potno3o = 0.f; /// kg, no3 amount out of pothole
    float potnh4o = 0.f; /// kg, nh4 amount out of pothole
    float potsolpo = 0.f; /// kg, soluble phosphorus out of pothole
    float potorgno = 0.f;/// kg, orgN out
    float potorgpo = 0.f; /// kg, orgP out
    float potmpso = 0.f; /// kg, stable mineral phosphorus out
    float potmpao = 0.f; /// kg, active mineral phosphorus out
    //float potvol_ini = 0.f; /// m^3, pothole volume at the begin of the day
    //float potsa_ini = 0.f; /// ha, surface area of impounded water body at the begin of the day
    float sedloss = 0.f; /// kg, amount of sediment settling out of water during day
    float sanloss = 0.f;
    float silloss = 0.f;
    float claloss = 0.f;
    float sagloss = 0.f;
    float lagloss = 0.f;
    float no3loss = 0.f; /// kg, amount of nitrate lost from water body
    float nh4loss = 0.f; /// kg, amount of ammonian lost
    float solploss = 0.f; /// kg, amount of solP lost
    float orgnloss = 0.f; /// kg, amount of orgN lost
    float orgploss = 0.f; /// kg, amount of orgP lost
    float minpsloss = 0.f; /// kg, amount of stable minP lost
    float minpaloss = 0.f; /// kg, amount of active minP lost
    /* pot_fr is the fraction of the cell draining into the pothole
     * the remainder (1-pot_fr) goes directly to runoff
     * currently, we assumed that the entire cell is pothole/impounded area
     */
    float pot_fr = 1.f;
    float qIn = OL_Flow[id] * pot_fr; /// inflow = surface flow, not include lateral flow, groundwater, etc.
    float qdayTmp = OL_Flow[id] * (1 - pot_fr); /// qdayTmp is the actual surface runoff generated
    if (DPST != NULL && DPST[id] > 0.f) {
        qIn += DPST[id]; /// depression storage should be added
        DPST[id] = 0.f;
    }
    if (DEET != NULL && DEET[id] > 0.f) {
        qIn += DEET[id]; /// since the evaporation will be calculated below, the DEET should be added
        DEET[id] = 0.f;
    }
    /// update volume of water in pothole
    pot_vol[id] += qIn;
    //m_potFlowIn[id] += qIn; // TODO, this should be routing cell by cell. by lj

    /* compute surface area of pothole
     * SWAT assuming a cone shape, ha
     * i.e., potholeSurfaceArea(id);
     * However, currently, we assume it is cell area
     */
    pot_sa[id] = cellArea;
    //potvol_ini = pot_vol[id];
    //potsa_ini = pot_sa[id];

    /// update sediment in pothole
    potSed[id] += SED_OL[id] * pot_fr;
    // float m_potSedIn = potSed[id];
    potSand[id] += sand_yld[id] * pot_fr;
    float m_potSandIn = potSand[id];
    potSilt[id] += silt_yld[id] * pot_fr;
    float m_potSiltIn = potSilt[id];
    potClay[id] += clay_yld[id] * pot_fr;
    float m_potClayIn = potClay[id];
    potSag[id] += sag_yld[id] * pot_fr;
    float m_potSagIn = potSag[id];
    potLag[id] += lag_yld[id] * pot_fr;
    float m_potLagIn = potLag[id];

    /// update sediment yields
    float yy = 1.f - pot_fr;
    SED_OL[id] *= yy;
    sand_yld[id] *= yy;
    silt_yld[id] *= yy;
    clay_yld[id] *= yy;
    sag_yld[id] *= yy;
    lag_yld[id] *= yy;
    // if(id == 46364) cout<<"pre surq no3: "<<sur_no3[id];
    // if(id == 46364) cout<<"pre orgp: "<<sedorgp[id];
    /// update forms of N and P in pothole
    float xx = pot_fr * cellArea;
    pot_no3[id] += sur_no3[id] * xx; // kg/ha * ha ==> kg
    pot_nh4[id] += sur_nh4[id] * xx;
    pot_orgn[id] += sedorgn[id] * xx;
    pot_solp[id] += sur_solp[id] * xx;
    potOrgP[id] += sedorgp[id] * xx;
    potActMinP[id] += sedminpa[id] * xx;
    potStaMinP[id] += sedminps[id] * xx;
    /// update forms of N and P in surface runoff
    sur_no3[id] *= yy;
    // if (id == 46364) cout<<", *=yy: "<<sur_no3[id];
    sur_nh4[id] *= yy;
    sedorgn[id] *= yy;
    sur_solp[id] *= yy;
    sedorgp[id] *= yy;
    sedminpa[id] *= yy;
    sedminps[id] *= yy;

    /// if overflow, then send the overflow to the cell's surface flow
    if (pot_vol[id] > pot_volmaxmm[id]) {
        qdayTmp += (pot_vol[id] - pot_volmaxmm[id]);
        spillo = pot_vol[id] - pot_volmaxmm[id];
        pot_vol[id] = pot_volmaxmm[id];
        if (spillo + pot_volmaxmm[id] < UTIL_ZERO) { // this should not happen
            xx = 0.f;
        } else {
            xx = spillo / (spillo + pot_volmaxmm[id]);
        }
        potsedo += potSed[id] * xx;
        potsano += potSand[id] * xx;
        potsilo += potSilt[id] * xx;
        potclao += potClay[id] * xx;
        potsago += potSag[id] * xx;
        potlago += potLag[id] * xx;
        potno3o += pot_no3[id] * xx;
        // if (id == 46364) cout<<"xx: "<<xx<<", potno3o: "<<potno3o<<endl;
        potnh4o += pot_nh4[id] * xx;
        potorgno += pot_orgn[id] * xx;
        potsolpo += pot_solp[id] * xx;
        potorgpo += potOrgP[id] * xx;
        potmpao += potActMinP[id] * xx;
        potmpso += potStaMinP[id] * xx;

        potSed[id] -= potsedo;
        potSand[id] -= potsano;
        potSilt[id] -= potsilo;
        potClay[id] -= potclao;
        potSag[id] -= potsago;
        potLag[id] -= potlago;
        pot_no3[id] -= potno3o;
        pot_nh4[id] -= potnh4o;
        pot_orgn[id] -= potorgno;
        pot_solp[id] -= potsolpo;
        potOrgP[id] -= potorgpo;
        potStaMinP[id] -= potmpso;
        potActMinP[id] -= potmpao;

        SED_OL[id] += potsedo / cellArea;
        sand_yld[id] += potsano / cellArea;
        silt_yld[id] += potsilo / cellArea;
        clay_yld[id] += potclao / cellArea;
        sag_yld[id] += potsago / cellArea;
        lag_yld[id] += potlago / cellArea;
        sur_no3[id] += potno3o / cellArea;
        // if (id == 46364) cout<<", +=potno3o: "<<sur_no3[id];
        sur_nh4[id] += potnh4o / cellArea;
        sedorgn[id] += potorgno / cellArea;
        sur_solp[id] += potsolpo / cellArea;
        sedorgp[id] += potorgpo / cellArea;
        sedminps[id] += potmpso / cellArea;
        sedminpa[id] += potmpao / cellArea;
    } /// end if overflow

    /// if no overflow, compute settling and losses, surface inlet tile
    /// flow, evap, seepage, and redistribute soil water
    if (pot_vol[id] > UTIL_ZERO) {
        /// compute settling  -clay and silt based on fall velocity (v = 411*d2) d=mm, v=m/hr
        float pot_depth = pot_vol[id];
        float drcla = 0.f, drsil = 0.f, drtot = 0.f;
        if (pot_depth > 10.f) { /// assume clay v(fall) = 10 mm/d
            drcla = 1.f - 0.5f * 10.f / pot_depth;
        } else {
            drcla = 0.5f * pot_depth / 10.f;
        }
        potClay[id] *= drcla;

        if (pot_depth > 1000.f) { /// assume silt v(fall) = 1000 mm/d
            drsil = 1.f - 0.5f * 1000.f / pot_depth;
        } else {
            drsil = 0.5f * pot_depth / 1000.f;
        }
        potSilt[id] *= drsil;

        /// assume complete settling of all other size (dr = 0)
        potSand[id] = 0.f;
        potSag[id] = 0.f;
        potLag[id] = 0.f;

        /// compute total delivery ratio for pot_sed
        float allSedPart = m_potClayIn + m_potSiltIn + m_potSandIn + m_potSagIn + m_potLagIn;

        if (allSedPart < UTIL_ZERO) {
            drtot = 0.f;
        } else {
            drtot = (potClay[id] + potSilt[id] + potSand[id] + potSag[id] + potLag[id]) / allSedPart;
        }
        potSed[id] *= drtot;

        /// compute organic settling assuming an enrichment ratio of 3 on clay (0.75)
        /// delivery of organics is 0.75 * dr(clay)- assuming dr on all non-clay = 1
        pot_orgn[id] *= 0.75f * drcla;
        potOrgP[id] *= 0.75f * drcla;
        potActMinP[id] *= 0.75f * drcla;
        potStaMinP[id] *= 0.75f * drcla;
        ///pot_no3[id] *= (1.f - pot_no3l);
        pot_solp[id] *= (1.f - pot_solpl);
        /*
         * first-order kinetics is adopted from Chowdary et al., 2004
         * to account for volatilization, nitrification, and denitrification in impounded water
         */
        float nh3V = pot_nh4[id] * (1.f - exp(-kv_paddy * TIMESTEP / 86400.f));
        float no3N = pot_nh4[id] * (1.f - exp(-kn_paddy * TIMESTEP / 86400.f));
        /// update
        pot_nh4[id] -= (nh3V + no3N);
        pot_no3[id] += no3N;

        pot_nh4[id] = max(pot_nh4[id], UTIL_ZERO);
        pot_no3[id] = max(pot_no3[id], UTIL_ZERO);

        /// compute flow from surface inlet tile
        tileo = min(pot_tilemm, pot_vol[id]);
        float potvol_tile = pot_vol[id];
        pot_vol[id] -= tileo;
        qdayTmp += tileo;

        /// limit seepage into soil if profile is near field capacity
        /* pot_k: hydraulic conductivity of soil surface of pothole
         * [defaults to conductivity of upper soil (0.01--10.) layer]
         * set as input parameters from database
         */
        if (pot_k > 0.f) {
            yy = pot_k;
        } else {
            yy = Conductivity[id][0];
        }
        /// calculate seepage into soil
        potsep = yy * pot_sa[id] * 240.f / cnv; /// mm/hr*ha/240=m3/cnv=mm
        potsep = min(potsep, pot_vol[id]);
        float potvol_sep = pot_vol[id];
        pot_vol[id] -= potsep;
        potSeep[id] += potsep;
        solst[id][0] += potsep; /// this will be handled in the next time step, added by LJ

        ///// force the soil water storage to field capacity
        //for (int ly = 0; ly < (int)soillayers[id]; ly++)
        //{
        //	float dep2cap = sol_ul[id][ly] - solst[id][ly];
        //	if (dep2cap > 0.f)
        //	{
        //		dep2cap = min(dep2cap, pot_vol[id]);
        //		solst[id][ly] += dep2cap;
        //		pot_vol[id] -= dep2cap;
        //	}
        //}
        //if (pot_vol[id] < pot_vollowmm[id])
        //	pot_vol[id] = pot_vollowmm[id]; /// force to reach the lowest depth.
        ///// recompute total soil water storage
        //solsw[id] = 0.f;
        //for (int ly = 0; ly < (int)soillayers[id]; ly++)
        //	solsw[id] += solst[id][ly];

        /// compute evaporation from water surface
        if (LAIDAY[id] < evlai) {
            potev = (1.f - LAIDAY[id] / evlai) * PET[id];
            // if (id == 46364) cout<<"pet: "<<PET[id]<<", laiday: "<<LAIDAY[id]<<", potEvap: "<<potev<<", ";
            potev = min(potev, pot_vol[id]);
            pot_vol[id] -= potev;
            potEvap[id] += potev;
        }
        if (potvol_tile > UTIL_ZERO) {
            sedloss = potSed[id] * tileo / potvol_tile;
            sedloss = min(sedloss, potSed[id]);
            potSed[id] -= sedloss;
            potsedo += sedloss;
            SED_OL[id] += sedloss;

            no3loss = pot_no3[id] * tileo / potvol_tile;
            no3loss = min(no3loss, pot_no3[id]);
            pot_no3[id] -= no3loss;
            sur_no3[id] += no3loss / cellArea;
            // if (id == 46364) cout<<", += tile loss: "<<sur_no3[id];
            nh4loss = pot_nh4[id] * tileo / potvol_tile;
            nh4loss = min(nh4loss, pot_nh4[id]);
            pot_nh4[id] -= nh4loss;
            sur_nh4[id] += nh4loss / cellArea;

            solploss = pot_solp[id] * tileo / potvol_tile;
            solploss = min(solploss, pot_solp[id]);
            pot_solp[id] -= solploss;
            sur_solp[id] += solploss / cellArea;

            orgnloss = pot_orgn[id] * tileo / potvol_tile;
            orgnloss = min(orgnloss, pot_orgn[id]);
            pot_orgn[id] -= orgnloss;
            sedorgn[id] += orgnloss / cellArea;

            orgploss = potOrgP[id] * tileo / potvol_tile;
            orgploss = min(orgploss, potOrgP[id]);
            potOrgP[id] -= orgploss;
            sedorgp[id] += orgploss / cellArea;

            minpsloss = potStaMinP[id] * tileo / potvol_tile;
            minpsloss = min(minpsloss, potStaMinP[id]);
            potStaMinP[id] -= minpsloss;
            sedminps[id] += minpsloss / cellArea;

            minpaloss = potActMinP[id] * tileo / potvol_tile;
            minpaloss = min(minpaloss, potActMinP[id]);
            potActMinP[id] -= minpaloss;
            sedminpa[id] += minpaloss / cellArea;

            sanloss = potSand[id] * tileo / potvol_tile;
            potSand[id] -= sanloss;
            potsano += sanloss;
            sand_yld[id] += sanloss;

            silloss = potSilt[id] * tileo / potvol_tile;
            potSilt[id] -= silloss;
            potsilo += silloss;
            silt_yld[id] += silloss;

            claloss = potClay[id] * tileo / potvol_tile;
            potClay[id] -= claloss;
            potclao += claloss;
            clay_yld[id] += claloss;

            sagloss = potSag[id] * tileo / potvol_tile;
            potSag[id] -= sagloss;
            potsago += sagloss;
            sag_yld[id] += sagloss;

            lagloss = potLag[id] * tileo / potvol_tile;
            potLag[id] -= lagloss;
            potlago += lagloss;
            lag_yld[id] += lagloss;
        }
        if (potvol_sep > UTIL_ZERO) {
            float lossRatio = potsep / potvol_sep;
            sedloss = potSed[id] * lossRatio;
            sedloss = min(sedloss, potSed[id]);
            potSed[id] -= sedloss;

            no3loss = pot_no3[id] * lossRatio;
            no3loss = min(no3loss, pot_no3[id]);
            pot_no3[id] -= no3loss;
            // if (id == 46364) cout<<", loss ratio: "<<lossRatio<<", no3 loss from seepage: "<<no3loss<<endl;
            nh4loss = pot_nh4[id] * lossRatio;
            nh4loss = min(nh4loss, pot_nh4[id]);
            pot_nh4[id] -= nh4loss;

            solploss = pot_solp[id] * lossRatio;
            solploss = min(solploss, pot_solp[id]);
            pot_solp[id] -= solploss;

            orgnloss = pot_orgn[id] * lossRatio;
            orgnloss = min(orgnloss, pot_orgn[id]);
            pot_orgn[id] -= orgnloss;

            orgploss = potOrgP[id] * lossRatio;
            orgploss = min(orgploss, potOrgP[id]);
            potOrgP[id] -= orgploss;

            minpsloss = potStaMinP[id] * lossRatio;
            minpsloss = min(minpsloss, potStaMinP[id]);
            potStaMinP[id] -= minpsloss;

            minpaloss = potActMinP[id] * lossRatio;
            minpaloss = min(minpaloss, potActMinP[id]);
            potActMinP[id] -= minpaloss;

            sanloss = potSand[id] * lossRatio;
            potSand[id] -= sanloss;

            silloss = potSilt[id] * lossRatio;
            potSilt[id] -= silloss;

            claloss = potClay[id] * lossRatio;
            potClay[id] -= claloss;

            sagloss = potSag[id] * lossRatio;
            potSag[id] -= sagloss;

            lagloss = potLag[id] * lossRatio;
            potLag[id] -= lagloss;
        }
    }
    // force to auto-irrigation at the end of the day, added by LJ.
    if (pot_vol[id] < UTIL_ZERO) {
        pot_vol[id] = pot_vollowmm[id];
    }
    //potholeSurfaceArea(id);
    OL_Flow[id] = qdayTmp;
    //if (id == 46364)  /// dianbu 46364, dianbu2 1085
    //	cout<<"surfaceQ: "<<OL_Flow[id]<<", potVol: "<<pot_vol[id]<<
    //	", potNh4: "<<pot_nh4[id]<<", surqNh4: "<<sur_nh4[id]<<endl;
}

void IMP_SWAT::potholeSurfaceArea(int id) {
    /// compute surface area assuming a cone shape, ha
    float potVol_m3 = pot_vol[id] * cnv;
    pot_sa[id] = PI * pow((3.f * potVol_m3 / (PI * slope[id])), 0.6666f);
    pot_sa[id] /= 10000.f; /// convert to ha
    if (pot_sa[id] <= UTIL_ZERO) {
        pot_sa[id] = 0.001f;
    }
    if (pot_sa[id] > cellArea) {
        pot_sa[id] = cellArea;
    }
}

void IMP_SWAT::releaseWater(int id) {
    float proption = 1.f;
    float xx = proption * cellArea;
    if (pot_vol[id] < UTIL_ZERO) {
        return;
    }
    OL_Flow[id] += pot_vol[id] * proption;
    pot_vol[id] *= (1.f - proption);
    if (potSed[id] < UTIL_ZERO) {
        potSed[id] = 0.f;
        sand_yld[id] = 0.f;
        silt_yld[id] = 0.f;
        clay_yld[id] = 0.f;
        sag_yld[id] = 0.f;
        lag_yld[id] = 0.f;
    } else {
        SED_OL[id] += potSed[id] * proption;
        sand_yld[id] += potSand[id] * proption;
        silt_yld[id] += potSilt[id] * proption;
        clay_yld[id] += potClay[id] * proption;
        sag_yld[id] += potSag[id] * proption;
        lag_yld[id] += potLag[id] * proption;
        potSed[id] *= (1.f - proption);
        potSand[id] *= (1.f - proption);
        potSilt[id] *= (1.f - proption);
        potClay[id] *= (1.f - proption);
        potSag[id] *= (1.f - proption);
        potLag[id] *= (1.f - proption);
    }
    if (pot_no3[id] < UTIL_ZERO) {
        pot_no3[id] = 0.f;
    } else {
        sur_no3[id] += pot_no3[id] * xx;
        // if (id == 46364) cout<<", release: "<<sur_no3[id]<<endl;
        pot_no3[id] *= (1.f - proption);
    }
    if (pot_nh4[id] < UTIL_ZERO) {
        pot_nh4[id] = 0.f;
    } else {
        sur_nh4[id] += pot_nh4[id] * xx;
        pot_nh4[id] *= (1.f - proption);
    }
    if (pot_solp[id] < UTIL_ZERO) {
        pot_solp[id] = 0.f;
    } else {
        sur_solp[id] += pot_solp[id] * xx;
        pot_solp[id] *= (1.f - proption);
    }
    if (pot_orgn[id] < UTIL_ZERO) {
        pot_orgn[id] = 0.f;
    } else {
        sedorgn[id] += pot_orgn[id] * xx;
        pot_orgn[id] *= (1.f - proption);
    }
    if (potOrgP[id] < UTIL_ZERO) {
        potOrgP[id] = 0.f;
    } else {
        sedorgp[id] += potOrgP[id] * xx;
        potOrgP[id] *= (1.f - proption);
    }
    if (potActMinP[id] < UTIL_ZERO) {
        potActMinP[id] = 0.f;
    } else {
        sedminpa[id] += potActMinP[id] * xx;
        potActMinP[id] *= (1.f - proption);
    }
    if (potStaMinP[id] < UTIL_ZERO) {
        potStaMinP[id] = 0.f;
    } else {
        sedminps[id] += potStaMinP[id] * xx;
        potStaMinP[id] *= (1.f - proption);
    }
    /// Debugging: dianbu 46364, dianbu2 1085
    // if (id == 46364) cout<<"releaseWater, "<<OL_Flow[id]<<", "<<pot_vol[id]<<", surqNh4: "<<sur_nh4[id]<<endl;
}

void IMP_SWAT::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_POT_VOL)) { *data = pot_vol; }
    else if (StringMatch(sk, VAR_POT_SA)) { *data = pot_sa; }
    else if (StringMatch(sk, VAR_POT_NO3)) { *data = pot_no3; }
    else if (StringMatch(sk, VAR_POT_NH4)) { *data = pot_nh4; }
    else if (StringMatch(sk, VAR_POT_SOLP)) { *data = pot_solp; }
    else {
        throw ModelException(MID_IMP_SWAT, "Get1DData", "Parameter" + sk + "does not exist.");
    }
    *n = m_nCells;
    return;
}
