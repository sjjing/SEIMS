#include "seims.h"
#include "NutrCH_QUAL2E.h"

using namespace std;

NutrCH_QUAL2E::NutrCH_QUAL2E(void) :
//input
    m_nCells(-1), DT_CH(-1), QUPREACH(0.f), rnum1(0.f), igropt(-1),
    ai0(-1.f), ai1(-1.f), ai2(-1.f), ai3(-1.f), ai4(-1.f), ai5(-1.f),
    ai6(-1.f), lambda0(-1.f), lambda1(-1.f), lambda2(-1.f), cod_n(-1), cod_k(-1),
    k_l(-1.f), k_n(-1.f), k_p(-1.f), p_n(-1.f), tfact(-1.f), mumax(-1.f), rhoq(-1.f),
    STREAM_LINK(NULL), SOTE(NULL), daylength(NULL), SR(NULL), 
    BKST(NULL), chOrder(NULL), QRECH(NULL), CHST(NULL), preCHST(NULL),
    CHWTDEPTH(NULL), prechwtdepth(NULL), wattemp(NULL),
    bc1(NULL), bc2(NULL), bc3(NULL), bc4(NULL),
    rs1(NULL), rs2(NULL), rs3(NULL), rs4(NULL), rs5(NULL),
    rk1(NULL), rk2(NULL), rk3(NULL), rk4(NULL),
    latno3ToCh(NULL), sur_no3_ToCh(NULL), SUR_NH4_TOCH(NULL), sur_solp_ToCh(NULL), no3gwToCh(NULL),
    minpgwToCh(NULL), sedorgnToCh(NULL), sedorgpToCh(NULL), sedminpaToCh(NULL),
    sedminpsToCh(NULL), nitriteToCh(NULL), sur_cod_ToCH(NULL),
    chSr(NULL), chDaylen(NULL), chCellCount(NULL),
    // reaches related
    reachDownStream(NULL), ch_onco(NODATA_VALUE), ch_opco(NODATA_VALUE),
    // point source loadings to channel
    ptNO3ToCh(NULL), ptNH4ToCh(NULL), ptOrgNToCh(NULL), ptTNToCh(NULL),
    ptSolPToCh(NULL), ptOrgPToCh(NULL), ptTPToCh(NULL), ptCODToCh(NULL),
    // channel erosion
    rch_deg(NULL),
    // nutrient storage in channel
    chAlgae(NULL), chOrgN(NULL), CHSTR_NH4(NULL), chNO2(NULL), CHSTR_NO3(NULL), CHSTR_TN(NULL),
    chOrgP(NULL), chSolP(NULL), CHSTR_TP(NULL), chCOD(NULL), chDOx(NULL), chChlora(NULL),
    soxy(NODATA_VALUE),
    // nutrient amount outputs of channels
    CH_chlora(NULL), ch_algae(NULL), CH_ORGN(NULL), CH_ORGP(NULL), ch_nh4(NULL),
    CH_NO2(NULL), CH_NO3(NULL), CH_SOLP(NULL), ch_dox(NULL), CH_COD(NULL), 
    CH_TN(NULL), CH_TP(NULL),
    // nutrient concentration outputs of channels
    ch_algaeConc(NULL), CH_chloraConc(NULL), CH_ORGNConc(NULL), CH_ORGPConc(NULL),
    ch_nh4Conc(NULL), CH_NO2Conc(NULL), CH_NO3Conc(NULL), CH_SOLPConc(NULL), CH_CODConc(NULL),
    ch_doxConc(NULL), CH_TNConc(NULL), CH_TPConc(NULL) {
}

NutrCH_QUAL2E::~NutrCH_QUAL2E(void) {
    /// reach parameters
    if (chCellCount != NULL) Release1DArray(chCellCount);
    if (reachDownStream != NULL) Release1DArray(reachDownStream);
    if (chOrder != NULL) Release1DArray(chOrder);
    if (bc1 != NULL) Release1DArray(bc1);
    if (bc2 != NULL) Release1DArray(bc2);
    if (bc3 != NULL) Release1DArray(bc3);
    if (bc4 != NULL) Release1DArray(bc4);
    if (rk1 != NULL) Release1DArray(rk1);
    if (rk2 != NULL) Release1DArray(rk2);
    if (rk3 != NULL) Release1DArray(rk3);
    if (rk4 != NULL) Release1DArray(rk4);
    if (rs1 != NULL) Release1DArray(rs1);
    if (rs2 != NULL) Release1DArray(rs2);
    if (rs3 != NULL) Release1DArray(rs3);
    if (rs4 != NULL) Release1DArray(rs4);
    if (rs5 != NULL) Release1DArray(rs5);

    if (ptNO3ToCh != NULL) Release1DArray(ptNO3ToCh);
    if (ptNH4ToCh != NULL) Release1DArray(ptNH4ToCh);
    if (ptOrgNToCh != NULL) Release1DArray(ptOrgNToCh);
    if (ptTNToCh != NULL) Release1DArray(ptTNToCh);
    if (ptSolPToCh != NULL) Release1DArray(ptSolPToCh);
    if (ptOrgPToCh != NULL) Release1DArray(ptOrgPToCh);
    if (ptTPToCh != NULL) Release1DArray(ptTPToCh);
    if (ptCODToCh != NULL) Release1DArray(ptCODToCh);
    /// storage in channel
    if (chAlgae != NULL) Release1DArray(chAlgae);
    if (chOrgN != NULL) Release1DArray(chOrgN);
    if (CHSTR_NH4 != NULL) Release1DArray(CHSTR_NH4);
    if (chNO2 != NULL) Release1DArray(chNO2);
    if (CHSTR_NO3 != NULL) Release1DArray(CHSTR_NO3);
    if (CHSTR_TN != NULL) Release1DArray(CHSTR_TN);
    if (chOrgP != NULL) Release1DArray(chOrgP);
    if (chSolP != NULL) Release1DArray(chSolP);
    if (CHSTR_TP != NULL) Release1DArray(CHSTR_TP);
    if (chCOD != NULL) Release1DArray(chCOD);
    if (chDOx != NULL) Release1DArray(chDOx);
    if (chChlora != NULL) Release1DArray(chChlora);
    /// amount out of channel
    if (CH_chlora != NULL) Release1DArray(CH_chlora);
    if (ch_algae != NULL) Release1DArray(ch_algae);
    if (CH_ORGN != NULL) Release1DArray(CH_ORGN);
    if (CH_ORGP != NULL) Release1DArray(CH_ORGP);
    if (ch_nh4 != NULL) Release1DArray(ch_nh4);
    if (CH_NO2 != NULL) Release1DArray(CH_NO2);
    if (CH_NO3 != NULL) Release1DArray(CH_NO3);
    if (CH_SOLP != NULL) Release1DArray(CH_SOLP);
    if (ch_dox != NULL) Release1DArray(ch_dox);
    if (CH_COD != NULL) Release1DArray(CH_COD);
    if (CH_TN != NULL) Release1DArray(CH_TN);
    if (CH_TP != NULL) Release1DArray(CH_TP);
    /// concentration out of channel
    if (CH_chloraConc != NULL) Release1DArray(CH_chloraConc);
    if (ch_algaeConc != NULL) Release1DArray(ch_algaeConc);
    if (CH_ORGNConc != NULL) Release1DArray(CH_ORGNConc);
    if (CH_ORGPConc != NULL) Release1DArray(CH_ORGPConc);
    if (ch_nh4Conc != NULL) Release1DArray(ch_nh4Conc);
    if (CH_NO2Conc != NULL) Release1DArray(CH_NO2Conc);
    if (CH_NO3Conc != NULL) Release1DArray(CH_NO3Conc);
    if (CH_SOLPConc != NULL) Release1DArray(CH_SOLPConc);
    if (ch_doxConc != NULL) Release1DArray(ch_doxConc);
    if (CH_CODConc != NULL) Release1DArray(CH_CODConc);
    if (CH_TNConc != NULL) Release1DArray(CH_TNConc);
    if (CH_TPConc != NULL) Release1DArray(CH_TPConc);
}

void NutrCH_QUAL2E::ParametersSubbasinForChannel() {
    if (chCellCount == NULL) {
        Initialize1DArray(nReaches + 1, chCellCount, 0);
    }
    if (chDaylen == NULL) {
        Initialize1DArray(nReaches + 1, chDaylen, 0.f);
        Initialize1DArray(nReaches + 1, chSr, 0.f);
        Initialize1DArray(nReaches + 1, wattemp, 0.f);
    } else {
        return;
    }
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        if (STREAM_LINK[i] <= 0.f) {
            continue;
        }
        int subi = (int) STREAM_LINK[i];
        if (nReaches == 1) {
            subi = 1;
        } else if (subi >= nReaches + 1) {
            throw ModelException(MID_NUTRCH_QUAL2E, "Execute", "The subbasin " + ValueToString(subi) + " is invalid.");
        }

        chDaylen[subi] += daylength[i];
        chSr[subi] += SR[i];
        wattemp[subi] += SOTE[i];
        chCellCount[subi] += 1;
    }

    for (int i = 1; i <= nReaches; i++) {
        chDaylen[i] /= chCellCount[i];
        chSr[i] /= chCellCount[i];
        wattemp[i] /= chCellCount[i];
    }
}

bool NutrCH_QUAL2E::CheckInputCellSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRCH_QUAL2E, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n) {
        if (m_nCells <= 0) {
            m_nCells = n;
        } else {
            throw ModelException(MID_NUTRCH_QUAL2E, "CheckInputCellSize", "Input data for " + string(key) +
                " is invalid with size: " + ValueToString(n) +
                ". The origin size is " +
                ValueToString(m_nCells) + ".\n");
        }
    }
    return true;
}

bool NutrCH_QUAL2E::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_NUTRCH_QUAL2E, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (nReaches != n - 1) {
        if (nReaches <= 0) {
            nReaches = n - 1;
        } else {
            throw ModelException(MID_NUTRCH_QUAL2E, "CheckInputSize", "Input data for " + string(key) +
                " is invalid with size: " + ValueToString(n) +
                ". The origin size is " +
                ValueToString(nReaches) + ".\n");
        }
    }
    return true;
}

bool NutrCH_QUAL2E::CheckInputData() {
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, DT_CH, "TIMESTEP");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, nReaches, "nReaches");
    CHECK_DATA(MID_NUTRCH_QUAL2E, QUPREACH < 0.f, QUPREACH, "should greater or equal than 0.");
    //CHECK_POSITIVE(MID_NUTRCH_QUAL2E, QUPREACH, "QUPREACH");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, rnum1, "rnum1");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, igropt, "igropt");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, cod_n, "cod_n");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, cod_k, "cod_k");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai0, "ai0");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai1, "ai1");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai2, "ai2");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai3, "ai3");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai4, "ai4");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai5, "ai5");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, ai6, "ai6");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, lambda0, "lambda0");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, lambda1, "lambda1");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, lambda2, "lambda2");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, k_l, "k_l");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, k_n, "k_n");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, k_p, "k_p");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, p_n, "p_n");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, tfact, "tfact");
    CHECK_POSITIVE(MID_NUTRCH_QUAL2E, mumax, "mumax");
    //CHECK_POSITIVE(MID_NutCHRout, m_rhoqv, "m_rhoqv");

    CHECK_POINTER(MID_NUTRCH_QUAL2E, daylength, "daylength");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, SR, "SR");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, QRECH, "QRECH");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, CHST, "CHST");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, preCHST, "preCHST");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, CHWTDEPTH, "CHWTDEPTH");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, latno3ToCh, "latno3ToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sur_no3_ToCh, "sur_no3_ToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sur_solp_ToCh, "sur_solp_ToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sur_cod_ToCH, "m_codToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, no3gwToCh, "no3gwToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, minpgwToCh, "minpgwToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sedorgnToCh, "sedorgnToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sedorgpToCh, "sedorgpToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sedminpaToCh, "sedminpaToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, sedminpsToCh, "sedminpsToCh");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, STREAM_LINK, "STREAM_LINK");
    CHECK_POINTER(MID_NUTRCH_QUAL2E, SOTE, "SOTE");
    return true;
}

void NutrCH_QUAL2E::SetValue(const char *key, float value) {
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) { SetOpenMPThread((int) value); }
    else if (StringMatch(sk, Tag_ChannelTimeStep)) { DT_CH = (int) value; }
    else if (StringMatch(sk, VAR_QUPREACH)) { QUPREACH = value; }
    else if (StringMatch(sk, VAR_RNUM1)) { rnum1 = value; }
    else if (StringMatch(sk, VAR_IGROPT)) { igropt = (int) value; }
    else if (StringMatch(sk, VAR_COD_N)) { cod_n = value; }
    else if (StringMatch(sk, VAR_COD_K)) { cod_k = value; }
    else if (StringMatch(sk, VAR_AI0)) { ai0 = value; }
    else if (StringMatch(sk, VAR_AI1)) { ai1 = value; }
    else if (StringMatch(sk, VAR_AI2)) { ai2 = value; }
    else if (StringMatch(sk, VAR_AI3)) { ai3 = value; }
    else if (StringMatch(sk, VAR_AI4)) { ai4 = value; }
    else if (StringMatch(sk, VAR_AI5)) { ai5 = value; }
    else if (StringMatch(sk, VAR_AI6)) { ai6 = value; }
    else if (StringMatch(sk, VAR_LAMBDA0)) { lambda0 = value; }
    else if (StringMatch(sk, VAR_LAMBDA1)) { lambda1 = value; }
    else if (StringMatch(sk, VAR_LAMBDA2)) { lambda2 = value; }
    else if (StringMatch(sk, VAR_K_L)) {
        k_l = value;
        //convert units on k_l:read in as kJ/(m2*min), use as MJ/(m2*hr)
        k_l = k_l * 1.e-3f * 60.f;
    } else if (StringMatch(sk, VAR_K_N)) { k_n = value; }
    else if (StringMatch(sk, VAR_K_P)) { k_p = value; }
    else if (StringMatch(sk, VAR_P_N)) { p_n = value; }
    else if (StringMatch(sk, VAR_TFACT)) { tfact = value; }
    else if (StringMatch(sk, VAR_MUMAX)) { mumax = value; }
    else if (StringMatch(sk, VAR_RHOQ)) { rhoq = value; }
    else if (StringMatch(sk, VAR_CH_ONCO)) { ch_onco = value; }
    else if (StringMatch(sk, VAR_CH_OPCO)) { ch_opco = value; }
    else {
        throw ModelException(MID_NUTRCH_QUAL2E, "SetValue", "Parameter " + sk + " does not exist.");
    }
}

void NutrCH_QUAL2E::Set1DData(const char *key, int n, float *data) {
    string sk(key);
    if (StringMatch(sk, VAR_DAYLEN)) {
        if (!CheckInputCellSize(key, n)) {
            return;
        }
        daylength = data;
        return;
    } else if (StringMatch(sk, DataType_SolarRadiation)) {
        if (!CheckInputCellSize(key, n)) {
            return;
        }
        SR = data;
        return;
    } else if (StringMatch(sk, VAR_STREAM_LINK)) {
        if (!CheckInputCellSize(key, n)) {
            return;
        }
        STREAM_LINK = data;
        return;
    } else if (StringMatch(sk, VAR_SOTE)) {
        if (!CheckInputCellSize(key, n)) {
            return;
        }
        SOTE = data;
        return;
    }

    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_BKST)) { BKST = data; }
    else if (StringMatch(sk, VAR_QRECH)) { QRECH = data; }
    else if (StringMatch(sk, VAR_CHST)) {
        CHST = data;
    } else if (StringMatch(sk, VAR_PRECHST)) {
        preCHST = data;
        for (int i = 0; i <= nReaches; i++) {
            // input from SetReaches(), unit is mg/L, need to be converted to kg
            float cvt_conc2amount = preCHST[i] / 1000.f;
            chAlgae[i] *= cvt_conc2amount;
            chOrgN[i] *= cvt_conc2amount;
            chOrgP[i] *= cvt_conc2amount;
            CHSTR_NH4[i] *= cvt_conc2amount;
            chNO2[i] *= cvt_conc2amount;
            CHSTR_NO3[i] *= cvt_conc2amount;
            chSolP[i] *= cvt_conc2amount;
            chDOx[i] *= cvt_conc2amount;
            chCOD[i] *= cvt_conc2amount;
        }
    } else if (StringMatch(sk, VAR_CHWTDEPTH)) { CHWTDEPTH = data; }
    else if (StringMatch(sk, VAR_PRECHWTDEPTH)) { prechwtdepth = data; }
    else if (StringMatch(sk, VAR_WATTEMP)) { wattemp = data; }

    else if (StringMatch(sk, VAR_LATNO3_TOCH)) { latno3ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_NO3_TOCH)) { sur_no3_ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_NH4_TOCH)) { SUR_NH4_TOCH = data; }
    else if (StringMatch(sk, VAR_SUR_SOLP_TOCH)) { sur_solp_ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_COD_TOCH)) { sur_cod_ToCH = data; }
    else if (StringMatch(sk, VAR_NO3GW_TOCH)) { no3gwToCh = data; }
    else if (StringMatch(sk, VAR_MINPGW_TOCH)) { minpgwToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGN_TOCH)) { sedorgnToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGP_TOCH)) { sedorgpToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPA_TOCH)) { sedminpaToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPS_TOCH)) { sedminpsToCh = data; }
    else if (StringMatch(sk, VAR_NO2_TOCH)) { nitriteToCh = data; }
    else if (StringMatch(sk, VAR_RCH_DEG)) { rch_deg = data; }
    else {
        throw ModelException(MID_NUTRCH_QUAL2E, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrCH_QUAL2E::SetReaches(clsReaches *reaches) {
    if (reaches != NULL) {
        nReaches = reaches->GetReachNumber();
        m_reachId = reaches->GetReachIDs();
        if (reachDownStream == NULL) {
            Initialize1DArray(nReaches + 1, reachDownStream, 0.f);
            Initialize1DArray(nReaches + 1, chOrder, 0.f);
            /// initialize reach related parameters
            Initialize1DArray(nReaches + 1, bc1, 0.f);
            Initialize1DArray(nReaches + 1, bc2, 0.f);
            Initialize1DArray(nReaches + 1, bc3, 0.f);
            Initialize1DArray(nReaches + 1, bc4, 0.f);
            Initialize1DArray(nReaches + 1, rk1, 0.f);
            Initialize1DArray(nReaches + 1, rk2, 0.f);
            Initialize1DArray(nReaches + 1, rk3, 0.f);
            Initialize1DArray(nReaches + 1, rk4, 0.f);
            Initialize1DArray(nReaches + 1, rs1, 0.f);
            Initialize1DArray(nReaches + 1, rs2, 0.f);
            Initialize1DArray(nReaches + 1, rs3, 0.f);
            Initialize1DArray(nReaches + 1, rs4, 0.f);
            Initialize1DArray(nReaches + 1, rs5, 0.f);
        }
        if (chOrgN == NULL) {
            Initialize1DArray(nReaches + 1, chAlgae, 0.f);
            Initialize1DArray(nReaches + 1, chOrgN, 0.f);
            Initialize1DArray(nReaches + 1, CHSTR_NH4, 0.f);
            Initialize1DArray(nReaches + 1, chNO2, 0.f);
            Initialize1DArray(nReaches + 1, CHSTR_NO3, 0.f);
            Initialize1DArray(nReaches + 1, CHSTR_TN, 0.f);
            Initialize1DArray(nReaches + 1, chOrgP, 0.f);
            Initialize1DArray(nReaches + 1, chSolP, 0.f);
            Initialize1DArray(nReaches + 1, CHSTR_TP, 0.f);
            Initialize1DArray(nReaches + 1, chCOD, 0.f);
            Initialize1DArray(nReaches + 1, chDOx, 0.f);
            Initialize1DArray(nReaches + 1, chChlora, 0.f);
        }
        for (vector<int>::iterator it = m_reachId.begin(); it != m_reachId.end(); it++) {
            int i = *it;
            clsReach *tmpReach = reaches->GetReachByID(i);
            reachDownStream[i] = (float) tmpReach->GetDownStream();
            chOrder[i] = (float) tmpReach->GetUpDownOrder();
            bc1[i] = tmpReach->GetBc1();
            bc2[i] = tmpReach->GetBc2();
            bc3[i] = tmpReach->GetBc3();
            bc4[i] = tmpReach->GetBc4();
            rk1[i] = tmpReach->GetRk1();
            rk2[i] = tmpReach->GetRk2();
            rk3[i] = tmpReach->GetRk3();
            rk4[i] = tmpReach->GetRk4();
            rs1[i] = tmpReach->GetRs1();
            rs2[i] = tmpReach->GetRs2();
            rs3[i] = tmpReach->GetRs3();
            rs4[i] = tmpReach->GetRs4();
            rs5[i] = tmpReach->GetRs5();
            /// these parameters' unit is mg/L now, and will be converted to kg in Set1DData.
            chAlgae[i] = tmpReach->GetAlgae();
            chOrgN[i] = tmpReach->GetOrgN();
            chOrgP[i] = tmpReach->GetOrgP();
            CHSTR_NH4[i] = tmpReach->GetNH4();
            chNO2[i] = tmpReach->GetNO2();
            CHSTR_NO3[i] = tmpReach->GetNO3();
            chSolP[i] = tmpReach->GetSolP();
            chDOx[i] = tmpReach->GetDisOxygen();
            chCOD[i] = tmpReach->GetCOD();
        }

        m_reachUpStream.resize(nReaches + 1);
        if (nReaches > 1) {
            for (int i = 1; i <= nReaches; i++)// index of the reach is the equal to streamlink ID(1 to maxReachOrder)
            {
                int downStreamId = int(reachDownStream[i]);
                if (downStreamId <= 0) {
                    continue;
                }
                m_reachUpStream[downStreamId].push_back(i);
            }
        }
    } else {
        throw ModelException(MID_NUTRCH_QUAL2E, "SetReaches", "The reaches input can not to be NULL.");
    }
}

void NutrCH_QUAL2E::SetScenario(Scenario *sce) {
    if (sce != NULL) {
        map<int, BMPFactory *> tmpBMPFactories = sce->GetBMPFactories();
        for (map<int, BMPFactory *>::iterator it = tmpBMPFactories.begin(); it != tmpBMPFactories.end(); it++) {
            /// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
            if (it->first / 100000 == BMP_TYPE_POINTSOURCE) {
                m_ptSrcFactory[it->first] = (BMPPointSrcFactory *) it->second;
            }
        }
    } else {
        throw ModelException(MID_NUTRCH_QUAL2E, "SetScenario", "The scenario can not to be NULL.");
    }
}

void NutrCH_QUAL2E::initialOutputs() {
    if (nReaches <= 0) {
        throw ModelException(MID_NUTRCH_QUAL2E, "initialOutputs", "Reaches data should be set.");
    }

    if (m_reachLayers.empty()) {
        CheckInputData();
        for (int i = 1; i <= nReaches; i++) {
            if (chOrder == NULL) {
                throw ModelException(MID_NUTRCH_QUAL2E, "initialOutputs",
                                     "Stream order is not loaded successful from Reach table.");
            }
            int order = (int) chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }

    if (ch_algae == NULL) {
        soxy = 0.f;
        Initialize1DArray(nReaches + 1, CH_chlora, 0.f);
        Initialize1DArray(nReaches + 1, ch_algae, 0.f);
        Initialize1DArray(nReaches + 1, CH_ORGN, 0.f);
        Initialize1DArray(nReaches + 1, CH_ORGP, 0.f);
        Initialize1DArray(nReaches + 1, ch_nh4, 0.f);
        Initialize1DArray(nReaches + 1, CH_NO2, 0.f);
        Initialize1DArray(nReaches + 1, CH_NO3, 0.f);
        Initialize1DArray(nReaches + 1, CH_SOLP, 0.f);
        Initialize1DArray(nReaches + 1, ch_dox, 0.f);
        Initialize1DArray(nReaches + 1, CH_COD, 0.f);
        Initialize1DArray(nReaches + 1, CH_TN, 0.f);
        Initialize1DArray(nReaches + 1, CH_TP, 0.f);

        Initialize1DArray(nReaches + 1, CH_chloraConc, 0.f);
        Initialize1DArray(nReaches + 1, ch_algaeConc, 0.f);
        Initialize1DArray(nReaches + 1, CH_ORGNConc, 0.f);
        Initialize1DArray(nReaches + 1, CH_ORGPConc, 0.f);
        Initialize1DArray(nReaches + 1, ch_nh4Conc, 0.f);
        Initialize1DArray(nReaches + 1, CH_NO2Conc, 0.f);
        Initialize1DArray(nReaches + 1, CH_NO3Conc, 0.f);
        Initialize1DArray(nReaches + 1, CH_SOLPConc, 0.f);
        Initialize1DArray(nReaches + 1, ch_doxConc, 0.f);
        Initialize1DArray(nReaches + 1, CH_CODConc, 0.f);
        Initialize1DArray(nReaches + 1, CH_TNConc, 0.f);
        Initialize1DArray(nReaches + 1, CH_TPConc, 0.f);
    }
}

void NutrCH_QUAL2E::PointSourceLoading() {
    /// initialization and reset to 0.f
    if (ptNO3ToCh == NULL) {
        Initialize1DArray(nReaches + 1, ptNO3ToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptNH4ToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptOrgNToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptTNToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptSolPToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptOrgPToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptTPToCh, 0.f);
        Initialize1DArray(nReaches + 1, ptCODToCh, 0.f);
    } else {
        /// reset to zero for the current timestep
#pragma omp parallel for
        for (int i = 0; i <= nReaches; i++) {
            ptNO3ToCh[i] = 0.f;
            ptNH4ToCh[i] = 0.f;
            ptOrgNToCh[i] = 0.f;
            ptTNToCh[i] = 0.f;
            ptSolPToCh[i] = 0.f;
            ptOrgPToCh[i] = 0.f;
            ptTPToCh[i] = 0.f;
            ptCODToCh[i] = 0.f;
        }
    }
    /// load point source nutrient (kg) on current day from Scenario
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
            // 1.2 Otherwise, get the nutrient concentration, mg/L
            float per_wtr = curPtMgt->GetWaterVolume();
            float per_no3 = curPtMgt->GetNO3();
            float per_nh4 = curPtMgt->GetNH4();
            float per_orgn = curPtMgt->GetOrgN();
            float per_solp = curPtMgt->GetSolP();
            float per_orgP = curPtMgt->GetOrgP();
            float per_cod = curPtMgt->GetCOD();
            // 1.3 Sum up all point sources
            for (vector<int>::iterator locIter = m_ptSrcIDs.begin(); locIter != m_ptSrcIDs.end(); locIter++) {
                if (m_pointSrcLocsMap.find(*locIter) != m_pointSrcLocsMap.end()) {
                    PointSourceLocations *curPtLoc = m_pointSrcLocsMap.at(*locIter);
                    int curSubID = curPtLoc->GetSubbasinID();
                    float cvt = per_wtr * curPtLoc->GetSize() / 1000.f * DT_CH / 86400.f;/// mg/L ==> kg / timestep
                    ptNO3ToCh[curSubID] += per_no3 * cvt;
                    ptNH4ToCh[curSubID] += per_nh4 * cvt;
                    ptOrgNToCh[curSubID] += per_orgn * cvt;
                    ptOrgPToCh[curSubID] += per_orgP * cvt;
                    ptSolPToCh[curSubID] += per_solp * cvt;
                    ptCODToCh[curSubID] += per_cod * cvt;
                    ptTNToCh[curSubID] += (per_no3 + per_nh4 + per_orgn) * cvt;
                    ptTPToCh[curSubID] += (per_solp + per_orgP) * cvt;
                }
            }
        }
    }
    // sum up point sources loadings of all subbasins
    for (int i = 1; i <= nReaches; i++) {
        ptTNToCh[0] += ptTNToCh[i];
        ptTPToCh[0] += ptTPToCh[i];
        ptCODToCh[0] += ptCODToCh[i];
    }
}

int NutrCH_QUAL2E::Execute() {
    CheckInputData();
    initialOutputs();
    // load point source loadings from Scenarios
    PointSourceLoading();
    // Calculate average day length, solar radiation, and temperature for each channel
    ParametersSubbasinForChannel();

    map < int, vector < int > > ::iterator
    it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int reachNum = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < reachNum; ++i) {
            // index in the array
            int reachIndex = it->second[i];
            NutrientTransform(reachIndex);
            AddInputNutrient(reachIndex);
            RouteOut(reachIndex);
        }
    }
    //cout<<"NUTR_QUAL2E, surNO3ToCh: "<<sur_no3_ToCh[12]<<", gwno3ToCh: "<<no3gwToCh[12]<<", ptNo3ToCh: "<<ptNO3ToCh[12]
    //<<", chOutNO3: "<<CH_NO3[12]<<", chOutNO3Conc: "<<CH_TNConc[12]<<", TN: "<<CH_TN[12]<<", TNConc: "<<CH_TNConc[12]<<endl;
    //cout<<"chStr_NO3: "<<CHSTR_NO3[12]<<", chStr_NH4: "<<CHSTR_NH4[12]<<", chStr_TN: "<<CHSTR_TN[12]<<endl;
    return 0;
}

void NutrCH_QUAL2E::AddInputNutrient(int i) {
    //cout<<"subID: "<<i<<", initial nh4: "<<CHSTR_NH4[i]<<", ";
    //cout<<"subID: "<<i<<", initial cod: "<<chCOD[i]<<", ";
    /// nutrient amount from upstream routing will be accumulated to current storage
    for (size_t j = 0; j < m_reachUpStream[i].size(); ++j) {
        int upReachId = m_reachUpStream[i][j];
        //cout<<"upSubID: "<<upReachId<<", "<<ch_nh4[upReachId]<<", ";
        chOrgN[i] += CH_ORGN[upReachId];
        CHSTR_NO3[i] += CH_NO3[upReachId];
        chNO2[i] += CH_NO2[upReachId];
        CHSTR_NH4[i] += ch_nh4[upReachId];
        chOrgP[i] += CH_ORGP[upReachId];
        chSolP[i] += CH_SOLP[upReachId];
        chCOD[i] += CH_COD[upReachId];
        chDOx[i] += ch_dox[upReachId];
        chChlora[i] += CH_chlora[upReachId];
        chAlgae[i] += ch_algae[upReachId];
    }
    //cout<<", added upstream, nh4: "<<CHSTR_NH4[i]<<endl;
    //cout<<", added upstream, cod: "<<chCOD[i]<<", ";
    /// absorbed organic N, P from overland sediment routing
    chOrgN[i] += sedorgnToCh[i];
    chOrgP[i] += sedorgpToCh[i];
    /// organic N, P contribution from channel erosion
    if (rch_deg != NULL && ch_opco != NODATA_VALUE && ch_onco != NODATA_VALUE) {
        chOrgN[i] += rch_deg[i] * ch_onco / 1000.f;
        chOrgP[i] += rch_deg[i] * ch_opco / 1000.f;
    }
    /// dissolved N, P from overland surface flow routing and groundwater
    CHSTR_NO3[i] += sur_no3_ToCh[i] + latno3ToCh[i] + no3gwToCh[i];
    if (SUR_NH4_TOCH != NULL && SUR_NH4_TOCH[i] > 0.f) CHSTR_NH4[i] += SUR_NH4_TOCH[i];
    chSolP[i] += sur_solp_ToCh[i] + minpgwToCh[i];

    // if(m_nh4ToCh != NULL && m_nh4ToCh[i] > 0.f) CHSTR_NH4[i] += m_nh4ToCh[i];
    if (nitriteToCh != NULL && nitriteToCh[i] > 0.f) chNO2[i] += nitriteToCh[i];
    if (sur_cod_ToCH != NULL && sur_cod_ToCH[i] > 0.f) {
        chCOD[i] += sur_cod_ToCH[i];
        //cout<<", added surface, cod: "<<chCOD[i]<<", ";
    }
    /// add point source loadings to channel
    if (ptNO3ToCh != NULL && ptNO3ToCh[i] > 0.f) CHSTR_NO3[i] += ptNO3ToCh[i];
    if (ptNH4ToCh != NULL && ptNH4ToCh[i] > 0.f) CHSTR_NH4[i] += ptNH4ToCh[i];
    if (ptOrgNToCh != NULL && ptOrgNToCh[i] > 0.f) chOrgN[i] += ptOrgNToCh[i];
    if (ptSolPToCh != NULL && ptSolPToCh[i] > 0.f) chSolP[i] += ptSolPToCh[i];
    if (ptOrgPToCh != NULL && ptOrgPToCh[i] > 0.f) chOrgP[i] += ptOrgPToCh[i];
    if (ptCODToCh != NULL && ptCODToCh[i] > 0.f) {
        chCOD[i] += ptCODToCh[i];
        //cout<<", added point source, cod: "<<chCOD[i]<<endl;
    }
}

void NutrCH_QUAL2E::RouteOut(int i) {
    // reinitialize out variables to 0
    ch_algae[i] = 0.f;
    ch_algaeConc[i] = 0.f;
    CH_chlora[i] = 0.f;
    CH_chloraConc[i] = 0.f;
    CH_ORGN[i] = 0.f;
    CH_ORGNConc[i] = 0.f;
    ch_nh4[i] = 0.f;
    ch_nh4Conc[i] = 0.f;
    CH_NO2[i] = 0.f;
    CH_NO2Conc[i] = 0.f;
    CH_NO3[i] = 0.f;
    CH_NO3Conc[i] = 0.f;
    CH_ORGP[i] = 0.f;
    CH_ORGPConc[i] = 0.f;
    CH_SOLP[i] = 0.f;
    CH_SOLPConc[i] = 0.f;
    CH_COD[i] = 0.f;
    CH_CODConc[i] = 0.f;
    ch_dox[i] = 0.f;
    ch_doxConc[i] = 0.f;
    CH_TN[i] = 0.f;
    CH_TNConc[i] = 0.f;
    CH_TP[i] = 0.f;
    CH_TPConc[i] = 0.f;
    //get out flow water fraction
    float wtrOut = QRECH[i] * DT_CH; // m**3
    // float wtrTotal = wtrOut + CHST[i];
    float wtrTotal = preCHST[i];
    if (wtrTotal <= 0.f)//wtrOut <= 0.f ||  || CHWTDEPTH[i] <= 0.f)
    {
        // return with initialized values directly
        return;
    }
    float outFraction = wtrOut / wtrTotal;
    //if(i == 12) cout << "wtrOut: " << wtrOut << ", CHST: " << CHST[i] << ", outFrac: "<<outFraction<<endl;
    if (outFraction >= 1.f) outFraction = 1.f;
    if (outFraction <= UTIL_ZERO) outFraction = UTIL_ZERO;
    CH_ORGN[i] = chOrgN[i] * outFraction;
    CH_NO3[i] = CHSTR_NO3[i] * outFraction;
    CH_NO2[i] = chNO2[i] * outFraction;
    ch_nh4[i] = CHSTR_NH4[i] * outFraction;
    CH_ORGP[i] = chOrgP[i] * outFraction;
    CH_SOLP[i] = chSolP[i] * outFraction;
    CH_COD[i] = chCOD[i] * outFraction;
    ch_dox[i] = chDOx[i] * outFraction;
    ch_algae[i] = chAlgae[i] * outFraction;
    CH_chlora[i] = chChlora[i] * outFraction;
    CH_TN[i] = CH_ORGN[i] + ch_nh4[i] + CH_NO2[i] + CH_NO3[i];
    CH_TP[i] = CH_ORGP[i] + CH_SOLP[i];
    //if(i == 12) cout << "CH_ORGP: " << CH_ORGP[i] << ", chOrgP: " << chOrgP[i] << ", outFrac: "<<outFraction<<endl;
    // kg ==> mg/L
    float cvt = 1000.f / wtrOut;
    CH_ORGNConc[i] = CH_ORGN[i] * cvt;
    CH_NO3Conc[i] = CH_NO3[i] * cvt;
    CH_NO2Conc[i] = CH_NO2[i] * cvt;
    ch_nh4Conc[i] = ch_nh4[i] * cvt;
    CH_ORGPConc[i] = CH_ORGP[i] * cvt;
    CH_SOLPConc[i] = CH_SOLP[i] * cvt;
    CH_CODConc[i] = CH_COD[i] * cvt;
    ch_doxConc[i] = ch_dox[i] * cvt;
    ch_algaeConc[i] = ch_algae[i] * cvt;
    CH_chloraConc[i] = CH_chlora[i] * cvt;
    /// total N and total P
    CH_TNConc[i] = CH_TN[i] * cvt;
    CH_TPConc[i] = CH_TP[i] * cvt;

    CHSTR_NO3[i] -= CH_NO3[i];
    chNO2[i] -= CH_NO2[i];
    CHSTR_NH4[i] -= ch_nh4[i];
    chOrgN[i] -= CH_ORGN[i];
    chOrgP[i] -= CH_ORGP[i];
    chSolP[i] -= CH_SOLP[i];
    chCOD[i] -= CH_COD[i];
    chDOx[i] -= ch_dox[i];
    chAlgae[i] -= ch_algae[i];
    chChlora[i] -= CH_chlora[i];
    // recalculate TN TP stored in reach
    CHSTR_TN[i] = chOrgN[i] + CHSTR_NH4[i] + chNO2[i] + CHSTR_NO3[i];
    CHSTR_TP[i] = chOrgP[i] + chSolP[i];

    /// in case of zero
    if (CHSTR_NO3[i] < UTIL_ZERO) CHSTR_NO3[i] = UTIL_ZERO;
    if (chNO2[i] < UTIL_ZERO) chNO2[i] = UTIL_ZERO;
    if (CHSTR_NH4[i] < UTIL_ZERO) CHSTR_NH4[i] = UTIL_ZERO;
    if (chOrgN[i] < UTIL_ZERO) chOrgN[i] = UTIL_ZERO;
    if (chOrgP[i] < UTIL_ZERO) chOrgP[i] = UTIL_ZERO;
    if (chSolP[i] < UTIL_ZERO) chSolP[i] = UTIL_ZERO;
    if (chCOD[i] < UTIL_ZERO) chCOD[i] = UTIL_ZERO;
    if (chDOx[i] < UTIL_ZERO) chDOx[i] = UTIL_ZERO;
    if (chAlgae[i] < UTIL_ZERO) chAlgae[i] = UTIL_ZERO;
    if (chChlora[i] < UTIL_ZERO) chChlora[i] = UTIL_ZERO;
}

void NutrCH_QUAL2E::NutrientTransform(int i) {
    float thbc1 = 1.083f;    ///temperature adjustment factor for local biological oxidation of NH3 to NO2
    float thbc2 = 1.047f;    ///temperature adjustment factor for local biological oxidation of NO2 to NO3
    float thbc3 = 1.04f;    ///temperature adjustment factor for local hydrolysis of organic N to ammonia N
    float thbc4 = 1.047f;    ///temperature adjustment factor for local decay of organic P to dissolved P

    float thgra = 1.047f;    ///temperature adjustment factor for local algal growth rate
    float thrho = 1.047f;    ///temperature adjustment factor for local algal respiration rate

    float thm_rk1 = 1.047f;    ///temperature adjustment factor for local CBOD deoxygenation
    float thm_rk2 = 1.024f;    ///temperature adjustment factor for local oxygen reaeration rate
    float thm_rk3 = 1.024f;    ///temperature adjustment factor for loss of CBOD due to settling
    float thm_rk4 = 1.060f;    ///temperature adjustment factor for local sediment oxygen demand

    float thrs1 = 1.024f;    ///temperature adjustment factor for local algal settling rate
    float thrs2 = 1.074f;    ///temperature adjustment factor for local benthos source rate for dissolved phosphorus
    float thrs3 = 1.074f;    ///temperature adjustment factor for local benthos source rate for ammonia nitrogen
    float thrs4 = 1.024f;    ///temperature adjustment factor for local organic N settling rate
    float thrs5 = 1.024f;    ///temperature adjustment factor for local organic P settling rate
    // Currently, by junzhi
    // assume the water volume used to contain nutrients at current time step equals to :
    //     flowout plus the storage at the end of day (did not consider the nutrients
    //     from stream to groundwater through seepage and bank storage)
    // float wtrOut = QRECH[i] * TIMESTEP;
    // float wtrTotal = wtrOut + CHST[i]; /// m3
    float wtrTotal = preCHST[i]; // by LJ
    float tmpChWtDepth = prechwtdepth[i]; /// m
    if (tmpChWtDepth <= 0.01f) {
        tmpChWtDepth = 0.01f;
    }
    if (wtrTotal <= 0.f)/// which means no flow out of current channel    || wtrOut <= 0.f
    {
        chAlgae[i] = 0.f;
        chChlora[i] = 0.f;
        chOrgN[i] = 0.f;
        CHSTR_NH4[i] = 0.f;
        chNO2[i] = 0.f;
        CHSTR_NO3[i] = 0.f;
        CHSTR_TN[i] = 0.f;
        chOrgP[i] = 0.f;
        chSolP[i] = 0.f;
        CHSTR_TP[i] = 0.f;
        chDOx[i] = 0.f;
        chCOD[i] = 0.f;
        soxy = 0.f;
        return;// return and route out with 0.f
    }
    // initial algal biomass concentration in reach (algcon mg/L, i.e. g/m3)   kg ==> mg/L
    float cvt_amout2conc = 1000.f / wtrTotal;
    float algcon = cvt_amout2conc * chAlgae[i];
    // initial organic N concentration in reach (orgncon mg/L)
    float orgncon = cvt_amout2conc * chOrgN[i];
    // initial ammonia concentration in reach (nh4con mg/L)
    float nh4con = cvt_amout2conc * CHSTR_NH4[i];
    // initial nitrite concentration in reach (no2con mg/L)
    float no2con = cvt_amout2conc * chNO2[i];
    // initial nitrate concentration in reach (no3con mg/L)
    float no3con = cvt_amout2conc * CHSTR_NO3[i];
    // initial organic P concentration in reach (orgpcon  mg/L)
    float orgpcon = cvt_amout2conc * chOrgP[i];
    // initial soluble P concentration in reach (solpcon mg/L)
    float solpcon = cvt_amout2conc * chSolP[i];
    // initial carbonaceous biological oxygen demand (cbodcon mg/L)
    float cbodcon = cvt_amout2conc * chCOD[i];
    // initial dissolved oxygen concentration in reach (o2con mg/L)
    float o2con = cvt_amout2conc * chDOx[i];

    // temperature of water in reach (wtmp deg C)
    float wtmp = max(wattemp[i], 0.1f);
    // calculate effective concentration of available nitrogen (cinn), QUAL2E equation III-15
    float cinn = nh4con + no3con;

    // calculate saturation concentration for dissolved oxygen, QUAL2E section 3.6.1 equation III-29
    // variable to hold intermediate calculation result
    float ww = -139.34410f + (1.575701e+05f / (wtmp + 273.15f));
    float xx = 6.642308e+07f / pow((wtmp + 273.15f), 2.f);
    float yy = 1.243800e+10f / pow((wtmp + 273.15f), 3.f);
    float zz = 8.621949e+11f / pow((wtmp + 273.15f), 4.f);
    soxy = 0.f;
    soxy = exp(ww - xx + yy - zz);
    if (soxy < 1.e-6f) {
        soxy = 0.f;
    }

    // O2 impact calculations
    // calculate nitrification rate correction factor for low oxygen QUAL2E equation III-21(cordo)
    float cordo = 0.f;
    float o2con2 = o2con;
    if (o2con2 <= 0.1f) {
        o2con2 = 0.1f;
    }
    if (o2con2 > 30.f) {
        o2con2 = 30.f;
    }
    cordo = 1.f - exp(-0.6f * o2con2);
    if (o2con <= 0.001f) {
        o2con = 0.001f;
    }
    if (o2con > 30.f) {
        o2con = 30.f;
    }
    cordo = 1.f - exp(-0.6f * o2con);


    // modify ammonia and nitrite oxidation rates to account for low oxygen
    // rate constant for biological oxidation of NH3 to NO2 modified to reflect impact of low oxygen concentration (bc1mod)
    float bc1mod = 0.f;
    // rate constant for biological oxidation of NO2 to NO3 modified to reflect impact of low oxygen concentration (bc1mod)
    float bc2mod = 0.f;
    bc1mod = bc1[i] * cordo;
    bc2mod = bc2[i] * cordo;

    //	tday is the calculation time step = 1 day
    float tday = 1.f;

    // algal growth
    // calculate light extinction coefficient (lambda)
    float lambda = 0.f;
    if (ai0 * algcon > 1.e-6f) {
        lambda = lambda0 + (lambda1 * ai0 * algcon) + lambda2 * pow((ai0 * algcon), 0.66667f);
    } else {
        lambda = lambda0;
    }
    if (lambda > lambda0) lambda = lambda0;
    // calculate algal growth limitation factors for nitrogen and phosphorus, QUAL2E equations III-13 & III-14
    float fnn = 0.f;
    float fpp = 0.f;
    fnn = cinn / (cinn + k_n);
    fpp = solpcon / (solpcon + k_p);

    // calculate daylight average, photo synthetically active (algi)
    float algi = 0.f;
    if (chDaylen[i] > 0.f) {
        algi = chSr[i] * tfact / chDaylen[i];
    } else {
        algi = 0.00001f;
    }

    // calculate growth attenuation factor for light, based on daylight average light intensity
    float fl_1 = 0.f;
    float fll = 0.f;
    fl_1 = (1.f / (lambda * tmpChWtDepth)) *
        log((k_l + algi) / (k_l + algi * exp(-lambda * tmpChWtDepth)));

    fll = 0.92f * (chDaylen[i] / 24.f) * fl_1;

    // temporary variables
    float gra = 0.f;
    //float dchla = 0.f;
    float dbod = 0.f;
    float ddisox = 0.f;
    float dorgn = 0.f;
    float dnh4 = 0.f;
    float dno2 = 0.f;
    float dno3 = 0.f;
    float dorgp = 0.f;
    float dsolp = 0.f;
    switch (igropt) {
        case 1:
            // multiplicative
            gra = mumax * fll * fnn * fpp;
        case 2:
            // limiting nutrient
            gra = mumax * fll * min(fnn, fpp);
        case 3:
            // harmonic mean
            if (fnn > 1.e-6f && fpp > 1.e-6f) {
                gra = mumax * fll * 2.f / ((1.f / fnn) + (1.f / fpp));
            } else {
                gra = 0.f;
            }
    }

    // calculate algal biomass concentration at end of day (phytoplanktonic algae), QUAL2E equation III-2
    float dalgae = 0.f;
    float setl = min(1.f, corTempc(rs1[i], thrs1, wtmp) / tmpChWtDepth);
    dalgae = algcon +
        (corTempc(gra, thgra, wtmp) * algcon - corTempc(rhoq, thrho, wtmp) * algcon - setl * algcon) * tday;
    if (dalgae < 1.e-6f) {
        dalgae = 1.e-6f;
    }
    float dcoef = 3.f;
    /// set algae limit (watqual.f)
    if (dalgae > 5000.f) dalgae = 5000.f;
    if (dalgae > dcoef * algcon) dalgae = dcoef * algcon;
    // calculate chlorophyll-a concentration at end of day, QUAL2E equation III-1
    //dchla = dalgae * ai0 / 1000.f;

    // oxygen calculations
    // calculate carbonaceous biological oxygen demand at end of day (dbod)
    float yyy = 0.f;
    float zzz = 0.f;
    //1. COD convert to CBOD
    //if(i == 12) cout << "pre_cod, mg/L: " << cbodcon << ", ";
    cbodcon /= (cod_n * (1.f - exp(-5.f * cod_k)));
    //if(i == 12) cout << "pre_cbod, mg/L: " << cbodcon << ", ";
    yyy = corTempc(rk1[i], thm_rk1, wtmp) * cbodcon;
    zzz = corTempc(rk3[i], thm_rk3, wtmp) * cbodcon;
    dbod = 0.f;
    dbod = cbodcon - (yyy + zzz) * tday;
    /********* watqual.f code ***********/
    //float coef = 0.f;
    ///// deoxygenation rate
    //coef = exp(-1.f * corTempc(rk1[i], thm_rk1, wtmp)*tday);
    //float tmp = coef * cbodcon;
    //// cbod rate loss due to setting
    //coef = exp(-1.f * corTempc(rk3[i], thm_rk1, wtmp)*tday);
    //tmp *= coef;
    //dbod = tmp;
    ////if(i == 12) cout << "trans_cbod, mg/L: " << dbod << ", ";
    //if (dbod < 1.e-6f) dbod = 1.e-6f;
    //if (dbod > dcoef * cbodcon) dbod = dcoef * cbodcon;
    /********* watqual2.f code ***********/
    if (dbod < 1.e-6f) dbod = 1.e-6f;
    //if(i == 12) cout << "trans_cbod2, mg/L: " << dbod << ", ";
    //2. CBOD convert to COD, now dbod is COD
    dbod *= cod_n * (1.f - exp(-5.f * cod_k));
    //if(i == 12) cout << "cod: " << dbod << endl;

    // calculate dissolved oxygen concentration if reach at end of day (ddisox)
    float uu = 0.f;     // variable to hold intermediate calculation result
    float vv = 0.f;      // variable to hold intermediate calculation result
    ww = 0.f;    // variable to hold intermediate calculation result
    xx = 0.f;     // variable to hold intermediate calculation result
    yy = 0.f;     // variable to hold intermediate calculation result
    zz = 0.f;     // variable to hold intermediate calculation result

    //rk2[i] =1.f;	// Why define this value?

    //float hh = corTempc(rk2[i], thm_rk2, wtmp);
    uu = corTempc(rk2[i], thm_rk2, wtmp) * (soxy - o2con);
    //vv = (ai3 * corTempc(gra, thgra, wtmp) - ai4 * corTempc(rhoq, thrho, wtmp)) * algcon;
    if (algcon > 0.001f) {
        vv = (ai3 * corTempc(gra, thgra, wtmp) - ai4 * corTempc(rhoq, thrho, wtmp)) * algcon;
    } else {
        algcon = 0.001f;
    }

    ww = corTempc(rk1[i], thm_rk1, wtmp) * cbodcon;
    if (tmpChWtDepth > 0.001f) {
        xx = corTempc(rk4[i], thm_rk4, wtmp) / (tmpChWtDepth * 1000.f);
    }
    if (nh4con > 0.001f) {
        yy = ai5 * corTempc(bc1mod, thbc1, wtmp) * nh4con;
    } else {
        nh4con = 0.001f;
    }
    if (no2con > 0.001f) {
        zz = ai6 * corTempc(bc2mod, thbc2, wtmp) * no2con;
    } else {
        no2con = 0.001f;
    }
    ddisox = 0.f;
    ddisox = o2con + (uu + vv - ww - xx - yy - zz) * tday;
    //o2proc = o2con - ddisox;   // not found variable "o2proc"
    if (ddisox < 0.1f || ddisox != ddisox) {
        ddisox = 0.1f;
    }
    // algea O2 production minus respiration
    //float doxrch = soxy;
    // cbod deoxygenation
    //coef = exp(-0.1f * ww);
    //doxrch *= coef;
    // benthic sediment oxidation
    //coef = 1.f - corTempc(rk4[i], thm_rk4, wtmp) / 100.f;
    //doxrch *= coef;
    // ammonia oxydation
    //coef = exp(-0.05f * yy);
    //doxrch *= coef;
    // nitrite oxydation
    //coef = exp(-0.05f * zz);
    //doxrch *= coef;
    // reaeration
    //uu = corTempc(rk2[i], thm_rk2, wtmp) / 100.f * (soxy - doxrch);
    //ddisox = doxrch + uu;
    //if (ddisox < 1.e-6f) ddisox = 0.f;
    //if (ddisox > soxy) ddisox = soxy;
    //if (ddisox > dcoef * o2con) ddisox = dcoef * o2con;
    //////end oxygen calculations//////
    // nitrogen calculations
    // calculate organic N concentration at end of day (dorgn)
    xx = 0.f;
    yy = 0.f;
    zz = 0.f;
    xx = ai1 * corTempc(rhoq, thrho, wtmp) * algcon;
    yy = corTempc(bc3[i], thbc3, wtmp) * orgncon;
    zz = corTempc(rs4[i], thrs4, wtmp) * orgncon;
    dorgn = 0.f;
    dorgn = orgncon + (xx - yy - zz) * tday;
    if (dorgn < 1.e-6f) dorgn = 0.f;
    if (dorgn > dcoef * orgncon) dorgn = dcoef * orgncon;
    // calculate fraction of algal nitrogen uptake from ammonia pool
    float f1 = 0.f;
    f1 = p_n * nh4con / (p_n * nh4con + (1.f - p_n) * no3con + 1.e-6f);

    //cout<<"subID: "<<i<<", initial nh4 conc: "<<nh4con<<", "<<"initial orgn: "<<orgncon<<", ";
    // calculate ammonia nitrogen concentration at end of day (dnh4)
    ww = 0.f;
    xx = 0.f;
    yy = 0.f;
    zz = 0.f;
    ww = corTempc(bc3[i], thbc3, wtmp) * orgncon;
    xx = corTempc(bc1mod, thbc1, wtmp) * nh4con;
    yy = corTempc(rs3[i], thrs3, wtmp) / (tmpChWtDepth * 1000.f);
    zz = f1 * ai1 * algcon * corTempc(gra, thgra, wtmp);
    dnh4 = 0.f;
    dnh4 = nh4con + (ww - xx + yy - zz) * tday;
    if (dnh4 < 1.e-6f) dnh4 = 0.f;
    if (dnh4 > dcoef * nh4con && nh4con > 0.f) {
        dnh4 = dcoef * nh4con;
    }
    //if(i == 12) cout<<"orgncon: "<<orgncon<<", ��: "<<corTempc(bc3[i], thbc3, wtmp) <<", xx: "<<xx<<", yy: "<<yy<<", zz: "<<zz<<",\n nh4 out: "<<dnh4<<endl;
    // calculate concentration of nitrite at end of day (dno2)
    yy = 0.f;
    zz = 0.f;
    yy = corTempc(bc1mod, thbc1, wtmp) * nh4con;
    zz = corTempc(bc2mod, thbc2, wtmp) * no2con;
    dno2 = 0.f;
    dno2 = no2con + (yy - zz) * tday;
    if (dno2 < 1.e-6f) dno2 = 0.f;
    if (dno2 > dcoef * no2con && no2con > 0.f) {
        dno2 = dcoef * no2con;
    }
    // calculate nitrate concentration at end of day (dno3)
    yy = 0.f;
    zz = 0.f;
    yy = corTempc(bc2mod, thbc2, wtmp) * no2con;
    zz = (1.f - f1) * ai1 * algcon * corTempc(gra, thgra, wtmp);
    dno3 = 0.f;
    dno3 = no3con + (yy - zz) * tday;
    if (dno3 < 1.e-6) dno3 = 0.f;
    /////end nitrogen calculations//////
    // phosphorus calculations
    // calculate organic phosphorus concentration at end of day
    xx = 0.f;
    yy = 0.f;
    zz = 0.f;
    xx = ai2 * corTempc(rhoq, thrho, wtmp) * algcon;
    yy = corTempc(bc4[i], thbc4, wtmp) * orgpcon;
    zz = corTempc(rs5[i], thrs5, wtmp) * orgpcon;
    dorgp = 0.f;
    dorgp = orgpcon + (xx - yy - zz) * tday;
    if (dorgp < 1.e-6f) dorgp = 0.f;
    if (dorgp > dcoef * orgpcon) {
        dorgp = dcoef * orgpcon;
    }

    // calculate dissolved phosphorus concentration at end of day (dsolp)
    xx = 0.f;
    yy = 0.f;
    zz = 0.f;
    xx = corTempc(bc4[i], thbc4, wtmp) * orgpcon;
    yy = corTempc(rs2[i], thrs2, wtmp) / (tmpChWtDepth * 1000.f);
    zz = ai2 * corTempc(gra, thgra, wtmp) * algcon;
    dsolp = 0.f;
    dsolp = solpcon + (xx + yy - zz) * tday;
    if (dsolp < 1.e-6) dsolp = 0.f;
    if (dsolp > dcoef * solpcon) {
        dsolp = dcoef * solpcon;
    }
    /////////end phosphorus calculations/////////
    // storage amount (kg) at end of day
    chAlgae[i] = dalgae * wtrTotal / 1000.f;
    chChlora[i] = chAlgae[i] * ai0;
    chOrgN[i] = dorgn * wtrTotal / 1000.f;
    CHSTR_NH4[i] = dnh4 * wtrTotal / 1000.f;
    chNO2[i] = dno2 * wtrTotal / 1000.f;
    CHSTR_NO3[i] = dno3 * wtrTotal / 1000.f;
    chOrgP[i] = dorgp * wtrTotal / 1000.f;
    chSolP[i] = dsolp * wtrTotal / 1000.f;
    chCOD[i] = dbod * wtrTotal / 1000.f;
    //if(i == 12) cout << "chCOD: " << chCOD[i] << endl;
    chDOx[i] = ddisox * wtrTotal / 1000.f;
}

float NutrCH_QUAL2E::corTempc(float r20, float thk, float tmp) {
    float theta = 0.f;
    theta = r20 * pow(thk, (tmp - 20.f));
    return theta;
}

void NutrCH_QUAL2E::GetValue(const char *key, float *value) {
    string sk(key);
    if (StringMatch(sk, VAR_SOXY)) {
        *value = soxy;
    }
}

void NutrCH_QUAL2E::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    *n = nReaches + 1;
    if (StringMatch(sk, VAR_CH_ALGAE)) { *data = ch_algae; }
    else if (StringMatch(sk, VAR_CH_ALGAEConc)) { *data = ch_algaeConc; }
    else if (StringMatch(sk, VAR_CH_NO2)) { *data = CH_NO2; }
    else if (StringMatch(sk, VAR_CH_NO2Conc)) { *data = CH_NO2Conc; }
    else if (StringMatch(sk, VAR_CH_COD)) { *data = CH_COD; }
    else if (StringMatch(sk, VAR_CH_CODConc)) { *data = CH_CODConc; }
    else if (StringMatch(sk, VAR_CH_CHLORA)) { *data = CH_chlora; }
    else if (StringMatch(sk, VAR_CH_CHLORAConc)) { *data = CH_chloraConc; }
    else if (StringMatch(sk, VAR_CH_NO3)) { *data = CH_NO3; }
    else if (StringMatch(sk, VAR_CH_NO3Conc)) { *data = CH_NO3Conc; }
    else if (StringMatch(sk, VAR_CH_SOLP)) { *data = CH_SOLP; }
    else if (StringMatch(sk, VAR_CH_SOLPConc)) { *data = CH_SOLPConc; }
    else if (StringMatch(sk, VAR_CH_ORGN)) { *data = CH_ORGN; }
    else if (StringMatch(sk, VAR_CH_ORGNConc)) { *data = CH_ORGNConc; }
    else if (StringMatch(sk, VAR_CH_ORGP)) { *data = CH_ORGP; }
    else if (StringMatch(sk, VAR_CH_ORGPConc)) { *data = CH_ORGPConc; }
    else if (StringMatch(sk, VAR_CH_NH4)) { *data = ch_nh4; }
    else if (StringMatch(sk, VAR_CH_NH4Conc)) { *data = ch_nh4Conc; }
    else if (StringMatch(sk, VAR_CH_DOX)) { *data = ch_dox; }
    else if (StringMatch(sk, VAR_CH_DOXConc)) { *data = ch_doxConc; }
    else if (StringMatch(sk, VAR_CH_TN)) { *data = CH_TN; }
    else if (StringMatch(sk, VAR_CH_TNConc)) { *data = CH_TNConc; }
    else if (StringMatch(sk, VAR_CH_TP)) { *data = CH_TP; }
    else if (StringMatch(sk, VAR_CH_TPConc)) { *data = CH_TPConc; }
    else if (StringMatch(sk, VAR_PTTN2CH)) { *data = ptTNToCh; }
    else if (StringMatch(sk, VAR_PTTP2CH)) { *data = ptTPToCh; }
    else if (StringMatch(sk, VAR_PTCOD2CH)) {
        *data = ptCODToCh;
        /// output nutrient storage in channel
    } else if (StringMatch(sk, VAR_CHSTR_NO3)) { *data = CHSTR_NO3; }
    else if (StringMatch(sk, VAR_CHSTR_NH4)) { *data = CHSTR_NH4; }
    else if (StringMatch(sk, VAR_CHSTR_TN)) { *data = CHSTR_TN; }
    else if (StringMatch(sk, VAR_CHSTR_TP)) { *data = CHSTR_TP; }
    else {
        throw ModelException(MID_NUTRCH_QUAL2E, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}
