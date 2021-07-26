#pragma once
#ifndef PublicTransportScheduleParametersH
#define PublicTransportScheduleParametersH

#include "ScheduleParameters.h"

#include <vector>
#include <map>

class Arret;
class Trip;

class PublicTransportScheduleParameters : public ScheduleParameters
{

public:
    PublicTransportScheduleParameters();
    virtual ~PublicTransportScheduleParameters();

    // Chargement de du jeu de param�tres � partir du noeud XML correspondant
    virtual bool Load(XERCES_CPP_NAMESPACE::DOMNode * pXMLNode, Logger & loadingLogger);

    // Association des dur�es d'arr�ts et des arr�ts
    bool AssignStopTimesToStops(Trip * pLigne, Logger & loadingLogger);

    // Accesseur aux dur�es d'arr�t d�finies
    const std::map<Arret*, double> & GetStopTimes();

protected:

    // Liste des dur�es d'arr�ts telles que d�finie dans le noeud XML
    std::vector<double> m_StopTimes;

    // Association entre les arr�ts et les dur�es d'arr�ts d�finies
    std::map<Arret*, double> m_mapStopTimes;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // PublicTransportScheduleParametersH
