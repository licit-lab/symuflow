#include "stdafx.h"
#include "Segment.h"

#include "frontiere.h"
#include "reseau.h"
#include "voie.h"
#include "tuyau.h"
#include "SystemUtil.h"
#include "TraceDocTrafic.h"
#include "vehicule.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

const double SegmentMacro::EPSILON_01 = 1.0E-3;

//================================================================
    Segment::Segment
//----------------------------------------------------------------
// Fonction  : Constructeur
// Version du: 23/10/2006
// Historique: 23/10/2006 (C.B�carie - Tinea)
//             Reprise
//================================================================
(
)
{
}

//================================================================
    Segment::Segment
//----------------------------------------------------------------
// Fonction  : Constructeur
// Version du: 13/11/2006
// Historique: 13/11/2006 (C.B�carie - Tinea)
//             Ajout de la gestion d'un ID des segments
//================================================================
(
    int     nID,
    double  dbVitReg
    ):Troncon(dbVitReg,DBL_MAX,"")
{
    m_nID = nID;
}

//---------------------------------------------------------------------------
// COnstructeurs, destructeurs et assimil�s
//---------------------------------------------------------------------------

// Destructeur
SegmentMacro::~SegmentMacro()
{
        cell_amont=NULL;
        cell_aval=NULL;

         m_pFrontiereAmont = NULL;
         m_pFrontiereAval = NULL;
}

// Constructeur par d�faut
SegmentMacro::SegmentMacro()
{
        cell_amont=NULL;
        cell_aval=NULL;

        m_pFrontiereAmont   = NULL;
        m_pFrontiereAval    = NULL;

};

// Constructeur normal
SegmentMacro::SegmentMacro
(
	std::string	sLabel,
    double  _pasespace,
    double  _pastemps,
    int     _nbVoies,    
    Frontiere *pFAmont,
    Frontiere *pFAval,
    int     &nLastID,
    double  dbVitReg
):Segment(nLastID, dbVitReg)
{
 //constructeur appel� dans la segmentation
        m_strLabel = sLabel;

        m_dbPasEspace=_pasespace;
        m_dbPasTemps=_pastemps;
        Nb_Voies=_nbVoies;

        m_pFrontiereAmont   = NULL;
        m_pFrontiereAval    = NULL;

        cell_amont=NULL;
        cell_aval=NULL;
      
        m_pFrontiereAmont = pFAmont;
        m_pFrontiereAval = pFAval;
}

//---------------------------------------------------------------------------
// Fonctions d'affectation des variables
//---------------------------------------------------------------------------

// Affecte pointeur sur segment amont
void SegmentMacro::setAmont(SegmentMacro *s)
{   cell_amont=s;  }

// Affecte pointeur sur segment aval
void SegmentMacro::setAval(SegmentMacro *s)
{   cell_aval=s;   }


//---------------------------------------------------------------------------
// Fonctions de renvoi des informations du segment
//---------------------------------------------------------------------------


double SegmentMacro::getConc  (void)
{  //retourne la concentration
    //return concentration;
    return m_dbConc;
}

double SegmentMacro::getNbVoie()
{
        return Nb_Voies;
}

SegmentMacro* SegmentMacro::getSegmentAmont  (void)
{  //retourne le segment en amont
 return cell_amont;
}

SegmentMacro* SegmentMacro::getSegmentAval  (void)
{  //retourne le segment en aval
  return cell_aval;
}

//---------------------------------------------------------------------------
// Fonctions utilitaires
//---------------------------------------------------------------------------

// Ecriture des fichiers de sortie trafic
void SegmentMacro::TrafficOutput()
{
	std::string ssTuyau = "";
	std::string ssLib = GetLabel();
    std::deque< TimeVariation<TraceDocTrafic> >::iterator itXmlDocTrafic;

	// Ecriture des fichiers r�sultat
    for( itXmlDocTrafic = m_pReseau->m_xmlDocTrafics.begin(); itXmlDocTrafic!= m_pReseau->m_xmlDocTrafics.end(); ++itXmlDocTrafic )
    {
		itXmlDocTrafic->m_pData->AddCellSimu(m_nID, m_dbConc, m_dbDebit, m_pFrontiereAmont->GetVit(0), m_pFrontiereAmont->GetAcc(0), m_pFrontiereAmont->GetN(0), m_pFrontiereAval->GetVit(0), m_pFrontiereAval->GetAcc(0), m_pFrontiereAval->GetN(0)
            , ssLib, ssTuyau, GetAbsAmont(), GetOrdAmont(), GetHautAmont(), GetAbsAval(),GetOrdAval(), GetHautAval() );	
    }
}

//================================================================
    void SegmentMacro::AddVoisinAmont
//----------------------------------------------------------------
// Fonction  : Ajoute un voisin � la liste des voisins amont
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro*   pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;
    Voisin  v;

    // On v�rifie si le segment est d�j� dans la liste
    debut   = m_LstVoisinsAmont.begin();
    fin     = m_LstVoisinsAmont.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return;
    }

    // Sinon on l'ajoute
    v.pSegment              = pSegment;
    v.dbDebit               = 0;
    v.dbDemande             = 0;
    v.dbDebitInjecteCumule  = 0;

    m_LstVoisinsAmont.push_back(v);
}

//================================================================
    void SegmentMacro::AddVoisinAval
//----------------------------------------------------------------
// Fonction  : Ajoute un voisin � la liste des voisins aval
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro *   pSegment,
    double      dbDebit
)
{
    std::deque <Voisin>::iterator cur, debut, fin;
    Voisin  v;

    // On v�rifie si le segment est d�j� dans la liste
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
        {
            cur->dbDebit = dbDebit;
            return;
        }
    }

    // Sinon on l'ajoute
    v.pSegment              = pSegment;
    v.dbDebit               = dbDebit;
    v.dbDemande             = 0;
    v.dbDebitInjecteCumule  = 0;

    m_LstVoisinsAval.push_back(v);
}

//================================================================
    double SegmentMacro::GetDebitInjecteTo
//----------------------------------------------------------------
// Fonction  : Renvoie le d�bit inject� au segment pass� en param�tre
//             par le segment courant
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro         *pSegment,
    bool            bAnt        /* = false */       // Indique si on renvoie le d�bit du pas de temps
                                                    // pr�c�dent
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
        {
            if(bAnt)
                return cur->dbDebitAnt;
            else
                return cur->dbDebit;
        }
    }

    // Segment non trouv�, on retourne un d�bit nul
    return 0;
}

//================================================================
    double SegmentMacro::GetDebitEquiTo
//----------------------------------------------------------------
// Fonction  : Renvoie le d�bit inject� au segment pass� en param�tre
//             par le segment courant
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro         *pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
                return cur->dbDebitEqui;
    }

    // Segment non trouv�, on retourne un d�bit nul
    return 0;
}


//================================================================
    void SegmentMacro::SetDebitInjecteTo
//----------------------------------------------------------------
// Fonction  : Affecte le d�bit inject� au segment pass� en param�tre
//             par le segment courant
// Version du: 10/01/2005
// Historique: 10/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *  pSegment,
    double          dbDebit,
    bool            bMAJAnt     /* = true */    // Indique si il faut mettre � jour
                                                // le d�bit ant�rieur
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
        {
            if( bMAJAnt )
                cur->dbDebitAnt = cur->dbDebit;
            cur->dbDebit = dbDebit;
            cur->dbDebitInjecteCumule += dbDebit;
        }
    }
}

//================================================================
    void SegmentMacro::SetDebitEquiTo
//----------------------------------------------------------------
// Fonction  : Affecte le d�bit inject� au segment pass� en param�tre
//             par le segment courant
// Version du: 10/01/2005
// Historique: 10/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *  pSegment,
    double          dbDebit,
    bool            bAdd        /*=false*/
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
        {
            if(bAdd)
                cur->dbDebitEqui += dbDebit;
            else
                cur->dbDebitEqui = dbDebit;
        }
    }
}

//================================================================
    double SegmentMacro::GetDebitInjecteCumuleTo
//----------------------------------------------------------------
// Fonction  : Renvoie le d�bit inject� cumul� au segment pass� en param�tre
//             par le segment courant
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro         *pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return cur->dbDebitInjecteCumule;
    }

    // Segment non trouv�, on retourne un d�bit nul
    return 0;
}

//================================================================
    void SegmentMacro::SetDebitInjecteCumuleTo
//----------------------------------------------------------------
// Fonction  : Affecte le d�bit inject� cumule au segment pass� en param�tre
//             par le segment courant
// Version du: 10/01/2005
// Historique: 10/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *pSegment,
    double       dbDebit
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            cur->dbDebitInjecteCumule = dbDebit;
    }
}

//================================================================
    void SegmentMacro::SetDemandeTo
//----------------------------------------------------------------
// Fonction  : Affecte la demande du segment courant pour
//             le segment pass� en param�tre
// Version du: 10/01/2005
// Historique: 10/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *pSegment,
    double       dbDemande
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            cur->dbDemande = dbDemande;
    }
}

//================================================================
    double SegmentMacro::GetDemandeTo
//----------------------------------------------------------------
// Fonction  : Retourne la demande du segment courant pour le
//             segment voisin pass� en param�tre
// Version du: 18/01/2005
// Historique: 18/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return cur->dbDemande;
    }
    
    return 0;
}

//================================================================
    double SegmentMacro::GetDemandeFrom
//----------------------------------------------------------------
// Fonction  : Retourne la demande du segment pass� en param�tre
//             pour le segment courant
// Version du: 18/01/2005
// Historique: 18/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro      *pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments amont
    debut   = m_LstVoisinsAmont.begin();
    fin     = m_LstVoisinsAmont.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return cur->pSegment->GetDemandeTo(this);
    }
    
    return 0;
}

//================================================================
    double SegmentMacro::GetDebitRecuFrom
//----------------------------------------------------------------
// Fonction  : Renvoie le d�bit re�u par le segment courant et
//             provenant du segment pass� en param�tre
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro    *pSegment,
     bool bAnt  /* = false */    // Indique si on renvoie le d�bit du pas de temps
                                // pr�c�dent
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments amont
    debut   = m_LstVoisinsAmont.begin();
    fin     = m_LstVoisinsAmont.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return pSegment->GetDebitInjecteTo(this, bAnt);
    }

    // Segment non trouv�, on retourne un d�bit nul
    return 0;
}

//================================================================
    double SegmentMacro::GetDebitCumuleRecuFrom
//----------------------------------------------------------------
// Fonction  : Renvoie le d�bit cumul� re�u par le segment courant et
//             provenant du segment pass� en param�tre
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    SegmentMacro    *pSegment
)
{
    std::deque <Voisin>::iterator cur, debut, fin;

    // Recherche du segment dans la liste des segments amont
    debut   = m_LstVoisinsAmont.begin();
    fin     = m_LstVoisinsAmont.end();

    for (cur=debut;cur!=fin;cur++)
    {
        if( cur->pSegment == pSegment )
            return pSegment->GetDebitInjecteCumuleTo(this);
    }

    // Segment non trouv�, on retourne un d�bit nul
    return 0;
}

//================================================================
    double SegmentMacro::GetSommeDebitInjecte
//----------------------------------------------------------------
// Fonction  : Renvoie la somme des d�bits inject�s par le segment
//             courant vers les segments en aval
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    double  dbDebit;

    std::deque <Voisin>::iterator cur, debut, fin;

    dbDebit = 0;

    // Parcours de la liste des segments en aval
    debut   = m_LstVoisinsAval.begin();
    fin     = m_LstVoisinsAval.end();

    for (cur=debut;cur!=fin;cur++)
    {
        dbDebit += cur->dbDebit;
    }

    return dbDebit;
}

//================================================================
    double SegmentMacro::GetSommeDebitRecu
//----------------------------------------------------------------
// Fonction  : Renvoie la somme des d�bits re�us par le segment
//             courant et provenant des segments en amont
// Version du: 07/01/2005
// Historique: 07/01/2005 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    bool bAnt  /* = false */    // Indique si on renvoie le d�bit du pas de temps
                                // pr�c�dent
)
{
    double  dbDebit;

    std::deque <Voisin>::iterator cur, debut, fin;

    dbDebit = 0;

    // Parcours de la liste des segments en aval
    debut   = m_LstVoisinsAmont.begin();
    fin     = m_LstVoisinsAmont.end();

    for (cur=debut;cur!=fin;cur++)
        if(cur->pSegment)
        {
            dbDebit += GetDebitRecuFrom(cur->pSegment, bAnt);
        }
    return dbDebit;
}


//================================================================
    void SegmentMacro::ComputeTrafficEx
//----------------------------------------------------------------
// Fonction  :  Calcule les autres variables caract�ristiques du trafic
//              � partir de la quantit� N
// Version du:  03/07/2006
// Historique:  03/07/2006 (C.B�carie - Tinea)
//              Cr�ation
//================================================================
(
    double /*nInstant*/
)
{
    
    // D�bit de sortie de la cellule
    if( fabs(m_pFrontiereAval->GetN(0) - m_pFrontiereAval->GetN(1)) < ZERO_DOUBLE )
        m_dbDebit = 0;
    else
        m_dbDebit =  (m_pFrontiereAval->GetN(0) - m_pFrontiereAval->GetN(1)) / m_dbPasTemps;

    // Concentration (calcul�e � la fin du pas de temps)
    if( fabs(m_pFrontiereAmont->GetN(0) - m_pFrontiereAval->GetN(0)) < ZERO_DOUBLE )
        m_dbConc = 0;
    else
        m_dbConc =  ( m_pFrontiereAmont->GetN(0) - m_pFrontiereAval->GetN(0) ) / m_dbPasEspace;
}

//================================================================
    double SegmentMacro::CalculDemandeVar
//----------------------------------------------------------------
// Fonction  :  Calcule de la demande dans le cas de la m�thode
//              variationnelle
// Version du:  03/07/2006
// Historique:  03/07/2006 (C.B�carie - Tinea)
//              Cr�ation
//================================================================
(
)
{
    double  dbDemande;
    double  dbQx;

	TypeVehicule *pTV = m_pReseau->m_LstTypesVehicule[0];

    // Qmax
    dbQx = pTV->GetDebitMax();

    dbDemande = min ( m_pFrontiereAmont->GetN(1) - m_pFrontiereAval->GetN(1), dbQx * m_dbPasTemps);
    SetDemandeTo(cell_aval, dbDemande);

    return dbDemande;
}

//================================================================
    double SegmentMacro::CalculOffreVar
//----------------------------------------------------------------
// Fonction  :  Calcule de l'offre dans le cas de la m�thode
//              variationnelle
// Version du:  03/07/2006
// Historique:  03/07/2006 (C.B�carie - Tinea)
//              Cr�ation
//================================================================
(
)
{
    double dbOffre;    
    double dbNx0tp;
    double dbKx;
    double dbQx;

	TypeVehicule *pTV = m_pReseau->m_LstTypesVehicule[0];
    
    // Kmax
    dbKx = pTV->GetKx();

    // Qmax
    dbQx = pTV->GetDebitMax();

    // N( x0, t + dt ) = N( x0 + dx, t1) + Kmax * dx
    dbNx0tp = m_pFrontiereAval->GetLastN() +  dbKx * m_dbPasEspace;

    // Offre = min ( N( x0, t + dt ) -  N( x0, t ) , Qmax * dt )
    dbOffre = min ( dbNx0tp - m_pFrontiereAmont->GetN(1), dbQx * m_dbPasTemps);

    return dbOffre;
}

//================================================================
    CDiagrammeFondamental* SegmentMacro::GetDiagFonda
//----------------------------------------------------------------
// Fonction  :  Renvoie le pointeur sur le diagramme fondamental
//              du tuyau et remet � jour le contexte
//              (c'est le diagramme du tuyau grand-parent)
// Version du:  05/01/2007
// Historique:  05/01/2007 (C.B�carie - Tinea )
//              Cr�ation
//================================================================
(
)
{
    if(!GetParent())
        return NULL;

    return ((VoieMacro*)GetParent())->GetDiagFonda();
}

//================================================================
    void SegmentMacro::InitVarSimu
//----------------------------------------------------------------
// Fonction  :  Initialise les variables de simulation
// Version du:  13/07/2007
// Historique:  13/07/2007 (C.B�carie - Tinea )
//              Cr�ation
//================================================================
(
)
{
	TypeVehicule *pTV = m_pReseau->m_LstTypesVehicule[0];

    m_dbConc = 0;
    m_dbDebit = 0;

    if(m_pFrontiereAmont)
        m_pFrontiereAmont->InitSimu( pTV->GetVx() );

    if(m_pFrontiereAval)
       m_pFrontiereAval->InitSimu( pTV->GetVx() );
}

//================================================================
    int    Segment::GetSpeedRange
//----------------------------------------------------------------
// Fonction  :  Donne les bornes min et max de la plage de vitesse
// dont l'index est donn� en param�tre
// Version du:  10/04/2012
// Historique:  10/04/2012 (O.Tonck - IPSIS)
//              Cr�ation
//================================================================        
(
    double dbVit
)
{
    int result;

    if(dbVit <= 0)
    {
        result = 0;
    }
    else
    {
        // Plage particuli�re de 0 � 1 (les deux exclus)
        if(dbVit>0 && dbVit<1)
        {
            result = 1;
        }
        else
        {
            int i = (int)floor(dbVit-1.0);
            if(i % 2 == 0)
            {
                result = 2+i/2;
            }
            else
            {
                result = 2+(i-1)/2;
            }
        }
    }

    return result;
}

//================================================================
    void Segment::GetSpeedBounds
//----------------------------------------------------------------
// Fonction  :  Calcule les bornes inf�rieure et sup�rieure 
// (en km/h) de la plage dont l'index est pass� en param�tre
// Version du:  10/04/2012
// Historique:  10/04/2012 (O.Tonck - IPSIS)
//              Cr�ation
//================================================================
(
    int     nPlage,
    double& dbVitMin,
    double& dbVitMax
)
{
    if(nPlage == 0)
    {
        dbVitMin = 0;
        dbVitMax = 0;
    }
    else if(nPlage == 1)
    {
        dbVitMin = 0;
        dbVitMax = 1;
    }
    else
    {
        dbVitMin = 1+(nPlage-2)*2;
        dbVitMax = dbVitMin+2;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void Voisin::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void Voisin::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void Voisin::serialize(Archive& ar, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(pSegment);
    ar & BOOST_SERIALIZATION_NVP(dbDebit);
    ar & BOOST_SERIALIZATION_NVP(dbDebitEqui);
    ar & BOOST_SERIALIZATION_NVP(dbDemande);
    ar & BOOST_SERIALIZATION_NVP(dbDebitInjecteCumule);
    ar & BOOST_SERIALIZATION_NVP(dbDebitAnt);
}

template void Segment::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void Segment::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);
    
template<class Archive>
void Segment::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Troncon);

    ar & BOOST_SERIALIZATION_NVP(m_nID);
}
    
template void SegmentMacro::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void SegmentMacro::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void SegmentMacro::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Segment);

	ar & BOOST_SERIALIZATION_NVP(Nb_Voies);

	ar & BOOST_SERIALIZATION_NVP(cell_aval);
    ar & BOOST_SERIALIZATION_NVP(cell_amont);

    ar & BOOST_SERIALIZATION_NVP(m_pFrontiereAmont);
    ar & BOOST_SERIALIZATION_NVP(m_pFrontiereAval);

    ar & BOOST_SERIALIZATION_NVP(m_dbConc);
    ar & BOOST_SERIALIZATION_NVP(m_dbDebit);

    ar & BOOST_SERIALIZATION_NVP(m_LstVoisinsAmont);
    ar & BOOST_SERIALIZATION_NVP(m_LstVoisinsAval);
}