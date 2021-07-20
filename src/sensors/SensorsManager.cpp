#include "stdafx.h"
#include "SensorsManager.h"

#include "../reseau.h"
#include "../tuyau.h"
#include "../voie.h"
#include "../DocTrafic.h"
#include "../SystemUtil.h"
#include "../vehicule.h"
#include "../CSVOutputWriter.h"
#include "../TraceDocTrafic.h"
#include "GlobalSensor.h"
#include "PonctualSensor.h"
#include "LongitudinalSensor.h"
#include "BlueToothSensor.h"
#include "MFDSensor.h"
#include "TankSensor.h"

#include <boost/archive/xml_woarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

//---------------------------------------------------------------------------

//#pragma package(smart_init)

//================================================================
    SensorsManager::SensorsManager
//----------------------------------------------------------------
// Fonction  : Constructeur par d�faut
// Version du: 12/12/2006
// Historique: 12/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    m_pReseau = NULL;

    m_nPeriode = 0;
    m_dbT0 = 0.0;

	m_nNbInst = 0;
}

//================================================================
    SensorsManager::SensorsManager
//----------------------------------------------------------------
// Fonction  : Constructeur
// Version du: 12/12/2006
// Historique: 12/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    Reseau  *pReseau,                // R�seau
    double  dbPeriodeAgregation,     // P�riode d'agr�gation
    double  dbT0,                    // Instant de d�marrage de la premi�re p�riode
	bool	bGlobal
)
{
    m_pReseau = pReseau;
    m_dbPeriodeAgregation = dbPeriodeAgregation;
    m_dbT0 = dbT0;

    m_nPeriode = 0;

    m_nNbInst = -1;

	if(bGlobal)
    {
        GlobalSensor * pGlobalSensor = new GlobalSensor();
        m_LstCapteurs.push_back(pGlobalSensor);
        // Ajout du capteur � tous les tron�ons du r�seau
        for (size_t iLink = 0; iLink < pReseau->m_LstTuyaux.size(); iLink++)
        {
            pReseau->m_LstTuyaux[iLink]->getLstRelatedSensorsBySensorsManagers()[this].push_back(pGlobalSensor);
        }
    }
}

//================================================================
    SensorsManager::~SensorsManager
//----------------------------------------------------------------
// Fonction  : Destructeur
// Version du: 12/12/2006
// Historique: 12/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    std::vector<AbstractSensor*>::iterator itTmp;
    for(itTmp = m_LstCapteurs.begin(); itTmp!=m_LstCapteurs.end(); itTmp++)   
	{
		delete( *itTmp );        
	}
}

//================================================================
    PonctualSensor* SensorsManager::AddCapteurPonctuel
//----------------------------------------------------------------
// Fonction  : Cr�ation et ajout d'un capteur � la liste, retourne
//              le nombre de capteurs de la liste
// Version du: 12/12/2006
// Historique: 12/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    const char*   strNom,
    Tuyau*  pTuyau,
    double  dbPosition
)
{
    if(!m_pReseau)
		return NULL;

	PonctualSensor *pCpt = new PonctualSensor(strNom, pTuyau, dbPosition);
    pTuyau->getLstRelatedSensorsBySensorsManagers()[this].push_back(pCpt);
    pCpt->InitData(m_pReseau, m_dbPeriodeAgregation);

    m_LstCapteurs.push_back(pCpt);

    return pCpt;
}

//================================================================
    int SensorsManager::AddCapteurLongitudinal
//----------------------------------------------------------------
// Fonction  : Cr�ation et ajout d'un capteur longitudinal � la 
//             liste, retourne le nombre de capteurs de la liste
// Version du: 18/05/2011
// Historique: 18/05/2011 (O.Tonck - IPSIS)
//             Cr�ation
//================================================================
(
    char*   strNom,
    Tuyau*  pTuyau,
    double  dbPosDebut,
    double  dbPosFin
)
{
    if(!m_pReseau)
		return -1;

	LongitudinalSensor*    pCpt = new LongitudinalSensor(strNom, pTuyau, dbPosDebut, dbPosFin);
    pTuyau->getLstRelatedSensorsBySensorsManagers()[this].push_back(pCpt);
    pCpt->InitData();

    m_LstCapteurs.push_back(pCpt);

    return (int)m_LstCapteurs.size();
}

//================================================================
    int SensorsManager::AddCapteurMFD
//----------------------------------------------------------------
// Fonction  : Cr�ation et ajout d'un capteur MFD � la 
//             liste, retourne le nombre de capteurs de la liste
// Version du: 05/03/2015
// Historique: 05/03/2015 (O.Tonck - IPSIS)
//             Cr�ation
//================================================================
(
    char* strNom,
    const std::vector<Tuyau*> & Tuyaux,
    const std::vector<double> & dbPosDebut,
    const std::vector<double> & dbPosFin,
    const std::vector<int> & eTuyauType,
    bool bIsMFD
)
{
    if(!m_pReseau)
		return -1;

    MFDSensor* pCpt = new MFDSensor(m_pReseau, this, m_dbPeriodeAgregation, strNom, Tuyaux, dbPosDebut, dbPosFin, eTuyauType, bIsMFD, false);

    m_LstCapteurs.push_back(pCpt);

    return (int)m_LstCapteurs.size();
}


//================================================================
    int SensorsManager::AddCapteurReservoir
//----------------------------------------------------------------
// Fonction  : Cr�ation et ajout d'un capteur MFD � la 
//             liste, retourne le nombre de capteurs de la liste
// Version du: 05/03/2015
// Historique: 05/03/2015 (O.Tonck - IPSIS)
//             Cr�ation
//================================================================
(
    char* strNom,
    const std::vector<Tuyau*> & Tuyaux
)
{
    if (!m_pReseau)
        return -1;

    TankSensor* pCpt = new TankSensor(m_pReseau, this, strNom, Tuyaux);

    m_LstCapteurs.push_back(pCpt);

    return (int)m_LstCapteurs.size();
}

//================================================================
    int SensorsManager::AddCapteurBlueTooth
//----------------------------------------------------------------
// Fonction  : Cr�ation et ajout d'un capteur BlueTooth � la 
//             liste, retourne le nombre de capteurs de la liste
// Version du: 07/01/2013
// Historique: 07/01/2013 (O.Tonck - IPSIS)
//             Cr�ation
//================================================================
(
    char*   strNom,
    Connexion *pNode,
    const std::map<Tuyau*, double> &mapPositions
)
{
	if(!m_pReseau)
		return -1;

    BlueToothSensor* pCpt = new BlueToothSensor(this, strNom, pNode, mapPositions);

    // Ajout du capteur � la liste
    m_LstCapteurs.push_back(pCpt);

    return (int)m_LstCapteurs.size();
}

//================================================================
    bool SensorsManager::Init
//----------------------------------------------------------------
// Fonction  : Initialisation
// Version du: 14/12/2006
// Historique: 14/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    if(!m_pReseau)
        return false;

	m_nPeriode = 0;	

	std::vector<AbstractSensor*>::iterator itCpt;
	for(itCpt=m_LstCapteurs.begin(); itCpt!=m_LstCapteurs.end(); itCpt++)
	{
        (*itCpt)->ResetData();
	}	

    return true;
}

//================================================================
    void SensorsManager::Terminate
//----------------------------------------------------------------
// Fonction  : Fin
// Version du: 14/12/2006
// Historique: 14/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
)
{
    // Ecriture derni�re p�riode si pr�sente et si elle dure au moins un pas de temps
    if (m_nPeriode > 0 && (m_dbPeriodeAgregation == -1 || ((m_dbT0 + (m_nPeriode - 1)*m_dbPeriodeAgregation) != m_pReseau->GetInstSim()*m_pReseau->GetTimeStep())))
    {
        std::deque<TraceDocTrafic* > docTrafics = m_pReseau->GetXmlDocTrafic(); 
        std::deque<TraceDocTrafic* >::iterator itDocTraf;
        if( docTrafics.size()<1 && !m_pReseau->m_CSVOutputWriter)
			return;

		double dbDeb, dbFin;
        dbDeb = m_dbT0 + (m_nPeriode-1)*m_dbPeriodeAgregation;
        dbFin = m_pReseau->GetInstSim()*m_pReseau->GetTimeStep();

        if(docTrafics.size()>0)
        {
            for( itDocTraf = docTrafics.begin(); itDocTraf != docTrafics.end(); itDocTraf++ )
            {
                if(m_LstCapteurs.size() > 0)
                {            
                    (*itDocTraf)->AddPeriodeCapteur(dbDeb, dbFin, m_LstCapteurs.front()->GetSensorPeriodXMLNodeName()); 
                }
            }// rof each docTrafic
        }

        // ******************************************************
        // �criture des r�sultats des capteurs
        // ******************************************************
        for(size_t j=0; j< m_LstCapteurs.size(); j++)
        {
            m_LstCapteurs[j]->Write(m_dbT0 + (m_nPeriode) * m_dbPeriodeAgregation, m_pReseau, m_dbPeriodeAgregation, dbDeb, dbFin, docTrafics, m_pReseau->IsCSVOutput()?m_pReseau->m_CSVOutputWriter:NULL);
        }
    }    
}

//================================================================
    void SensorsManager::AddChgtVoie
//----------------------------------------------------------------
// Fonction  : Gestion du comptage des changements de voie
// Version du: 18/05/2011
// Historique: 18/05/2011 (O.Tonck - IPSIS)
//             Cr�ation
//================================================================
(
    double  dbInstant,      // Instant de simulation
    Tuyau * pTuyau,         // Tuyau dans lequel se d�roule le changement de voie
    double dbPosition,      // Position du changement de voie
    int nVoieOrigine,       // Voie d'origine du changement de voie
    int nVoieDestination    // Voie de destination du changement de voie
)
{
    if(dbInstant < m_dbT0)
    {
        return;
    }

    // pour tous les capteurs couvrant la zone du d�passement
    std::vector<AbstractSensor*>::iterator it = m_LstCapteurs.begin();
    std::vector<AbstractSensor*>::iterator itEnd = m_LstCapteurs.end();
    while(it != itEnd) {

        ((LongitudinalSensor*)(*it))->AddChgtVoie(pTuyau, dbPosition, nVoieOrigine, nVoieDestination);

        it++;
    }
}

//================================================================
    void SensorsManager::CalculInfoCapteurs
//----------------------------------------------------------------
// Fonction  : Calcul des informations des capteurs � la fin du pas
//             de temps courant
// Version du: 12/12/2006
// Historique: 12/12/2006 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
    double  dbInstant       // Instant courant    
)
{
    if(dbInstant < m_dbT0)
    {
        return;
    }

    AbstractSensor * pSensor;
    bool bNewPeriode;
    double dbDeb, dbFin, dbInstNewPeriode = 0;

    // Mise � jour de la p�riode
    bNewPeriode = false;
    if (m_dbPeriodeAgregation == -1)
    {
        // Cas des types de capteurs sans p�riode d'agr�gation
        if (m_nPeriode == 0) {
            bNewPeriode = true;
        }
    }
    else
    {
        if (dbInstant - (m_dbT0 + m_nPeriode * m_dbPeriodeAgregation) >= 0)
        {
            // On attaque une nouvelle p�riode
            bNewPeriode = true;

            // D�but de la nouvelle p�riode
            // (pour le pas de temps courant, il peut y avoir des franchissement de capteurs
            // � consid�rer dans la nouvelle p�riode...)
            dbInstNewPeriode = m_dbT0 + m_nPeriode * m_dbPeriodeAgregation;
        }
    }


    // Ecriture du nouveau noeud p�riode si besoin
    std::deque<TraceDocTrafic* > docTrafics = m_pReseau->GetXmlDocTrafic();
    std::deque<TraceDocTrafic* >::iterator itDocTraf;
    if(bNewPeriode && m_nPeriode > 0 && (!docTrafics.empty() || m_pReseau->m_CSVOutputWriter))
    {
        assert(m_dbPeriodeAgregation != -1); // On n'est pas sens�s passer ici pour les capteurs sans p�riode d'agr�gation

        dbDeb = m_dbT0 + (m_nPeriode-1)*m_dbPeriodeAgregation;
        dbFin = m_dbT0 + (m_nPeriode)*m_dbPeriodeAgregation;
        if(docTrafics.size()>0)
        {
            for( itDocTraf = docTrafics.begin(); itDocTraf != docTrafics.end(); itDocTraf++ )
            {
                if(m_LstCapteurs.size() > 0)
                {            
                    (*itDocTraf)->AddPeriodeCapteur(dbDeb, dbFin, m_LstCapteurs.front()->GetSensorPeriodXMLNodeName()); 
                }
            }// rof each docTrafic
        }
    }

	// Ajout de la contribution de chaque v�hicule du r�seau aux capteurs
    for (size_t iVeh = 0; iVeh < m_pReseau->m_LstVehicles.size(); iVeh++)
    {
        boost::shared_ptr<Vehicule> pVeh = m_pReseau->m_LstVehicles[iVeh];

        // R�cup�ration des capteurs potentiellement impact�s par le v�hicule :
        std::set<AbstractSensor*> potentiallyImpactedSensors = pVeh->GetPotentiallyImpactedSensors(this);
        // Traitement des capteurs potentiellement impact�s
        for (std::set<AbstractSensor*>::iterator iterSensor = potentiallyImpactedSensors.begin(); iterSensor != potentiallyImpactedSensors.end(); ++iterSensor)
        {
            (*iterSensor)->CalculInfoCapteur(m_pReseau, dbInstant, bNewPeriode, dbInstNewPeriode, pVeh);
        }
    }

    if (bNewPeriode)
    {
        for(size_t j=0; j< m_LstCapteurs.size(); j++)
        {
            pSensor = m_LstCapteurs[j];

            if(m_nPeriode > 0 && (!docTrafics.empty() || m_pReseau->m_CSVOutputWriter))	
            {
                pSensor->Write(m_dbT0 + (m_nPeriode) * m_dbPeriodeAgregation, m_pReseau, m_dbPeriodeAgregation, dbDeb, dbFin, docTrafics, m_pReseau->IsCSVOutput()?m_pReseau->m_CSVOutputWriter:NULL);
            }  

            pSensor->PrepareNextPeriod();
        }

        m_nPeriode++;
    }
}

//================================================================
    void SensorsManager::AddMesoVehicle
//----------------------------------------------------------------
// Fonction  : Prise en compte d'un v�hicule qui sort d'un troncon
//             meso 
// Version du: 07/09/2016
// Historique: 07/09/2016 (O.Tonck - Ipsis)
//             Cr�ation
//================================================================
(
    double dbInstant,
    Vehicule * pVeh,
    Tuyau * pLink,
    Tuyau * pDownstreamLink,
    double dbLengthInLink
)
{
    AbstractSensor * pSensor;
    for(size_t j=0; j< m_LstCapteurs.size(); j++)
    {
        pSensor = m_LstCapteurs[j];
        pSensor->AddMesoVehicle(dbInstant, pVeh, pLink, pDownstreamLink, dbLengthInLink);
    }
}

//================================================================
    void SensorsManager::UpdateTabNbVeh()
//----------------------------------------------------------------
// Fonction  : Mise � jour du tableau du nombre de v�hicule 
//             franchissant le capteur 
// Version du: 06/10/2008
// Historique: 26/11/2007 (C.B�carie - Tinea)
//             Cr�ation
//             06/10/2008 (C.B�carie - INEO TINEA)
//              Ajout possibilit� de calculer le nombre de v�hicule
//              vu par le capteur � l'aide des deltaN
//             16/01/2013 (O.Tonck - IPSIS)
//             optimisation des performances. la d�tection des
//             franchissements � lieu dans reseau.cpp
//================================================================
{
    int nLastInd = (int)(m_dbPeriodeAgregation / m_pReseau->GetTimeStep())-1;

    PonctualSensor * pSensor;
    std::vector<AbstractSensor*>::iterator beginCapt, endCapt, itCpt;
	beginCapt = m_LstCapteurs.begin();
	endCapt = m_LstCapteurs.end();

    // Calcul de l'indice du tableau � mettre � jour
    if( m_nNbInst < nLastInd )
    {
        m_nNbInst++;
        nLastInd = m_nNbInst;
    }
    else
    {
        // D�calage des donn�es du tableau afin de garder uniquement les valeurs utiles pour le calcul de la moyenne glissante
        // Pour tous les capteurs
        for(itCpt=beginCapt; itCpt!=endCapt; itCpt++)
        {
            pSensor = (PonctualSensor*)(*itCpt);
			// It�ration sur tous les types de v�hicule
			std::map<TypeVehicule*, std::map<Voie*, std::vector<double>, LessPtr<Voie>>, LessPtr<TypeVehicule>>::iterator beginTVeh, endTVeh, itTVeh;
			beginTVeh = pSensor->GetNbTypeVehInst().begin();
			endTVeh = pSensor->GetNbTypeVehInst().end();
			for(itTVeh = beginTVeh; itTVeh != endTVeh; itTVeh++)
			{				
				// It�ration sur toutes les voies
				std::map<Voie*, std::vector<double>, LessPtr<Voie>>::iterator beginVoie, endVoie, itVoie;
				beginVoie = (*itTVeh).second.begin();
				endVoie = (*itTVeh).second.end();
				for (itVoie = beginVoie; itVoie != endVoie; itVoie++)
				{
					// D�calage du tableau des compteurs
					itVoie->second.erase(itVoie->second.begin());
					itVoie->second.push_back(0);
				}
			}

        }
    }   

    // Mise � jour de la valeur de nLastInd pour les capteurs associ�s � ce couvergent
    for(itCpt=beginCapt; itCpt!=endCapt; itCpt++)
    {
        pSensor = (PonctualSensor*)(*itCpt);
        pSensor->SetLastInd(nLastInd);
    }
}

//================================================================
	bool SensorsManager::CapteurExists
//================================================================
(
	AbstractSensor *pCpt		// Capteur � tester dans la liste "m_LstCapteurs"
 )
{
	bool bNotFound = true;

	std::vector<AbstractSensor*>::iterator beginCapt, endCapt, itCpt;
	beginCapt = m_LstCapteurs.begin();
	endCapt = m_LstCapteurs.end();

	for (itCpt = beginCapt; (itCpt != endCapt) && bNotFound; itCpt++)
	{
		if ((*itCpt) == pCpt)
		{
			bNotFound = false;
		}
	}

	return !bNotFound;
}

//================================================================
    double SensorsManager::GetDebitMoyen
//----------------------------------------------------------------
// Fonction  : Retourne le d�bit moyen d'une voie vu d'un capteur et 
//             calcul� sur la p�riode d'agr�gation (moyenne glissante)
// Version du: 26/11/2007
// Historique: 26/11/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(    
    PonctualSensor *pCpt,      // Capteur demand�
    int nVoie           // Num�ro de voie demand�
)
{
	if (!CapteurExists(pCpt))
		return 0;

    double dbTmp = 0;
    int nLastInd = (int)(m_dbPeriodeAgregation / m_pReseau->GetTimeStep());

	Voie * pVoie = pCpt->GetTuyau()->GetLstLanes()[nVoie];

	// It�ration sur tous les types de v�hicule
    int kMax = min(m_nNbInst, nLastInd);
	std::map<TypeVehicule*, std::map<Voie*, std::vector<double>, LessPtr<Voie>>, LessPtr<TypeVehicule>>::iterator beginTVeh, endTVeh, itTVeh;
	beginTVeh = pCpt->GetNbTypeVehInst().begin();
	endTVeh = pCpt->GetNbTypeVehInst().end();
	for(itTVeh = beginTVeh; itTVeh != endTVeh; itTVeh++)
	{
		// It�ration sur les compteurs
		const std::vector<double> & pTabNb = (*itTVeh).second[pVoie];
		for (int k = 0; k < kMax; k++)
		{
			dbTmp += pTabNb[k];
		}
	}

    return dbTmp / (min(m_nNbInst,nLastInd) * m_pReseau->GetTimeStep());
}

//================================================================
    double SensorsManager::GetRatioTypeVehicule
//----------------------------------------------------------------
// Fonction  : Retourne le ratio du nombre de v�hicule du type pass� en param�tre
//             sur le nombre total de v�hicule
//             vu d'un capteur durant la p�riode d'agr�gation glissante
// Version du: 26/11/2007
// Historique: 26/11/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(    
    PonctualSensor *pCpt,          // Capteur demand�
    int             nVoie,          // Num�ro de voie demand�
    TypeVehicule    *pTV            // Type de v�hicule demand�
)
{
	if (!CapteurExists(pCpt))
		return 0;

    double dbVehType = 0;
    double dbTotal = 0;

    int nLastInd = (int)(m_dbPeriodeAgregation / m_pReseau->GetTimeStep());

	Voie * pVoie = pCpt->GetTuyau()->GetLstLanes()[nVoie];

	// It�ration sur tous les v�hicules
    int kMax = min(m_nNbInst, nLastInd);
	std::map<TypeVehicule*, std::map<Voie*, std::vector<double>, LessPtr<Voie>>, LessPtr<TypeVehicule>>::iterator beginTVeh, endTVeh, itTVeh;
	beginTVeh = pCpt->GetNbTypeVehInst().begin();
	endTVeh = pCpt->GetNbTypeVehInst().end();
	for(itTVeh = beginTVeh; itTVeh != endTVeh; itTVeh++)
	{
		// Choix du tableau de compteur de la voie choisie
		const std::vector<double>& pTabNb = (*itTVeh).second[pVoie];
		// It�ration sur tous les compteurs
		for(int k = 0; k < kMax ; k++)
		{
			dbTotal += pTabNb[k];
		}
	}

	// Choix du table de compteurs d'un type de v�hicule et de la voie choisie
	const std::vector<double> & pLstNb = pCpt->GetNbTypeVehInst()[pTV][pVoie];
	// It�ration sur les compteurs
	for(int k = 0; k < kMax ; k++)
	{
		dbVehType += pLstNb[k];
	}

    if(dbTotal>0)
        return dbVehType / dbTotal;
    else
        return 0;
}

//================================================================
    double SensorsManager::GetNbVehTotal
//----------------------------------------------------------------
// Fonction  : Retourne le nombre de v�hicule total
//             vu d'un capteur durant la p�riode d'agr�gation glissante
// Version du: 26/11/2007
// Historique: 26/11/2007 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(    
    PonctualSensor *pCpt,          // Capteur demand�
    int             nVoie           // Num�ro de voie demand�    
)
{  
	if (!CapteurExists(pCpt))
		return 0;

    double dbTotal = 0;

    int nLastInd = (int)(m_dbPeriodeAgregation / m_pReseau->GetTimeStep());

	Voie * pVoie = pCpt->GetTuyau()->GetLstLanes()[nVoie];

	// It�ration sur tous les v�hicules
    int kMax = min(m_nNbInst, nLastInd);
	std::map<TypeVehicule*, std::map<Voie*, std::vector<double>, LessPtr<Voie>>, LessPtr<TypeVehicule>>::iterator beginTVeh, endTVeh, itTVeh;
	beginTVeh = pCpt->GetNbTypeVehInst().begin();
	endTVeh = pCpt->GetNbTypeVehInst().end();
	for(itTVeh = beginTVeh; itTVeh != endTVeh; itTVeh++)
	{
		// Choix de la liste des compteurs associ�e � voie choisie et au type de v�hicule courant
		const std::vector<double> &pTabNb = (*itTVeh).second[pVoie];
		// It�ration sur les compteurs
		for(int k = 0; k < kMax ; k++)
		{
			dbTotal += pTabNb[k];
		}
	}

    return dbTotal;
}

AbstractSensor * SensorsManager::GetCapteurFromID(const std::string & sID)
{
	vector<AbstractSensor*>::iterator it, itEnd;
	AbstractSensor * pCpt = nullptr;

	itEnd = m_LstCapteurs.end();
	for (it=m_LstCapteurs.begin(); (it!=itEnd) && (pCpt == nullptr); ++it)
	{
		if (!sID.compare((*it)->GetUnifiedID()))
			pCpt = (*it);
	}

	return pCpt;
}


SensorsManagers::SensorsManagers()
{
    m_pGestionCapteursPonctuels = NULL;
    m_pGestionCapteursEdie = NULL;
    m_pGestionCapteursMFD = NULL;
    m_pGestionCapteursReservoir = NULL;
    m_pGestionCapteursLongitudinaux = NULL;
    m_pGestionCapteursBT = NULL;
    m_pGestionCapteurGlobal = NULL;
}

SensorsManagers::SensorsManagers(Reseau *pReseau, double dbPeriodeAgregation, double dbT0, double dbPeriodeAgregationEdie, double dbT0Edie,
        double dbPeriodeAgregationLong, double dbT0Long, double dbPeriodeAgregationMFD, double dbT0MFD,
        double dbPeriodeAgregationBT, double dbT0BT, double dbT0Reservoir)
{
    m_GestionsCapteurs.push_back(m_pGestionCapteursPonctuels = new SensorsManager(pReseau, dbPeriodeAgregation, dbT0));
    m_GestionsCapteurs.push_back(m_pGestionCapteursLongitudinaux = new SensorsManager(pReseau, dbPeriodeAgregationMFD, dbT0MFD));
    m_GestionsCapteurs.push_back(m_pGestionCapteursEdie = new SensorsManager(pReseau, dbPeriodeAgregationEdie, dbT0Edie));
    m_GestionsCapteurs.push_back(m_pGestionCapteursMFD = new SensorsManager(pReseau, dbPeriodeAgregationLong, dbT0Long));
    m_GestionsCapteurs.push_back(m_pGestionCapteursReservoir = new SensorsManager(pReseau, -1, dbT0Reservoir));
    m_GestionsCapteurs.push_back(m_pGestionCapteursBT = new SensorsManager(pReseau, dbPeriodeAgregationBT, dbT0BT));
	m_GestionsCapteurs.push_back(m_pGestionCapteurGlobal = new SensorsManager(pReseau, dbPeriodeAgregation, dbT0, true));
}

SensorsManagers::~SensorsManagers()
{
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        delete m_GestionsCapteurs[i];
    }
}



PonctualSensor* SensorsManagers::AddCapteurPonctuel(char* strNom, Tuyau *pTuyau, double dbPosition)
{
    return m_pGestionCapteursPonctuels->AddCapteurPonctuel(strNom, pTuyau, dbPosition);
}
    
int SensorsManagers::AddCapteurLongitudinal(char* strNom, Tuyau *pTuyau, double dbPosDebut, double dbPosFin)
{
    return m_pGestionCapteursLongitudinaux->AddCapteurLongitudinal(strNom, pTuyau, dbPosDebut, dbPosFin);
}

int SensorsManagers::AddCapteurMFD(char* strNom, const std::vector<Tuyau*> & Tuyaux, const std::vector<double> & dbPosDebut, const std::vector<double> & dbPosFin,
        const std::vector<int> & eTuyauType, bool bIsMFD)
{
    if(bIsMFD)
    {
        return m_pGestionCapteursMFD->AddCapteurMFD(strNom, Tuyaux, dbPosDebut, dbPosFin, eTuyauType, bIsMFD);
    }
    else
    {
        return m_pGestionCapteursEdie->AddCapteurMFD(strNom, Tuyaux, dbPosDebut, dbPosFin, eTuyauType, bIsMFD);
    }
}

int SensorsManagers::AddCapteurReservoir(char* strNom, const std::vector<Tuyau*> & Tuyaux)
{
    return m_pGestionCapteursReservoir->AddCapteurReservoir(strNom, Tuyaux);
}


int SensorsManagers::AddCapteurBlueTooth(char* strNom, Connexion *pNode, const std::map<Tuyau*, double> & mapPositions)
{
    return m_pGestionCapteursBT->AddCapteurBlueTooth(strNom, pNode, mapPositions);
}

void SensorsManagers::Init()
{
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        m_GestionsCapteurs[i]->Init();
    }
}

void SensorsManagers::Terminate()
{ 
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        m_GestionsCapteurs[i]->Terminate();
    }
}

void SensorsManagers::CalculInfoCapteurs(double dbInstSimu)
{
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        m_GestionsCapteurs[i]->CalculInfoCapteurs(dbInstSimu);
    }
}

void SensorsManagers::AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink)
{
    // Ajout de la distance meso parcourue (pour le capteur global)
    pVeh->SetDstCumulee(pVeh->GetDstCumulee() + std::min<double>(dbLengthInLink, pLink->GetLength()));

    // prise en compte de la longueur du mouvement interne si brique �ventuelle !
    if(dbLengthInLink == DBL_MAX && pDownstreamLink && pLink->GetBriqueAval())
    {
        std::vector<Tuyau*> lstTuyauxInternes;
        if(pLink->GetBriqueAval()->GetTuyauxInternes(pLink, pDownstreamLink, lstTuyauxInternes))
        {
            for(size_t i = 0; i < lstTuyauxInternes.size(); i++)
            {
                pVeh->SetDstCumulee(pVeh->GetDstCumulee() + lstTuyauxInternes[i]->GetLength());
            }
        }
    }

    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        m_GestionsCapteurs[i]->AddMesoVehicle(dbInstant, pVeh, pLink, pDownstreamLink, dbLengthInLink);
    }
}

double SensorsManagers::GetPeriodeAgregationPonctuels()
{
    return m_pGestionCapteursPonctuels->GetPeriode();
}

std::vector<AbstractSensor*> &SensorsManagers::GetCapteursPonctuels()
{
    return m_pGestionCapteursPonctuels->m_LstCapteurs;
}

PonctualSensor* SensorsManagers::GetCapteurPonctuel(size_t i)
{
    return (PonctualSensor*)m_pGestionCapteursPonctuels->m_LstCapteurs[i];
}

SensorsManager* SensorsManagers::GetGestionCapteursPonctuels()
{
    return m_pGestionCapteursPonctuels;
}

std::deque<EdieSensor*> SensorsManagers::GetCapteursEdie()
{
    std::deque<EdieSensor*> result;
    for(size_t i = 0; i < m_pGestionCapteursMFD->m_LstCapteurs.size(); i++)
    {
        MFDSensor * pSensor = (MFDSensor*)m_pGestionCapteursMFD->m_LstCapteurs[i];
        if(pSensor->IsEdie())
        {
            result.push_back(pSensor->GetLstSensors().front());
        }
    }

    return result;
}

std::vector<AbstractSensor*> & SensorsManagers::GetCapteursLongitudinaux()
{
    return m_pGestionCapteursLongitudinaux->m_LstCapteurs;
}

SensorsManager* SensorsManagers::GetGestionCapteursLongitudinaux()
{
    return m_pGestionCapteursLongitudinaux;
}

SensorsManager* SensorsManagers::GetGestionCapteursBT()
{
    return m_pGestionCapteursBT;
}

SensorsManager* SensorsManagers::GetGestionCapteursEdie()
{
	return m_pGestionCapteursEdie;
}

SensorsManager* SensorsManagers::GetGestionCapteursMFD()
{
    return m_pGestionCapteursMFD;
}

SensorsManager* SensorsManagers::GetGestionCapteurGlobal()
{
    return m_pGestionCapteurGlobal;
}

std::vector<AbstractSensor*> SensorsManagers::GetAllSensors()
{
    std::vector<AbstractSensor*> result;
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        result.insert(result.end(), m_GestionsCapteurs[i]->m_LstCapteurs.begin(), m_GestionsCapteurs[i]->m_LstCapteurs.end());
    }
    return result;
}

std::vector<int> SensorsManagers::GetPeriodes()
{
    std::vector<int> result;
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        result.push_back(m_GestionsCapteurs[i]->GetPeriode());
    }
    return result;
}

void SensorsManagers::SetPeriodes(const std::vector<int> & periods)
{
    for(size_t i = 0; i < m_GestionsCapteurs.size(); i++)
    {
        m_GestionsCapteurs[i]->SetPeriode(periods[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template void SensorsManager::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void SensorsManager::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void SensorsManager::serialize(Archive& ar, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(m_pReseau);
	ar & BOOST_SERIALIZATION_NVP(m_dbPeriodeAgregation);
    ar & BOOST_SERIALIZATION_NVP(m_dbT0);
    ar & BOOST_SERIALIZATION_NVP(m_nPeriode);
    ar & BOOST_SERIALIZATION_NVP(m_nNbInst);
	ar & BOOST_SERIALIZATION_NVP(m_LstCapteurs);
}

template void SensorsManagers::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void SensorsManagers::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void SensorsManagers::serialize(Archive& ar, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursPonctuels);
    ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursEdie);
	ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursMFD);
    ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursLongitudinaux);
	ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursBT);
    ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteurGlobal);
    ar & BOOST_SERIALIZATION_NVP(m_pGestionCapteursReservoir);

    ar & BOOST_SERIALIZATION_NVP(m_GestionsCapteurs);
}
