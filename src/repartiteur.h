#pragma once
#ifndef repartiteurH
#define repartiteurH

#include "ConnectionPonctuel.h"

class Tuyau;

// La classe r�partiteur est une classe fille de connection. Le r�partiteur g�re
// les �changes de flux entre les diff�rents tron�ons situ�s amont et aval.

class Repartiteur :  public ConnectionPonctuel
{
private:

        // Variables caract�ristiques du r�partiteur

	    char m_cType;       // Type de r�partiteur (M: macro (flux) ; H: microscopique)

        // Param�tres g�om�triques du r�partiteur        
        double rayon; // rayon du r�partiteur        

        // Variables de simulation
        std::vector<std::vector<double> > debit_entree; // Tableau des d�bits pour chaque entr�e du r�partiteur
        std::vector<std::vector<double> > debit_sortie; // Tableau des d�bits pour chaque sortie du r�partiteur
        std::vector<std::vector<bool> > sortie_j_fluide; // Tableau de bool�ans indiquant si les sorties sont dans un �tat fluide (true) ou congestionn� (false)
        std::vector<std::vector<bool> > entree_i_fluide; // Tableau de bool�ans indiquant si les entr�es sont dans un �tat fluide (true) ou congestionn� (false)

        int     m_NbEltAmont; // taille des double buffers
        int     m_NbEltAval; // taille des double buffers

        std::vector<std::vector<double> > Vmax; // Tableau des vitesses maximales autoris�es au niveau de chacune des sorties du r�partiteur

public:

    // Constructeurs, destructeurs et assimil�s
    virtual ~Repartiteur(void); // Destructeur
    Repartiteur(void) ; // Constructeur par d�faut
    Repartiteur(std::string, char _Type, Reseau *pR) ; 
	Repartiteur(std::string ,char, double, double, double, double, double,Reseau *pR); // Constructeur normal (Nom, Type, Abscisse, Ordonnee, Rayon)

    void Constructeur_complet(int,int, bool bInit = true); // constructeur complet du r�partiteur (Nb elt amont, Nb elt aval)
                                            // son r�le est d'initialiser les tableaux dynamiques

    void    FinInit(Logger *pChargement);

    // cr�ation automatique des mouvements autoris�s pour un r�partiteur g�n�r� automatiquement pour l'insertion
    void CreateMouvementsAutorises(int nbVoiesInsertionDroite, int nbVoiesInsertiongauche, const std::string & strTAvPrincipal);

    // Fonctions relatives � la simulation des r�partiteurs
    void    InitSimuTrafic(void);        // Initialise la simualtion des r�partiteurs
    void    CalculEcoulementConnections(double);    // Simulation � chaque pas de temps (instant de la simulation , pas de temps, debit cumule ?)
    void    FinSimuTrafic();						// Termine la simulation des r�partiteurs

    void    CalculVitMaxSortie(double  dbTime);               // Calcul des vitesses maximales au niveau des sorties

    // Fonctions li�es � la gestion des autobus
    bool FeuOk(Tuyau*, int);   // retourne true si le feu est vert pour passer d'un troncon a un autre (pointeur troncon amont, pointeur troncon aval)

    // Fonctions de renvoi de l'�tat du r�partiteur
    double Debit_entree(std::string, int nVoie); // renvoi du d�bit entrant provenant un troncon donn� (nom troncon)
    double Debit_sortie(std::string, int nVoie); // renvoi du d�bit sortant vers un troncon donn� (nom troncon)
    bool get_entree_fluide(std::string, int nVoie); // renvoi de l'�tat de l'entr�e pour un troncon donn� (nom troncon)
    bool get_sortie_fluide(std::string, int nVoie); // renvoi de l'�tat de la sortie pour un troncon donn� (nom troncon)
    double getVitesseMaxEntree(std::string, int nVoie); // renvoi la vitesse maximum autoris�e � la sortie avec le tronc�on donn� (nom troncon)

    // Fonctions utilitaires   
    void calculRayon(void); // Calcul le rayon du r�partiteur     

    // Fonctions de renvoi des param�tres du r�partiteur
    int  getNbAmont(void); // renvoi le nombre de tron�ons en amont

    // Fonctions de modifications des param�tres du r�partieur
    void SetType(char); // Affecte le type du r�partiteur (type du r�pariteur P ponctuel, C complexe)
    void setNbAmont(int); // (Nombre de tron�ons en amont)
    void setNbAval(int); // (Nombre de tron�ons en aval)

    bool    IsFlux()    {return m_cType=='M';};
    bool    IsMicro()   {return m_cType=='H';};
        
    double GetVitesseMaxSortie(int n, int nVoie, double  dbTime);        // Retourne la vitesse maximale de sortie

    // Fonctions utilitaires
    void    AllocTabDblElt(double**   pTab, int nElt);
    void    DesallocTabDblElt(double**   pTab, int nElt);

    void    MiseAJourSAS(double dbInstant);

    void    IncNbVehRep(TypeVehicule* pTypeVeh, int i, int nVoieAmont, int j, int nVoieAval, double time);
    int     GetNbVehRep(TypeVehicule* pTypeVeh, int i, int nVoieAmont, int j, int nVoieAval, int nVar);

    //! Calcule le cout d'un mouvement autoris�
    virtual double ComputeCost(TypeVehicule* pTypeVeh, Tuyau* pTuyauAmont, Tuyau * pTuyauAval);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif
