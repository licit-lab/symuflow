#pragma once

#ifndef SYMUVIAPUBLICTRANSPORTPATTERN_H
#define SYMUVIAPUBLICTRANSPORTPATTERN_H

#include <SymuCore/Graph/Model/PublicTransport/PublicTransportPattern.h>

class Reseau;
class Tuyau;
class Arret;
class Trip;
class TypeVehicule;
class PublicTransportLine;

class SymuViaPublicTransportPattern : public SymuCore::PublicTransportPattern
{
public:

    SymuViaPublicTransportPattern(SymuCore::Link* pLink, SymuCore::PublicTransportLine* pLine, Reseau * pNetwork, Trip * pSymuViaLine,
                                  const std::vector<Tuyau*> &LstTuyau, Arret * pUpstreamStop, Arret * pDownstreamStop);

    virtual ~SymuViaPublicTransportPattern();

    PublicTransportLine * GetSymuViaLine();
    const std::vector<Tuyau*> & GetLstTuyau() const;
    Arret* GetUpstreamStop() const;
    Arret* GetDownstreamStop() const;

    virtual void fillMeasuredCostsForTravelTimesUpdatePeriod(int iTravelTimesUpdatePeriodIndex, const std::vector<SymuCore::SubPopulation *> &listSubPopulations, const std::map<SymuCore::SubPopulation*, SymuCore::CostFunction> & mapCostFunctions);

protected:

    Reseau *                    m_pNetwork;
    PublicTransportLine*        m_pSymuViaBusLine;
    std::vector<Tuyau*>         m_LstTuyau; //liste des tuyaux composant le pattern
    Arret*                      m_pUpstreamStop; //arr�t amont
    Arret*                      m_pDownstreamStop; //arr�t aval

    TypeVehicule*               m_pTypeVehicleForLine; // TYpe de v�hicule associ� � la ligne de transport public
};

#endif // SYMUVIAPUBLICTRANSPORTPATTERN_H


