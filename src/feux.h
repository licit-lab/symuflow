#pragma once
#ifndef feuxH
#define feuxH

#include "tools.h"

//---------------------------------------------------------------------------
//***************************************************************************//
//                          structure CycleFeux                              //
//***************************************************************************//
struct CycleFeu
{
    //double      dbDuree;                         // Dur�e d'application du cycle

    double      dbCycle;                        // Dur�e du cycle
    double      dbOrange;                       // Dur�e de l'orange

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

// La classe feux contient toutes les informations correspondant au fonctionnement
// de circulation durant la simulation. Pour info, un feu est d�fini pour une voie
// d'un tron�on en amont d'un r�partiteur

class Repartiteur;

class Feux
{
private:
    //std::deque<CycleFeu*>        m_LstCycles;                   
    Repartiteur*                m_pRepartiteur;                 // R�partiteur parent

    std::deque<TimeVariation<CycleFeu>>           *m_pLstCycle; // Liste des cycles

public:

    // Constructeurs, destructeurs et assimil�s
    ~Feux(void) ; // destructeur
	Feux(Repartiteur *pRep) ; // constructeur par d�faut      

    Repartiteur*    GetRepartiteur(){return m_pRepartiteur;};

    std::deque<TimeVariation<CycleFeu>>* GetLstCycle(){return m_pLstCycle;};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);

};

#endif
