#pragma once
#ifndef XMLDocItineraireH
#define XMLDocItineraireH

#include <xercesc/util/XercesVersion.hpp>

namespace XERCES_CPP_NAMESPACE {
    class DOMNode;
    class DOMDocument;
};

namespace boost {
    namespace serialization {
        class access;
    }
}

class Reseau;

/*===========================================================================================*/
/* Classe de mod�lisation du document XML des itin�raires   								 */
/*===========================================================================================*/
class XMLDocItineraire 
{
public:	
	// Le document XML
	XERCES_CPP_NAMESPACE::DOMDocument * pXMLDoc;

    // Constructeur par d�faut
	XMLDocItineraire();

    // Constructeur
	XMLDocItineraire(Reseau * pNetwork, XERCES_CPP_NAMESPACE::DOMDocument* pXMLDocument);

	~XMLDocItineraire();

private:
	XERCES_CPP_NAMESPACE::DOMNode			*m_XmlNodeOrigines;

    Reseau * m_pNetwork;

public:
	XERCES_CPP_NAMESPACE::DOMNode*        GetXmlNodeOrigines()
	{
		return m_XmlNodeOrigines;
	};

    // Initialisation
	void Create();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif