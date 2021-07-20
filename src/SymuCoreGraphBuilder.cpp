#include "stdafx.h"
#include "SymuCoreGraphBuilder.h"

#include "reseau.h"

#include <SymuCore/Graph/Model/Node.h>
#include <SymuCore/Graph/Model/Link.h>
#include <SymuCore/Graph/Model/Pattern.h>
#include <SymuCore/Graph/Model/AbstractPenalty.h>
#include <SymuCore/Graph/Model/MultiLayersGraph.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace SymuCore;

SymuCoreGraphBuilder::SymuCoreGraphBuilder()
: m_pNetwork(NULL),
m_pGraph(NULL),
m_bForSymuMaster(true),
m_DrivingGraphBuilder(NULL, true),
m_PublicTransportGraphBuilder(NULL)
{
	m_DrivingGraphBuilder.SetParent(this);
	m_PublicTransportGraphBuilder.SetParent(this);
}

SymuCoreGraphBuilder::SymuCoreGraphBuilder(Reseau * pNetwork, bool bForSymuMaster)
: m_pNetwork(pNetwork),
m_pGraph(NULL),
m_bForSymuMaster(bForSymuMaster),
m_DrivingGraphBuilder(pNetwork, bForSymuMaster),
m_PublicTransportGraphBuilder(pNetwork)
{
	m_DrivingGraphBuilder.SetParent(this);
	m_PublicTransportGraphBuilder.SetParent(this);
}


SymuCoreGraphBuilder::~SymuCoreGraphBuilder()
{
}

const SymuCoreDrivingGraphBuilder & SymuCoreGraphBuilder::GetDrivingGraphBuilder() const
{
    return m_DrivingGraphBuilder;
}

const SymuCorePublicTransportGraphBuilder & SymuCoreGraphBuilder::GetPublicTransportGraphBuilder() const
{
    return m_PublicTransportGraphBuilder;
}

SymuCore::MultiLayersGraph * SymuCoreGraphBuilder::GetGraph() const
{
    return m_pGraph;
}

bool SymuCoreGraphBuilder::Build(SymuCore::MultiLayersGraph * pGraph, bool bIsPrimaryGraph)
{
    std::vector<SymuCore::Node*> lstOrigins, lstDestination;
    bool bOk = m_DrivingGraphBuilder.CreateDrivingLayer(pGraph, lstOrigins, lstDestination);

    if (bOk && (m_bForSymuMaster || m_pNetwork->IsWithPublicTransportGraph()))
    {
        bOk = m_PublicTransportGraphBuilder.CreatePublicTransportLayer(pGraph, &m_DrivingGraphBuilder);
    }

    // remarque : construction un peu asym�trique entre couche conduite et couche TEC : la couche Conduite
    // g�re les origines/destinations et construit les noeuds Zone/plaque alors qu'il s'agit de noeuds utilis�s aussi en mode TEC.
    // Pourrait �tre g�nant si on veut d�sactiver la couche driving, l'autre ne pourrait pas fonctionner sans...
    if (bOk && bIsPrimaryGraph)
    {
        for (size_t iOrigin = 0; iOrigin < lstOrigins.size(); iOrigin++)
        {
            pGraph->CreateLinkToOrigin(lstOrigins[iOrigin]);
            if (m_bForSymuMaster)
            {
                SymuCore::Node * pPublicTransportOrigin = m_PublicTransportGraphBuilder.GetOriginNodeFromNode(lstOrigins[iOrigin]);
                if (pPublicTransportOrigin)
                {
                    pGraph->CreateLinkToOrigin(pPublicTransportOrigin);
                }
            }
        }
        for (size_t iDestination = 0; iDestination < lstDestination.size(); iDestination++)
        {
            pGraph->CreateLinkToDestination(lstDestination[iDestination]);
            if (m_bForSymuMaster)
            {
                SymuCore::Node * pPublicTransportDestination = m_PublicTransportGraphBuilder.GetDestinationNodeFromNode(lstDestination[iDestination]);
                if (pPublicTransportDestination)
                {
                    pGraph->CreateLinkToDestination(pPublicTransportDestination);
                }
            }
        }
    }

    m_pGraph = pGraph;
    
    return bOk;
}






