#pragma once
#ifndef traveltimsoutputmanagerH
#define traveltimsoutputmanagerH

#pragma warning(disable : 4003)
#include <rapidjson/document.h>
#pragma warning(default : 4003)

class Reseau;
class SensorsManager;
class Tuyau;
class AbstractSensorSnapshot;
class SimulationSnapshot;

#include <vector>
#include <map>

class TravelTimesOutputManagerSnapshot
{
public:
    int                                         nPeriode;
    std::map<Tuyau*, std::pair<int, double> >   mapTravelTimes;
    std::vector<AbstractSensorSnapshot*>        sensorsSnapshots;
};

class TravelTimesOutputManager
{
public:
    TravelTimesOutputManager();
    TravelTimesOutputManager(Reseau * pNetwork);
    virtual ~TravelTimesOutputManager();

    void Restore(TravelTimesOutputManagerSnapshot* pSnapshot);
    TravelTimesOutputManagerSnapshot * TakeSnapshot();


    void AddMeasuredTravelTime(Tuyau * pLink, double dbMeasuredTravelTime);

    void CalculInfoCapteurs(double dbInstant, bool bTerminate = false);

    // passage en mode d'�criture dans fichiers temporaires (affectation dynamique)
    void doUseTempFiles(SimulationSnapshot* pSimulationSnapshot);
    // validation de la derni�re p�riode d'affectation et recopie des fichiers temporaires dans les fichiers finaux
    void converge(SimulationSnapshot * pSimulationSnapshot);

private:
    Reseau*                                     m_pNetwork;

    SensorsManager*                             m_pSensorsManager;

    rapidjson::Document                         m_TravelTimes;

    SimulationSnapshot*                         m_pCurrentSimulationSnapshot;
    
    // variables de travail
    int                                         m_nPeriode;         // Num�ro de la p�riode courante
    std::map<Tuyau*, std::pair<int, double> >   m_mapTravelTimes;
};

#endif // traveltimsoutputmanagerH
