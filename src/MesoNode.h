#ifndef _MESONODE_H__
#define _MESONODE_H__

#include "tools.h"
#include <map>
#include <set>

class Vehicule;
class CTuyauMeso;
class Tuyau;

class CMesoNode;

//! Classe d�finissant une connexion de type Meso
class CMesoNode
{

public:
    //! Default constructor
    CMesoNode(void);
    //! Destructor
    virtual ~CMesoNode(void);
      
    //! Ajoute un couple vehicule, instant arrivant sur un troncon de la connexion.
    // Si le pTuyau est null le vehicule arrive d'une entr�e du r�seau
    void AddArrival( Tuyau * pTuyau, double dArrivalTime,Vehicule *pVehicule, bool bEnsureIsLastArrival);

    //! Ajoute un couple vehicule/instant arrivant sur un troncon de la connexion depuis un r�seau non m�so.
    // Si le pTuyau est null le vehicule arrive d'une entr�e du r�seau
    bool AddEnteringVehiculeFromOutside(Tuyau * pTuyauFrom, CTuyauMeso * pTuyauNext, double dArrivalTime,Vehicule *pVehicule);

    //! Retourne les couples vehicule, instant arrivant sur un troncon de la connexion
    // Si le pTuyau est null les vehicule arrivent d'une entr�e du r�seau
    std::list< std::pair<double, Vehicule*> > GetArrivalTimes(Tuyau *pTuyau) const;

 
    //! D�fini le prochain couple instant � "passer" le connexion vers le tuyau pTuyau
    // Si le pTuyau est null le vehicule sort vers une sortie
    void SetNextSupplyTime(Tuyau * pTuyau, double dSupplyTime);

    //! Obtient le prochain couple tuyau instant � "passer" le connexion vers le tuyau pTuyau
    // Si le pTuyau est null le vehicule sort vers une sortie
    double GetNextSupplyTime( Tuyau * pTuyau)  const;

    //! Obtient la prochaine arriv�e d'un troncon
    std::pair<double,Vehicule* > GetMinArrival( Tuyau * pTuyau ) const;
   
    //! Defini le prochain evenement de la connexion
    void SetGlobalNextEvent( double dTime, Tuyau * pEventEntryTuyau);

    //! D�fini le rand du prochain v�hicule arrivant
    void SetRankNextIncomingVeh(Tuyau * pEventTuyau, int iRank);

    //! Retourne l'instant du prochain �v�nement
    double GetNextEventTime() const {return m_dNextEventTime; }

    //! Vide les arriv�s sur le troncon pTuyau de la connexion
    void ClearArrivals(Tuyau * pTuyau);

    //! Remplace la liste des arriv�es de vehicules sur un troncon de la connexion    
    void ReplaceArrivalTimes(Tuyau * pTuyau, std::list< std::pair<double, Vehicule*> >);
    
    //! Ajoute un temps de passage sur un troncon amont
    void AddUpstreamPassingTime(Tuyau * pTuyau,  double dInstant  );

    //! Ajoute un temps de passage sur un troncon aval
    void AddDownstreamPassingTime(Tuyau * pTuyau,  double dInstant  );

    //! Incr�mente le nombre de vehicule allant entrer sur le troncon de la connexion
    void IncreaseRankNextIncommingVeh(Tuyau * pTuyau);

    //! Incr�mente le nombre de v�hicule allant sortir du troncon de la connexion
    void IncreaseRankNextOutgoingVeh(Tuyau * pTuyau);

    //! D�cr�mente le nombre de v�hicule allant sortir du troncon de la connexion
    void DecreaseRankNextOutgoingVeh(Tuyau * pTuyau);
   
    void DecreaseRankNextIncommingVeh(Tuyau *pTuyau);
     
    void RemoveFirstDownStreamPassingTime(Tuyau * pTuyau);

    void RemoveFirstUpstreamPassingTime(Tuyau * pTuyau );

    //! Obtient le nombre de v�hicule allant sortir du troncon de la connexion
    int GetRankNextOutgoingVehicule(Tuyau * pTuyau) const;

    //! Obtient les temps de passages des vehicules entrant sur le pTuyau de la connexion
    std::deque<double> GetUpstreamPassingTimes(Tuyau * pUpstream) const;

    //! Retourne le troncon d'entr�e du prochain �v�nement
    Tuyau * GetNextEventIncommingLink() const{ return m_pNextEventTuyauEntry;}

    //! Recup�re le tuyau et l'instant du prochain arrival
    std::pair<Tuyau*, std::pair<double, Vehicule*> > GetNextIncomingLink(Tuyau * pLink) const;

    //! Retourne vrai si la connexion est une origine, faux sinon
    virtual bool IsAnOrigine() const = 0;
    //! Retourne vrai si la connexion est une destination, faux sinon
    virtual bool IsADestination() const = 0;

    //! Obtient le prochain couple Vehicule, instant arrivant sur un troncon de la connexion
    // Si le pTuyau est null le vehicule arrive d'une entr�e du r�seau
    std::pair<double,Vehicule* >  GetNextArrival(Tuyau *pTuyau) const;

    //! Defini le prohain couple Vehicule, instant arrivant sur un troncon de la connexion
    // Si le pTuyau est null le vehicule arrive d'une entr�e du r�seau
    void SetNextArrival(Tuyau * pTuyau, std::pair<double, Vehicule*> arrival );
    
    double GetMaxArrivalTime(Tuyau* pTuyau) const;

    //! Retourne vrai si le noeud a au moin un troncon mesoscopique
    virtual bool HasMesoTroncon( ) const = 0;

    //! Passage en mode micro du tuyau connect� sp�cifi�
    void ToMicro(Tuyau * pLink, Reseau * pNetwork);

    //! Passage en mode meso du tuyau connect� sp�cifi�
    void ToMeso(Tuyau * pLink);

    // Accesseurs pour la sauvegarde/restitution en mode affectation dynamique convergente
    std::map<Tuyau*,std::list< std::pair<double, Vehicule* > > > & getArrivals() {return m_arrivals;}
    std::map<Tuyau*, std::deque<double > > & getUpstreamPassingTimes() {return m_upstreamPassingTimes;}
    std::map<Tuyau*, std::deque<double > > & getDownstreamPassingTimes() {return m_downstreamPassingTimes;}
    std::map<Tuyau*, std::pair<double, Vehicule* > > & getNextArrivals() {return m_nextArrival;}
    std::map<Tuyau*, int> & getMapRanksIncomming() {return m_mapRanksIncomming;}
    std::map<Tuyau*, int> & getMapRanksOutGoing() {return m_mapRanksOutGoing;}
    std::map<Tuyau*, double> & getNextSupplyTime() {return m_dNextSupplyTime;}


private:

    //! Liste des couples vehicules; instants d'arriv�s depuis une entr�e
    std::map<Tuyau*,std::list< std::pair<double, Vehicule* > > > m_arrivals;

    //! Liste des temps de passage amont ordonn�es dans l'ordre de passage d'apr�s le tuyau d'origine
    std::map<Tuyau*, std::deque<double > > m_upstreamPassingTimes;

    //! Liste des temps de passage aval ordonn�es dans l'ordre de passage
    std::map<Tuyau*, std::deque<double > > m_downstreamPassingTimes;

    //! Couples vehicules; instants de la prochaines arriv�e depuis une entr�e
    std::map<Tuyau*, std::pair<double, Vehicule* > > m_nextArrival;

    //! Nombre de v�hicules entrant sur les troncons de la connexion 
    // (si tuyau = null il s'agit d'une entr�e )
    std::map< Tuyau*, int> m_mapRanksIncomming;
    //! Nombre de v�hicules sortant sur les troncons de la connexion 
    // (si tuyau = null il s'agit d'une sortie )
    std::map< Tuyau*, int> m_mapRanksOutGoing;

    //! Tuyau d'entr�e du prochain �v�nement
    Tuyau * m_pNextEventTuyauEntry;

    double m_dNextEventTime;

    std::map<Tuyau*,  double> m_dNextSupplyTime;
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif