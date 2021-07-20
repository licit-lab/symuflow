#include "stdafx.h"
#include "PointDeLivraison.h"

PointDeLivraison::PointDeLivraison()
    : TripNode()
{
    // Les v�hicules doivent s'arr�ter au point de livraison.
    m_bStopNeeded = true;
    m_bListVehiclesInTripNode = true;

    m_nNbPlaces = INT_MAX;
}

PointDeLivraison::PointDeLivraison(const std::string & strID, Reseau * pNetwork)
    : TripNode(strID, pNetwork)
{
    // Les v�hicules doivent s'arr�ter au point de livraison.
    m_bStopNeeded = true;
    m_bListVehiclesInTripNode = true;

    m_nNbPlaces = INT_MAX;
}

PointDeLivraison::~PointDeLivraison()
{
}

void PointDeLivraison::SetNbPlaces(int nbPlaces)
{
    m_nNbPlaces = nbPlaces;
}

void PointDeLivraison::VehiculeEnter(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep, bool bForcedOutside)
{
    // Si le point de livraison est complet, on laisse le v�hicule immobile, sinon on le traite normalement
    if((int)m_LstStoppedVehicles.size() < m_nNbPlaces)
    {
        // Appel de la m�thode de la classe m�re
        TripNode::VehiculeEnter(pVehicle, pDestinationEnterLane, dbInstDestinationReached, dbInstant, dbTimeStep, bForcedOutside);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void PointDeLivraison::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void PointDeLivraison::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void PointDeLivraison::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TripNode);

    ar & BOOST_SERIALIZATION_NVP(m_nNbPlaces);
}