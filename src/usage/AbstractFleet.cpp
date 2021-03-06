#include "stdafx.h"
#include "AbstractFleet.h"

#include "vehicule.h"
#include "TripNode.h"
#include "TripLeg.h"
#include "Trip.h"
#include "reseau.h"
#include "Schedule.h"
#include "Logger.h"
#include "RepartitionTypeVehicule.h"
#include "tuyau.h"
#include "voie.h"
#include "DiagFonda.h"
#include "Xerces/XMLUtil.h"
#include "SymuCoreManager.h"
#include "ScheduleParameters.h"
#include "VehicleToCreate.h"
#include "sortie.h"

#include "xercesc/dom/DOMNode.hpp"
#include "xercesc/dom/DOMXPathResult.hpp"

#include <boost/make_shared.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

XERCES_CPP_NAMESPACE_USE

AbstractFleetSnapshot::AbstractFleetSnapshot()
{
}

AbstractFleetSnapshot::~AbstractFleetSnapshot()
{
    std::map<TripNode*, TripNodeSnapshot*>::const_iterator iter;
    for(iter = mapTripNodeState.begin(); iter != mapTripNodeState.end(); ++iter)
    {
        delete iter->second;
    }

    std::map<Trip*, TripSnapshot*>::const_iterator iterTrip;
    for (iterTrip = mapTripState.begin(); iterTrip != mapTripState.end(); ++iterTrip)
    {
        delete iterTrip->second;
    }
}

bool AbstractFleet::LoadUsageParams(DOMNode * pXMLFleetNode, Logger & loadingLogger)
{
    DOMXPathResult * pXMLNodes = m_pNetwork->GetXMLUtil()->SelectNodes( "./PARAMETRAGE_TYPES_VEHICULES/PARAMETRAGE_TYPE_VEHICULE", pXMLFleetNode->getOwnerDocument(), (DOMElement*)pXMLFleetNode);
    XMLSize_t nbTypes = pXMLNodes->getSnapshotLength();
	for(XMLSize_t iType = 0; iType<nbTypes; iType++)
    {
        pXMLNodes->snapshotItem(iType);
        DOMNode * pNodeCapteurTroncon = pXMLNodes->getNodeValue();

        std::string strTmp;
        GetXmlAttributeValue(pNodeCapteurTroncon, "id", strTmp, &loadingLogger);
        TypeVehicule * pTV = m_pNetwork->GetVehicleTypeFromID(strTmp);

        VehicleTypeUsageParameters & usageParams = m_MapUsageParameters[pTV];

        DOMNode * pLoadNode = m_pNetwork->GetXMLUtil()->SelectSingleNode("./PARAMETRAGE_CHARGEMENT", pNodeCapteurTroncon->getOwnerDocument(), (DOMElement*)pNodeCapteurTroncon);
        if(pLoadNode)
        {
            usageParams.GetLoadUsageParameters().Load(pLoadNode, loadingLogger);
        }
    }
    pXMLNodes->release();

    return true;
}

AbstractFleet::AbstractFleet()
{
    m_pNetwork = NULL;
}

AbstractFleet::AbstractFleet(Reseau * pNetwork)
{
    m_pNetwork = pNetwork;
}

AbstractFleet::~AbstractFleet()
{
    std::map<Trip*, Schedule*>::iterator iter;
    for(iter = m_MapSchedules.begin(); iter != m_MapSchedules.end(); ++iter)
    {
        delete iter->second;
    }

    std::map<Trip*, RepartitionTypeVehicule*>::iterator iterRep;
    for(iterRep = m_MapRepartitionTypeVehicule.begin(); iterRep != m_MapRepartitionTypeVehicule.end(); ++iterRep)
    {
        delete iterRep->second;
    }   

    for(size_t i = 0; i < m_LstTrips.size(); i++)
    {
        delete m_LstTrips[i];
    }

    for(size_t i = 0; i < m_LstTripNodes.size(); i++)
    {
        delete m_LstTripNodes[i];
    }
}

 const std::vector<TripNode*> & AbstractFleet::GetTripNodes()
 {
     return m_LstTripNodes;
 }

 const std::vector<Trip*> & AbstractFleet::GetTrips()
 {
     return m_LstTrips;
 }

 Trip* AbstractFleet::GetTrip(const std::string & tripID)
 {
     Trip * pTrip = NULL;
     for(size_t iTrip = 0; iTrip < m_LstTrips.size() && !pTrip; iTrip++)
     {
         if(!m_LstTrips[iTrip]->GetID().compare(tripID))
         {
             pTrip = m_LstTrips[iTrip];
         }
     }
     return pTrip;
 }

TripNode* AbstractFleet::GetTripNode(const std::string & tripNodeID)
{
    TripNode * pTripNode = NULL;
    for(size_t iTripNode = 0; iTripNode < m_LstTripNodes.size() && !pTripNode; iTripNode++)
    {
        if(!m_LstTripNodes[iTripNode]->GetID().compare(tripNodeID))
        {
            pTripNode = m_LstTripNodes[iTripNode];
        }
    }
    return pTripNode;
}

 std::vector<boost::shared_ptr<Vehicule> > AbstractFleet::GetVehicles(Trip * pTrip)
 {
     std::vector<boost::shared_ptr<Vehicule> > result;
     for(size_t i = 0; i < m_LstVehicles.size(); i++)
     {
         if(!m_LstVehicles[i]->GetTrip()->GetID().compare(pTrip->GetID()))
         {
             result.push_back(m_LstVehicles[i]);
         }
     }
     return result;
 }

 Schedule * AbstractFleet::GetSchedule(Trip * pTrip)
 {
     return m_MapSchedules[pTrip];
 }

void AbstractFleet::InitSimuTrafic(std::deque< TimeVariation<TraceDocTrafic> > & docTrafics)
{
    for(size_t i = 0; i < m_LstTripNodes.size(); i++)
    {
        m_LstTripNodes[i]->Initialize(docTrafics);
    }
}

void AbstractFleet::ActivateVehicles(double dbInstant, double dbTimeStep)
{
    for(size_t i = 0; i < m_LstTrips.size(); i++)
    {
        std::vector<boost::shared_ptr<Vehicule> > lstActivatedVehicles = ActivateVehiclesForTrip(dbInstant, dbTimeStep, m_LstTrips[i]);
        for(size_t j = 0; j < lstActivatedVehicles.size(); j++)
        {
            OnVehicleActivated(lstActivatedVehicles[j], dbInstant);
        }
    }
}

void AbstractFleet::ActivateVehicle(double dbInstant, VehicleToCreate * pVehicleToCreate)
{
    boost::shared_ptr<Vehicule> pVeh = CreateVehicle(dbInstant, m_pNetwork->GetTimeStep(), pVehicleToCreate->GetTrip(), pVehicleToCreate->GetVehicleID(), NULL);
    OnVehicleActivated(pVeh, dbInstant);
}

bool AbstractFleet::IsCurrentLegDone(Vehicule * pVehicle, TripLeg * pCurrentLeg, double dbInstant, VoieMicro * pLane, double laneLength,
                                     double startPositionOnLane, double endPositionOnLane, bool bExactPosition)
{
    return pCurrentLeg->IsDone(dbInstant, pLane, laneLength, startPositionOnLane, endPositionOnLane, bExactPosition);
}

void AbstractFleet::OnCurrentLegFinished(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep)
{
    // Action li??es ?? l'entr??e du v??hicule sur le TripNode
    TripNode * pDestination = pVehicle->GetTrip()->GetCurrentLeg()->GetDestination();
    if (pDestination)
    {
        pDestination->VehiculeEnter(pVehicle, pDestinationEnterLane, dbInstDestinationReached, dbInstant, dbTimeStep);
    }
}

void AbstractFleet::SetLinkUsed(double dbInstant, Vehicule * pVeh, Tuyau * pLink)
{
    pVeh->GetTrip()->SetLinkUsed(pLink);
}

// Effectue les op??rations ?? faire ?? l'activation d'un v??hicule
void AbstractFleet::OnVehicleActivated(boost::shared_ptr<Vehicule> pVeh, double dbInstant)
{
    DoVehicleAssociation(pVeh);

    // Positionnement du v??hicule ?? la sortie du TripNode correspondant
    Trip * pTrip = pVeh->GetTrip();

    const Position & departurePos = pTrip->GetOrigin()->GetOutputPosition();
    pVeh->SetPos(departurePos.GetPosition());
    pVeh->SetTuyau(departurePos.GetDownstreamLink(), dbInstant);

    int numVoie = pVeh->GetTrip()->GetOrigin()->getNumVoie(pVeh->GetTrip(), pVeh->GetType());
    pVeh->SetVoie((VoieMicro*)pVeh->GetTrip()->GetOrigin()->GetOutputPosition().GetDownstreamLink()->GetLstLanes()[numVoie]);
    if(departurePos.IsOutside())
    {
        pVeh->SetOutside(true);
        pVeh->SetVoieDesiree(numVoie);
        pVeh->SetNumVoieInsertion(numVoie);
    }
    else
    {
        // l'insertion n'est plus ?? faire
        pVeh->SetInstInsertion(dbInstant);

        // rmq. : une am??lioration possible dans le cas des v??hicules cr????s aux entr??e serait de les int??grer ??ventuellement
        // ?? la liste des v??hicules en attente ?? l'entr??e le cas ??ch??ant ...
    }

    pVeh->SetHeureEntree(dbInstant);

    pVeh->InitializeCarFollowing();
    pVeh->SetVitRegTuyAct( pVeh->GetLink(0)->GetVitRegByTypeVeh(pVeh->GetType(), dbInstant, pVeh->GetPos(0), 0));

    // le premier arr??t est consid??r?? comme trait?? d??s le d??part du bus

    // Mise ?? jour de la liste des v??hicules du doc XML			
    std::string ssNomLigne = pTrip->GetID();
    std::string ssLibelle = GetVehicleIdentifier(pVeh);

	std::string ssEntree = pVeh->GetOrigine()->GetOutputID(); 
    std::string ssSortie = pVeh->GetTrip()->GetFinalDestination()->GetInputID(); 

	std::string ssVoieEntree;
    if( m_pNetwork->IsTraceRoute() )
    {
        ssVoieEntree = pVeh->GetLink(0)->GetLstLanes()[0]->GetLabel();
    }

	const std::string & ssType = pVeh->GetType()->GetLabel();
    const std::string & ssGMLType = pVeh->GetType()->GetGMLLabel();

    // TODO - JORGE - pour l'instant, je ne touche pas au champ actuel agressif des v??hicules qui sert ?? la proc??dure d'agressivit??. Faut-il fusionner les deux notions ?
	// Agressivit??
	pVeh->SetAgressif(false);		// A reprendre, les autres flottes que la SymuVIaFleet ne sont pas agressifs pour l'instant (car reli?? aux entr??es, ce qui n'est pas forc??ment le cas dans les autres usages)

    // TODO - JORGE - a priori pas de raison de ne pas le g??rer pour toute sles flottes ?
    pVeh->TirageJorge();

    std::vector<std::string> initialPath;
    std::vector<Tuyau*> * pFullPath = pTrip->GetFullPath();
    for(size_t iTuy = 0; iTuy < pFullPath->size(); iTuy++)
    {
        initialPath.push_back(pFullPath->at(iTuy)->GetLabel());
    }

    // remarque : pas de notion de motif pour les flottes non SymuVia pour le moment.
    m_pNetwork->AddVehiculeToDocTrafic( pVeh->GetID(), ssLibelle, ssType, ssGMLType, pVeh->GetDiagFonda()->GetKMax(), pVeh->GetDiagFonda()->GetVitesseLibre(), pVeh->GetDiagFonda()->GetW(), ssEntree, ssSortie, "", "", "", "", dbInstant, ssVoieEntree, pVeh->IsAgressif(), 0, ssNomLigne, initialPath, pVeh->GetType()->GetLstPlagesAcceleration(), "", "");

    m_pNetwork->AddVehicule(pVeh);
}

// Associe le v??hicule ?? la flotte et vice versa
void AbstractFleet::DoVehicleAssociation(boost::shared_ptr<Vehicule> pVeh)
{
    AssignFleetparameters(pVeh);
    pVeh->SetFleet(this);
    m_LstVehicles.push_back(pVeh);
}

// Cr??e et associ?? au v??hicule les param??tres li??s ?? la flotte
void AbstractFleet::AssignFleetparameters(boost::shared_ptr<Vehicule> pVeh)
{
    if (pVeh->GetFleetParameters() == NULL)
    {
        AbstractFleetParameters * pFleetParams = CreateFleetParameters();
        pFleetParams->GetUsageParameters() = m_MapUsageParameters[pVeh->GetType()];
        ScheduleElement * pScheduleElement = pVeh->GetScheduleElement();
        if (pScheduleElement)
        {
            std::map<ScheduleElement*, ScheduleParameters*>::const_iterator iter = m_MapScheduleParameters.find(pScheduleElement);
            if (iter != m_MapScheduleParameters.end())
            {
                pFleetParams->SetScheduleParameters(iter->second);
            }
        }
        pVeh->SetFleetParameters(pFleetParams);
    }
}


// Effectue les op??rations ?? faire ?? la destruction d'un v??hicule
void AbstractFleet::OnVehicleDestroyed(boost::shared_ptr<Vehicule> pVeh)
{
    // On enl??ve le v??hicule de la liste des v??hicules associ??s ?? la flotte
    m_LstVehicles.erase(std::remove(m_LstVehicles.begin(), m_LstVehicles.end(), pVeh), m_LstVehicles.end());

    // On l'enl??ve du Trip correspondant le cas ??ch??ant
    Trip * pTrip = GetTrip(pVeh->GetTrip()->GetID());
    if(pTrip)
    {
        pTrip->RemoveAssignedVehicle(pVeh);
    }
}

// Activation des v??hicules pour le Trip pass?? en param??tres (si besoin)
std::vector<boost::shared_ptr<Vehicule> > AbstractFleet::ActivateVehiclesForTrip(double dbInstant, double dbTimeStep, Trip * pTrip)
{
    // M??thode du cas g??n??ral simple : un trip associ?? ?? un schedule. a priori utilisable pour tout un tas de cas d'usage,
    // donc factoris?? dans AbstractFleet
    
    std::vector<boost::shared_ptr<Vehicule> > result;
    
    // R??cup??ration du schedule
    std::map<Trip*,Schedule*>::const_iterator iter = m_MapSchedules.find(pTrip);
    if(iter != m_MapSchedules.end())
    {
        // rmq. : hypoth??se : on ne g??re pas plusieurs cr??ations pour un pas de temps avec un Schedule.
        ScheduleElement * pScheduleElement = NULL;
        if(iter->second->CheckTime(dbInstant, dbTimeStep, &pScheduleElement))
        {
            boost::shared_ptr<Vehicule> pVeh = CreateVehicle(dbInstant, dbTimeStep, pTrip, -1, pScheduleElement);
            if(pVeh)
            {
                result.push_back(pVeh);
            }
        }
    }
    else
    {
        m_pNetwork->log() << Logger::Warning << "No schedule defined for trip '" << pTrip->GetID() << "' : no vehicle will be generated for it !" << std::endl;
        m_pNetwork->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir ?? reprendre tous les appels aux log en pr??cisant que c'est des INFO. ?? supprimer si on reprend tous les appels au log.
    }

    return result;
}

boost::shared_ptr<Vehicule> AbstractFleet::CreateVehicle(double dbInstant, double dbTimeStep, Trip * pTrip, int vehId, ScheduleElement * pScheduleElement)
{
    boost::shared_ptr<Vehicule> pVehicle;

    // Cr??ation du v??hicule
    std::map<Trip*, RepartitionTypeVehicule*>::iterator iterRepType = m_MapRepartitionTypeVehicule.find(pTrip);
    if(iterRepType != m_MapRepartitionTypeVehicule.end())
    {
        // rmq. : pas de gestion du num??ro de voie dans le cas de AbstractFleet : une seule r??partition des types de v??hicules ?? l'indice 0.
        TypeVehicule * pTypeVeh = iterRepType->second->CalculTypeNewVehicule(m_pNetwork, dbInstant, 0);

        // rmq. : on d??marre le v??hicule ?? vitesse nulle dans le cas de la gestion g??n??rique des flottes... ?? sp??cialiser si besoin
        pVehicle = boost::make_shared<Vehicule>(m_pNetwork, vehId == -1 ? m_pNetwork->IncLastIdVeh() : vehId, 0, pTypeVeh, dbTimeStep);
        pVehicle->SetScheduleElement(pScheduleElement);

        // Ajout du v??hicule ?? la liste des v??hicules g??r??s par le Trip
        pTrip->AddAssignedVehicle(pVehicle);

        Trip * pVehicleTrip = new Trip();
        pTrip->CopyTo(pVehicleTrip);
        ComputeTripPath(pVehicleTrip, pVehicle->GetType());
        pVehicle->SetTrip(pVehicleTrip);

        // pour pouvoir utiliser d??s ici les param??tres de chargement des v??hicules qui sont cr????s ?? un arr??t de bus par exemple,
        // et qui doivent embarquer les utilisateurs qui s'y trouvent, cf. Arret::VehicleExit par exemple
        AssignFleetparameters(pVehicle);

        pVehicle->MoveToNextLeg();
    }
    else
    {
        m_pNetwork->log() << Logger::Warning << "No vehicle types repartition definition for trip '" << pTrip->GetID() << "' : no vehicle will be generated for it !" << std::endl;
        m_pNetwork->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir ?? reprendre tous les appels aux log en pr??cisant que c'est des INFO. ?? supprimer si on reprend tous les appels au log.
    }
    
    return pVehicle;
}

double AbstractFleet::GetStopDuration(boost::shared_ptr<Vehicule> pVehicle, TripNode * pTripNode, bool bIsRealStop)
{
    return pTripNode->GetStopDuration(pVehicle, bIsRealStop);
}

std::map<std::string, std::string> AbstractFleet::GetOutputAttributes(Vehicule * pV, TripNode * pTripNode)
{
    return pTripNode->GetOutputAttributes(pV);
}

std::map<std::string, std::string> AbstractFleet::GetOutputAttributesAtEnd(Vehicule * pV)
{
    static const std::map<std::string, std::string> emptyAttributes;
    return emptyAttributes;
}

void AbstractFleet::ComputeTripPath(Trip * pTrip, TypeVehicule * pTypeVeh)
{
    TripNode * pOrigin = pTrip->GetOrigin();
    for(size_t iLeg = 0; iLeg < pTrip->GetLegs().size(); iLeg++)
    {
        TripLeg * pLeg = pTrip->GetLegs().at(iLeg);
        if(pLeg->GetPath().empty())
        {
            // Calcul du Path n??cessaire !
            std::map<std::vector<Tuyau*>, double> MapFilteredItis;
            std::vector<PathResult> paths;
            m_pNetwork->GetSymuScript()->ComputePaths(pOrigin, pLeg->GetDestination(), pTypeVeh, m_pNetwork->m_dbInstSimu, 1, paths, MapFilteredItis, false);
            if(paths.size() == 1)
            {
                pLeg->SetPath(paths.front().links);
            }
        }

        // Pour l'it??ration suivante
        pOrigin = pLeg->GetDestination();
    }
}

bool AbstractFleet::AddTripNodeToTrip(Trip * pTrip, TripNode * pTripNode, size_t positionIndex, Vehicule * pVehicle)
{
    if(positionIndex > pTrip->GetLegs().size())
    {
        // Si la position est trop grande, on la ram??ne au nombre de legs actuel (comme ??a le nouveau leg est ajout?? ?? la fin du Trip)
        positionIndex = pTrip->GetLegs().size();
    }

    bool bOk = true;
    if(pVehicle)
    {
        // Si la position du nouveau leg est la position du leg courant, il faut adapter les itin??raires en cours de parcours ...
        bool bCurrentLegChange = pTrip->GetCurrentLegIndex() == (int)positionIndex;

        std::map<std::vector<Tuyau*>, double> MapFilteredItis;
        std::vector<PathResult> paths;
        std::vector<Tuyau*> remainingPath;
        if(bCurrentLegChange)
        {
            // dans ce cas, on calcule l'itin??raire ?? partir de la position courante du v??hicule
            remainingPath = pVehicle->GetTrip()->GetCurrentLeg()->GetRemainingPath();
            m_pNetwork->GetSymuScript()->ComputePaths(remainingPath.front()->GetCnxAssAv(), pTripNode, pVehicle->GetType(), m_pNetwork->m_dbInstSimu, 1, remainingPath.front(), paths, MapFilteredItis, false);
        }
        else
        {
            TripNode * pOrigin = positionIndex == 0 ? pTrip->GetOrigin() : pTrip->GetLegs()[positionIndex-1]->GetDestination();
            m_pNetwork->GetSymuScript()->ComputePaths(pOrigin, pTripNode, pVehicle->GetType(), m_pNetwork->m_dbInstSimu, 1, paths, MapFilteredItis, false);
        }
        if(paths.size() == 1)
        {
            bool bNextPathChanged = false;
            std::vector<Tuyau*> path, nextPath;

            path = paths.front().links;

            // Recalcul de l'itin??raire du leg suivant ?? partir de la nouvelle origine s'il y en a un (de leg suivant)
            if(positionIndex < pTrip->GetLegs().size())
            {
                MapFilteredItis.clear();
                paths.clear();
                m_pNetwork->GetSymuScript()->ComputePaths(pTripNode, pTrip->GetLegs()[positionIndex]->GetDestination(), pVehicle->GetType(), m_pNetwork->m_dbInstSimu, 1, paths, MapFilteredItis, false);
                if(paths.size() == 1)
                {
                    bNextPathChanged = true;
                    nextPath = paths.front().links;
                }
                else
                {
                    bOk = false;
                }
            }

            if(bOk)
            {
                if(bCurrentLegChange)
                {
                    // Modif du leg en cours
                    TripNode * pNextDest = pTrip->GetCurrentLeg()->GetDestination();
                    pTrip->GetCurrentLeg()->SetDestination(pTripNode);
                    std::vector<Tuyau*> newIti;
                    newIti.push_back(remainingPath.front());
                    newIti.insert(newIti.end(), path.begin(), path.end());
                    pTrip->ChangeRemainingPath(newIti, pVehicle);

                    // Ajout du leg suivant
                    if(bNextPathChanged)
                    {
                        TripLeg * pTripLeg = new TripLeg();
                        pTripLeg->SetDestination(pNextDest);
                        pTripLeg->SetPath(nextPath);
                        pTrip->InsertTripLeg(pTripLeg, positionIndex+1);
                    }
                }
                else
                {
                    if(bNextPathChanged)
                    {
                        pTrip->GetLegs()[positionIndex]->SetPath(nextPath);
                    }
                    TripLeg * pTripLeg = new TripLeg();
                    pTripLeg->SetDestination(pTripNode);
                    pTripLeg->SetPath(path);
                    pTrip->InsertTripLeg(pTripLeg, positionIndex);
                }
            }
        }
        else
        {
            bOk = false;
        }
    }
    else
    {
        TripLeg * pTripLeg = new TripLeg();
        pTripLeg->SetDestination(pTripNode);
        pTrip->InsertTripLeg(pTripLeg, positionIndex);
        // rmq. : on ne recalcule pas d'itin??raire ici car les itin??raires sont calcul??s ?? l'activation des v??hicules
        // (tel que cod?? au moment de la r??alisation de cette m??thode en tout cas)
    }

    return bOk;
}

bool AbstractFleet::RemoveTripLegFromTrip(Trip * pTrip, size_t positionIndex, Vehicule * pVehicle)
{
    if(positionIndex >= pTrip->GetLegs().size())
        return false;

    bool bOk = true;
    if(pVehicle)
    {
        // Recalcul de l'itin??raire du leg suivant le leg supprim?? (son origine change), avec gestion du cas particulier o?? le v??hicule est sur le leg supprim?? !
        bool bCurrentLegRemoved = pTrip->GetCurrentLegIndex() == (int)positionIndex;
        bool bNextLegChanged = false;
        std::vector<Tuyau*> nextPath;
        std::vector<Tuyau*> remainingPath;
        if(positionIndex+1 < pTrip->GetLegs().size())
        {
            std::map<std::vector<Tuyau*>, double> MapFilteredItis;
            std::vector<PathResult> paths;
            
            TripNode * pNextTripNode = pTrip->GetLegs()[positionIndex+1]->GetDestination();
            if(bCurrentLegRemoved)
            {
                // dans ce cas, on calcule l'itin??raire ?? partir de la position courante du v??hicule
                remainingPath = pVehicle->GetTrip()->GetCurrentLeg()->GetRemainingPath();
                m_pNetwork->GetSymuScript()->ComputePaths(remainingPath.front()->GetCnxAssAv(), pNextTripNode, pVehicle->GetType(), m_pNetwork->m_dbInstSimu, 1, remainingPath.front(), paths, MapFilteredItis, false);
            }
            else
            {
                TripNode * pOrigin = positionIndex == 0 ? pTrip->GetOrigin() : pTrip->GetLegs()[positionIndex-1]->GetDestination();
                m_pNetwork->GetSymuScript()->ComputePaths(pOrigin, pNextTripNode, pVehicle->GetType(), m_pNetwork->m_dbInstSimu, 1, paths, MapFilteredItis, false);
            }
            if(paths.size() == 1)
            {
                bNextLegChanged = true;
                nextPath = paths.front().links;
            }
            else
            {
                bOk = false;
            }
        }

        if(bOk)
        {
            if(bNextLegChanged)
            {
                // On modifie le TripLeg courant pour aller vers la destination suivante avec l'itin??raire calcul??,
                // et on supprime le TripLeg suivant
                TripLeg * pLeg = pTrip->GetLegs()[positionIndex];
                pLeg->SetDestination(pTrip->GetLegs()[positionIndex+1]->GetDestination());
                if(bCurrentLegRemoved)
                {
                    // Modif du leg en cours
                    std::vector<Tuyau*> newIti;
                    newIti.push_back(remainingPath.front());
                    newIti.insert(newIti.end(), nextPath.begin(), nextPath.end());
                    pTrip->ChangeRemainingPath(newIti, pVehicle);
                }
                else
                {
                    pLeg->SetPath(nextPath);
                }
                pTrip->RemoveTripLeg(positionIndex+1);
            }
            else
            {
                // Dans ce cas il s'agit simplement de supprimer le dernier TripLeg du Trip
                // rmq. : pour l'instant impossible pour la flotte de livraison car on ne peut pas supprimer la destination finale d'un Trip
                // puisqu'on ne permet de supprimer que des points de livraison pour l'instant. Peut-??tre faudra-t-il faire plus de choses ici
                // si on y passe un jour (quid du v??hicule qui n'a plus rien ?? faire ? ou le supprime ?)
                pTrip->RemoveTripLeg(positionIndex);
            }
        }
    }
    else
    {
        pTrip->RemoveTripLeg(positionIndex);
        // rmq. : on ne recalcule pas d'itin??raire ici car les itin??raires sont calcul??s ?? l'activation des v??hicules
        // (tel que cod?? au moment de la r??alisation de cette m??thode en tout cas)
    }

    return bOk;
}

AbstractFleetSnapshot * AbstractFleet::TakeSnapshot()
{
    AbstractFleetSnapshot * pResult = new AbstractFleetSnapshot();
    for(size_t i = 0; i < m_LstVehicles.size(); i++)
    {
        pResult->lstVehicles.push_back(m_LstVehicles[i]->GetID());
    }
    std::map<Trip*, Schedule*>::const_iterator iter;
    for(iter = m_MapSchedules.begin(); iter != m_MapSchedules.end(); ++iter)
    {
        pResult->mapScheduleState[iter->first] = iter->second->GetLastStartElementIndex();
    }
    for(size_t i = 0; i < m_LstTrips.size(); i++)
    {
        Trip * pTrip = m_LstTrips[i];
        pResult->mapTripState[pTrip] = pTrip->TakeSnapshot();
    }
    for(size_t i = 0; i < m_LstTripNodes.size(); i++)
    {
        TripNode * pTripNode = m_LstTripNodes[i];
        pResult->mapTripNodeState[pTripNode] = pTripNode->TakeSnapshot();
    }
    return pResult;
}

void AbstractFleet::Restore(Reseau * pNetwork, AbstractFleetSnapshot * backup)
{
    m_LstVehicles.clear();
    for(size_t i = 0; i < backup->lstVehicles.size(); i++)
    {
        boost::shared_ptr<Vehicule> pVehToRestore = pNetwork->GetVehiculeFromID(backup->lstVehicles[i]);
        if (!pVehToRestore)
        {
            // il s'agit d'un v??hicule en attente ?? une entr??e :
            for (size_t iNode = 0; iNode < m_LstTripNodes.size(); iNode++)
            {
                SymuViaTripNode * pTripNode = dynamic_cast<SymuViaTripNode*>(m_LstTripNodes[iNode]);
                assert(pTripNode);
                for (std::map<Tuyau*, std::map<int, std::deque<boost::shared_ptr<Vehicule>>>>::const_iterator iter = pTripNode->m_mapVehEnAttente.begin(); iter != pTripNode->m_mapVehEnAttente.end() && !pVehToRestore; ++iter)
                {
                    for (std::map<int, std::deque<boost::shared_ptr<Vehicule>>>::const_iterator iterLane = iter->second.begin(); iterLane != iter->second.end() && !pVehToRestore; ++iterLane)
                    {
                        for (size_t iVeh = 0; iVeh < iterLane->second.size() && !pVehToRestore; iVeh++)
                        {
                            if (iterLane->second[iVeh]->GetID() == backup->lstVehicles[i])
                            {
                                pVehToRestore = iterLane->second[iVeh];
                            }
                        }
                    }
                }
            }
        }
        m_LstVehicles.push_back(pVehToRestore);
    }
    std::map<Trip*, int>::const_iterator iter;
    for(iter = backup->mapScheduleState.begin(); iter != backup->mapScheduleState.end(); ++iter)
    {
        m_MapSchedules[iter->first]->SetLastStartElementIndex(iter->second);
    }
    for(size_t i = 0; i < m_LstTrips.size(); i++)
    {
        Trip * pTrip = m_LstTrips[i];
        pTrip->Restore(pNetwork, backup->mapTripState[pTrip]);
    }
    for(size_t i = 0; i < m_LstTripNodes.size(); i++)
    {
        TripNode * pTripNode = m_LstTripNodes[i];
        pTripNode->Restore(pNetwork, backup->mapTripNodeState[pTripNode]);
    }
}

template void AbstractFleetSnapshot::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void AbstractFleetSnapshot::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void AbstractFleetSnapshot::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_NVP(lstVehicles);
    ar & BOOST_SERIALIZATION_NVP(mapScheduleState);
    ar & BOOST_SERIALIZATION_NVP(mapTripState);
    ar & BOOST_SERIALIZATION_NVP(mapTripNodeState);
}

template void AbstractFleet::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void AbstractFleet::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void AbstractFleet::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_NVP(m_pNetwork);
    ar & BOOST_SERIALIZATION_NVP(m_LstTripNodes);
    ar & BOOST_SERIALIZATION_NVP(m_LstTrips);
    ar & BOOST_SERIALIZATION_NVP(m_MapSchedules);
    ar & BOOST_SERIALIZATION_NVP(m_MapRepartitionTypeVehicule);
    ar & BOOST_SERIALIZATION_NVP(m_LstVehicles);
    ar & BOOST_SERIALIZATION_NVP(m_MapUsageParameters);
    ar & BOOST_SERIALIZATION_NVP(m_MapScheduleParameters);
}
