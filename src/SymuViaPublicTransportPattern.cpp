#include "stdafx.h"
#include "SymuViaPublicTransportPattern.h"

#include "reseau.h"
#include "tuyau.h"
#include "arret.h"
#include "ConnectionPonctuel.h"
#include "usage/PublicTransportFleet.h"
#include "usage/PublicTransportLine.h"

#include <SymuCore/Graph/Model/Link.h>
#include <SymuCore/Graph/Model/Node.h>
#include <SymuCore/Graph/Model/Graph.h>
#include <SymuCore/Graph/Model/Cost.h>
#include <SymuCore/Graph/Model/PublicTransport/PublicTransportLine.h>
#include <SymuCore/Utils/Point.h>
#include <SymuCore/Demand/SubPopulation.h>
#include <SymuCore/Demand/Population.h>


using namespace SymuCore;

SymuViaPublicTransportPattern::SymuViaPublicTransportPattern(SymuCore::Link* pLink, SymuCore::PublicTransportLine *pLine, Reseau * pNetwork, ::Trip * pSymuViaLine,
    const std::vector<Tuyau*>& LstTuyau, Arret * pUpstreamStop, Arret * pDownstreamStop)
: PublicTransportPattern(pLink, SymuCore::PT_PublicTransport, pLine)
{
    m_pNetwork = pNetwork;
    m_pSymuViaBusLine = (::PublicTransportLine*)pSymuViaLine;
    m_LstTuyau = LstTuyau;
    m_pUpstreamStop = pUpstreamStop;
    m_pDownstreamStop = pDownstreamStop;

    m_pTypeVehicleForLine = m_pNetwork->GetPublicTransportFleet()->GetTypeVehicule(pSymuViaLine);
}

SymuViaPublicTransportPattern::~SymuViaPublicTransportPattern()
{

}

::PublicTransportLine * SymuViaPublicTransportPattern::GetSymuViaLine()
{
    return m_pSymuViaBusLine;
}

const std::vector<Tuyau*> & SymuViaPublicTransportPattern::GetLstTuyau() const
{
    return m_LstTuyau;
}

Arret* SymuViaPublicTransportPattern::GetUpstreamStop() const
{
    return m_pUpstreamStop;
}
Arret* SymuViaPublicTransportPattern::GetDownstreamStop() const
{
    return m_pDownstreamStop;
}

void SymuViaPublicTransportPattern::fillMeasuredCostsForTravelTimesUpdatePeriod(int iTravelTimesUpdatePeriodIndex, const std::vector<SubPopulation*>& listSubPopulations, const std::map<SymuCore::SubPopulation*, SymuCore::CostFunction> & mapCostFunctions)
{
    const TimeFrame<Cost> & costVariation = getTemporalCosts().front().getTimeFrame((size_t)iTravelTimesUpdatePeriodIndex);

    double dbTravelTime = 0;

    // Si on veut exploiter la mesure du dernier temps de parcours du dernier bus :
    bool bFound = false;
    if (m_pNetwork->IsSymuMasterMode() && m_pNetwork->UseLastBusIfAnyForTravelTimes() && !m_pNetwork->UseEmptyTravelTimesForBusLines())
    {
        bFound = m_pSymuViaBusLine->GetLastDrivingTimeBewteenStops(m_pUpstreamStop, m_pDownstreamStop, dbTravelTime);
    }
    
    if(!bFound)
    {
        SymuCore::MacroType * pMainMacroType = m_pNetwork->GetMacroTypeForVLs();

        //Pour chaque Tuyau
        for (size_t iTuyau = 0; iTuyau < m_LstTuyau.size(); iTuyau++)
        {
            TuyauMicro * pTuyau = (TuyauMicro*)m_LstTuyau[iTuyau];

            double dbTuyauTravelTime;

            // Calcul du temps de parcours � vide du tuyau
            double dbTuyauEmptyTravelTime = pTuyau->GetCoutAVide(m_pNetwork->m_dbInstSimu, m_pTypeVehicleForLine, false, iTuyau + 1 != m_LstTuyau.size());
            if (dbTuyauEmptyTravelTime >= DBL_MAX)
            {
                dbTuyauEmptyTravelTime = std::numeric_limits<double>::infinity(); // ne devrait pas arriver si le r�seau et les lignes sont bien d�finies, mais bon...
            }
            else if (m_pNetwork->GetMinSpeedForTravelTime() > 0)
            {
                dbTuyauEmptyTravelTime = std::min<double>(dbTuyauEmptyTravelTime, pTuyau->GetLength() / m_pNetwork->GetMinSpeedForTravelTime());
            }

            if (!m_pNetwork->UseEmptyTravelTimesForBusLines())
            {
                if (m_pNetwork->IsSymuMasterMode())
                {
                    dbTuyauTravelTime = pTuyau->GetTravelTime(pMainMacroType);
                    if (dbTuyauTravelTime == std::numeric_limits<double>::infinity())
                    {
                        // Si le tuyau est r�serv� aux bus, on utilise le travel time � vide pour les bus.
                        dbTuyauTravelTime = dbTuyauEmptyTravelTime;
                    }
                }
                else
                {
                    // Cas du SG : pas de macrotypes d�finis, on utilise les m�thodes classiques
                    dbTuyauTravelTime = pTuyau->ComputeCost(m_pNetwork->m_dbInstSimu, m_pTypeVehicleForLine, false);
                }
            }
            else
            {
                dbTuyauTravelTime = dbTuyauEmptyTravelTime;
            }

            dbTravelTime += dbTuyauTravelTime;

            // Gestion du premier et dernier tron�on :
            if (iTuyau == 0)
            {
                dbTravelTime -= dbTuyauTravelTime*(m_pUpstreamStop->GetOutputPosition().GetPosition() / pTuyau->GetLength());
            }
            else if (iTuyau + 1 == m_LstTuyau.size())
            {
                dbTravelTime -= dbTuyauTravelTime*(1.0 - m_pDownstreamStop->GetInputPosition().GetPosition() / pTuyau->GetLength());
            }

            // Prise en compte des p�nalit�s aux noeuds
            if (iTuyau + 1 != m_LstTuyau.size())
            {
                Connexion * pConnection = pTuyau->GetCnxAssAv();

                // Calcul du cout � vide pour la p�nalit�
                double dbEmptyPenalty = pConnection->ComputeEmptyCost(m_pTypeVehicleForLine, pTuyau, m_LstTuyau[iTuyau + 1]);
                if (dbEmptyPenalty >= DBL_MAX)
                {
                    dbEmptyPenalty = std::numeric_limits<double>::infinity(); // ne devrait pas arriver si le r�seau et les lignes sont bien d�finies, mais bon...
                }

                double dbPenalty;
                if (!m_pNetwork->UseEmptyTravelTimesForBusLines())
                {
                    if (m_pNetwork->IsSymuMasterMode())
                    {
                        dbPenalty = pConnection->ComputeCost(pMainMacroType, pTuyau, m_LstTuyau[iTuyau + 1]);
                        if (dbPenalty == std::numeric_limits<double>::infinity())
                        {
                            // Si le mouvement est r�serv� aux bus, on utilise le travel time � vide pour les bus :
                            dbPenalty = dbEmptyPenalty;
                        }
                    }
                    else
                    {
                        // Cas du SG : pas de macrotypes d�finis, on utilise les m�thodes classiques
                        dbPenalty = pConnection->ComputeCost(m_pTypeVehicleForLine, pTuyau, m_LstTuyau[iTuyau + 1]);
                    }
                }
                else
                {
                    dbPenalty = dbEmptyPenalty;
                }
                
                dbTravelTime += dbPenalty;
            }
        }
    }
    
    Cost & cost = *costVariation.getData();

    cost.setTravelTime(dbTravelTime);
    cost.setUsedCostValue(dbTravelTime);
    cost.setOtherCostValue(CF_TravelTime, dbTravelTime);
    cost.setOtherCostValue(CF_Marginals, dbTravelTime);
}

