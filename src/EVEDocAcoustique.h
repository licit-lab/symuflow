#pragma once

#include "DocAcoustique.h"
#include "TrafficState.h"

class EVEDocAcoustique :
	public DocAcoustique
{
public:
	EVEDocAcoustique(void);
	virtual ~EVEDocAcoustique(void);

private:
	eveShared::TrafficState * m_pTrafState;

public:

	// Obtention des informations du trafic
	eveShared::TrafficState * GetTrafficState();

	// Fin
	virtual void Terminate();

    // Ajout d'un instant
	virtual PINSTANT	AddInstant(double dbInstant, int nNbVeh);

    // Sauvegarde des donn�es acoustiques d'un instant
	virtual void SaveLastInstant();

    // Ajout de la description d'une cellule acoustique
	virtual void AddCellule(int nID, const std::string & strLibelle, const std::string & strTuyau, double dbXam, double dbYam, double dbZam, double dbXav,  double dbYav, double dbZav);

    // Ajout des donn�es acoustique d'une source ponctuelle � l'instant consid�r�
	virtual void AddSourcePonctuelle(int nID, double dbAbsDeb, double dbOrdDeb, double dbHautDeb, double dbAbsFin, double dbOrdFin, double dbHautFin, double * arEmissions);

    // Ajout des donn�es acoustiques d'une cellule � l'instant consid�r�
	virtual void AddCellSimu(int nID, double * arEmissions ) ;

    // Ajout des donn�es acoustiques d'une cellule � l'instant consid�r�
	virtual void AddCellSimuEx(int nID, double * arEmissions, const std::string & strTuyau, double dbXam, double dbYam, double dbZam, double dbXav,  double dbYav, double dbZav );

};
