#pragma once
#ifndef AbstractFleetParametersH
#define AbstractFleetParametersH

#include "VehicleTypeUsageParameters.h"

class ScheduleParameters;

class AbstractFleetParameters
{

public:
    AbstractFleetParameters();
    virtual ~AbstractFleetParameters();

    virtual AbstractFleetParameters * Clone();

    VehicleTypeUsageParameters & GetUsageParameters();
    
    void SetScheduleParameters(ScheduleParameters * pScheduleParams);
    ScheduleParameters * GetScheduleParameters();

protected:

    // Param�tres pseudo g�n�riques communs � plusieurs types de flottes
    VehicleTypeUsageParameters m_UsageParameters;

    // Pointeur vers le jeu de param�tres li� � l'�lement de calendrier d�clenchant l'activation du v�hicule
    ScheduleParameters * m_pScheduleParameters;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // AbstractFleetParametersH
