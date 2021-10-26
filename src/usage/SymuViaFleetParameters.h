#pragma once
#ifndef SymuViaFleetParametersH
#define SymuViaFleetParametersH

#include "AbstractFleetParameters.h"

#include <vector>
#include <string>
#include <set>

class Parking;
class Tuyau;
class CMotifCoeff;
class CPlaque;
class Connexion;
class ZoneDeTerminaison;

class SymuViaFleetParameters : public AbstractFleetParameters
{

public:
    SymuViaFleetParameters();
    virtual ~SymuViaFleetParameters();

    // Itin�raire pr�d�fini
    void                        SetRouteId(const std::string & routeId);
    const std::string &         GetRouteId();
    
    void                        SetIsTerminaison(bool bTerminaison);
    bool                        IsTerminaison();
    void                        SetIsTerminaisonResidentielle(bool bTerminaisonResidentielle);
    bool                        IsTerminaisonResidentielle();
    void                        SetIsTerminaisonSurfacique(bool bTerminaisonSurfacique);
    bool                        IsTerminaisonSurfacique();
    void                        SetIsRechercheStationnement(bool bIsRechercheStationnement, double dbInstant);
    bool                        IsRechercheStationnement();
    void                        SetDoReaffecteVehicule(bool bValue);
    bool                        DoReaffecteVehicule();

    void                        UpdateGoalDistance(double dbGoalDistance, double dbInstant);
    double                      GetGoalDistance();
    double                      GetDistanceRechercheParcourue();
    bool                        IsGoalDistanceUpdateNeeded(double dbInstant, ZoneDeTerminaison * pZone);
    std::vector<Tuyau*> &       GetAccessibleLinksForParking();

    bool                        IsMaxSurfaciqueDureeReached(double dbInstant, ZoneDeTerminaison * pZone);

    void                        SetForcePerimetreEtendu(bool bValue);

    bool                        IsPerimetreElargi(double dbInstant, ZoneDeTerminaison * pZone);
    bool                        IsRearbitrageNeeded(double dbInstant, ZoneDeTerminaison * pZone);

    void                        InitializeZoneTravel(double dbInstant, const std::string & zoneId, double dbDistance, Connexion * pInputJunction);
    double                      GetInstantEntreeZone();
    void                        IncDistanceParcourue(double dbDistance);
    void                        SetPlaqueOrigin(CPlaque * pPlaqueOrigin);
    void                        SetLastLinkInOriginZone(Tuyau * pLink);
    void                        SetPlaqueDestination(CPlaque * pPlaqueDestination);
    std::set<Tuyau*>            &GetLstTuyauxDejaParcourus();
    CMotifCoeff*                GetMotif();
    void                        SetMotif(CMotifCoeff * pMotif);
    const std::string &         GetZoneId();
    double                      GetDistanceParcourue();
    Connexion*                  GetZoneDestinationInputJunction();
    void                        SetZoneDestinationInputJunction(Connexion* pJunction);
    CPlaque*                    GetPlaqueOrigin();
    Tuyau*                      GetLastLinkInOriginZone();
    CPlaque*                    GetPlaqueDestination();


    virtual AbstractFleetParameters * Clone();
    
protected:

    // Identifiant de l'itin�raire pr�d�fini �ventuel
    std::string                 m_RouteId;

    // Gestion de la terminaison en zone
    bool                        m_bIsTerminaison;
    bool                        m_bIsTerminaisonResidentielle;
    bool                        m_bIsTerminaisonSurfacique;
    bool                        m_bIsRechercheStationnement;
    double                      m_dbInstantDebutRechercheStationnement;
    double                      m_dbInstantLastArbitrage;
    double                      m_dbInstantLastMAJGoalDistance;
    bool                        m_bForcePerimetreEtendu;
    std::set<Tuyau*>            m_LstTuyauxDejaParcourus;    // Liste des tron�ons de la zone d�j� parcourus � la recherche de stationnement
    bool                        m_bDoReaffecteVehicule;     // Indique si on doit r�arbitrer le stationnement final

    CMotifCoeff*                m_pMotif;                   // Motif de d�placement �ventuel du v�hicule

    double                      m_dbInstantEntreeZone;      // Instant de l'entr�e en zone
    std::string                 m_zoneId;                   // Identifiant de la zone cible
    double                      m_dbDistanceParcourue;      // Pour sortie de la distance parcourue depuis la rentr�e en zone
    double                      m_dbDistanceRechercheParcourue; // Distance parcourue en recherche de stationnement (pour le mode de stationnement bas� sur la distance � parcourir)
    double                      m_dbGoalDistance;           // Distance � parcourir avant de trouver une place de stationnement (mode de stationnement surfacique simplifi�)
    Connexion*                  m_pInputJunction;           // Point de jonction d'entr�e dans la zone de destination

    CPlaque*                    m_pPlaqueOrigin;            // Plaque d'origine affect�e au v�hicule.
    Tuyau*                      m_pLastLinkInOriginZone;    // Dernier tuyau de l'itin�raire initial du v�hicule dans sa zone d'origine (pour calcul des temps de d�part en zone pour SymuPlayer)
    CPlaque*                    m_pPlaqueDestination;       // Plaque de destination affect�e au v�hicule (cas o� SymuMaster l'impose, SymuVia n'a plus � effectuer de tirage)

    std::vector<Tuyau*>         m_LstAccessibleLinksForParking; // Liste des liens accessibles pour le parking surfacique ne mode simplifi�

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // SymuViaFleetParametersH
