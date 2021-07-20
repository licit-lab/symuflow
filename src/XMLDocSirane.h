#pragma once
#ifndef XMLDocSiraneH
#define XMLDocSiraneH

#include <xercesc/util/XercesVersion.hpp>

namespace XERCES_CPP_NAMESPACE {
    class DOMElement;
    class DOMDocument;
    class DOMNode;
};

namespace boost {
    namespace serialization {
        class access;
    }
}

#include <string>
#include <vector>

struct Point;
class DOMLSSerializerSymu;
class Reseau;

// Types de cellule (segment de tron�on ou brique)
typedef enum { CELL_BRIN = 0, CELL_NOEUD = 1} eTypeCelluleSirane;
typedef enum { UNDEFINED_PHASE = -1, ACCELERATION_PHASE = 1, STABILIZATION_PHASE = 2, DECELERATION_PHASE = 3} ePhaseCelluleSirane;



/*===========================================================================================*/
/* Classe de mod�lisation du document XML des donn�es n�cessaire � Sirane                    */
/*===========================================================================================*/
class XMLDocSirane
{
private:

	std::string				m_strFilename;
	DOMLSSerializerSymu		* m_XmlWriter;

    Reseau *                m_pNetwork;

	XERCES_CPP_NAMESPACE::DOMNode   *m_XmlNodeSimulation;

    // noeud de la p�riode courante en cours d'�criture
    XERCES_CPP_NAMESPACE::DOMElement    *m_XmlNodePeriode;

    // noeud de la cellule courante en cours d'�criture
    XERCES_CPP_NAMESPACE::DOMElement    *m_XmlNodeCell;

	// Le document XML
	XERCES_CPP_NAMESPACE::DOMDocument   *pXMLDoc;

    bool                    m_bReinitialized;       // Indique si le writer courant est issu d'un restore de snapshot

    // Compteur du nombre de cellules
    int                     m_nNbCells;

public:
    // Constructeurs
    XMLDocSirane();
    XMLDocSirane(Reseau * pNetwork, XERCES_CPP_NAMESPACE::DOMDocument * pXMLDocument);
	~XMLDocSirane();

    // Initialisation
	void Init(const std::string & strFilename, const std::vector<Point> & boundingRect);

    // Re-initialisation (apr�s une d�s�rialisation par exemple)
    virtual void ReInit(const std::string & result);

    // Fin
	void Terminate();

    // Mise en veille (prise snapshot pour reprise plus tard)
    virtual void Suspend();

    // Ajout de la description d'une cellule sirane
	virtual void AddCellule(const std::string & strLibelle, eTypeCelluleSirane type, const std::vector<Point*> & lstPts, ePhaseCelluleSirane iPhase);

    // Ajout d'une nouvelle p�riode d'agr�gation
    virtual void AddPeriod(double dbInstantDebut, double dbInstantFin);

    // Ajout des r�sultats d'une cellule sur la p�riode
    virtual void AddCellSimu(const std::string & strLibelle);

    // Ajout des r�sultats d'une cellule sur la p�riode pour un type de v�hicule
    virtual void AddCellSimuForType(const std::string & strType, const std::vector<double> & vitMins, const std::vector<double> & vitMaxs, const std::vector<double> & durees);

    // Fin de la sauvegarde de la p�riode d'agr�gation courante
    virtual void ClosePeriod();



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif