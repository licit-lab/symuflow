#pragma once
#ifndef DiagFondaH
#define DiagFondaH  

namespace boost {
    namespace serialization {
        class access;
    }
}

#include <fstream>   

//------------------------------------------------------------------------------
//  Code des types de calcul
//------------------------------------------------------------------------------
    const int CALCUL_DEBIT        = 1;            // D�bit
    const int CALCUL_OFFRE        = 2;            // Offre
    const int CALCUL_DEMANDE      = 3;            // Demande
//------------------------------------------------------------------------------
//  Code de retour des m�thodes de calcul
//------------------------------------------------------------------------------
    const int ERR_ERREUR        = 0;            // Echec
    const int ERR_SUCCES        = 1;            // Succ�s
    const int ERR_NONTRAITE     = 2;            // Cas non trait� - pas de r�sultat
    const int ERR_DIVZERO       = 3;            // Division par z�ro
    const int ERR_DONNEES       = 4;            // Incoh�rence dans les donn�es
//------------------------------------------------------------------------------
    const int VAL_NONDEF        = -99999;       // Valeur non d�finie

class CDiagrammeFondamental
{

protected:

    // Donn�es du DF
    double      m_dbVitesseLibre;       // vitesse libre   
    double      m_dbKMax;               // Concentration maximum
    double      m_dbW;                  // W

    // Variables calcul�es
    double      m_dbDebitMax;           // d�bit maximum    
    double      m_dbKCritique;          // Concentration critique    

	std::ofstream   *m_pFicLog;              // Fichier log
	std::string     m_strContexte;    // Contexte (cha�ne de caract�re permettant de r�cup�rer
                                        // le contexte lorsque une erreur est trac�e dans le fichier log
    
public:
        // Constructeur
        CDiagrammeFondamental       ();
        CDiagrammeFondamental     (	
                                        double      dbW,										
										double		dbKMax,                                                                             
                                        double      dbVitMax,                                                                                                                        
										std::ofstream    *pFicLog
                                    );		

        // M�thodes Get     
        double  GetW()                  {return m_dbW;};
        double  GetVitesseLibre()       {return m_dbVitesseLibre;};       
        double  GetDebitMax()           {return m_dbDebitMax;};
        double  GetKMax()               {return m_dbKMax;};        
        double  GetKCritique()          {return m_dbKCritique;};

		void	SetProperties			(	
                                        double      dbW,										
										double		dbKMax,                                                                             
                                        double      dbVitMax                                                                                                                										
										);

        // M�thodes Set
		void    SetContexte(std::string sContexte);

        // Calcul du d�bit
        int     CalculDebitEqDiagrOrig(
                                        double &dbConcentration,
                                        double &dbDebit,
                                        int    nCalcul = CALCUL_DEBIT
                                        );            

        // Calcul de la concentration
        int     CalculConcEqDiagrOrig(
                                        double  &dbConcentration,
                                        double  &dbDebit,
                                        bool    bFluide
                                    );

          
        // Calcul de la vitesse
        int     CalculVitEqDiagrOrig(
                                        double  &dbVitesse,
                                        double  &dbConcentration,
                                        double  &dbDebit,
                                        bool    bFluide = true
                                        );                

private:

        // Trace
        void    TraceErreur(const char strMethode[64], const char strComment[1024]);     

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif

