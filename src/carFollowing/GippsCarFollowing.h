#pragma once
#ifndef GippsCarFollowingH
#define GippsCarFollowingH

#include "AbstractCarFollowing.h"

class GippsCarFollowing : public AbstractCarFollowing
{
public:
    GippsCarFollowing();
    virtual ~GippsCarFollowing();

    virtual bool IsPositionComputed();
    virtual bool IsSpeedComputed();
    virtual bool IsAccelerationComputed();

    virtual double GetMaxInfluenceRange();

    // OTK - LOI - Question : voir si on laisse cette m�thode, ou bien si on estime la vitesse du leader avec la loi Newell, ce qui est � pr�sent
    // possible puisqu'on d�finit le diagramme fondamental Newell pour tous les v�hicules.
    virtual double CalculVitesseApprochee(boost::shared_ptr<Vehicule> pVehicle, boost::shared_ptr<Vehicule> pVehLeader, double dbDistanceBetweenVehicles);

    virtual double ComputeLaneSpeed(VoieMicro * pTargetLane);

protected:
    virtual void InternalCompute(double dbTimeStep, double dbInstant, CarFollowingContext * pContext);

private:
    double ComputeFreeFlowSpeed(double dbTimeStep, double dbInstant, CarFollowingContext * pContext);
    double ComputeCongestedSpeed(double dbInstant, double dbTimeStep, CarFollowingContext * pContext);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // GippsCarFollowingH
