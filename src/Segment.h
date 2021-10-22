#pragma once
#ifndef SegmentNewH
#define SegmentNewH

#include "troncon.h"

class SegmentMacro;
class Frontiere;
class CDiagrammeFondamental;

//***************************************************************************//
//                          structure Voisin                                 //
//***************************************************************************//
struct Voisin
{
    SegmentMacro    *pSegment;       // Pointeur sur le segment voisin
    double          dbDebit;        // D�bit inject� par le segment courant vers le segment voisin
    double          dbDebitEqui;    // D�bit inject� par le segment courant vers le segment voisin lors d'unaclaul de d�bit par phase (travers�e de DM)
    double          dbDemande;      // Demande du segment courant
    double          dbDebitInjecteCumule; // D�bit inject� cumul� (utile pour le calcul de la vitesse pour le processus d�terministe)
    double          dbDebitAnt;     // D�bit inject� au cours du pas de temps pr�c�dent

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

//***************************************************************************//
//                          classe Segment                                   //
//***************************************************************************//
class Segment : public Troncon
{
public:
      Segment();
      Segment(int   nID, double dbVitReg);

        int     m_nID;          // Identifiant unique du segment pour tout le r�seau        
public:
        virtual void    ComputeTraffic(double dbInstant){};
        virtual void    ComputeTrafficEx(double dbInstant){};
        virtual void    TrafficOutput(){};

        int             GetID(){return m_nID;};

protected:
        virtual void    GetSpeedBounds(int nPlage, double& dbVitMin, double& dbVitMax);
        virtual int     GetSpeedRange(double dbVit);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

//***************************************************************************//
//                          classe SegmentMacro                              //
//***************************************************************************//

// La classe segment correpond aux �lements resultant du processus de discr�tisation
// des tron�ons macroscopiques. Les segments contiennent toutes les informations trafic simul�s dynamiquement
// par le mod�le de trafic.

class SegmentMacro : public Segment
{
private:
        // Variables caract�ristiques des segments
        int     Nb_Voies;                   // Nb de voies


protected :

        // Variables descriptives des segments
        static const double EPSILON_01;

        SegmentMacro *cell_aval; // pointeur vers segment aval au segment actuel
        SegmentMacro *cell_amont; // pointeur vers segment amont au segment actuel

        Frontiere   *m_pFrontiereAmont;  // Pointeur sur la fronti�re amont
        Frontiere   *m_pFrontiereAval;   // Pointeur sur la fronti�re aval

        double  m_dbConc;           // Concentration dans le segment
        double  m_dbDebit;          // D�bit dans le segment (TEMPORAIRE ???)

        // Gestion du voisinage
        std::deque<Voisin>   m_LstVoisinsAmont;      // Liste des voisins amont
        std::deque<Voisin>   m_LstVoisinsAval;       // Liste des voisins aval

public:
        // Constructeurs, destructeurs et associ�s
        ~SegmentMacro(void) ;
		SegmentMacro(std::string sLabel, double _pasespace, double _pastemps, int _nbVoies, Frontiere *pFAmont, Frontiere *pFAval, int &nLastID, double dbVitReg);
                // (Nom du segment, Pasespace, pastemps, NbVoies, type diagramme fluide (PL), type diagramme congestionn�)
        SegmentMacro(void);

        void InitVarSimu();

        // Fonction d'affectation des variables
        void setAmont(SegmentMacro *); // (p sur segment amont)
        void setAval(SegmentMacro *); // (p sur segment aval)

        // Fonctions de renvoi d'informations sur les segments
        double getVitesse(void);
        double getVitesseMax(void);
        double getVitesseEntree();
        double getConc(void);
        double getConcAnt(void);
        double getDemande(void);
        double getDemande(double dbConc);
        double GetOffre(bool bOffreModifie, double dbConc);
        double getNbVoie();
        SegmentMacro* getSegmentAmont(void);
        SegmentMacro* getSegmentAval(void);


        // Fonctions utilitaires
        void TrafficOutput();        

        // Fonction de gestion du voisinage
        void    AddVoisinAmont (SegmentMacro *pSegment);
        void    AddVoisinAval  (SegmentMacro *pSegment, double dbDebit);

        double  GetDebitInjecteTo(SegmentMacro *pSegment, bool bAnt = false);
        void    SetDebitInjecteTo(SegmentMacro *pSegment, double dbDebit, bool bMAJAnt = true);

        double  GetDebitEquiTo(SegmentMacro *pSegment);
        void    SetDebitEquiTo(SegmentMacro *pSegment, double dbDebit, bool bAdd = false);

        double  GetDebitRecuFrom(SegmentMacro *pSegment, bool bAnt = false);

        double  GetSommeDebitInjecte();
        double  GetSommeDebitRecu(bool bAnt = false);

        double  GetDemandeTo(SegmentMacro *pSegment);
        void    SetDemandeTo(SegmentMacro *pSegment, double dbDemande);
        double  GetDemandeFrom(SegmentMacro  *pSegment);

        double  GetDebitInjecteCumuleTo(SegmentMacro *pSegment);
        void    SetDebitInjecteCumuleTo(SegmentMacro *pSegment, double dbDebit);

        double  GetDebitCumuleRecuFrom(SegmentMacro *pSegment);

        CDiagrammeFondamental*  GetDiagFonda();

        // Gestion des DMF
//        void    MiseAJourListeDMF(Tuyau *Tuyau, std::deque<DiscontinuiteMobileFictive*>*  pLstDMF);
//        void    AddDMFGestionFeux(Tuyau *Tuyau, std::deque<DiscontinuiteMobileFictive*>*  pLstDMF, double dbInstant);
        void    CalculVitessePositionDMF();

        std::deque<Voisin>   GetLstVoisinsAval(){return m_LstVoisinsAval;};
        std::deque<Voisin>   GetLstVoisinsAmont(){return m_LstVoisinsAmont;};

    virtual void    ComputeTraffic        (double dbInstant){};
    virtual void    ComputeTrafficEx(double dbInstant);

        double GetDebit(){return m_dbDebit;};

        Frontiere*  GetFrontiereAmont(){return m_pFrontiereAmont;};
        Frontiere*  GetFrontiereAval(){return m_pFrontiereAval;};

        double      CalculDemandeVar();
        double      CalculOffreVar();      


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};
#endif
