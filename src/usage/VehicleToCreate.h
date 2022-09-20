#pragma once
#ifndef VehicleToCreateH
#define VehicleToCreateH

class AbstractFleet;
class Trip;

namespace boost {
    namespace serialization {
        class access;
    }
}

#include <string>

// D�finition des param�tres de cr�ation arbitraire d'un v�hicule (fonctions SymCreateVehicle)
class VehicleToCreate
{
public:
    VehicleToCreate();
    VehicleToCreate(int vehId, AbstractFleet * pFleet);
    virtual ~VehicleToCreate();

    int GetVehicleID();
    AbstractFleet * GetFleet();

    Trip * GetTrip();
    void SetTrip(Trip * pTrip);

    void SetNoKnownDestination(bool bNoKnownDestination);
    bool GetNoKnownDestination() const;

    void SetInitialPosition(double dbInitialPosition);
    double GetInitialPosition() const;

    void SetInitialSpeed(double dbInitialSpeed);
    double GetInitialSpeed() const;

    void SetInitialAcceleration(double dbInitialAcceleration);
    double GetInitialAcceleration() const;

protected:
    int m_VehicleId;
    AbstractFleet * m_pFleet;
    Trip * m_pTrip;

    bool m_bNoKnownDestination;
    double m_dbInitialPosition;
    double m_dbInitialSpeed;
    double m_dbInitialAcceleration;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // VehicleToCreateH