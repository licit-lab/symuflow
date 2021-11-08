#pragma once
#ifndef TripH
#define TripH

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

class TripNode;
class TripLeg;
class Tuyau;
class Vehicule;
class Reseau;

namespace boost {
    namespace serialization {
        class access;
    }
}

class TripSnapshot {

public:
    TripSnapshot();
    virtual ~TripSnapshot();

    std::vector<int>                            assignedVehicles; // Liste des v�hicules affect�s au trip

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // S�rialisation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

class Trip
{
public:

    // Constructeurs / Destructeurs
    Trip();
    Trip(const Trip & other);
    virtual ~Trip();

    // Accesseurs
    void SetID(const std::string & strID);
    const std::string & GetID();
    void SetOrigin(TripNode * pOrigin);
    TripNode * GetOrigin();
    const std::vector<TripLeg*> & GetLegs();
    TripNode * GetFinalDestination();
    TripNode * GetCurrentDestination();
    TripNode * GetInitialDestination();
    std::vector<Tuyau*> * GetFullPath();
    std::vector<Tuyau*> GetFullPathThreadSafe();

	// Renvoie l'itin�raire restant dans le Trip
	std::vector<Tuyau*> GetRemainingPath();

    
    TripLeg * GetCurrentLeg();
    int GetCurrentLegIndex();
    void IncCurrentLegIndex();

    const std::vector<boost::shared_ptr<Vehicule> > & GetAssignedVehicles();
    void AddAssignedVehicle(boost::shared_ptr<Vehicule> pVeh);
    void RemoveAssignedVehicle(boost::shared_ptr<Vehicule> pVeh);
    void ClearAssignedVehicles();

    std::vector<TripNode*> GetTripNodes();
    TripNode * GetTripNode(const std::string & nodeName) const;

    // Traitements
    ///////////////////////////////////

    // Recopie le Trip vers le Trip pass� en param�tres
    void CopyTo(Trip * pTrip);

    // Met � jour le Trip en fonction des tuyaux parcourus
    void SetLinkUsed(Tuyau * pLink);

    // Ajout d'une �tape suppl�mentaire
    void AddTripLeg(TripLeg* pTripLeg);

    // Insertion d'une �tape suppl�mentaire � une position donn�e.
    // rmq. : ne met pas � jour les itin�raires correspondants.
    void InsertTripLeg(TripLeg * pTripLeg, size_t position);

    // Suppression d'une �tape
    void RemoveTripLeg(size_t position);

    // Modification de l'itin�raire restant dans l'�tape actuelle
    bool ChangeRemainingPath(const std::vector<Tuyau*> & newPath, Vehicule * pVeh);

    // Modification de l'itin�raire complet de l'�tape Actuelle
    void ChangeCurrentPath(const std::vector<Tuyau*> & newPath, int currentLinkInPath);

    // Suppression des tron�ons non encore parcourus du leg courant de l'itin�raire du leg
    // (appel� lorsqu'on passe au leg suivant)
    void EndCurrentPath();

    // Indique si le trajet implique de s'arr�ter � la prochaine destination d'�tape ou non
    bool ContinueDirectlyAfterDestination();

    // Indique si un itin�raire est d�fini pour le Trip
    bool IsPathDefined();

    // Renvoie le nombre de passage d�j� effectu�s � un TripNode jusqu'au TripLeg courant (ne peut �tre > 0 que dans les cas o� un Trip passe plusieurs fois par un m�me TripNode)
    int GetPassageNbToCurrentLeg(TripNode * pTripNode);

    // Renvoie le nombre de passage d�j� effectu�s � un TripNode jusqu'au TripLeg d'indice tripLegIndex exclu (ne peut �tre > 0 que dans les cas o� un Trip passe plusieurs fois par un m�me TripNode)
    int GetPassageNb(TripNode * pTripNode, size_t tripLegIndex);

    // m�thode pour la sauvegarde et restitution de l'�tat des Trips (affectation dynamique convergente).
    virtual TripSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, TripSnapshot * backup);

    // Invalidation du chemin complet sur tous les legs calcul� pr�c�demment (il sera recalcul� quand n�cessaire)
    void InvalidateFullPath();

public:
    // Liste des v�hicules assign�s au trip (uniquement pour les instances de Trip associ�es aux flottes, pas aux v�hicules)
    // rmq. mis en public pour conserver la compatibilit� avec les scripts python existants
    std::vector<boost::shared_ptr<Vehicule> > m_AssignedVehicles;

private:

    std::vector<Tuyau*> * BuildFullPath();

private:

    // Identifiant du trip
    std::string             m_strID;

    TripNode               *m_pOrigin;
    std::vector<TripLeg*>   m_Path;

    // Position actuelle du v�hicule dans le Trip
    int                     m_CurrentLegIndex;

    // Liste compl�te des tuyaux pour r�aliser le Trip (�quivalent de l'ancien m_Itineraire de la classe Vehicule)
    std::vector<Tuyau*>*    m_pFullPath;

    boost::mutex            m_Mutex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // TripH
