#include "stdafx.h"
#include "CarrefourAFeux.h"
#include "repartiteur.h"
#include "reseau.h"
#include "tools.h"
#include "ControleurDeFeux.h"

//================================================================
CarrefourAFeux::CarrefourAFeux
//----------------------------------------------------------------
// Fonction  : Constructeur
// Version du: 29/04/2008
// Historique: 29/04/2008 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(   
    char    *strID, 
    double  dbVitMax, 
    double  dbTagreg, 
    double  dbGamma, 
    double  dbMu ,     
    double  dbLong,
    Reseau *pReseau
):BriqueDeConnexion(strID, pReseau, 'C')
{
    m_dbVitMax = dbVitMax;
    m_dbTagreg = dbTagreg;
    m_dbGamma = dbGamma;
    m_dbMu = dbMu;    
    m_dbLongueurCellAcoustique = dbLong;
};

//================================================================
    void CarrefourAFeux::Init
//----------------------------------------------------------------
// Fonction  : Initialisation du carrefour � feux (construction des 
// objets -tron�ons et connexions- internes)
// Version du: 29/04/2007
// Historique: 29/04/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    Convergent  *pCvgt;
    Divergent   *pDvgt;
    Repartiteur *pRep;       
    EntreeCAF eCAF;
    TuyauMicro  *pT;
    char        sTmp[256];
    char        sNum[32];
    Tuyau *pTP;
    int nInd;
    Point  ptTra;
    double dbPos, dbPosP;
    int nCellAcous;

    // Boucle sur les entr�es du carrefour afin de construire tous les �l�ments utiles
    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
        eCAF = m_LstEntrees[i];

        // Cr�ation du r�partiteur avec feu ( 1 tron�on amont - 2 tron�ons aval : tron�on commun interne et tourne � droite)
        strcpy_s(sTmp, GetLabel());
        strcat_s(sTmp, "R");
        _itoa_s(i+1, sNum, 10);
        strcat_s(sTmp, sNum);

        pRep = new Repartiteur(sTmp, 'M', m_pReseau);
        m_pReseau->GetLstRepartiteurs()->push_back(pRep);
        eCAF.pTAm->setConnectionAval(pRep);
        eCAF.pTAm->SetTypeAval('R');
        pRep->AddEltAmont(eCAF.pTAm);

        pRep->SetNbMaxVoies(1);            

        // Cr�ation du divergent ( 1 tron�on aval - 2 tron�ons amont)
        strcpy_s(sTmp, GetLabel());
        strcat_s(sTmp, "D");
        _itoa_s(i+1, sNum, 10);
        strcat_s(sTmp, sNum);

        pDvgt = new Divergent(sTmp, m_pReseau);
        m_pReseau->AddDivergent(pDvgt);        

        // Cr�ation du convergent multipe de sortie ( 3 tron�ons amont - 1 tron�on aval)
        strcpy_s(sTmp, GetLabel());
        strcat_s(sTmp, "C");
        _itoa_s(i+1, sNum, 10);
        strcat_s(sTmp, sNum);

        pCvgt = new Convergent(sTmp, m_dbTagreg, m_dbGamma, m_dbMu , m_pReseau);         
        m_pReseau->AddConvergent(pCvgt);

        eCAF.pTAvTtDroit->setConnectionAmont(pCvgt);
        eCAF.pTAvTtDroit->SetTypeAmont('C');
        pCvgt->AddEltAval(eCAF.pTAvTtDroit);                                      
        
        // Cr�ation tron�on repartiteur -> divergent (tron�on commun interne)
        strcpy_s(sTmp, pRep->getNom());
        strcat_s(sTmp, "_");
        strcat_s(sTmp, pDvgt->getNom());

        pT = new TuyauMicro(m_pReseau, sTmp, 'R', 'D', pDvgt, pRep, 'M', eCAF.pTAm->GetRevetement(), eCAF.pTAm->getLargeurVoie()
            , eCAF.pTAm->GetAbsAval(), eCAF.pTAm->GetOrdAval(), 
            eCAF.ptDiv.dbX, eCAF.ptDiv.dbY, eCAF.pTAm->GetHautAval(), eCAF.ptDiv.dbZ, 
            1, m_pReseau->GetPasTemps()
            , 1, eCAF.pTAm->GetVitReg() );

        // Recopie des vitesses r�glementaires / types de v�hicule
        for(int j=0; j< (int)m_pReseau->GetLstTypesVehicule().size();j++)
        {
            pT->AddVitRegByTypeVeh( eCAF.pTAm->GetVitRegByTypeVeh( m_pReseau->GetLstTypesVehicule()[j]), m_pReseau->GetLstTypesVehicule()[j] );
        }

        m_pReseau->GetLstTuyauxMicro()->push_back(pT);
        m_pReseau->GetLstTuyaux()->push_back(pT);

        pT->SetBriqueParente(this);

        pRep->AddEltAval(pT);  
        pDvgt->AddEltAmont(pT);

        pT->AddTypeVeh(m_pReseau->GetLstTypesVehicule()[0]);
        pT->CalculeLongueur(0);

        if(!m_pReseau->IsSortieAcoustiquePonctuel())
        {
            nCellAcous = (int)System::Math::Round(pT->GetLongueur() / m_dbLongueurCellAcoustique);
            if(nCellAcous==0)
                nCellAcous=1;
            pT->SetNbCellAcoustique(nCellAcous);
            pT->SetNbCell(nCellAcous);
        }

        // Cr�ation tron�on divergent -> convergent multiple (tron�on tout droit interne)
        strcpy_s(sTmp, pDvgt->getNom());
        strcat_s(sTmp, "_");
        strcat_s(sTmp, pCvgt->getNom());

        pT = new TuyauMicro(m_pReseau, sTmp, 'D', 'C', pCvgt, pDvgt, 'M', eCAF.pTAm->GetRevetement(), eCAF.pTAm->getLargeurVoie()
            , eCAF.ptDiv.dbX, eCAF.ptDiv.dbY, eCAF.pTAvTtDroit->GetAbsAmont(), eCAF.pTAvTtDroit->GetOrdAmont()
            , eCAF.ptDiv.dbZ, eCAF.pTAvTtDroit->GetHautAmont()
            , 1, m_pReseau->GetPasTemps()
            , 1, eCAF.pTAm->GetVitReg() );

         // Recopie des vitesses r�glementaires / types de v�hicule
        for(int j=0; j< (int)m_pReseau->GetLstTypesVehicule().size();j++)
        {
            pT->AddVitRegByTypeVeh( eCAF.pTAm->GetVitRegByTypeVeh( m_pReseau->GetLstTypesVehicule()[j]), m_pReseau->GetLstTypesVehicule()[j] );
        }

        m_pReseau->GetLstTuyauxMicro()->push_back(pT);
        m_pReseau->GetLstTuyaux()->push_back(pT);

        pT->SetBriqueParente(this);

        pDvgt->AddEltAval(pT);  
        pCvgt->AddEltAmont(pT);
        pCvgt->AddTuyauAmont(pT, 2, 1);

        pT->AddTypeVeh(m_pReseau->GetLstTypesVehicule()[0]);
        pT->CalculeLongueur(0);        

        if(!m_pReseau->IsSortieAcoustiquePonctuel())
        {
            nCellAcous = (int)System::Math::Round(pT->GetLongueur() / m_dbLongueurCellAcoustique);
            if(nCellAcous==0)
                nCellAcous=1;
            pT->SetNbCellAcoustique(nCellAcous);
            pT->SetNbCell(nCellAcous);
        }
    }

    // Deuxi�me passe
    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
        eCAF = m_LstEntrees[i];

        // Cr�ation tron�on tourne � droite
        pRep = (Repartiteur*)eCAF.pTAm->getConnectionAval();
        pCvgt = (Convergent*)eCAF.pTAvTourneDroite->getConnectionAmont();

        strcpy_s(sTmp, pRep->getNom());
        strcat_s(sTmp, "_");
        strcat_s(sTmp, pCvgt->getNom());

        pT = new TuyauMicro(m_pReseau, sTmp, 'R', 'C', pCvgt, pRep, 'M', eCAF.pTAm->GetRevetement(), eCAF.pTAm->getLargeurVoie()
            , eCAF.pTAm->GetAbsAval(), eCAF.pTAm->GetOrdAval(), eCAF.pTAvTourneDroite->GetAbsAmont(), eCAF.pTAvTourneDroite->GetOrdAmont()
            , eCAF.pTAm->GetHautAval(), eCAF.pTAvTourneDroite->GetHautAmont()
            , 1, m_pReseau->GetPasTemps()
            , 1, m_dbVitMax );
        m_pReseau->GetLstTuyauxMicro()->push_back(pT);
        m_pReseau->GetLstTuyaux()->push_back(pT);

        pT->SetBriqueParente(this);

        pRep->AddEltAval(pT);
        pCvgt->AddEltAmont(pT);  
        pCvgt->AddTuyauAmont(pT, 2, 2);

        pT->AddTypeVeh(m_pReseau->GetLstTypesVehicule()[0]);
        pT->CalculeLongueur(0);

        if(!m_pReseau->IsSortieAcoustiquePonctuel())
        {
            nCellAcous = (int)System::Math::Round(pT->GetLongueur() / m_dbLongueurCellAcoustique);
            if(nCellAcous==0)
                nCellAcous=1;
            pT->SetNbCellAcoustique(nCellAcous);
            pT->SetNbCell(nCellAcous);
        }

    }

    // Trois�me passe
    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
        eCAF = m_LstEntrees[i];

        // Cr�ation tron�on tourne � gauche ( � partir du divergent )
        pDvgt = (Divergent*)eCAF.pTAm->getConnectionAval()->getP_aval()[0]->getConnectionAval();
        pCvgt = (Convergent*)eCAF.pTAvTourneGauche->getConnectionAmont();

        strcpy_s(sTmp, pDvgt->getNom());
        strcat_s(sTmp, "_");
        strcat_s(sTmp, pCvgt->getNom());

        pT = new TuyauMicro(m_pReseau, sTmp, 'D', 'C', pCvgt, pDvgt, 'M', eCAF.pTAm->GetRevetement(), eCAF.pTAm->getLargeurVoie()
            , eCAF.ptDiv.dbX, eCAF.ptDiv.dbY, eCAF.pTAvTourneGauche->GetAbsAmont(), eCAF.pTAvTourneGauche->GetOrdAmont()
            , eCAF.ptDiv.dbZ, eCAF.pTAvTourneGauche->GetHautAmont()
            , 1, m_pReseau->GetPasTemps()
            , 1, m_dbVitMax );
        m_pReseau->GetLstTuyauxMicro()->push_back(pT);
        m_pReseau->GetLstTuyaux()->push_back(pT);

        pT->SetBriqueParente(this);

        pDvgt->AddEltAval(pT);
        pCvgt->AddEltAmont(pT);  
        pCvgt->AddTuyauAmont(pT, 2, 3);

        pDvgt->Init();

        pT->AddTypeVeh(m_pReseau->GetLstTypesVehicule()[0]);
        pT->CalculeLongueur(0);

        if(!m_pReseau->IsSortieAcoustiquePonctuel())
        {
            nCellAcous = (int)System::Math::Round(pT->GetLongueur() / m_dbLongueurCellAcoustique);
            if(nCellAcous==0)
                nCellAcous=1;
            pT->SetNbCellAcoustique(nCellAcous);
            pT->SetNbCell(nCellAcous);
        }
    }
    
    // fin d'initialisation
    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
         eCAF = m_LstEntrees[i];
         pRep = (Repartiteur*)eCAF.pTAm->getConnectionAval();         

         pRep->calculBary();
         pRep->calculRayon();
         pRep->Constructeur_complet(pRep->GetNbElAmont(),pRep->GetNbElAval(),false);

         pCvgt = (Convergent*)eCAF.pTAm->getConnectionAval()->getP_aval()[0]->getConnectionAval()->getP_aval()[0]->getConnectionAval();         
         pCvgt->Init(eCAF.pTAvTtDroit, 10);
         
         // Ajout des travers�es pour les mouvements de tourne � gauche
         pDvgt = (Divergent*)eCAF.pTAm->getConnectionAval()->getP_aval()[0]->getConnectionAval();
         pT = (TuyauMicro*)pDvgt->getP_aval()[1];                 

         nInd = i+2;
         if(nInd >= (int)m_LstEntrees.size())
             nInd -= (int)m_LstEntrees.size();

         pTP = m_LstEntrees[nInd].pTAm->getConnectionAval()->getP_aval()[0]->getConnectionAval()->getP_aval()[0];
         if( CalculIntersection(pT, pTP, ptTra) )
         {
            dbPos = GetDistance(*pT->GetExtAmont(), ptTra);
            dbPosP = GetDistance(*pTP->GetExtAmont(), ptTra);
            Traversee *pTra = pT->AddTraversee( pTP, dbPos, dbPosP );

            // Temps de travers�e
            for(int j=0; j<(int)m_LstTf.size(); j++)
            {
                pT->AddTf( pTra, m_LstTf[j].pTV, m_LstTf[j].dbtc );
            }
         }
    }
}

//================================================================
    void CarrefourAFeux::AddEntree
//----------------------------------------------------------------
// Fonction  : Ajout une entr�e � la liste
// Version du: 29/04/2007
// Historique: 29/04/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    TuyauMicro*  pTAm, 
    TuyauMicro*  pTAvTtDroit, 
    TuyauMicro*  pTAvTourneDroite, 
    TuyauMicro*  pTAvTourneGauche, 
    Point   ptDiv
)
{
    EntreeCAF eCAF;

    eCAF.pTAm = pTAm;
    eCAF.pTAvTourneDroite = pTAvTourneDroite;
    eCAF.pTAvTourneGauche = pTAvTourneGauche;
    eCAF.pTAvTtDroit = pTAvTtDroit;
    eCAF.ptDiv = ptDiv;

    m_LstEntrees.push_back(eCAF);
}

//================================================================
    Repartiteur* CarrefourAFeux::GetRepartiteurEntree
//----------------------------------------------------------------
// Fonction  : Retourne le r�partiteur de l'entr�e d�sign�e par le 
//             tron�on amont pass� en param�tre
// Version du: 29/04/2007
// Historique: 29/04/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    TuyauMicro* pT
)
{
    EntreeCAF eCAF;

    if(!pT)
        return NULL;

    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
        eCAF = m_LstEntrees[i];
        if(eCAF.pTAm == pT)
            return (Repartiteur*)pT->getConnectionAval();
    }

    return NULL;
}

//================================================================
    void    CarrefourAFeux::AddTf
//----------------------------------------------------------------
// Fonction  : Ajout un temps d'insertion pour un type de v�hicule
// Remarque  : 
// Version du: 23/11/2007
// Historique: 23/11/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    TypeVehicule *pTV, 
    double dbTf
)
{
    TempsCritique TpsIns;

    TpsIns.pTV = pTV;
    TpsIns.dbtc = dbTf;

    m_LstTf.push_back(TpsIns);
}

//================================================================
    void CarrefourAFeux::UpdateConvergents
//----------------------------------------------------------------
// Fonction  : Mise � jour des convergents (priorit�) en fonction de la 
//             couleur des feux
// Version du: 19/05/2008
// Historique: 19/05/2008 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    double dbInstant
)
{
    Repartiteur *pRep;
    Convergent  *pCvgt;    
    Tuyau       *pTD, *pTG, *pTM;
    double      dbPropVert;

    for(int i=0; i<(int)m_LstEntrees.size(); i++)
    {
        pRep = (Repartiteur*)m_LstEntrees[i].pTAm->getConnectionAval();

        pCvgt = (Convergent*)m_LstEntrees[i].pTAvTtDroit->getConnectionAmont();        

        pTM = (Tuyau*)pCvgt->getP_amont()[0];
        pTD = (Tuyau*)pCvgt->getP_amont()[1];
        pTG = (Tuyau*)pCvgt->getP_amont()[2];
        
        if(m_pCtrlDeFeux)
        {
            if(m_pCtrlDeFeux->GetCouleurFeux(dbInstant, m_LstEntrees[i].pTAm, m_LstEntrees[i].pTAvTtDroit, dbPropVert) )
            {
                pCvgt->UpdatePriorite(pTM, 1);
                pCvgt->UpdatePriorite(pTD, 2);
                pCvgt->UpdatePriorite(pTG, 3);
            }
            else
            {
                pCvgt->UpdatePriorite(pTM, 3);
                pCvgt->UpdatePriorite(pTD, 1);
                pCvgt->UpdatePriorite(pTG, 2);
            }
        }
    }
}

//================================================================
    bool    CarrefourAFeux::GetTuyauxInternes
//----------------------------------------------------------------
// Fonction  : Retourne la liste des tuyaux internes du CAF
//             d�finissant l'itin�raire de pTAm � pTAv
// Version du: 30/09/2008
// Historique: 30/09/2008 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
( 
    Tuyau *pTAm, 
    Tuyau *pTAv, 
    std::deque<Tuyau*> &dqTuyaux
)
{
    Tuyau   *pT;
    int     i;

    // Recherche de l'entr�e
    for(i=0; i< (int)m_LstEntrees.size(); i++)
    {
        if( pTAm == m_LstEntrees[i].pTAm )
            break;
    }

    // Recherche du mouvement :

    // Tourne � droite
    if( pTAv == m_LstEntrees[i].pTAvTourneDroite )
    {
        dqTuyaux.push_back( pTAm->getConnectionAval()->getP_aval()[1] );
        return true;
    }

    // Tout droit    
    if( pTAv == m_LstEntrees[i].pTAvTtDroit )
    {
        pT = pTAm->getConnectionAval()->getP_aval()[0];
        dqTuyaux.push_back(pT);
        dqTuyaux.push_back( pT->getConnectionAval()->getP_aval()[0] );
        return true;
    }

    // Tourne � gauche    
    if( pTAv == m_LstEntrees[i].pTAvTourneGauche )
    {
        pT = pTAm->getConnectionAval()->getP_aval()[0];
        dqTuyaux.push_back(pT);
        dqTuyaux.push_back( pT->getConnectionAval()->getP_aval()[1] );
        return true;
    }

    return false;
}

void CarrefourAFeux::SetCtrlDeFeux(ControleurDeFeux *pCDF)
{
    Repartiteur *pRep;

    m_pCtrlDeFeux = pCDF;

    for(int i=0; i< (int)m_LstEntrees.size(); i++)
    {
        pRep = (Repartiteur*)(m_LstEntrees[i].pTAm)->getConnectionAval();     

        if(pRep)
            pRep->SetCtrlDeFeux( m_pCtrlDeFeux );
    }
}
