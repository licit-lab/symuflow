#pragma once
#ifndef PointDeLivraisonH
#define PointDeLivraisonH

#include "usage/TripNode.h"

// Sp�cialisation de la classe TripNode pour g�rer l'�ventuelle limite en places d'un point de livraison
class PointDeLivraison : public TripNode
{
public:
    PointDeLivraison();
    PointDeLivraison(const std::string & strID, Reseau * pNetwork);
    virtual ~PointDeLivraison();

    void SetNbPlaces(int nbPlaces);

    // Sp�cialisation du traitement sur entr�e d'un v�hicule dans le Point de livraison pour bloquer le v�hicule si le nombre de places max est atteint
    virtual void VehiculeEnter(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep, bool bForcedOutside = false);

protected:

    // Nombre de places sur l'aire de livraison
    int m_nNbPlaces;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // PointDeLivraisonH