/*
 * \brief Non point source management
 * \author Liang-Jun Zhu
 * \date Jul. 2016
 */
#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;
/** \defgroup NPSMGT
 * \ingroup Management
 * \brief Non point source management
 */
/*!
 * \class NPS_Management
 * \ingroup NPSMGT
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 */
class NPS_Management : public SimulationModule {

    /*!
    * areal source operations
    * key: unique index, BMPID * 100000 + subScenarioID
    * value: areal source management factory instance
    */
    map<int, BMPArealSrcFactory *> m_arealSrcFactory;

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description cell width (m)
    float CELLWIDTH;

    // @In
    // @Description area of cell (m^2)
    float cellArea;

    // @In
    // @Description time step (second)
    float TIMESTEP;

    // @In
    // @Description management fields raster
    float *mgt_fields;

    /// variables to be updated (optionals£©

    // @In
    // @Description water storage of soil layers
    float **solst;

    // @In
    // @Description nitrate kg/ha
    float **sol_no3;

    // @In
    // @Description ammonium kg/ha
    float **sol_nh4;

    // @In
    // @Description soluble phosphorus kg/ha
    float **sol_solp;

    // @In
    // @Description organic p
    float **sol_orgn;

    // @In
    // @Description organic n
    float **sol_orgp;

public:
    //! Constructor
    NPS_Management(void);

    //! Destructor
    ~NPS_Management(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int n, int col, float **data);

    //virtual void Get1DData(const char *key, int *n, float **data);

    //virtual void Get2DData(const char *key, int *n, int *col, float ***data);

    virtual void SetScenario(Scenario *sce);

private:
    /*!
     * \brief check the input data. Make sure all the input data is available.
     * \return bool The validity of the input data.
     */
    bool CheckInputData(void);

    /*!
     * \brief check the input size. Make sure all the input data have same dimension.
     *
     *
     * \param[in] key The key of the input data
     * \param[in] n The input data dimension
     * \return bool The validity of the dimension
     */
    bool CheckInputSize(const char *, int);
};

VISITABLE_STRUCT(NPS_Management, m_nCells, CELLWIDTH, cellArea, TIMESTEP, mgt_fields, solst, sol_no3, sol_nh4, sol_solp, sol_orgn, sol_orgp);