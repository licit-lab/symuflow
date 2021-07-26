#pragma once

class TypeVehicule;

namespace boost {
    namespace serialization {
        class access;
    }
}

// Structure de stockage des temps d'insertion sp�cifique � chaque type de v�hicule
struct TempsCritique
{
    TypeVehicule    *pTV;   // Type de v�hicule
    double          dbtc;   // Temps d'insertion

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};