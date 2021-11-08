#include "stdafx.h"
#include "feux.h"

#include "repartiteur.h"

#include <boost/serialization/deque.hpp>
#include <boost/serialization/shared_ptr.hpp>

//---------------------------------------------------------------------------
// Constructeurs, destructeurs et assimil�s
//--------------------------------------------------------------------------

// Destructeur
Feux::~Feux()
{
    //DeleteCycles();
    EraseListOfVariation(m_pLstCycle);
    delete m_pLstCycle;
}

// Constructeur par d�faut
Feux::Feux(Repartiteur *pRep)
{
    m_pRepartiteur = pRep;

    m_pLstCycle = new std::deque<TimeVariation<CycleFeu>>;
}

//================================================================
/*    void Feux::DeleteCycles
//----------------------------------------------------------------
// Fonction  : Suppression des cycles de feu
// Version du: 09/11/2006
// Historique: 09/11/2006 (C.B�carie - Tinea)
//================================================================
(
)
{
	std::deque <CycleFeu*>::iterator par;
    std::deque <CycleFeu*>::iterator debut = m_LstCycles.begin();
    std::deque <CycleFeu*>::iterator fin = m_LstCycles.end();

    for (par=debut;par!=fin;par++)
    {
        delete *(par);
    }
    m_LstCycles.erase(debut,fin);
}*/

/*//================================================================
    CycleFeu* Feux::GetCycle
//----------------------------------------------------------------
// Fonction  : Retourne le cycle de l'instant pass� en param�tre
// Version du: 05/09/2009
// Historique: 05/09/2009 (C.B�carie - Tinea)
//================================================================
(
    double   dbInstant,
    double  &dbTpsPrevCycle      // Temps pass� dans les cycles pr�c�dents
)
{
    std::deque <CycleFeu*>::iterator cur, debut, fin;
    double              dbInt;
    double              dbOldInt;
    CycleFeu*    pCF;

    // Recherche de la r�partition active
    debut   = m_LstCycles.begin();
    fin     = m_LstCycles.end();

    pCF = NULL;

    dbInt = 0;
    dbOldInt = 0;
    dbTpsPrevCycle = 0;
    for (cur=debut;cur!=fin;cur++)
    {
        pCF = *cur;
        dbInt += pCF->dbDuree;

        if( dbInt >= dbInstant )
        {
            dbTpsPrevCycle = dbOldInt;
            return *cur;
        }
        dbOldInt = dbInt;
    }

    // L'instant simul� est sup�rieur � la somme des dur�es des feux d�finies, on renvoie alors
    // le dernier cycle et un message de log est g�n�r�
    *Reseau::m_pFicSimulation << "Cycle du r�partiteur "<< m_pRepartiteur->getNom() <<" non d�fini pour l'instant simul�. " << std::endl;

    dbTpsPrevCycle = dbOldInt;    
    return pCF;
}*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void CycleFeu::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void CycleFeu::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void CycleFeu::serialize(Archive & ar, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(dbCycle);
    ar & BOOST_SERIALIZATION_NVP(dbOrange);
}

template void Feux::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void Feux::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void Feux::serialize(Archive & ar, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(m_pRepartiteur);
    ar & BOOST_SERIALIZATION_NVP(m_pLstCycle);
}