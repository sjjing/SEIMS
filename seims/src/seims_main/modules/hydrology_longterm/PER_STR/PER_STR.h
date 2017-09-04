/*!
 * \brief Percolation calculated by storage routing method
 * reference SWAT theory manual, p151-152
 * \author Junzhi Liu
 * \date May 2011
 * \revised LiangJun Zhu
 * \date 2016-5-29
 * \date 2016-9-8
 * \description: 1. ReWrite according to percmain.f and sat_excess.f of SWAT
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/*!
 * \defgroup PER_STR
 * \ingroup Hydrology_longterm
 * \brief Percolation calculated by storage routing method.
 */

/*!
 * \class PER_STR
 * \ingroup PER_STR
 * \brief Percolation calculated by storage routing method.
 * 
 */
class PER_STR : public SimulationModule {

    // @In
    // @Description number of soil layers
    int nSoilLayers;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description soil thickness
    float **soilthick;

    // @In
    // @Description time step
    int TIMESTEP;

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description threshold soil freezing temperature
    float t_soil;

    // @In
    // @Description saturated conductivity
    float **Conductivity;

    // @In
    // @Description amount of water held in the soil layer at saturation (sat - wp water), mm
    float **sol_ul;

    // @In
    // @Description amount of water held in the soil layer at field capacity (fc - wp water) mm H2O
    float **sol_awc;

    // @In
    // @Description soil moisture, mm H2O
    float **solst;

    // @In
    // @Description amount of water stored in soil profile on current day, sol_sw in SWAT
    float *solsw;

    // @In
    // @Description soil temperature
    float *SOTE;

    // @In
    // @Description infiltration, mm
    float *INFIL;

    // @In
    // @Description surface runoff, mm
    float *SURU;

    // @In
    // @Description pothole volume, mm
    float *pot_vol;

    // @Out
    // @Description percolation
    float **Perco;

public:
    /// Constructor
    PER_STR(void);

    /// Destructor
    ~PER_STR(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int nRows, float *data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

private:
    /**
    *	@brief check the input data. Make sure all the input data is available.
    *
    *	@return bool The validity of the input data.
    */
    bool CheckInputData(void);

    /**
    *	@brief check the input size. Make sure all the input data have same dimension.
    *
    *	@param key The key of the input data
    *	@param n The input data dimension
    *	@return bool The validity of the dimension
    */
    bool CheckInputSize(const char *, int);

    void initialOutputs(void);
};

VISITABLE_STRUCT(PER_STR, m_nCells, nSoilLayers, soillayers, soilthick, TIMESTEP, t_soil, Conductivity, sol_ul, sol_awc, solst, solsw, 
	SOTE, INFIL, SURU, pot_vol, Perco);