/*!
 * \brief Simulates depressional areas that do not drain to the stream network (pothole) and impounded areas such as rice paddies
 * \author Liang-Jun Zhu
 * \date Sep 2016
 *           1. Source code of SWAT include: pothole.f
 *           2. Add the simulation of Ammonia n transported with surface runoff, 2016-9-27
 *           3. Add DEET and DPST from DEP_LENSLEY module
 *           4. Using a simple model (first-order kinetics equation) to simulate N transformation in impounded area.
 * \data 2016-10-10
 * \description: 1. Update all related variables after the simulation of pothole.
 */
#pragma once

#include <visit_struct/visit_struct_intrusive.hpp>
#include <visit_struct/visit_struct.hpp>
#include "SimulationModule.h"

using namespace std;

class IMP_SWAT : public SimulationModule {

    // @In
    // @Description valid cells number
    int m_nCells;

    // @In
    // @Description cell width, m
    float CELLWIDTH;
   
    // @In
    // @Description timestep, sec
    float TIMESTEP;

    // @In
    // @Description soil layers
    float *soillayers;

    // @In
    // @Description max soil layers
    int nSoilLayers;

    // @In
    // @Description subbasin ID
    float *subbasin;

    // @In
    // @Description subbasin number
    int subbasinNum;

    // @In
    // @Description Routing layers according to the flow direction
    float **ROUTING_LAYERS;

    // @In
    // @Description number of routing layers
    int nRoutingLayers;

    // @In
    // @Description leaf area index at which no evaporation occurs from water surface
    float evlai;

    // @In
    // @Description slope gradient (%)
    float *slope;

    // @In
    // @Description saturated conductivity
    float **Conductivity;

    // @In
    // @Description saturated soil water, mm
    float **sol_ul;

    // @In
    // @Description field capacity on soil profile (mm, FC-WP)
    float *sol_sumAWC;

    // @In
    // @Description soil thickness
    float **soilthick;

    // @In
    // @Description porosity mm/mm
    float **Porosity;

    // @In
    // @Description Average daily outflow to main channel from tile flow if drainage tiles are installed in the pothole, mm
    float pot_tilemm;

    // @In
    // @Description Nitrate decay rate in impounded water body
    float pot_no3l;

    // @In
    // @Description Soluble phosphorus decay rate in impounded water body
    float pot_solpl;

    // @In
    // @Description volatilization rate constant in impounded water body, /day
    float kv_paddy;

    // @In
    // @Description nitrification rate constant in impounded water body, /day
    float kn_paddy;

    // @In
    // @Description hydraulic conductivity of soil surface of pothole, mm/hr
    float pot_k;

    // @In
    // @Description impounding trigger
    float *impound_trig;

    // @Out
    // @Description surface area of impounded area, ha
    float *pot_sa;

    // @In
    // @Description lai in the current day
    float *LAIDAY;

    // @In
    // @Description pet
    float *PET;

    // @In
    // @Description evaporation from depression, mm
    float *DEET;

    // @In
    // @Description depression storage, mm
    float *DPST;

    // @In
    // @Description surface runoff, mm
    float *OL_Flow;

    // @In
    // @Description sediment yield transported on each cell, kg
    float *SED_OL;

    // @In
    // @Description sand yield
    float *sand_yld;

    // @In
    // @Description silt yield
    float *silt_yld;

    // @In
    // @Description clay yield
    float *clay_yld;

    // @In
    // @Description small aggregate yield
    float *sag_yld;

    // @In
    // @Description large aggregate yield
    float *lag_yld;

    // @In
    // @Description amount of water stored in soil layers on current day, sol_st in SWAT
    float **solst;

    // @In
    // @Description amount of water stored in soil profile on current day, sol_sw in SWAT
    float *solsw;

    // @In
    // @Description amount of nitrate transported with surface runoff, kg/ha
    float *sur_no3;

    // @In
    // @Description amount of ammonian transported with surface runoff, kg/ha
    float *sur_nh4;

    // @In
    // @Description amount of soluble phosphorus transported with surface runoff, kg/ha
    float *sur_solp;

    // @In
    // @Description kg/ha
    float *sur_cod;

    // @In
    // @Description kg/ha
    float *sedorgn;

    // @In
    // @Description kg/ha
    float *sedorgp;

    // @In
    // @Description kg/ha
    float *sedminpa;

    // @In
    // @Description kg/ha
    float *sedminps;

    // @Out
    // @Description no3 amount kg
    float *pot_no3;

    // @Out
    // @Description nh4 amount kg
    float *pot_nh4;

    // @Out
    // @Description orgN amount kg
    float *pot_orgn;

    // @Out
    // @Description soluble phosphorus amount, kg
    float *pot_solp;

    // @Out
    // @Description orgP amount kg
    float *potOrgP;

    // @Out
    // @Description active mineral P kg
    float *potActMinP;

    // @Out
    // @Description stable mineral P kg
    float *potStaMinP;

    // @Out
    // @Description sediment amount kg
    float *potSed;

    // @Out
    // @Description sand
    float *potSand;

    // @Out
    // @Description silt
    float *potSilt;

    // @Out
    // @Description clay
    float *potClay;

    // @Out
    // @Description small aggregate
    float *potSag;

    // @Out
    // @Description large aggregate
    float *potLag;

    // @Out
    // @Description volume   mm
    float *pot_vol;

    // @In
    // @Description maximum volume mm
    float *pot_volmaxmm;

    // @In
    // @Description lowest volume mm
    float *pot_vollowmm;

    // @In
    // @Description seepage water of pothole, mm
    float *potSeep;

    // @In
    // @Description evaporation, mm
    float *potEvap;

    BEGIN_VISITABLES(IMP_SWAT);

    // @In
    // @Description surface runoff to channel, m^3/s
    //float *SBOF;
    VISITABLE(float *, SBOF);

    // @In
    // @Description sediment transported to channel, kg
    //float *SEDTOCH;
    VISITABLE(float *, SEDTOCH);

    // @In
    // @Description amount of nitrate transported with surface runoff
    //float *sur_no3_ToCh;
    VISITABLE(float *, sur_no3_ToCh);

    // @In
    // @Description amount of ammonian transported with surface runoff
    //float *SUR_NH4_TOCH;
    VISITABLE(float *, SUR_NH4_TOCH);

    // @In
    // @Description amount of soluble phosphorus in surface runoff
    //float *sur_solp_ToCh;
    VISITABLE(float *, sur_solp_ToCh);

    // @In
    // @Description cod to reach in surface runoff (kg)
    //float *sur_cod_ToCH;
    VISITABLE(float *, sur_cod_ToCH);

    // @In
    // @Description amount of organic nitrogen in surface runoff
    //float *sedorgnToCh;
    VISITABLE(float *, sedorgnToCh);

    // @In
    // @Description amount of organic phosphorus in surface runoff
    //float *sedorgpToCh;
    VISITABLE(float *, sedorgpToCh);

    // @In
    // @Description amount of active mineral phosphorus absorbed to sediment in surface runoff
    //float *sedminpaToCh;
    VISITABLE(float *, sedminpaToCh);

    // @In
    // @Description amount of stable mineral phosphorus absorbed to sediment in surface runoff
    //float *sedminpsToCh;
    VISITABLE(float *, sedminpsToCh);

    END_VISITABLES;

private:
    // conversion factor (mm/ha => m^3)
    float cnv;

    // cell area, ha
    float cellArea;


public:
    //! Constructor
    IMP_SWAT(void);

    //! Destructor
    ~IMP_SWAT(void);

    virtual int Execute(void);

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Set2DData(const char *key, int n, int col, float **data);

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

    /*!
     * \brief check the input size of 2D data. Make sure all the input data have same dimension.
     *
     *
     * \param[in] key The key of the input data
     * \param[in] n The first dimension input data 
     * \param[in] col The second dimension of input data
     * \return bool The validity of the dimension
     */
    bool CheckInputSize2D(const char *key, int n, int col);

    /// initialize all possible outputs
    void initialOutputs(void);

    /*!
     * \brief Simulates depressional areas that do not
     * drain to the stream network (pothole) and impounded areas such as rice paddies
     * reWrite from pothole.f of SWAT
     */
    void potholeSimulate(int id);

    /*!
     * compute surface area assuming a cone shape, ha
     */
    void potholeSurfaceArea(int id);

    /*!
     * release water stored in pothole
     */
    void releaseWater(int id);
};

VISITABLE_STRUCT(IMP_SWAT, m_nCells, CELLWIDTH, TIMESTEP, soillayers, nSoilLayers, subbasin, subbasinNum, 
	ROUTING_LAYERS, nRoutingLayers, evlai, slope, Conductivity, sol_ul, sol_sumAWC, soilthick, Porosity, pot_tilemm, 
	pot_no3l, pot_solpl, kv_paddy, kn_paddy, pot_k, impound_trig, pot_sa, LAIDAY, PET, DEET, DPST, OL_Flow, SED_OL, 
	sand_yld, silt_yld, clay_yld, sag_yld, lag_yld, solst, solsw, sur_no3, sur_nh4, sur_solp, sur_cod, sedorgn, sedorgp, 
	sedminpa, sedminps, pot_no3, pot_nh4, pot_orgn, pot_solp, potOrgP, potActMinP, potStaMinP, potSed, potSand, potSilt, 
	potClay, potSag, potLag, pot_vol, pot_volmaxmm, pot_vollowmm, potSeep, potEvap);