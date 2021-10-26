#pragma once
#ifndef ScheduleParametersH
#define ScheduleParametersH

#include <string>

#include <xercesc/util/XercesVersion.hpp>

namespace XERCES_CPP_NAMESPACE {
    class DOMNode;
}

namespace boost {
    namespace serialization {
        class access;
    }
}

class Logger;

class ScheduleParameters
{

public:
    ScheduleParameters();
    virtual ~ScheduleParameters();

    // Chargement de du jeu de param�tres � partir du noeud XML correspondant
    virtual bool Load(XERCES_CPP_NAMESPACE::DOMNode * pXMLNode, Logger & loadingLogger) = 0;

    void SetID(const std::string & strID);
    const std::string & GetID();

protected:

    // Identifiant du jeu de param�tre
    std::string m_strID;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // ScheduleParametersH
