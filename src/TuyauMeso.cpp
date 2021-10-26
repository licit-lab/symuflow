#include "stdafx.h"
#include "TuyauMeso.h"
#include "tuyau.h"
#include "vehicule.h"
#include "voie.h"
#include "reseau.h"
#include "ConnectionPonctuel.h"
#include "Logger.h"
#include "DiagFonda.h"
#include "sensors/SensorsManager.h"

#include <algorithm>

#include <boost/serialization/deque.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>

CTuyauMeso::CTuyauMeso()
    :
	TuyauMicro()
{
    m_dSupplyTime = -DBL_MAX;

    m_dCurrentMeanW = 0;
    m_dCurrentMeanK = 0;
}

CTuyauMeso::CTuyauMeso( Reseau *pReseau, std::string _Nom,char _Type_amont ,char _Type_aval,ConnectionPonctuel *_p_aval,ConnectionPonctuel *_p_amont,
    char typevoie, std::string nom_rev, std::vector<double> larg_voie, double _Abscisse_amont,double _Ordonnee_amont,
    double _Abscisse_aval,double _Ordonnee_aval, double _Hauteur_amont, double _Hauteur_aval, int _Nb_voies,
    double pastemps, double dbVitReg, double dbVitCat, const std::string & strRoadLabel, ETuyauType resolution)
    :
	TuyauMicro( pReseau, _Nom, _Type_amont ,_Type_aval,_p_aval,_p_amont, typevoie, nom_rev, larg_voie, _Abscisse_amont,_Ordonnee_amont,
        _Abscisse_aval,_Ordonnee_aval, _Hauteur_amont, _Hauteur_aval, _Nb_voies,
        pastemps, dbVitReg, dbVitCat, strRoadLabel)
{
    m_dSupplyTime = -DBL_MAX;

    m_dCurrentMeanW = 0;
    m_dCurrentMeanK = 0;

    m_nextArrival = std::pair<double, Vehicule*>(DBL_MAX, nullptr);

    // Si le tuyau est cr�� en tant que tuyau meso, on le place dans la bonne r�solution (plut�t que la r�solution Micro positionn�e
    // dans le constructeur de la classe m�re.
    if(resolution == TT_MESO)
    {
        m_nResolution = RESOTUYAU_MESO;
    }
}


CTuyauMeso::~CTuyauMeso(void)
{
}

Tuyau::ETuyauType CTuyauMeso::GetType() const
{
    if(m_nResolution == RESOTUYAU_MESO)
    {
        return TT_MESO;
    }
    else
    {
        return TT_MICRO;
    }
}
    
void CTuyauMeso::ComputeMeanW()
{
    // nb_veh / sum(1/(des w d veh tdu troncon)) 
    double dSumW = 0;
    size_t i;
    for( i = 0; i< m_currentVehicules.size(); ++i)
    {
       dSumW += 1.0/ m_currentVehicules[i]->GetType()->GetW();
    }

    if( dSumW != 0 )
    {
        m_dCurrentMeanW = m_currentVehicules.size() / dSumW;
    }
    else
    {
        m_dCurrentMeanW = 0;
    }
}

void CTuyauMeso::ComputeMeanK()
{
    
    double dSumK = 0;
    for( size_t i = 0; i< m_currentVehicules.size(); ++i)
    {
        dSumK += m_currentVehicules[i]->GetType()->GetKx();
    }
    if( dSumK !=0 )
    {
        m_dCurrentMeanK = dSumK / m_currentVehicules.size();
    }
    else
    {
        m_dCurrentMeanK = 0;
    }
}


void CTuyauMeso::AddCurrentVehicule( Vehicule * pVehicule)
{ 
    m_currentVehicules.push_back(pVehicule);
    pVehicule->SetPos(0.0); // init position pour l'�criture
}



std::list< std::pair<double, Vehicule *> > CTuyauMeso::GetArrivalTimes() const
{
    return m_arrivals;
}

double CTuyauMeso::GetMaxArrivalTime() const
{
    if( m_arrivals.size()>0)
    {
        return m_arrivals.back().first;
    }
    else
    {
        return -1;
    }
}


void CTuyauMeso::AddArrival(double dArrivalTime, Vehicule *pVehicule, bool bEnsureIsLastArrival)
{
    double dRealArrivalTime = dArrivalTime;
    if (bEnsureIsLastArrival)
    {
        dRealArrivalTime = std::max<double>(dRealArrivalTime, GetMaxArrivalTime());
    }
    // rmq. on note ici le temps pr�vu pour la sortie (sera �cras� lors de la sortie r�elle),
    // utilis� pour la conversion du troncon meso en troncon micro pour positionner le v�hicule dessus
    pVehicule->SetTimeCode(this, Vehicule::TC_OUTGOING, dRealArrivalTime);

    m_arrivals.push_back(std::pair<double, Vehicule*>(dRealArrivalTime, pVehicule));

	m_arrivals.sort(LessPairPtrIDRef<std::pair<double, Vehicule*> >());
}


std::pair<double,Vehicule* >   CTuyauMeso::GetMinArrival() const
{
    if( m_arrivals.size()>0)
    {
        return m_arrivals.front();
    }
    else
    {
        return std::pair<double,Vehicule* >();
    }
}

 Vehicule* CTuyauMeso::GetCurrentVehiculeAt(int iRank) const
 {
     if( iRank>=0 && iRank< (int)m_arrivals.size())
     {
         std::list< std::pair< double, Vehicule*> >::const_iterator it= m_arrivals.begin();
         while( iRank>0)
         {
            it++;
            iRank--;
         }
         return it->second;
     }
     return NULL;
 }

double  CTuyauMeso::GetDownstreamPassingTime(int iRank) const
{
   if( iRank>=0 && iRank< (int)m_downstreamPassingTimes.size() )
   {
       return m_downstreamPassingTimes.at(iRank);
   }
    return 0;
}
void CTuyauMeso::RemoveFirstDownStreamPassingTime( )
{
    if( m_downstreamPassingTimes.size() > 0)
    {
        m_downstreamPassingTimes.pop_front( );
    }

   
}

void CTuyauMeso::RemoveFirstUpstreamPassingTime( )
{
    
    if( m_upstreamPassingTimes.size() > 0)
    {
         m_upstreamPassingTimes.pop_front( );
    }

     
}


void CTuyauMeso::AddUpstreamPassingTime( double dInstant  )
{

    m_upstreamPassingTimes.push_back( dInstant );

}
void CTuyauMeso::AddDownstreamPassingTime(  double dInstant  )
{

   m_downstreamPassingTimes.push_back( dInstant );

}
std::deque<double> CTuyauMeso::GetUpstreamPassingTimes(  ) const
{
   if( m_upstreamPassingTimes.size()>0)
   {
     return m_upstreamPassingTimes;
   }
   return std::deque<double>();
    
}


void CTuyauMeso::ClearArrivals()
{
    m_arrivals.clear();
}

void CTuyauMeso::ReplaceArrivalTimes(std::list< std::pair<double, Vehicule *>  >newList )
{

    m_arrivals.clear();
    m_arrivals.assign(newList.begin(), newList.end()) ;
	m_arrivals.sort(LessPairPtrIDRef<std::pair<double, Vehicule*> >());


}

void CTuyauMeso::SetNextArrival( std::pair<double, Vehicule*> arrival)
{
    m_nextArrival = arrival;
}

// Obtient le prochain vehicule entrant sur le noeud
Vehicule * CTuyauMeso::GetNextIncommingVehicle() const
{
    if(m_arrivals.size()>0)
    {
        return m_arrivals.begin()->second;
    }
    else
    {
        return NULL;
    }
}

void CTuyauMeso::CalculTempsDeParcours( double dbInst, TypeVehicule *pTypeVeh, 
    char cTypeEvent, double dbPeriode, char cTypeCalcul )
{
    if (IsMeso())
    {
        double dbTps;
        double dbTpsCout;

        dbTpsCout = ComputeCost(dbInst, pTypeVeh, false);
        double d = -DBL_MAX;

        if (this->GetNextSupplyTime() != (-DBL_MAX))
        {
            if (this->GetNextSupplyTime() == DBL_MAX)
            {
                dbTpsCout = DBL_MAX;
            }
            else
            {
                dbTpsCout = std::max<double>(dbTpsCout, this->GetNextSupplyTime() - this->GetMinArrival().first);

            }
        }

        dbTps = dbTpsCout;

        m_mapTempsParcours[pTypeVeh] = dbTps;
        m_mapTempsParcoursRealise[pTypeVeh] = dbTps;
    }
    else
    {
        return TuyauMicro::CalculTempsDeParcours(dbInst, pTypeVeh, cTypeEvent, dbPeriode, cTypeCalcul);
    }
}

bool CTuyauMeso::IsCongested( ) const
{
    bool bReturn =  m_currentVehicules.size();
    bReturn  = m_currentVehicules.size()!=0 &&   (m_currentVehicules.size()-  GetLength() * getNb_voies() * GetMeanK() )>1;
  
    return  bReturn;
    
}

void CTuyauMeso::RemoveCurrentVehicule(Vehicule * pVehicule) 
{
    std::deque<Vehicule*>::iterator it = m_currentVehicules.begin();
    bool bFind =false;
    while( !bFind
        && it != m_currentVehicules.end())
    {
        if( *it ==  pVehicule)
        {
            m_currentVehicules.erase(it);
            bFind = true;
        }
        else
        {
            it++;
        }
    }
}

void CTuyauMeso::ToMicro()
{
    m_nResolution = RESOTUYAU_MICRO;

    // map des positions du dernier v�hicule replac� sur chaque voie afin de respecter un espacement suffisant entre
    // ceux-ci (d�pendant de kmean)
    std::map<int, double> lastPosOnLane;

    std::list< std::pair< double, Vehicule*> >::const_iterator it;
    for(it = m_arrivals.begin(); it != m_arrivals.end(); ++it)
    {
        const std::pair<double, Vehicule*> & vehicle = *it;

        double dbIncomingTime = vehicle.second->GetTimeCode(this, Vehicule::TC_INCOMMING);
        double dbArrivalTime = vehicle.first;//vehicle.second->GetTimeCode(this, Vehicule::TC_OUTGOING); // rmq. : le TC_OUTGOING n'est pas r�actualis� en fonction des feux rouges, etc...

        // Position calcul�e au prorata de la dur�e �coul�e par rapport � la dur�e de travers�e pr�vue du tuyau
        double dbPosInLink;
        if(dbArrivalTime != dbIncomingTime)
        {
            // rmq. : il peut arriver que m_pReseau->m_dbInstSimu soit sup�rieur � l'instant d'arriv�e des v�hicules. Dans ce cas,
            // dbPosInLink est sup�rieur � la longueur de tron�on, et si c'est le cas de plusieurs v�hicules (reproductible facilement avec un feu rouge par exemple),
            // ces v�hicules sont tous cr��s � la m�me position (� la fin du tron�on). A am�liorer si g�nant.
            dbPosInLink = (m_pReseau->m_dbInstSimu - dbIncomingTime) / (dbArrivalTime - dbIncomingTime) * GetLength();
        }
        else
        {
            // ne devrait jamais arriver ?
            dbPosInLink = 0;

            m_pReseau->log() << Logger::Warning << std::endl << "WARNING : the arrival time of vehicle " << vehicle.second->GetID() << " is equal to its incoming time ..." << std::endl;
            m_pReseau->log() << Logger::Info; // rebascule en mode INFO pour ne pas avoir � reprendre tous les appels aux log en pr�cisant que c'est des INFO. � supprimer si on reprend tous les appels au log.
        }

        dbPosInLink = std::max<double>(0, std::min<double>(GetLength() - 0.00001, dbPosInLink));

        double dbAvailablePos;
        int iLane = m_pReseau->ComputeAvailableLaneNumber(vehicle.second, this, NULL, dbPosInLink, &lastPosOnLane, dbAvailablePos);
        dbPosInLink = dbAvailablePos;

        lastPosOnLane[iLane] = dbPosInLink;

        vehicle.second->SetVoie((VoieMicro*)this->GetVoie(iLane));
        vehicle.second->SetTuyau(this, m_pReseau->m_dbInstSimu);
        vehicle.second->SetPos(dbPosInLink);

        if (m_pReseau->m_dbInstSimu != dbIncomingTime)
        {
            vehicle.second->SetVit(dbPosInLink / (m_pReseau->m_dbInstSimu - dbIncomingTime));
        }
        else
        {
            // division par zero si on applique la formule ci-dessus.. on prend la vitesse max du v�hicule...
            vehicle.second->SetVit(vehicle.second->GetVitMax());
        }
        
        vehicle.second->SetTuyauMeso(NULL, m_pReseau->m_dbInstSimu);

        // gestion des capteurs pour la partie meso du tron�on d�j� parcourue
        if(m_pReseau->GetGestionsCapteur())
        {
            m_pReseau->GetGestionsCapteur()->AddMesoVehicle(m_pReseau->m_dbInstSimu, vehicle.second, this, NULL, dbPosInLink);
        }
    }

    m_currentVehicules.clear();
    m_arrivals.clear();
    m_nextArrival = std::pair<double, Vehicule*>( DBL_MAX, nullptr);
    m_upstreamPassingTimes.clear();
    m_downstreamPassingTimes.clear();
    m_dSupplyTime = -DBL_MAX;

    assert(!GetBriqueParente());

    GetCnxAssAv()->ToMicro(this, m_pReseau);
    GetCnxAssAm()->ToMicro(this, m_pReseau);

    if(GetCnxAssAm()->IsAnOrigine())
    {
        GetCnxAssAm()->ToMicro(NULL, m_pReseau);
    }
}

void CTuyauMeso::ToMeso()
{
    m_nResolution = RESOTUYAU_MESO;

    // rmq. : On ne fait rien ici pour les v�hicules micro qui sont sut le tron�on. Il y a donc une approximation ici
    // puisque ces v�hicules ne seront pas pris en compte par le mod�le m�so de ce tuyau alors qu'ils vont y passer encore
    // un peu de temps...

    GetCnxAssAv()->ToMeso(this);
    GetCnxAssAm()->ToMeso(this);

    if (GetCnxAssAm()->IsAnOrigine())
    {
        GetCnxAssAm()->ToMeso(NULL);
    }

    // m�me si le tuyau est micro, on peut mettre des choses l�-dedans (voir par ex. le adddownstreampassingtime),
    // du coup on nettoie lorsqu'on repasse en meso, sinon on observe des blocages (
    m_upstreamPassingTimes.clear();
    m_downstreamPassingTimes.clear();
}

void CTuyauMeso::SortieTrafic(DocTrafic *pXMLDocTrafic)
{
    if (m_pReseau->DoSortieTraj())
    {
        // Calcul de la concentration (calcul issu du calcul de concentration de SymuPlayer)
        double dbConcentration = 0.0;
        double dbLength = 0.0;
        for(int iVoie = 0; iVoie < this->m_nNbVoies; iVoie++) {
		    dbLength += m_pLstVoie[iVoie]->GetLength();   
	    }
		if(dbLength > 0) {
			dbConcentration = m_currentVehicules.size() / dbLength;
		}

        pXMLDocTrafic->AddLink(GetLabel(), dbConcentration);
    }
}

template void CTuyauMeso::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void CTuyauMeso::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void CTuyauMeso::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TuyauMicro);

    ar & BOOST_SERIALIZATION_NVP(m_currentVehicules);
    ar & BOOST_SERIALIZATION_NVP(m_dCurrentMeanW);
    ar & BOOST_SERIALIZATION_NVP(m_dCurrentMeanK);
    ar & BOOST_SERIALIZATION_NVP(m_arrivals);
    ar & BOOST_SERIALIZATION_NVP(m_nextArrival);
    ar & BOOST_SERIALIZATION_NVP(m_upstreamPassingTimes);
    ar & BOOST_SERIALIZATION_NVP(m_downstreamPassingTimes);
    ar & BOOST_SERIALIZATION_NVP(m_dSupplyTime);
}
