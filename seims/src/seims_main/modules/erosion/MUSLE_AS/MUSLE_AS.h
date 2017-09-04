/*!
 * \brief use MUSLE method to calculate sediment yield of each cell
 * \author Zhiqiang Yu
 * \date Feb. 2012
 * \revised LiangJun Zhu
 * \revised date May. 2016
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup MUSLE_AS
 * \ingroup Erosion
 * \brief use MUSLE method to calculate sediment yield of each cell
 */
/*!
 * \class MUSLE_AS
 * \ingroup MUSLE_AS
 *
 * \brief use MUSLE method to calculate sediment yield of each cell
 *
 */
class MUSLE_AS : public SimulationModule {
public:
    //! Constructor
    MUSLE_AS(void);

    //! Destructor
    ~MUSLE_AS(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void GetValue(const char *key, float *value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetSubbasins(clsSubbasins *subbasins);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

	// @In
	// @Description valid cell number
    int m_nCells;

	// @In
	// @Description cell width (m)
    float CELLWIDTH;

	// @In
	// @Description subbasin number
    int nsub;

	// @In
	// @Description soil layer number
    int nSoilLayers;

	// @In
	// @Description deposition ratio
    float depRatio;  

	// @In
	// @Description grid from parameter, sand fraction
    float *det_sand;

	// @In
	// @Description silt fraction
    float *det_silt;

	// @In
	// @Description clay fraction
    float *det_clay;

	// @In
	// @Description small aggregate fraction
    float *det_smagg;

	// @In
	// @Description large aggregate fraction
    float *det_lgagg;

	// @In
	// @Description USLE P factor (Practice)
    float *USLE_P;

	// @In
	// @Description USLE K factor (erodibility), multi-layer paramters. By LJ
    float **USLE_K;

	// @In
	// @Description USLE C factor (land cover)
    float *USLE_C;

	// @In
	// @Description Slope gradient (drop/distance)
    float *slope;

	// @In
	// @Description flow accumulation (number of accumulated cells)
    float *acc;

	// @In
	// @Description stream link
    float *STREAM_LINK;
  
	// @In
	// @Description USLE LS factor
    float *USLE_LS;

	// @In
	// @Description cell area (A, km^2)
    float cellAreaKM;

	// @In
	// @Description cell area factor (3.79 * A^0.7)
    float cellAreaKM1;

	// @In
	// @Description cell area factor (0.903 * A^0.017)
    float cellAreaKM2;

	// @In
	// @Description Slope^0.16
    float *slopeForPq;

    //grid from other modules

	// @In
	// @Description snow accumulation
    float *SNAC;

	// @In
	// @Description surface runoff (mm)
    float *OL_Flow;

    //result

	// @Out
	// @Description sediment yield on each cell
    float *SOER;

	// @Out
	// @Description sand yield
    float *sand_yld;

	// @Out
	// @Description silt yield
    float *silt_yld;

	// @Out
	// @Description clay yield
    float *clay_yld;

	// @Out
	// @Description small aggregate yield
    float *sag_yld;

	// @Out
	// @Description large aggregate yield
    float *lag_yld;

    //! initialize outputs
    void initialOutputs(void);

    //!
    float getPeakRunoffRate(int);
};

VISITABLE_STRUCT(MUSLE_AS, m_nCells, CELLWIDTH, nsub, nSoilLayers, depRatio, det_sand, det_silt, det_clay, det_smagg, det_lgagg,
	USLE_P, USLE_K, USLE_C, slope, acc, STREAM_LINK, USLE_LS, cellAreaKM, cellAreaKM1, cellAreaKM2, slopeForPq, SNAC, OL_Flow, 
	SOER, sand_yld, silt_yld, clay_yld, sag_yld, lag_yld);