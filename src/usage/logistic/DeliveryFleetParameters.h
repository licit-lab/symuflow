#pragma once
#ifndef DeliveryFleetParametersH
#define DeliveryFleetParametersH

#include "usage/AbstractFleetParameters.h"

#include <map>
#include <vector>

class TripNode;

class DeliveryFleetParameters : public AbstractFleetParameters
{

public:
    DeliveryFleetParameters();
    virtual ~DeliveryFleetParameters();

    std::map<TripNode*, std::vector<std::pair<int,int> > > & GetMapLoadsUnloads();

    virtual AbstractFleetParameters * Clone();
    
protected:

    // D�finition des quantit�s � charger et d�charger � chaque pointde livraison (un vector car une tourn�e peut passer plusieurs fois par un m�me point de livraison)
    std::map<TripNode*, std::vector<std::pair<int,int> > > m_mapLoadsUnloads;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // DeliveryFleetParametersH
