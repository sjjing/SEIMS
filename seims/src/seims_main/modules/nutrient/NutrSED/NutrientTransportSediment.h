/*!
 * \brief Nutrient removed and lost in surface runoff.
 *        Support three carbon model, static method (orgn.f), C-FARM one carbon model (orgncswat.f), and CENTURY C/N cycling model (NCsed_leach.f90) from SWAT
 *        As for phosphorus, psed.f of SWAT calculates the attached to sediment in surface runoff.
 * \author Huiran Gao
 * \date April 2016
 * 
 * \revised Liang-Jun Zhu
 * \date 2016-9-28
 * \description: 1. Code revision.
 *               2. Add CENTURY model of calculating organic nitrogen removed in surface runoff
 * \TODO         1. Ammonian adsorbed to soil should be considered.
 */

#pragma once

#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"
#include "NutrientCommon.h"

using namespace std;
/** \defgroup NUTRSED
 * \ingroup Nutrient
 * \brief Nutrient removed and lost with the eroded sediment in surface runoff
 */

/*!
 * \class NutrientTransportSediment
 * \ingroup NUTRSED
 *
 * \brief Nutrient removed and lost with the eroded sediment in surface runoff
 *
 */

class NutrientTransportSediment : public SimulationModule {
public:
    NutrientTransportSediment(void);

    ~NutrientTransportSediment(void);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void SetValue(const char *key, float value);

    virtual void SetSubbasins(clsSubbasins *);

    virtual int Execute(void);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    // @In
    // @Description cell width of grid map (m)
    float CELLWIDTH;

    // @In
    // @Description number of cells
    int m_nCells;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description maximum soil layers
    int nSoiLayers;

    // @In
    // @Description soil rock content, %
    float **rock;

    // @In
    // @Description sol_ul, soil saturated water amount, mm
    float **sol_ul;

    // @In
    // @Description carbon modeling method, 0 Static soil carbon (old mineralization routines), 1 C-FARM one carbon pool model, 2 Century model 
    int cswat;

    // @In
    // @Description enrichment ratio
    float *enratio;

    ///inputs

    // @In
    // @Description soil loss caused by water erosion
    float *SED_OL;

    // @In
    // @Description surface runoff generated
    float *OL_Flow;

    // @In
    // @Description bulk density of the soil
    float **density;

    // @In
    // @Description thickness of soil layer
    float **soilthick;

    // @In
    // @Description subbasin grid (subbasins ID)
    float *subbasin;    

    ///output data

    // @Out
    // @Description amount of organic nitrogen in surface runoff
    float *sedorgn;

    // @Out
    // @Description amount of organic phosphorus in surface runoff
    float *sedorgp;

    // @Out
    // @Description amount of active mineral phosphorus sorbed to sediment in surface runoff
    float *sedminpa;

    // @Out
    // @Description amount of stable mineral phosphorus sorbed to sediment in surface runoff
    float *sedminps;

    /// output to channel

    // @Out
    // @Description amount of organic N in surface runoff to channel, kg
    float *sedorgnToCh;  
    
    // @Out
    // @Description amount of organic P in surface runoff to channel, kg
    float *sedorgpToCh;  

    // @Out
    // @Description amount of active mineral P in surface runoff to channel, kg
    float *sedminpaToCh; 

    // @Out
    // @Description amount of stable mineral P in surface runoff to channel, kg
    float *sedminpsToCh; 

    ///input & output

    // @In
    // @Description amount of nitrogen stored in the active organic (humic) nitrogen pool, kg N/ha
    float **sol_aorgn;

    // @In
    // @Description amount of nitrogen stored in the fresh organic (residue) pool, kg N/ha
    float **sol_fon;

    // @In
    // @Description amount of nitrogen stored in the stable organic N pool, kg N/ha
    float **sol_orgn;

    // @In
    // @Description amount of phosphorus stored in the organic P pool, kg P/ha
    float **sol_orgp;

    // @In
    // @Description amount of phosphorus stored in the fresh organic (residue) pool, kg P/ha
    float **sol_fop;

    // @In
    // @Description amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool, kg P/ha
    float **sol_stap;

    // @In
    // @Description amount of phosphorus stored in the active mineral phosphorus pool, kg P/ha
    float **sol_actp;

    // @In
    // @Description for C-FARM one carbon model
    float **sol_mp;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LSN;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LMN;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_HPN;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_HSN;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_HPC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_HSC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LMC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LSC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LS;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LM;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LSL;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LSLC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_LSLNC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_BMC;

    // @In
    // @Description CENTURY C/Y cycling model
    float **sol_WOC;

    // @In
    // @Description the amount of water percolated from the soil water reservoir, i.e., groundwater recharge
    float **Perco;

    // @In
    // @Description Subsurface runoff
    float **SSRU;

    /// outputs

    // @Out
    // @Description lateral flow Carbon loss in each soil layer
    float **sol_latc; 

    // @Out
    // @Description percolation Carbon loss in each soil layer
    float **sol_percoc; 

    // @Out
    // @Description lateral flow Carbon loss in soil profile
    float *latc; 

    // @Out
    // @Description percolation Carbon loss in soil profile
    float *percoc; 

    // @Out
    // @Description amount of C lost with sediment pools
    float *sedc;

private:

    //! subbasin IDs
    vector<int> m_subbasinIDs;

    /// subbasins information
    clsSubbasins *m_subbasinsInfo;

    // cell area of grid map (ha)
    float cellArea;

    // the total number of subbasins
    int nSubbasins;


private:

    /*!
     * \brief check the input data. Make sure all the input data is available.
     * \return bool The validity of the input data.
     */
    bool CheckInputData(void);

    /*!
     * \brief check the input data for running CENTURY model. Make sure all the inputs data is available.
     * \return bool The validity of the inputs data.
     */
    bool CheckInputData_CENTURY(void);

    /*!
     * \brief check the input data for running C-FARM one carbon model. Make sure all the inputs data is available.
     * \return bool The validity of the inputs data.
     */
    bool CheckInputData_CFARM(void);

    /*!
     * \brief check the input size. Make sure all the input data have same dimension.
     *
     * \param[in] key The key of the input data
     * \param[in] n The input data dimension
     * \return bool The validity of the dimension
     */
    bool CheckInputSize(const char *, int);

    /*!
     * \brief calculates the amount of organic nitrogen removed in surface runoff.
     *        orgn.f of SWAT, when CSWAT = 0
     * \return void
     */
    void OrgNRemovedInRunoff_StaticMethod(int i);

    /*!
     * \brief calculates the amount of organic nitrogen removed in surface runoff.
     *        orgnswat.f of SWAT, when CSWAT = 1
     * \TODO THIS IS ON TODO LIST
     * \return void
     */
    void OrgNRemovedInRunoff_CFARMOneCarbonModel(int i);

    /*!
     * \brief calculates the amount of organic nitrogen removed in surface runoff.
     *        NCsed_leach.f90 of SWAT, when CSWAT = 2
     * \return void
     */
    void OrgNRemovedInRunoff_CENTURY(int i);

    /*!
     * \brief Calculates the amount of organic and mineral phosphorus attached to sediment in surface runoff.
     * psed.f of SWAT
     * \return void
     */
    void OrgPAttachedtoSed(int i);

    /// initial outputs
    void initialOutputs(void);
};

VISITABLE_STRUCT(NutrientTransportSediment, m_nCells, CELLWIDTH, soillayers, nSoiLayers, rock, sol_ul, cswat, enratio, SED_OL, OL_Flow, density, 
	soilthick, subbasin, sedorgn, sedorgp, sedminpa, sedminps, sedorgnToCh, sedorgpToCh, sedminpaToCh, sedminpsToCh, sol_aorgn, sol_fon,
	sol_orgn, sol_orgp, sol_fop, sol_stap, sol_actp, sol_mp, sol_LSN, sol_LMN, sol_HPN, sol_HSN, sol_HPC, sol_HSC, sol_LMC, sol_LSC, sol_LS, sol_LM,
	sol_LSL, sol_LSLC, sol_LSLNC, sol_BMC, sol_WOC, Perco, SSRU, sol_latc, sol_percoc, latc, percoc, sedc);