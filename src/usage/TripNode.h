#pragma once
#ifndef TripNodeH
#define TripNodeH

#include "Position.h"
#include "tools.h"

class Reseau;
class TypeVehicule;
class Vehicule;
class VoieMicro;
class TraceDocTrafic;
class Trip;

#include <string>
#include <vector>

class TripNodeSnapshot {

public:
    TripNodeSnapshot();
    virtual ~TripNodeSnapshot();

    std::vector<int>                            lstStoppedVehicles; // Liste des v�hicules dans le TripNode

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

class TripNode
{
public:
    TripNode();
    TripNode(const std::string & strID, Reseau * pNetwork);
    virtual ~TripNode();

    // positionnement du TripNode (point d'entr�e et de sortie)
    virtual void SetInputPosition(const Position & pos);
    virtual void SetOutputPosition(const Position & pos);

    virtual const Position & GetInputPosition() const;
    virtual const Position & GetOutputPosition() const;

    void SetStopNeeded(bool bStopNeeded);
    void SetListVehiclesInTripNode(bool bListVehicles);

	bool IsStopNeeded() { return m_bStopNeeded; }

    Reseau * GetNetwork() { return m_pNetwork; }

    // Identifiant de l'entr�e dans le TripNode
    virtual const std::string & GetInputID() const {return GetID();}

    // Identifiant de la sortie du TripNode
    virtual std::string GetOutputID() const {return GetID();}

    // Identifiant du TripNode
    virtual const std::string & GetID() const {return m_strID;}

    // Initialisation du TripNode pour la simulation de trafic
    virtual void Initialize(std::deque<TimeVariation<TraceDocTrafic> > & docTrafics) {}

    // Indique si l'atteinte du TripNode implique l'arr�t du v�hicule
    virtual bool ContinueDirectlyAfterDestination();

    // Indique si le type de v�hicule est autoris� pour cette destination
    virtual bool IsAllowedVehiculeType(TypeVehicule * pTypeVehicule) {return true;}

    // Indique si le TripNode est plein (cas d'un parking par exemple)
    virtual bool IsFull() {return false;}

    // Par d�faut, pas de restriction ou de temps interv�hiculaire � l'entr�e dans un tripnode (� sp�cialiser pour les sorties ou parkings par exemple)
    virtual double GetNextEnterInstant(int nNbVoie, double  dbPrevInstSortie, double  dbInstant, double  dbPasDeTemps, const std::string & nextRouteID) { return dbInstant; }

    // Traitement sur entr�e d'un v�hicule dans le TripNode
    virtual void VehiculeEnter(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep, bool bForcedOutside = false);

    // Traitement sur sortie d'un v�hicule du TripNode
    virtual void VehiculeExit(boost::shared_ptr<Vehicule> pVehicle);

    // Test de l'atteinte de la destination
    virtual bool IsReached(VoieMicro * pLane, double laneLength, double startPositionOnLane, double endPositionOnLane, bool bExactPosition);

    // Teste si le v�hicule est en cours d'arr�t dans le TripNode
    virtual bool HasVehicle(boost::shared_ptr<Vehicule> pVeh);

    // Gestion de l'arr�t d'un v�hicule dans le TripNode
    virtual bool ManageStop(boost::shared_ptr<Vehicule> pVeh, double dbInstant, double dbTimeStep);

    // Indique si on a besoin de lister les v�hicules arr�t�s dans ce TripNode
    virtual bool ListVehiclesInTripNode() {return m_bListVehiclesInTripNode;}

    // Indique si le TripNode correspond � un num�ro de voie en particulier ou non
    virtual bool hasNumVoie(Trip * pTrip);

    // Indique le num�ro de voie du TripNode
    virtual int getNumVoie(Trip * pTrip, TypeVehicule * pTypeVeh);

    // Calcule le temps d'arr�t du v�hicule dans le TripNode
    virtual double GetStopDuration(boost::shared_ptr<Vehicule> pVehicle, bool bIsRealStop);

    // Renvoie une map d'attributs � sortir de fa�on sp�cifique pour un v�hicule dans le TripNode
    virtual std::map<std::string, std::string> GetOutputAttributes(Vehicule * pV) {return std::map<std::string, std::string>();}

    // m�thode pour la sauvegarde et restitution de l'�tat des TripNodes (affectation dynamique convergente).
    // � sp�cialiser si des flottes utilisent des variables de simulation suppl�mentaires
    virtual TripNodeSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, TripNodeSnapshot * backup);

protected:

    std::string     m_strID;
    Reseau*         m_pNetwork;

    Position        m_InputPosition;
    Position        m_OutputPosition;

    bool            m_bStopNeeded;
    bool            m_bListVehiclesInTripNode;

    // Liste des v�hicules arr�t�s au TripNode
    std::vector<boost::shared_ptr<Vehicule> > m_LstStoppedVehicles;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // TripNodeH