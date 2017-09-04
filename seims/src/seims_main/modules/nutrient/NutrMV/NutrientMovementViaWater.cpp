#include "seims.h"
#include "NutrientMovementViaWater.h"

using namespace std;

NutrientMovementViaWater::NutrientMovementViaWater(void) :
//input
    m_nCells(-1), CELLWIDTH(-1.f), cellArea(-1.f), nSoiLayers(-1), SED_OL(NULL), nperco(-1.f),
    phoskd(-1.f), pperco(-1.f), cod_n(-1.f),
    cod_k(-1.f), qtile(-1.f), soillayers(NULL), anion_excl(NULL), isep_opt(-1), ldrain(NULL),
    dist2stream(NULL), OL_Flow(NULL), SSRU(NULL),
    Perco(NULL), sol_ul(NULL), sol_crk(NULL), density(NULL), soilDepth(NULL), soilthick(NULL),
    sol_cbn(NULL), sol_no3(NULL), sol_solp(NULL),
    nSubbasins(-1), subbasin(NULL), m_subbasinsInfo(NULL), STREAM_LINK(NULL),
    ROUTING_LAYERS(NULL), nRoutingLayers(-1), FLOWOUT_INDEX_D8(NULL),
    sedc(NULL), conv_wt(NULL),
    //output
    latno3(NULL), latno3ToCh(NULL), wshd_plch(-1.f),
    sur_no3(NULL), sur_nh4(NULL), sur_solp(NULL), sur_cod(NULL), chl_a(NULL),
    perco_n(NULL), perco_p(NULL), perco_n_gw(NULL), perco_p_gw(NULL),
    sur_no3_ToCh(NULL), sur_nh4ToCh(NULL), sur_solpToCh(NULL), sur_codToCh(NULL) {
}

NutrientMovementViaWater::~NutrientMovementViaWater(void) {
    if (latno3 != NULL) Release1DArray(latno3);
    if (latno3ToCh != NULL) Release1DArray(latno3ToCh);

    if (sur_no3 != NULL) Release1DArray(sur_no3);
    if (sur_nh4 != NULL) Release1DArray(sur_nh4);
    if (sur_solp != NULL) Release1DArray(sur_solp);
    if (sur_cod != NULL) Release1DArray(sur_cod);
    if (chl_a != NULL) Release1DArray(chl_a);

    if (sur_no3_ToCh != NULL) Release1DArray(sur_no3_ToCh);
    if (sur_nh4ToCh != NULL) Release1DArray(sur_nh4ToCh);
    if (sur_solpToCh != NULL) Release1DArray(sur_solpToCh);
    if (sur_codToCh != NULL) Release1DArray(sur_codToCh);

    if (perco_n != NULL) Release1DArray(perco_n);
    if (perco_p != NULL) Release1DArray(perco_p);
    if (perco_n_gw != NULL) Release1DArray(perco_n_gw);
    if (perco_p_gw != NULL) Release1DArray(perco_p_gw);
}

void NutrientMovementViaWater::SumBySubbasin() {
    // reset to zero
    for (int subi = 0; subi <= nSubbasins; subi++) {
        latno3ToCh[subi] = 0.f;
        sur_no3_ToCh[subi] = 0.f;
        sur_nh4ToCh[subi] = 0.f;
        sur_solpToCh[subi] = 0.f;
        sur_codToCh[subi] = 0.f;
        perco_n_gw[subi] = 0.f;
        perco_p_gw[subi] = 0.f;
    }

    // sum by subbasin
    for (int i = 0; i < m_nCells; i++) {
        //add today's flow
        int subi = (int) subbasin[i];
        if (nSubbasins == 1) {
            subi = 1;
        } else if (subi >= nSubbasins + 1) {
            throw ModelException(MID_NUTRMV, "Execute", "The subbasin ID " + ValueToString(subi) + " is invalid.");
        }

        sur_no3_ToCh[subi] += sur_no3[i] * cellArea; // kg/ha * ha = kg
        sur_nh4ToCh[subi] += sur_nh4[i] * cellArea;
        sur_solpToCh[subi] += sur_solp[i] * cellArea;
        sur_codToCh[subi] += sur_cod[i] * cellArea;
        //if(i == 1762)
        //	cout<<"sum by subbasin: perocN: "<<perco_n[i]<<endl;
        float ratio2gw = 1.f;
        perco_n_gw[subi] += perco_n[i] * cellArea * ratio2gw;
        perco_p_gw[subi] += perco_p[i] * cellArea * ratio2gw;
        if (STREAM_LINK[i] > 0) {
            latno3ToCh[subi] += latno3[i];
        }
    }
    //cout<<"sur_codToCh: "<<sur_codToCh[2]<<endl;

    // sum all the subbasins and put the sum value in the zero-index of the array
    for (int i = 1; i < nSubbasins + 1; i++) {
        sur_no3_ToCh[0] += sur_no3_ToCh[i];
        //cout<<"subID: "<<i<<", surNo3ToCh: "<<sur_no3_ToCh[i]<<endl;
        sur_nh4ToCh[0] += sur_nh4ToCh[i];
        sur_solpToCh[0] += sur_solpToCh[i];
        sur_codToCh[0] += sur_codToCh[i];
        latno3ToCh[0] += latno3ToCh[i];
        perco_n_gw[0] += perco_n_gw[i];
        //cout<<"subID: "<<i<<", perco_n_gw: "<<perco_n_gw[i]<<endl;
        perco_p_gw[0] += perco_p_gw[i];
    }
}

bool NutrientMovementViaWater::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
                m_nCells << ".\n";
            throw ModelException(MID_NUTRMV, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrientMovementViaWater::CheckInputData() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cells number can not be less than zero.");
    }
    if (this->CELLWIDTH <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (this->nSoiLayers <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (this->soillayers == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "Soil layers number must not be NULL");
    }
    if (this->SED_OL == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The distribution of soil loss caused by water erosion can not be NULL.");
    }
    if (this->sol_cbn == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The percent organic matter in soil layer can not be NULL.");
    }
    if (this->anion_excl == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The fraction of porosity from which anions are excluded can not be NULL.");
    }
    if (this->isep_opt < 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The septic operational condition code can not be NULL.");
    }
    //if (this->ldrain == NULL)
    //{
    //    throw ModelException(MID_NUTRMV, "CheckInputData", "The soil layer where drainage tile is located can not be NULL.");
    //}
    if (this->dist2stream == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The distance to the stream data can not be NULL.");
    }
    if (this->OL_Flow == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The distribution of surface runoff generated data can not be NULL.");
    }
    if (this->nperco <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The nitrate percolation coefficient can not be less than zero.");
    }
    if (this->cod_n <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cod_n can not be less than zero.");
    }
    if (this->cod_k <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cod_k can not be less than zero.");
    }
    if (this->SSRU == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The lateral flow in soil layer data can not be NULL.");
    }
    if (this->Perco == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "percolation from soil layer can not be NULL.");
    }
    if (this->sol_ul == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The amount of water held in the soil layer at saturation data can not be NULL.");
    }
    if (this->phoskd <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "Phosphorus soil partitioning coefficient can not be less than zero.");
    }
    if (this->soilthick == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The soilthick can not be NULL.");
    }
    if (this->sol_crk == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The crack volume potential of soil data can not be NULL.");
    }
    if (this->pperco <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "Phosphorus percolation coefficient can not be less than zero.");
    }
    if (this->density == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The bulk density of the soil data can not be NULL.");
    }
    if (this->soilDepth == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The depth to bottom of soil layer can not be NULL.");
    }
    if (FLOWOUT_INDEX_D8 == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The parameter: flow out index has not been set.");
    }
    if (nSubbasins <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The subbasins number must be greater than 0.");
    }
    if (m_subbasinIDs.empty()) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The subbasin IDs can not be EMPTY.");
    }
    if (m_subbasinsInfo == NULL) {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");
    }
    return true;
}

void NutrientMovementViaWater::SetSubbasins(clsSubbasins *subbasins) {
    if (m_subbasinsInfo == NULL) {
        m_subbasinsInfo = subbasins;
        nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
        m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
    }
}

void NutrientMovementViaWater::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) value);
    }
        //else if (StringMatch(sk, Tag_CellSize)) { this->m_nCells = value; }
    else if (StringMatch(sk, Tag_CellWidth)) {
        this->CELLWIDTH = value;
        this->cellArea = CELLWIDTH * CELLWIDTH * 0.0001f; /// ha
    } else if (StringMatch(sk, VAR_QTILE)) { this->qtile = value; }
    else if (StringMatch(sk, VAR_NPERCO)) { this->nperco = value; }
    else if (StringMatch(sk, VAR_PPERCO)) { this->pperco = value; }
    else if (StringMatch(sk, VAR_PHOSKD)) { this->phoskd = value; }
    else if (StringMatch(sk, VAR_ISEP_OPT)) { this->isep_opt = value; }
    else if (StringMatch(sk, VAR_COD_N)) { this->cod_n = value; }
    else if (StringMatch(sk, VAR_COD_K)) { this->cod_k = value; }
    else if (StringMatch(sk, VAR_CSWAT)) { this->cswat = (int) value; }
    else {
        throw ModelException(MID_NUTRMV, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrientMovementViaWater::Set1DData(const char *key, int n, float *data) {
    if (!this->CheckInputSize(key, n)) return;
    string sk(key);
    if (StringMatch(sk, VAR_OLFLOW)) {
        OL_Flow = data;
    } else if (StringMatch(sk, VAR_SUBBSN)) {
        subbasin = data;
    } else if (StringMatch(sk, VAR_STREAM_LINK)) {
        STREAM_LINK = data;
    } else if (StringMatch(sk, VAR_ANION_EXCL)) {
        anion_excl = data;
    } else if (StringMatch(sk, VAR_LDRAIN)) {
        ldrain = data;
    } else if (StringMatch(sk, VAR_DISTSTREAM)) {
        dist2stream = data;
    } else if (StringMatch(sk, VAR_SOL_CRK)) {
        sol_crk = data;
    } else if (StringMatch(sk, VAR_SOILLAYERS)) {
        soillayers = data;
    } else if (StringMatch(sk, VAR_SEDYLD)) {
        this->SED_OL = data;
    } else if (StringMatch(sk, Tag_FLOWOUT_INDEX_D8)) {
        FLOWOUT_INDEX_D8 = data;
    } else if (StringMatch(sk, VAR_SEDORGN)) {
        sedorgn = data;
    } else if (StringMatch(sk, VAR_TMEAN)) {
        TMEAN = data;
    } else if (StringMatch(sk, VAR_SEDLOSS_C)) {
        sedc = data;
    } else {
        throw ModelException(MID_NUTRMV, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientMovementViaWater::Set2DData(const char *key, int nRows, int nCols, float **data) {
    string sk(key);
    if (StringMatch(sk, Tag_ROUTING_LAYERS)) {
        nRoutingLayers = nRows;
        ROUTING_LAYERS = data;
        return;
    }
    if (!this->CheckInputSize(key, nRows)) return;
    nSoiLayers = nCols;
    if (StringMatch(sk, VAR_SSRU)) { SSRU = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { density = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { sol_solp = data; }
    else if (StringMatch(sk, VAR_SOILDEPTH)) { soilDepth = data; }
    else if (StringMatch(sk, VAR_PERCO)) { Perco = data; }
    else if (StringMatch(sk, VAR_SOL_CBN)) { sol_cbn = data; }
    else if (StringMatch(sk, VAR_SOILTHICK)) { soilthick = data; }
    else if (StringMatch(sk, VAR_SOL_UL)) { sol_ul = data; }
    else if (StringMatch(sk, VAR_CONV_WT)) { conv_wt = data; }
    else {
        throw ModelException(MID_NUTRMV, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrientMovementViaWater::initialOutputs() {
    if (this->m_nCells <= 0) {
        throw ModelException(MID_NUTRMV, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    // allocate the output variables
    if (latno3 == NULL) Initialize1DArray(m_nCells, latno3, 0.f);
    if (perco_n == NULL) Initialize1DArray(m_nCells, perco_n, 0.f);
    if (perco_p == NULL) Initialize1DArray(m_nCells, perco_p, 0.f);
    if (sur_no3 == NULL) Initialize1DArray(m_nCells, sur_no3, 0.f);
    if (sur_nh4 == NULL) Initialize1DArray(m_nCells, sur_nh4, 0.f);
    if (sur_solp == NULL) Initialize1DArray(m_nCells, sur_solp, 0.f);

    if (latno3ToCh == NULL) Initialize1DArray(nSubbasins + 1, latno3ToCh, 0.f);
    if (sur_no3_ToCh == NULL) Initialize1DArray(nSubbasins + 1, sur_no3_ToCh, 0.f);
    if (sur_nh4ToCh == NULL) Initialize1DArray(nSubbasins + 1, sur_nh4ToCh, 0.f);
    if (sur_solpToCh == NULL) Initialize1DArray(nSubbasins + 1, sur_solpToCh, 0.f);
    if (sur_codToCh == NULL) Initialize1DArray(nSubbasins + 1, sur_codToCh, 0.f);
    if (perco_n_gw == NULL) Initialize1DArray(nSubbasins + 1, perco_n_gw, 0.f);
    if (perco_p_gw == NULL) Initialize1DArray(nSubbasins + 1, perco_p_gw, 0.f);

    if (sur_cod == NULL) Initialize1DArray(m_nCells, sur_cod, 0.f);
    if (chl_a == NULL) Initialize1DArray(m_nCells, chl_a, 0.f);
    if (wshd_plch < 0) wshd_plch = 0.f;

    // input variables
    if (SSRU == NULL) Initialize2DArray(m_nCells, nSoiLayers, SSRU, 0.0001f);
    if (Perco == NULL) Initialize2DArray(m_nCells, nSoiLayers, Perco, 0.0001f);
    if (ldrain == NULL) Initialize1DArray(m_nCells, ldrain, -1.f);
    qtile = 0.0001f;
}

int NutrientMovementViaWater::Execute() {
    CheckInputData();
    //int cellid = 18605;
    //cout<<"NutrMV, pre solno3: ";
    //for (int j = 0; j < (int)soillayers[cellid]; j++)
    //	cout<<j<<", "<<sol_no3[cellid][j]<<", ";
    //cout<<endl;
    if (cswat == 2) /// check input data
    {
        if (sedc == NULL) {
            throw ModelException(MID_NUTRMV, "CheckInputData", "The amount of C lost with sediment must not be NULL.");
        }
    }
    initialOutputs();
    // compute nitrate movement leaching
    //cout<<"NUTRMV-exec, sol_no3[0]: "<<sol_no3[cellid][0]<<", "<<"surqno3: "<<sur_no3[5878]<<endl;
    NitrateLoss();
    //cout<<"NUTRMV-loss, sol_no3[0]: "<<sol_no3[cellid][0]<<", "<<"surqno3: "<<sur_no3[cellid]<<endl;
    // compute phosphorus movement
    PhosphorusLoss();
    // compute chl-a, CBOD and dissolved oxygen loadings
    SubbasinWaterQuality();
    // sum by sub-basin
    SumBySubbasin();
    //cout<<"NutrMV, after solno3: ";
    //for (int j = 0; j < (int)soillayers[cellid]; j++)
    //	cout<<j<<", "<<sol_no3[cellid][j]<<", ";
    //cout<<endl;
    return 0;
}

void NutrientMovementViaWater::NitrateLoss() {
/// debugging code to find the cell id with the maximum nutrient loss 
// 	float tmpPercN = NODATA_VALUE;
// 	int tmpIdx = -1;
    for (int iLayer = 0; iLayer < nRoutingLayers; iLayer++) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nCells = (int) ROUTING_LAYERS[iLayer][0];
#pragma omp parallel for
        for (int iCell = 1; iCell <= nCells; iCell++) {
            int i = (int) ROUTING_LAYERS[iLayer][iCell]; // cell ID
            float percnlyr = 0.f;
            latno3[i] = 0.f;
            for (int k = 0; k < (int) soillayers[i]; k++) {
                // add nitrate leached from layer above (kg/ha)
                //float tmpSolNo3 = sol_no3[i][k];
                sol_no3[i][k] = sol_no3[i][k] + percnlyr;
                //percnlyr = 0.f;
                if (sol_no3[i][k] < 1.e-6f) {
                    sol_no3[i][k] = 0.f;
                }
                // determine concentration of nitrate in mobile water
                float sro = 0.f;// surface runoff generated (sro)
                float mw = 0.f;// amount of mobile water in the layer (mw)
                float vno3 = 0.f; // kg/ha
                float con = 0.f;  // 100 mg/L
                float ww = 0.f;

                if (k == 0) {
                    sro = OL_Flow[i];
                } else {
                    sro = 0.f;
                }
                if (ldrain[i] == k) {
                    mw += qtile;
                }

                // Calculate the concentration of nitrate in the mobile water (con),
                // equation 4:2.1.2, 4:2.1.3 and 4:2.1.4 in SWAT Theory 2009, p269
                mw = Perco[i][k] + sro + SSRU[i][k] + 1.e-10f;
                //if (i == 918)
                //	cout<<"sol_perco, k: "<<k<<", "<<Perco[i][k]<<", flat: "
                //	<<SSRU[i][k]<<endl;
                float satportion = ((1.f - anion_excl[i]) * sol_ul[i][k]);
                //if (mw > satportion) mw = satportion;
                ww = -mw / satportion;
                vno3 = sol_no3[i][k] * (1.f - exp(ww)); // kg/ha
                if (mw > 1.e-10f) {
                    con = max(vno3 / mw, 0.f);
                } // kg/ha/mm = 100 mg/L
                //if (con > 0.1) con = 0.1;
                //if (con != con)
                //{
                //	cout<<"cellid: "<<i<<", layer: "<<k<<", perco water: "<<Perco[i][k]<<", satportion: "<<satportion<<
                //		", mv: "<<mw<<", ww: "<<ww<<", vno3: "<<vno3<<", con(100 mg/L): "<<con
                //		<<", pre sol no3: "<<tmpSolNo3<<", sol no3: "<<sol_no3[i][k]<<endl;
                //	//throw ModelException(MID_NUTRMV, "NitrateLoss", "NAN occurs of Soil NO3, please check!");
                //}

                // calculate nitrate in surface runoff
                // concentration of nitrate in surface runoff (cosurf)
                float cosurf = 0.f;
                if (isep_opt == 2)
                    cosurf = 1.f * con; // N percolation does not apply to failing septic place;
                else
                    cosurf = nperco * con;
                if (k == 0) {
                    sur_no3[i] = OL_Flow[i] * cosurf; // kg/ha
                    sur_no3[i] = min(sur_no3[i], sol_no3[i][k]);
                    sol_no3[i][k] -= sur_no3[i];
                }
                // calculate nitrate in tile flow, TODO
                if (ldrain[i] == k) {
                }
                // nitrate moved with subsuface flow (kg/ha)
                float ssfnlyr = 0.f;
                // calculate nitrate in lateral flow
                // nitrate transported in lateral flow from layer (ssfnlyr)
                if (k == 0)
                    ssfnlyr = cosurf * SSRU[i][k];
                else
                    ssfnlyr = con * SSRU[i][k]; // 100 mg/L * mm = kg/ha
                ssfnlyr = min(ssfnlyr, sol_no3[i][k]);
                latno3[i] += ssfnlyr;
                // move the lateral no3 flow to the downslope cell (routing considered)
                sol_no3[i][k] -= ssfnlyr;
                int idDownSlope = (int) FLOWOUT_INDEX_D8[i];
                if (idDownSlope >= 0)
                    sol_no3[idDownSlope][k] += latno3[i];
                /// old code: sol_no3[idDownSlope][k] += ssfnlyr; /// changed by LJ, 16-10-13

                // calculate nitrate in percolate
                percnlyr = con * Perco[i][k];
                //if(i == 5570)
                //	cout<<"layer: "<<k<<", con: "<<con<<", sol_perco: "<<Perco[i][k]<<", solno3: "<<sol_no3[i][k]<<endl;

                percnlyr = min(percnlyr, sol_no3[i][k]);
                sol_no3[i][k] -= percnlyr;
                //if(i == 0 && k == 0) cout << percnlyr << ", \n";
            }
            // calculate nitrate leaching from soil profile
            perco_n[i] = 0.f;
            perco_n[i] = percnlyr; // percolation of the last soil layer, kg/ha
            /// debugging code
            //if (i == 46364){
            //	float percomm = Perco[i][(int)soillayers[i]-1];
            //	float perco_n_conc = 0.f;
            //	if (percomm > 0.f){
            //		perco_n_conc = 100.f * perco_n[i] / percomm; /// mg/L
            //		cout<<"perco_n: "<<perco_n[i]<<", percomm: "<<percomm<<", perco_n_conc: "<<perco_n_conc<<endl;
            //	}
            //}
// 			if (tmpPercN < percnlyr){
// 				tmpIdx = i;
// 				tmpPercN = percnlyr;
// 			}
            // I think these should be removed, because the lost nitrate
            // have been added to it's downslope cell. by LJ
            //float nloss = 0.f;
            //nloss = (2.18f * dist2stream[i] - 8.63f) / 100.f;
            //nloss = min(1.f, max(0.f, nloss));
            //latno3[i] = (1.f - nloss) * latno3[i];
        }
    }
    //tmpIdx = 5570;
    //cout<<"NUTRMV, cell index: "<<tmpIdx<<", percoN "<<perco_n[tmpIdx]<<endl;
    //for(int i = 0; i < soillayers[tmpIdx];i++)
    //	cout<<"layer: "<<i<<": "<<sol_no3[tmpIdx][i]<<", ";
    //cout<<endl;
}

void NutrientMovementViaWater::PhosphorusLoss() {
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        //float wt1 = density[i][0] * soilthick[i][0] / 100.f; // mg/kg => kg/ha
        //float conv_wt = 1.e6f * wt1; // kg/kg => kg/ha

        // amount of P leached from soil layer (vap)
        float vap = 0.f;
        float vap_tile = 0.f;
        // compute soluble P lost in surface runoff
        float xx = 0.f;  // variable to hold intermediate calculation result
        xx = density[i][0] * soilDepth[i][0] * phoskd;
        // units ==> surqsolp = [kg/ha * mm] / [t/m^3 * mm * m^3/t] = kg/ha
        sur_solp[i] = sol_solp[i][0] * OL_Flow[i] / xx;
        sur_solp[i] = min(sur_solp[i], sol_solp[i][0]);
        sur_solp[i] = max(sur_solp[i], 0.f);
        sol_solp[i][0] = sol_solp[i][0] - sur_solp[i];

        // compute soluble P leaching
        vap = sol_solp[i][0] * Perco[i][0] / ((conv_wt[i][0] / 1000.f) * pperco);
        vap = min(vap, 0.5f * sol_solp[i][0]);
        sol_solp[i][0] = sol_solp[i][0] - vap;

        // estimate soluble p in tiles due to crack flow
        if (ldrain[i] > 0) {
            xx = min(1.f, sol_crk[i] / 3.f);
            vap_tile = xx * vap;
            vap = vap - vap_tile;
        }
        if (soillayers[i] >= 2) {
            sol_solp[i][1] = sol_solp[i][1] + vap;
        }
        for (int k = 1; k < soillayers[i]; k++) {
            vap = 0.f;
            //if (k != m_i_sep[i]) {  // soil layer where biozone exists (m_i_sep)
            vap = sol_solp[i][k] * Perco[i][k] / ((conv_wt[i][k] / 1000.f) * pperco);

            vap = min(vap, 0.2f * sol_solp[i][k]);
            sol_solp[i][k] = sol_solp[i][k] - vap;

            if (k < soillayers[i] - 1) {
                sol_solp[i][k + 1] += vap;//leach to next layer
            } else {
                perco_p[i] = vap;
            }//leach to groundwater
            //}
        }
        //if(i == 100 ) cout << "sol_solp: " << sol_solp[i][0] << endl;
        /// debugging code
        //if (i == 46364){
        //	float percomm = Perco[i][(int)soillayers[i]-1];
        //	float perco_p_conc = 0.f;
        //	if (percomm > 0.f){
        //		perco_p_conc = 100.f * perco_p[i] / percomm; /// mg/L
        //		cout<<"perco_p: "<<perco_p[i]<<", percomm: "<<percomm<<", perco_p_conc: "<<perco_p_conc<<endl;
        //	}
        //}
        // summary calculation
        wshd_plch = wshd_plch + vap * (1.f / m_nCells);
    }
}

void NutrientMovementViaWater::SubbasinWaterQuality() {
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        /// Note, doxq (i.e., dissolved oxygen concentration in the surface runoff on current day)
        ///       is not included here, just because doxq will not be used anywhere.
        ///       Also, the algorithm will be executed in Qual2E model in channel process.   By LJ

        // total amount of water entering main channel on current day, mm
        float qdr = 0.f;
        qdr = OL_Flow[i] + SSRU[i][0] + qtile;
        if (qdr > 1.e-4f) {
            // kilo moles of phosphorus in nutrient loading to main channel (tp)
            float tp = 0.f;
            tp = 100.f * (sedorgn[i] + sur_no3[i]) / qdr;   //100*kg/ha/mm = ppm
            // regional adjustment on sub chla_a loading, the default value is 40
            float chla_subco = 40.f;
            chl_a[i] = chla_subco * tp;
            chl_a[i] = chl_a[i] / 1000.f;  // um/L to mg/L

            // calculate enrichment ratio
            if (SED_OL[i] < 1e-4)SED_OL[i] = 0.f;
            float enratio = NutrCommon::CalEnrichmentRatio(SED_OL[i], OL_Flow[i], cellArea);

            // calculate organic carbon loading to main channel
            float org_c = 0.f;  /// kg
            if (cswat == 2) {
                org_c = sedc[i] * cellArea;
            } else {
                org_c = (sol_cbn[i][0] / 100.f) * enratio * (SED_OL[i] / 1000.f) * 1000.f;
            }
            // calculate carbonaceous biological oxygen demand (CBOD) and COD(transform from CBOD)
            float cbodu = 2.7f * org_c / (qdr * CELLWIDTH * CELLWIDTH * 1.e-6f); //  mg/L
            // convert cbod to cod
            // The translation relationship is combined Wang Cai-Qin et al. (2014) with
            // Guo and Long (1994); Xie et al. (2000); Jin et al. (2005).
            float cod = cod_n * (cbodu * (1.f - exp(-5.f * cod_k)));
            sur_cod[i] = OL_Flow[i] / 1000.f * cod * 10.f;    // mg/L converted to kg/ha
        } else {
            chl_a[i] = 0.f;
            sur_cod[i] = 0.f;
        }
    }
}

void NutrientMovementViaWater::GetValue(const char *key, float *value) {
    string sk(key);
    if (StringMatch(sk, VAR_WSHD_PLCH)) {
        *value = this->wshd_plch;
    } else {
        throw ModelException(MID_NUTRMV, "GetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrientMovementViaWater::Get1DData(const char *key, int *n, float **data) {
    string sk(key);
    initialOutputs();
    if (StringMatch(sk, VAR_LATNO3)) {
        *data = this->latno3;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_PERCO_N_GW)) {
        *data = perco_n_gw;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_PERCO_P_GW)) {
        *data = perco_p_gw;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SUR_NO3)) {
        *data = this->sur_no3;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SUR_NH4)) {
        *data = this->sur_nh4;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SUR_SOLP)) {
        *data = this->sur_solp;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_SUR_COD)) {
        *data = this->sur_cod;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_CHL_A)) {
        *data = this->chl_a;
        *n = m_nCells;
    } else if (StringMatch(sk, VAR_LATNO3_TOCH)) {
        *data = latno3ToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SUR_NO3_TOCH)) {
        *data = sur_no3_ToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SUR_NH4_TOCH)) {
        *data = sur_nh4ToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SUR_SOLP_TOCH)) {
        *data = sur_solpToCh;
        *n = nSubbasins + 1;
    } else if (StringMatch(sk, VAR_SUR_COD_TOCH)) {
        *data = sur_codToCh;
        *n = nSubbasins + 1;
    } else {
        throw ModelException(MID_NUTRMV, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}
//void NutrientMovementViaWater::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
//{
//	initialOutputs();
//    string sk(key);
//    *nRows = m_nCells;
//    *nCols = nSoiLayers;
//    if (StringMatch(sk, VAR_SOL_NO3))
//    {
//        *data = this->sol_no3;
//    }
//    else if (StringMatch(sk, VAR_SOL_SOLP))
//    {
//        *data = this->sol_solp;
//    }
//    else
//        throw ModelException(MID_NUTRMV, "Get2DData", "Output " + sk+" does not exist.");
//}
