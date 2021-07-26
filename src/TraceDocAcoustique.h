#pragma once
#ifndef _TRACEDOCACOUSTIQUE_H
#define _TRACEDOCACOUSTIQUE_H

#include "XMLDocAcoustique.h"

class EVEDocAcoustique;
namespace eveShared {
    class TrafficState;
};

class TraceDocAcoustique : public XMLDocAcoustique
{
public:
	TraceDocAcoustique(Reseau * pNetwork, XERCES_CPP_NAMESPACE::DOMDocument * pXMLDocument);

	virtual ~TraceDocAcoustique(void);

private:
	EVEDocAcoustique * m_pDocEveDocAcoustique;

public:
	// Obtention des informations du trafic
	eveShared::TrafficState * GetTrafficState();

    // Ajout d'un instant
	virtual PINSTANT	AddInstant(double dbInstant, int nNbVeh);

    // Ajout des donn�es acoustique d'une source ponctuelle � l'instant consid�r�
	virtual void AddSourcePonctuelle(int nID, double dbAbsDeb, double dbOrdDeb, double dbHautDeb, double dbAbsFin, double dbOrdFin, double dbHautFin, double * arEmissions);

    // Ajout des donn�es acoustiques d'une cellule � l'instant consid�r�
	virtual void AddCellSimuEx(int nID, double * arEmissions, const std::string & strTuyau, double dbXam, double dbYam, double dbZam, double dbXav,  double dbYav, double dbZav );

};
#endif
