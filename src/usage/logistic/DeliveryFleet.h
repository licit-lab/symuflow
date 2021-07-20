#pragma once
#ifndef DeliveryFleetH
#define DeliveryFleetH

#include "../AbstractFleet.h"

class PointDeLivraison;

class DeliveryFleet : public AbstractFleet
{
public:
    DeliveryFleet();
    DeliveryFleet(Reseau * pNetwork);
    virtual ~DeliveryFleet();

    // Instanciation de l'objet sp�cifique � la flotte contenant les param�tres d'un v�hicule li�s � celle-ci
    virtual AbstractFleetParameters * CreateFleetParameters();

    virtual bool Load(XERCES_CPP_NAMESPACE::DOMNode * pXMLNetwork, Logger & loadingLogger);

    // Effectue les op�rations � faire � l'activation d'un v�hicule
    virtual void OnVehicleActivated(boost::shared_ptr<Vehicule> pVeh, double dbInstant);

    // Calcule la dur�e d'arr�t � un tripnode.
    virtual double GetStopDuration(boost::shared_ptr<Vehicule> pVehicle, TripNode * pTripNode, bool bIsRealStop);

    // Renvoie une map d'attributs � sortir de fa�on sp�cifique pour un v�hicule dans le TripNode
    virtual std::map<std::string, std::string> GetOutputAttributes(Vehicule * pV, TripNode * pTripNode);

    // Indique s'il faut sortir la charge courante du v�hicule ou non
    virtual bool OutputVehicleLoad(Vehicule * pVehicle) {return true;}

    // Ajoute un point de livraison � une tourn�e
    bool AddDeliveryPointToTrip(Trip * pTrip, PointDeLivraison * pTripNode, int positionIndex, Vehicule * pVehicle, int dechargement, int chargement);

    // Supprime un point de livraison d'une tourn�e
    bool RemoveDeliveryPointFromTrip(Trip * pTrip, int positionIndex, Vehicule * pVehicle);

    // Construit le libell� du v�hicule, utilis� par exemple pour les bus
    virtual std::string GetVehicleIdentifier(boost::shared_ptr<Vehicule> pVeh);

protected:

    // Fonction utilitaire pour r�cup�rer la valeur qui nous int�resse dans les maps membres de DeliveryFleet
    std::pair<int,int> GetLoadFromMap(const std::map<TripNode*, std::vector<std::pair<int,int> > > & mapLoads, Trip * pTrip, TripNode * pTripNode);

    // Fonction utilitaire pour d�terminer l'index d'un tripleg � partir de l'index d'un point de livraison
    size_t GetTripLegIndexFromDeliveryPointIndex(Trip * pTrip, int positionIndex);

protected:

    // Quantit�s � charger et d�charger par tourn�e et par point de livraison (il peut y en avoir plusieurs par point de livraison s'il appara�t plusieurs fois dans une tourn�e)
    std::map<Trip*, std::map<TripNode*, std::vector<std::pair<int,int> > > > m_MapLoads;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


#endif // DeliveryFleetH