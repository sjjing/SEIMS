/*!
 * \brief Add nitrate from rainfall to the soil profile as in SWAT rev. 637, nrain.f
 * \author Huiran Gao
 * \date May 2016
 *
 * \revised Liang-Jun Zhu
 * \date 2016-7-24
 * \note: 1. Delete m_cellWidth, nSoilLayers, m_sol_z, which are useless
 *        2. Change wshd_rno3 to store nitrate from rainfall of current day
 *        3. Remove output of sol_no3, which is redundant and unnecessary
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

/** \defgroup ATMDEP
 * \ingroup Nutrient
 * \brief Calculate the atmospheric deposition of nitrogen, include nitrate and ammonia.
 */
/*!
 * \class AtmosphericDeposition
 * \ingroup ATMDEP
 */
class AtmosphericDeposition : public SimulationModule {
public:
    AtmosphericDeposition(void);

    ~AtmosphericDeposition(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void GetValue(const char *key, float *value);

    //virtual void Get1DData(const char* key, int* n, float** data);
    //virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

    void initialOutputs(void);

    // @In
    // @Description size of array
    int m_nCells;
   
    // @In
    // @Description maximum soil layers
    int soiLayers;

    /// parameters

    // @In
    // @Description concentration of nitrate in the rain (mg N/L)
    float rcn;

    // @In
    // @Description concentration of ammonia in the rain (mg N/L)
    float rca;

    // @In
    // @Description atmospheric dry deposition of nitrates (kg/ha)
    float drydep_no3;

    // @In
    // @Description atmospheric dry deposition of ammonia (kg/ha)
    float drydep_nh4;

    /// inputs

    // @In
    // @Description precipitation (mm H2O)
    float *D_P;

    // @In
    // @Description amount of ammonium in layer (kg/ha)
    float **sol_nh4;

    // @In
    // @Description amount of nitrate in layer (kg/ha)
    float **sol_no3;

    /// outputs

    // @Out
    // @Description amount of NO3 added to soil by rainfall in watershed on current day (kg/ha)
    float wshd_rno3;

private:

    /// temporaries

    // nitrate added by rainfall (kg/ha)
    float addrnh4;

    // ammonium added by rainfall (kg/ha)
    float addrno3;

};

VISITABLE_STRUCT(AtmosphericDeposition, m_nCells, soiLayers, rcn, rca, drydep_no3, drydep_nh4,
	D_P, sol_nh4, sol_no3, wshd_rno3);