#pragma once
#ifndef ParkingH
#define ParkingH

#include "ConnectionPonctuel.h"
#include "usage/SymuViaTripNode.h"

class ZoneDeTerminaison;
class DocTrafic;

class ParkingSnapshot : public TripNodeSnapshot
{
public:
	int stock;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


class Parking : public SymuViaTripNode
{
public:
    // ************************************
    // Constructeurs/Destructeur
    // ************************************
    Parking();
    Parking(char _Nom[256], Reseau *pR, int nStockMax, int nStockInitial, double dbInterTempsEntree, double dbInterTempsSortie, std::vector<TypeVehicule*> lstTypesInterdits, bool bParcRelais, double dbParcRelaisMaxDistance);
    virtual ~Parking();

    // ************************************
    // Accesseurs
    // ************************************
    void IncStock() {m_nStock++;}
    void DecStock() { m_nStock = std::max<int>(0, m_nStock-1); }
    int GetStock() const {return m_nStock;}

    int GetStockMax() const {return m_nStockMax;}
    double GetInterTpsEntree() const {return m_dbInterTempsEntree;}
    double GetInterTpsSortie() const {return m_dbInterTempsSortie;}
    const std::vector<TypeVehicule*> GetLstTypesInterdits() const {return m_lstVehInterdits;}

    void                SetZoneParent(ZoneDeTerminaison * pZone) {assert(!m_pParentZone); m_pParentZone = pZone;} // Un parking ne peut pas �tre associ� � plusieurs zones !
    ZoneDeTerminaison * GetZoneParent() {return m_pParentZone;}

    bool                IsParcRelais() { return m_bIsParcRelais; }
    double              GetParcRelaisMaxDistance() { return m_dbParcRelaisMaxDistance; }


    ::Point getCoordonnees();

    // ***********************************************
    // Impl�mentation des m�thodes de la classe m�re 
    // ***********************************************

    // Indique si on peut cr�er des v�hicules � cette origine (par d�faut, on peut toujours)
    virtual bool        IsVehiculeDisponible() {return m_nStock > 0;}

    virtual void        VehiculeExit(boost::shared_ptr<Vehicule> pVehicle) {DecStock();}

    // Peut-on cr�er un v�hicule au vu de l'instant courant et du dernier instant d'insertion ?
    virtual bool        CanInsertVehicle(double dbInst, double dbLastInstantInsertion) {return dbInst >= dbLastInstantInsertion + GetInterTpsSortie();}

    // Sortie des infos de trafic du parking
    void				SortieTrafic(DocTrafic *pXMLDocTrafic);

    // Renvoie la valeur de la demande pour l'origine consid�r�e
    virtual double      GetDemandeValue(double dbInst, double & dbVariationEndTime);

    virtual bool        IsAllowedVehiculeType(TypeVehicule * pTypeVehicule);

    virtual double      GetNextEnterInstant(int nNbVoie, double  dbPrevInstSortie, double  dbInstant, double  dbPasDeTemps, const std::string & nextRouteID);

    virtual void        VehiculeEnter(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep, bool bForcedOutside = false);

    virtual bool        IsFull();

    // m�thode pour la sauvegarde et restitution de l'�tat des TripNodes (affectation dynamique convergente).
    // � sp�cialiser si des flottes utilisent des variables de simulation suppl�mentaires
    virtual TripNodeSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, TripNodeSnapshot * backup);

private:
    // *******************************************
    // Champs priv�s
    // *******************************************
    // Stock maximal de v�hicules dans le parking
    int m_nStockMax;
    // Stock initial
    int m_nStock;
    // Inter-temps minimal entre deux entr�es dans le parking (barri�re automatique)
    double m_dbInterTempsEntree;
    // Inter-temps minimal entre deux sorties dans le parking (barri�re automatique)
    double m_dbInterTempsSortie;
    // Liste des types de v�hicules interdits dans le parking
    std::vector<TypeVehicule*> m_lstVehInterdits;

    // Zone de terminaison � laquelle est associ�e le parking
    ZoneDeTerminaison * m_pParentZone;

    // Vrai si ce parking est d�fini comme �tant un parc relais
    bool m_bIsParcRelais;

    // Distance de recherche de arr�ts associ�s autour du parc relai
    double m_dbParcRelaisMaxDistance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
}; 

#endif // ParkingH

