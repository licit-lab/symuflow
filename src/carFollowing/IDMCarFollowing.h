#pragma once
#ifndef IDMCarFollowingH
#define IDMCarFollowingH

#include "AbstractCarFollowing.h"

class IDMCarFollowing : public AbstractCarFollowing
{
public:
    IDMCarFollowing();
    IDMCarFollowing(bool bImprovedIDM);
    virtual ~IDMCarFollowing();

    virtual bool IsPositionComputed();
    virtual bool IsSpeedComputed();
    virtual bool IsAccelerationComputed();

    virtual double GetMaxInfluenceRange();

    // OTK - LOI - Question : voir si on laisse cette m�thode, ou bien si on estime la vitesse du leader avec la loi Newell, ce qui est � pr�sent
    // possible puisqu'on d�finit le diagramme fondamental Newell pour tous les v�hicules.
    virtual double CalculVitesseApprochee(boost::shared_ptr<Vehicule> pVehicle, boost::shared_ptr<Vehicule> pVehLeader, double dbDistanceBetweenVehicles);

    virtual double ComputeLaneSpeed(VoieMicro * pTargetLane);

    bool IsImprovedIDM();

protected:
    virtual void InternalCompute(double dbTimeStep, double dbInstant, CarFollowingContext * pContext);

	bool IDMFunction(double dbSpeed, double dbRelativePosition, double dbRelativeSpeed, double dbMaximumSpeedOnLane);

private:
    bool m_bImprovedIDM;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // IDMCarFollowingH
