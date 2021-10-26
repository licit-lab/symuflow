#pragma once
#ifndef sortieH
#define sortieH

#include "ConnectionPonctuel.h"
#include "usage/SymuViaTripNode.h"


// Sortie est un classe fixe de connection. Il s'agit d'un �l�ment de r�seau qui
// se trouve en aval d'un tron�on et qui est d�finit par sa capacit� � absorber
// le flux quittant le r�seau simul�

class Sortie : public SymuViaTripNode, public ConnectionPonctuel
{
private:        

    // D�finition des caract�ristiques de la sortie	
    ListOfTimeVariation<tracked_double>         *m_pLTVCapacite;

public:

	// Constructeurs, destructeurs et assimil�s
	virtual ~Sortie(void); // Destructeur
	Sortie(void); // constructeur par d�faut
    Sortie(char*, Reseau *pReseau);

    ListOfTimeVariation<tracked_double>*    GetLstCapacites() { return m_pLTVCapacite; }

    std::map<std::string, double> & GetCapacitiesPerRoutes() { return m_mapCapacityPerRoute; }

    // Surchage de GetNextEnterInstant pour application de la restriction de capacit�
    virtual double  GetNextEnterInstant(int nNbVoie, double  dbPrevInstSortie, double  dbInstant, double  dbPasDeTemps, const std::string & nextRouteID);

    // Arriv�e du v�hicule dans la sortie
    virtual void        VehiculeEnter(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep, bool bForcedOutside = false);

protected:
    // Utilis� pour les restrictions de capacit� par route impos�es par SymuMaster pour l'hybridation avec SimuRes
    std::map<std::string, double> m_mapCapacityPerRoute;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif
