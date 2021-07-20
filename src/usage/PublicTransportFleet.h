#pragma once
#ifndef PublicTransportFleetH
#define PublicTransportFleetH

#include "AbstractFleet.h"

class TypeVehicule;

class PublicTransportFleet : public AbstractFleet
{
public:
    PublicTransportFleet();
    PublicTransportFleet(Reseau * pNetwork);
    virtual ~PublicTransportFleet();

    // Instanciation de l'objet sp�cifique � la flotte contenant les param�tres d'un v�hicule li�s � celle-ci
    virtual AbstractFleetParameters * CreateFleetParameters();

    // r�cup�ration du type de v�hicule associ� � une ligne de bus
    TypeVehicule * GetTypeVehicule(Trip * pLine);

    virtual bool Load(XERCES_CPP_NAMESPACE::DOMNode * pXMLNetwork, Logger & loadingLogger);

    // Indique s'il faut sortir la charge courante du v�hicule ou non
    virtual bool OutputVehicleLoad(Vehicule * pVehicle);

    // Construit le libell� du v�hicule, utilis� par exemple pour les bus
    virtual std::string GetVehicleIdentifier(boost::shared_ptr<Vehicule> pVeh);

protected:
    // Param�tres des v�hicules li�s � un �l�ment de calendrier
    std::map<Trip*, std::map<std::string, ScheduleParameters*> > m_MapScheduleParametersByTrip;

    // Indique si la dur�e des arr�ts est dynamique ou non pour chaque ligne de bus
    std::map<Trip*, bool> m_MapDureeArretsDynamique;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


#endif // PublicTransportFleetH