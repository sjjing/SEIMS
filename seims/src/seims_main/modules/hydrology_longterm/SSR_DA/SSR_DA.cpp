#include "seims.h"
#include "SSR_DA.h"

SSR_DA::SSR_DA(void) : nSoilLayers(-1), TIMESTEP(-1), m_nCells(-1), CELLWIDTH(-1.f), nSubbasin(-1),
                       t_soil(NODATA_VALUE), Ki(NODATA_VALUE),
                       soillayers(NULL), soilthick(NULL), Conductivity(NULL), sol_ul(NULL), Poreindex(NULL),
                       sol_awc(NULL), sol_wpmm(NULL),
                       slope(NULL), CHWIDTH(NULL), STREAM_LINK(NULL), subbasin(NULL),
                       FLOWIN_INDEX_D8(NULL), ROUTING_LAYERS(NULL), nRoutingLayers(-1),
    /// input from other modules
                       solst(NULL), solsw(NULL), SOTE(NULL),
    /// outputs
                       SSRU(NULL), SSRUVOL(NULL), SBIF(NULL) {
}

SSR_DA::~SSR_DA(void) {
    if (SSRU != NULL) Release2DArray(m_nCells, SSRU);
    if (SSRUVOL != NULL) Release2DArray(m_nCells, SSRUVOL);
    if (SBIF != NULL) Release1DArray(SBIF);
}

void SSR_DA::FlowInSoil(int id) {
    float s0 = max(slope[id], 0.01f);
    float flowWidth = CELLWIDTH;
    // there is no land in this cell
    if (STREAM_LINK[id] > 0) {
        flowWidth -= CHWIDTH[id];
    }
    // initialize for current cell of current timestep
    for (int j = 0; j < (int) soillayers[id]; j++) {
        SSRU[id][j] = 0.f;
        SSRUVOL[id][j] = 0.f;
    }
    // return with initial values if flowWidth is less than 0
    if (flowWidth <= 0) return;
    // number of flow-in cells
    int nUpstream = (int) FLOWIN_INDEX_D8[id][0];
    solsw[id] = 0.f; // update soil storage on profile
    for (int j = 0; j < (int) soillayers[id]; j++) {
        float smOld = solst[id][j];
        //sum the upstream subsurface flow, m3
        float qUp = 0.f;
        for (int upIndex = 1; upIndex <= nUpstream; ++upIndex) {
            int flowInID = (int) FLOWIN_INDEX_D8[id][upIndex];
            //cout << id << "\t" << flowInID << "\t" << m_nCells << endl;
            if (SSRU[flowInID][j] > 0.f) {
                qUp += SSRU[flowInID][j];
            }// * m_flowInPercentage[id][upIndex];
            //cout << id << "\t" << flowInID << "\t" << m_nCells << "\t" << SSRU[flowInID][j] << endl;
        }
        // add upstream water to the current cell
        if (qUp < 0.f) qUp = 0.f;
        solst[id][j] += qUp; // mm
        //TEST
        if (solst[id][j] != solst[id][j] || solst[id][j] < 0.f) {
            ostringstream oss;
            oss << "cell id: " << id << "\t layer: " << j << "\nmoisture is less than zero: " << solst[id][j]
                << "\t"
                << smOld << "\nqUp: " << qUp << "\t depth:" << soilthick[id][j] << endl;
            cout << oss.str() << endl;
            throw ModelException(MID_SSR_DA, "Execute:FlowInSoil", oss.str());
        }

        // if soil moisture is below the field capacity, no interflow will be generated, otherwise:
        if (solst[id][j] > sol_awc[id][j]) {
            // for the upper two layers, soil may be frozen
            // also check if there are upstream inflow
            if (j == 0 && SOTE[id] <= t_soil && qUp <= 0.f) {
                continue;
            }

            float k = 0.f, maxSoilWater = 0.f, soilWater = 0.f, fcSoilWater = 0.f;
            soilWater = solst[id][j];
            maxSoilWater = sol_ul[id][j];
            fcSoilWater = sol_awc[id][j];
            //the moisture content can exceed the porosity in the way the algorithm is implemented
            if (solst[id][j] > maxSoilWater) {
                k = Conductivity[id][j];
            } else {
                /// Using Clapp and Hornberger (1978) equation to calculate unsaturated hydraulic conductivity.
                float dcIndex = 2.f * Poreindex[id][j] + 3.f; // pore disconnectedness index
                k = Conductivity[id][j] * pow(solst[id][j] / maxSoilWater, dcIndex);
                if (k < 0.f) k = 0.f;
            }
            SSRU[id][j] = Ki * s0 * k * TIMESTEP / 3600.f * soilthick[id][j] / 1000.f / flowWidth; // the unit is mm

            if (soilWater - SSRU[id][j] > maxSoilWater) {
                SSRU[id][j] = soilWater - maxSoilWater;
            } else if (soilWater - SSRU[id][j] < fcSoilWater) {
                SSRU[id][j] = soilWater - fcSoilWater;
            }
            SSRU[id][j] = max(0.f, SSRU[id][j]);

            SSRUVOL[id][j] = SSRU[id][j] / 1000.f * CELLWIDTH * flowWidth; //m3
            SSRUVOL[id][j] = max(0.f, SSRUVOL[id][j]);
            //Adjust the moisture content in the current layer, and the layer immediately below it
            solst[id][j] -= SSRU[id][j];
            solsw[id] += solst[id][j];
            if (solst[id][j] != solst[id][j] || solst[id][j] < 0.f) {
                ostringstream oss;
                oss << id << "\t" << j << "\nmoisture is less than zero: " << solst[id][j] << "\t"
                    << soilthick[id][j];
                throw ModelException(MID_SSR_DA, "Execute", oss.str());
            }
        }
    }
}

//Execute module
int SSR_DA::Execute() {
    CheckInputData();
    initialOutputs();

    for (int iLayer = 0; iLayer < nRoutingLayers; ++iLayer) {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nCells = (int) ROUTING_LAYERS[iLayer][0];

#pragma omp parallel for
        for (int iCell = 1; iCell <= nCells; ++iCell) {
            int id = (int) ROUTING_LAYERS[iLayer][iCell];
            FlowInSoil(id);
        }
    }

    //cout << "end flowinsoil" << endl;
    for (int i = 0; i < nSubbasin; i++) {
        SBIF[i] = 0.f;
    }

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++) {
        float qiAllLayers = 0.f;
        if (STREAM_LINK[i] > 0) {
            qiAllLayers = 0.f;
            for (int j = 0; j < (int) soillayers[i]; j++) {
                if (SSRUVOL[i][j] > UTIL_ZERO) {
                    qiAllLayers += SSRUVOL[i][j] / TIMESTEP;
                } /// m^3/s
            }
            //cout << nSubbasin << "\tsubbasin:" << subbasin[i] << "\t" << qiAllLayers << endl;
            //if (nSubbasin > 1)
            //    SBIF[int(subbasin[i])] += qiAllLayers;
            //else
            //    SBIF[1] += qiAllLayers;
            SBIF[int(subbasin[i])] += qiAllLayers;
        }
    }

    for (int i = 1; i <= nSubbasin; i++) {
        //cout<<", "<<i<<": "<<SBIF[i];
        SBIF[0] += SBIF[i];
    }
    //cout<<endl;
    return 0;
}

void SSR_DA::SetValue(const char *key, float data) {
    string s(key);
    if (StringMatch(s, VAR_OMP_THREADNUM)) {
        SetOpenMPThread((int) data);
    } else if (StringMatch(s, VAR_T_SOIL)) {
        t_soil = data;
    } else if (StringMatch(s, VAR_KI)) {
        Ki = data;
    } else if (StringMatch(s, Tag_CellWidth)) {
        CELLWIDTH = data;
    } else if (StringMatch(s, Tag_TimeStep)) {
        TIMESTEP = int(data);
    } else {
        throw ModelException(MID_SSR_DA, "SetValue", "Parameter " + s + " does not exist.");
    }
}

void SSR_DA::Set1DData(const char *key, int nRows, float *data) {
    string s(key);
    CheckInputSize(key, nRows);
    if (StringMatch(s, VAR_SLOPE)) {
        slope = data;
    } else if (StringMatch(s, VAR_CHWIDTH)) {
        CHWIDTH = data;
    } else if (StringMatch(s, VAR_STREAM_LINK)) {
        STREAM_LINK = data;
    } else if (StringMatch(s, VAR_SOTE)) {
        SOTE = data;
    } else if (StringMatch(s, VAR_SUBBSN)) {
        subbasin = data;
    } else if (StringMatch(s, VAR_SOILLAYERS)) {
        soillayers = data;
    } else if (StringMatch(s, VAR_SOL_SW)) {
        solsw = data;
    } else {
        throw ModelException(MID_SSR_DA, "SetValue", "Parameter " + s + " does not exist.");
    }
}

void SSR_DA::Set2DData(const char *key, int nrows, int ncols, float **data) {
    string sk(key);
    if (StringMatch(sk, VAR_SOILTHICK)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        soilthick = data;
    } else if (StringMatch(sk, VAR_CONDUCT)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        Conductivity = data;
    } else if (StringMatch(sk, VAR_SOL_UL)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        sol_ul = data;
    } else if (StringMatch(sk, VAR_SOL_AWC)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        sol_awc = data;
    } else if (StringMatch(sk, VAR_SOL_WPMM)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        sol_wpmm = data;
    } else if (StringMatch(sk, VAR_POREIDX)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        Poreindex = data;
    } else if (StringMatch(sk, VAR_SOL_ST)) {
        CheckInputSize(key, nrows);
        nSoilLayers = ncols;
        solst = data;
    } else if (StringMatch(sk, Tag_ROUTING_LAYERS)) {
        nRoutingLayers = nrows;
        ROUTING_LAYERS = data;
    } else if (StringMatch(sk, Tag_FLOWIN_INDEX_D8)) {
        CheckInputSize(key, nrows);
        FLOWIN_INDEX_D8 = data;
    } else {
        throw ModelException(MID_SSR_DA, "Set2DData", "Parameter " + sk + " does not exist.");
    }
}

void SSR_DA::SetSubbasins(clsSubbasins *subbasins) {
    if (subbasins != NULL) {
        if (nSubbasin < 0) {
            nSubbasin = subbasins->GetSubbasinNumber();
        }
    } else {
        throw ModelException(MID_SSR_DA, "SetSubbasins", "Subbasins data does not exist.");
    }
}

void SSR_DA::Get1DData(const char *key, int *n, float **data) {
    initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SBIF)) {
        *data = SBIF;
    } else {
        throw ModelException(MID_SSR_DA, "Get1DData", "Result " + sk + " does not exist.");
    }
    *n = nSubbasin + 1;
}

void SSR_DA::Get2DData(const char *key, int *nRows, int *nCols, float ***data) {
    initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = nSoilLayers;

    if (StringMatch(sk, VAR_SSRU)) {
        *data = SSRU;
    } else if (StringMatch(sk, VAR_SSRUVOL)) {
        *data = SSRUVOL;
    } else {
        throw ModelException(MID_SSR_DA, "Get2DData", "Output " + sk + " does not exist.");
    }
}

bool SSR_DA::CheckInputData() {
    if (m_nCells <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    }
    if (Ki <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "You have not set Ki.");
    }
    if (FloatEqual(t_soil, NODATA_VALUE)) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "You have not set frozen T.");
    }
    if (TIMESTEP <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "You have not set time step.");
    }
    if (CELLWIDTH <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "You have not set cell width.");
    }
    if (nSubbasin < 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The number of subbasins can not be less than 0.");
    }
    if (subbasin == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The parameter: subbasin can not be NULL.");
    }
    if (soillayers == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The soil layers number can not be NULL.");
    }
    if (soilthick == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The soil thickness can not be NULL.");
    }
    if (slope == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The slope can not be NULL.");
    }
    if (Conductivity == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The conductivity can not be NULL.");
    }
    if (sol_ul == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The porosity can not be NULL.");
    }
    if (Poreindex == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The pore index can not be NULL.");
    }
    if (sol_awc == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The field capacity can not be NULL.");
    }
    if (sol_wpmm == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The wilting point can not be NULL.");
    }
    if (solst == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The soil storage can not be NULL.");
    }
    if (solsw == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The soil storage on profile can not be NULL.");
    }
    if (SOTE == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The soil temperature can not be NULL.");
    }
    if (CHWIDTH == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The channel width can not be NULL.");
    }
    if (STREAM_LINK == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The stream link can not be NULL.");
    }
    if (FLOWIN_INDEX_D8 == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The flow in index can not be NULL.");
    }
    if (ROUTING_LAYERS == NULL) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The routing layers can not be NULL.");
    }
    if (nRoutingLayers <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputData", "The number of routing layers can not be less than 0.");
    }

    return true;
}

void SSR_DA::initialOutputs() {
    if (SBIF == NULL) Initialize1DArray(nSubbasin + 1, SBIF, 0.f);
    if (SSRU == NULL) {
        Initialize2DArray(m_nCells, nSoilLayers, SSRU, 0.f);
        Initialize2DArray(m_nCells, nSoilLayers, SSRUVOL, 0.f);
    }
}

bool SSR_DA::CheckInputSize(const char *key, int n) {
    if (n <= 0) {
        throw ModelException(MID_SSR_DA, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    }
    if (this->m_nCells != n) {
        if (this->m_nCells <= 0) { this->m_nCells = n; }
        else {
            throw ModelException(MID_SSR_DA, "CheckInputSize", "Input data for " + string(key) +
                " is invalid. All the input data should have same size.");
        }
    }
    return true;
}
