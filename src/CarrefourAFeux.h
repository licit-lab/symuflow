//---------------------------------------------------------------------------

#ifndef CarrefourAFeuxH
#define CarrefourAFeuxH

#include <deque>
#include "BriqueDeConnexion.h"
#include "symUtil.h"
#include "convergent.h"

class TuyauMicro;
class Reseau;
class Repartiteur;

struct EntreeCAF
{
    TuyauMicro*  pTAm;
    TuyauMicro*  pTAvTtDroit;
    TuyauMicro*  pTAvTourneDroite;
    TuyauMicro*  pTAvTourneGauche;
    Point   ptDiv;
};

class CarrefourAFeux : public BriqueDeConnexion
{
public:
    // Constructeurs, destructeurs et assimil�s
    CarrefourAFeux(){}; // Constructeur par d�faut    
    CarrefourAFeux(char *strID, double dbVitMax, double dbTagreg, double dbGamma, double dbMu , 
                    double  dbLong, Reseau *pReseau);
    ~CarrefourAFeux(){}; // Destructeur

private:

    // Variables caract�ristiques du carrefour � feux
    std::deque<EntreeCAF>     m_LstEntrees;      // Liste des entr�es

    double      m_dbTagreg;                     // P�riode d'agr�gation
    double      m_dbGamma;
    double      m_dbMu;
    
    double      m_dbLongueurCellAcoustique;     // Longueur d�sir�e des cellules acoustiques

    double          m_dbVitMax;                         // Vitesse max sur l'anneau

    std::deque<TempsCritique>      m_LstTf;            // Liste des temps d'insertion    

    // Variables de simulation
public:

    void AddEntree(TuyauMicro*  pTAm, TuyauMicro*  pTAvTtDroit, TuyauMicro*  pTAvTourneDroite, TuyauMicro*  pTAvTourneGauche, Point   ptDiv);
    void Init();    // Initialisation du carrefour � feux    

    void    AddTf(TypeVehicule *pTV, double dbTf);    

    Repartiteur*    GetRepartiteurEntree(TuyauMicro *pTAm);
    void            UpdateConvergents(double dbInstant);

    std::deque<EntreeCAF> GetLstEntree(){return m_LstEntrees;};

    virtual bool    GetTuyauxInternes( Tuyau *pTAm, Tuyau *pTAv, std::deque<Tuyau*> &dqTuyaux);
    virtual void    SetCtrlDeFeux(ControleurDeFeux *pCDF);
};

#endif
