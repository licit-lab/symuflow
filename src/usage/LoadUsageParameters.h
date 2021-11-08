#pragma once
#ifndef LoadUsageParametersH
#define LoadUsageParametersH

#include "usage/Passenger.h"

#include <xercesc/util/XercesVersion.hpp>

#include <vector>
#include <map>

namespace XERCES_CPP_NAMESPACE {
    class DOMNode;
}

namespace boost {
    namespace serialization {
        class access;
    }
}

class Logger;
class TripNode;

class LoadUsageParameters
{

public:
    LoadUsageParameters();
    virtual ~LoadUsageParameters();

    void    Load(XERCES_CPP_NAMESPACE::DOMNode * pNode, Logger & loadingLogger);
    
    bool    IsMonteeDescenteSimultanee();
	void    SetMonteeDescenteSimultanee(bool bMonteeDescenteSimultanee);

    double  GetTempsMonteeIndividuel();
	void    SetTempsMonteeIndividuel(double dbTempsMontee);

    double  GetTempsDescenteIndividuel();
	void    SetTempsDescenteIndividuel(double dbTempsDescente);

    int     GetMaxLoad();
	void    SetMaxLoad(int iMaxLoad);

    int     GetCurrentLoad();
	void    SetCurrentLoad(int iCurrentLoad);

    double  ComputeLoadTime(int iLoadQuantity, int iUnloadQuantity);

    std::vector<Passenger> & GetPassengers(TripNode * pTripNode);

protected:

    // PARAMETRES D'ENTREE
    //////////////////////////////

    // true si le v�hicule permet le chargement / d�chargement simultan�
    bool    m_bMonteeDescenteSimultanee;

    // dur�e (en secondes par unit� de stock) de chargement/d�chargement d'une unit� de chargement (passager, marchandise, ...)
    double  m_dbTempsMonteeIndividuel;
    double  m_dbTempsDescenteIndividuel;

    // Charge maximum que le v�hicule peut embarquer, sans unit� (en passager, en marchandises, ...)
    int     m_MaxLoad;

    // VARIABLES DE CALCUL
    ///////////////////////////////
    int     m_CurrentLoad;

    // Passagers embarqu�s tri�s par destination
    std::map<TripNode* , std::vector<Passenger> > m_currentPassengers;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // LoadUsageParametersH
