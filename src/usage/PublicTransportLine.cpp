#include "stdafx.h"
#include "PublicTransportLine.h"

#include "TripLeg.h"
#include "arret.h"
#include "reseau.h"

#include "usage/PublicTransportFleet.h"
#include "usage/Schedule.h"

#include <boost/serialization/map.hpp>

PublicTransportLineSnapshot::PublicTransportLineSnapshot()
{

}

PublicTransportLineSnapshot::~PublicTransportLineSnapshot()
{

}

PublicTransportLine::PublicTransportLine() : Trip()
{
}

PublicTransportLine::~PublicTransportLine()
{
}

void PublicTransportLine::ComputeCosts()
{
    m_LatestDrivingTime.clear();
    m_LatestStopDuration.clear();
    m_LatestFrequencyAtStop.clear();

    // Pour chaque arr�t :
    TripNode * pPreviousStop = this->GetOrigin();
    TripNode * pNextStop;
    for (size_t i = 0; i < this->GetLegs().size(); i++)
    {
        TripLeg * pTripLeg = this->GetLegs()[i];
        pNextStop = pTripLeg->GetDestination();
        // rmq. on peut ne pas avoir des arr�ts ici aux bouts des lignes (extremit�s ponctuelles si la ligne sort du r�seau)
        Arret * pUpstreamStop = dynamic_cast<Arret*>(pPreviousStop);
        Arret * pDownstreamStop = dynamic_cast<Arret*>(pNextStop);

        if (pUpstreamStop && pDownstreamStop)
        {
            // Calcul de l'intertemps entre deux arr�ts :
            const std::map<Trip*, std::map<int, std::pair<double, double> > > & passingTimes = pDownstreamStop->GetPassingTimes();
            const std::map<Trip*, std::map<int, std::pair<double, double> > > & upstreamPassingTimes = pUpstreamStop->GetPassingTimes();
            std::map<Trip*, std::map<int, std::pair<double, double> > >::const_iterator iterTimesForLine = passingTimes.find(this);
            if (iterTimesForLine != passingTimes.end())
            {
                std::map<Trip*, std::map<int, std::pair<double, double> > >::const_iterator iterUpstreamTimesForLine = upstreamPassingTimes.find(this);
                if (iterUpstreamTimesForLine != upstreamPassingTimes.end())
                {
                    // on commence par les instants d'arriv�e les plus r�cents :
                    std::map<double, int> dbMinTimeAndID;
                    for (std::map<int, std::pair<double, double> >::const_iterator iterVeh = iterTimesForLine->second.begin(); iterVeh != iterTimesForLine->second.end(); ++iterVeh)
                    {
                        assert(iterVeh->second.first != -1);
                        dbMinTimeAndID[iterVeh->second.first] = iterVeh->first;
                    }
                    // On regarde l'instant de d�part de l'arr�t pr�c�dent pour le v�hicule le plus r�cent
                    std::map<double, int>::const_reverse_iterator iterLatestVeh = dbMinTimeAndID.rbegin();
                    if (iterLatestVeh != dbMinTimeAndID.rend())
                    {
                        std::map<int, std::pair<double, double> >::const_iterator iterVehUpstream = iterUpstreamTimesForLine->second.find(iterLatestVeh->second);
                        if (iterVehUpstream != iterUpstreamTimesForLine->second.end())
                        {
                            if (iterVehUpstream->second.second != -1)
                            {
                                m_LatestDrivingTime[std::make_pair(pUpstreamStop, pDownstreamStop)] = iterLatestVeh->first - iterVehUpstream->second.second;
                            }
                            else
                            {
                                // a priori impossible : cas d'un bus arriv� � un arr�t sans �tre parti de l'arr�t pr�c�dent...
                                assert(false);
                            }
                        }
                    }
                }
            }
        }

        // Calcul du temps d'arr�t (pas la peine pour le dernier arr�t : le v�hicule est d�truit en principe, de plus
        // on ne devrait jamais exploiter cette valeur), du coup on s'�pargne de r�fl�chir aux -1 : si -1, c'est que l'instant n'est pas encore arriv�.
        if (pUpstreamStop)
        {
            const std::map<Trip*, std::map<int, std::pair<double, double> > > & passingTimes = pUpstreamStop->GetPassingTimes();
            std::map<Trip*, std::map<int, std::pair<double, double> > >::const_iterator iterTimesForLine = passingTimes.find(this);
            if (iterTimesForLine != passingTimes.end())
            {
                std::map<double, std::vector<int> > dbMinTimeAndID;
                for (std::map<int, std::pair<double, double> >::const_iterator iterVeh = iterTimesForLine->second.begin(); iterVeh != iterTimesForLine->second.end(); ++iterVeh)
                {
                    // on ingore les v�hicules non encore repartis de l'arr�t...
                    if (iterVeh->second.second != -1)
                    {
                        dbMinTimeAndID[iterVeh->second.second].push_back(iterVeh->first);
                    }
                }
                // On regarde l'instant de d�part de l'arr�t pr�c�dent pour le v�hicule le plus r�cent
                std::map<double, std::vector<int> >::const_reverse_iterator iterLatestVeh = dbMinTimeAndID.rbegin();
                if (iterLatestVeh != dbMinTimeAndID.rend())
                {
                    m_LatestStopDuration[pUpstreamStop] = iterLatestVeh->first - iterTimesForLine->second.at(iterLatestVeh->second.back()).first;
                }
            }
        }

        // Calcul de la fr�quence observ�e (pas la peine de faire le dernier, ne devrait pas �tre utilis� car pas d'attente � cet arr�t pour cette ligne)
        if (pUpstreamStop)
        {
            const std::map<Trip*, std::map<int, std::pair<double, double> > > & passingTimes = pUpstreamStop->GetPassingTimes();
            std::map<Trip*, std::map<int, std::pair<double, double> > >::const_iterator iterTimesForLine = passingTimes.find(this);
            if (iterTimesForLine != passingTimes.end())
            {
                std::map<double, std::vector<int> > dbMinTimeAndID;
                int nbVehicles = 0;
                for (std::map<int, std::pair<double, double> >::const_iterator iterVeh = iterTimesForLine->second.begin(); iterVeh != iterTimesForLine->second.end(); ++iterVeh)
                {
                    dbMinTimeAndID[iterVeh->second.first].push_back(iterVeh->first);
                    nbVehicles++;
                }
                if (nbVehicles >= 2)
                {
                    if (dbMinTimeAndID.rbegin()->second.size() == 1)
                    {
                        m_LatestFrequencyAtStop[pUpstreamStop] = dbMinTimeAndID.rbegin()->first - (++dbMinTimeAndID.rbegin())->first;
                    }
                    else
                    {
                        m_LatestFrequencyAtStop[pUpstreamStop] = 0; // Deux bus arrivent en m�me temps : fr�quence de 0s !
                    }
                }
            }
        }

        pPreviousStop = pNextStop;
    }
}

bool PublicTransportLine::GetLastDrivingTimeBewteenStops(Arret * pUpstreamStop, Arret * pDownstreamStop, double & dbLastDrivingTime)
{
    std::map<std::pair<Arret*, Arret*>, double>::const_iterator iter = m_LatestDrivingTime.find(std::make_pair(pUpstreamStop, pDownstreamStop));
    if (iter != m_LatestDrivingTime.end())
    {
        dbLastDrivingTime = iter->second;
        return true;
    }
    return false;
}

double PublicTransportLine::GetLastStopDuration(Arret * pStop)
{
    std::map<Arret*, double>::const_iterator iter = m_LatestStopDuration.find(pStop);
    if (iter != m_LatestStopDuration.end())
    {
        return iter->second;
    }
    else
    {
        // rmq. : on pourrait imaginer aller voir les dur�es d'arr�t d�finie pour chaque d�part de bus, 
        // mais ca ne semble pas �vident (il faudrait estimer quel d�part de bus correspond � l'instant d'arriv�e courant...)
        return pStop->getTempsArret();
    }
}

double PublicTransportLine::GetLastFrequency(Arret * pStop)
{
    double dbFrequency;
    std::map<Arret*, double>::const_iterator iter = m_LatestFrequencyAtStop.find(pStop);
    if (iter != m_LatestFrequencyAtStop.end())
    {
        // Fr�quence mesur�e
        dbFrequency = iter->second;
    }
    else
    {
        // fr�quence th�orique
        Schedule * pLineSchedule = pStop->GetNetwork()->GetPublicTransportFleet()->GetSchedule(this);
        dbFrequency = pLineSchedule->GetInstantaneousFrequency(pStop->GetNetwork()->m_dbInstSimu);
    }
    return dbFrequency;
}

TripSnapshot * PublicTransportLine::TakeSnapshot()
{
    PublicTransportLineSnapshot * pResult = new PublicTransportLineSnapshot();
    pResult->latestDrivingTime = m_LatestDrivingTime;
    pResult->latestStopDuration = m_LatestStopDuration;
    pResult->latestFrequencyAtStop = m_LatestFrequencyAtStop;
    return pResult;
}

void PublicTransportLine::Restore(Reseau * pNetwork, TripSnapshot * backup)
{
    PublicTransportLineSnapshot * pBackup = (PublicTransportLineSnapshot*)backup;
    m_LatestDrivingTime = pBackup->latestDrivingTime;
    m_LatestStopDuration = pBackup->latestStopDuration;
    m_LatestFrequencyAtStop = pBackup->latestFrequencyAtStop;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void PublicTransportLineSnapshot::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void PublicTransportLineSnapshot::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void PublicTransportLineSnapshot::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TripSnapshot);

    ar & BOOST_SERIALIZATION_NVP(latestDrivingTime);
    ar & BOOST_SERIALIZATION_NVP(latestStopDuration);
    ar & BOOST_SERIALIZATION_NVP(latestFrequencyAtStop);
}

template void PublicTransportLine::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void PublicTransportLine::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void PublicTransportLine::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Trip);

    ar & BOOST_SERIALIZATION_NVP(m_LatestDrivingTime);
    ar & BOOST_SERIALIZATION_NVP(m_LatestStopDuration);
    ar & BOOST_SERIALIZATION_NVP(m_LatestFrequencyAtStop);
}

