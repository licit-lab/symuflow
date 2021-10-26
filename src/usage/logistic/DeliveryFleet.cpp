#include "stdafx.h"
#include "DeliveryFleet.h"

#include "Logger.h"
#include "reseau.h"
#include "entree.h"
#include "sortie.h"
#include "RepartitionTypeVehicule.h"
#include "usage/TripNode.h"
#include "usage/TripLeg.h"
#include "usage/Trip.h"
#include "usage/Schedule.h"
#include "usage/logistic/PointDeLivraison.h"
#include "usage/logistic/DeliveryFleetParameters.h"
#include "Xerces/XMLUtil.h"

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMXPathResult.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

XERCES_CPP_NAMESPACE_USE

DeliveryFleet::DeliveryFleet()
{
}

DeliveryFleet::DeliveryFleet(Reseau * pNetwork)
    : AbstractFleet(pNetwork)
{
}

DeliveryFleet::~DeliveryFleet()
{
}

AbstractFleetParameters * DeliveryFleet::CreateFleetParameters()
{
    return new DeliveryFleetParameters();
}


bool DeliveryFleet::Load(DOMNode * pXMLReseau, Logger & loadingLogger)
{
    // SECTION RESEAU / PARAMETRAGE_LIVRAISONS
    DOMNode * pXMLNode = m_pNetwork->GetXMLUtil()->SelectSingleNode("./PARAMETRAGE_LIVRAISONS", pXMLReseau->getOwnerDocument(), (DOMElement*)pXMLReseau);
    
    if(pXMLNode)
    {        
        std::string strTmp;

        // Chargement des param�tres d'usage des types de v�hicules
        LoadUsageParams(pXMLNode, loadingLogger);

        // Chargement des centres logistiques
        // SECTION RESEAU / PARAMETRAGE_LIVRAISONS / CENTRES_LOGISTIQUES
        DOMXPathResult * pLogisticCenters = m_pNetwork->GetXMLUtil()->SelectNodes("./CENTRES_LOGISTIQUES/CENTRE_LOGISTIQUE", pXMLNode->getOwnerDocument(), (DOMElement*)pXMLNode);
        XMLSize_t nbCenters = pLogisticCenters->getSnapshotLength();
        for(XMLSize_t iCenter = 0; iCenter < nbCenters; iCenter++)
        {
            pLogisticCenters->snapshotItem(iCenter);
            DOMNode* pNodeCenter = pLogisticCenters->getNodeValue();

            // R�cup�ration de l'identifiant du centre logistique
            GetXmlAttributeValue(pNodeCenter, "id", strTmp, &loadingLogger);

            // R�cup�ration de la position du centre logistique
            DOMNode * pOutputNode = m_pNetwork->GetXMLUtil()->SelectSingleNode("./POSITION_SORTIE/POSITION_CONNEXION", pNodeCenter->getOwnerDocument(), (DOMElement*)pNodeCenter);
            Position outputPosition;
            if(!outputPosition.Load(pOutputNode, strTmp, m_pNetwork, loadingLogger))
            {
                return false;
            }

            DOMNode * pInputNode = m_pNetwork->GetXMLUtil()->SelectSingleNode("./POSITION_ENTREE/POSITION_CONNEXION", pNodeCenter->getOwnerDocument(), (DOMElement*)pNodeCenter);
            Position inputPosition;
            if(!inputPosition.Load(pInputNode, strTmp, m_pNetwork, loadingLogger))
            {
                return false;
            }

            TripNode * pCenter = new TripNode(strTmp, m_pNetwork);
            pCenter->SetOutputPosition(outputPosition);
            pCenter->SetInputPosition(inputPosition);
            m_LstTripNodes.push_back(pCenter);
        }
        pLogisticCenters->release();

        // Chargement des points de livraison
        // SECTION RESEAU / PARAMETRAGE_LIVRAISONS / POINTS_DE_LIVRAISON
        DOMXPathResult * pDeliveryPts = m_pNetwork->GetXMLUtil()->SelectNodes("./POINTS_DE_LIVRAISON/POINT_DE_LIVRAISON", pXMLNode->getOwnerDocument(), (DOMElement*)pXMLNode);
        XMLSize_t nbPoints = pDeliveryPts->getSnapshotLength();
        for(XMLSize_t iPoint = 0; iPoint < nbPoints; iPoint++)
        {
            pDeliveryPts->snapshotItem(iPoint);
            DOMNode* pNodePoint = pDeliveryPts->getNodeValue();

            // R�cup�ration de l'identifiant du point de livraison
            GetXmlAttributeValue(pNodePoint, "id", strTmp, &loadingLogger);

            PointDeLivraison * pPoint = new PointDeLivraison(strTmp, m_pNetwork);

            // R�cup�ration de la position du point de livraison
            DOMNode * pPositionNode = m_pNetwork->GetXMLUtil()->SelectSingleNode("./POSITION/POSITION_TRONCON", pNodePoint->getOwnerDocument(), (DOMElement*)pNodePoint);
            Position position;
            if(!position.Load(pPositionNode, strTmp, m_pNetwork, loadingLogger))
            {
                return false;
            }

            pPoint->SetOutputPosition(position);
            pPoint->SetInputPosition(position);

            // R�cup�ration du nombre de places disponibles
            int nbPlaces;
            if(GetXmlAttributeValue(pNodePoint, "nb_places", nbPlaces, &loadingLogger))
            {
                // Avertissement si le point de livraison n'est pas hors voie :
                if(!position.IsOutside())
                {
                    loadingLogger << Logger::Warning << "WARNING : a number of parking spots is defined for delivery point " << pPoint->GetID() << " whereas it is not off-lane. This number will be ignored." << std::endl;
                    loadingLogger << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
                }
                else
                {
                    pPoint->SetNbPlaces(nbPlaces);
                }
            }

            m_LstTripNodes.push_back(pPoint);
        }
        pDeliveryPts->release();

        // Pas besoin de r�cup pour chaque tourn�e ce noeud : c'est le m�me pour toutes.
        DOMNode * XMLRepTypes = m_pNetwork->GetXMLUtil()->SelectSingleNode("./REP_TYPEVEHICULES", pXMLNode->getOwnerDocument(), (DOMElement*)pXMLNode);
        
        // SECTION RESEAU / PARAMETRAGE_LIVRAISONS / TOURNEES_DE_LIVRAISON
        DOMXPathResult * pTournees = m_pNetwork->GetXMLUtil()->SelectNodes("./TOURNEES_DE_LIVRAISON/TOURNEE_DE_LIVRAISON", pXMLNode->getOwnerDocument(), (DOMElement*)pXMLNode);
        XMLSize_t nbTournees = pTournees->getSnapshotLength();
        for(XMLSize_t iTournee = 0; iTournee < nbTournees; iTournee++)
        {
            pTournees->snapshotItem(iTournee);
            DOMNode* pNodeTournee = pTournees->getNodeValue();

            // R�cup�ration de l'identifiant de la tourn�e
            GetXmlAttributeValue(pNodeTournee, "id", strTmp, &loadingLogger);

            // Cr�ation de la tourn�e
            Trip * pTournee = new Trip();
            pTournee->SetID(strTmp);
            m_LstTrips.push_back(pTournee);

            // D�finition de la r�partition des types de vehicules pour la tourn�e
            RepartitionTypeVehicule * pRepTypes = new RepartitionTypeVehicule();
            std::vector<TypeVehicule*> lstTypes;
            for(std::map<TypeVehicule*, VehicleTypeUsageParameters>::const_iterator iter = m_MapUsageParameters.begin(); iter != m_MapUsageParameters.end(); ++iter)
            {
                lstTypes.push_back(iter->first);
            }
            if(!m_pNetwork->LoadRepTypeVehicules(XMLRepTypes, pTournee->GetID(), 1, lstTypes, pRepTypes, &loadingLogger))
            {
                return false;
            }
            m_MapRepartitionTypeVehicule[pTournee] = pRepTypes;

            // Chargement du calendrier associ�
            Schedule * pSchedule = new Schedule();
            DOMNode * pXMLNodeSchedule = m_pNetwork->GetXMLUtil()->SelectSingleNode("./CALENDRIER", pNodeTournee->getOwnerDocument(), (DOMElement*)pNodeTournee);
            if(pXMLNodeSchedule)
            {
                std::map<std::string, ScheduleParameters*> placeHolder;
                if(!pSchedule->Load(pXMLNodeSchedule, m_pNetwork, loadingLogger, pTournee->GetID(), placeHolder, m_MapScheduleParameters))
                {
                    return false;
                }
            }
            m_MapSchedules[pTournee] = pSchedule;

            // r�cup�ration de l'origine
            DOMNode * pNodeOrigin = m_pNetwork->GetXMLUtil()->SelectSingleNode("./ORIGINE", pNodeTournee->getOwnerDocument(), (DOMElement*)pNodeTournee);
            GetXmlAttributeValue(pNodeOrigin, "id", strTmp, &loadingLogger);

            // On regarde s'il s'agit d'un centre logistique
            TripNode * pOrigin = GetTripNode(strTmp);
            if(!pOrigin)
            {
                // On regarde s'il s'agit d'une entr�e
                char cTmp;
                pOrigin = dynamic_cast<Entree*>(m_pNetwork->GetOrigineFromID(strTmp, cTmp));
                if(!pOrigin)
                {
                    loadingLogger << Logger::Error << " ERROR : Unable to find the input macthing " << strTmp << " for the delivery tour " << pTournee->GetID() << std::endl;
                    loadingLogger << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
                    return false;
                }
            }
            pTournee->SetOrigin(pOrigin);

            // Traitement de chaque point de livraison de la tourn�e !
            pDeliveryPts = m_pNetwork->GetXMLUtil()->SelectNodes("./POINTS_DE_LIVRAISON/POINT_DE_LIVRAISON", pNodeTournee->getOwnerDocument(), (DOMElement*)pNodeTournee);
            nbPoints = pDeliveryPts->getSnapshotLength();
            for(XMLSize_t iPoint = 0; iPoint < nbPoints; iPoint++)
            {
                pDeliveryPts->snapshotItem(iPoint);
                DOMNode* pNodePoint = pDeliveryPts->getNodeValue();

                // R�cup�ration de l'identifiant du point de livraison
                GetXmlAttributeValue(pNodePoint, "id", strTmp, &loadingLogger);

                TripNode * pDest = GetTripNode(strTmp);

                if(!pDest)
                {
                    loadingLogger << Logger::Error << " ERROR : Unable to find the delivery point " << strTmp << " for the tour " << pTournee->GetID() << std::endl;
                    loadingLogger << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
                    return false;
                }

                // rmq. le calcul de l'itin�raire est fait au moment de la cr�ation du v�hicule pour avoir le type de v�hicule correspondant
                TripLeg * pTripLeg = new TripLeg();
                pTripLeg->SetDestination(pDest);
                pTournee->AddTripLeg(pTripLeg);

                // R�cup�ration de la quantit� de marchandises pour le point de livraison et la tourn�e en cours de construction
                int iLoad, iUnload;
                GetXmlAttributeValue(pNodePoint, "dechargement", iUnload, &loadingLogger);
                GetXmlAttributeValue(pNodePoint, "chargement", iLoad, &loadingLogger);
                m_MapLoads[pTournee][pDest].push_back(std::make_pair(iLoad, iUnload));
            }
            pDeliveryPts->release();

            // r�cup�ration de la destination
            DOMNode * pNodeDestination = m_pNetwork->GetXMLUtil()->SelectSingleNode("./DESTINATION", pNodeTournee->getOwnerDocument(), (DOMElement*)pNodeTournee);
            GetXmlAttributeValue(pNodeDestination, "id", strTmp, &loadingLogger);

            // On regarde s'il s'agit d'un centre logistique
            TripNode * pDest = GetTripNode(strTmp);
            if(!pDest)
            {
                // On regarde s'il s'agit d'une sortie
                char cTmp;
                pDest = dynamic_cast<Sortie*>(m_pNetwork->GetDestinationFromID(strTmp, cTmp));
                if(!pDest)
                {
                    loadingLogger << Logger::Error << " ERROR : Unable to find the exit matching " << strTmp << " for tour " << pTournee->GetID() << std::endl;
                    loadingLogger << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
                    return false;
                }
            }

            TripLeg * pTripLeg = new TripLeg();
            pTripLeg->SetDestination(pDest);
            pTournee->AddTripLeg(pTripLeg);
        }
        pTournees->release();
    }

    return true;
}

// Effectue les op�rations � faire � l'activation d'un v�hicule
void DeliveryFleet::OnVehicleActivated(boost::shared_ptr<Vehicule> pVeh, double dbInstant)
{
    // Appel de la m�thode de la classe m�re
    AbstractFleet::OnVehicleActivated(pVeh, dbInstant);

    Trip * pTrip = GetTrip(pVeh->GetTrip()->GetID());

    // d�finition du chargement initial du v�hicule comme la somme des chargements � d�poser aux diff�rents points de livraison de la tourn�e
    std::map<Trip*,std::map<TripNode*,std::vector<std::pair<int,int> > > > ::const_iterator iter = m_MapLoads.find(pTrip);
    if(iter != m_MapLoads.end())
    {
        // Recopie des param�tres de chargement et d�chargement � partir de la d�finition actuelle de la tourn�e
        DeliveryFleetParameters * pParams = (DeliveryFleetParameters*)pVeh->GetFleetParameters();
        pParams->GetMapLoadsUnloads() = iter->second;

        int iUnload = 0;
        std::map<TripNode*,std::vector<std::pair<int,int> > >::const_iterator iter2;
        for(iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
        {
            for(size_t i = 0; i < iter2->second.size(); i++)
            {
                iUnload += iter2->second[i].second;
            }
        }
        LoadUsageParameters & loadParams = pVeh->GetFleetParameters()->GetUsageParameters().GetLoadUsageParameters();
        loadParams.SetCurrentLoad(iUnload);
        if(iUnload > loadParams.GetMaxLoad())
        {
            m_pNetwork->log() << Logger::Warning << "The maximum load capacity (" << loadParams.GetMaxLoad() << ") of vehicle " << pVeh->GetID() << " has been reached." << std::endl;
            m_pNetwork->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
        }
    }
}

double DeliveryFleet::GetStopDuration(boost::shared_ptr<Vehicule> pVehicle, TripNode * pTripNode, bool bIsRealStop)
{
    // valeur par d�faut : appel de la m�thode de la classe m�re (l'arr�t dure un pas de temps)
    double dbStopDuration = AbstractFleet::GetStopDuration(pVehicle, pTripNode, bIsRealStop);

    DeliveryFleetParameters * pParams = (DeliveryFleetParameters*)pVehicle->GetFleetParameters();
    std::pair<int,int> loadUnload = GetLoadFromMap(pParams->GetMapLoadsUnloads(), pVehicle->GetTrip(), pTripNode);

    LoadUsageParameters & loadParams = pVehicle->GetFleetParameters()->GetUsageParameters().GetLoadUsageParameters();
    dbStopDuration = loadParams.ComputeLoadTime(loadUnload.first, loadUnload.second);

    if(bIsRealStop)
    {
        loadParams.SetCurrentLoad(loadParams.GetCurrentLoad() - loadUnload.second + loadUnload.first);
        if(loadParams.GetCurrentLoad() > loadParams.GetMaxLoad())
        {
            m_pNetwork->log() << Logger::Warning << "The maximum load capacity (" << loadParams.GetMaxLoad() << ") of vehicle " << pVehicle->GetID() << " has been reached." << std::endl;
            m_pNetwork->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
        }
        if(loadParams.GetCurrentLoad() < 0)
        {
            // Peut arriver si on fait du pilotage des livraisons sur ordre et qu'on demande de d�charger plus que ce que le v�hicule n'a comme stock
            m_pNetwork->log() << Logger::Warning << "The load of vehicle " << pVehicle->GetID() << " went negative." << std::endl;
            m_pNetwork->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
        }
    }

    // On ne peut pas s'arr�ter moins qu'un pas de temps
    dbStopDuration = std::max<double>(dbStopDuration, m_pNetwork->GetTimeStep());
    return dbStopDuration;
}


std::map<std::string, std::string> DeliveryFleet::GetOutputAttributes(Vehicule * pV, TripNode * pTripNode)
{
    std::map<std::string, std::string> result;
    result["point_livraison"] = pTripNode->GetID();
    return result;
}

bool DeliveryFleet::AddDeliveryPointToTrip(Trip * pTrip, PointDeLivraison * pTripNode, int positionIndex, Vehicule * pVehicle, int dechargement, int chargement)
{
    // le positionIndex correspond � un indice de PointDeLivraison. Du coup, on converti en indice de TripLeg
    // Pour avoir quelquechose de g�n�rique et coh�rent avec la d�finition g�n�rale d'un Trip.
    size_t tripLegIndex = GetTripLegIndexFromDeliveryPointIndex(pTrip, positionIndex);

    bool bOk = AbstractFleet::AddTripNodeToTrip(pTrip, pTripNode, tripLegIndex, pVehicle);

    if(bOk)
    {
        // D�termination du num�ro de passage au point de livraison qu'il faut modifier
        int loadIndex = pTrip->GetPassageNb(pTripNode, tripLegIndex);

        // Vecteur � modifier
        std::vector<std::pair<int,int> > * pLoads;
        if(pVehicle)
        {
            DeliveryFleetParameters * pParams = (DeliveryFleetParameters*)pVehicle->GetFleetParameters();
            pLoads = &pParams->GetMapLoadsUnloads()[pTripNode];
        }
        else
        {
            pLoads = &m_MapLoads[pTrip][pTripNode];
        }

        pLoads->insert(pLoads->begin() + loadIndex, std::make_pair(chargement,dechargement));

    }

    return bOk;
}


bool DeliveryFleet::RemoveDeliveryPointFromTrip(Trip * pTrip, int positionIndex, Vehicule * pVehicle)
{
    // le positionIndex correspond � un indice de PointDeLivraison. Du coup, on converti en indice de TripLeg
    // Pour avoir quelquechose de g�n�rique et coh�rent avec la d�finition g�n�rale d'un Trip.
    size_t tripLegIndex = GetTripLegIndexFromDeliveryPointIndex(pTrip, positionIndex);

    // D�termination du num�ro de passage au point de livraison qu'il faut modifier
    TripNode * pTripNode = pTrip->GetLegs()[tripLegIndex]->GetDestination();
    int loadIndex = pTrip->GetPassageNb(pTripNode, tripLegIndex);

    bool bOk = AbstractFleet::RemoveTripLegFromTrip(pTrip, tripLegIndex, pVehicle);

    if(bOk)
    {
        // Map � modifier
        std::map<TripNode*, std::vector<std::pair<int,int> > > * pMapLoads;
        if(pVehicle)
        {
            DeliveryFleetParameters * pParams = (DeliveryFleetParameters*)pVehicle->GetFleetParameters();
            pMapLoads = &pParams->GetMapLoadsUnloads();
        }
        else
        {
            pMapLoads = &m_MapLoads[pTrip];
        }

        // On supprime l'item de la map s'il est vide
        std::map<TripNode*, std::vector<std::pair<int,int> > >::iterator iter = pMapLoads->find(pTripNode);
        if(iter != pMapLoads->end())
        {
            iter->second.erase(iter->second.begin() + loadIndex);
            if(iter->second.empty())
            {
                pMapLoads->erase(iter);
            }
        }
    }

    return bOk;
}

std::pair<int,int> DeliveryFleet::GetLoadFromMap(const std::map<TripNode*, std::vector<std::pair<int,int> > > & mapLoads, Trip * pTrip, TripNode * pTripNode)
{
    std::pair<int,int> result = std::make_pair(0, 0);
    std::map<TripNode*,std::vector<std::pair<int,int> > >::const_iterator iter = mapLoads.find(pTripNode);
    if(iter != mapLoads.end())
    {
        if(iter->second.size() == 1)
        {
            result = iter->second.front();
        }
        else
        {
            int passageNb = pTrip->GetPassageNbToCurrentLeg(pTripNode);
            result = iter->second[passageNb];
        }
    }
    return result;
}

size_t DeliveryFleet::GetTripLegIndexFromDeliveryPointIndex(Trip * pTrip, int positionIndex)
{
    int iPointIndex = 0;
    size_t tripLegIndex;
    for(tripLegIndex = 0; tripLegIndex < pTrip->GetLegs().size(); tripLegIndex++)
    {
        PointDeLivraison * pPoint = dynamic_cast<PointDeLivraison*>(pTrip->GetLegs()[tripLegIndex]->GetDestination());
        if(pPoint)
        {
            if(positionIndex == iPointIndex)
            {
                break;
            }
            iPointIndex++;
        }
    }
    return tripLegIndex;
}

std::string DeliveryFleet::GetVehicleIdentifier(boost::shared_ptr<Vehicule> pVeh)
{
    std::ostringstream oss;
    oss << "DELIVERY_" << pVeh->GetTrip()->GetID();
    return oss.str();
}


template void DeliveryFleet::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void DeliveryFleet::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void DeliveryFleet::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AbstractFleet);

    ar & BOOST_SERIALIZATION_NVP(m_MapLoads);
}

