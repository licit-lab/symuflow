#include "stdafx.h"
#include "IDMCarFollowing.h"

#include "../reseau.h"
#include "../vehicule.h"
#include "../voie.h"
#include "../tuyau.h"
#include "../DiagFonda.h"
#include "CarFollowingContext.h"
#ifdef USE_SYMUCOM
#include "../ITS/Stations/DerivedClass/BMAVehicle.h"
#endif // USE_SYMUCOM



IDMCarFollowing::IDMCarFollowing()
{
    m_bImprovedIDM = false;
}

IDMCarFollowing::IDMCarFollowing(bool bImprovedIDM)
{
    m_bImprovedIDM = bImprovedIDM;
}

IDMCarFollowing::~IDMCarFollowing()
{
}

bool IDMCarFollowing::IsPositionComputed()
{
    return false;
}

bool IDMCarFollowing::IsSpeedComputed()
{
    return false;
}

bool IDMCarFollowing::IsAccelerationComputed()
{
    return true;
}

#define IDM_ALPHA 4
#define IDM_BETA  1.5
#define IDM_T 1

double IDMCarFollowing::GetMaxInfluenceRange()
{
    double dbDeceleration = m_pVehicle->GetType()->GetDeceleration();
    // rmq. par d�faut, sans SymuVia, pas de d�c�l�ration d�finie = d�c�l�ration � 0. On remplace par une d�c�l�ration infinie !
    if(dbDeceleration == 0)
    {
        dbDeceleration = DBL_MAX;
    }
    double dbMaxDesiredSpacing = m_pVehicle->GetType()->GetEspacementArret() + std::max<double>(IDM_T * m_pVehicle->GetVitMax() + m_pVehicle->GetVitMax() *  m_pVehicle->GetVitMax() / (2.0 * sqrt(m_pVehicle->GetType()->GetAccMax(m_pVehicle->GetVitMax()) * dbDeceleration)),0);
    return 3 * dbMaxDesiredSpacing;
}

bool IDMCarFollowing::IDMFunction(double dbSpeed, double dbRelativePosition, double dbRelativeSpeed, double dbMaximumSpeedOnLane)
{
	bool bFreeFlow = true;

	// R�cup�ration de l'acc�l�ration maximum
	double dbMaximumAcceleration = m_pVehicle->GetAccMax(dbSpeed);

	double dbDeceleration = m_pVehicle->GetType()->CalculDeceleration(m_pNetwork);
	// rmq. par d�faut, sans SymuVia, pas de d�c�l�ration d�finie = d�c�l�ration � 0. On remplace par une d�c�l�ration infinie !
	if (dbDeceleration == 0)
	{
		dbDeceleration = DBL_MAX;
	}

	double dbZ;
	if (dbRelativePosition <= 0)
	{
		//rmq. patch pour adapter la formule au cas o� les v�hicules se rentrent dedans.
		//dbZ = DBL_MAX;
		dbZ = 2;
	}
	else
	{
		double dbDeltaSPrime = m_pVehicle->GetType()->GetEspacementArret() + std::max<double>(IDM_T * dbSpeed
			+ (dbSpeed * dbRelativeSpeed) / (2.0 * sqrt(dbMaximumAcceleration * dbDeceleration)), 0);
		dbZ = dbDeltaSPrime / dbRelativePosition;
	}

	bFreeFlow = dbZ < 1;

	if (m_bImprovedIDM)
	{
		if (dbSpeed <= dbMaximumSpeedOnLane)
		{
			if (dbZ >= 1)
			{
				m_dbComputedTravelAcceleration = dbMaximumAcceleration * (1.0 - dbZ*dbZ);
			}
			else
			{
				double dbAFree = dbMaximumAcceleration * (1.0 - pow(m_pVehicle->GetVit(1) / dbMaximumSpeedOnLane, IDM_ALPHA));
				if (dbAFree != 0)
				{
					m_dbComputedTravelAcceleration = dbAFree * (1.0 - pow(dbZ, IDM_BETA * dbMaximumAcceleration / dbAFree));
				}
				else
				{
					m_dbComputedTravelAcceleration = 0;
				}
			}
		}
		else
		{
			double dbAFree = -dbDeceleration * (1.0 - pow(dbMaximumSpeedOnLane / dbSpeed, dbMaximumAcceleration * IDM_ALPHA / dbDeceleration));
			if (dbZ >= 1)
			{
				m_dbComputedTravelAcceleration = dbAFree + dbMaximumAcceleration * (1.0 - pow(dbZ, IDM_BETA));
			}
			else
			{
				m_dbComputedTravelAcceleration = dbAFree;
			}
		}
	}
	else
	{
		// Calcul de la composante "libre", poussant le v�hicule � acc�l�rer pour se rapprocher de sa vitesse maximum d�sir�e
		double dbFreeFlowComposant = pow(dbSpeed / dbMaximumSpeedOnLane, IDM_ALPHA);

		// Calcul de la composante "congestionn�e", poussant le v�hicule � ne pas acc�l�rer pour se ternir � distance du leader
		double dbCongestedComposant = pow(dbZ, IDM_BETA);

		m_dbComputedTravelAcceleration = dbMaximumAcceleration * (1.0 - dbFreeFlowComposant - dbCongestedComposant);
	}

	return bFreeFlow;
}

void IDMCarFollowing::InternalCompute(double dbTimeStep, double dbInstant, CarFollowingContext * pContext)
{
    bool bFreeFlow = true;

	bool bUseBMA = false;
	double dbSpeed;
	double dbRelativePosition;
	double dbRelativeSpeed;
	double dbMaximumSpeedOnLane;


	// rmq. : contrairement � Newell, on ne tient compte que de la vitesse r�glementaire correspondant � la position
	// du v�hicule au d�but du pas de temps => approximation pas bien m�chante.
	if (!pContext->GetLstLanes().empty())
	{

		VoieMicro * pFirstLane = pContext->GetLstLanes().front();

		// R�cup�ration de la vitesse du vehicules de la vitesse maximum d�sir�e
		dbMaximumSpeedOnLane = ((Tuyau*)pFirstLane->GetParent())->GetVitRegByTypeVeh(m_pVehicle->GetType(), dbInstant, pContext->GetStartPosition(), pFirstLane->GetNum());
		dbMaximumSpeedOnLane = std::min<double>(m_pVehicle->GetVitMax(), dbMaximumSpeedOnLane);

#ifdef USE_SYMUCOM
		//On test si on a un v�hicule conn�ct�
		if (m_pVehicle->GetConnectedVehicle())
		{
			BMAVehicle* pBMAVeh = dynamic_cast<BMAVehicle*>(m_pVehicle->GetConnectedVehicle());
			if (pBMAVeh)
			{
				//Si le type de vehicule connect� est bien BMA, on r�cup�re la vitesse, la position relative et la vitesse relative directement gr�ce aux capteurs et aux communications entre v�hicules
				if (pBMAVeh->GetSpeed(dbSpeed))
				{
					if (!(pBMAVeh->GetRelativePosition(dbRelativePosition) && pBMAVeh->GetRelativeSpeed(dbRelativeSpeed)))
					{
						dbRelativePosition = DBL_MAX;
						dbRelativeSpeed = dbSpeed;
					}
					bUseBMA = true;
				}
			}
		}
#endif // USE_SYMUCOM

		//si aucun v�hicule conn�ct�, on utilise les valeurs direct du v�hicule
		if (!bUseBMA)
		{
			// R�cup�ration de la vitesse du vehicules ainsi que de la position et vitesse relative � son leader direct
			dbSpeed = m_pVehicle->GetVit(1);
			if (!pContext->GetLeaders().empty())
			{
				boost::shared_ptr<Vehicule> pLeader = pContext->GetLeaders().front();
				double dbVehicleDistance = pContext->GetLeaderDistances().front();

				dbRelativePosition = (dbVehicleDistance - pLeader->GetLength());
				dbRelativeSpeed = (m_pVehicle->GetVit(1) - pLeader->GetVit(1));
			}
			else
			{
				dbRelativePosition = DBL_MAX;
				dbRelativeSpeed = m_pVehicle->GetVit(1);
			}
		}

		bFreeFlow = IDMFunction(dbSpeed, dbRelativePosition, dbRelativeSpeed, dbMaximumSpeedOnLane);

	}
	else
	{
		m_dbComputedTravelAcceleration = 0;
	}

    pContext->SetFreeFlow(bFreeFlow);
}


//================================================================
double IDMCarFollowing::CalculVitesseApprochee
//----------------------------------------------------------------
// Fonction  :  Calcule de la vitesse approch�e d'un v�hicule � la fin
//              du pas de temps dans le cas de la nouvelle loi de poursuite
// Remarque  :  (3) de 'A microscopic dual-regime model for roundabouts'
// Version du:  07/12/2007
// Historique:  07/12/2007 (C.B�carie - Tinea )
//              Cr�ation
//================================================================
(
    boost::shared_ptr<Vehicule> pVehicle,
    boost::shared_ptr<Vehicule> pVehLeader,
    double dbDistanceBetweenVehicles
)
{    
    // OTK - IDM - pas la peine de l'impl�menter tant qu'on ne fait pas de lois de poursutie mixtes (plusieurs lois sur un m�me r�seau) : ne sert que pour Newell.
    return 0;
}


double IDMCarFollowing::ComputeLaneSpeed(VoieMicro * pTargetLane)
{
	double dbVitVoieCible = 0.0;
	double dbSf;

	// Recherche des v�hicules 'follower' et 'leader' de la voie cible
	boost::shared_ptr<Vehicule> pFollower = m_pNetwork->GetNearAmontVehicule(pTargetLane, pTargetLane->GetLength() / m_pVehicle->GetVoie(1)->GetLength() * m_pVehicle->GetPos(1) + 0.1);
	boost::shared_ptr<Vehicule> pLeader = m_pNetwork->GetNearAvalVehicule(pTargetLane, pTargetLane->GetLength() / m_pVehicle->GetVoie(1)->GetLength() * m_pVehicle->GetPos(1) + 0.1);

	// Calcul de l'inter-distance entre le v�hicule follower et le vehicule leader de la voie cible
	if (pFollower && pLeader)
	{
		dbSf = m_pNetwork->GetDistanceEntreVehicules(pLeader.get(), pFollower.get());
	}
	else
	{
		// OTK - LOI - DIAG - remplacer par une notion g�n�rique ?
		dbSf = 1 / m_pVehicle->GetDiagFonda()->GetKCritique();
	}

	// Calcul de la vitesse associ�e � l'inter-distance entre le v�hicule follower et leader de la voie cible
	if (dbSf>0)
	{
		double dbVal1 = 1 / dbSf;
		double dbVal2 = VAL_NONDEF;

		if (pLeader)
			pLeader->GetDiagFonda()->CalculVitEqDiagrOrig(dbVitVoieCible, dbVal1, dbVal2, true); // Utilisation du diagramme fondamental du v�hicule leader de la voie cible
		else
			m_pVehicle->GetDiagFonda()->CalculVitEqDiagrOrig(dbVitVoieCible, dbVal1, dbVal2, true); // Utilisation du diagramme fondamental du v�hicule candidat au changement de voie

		if (pLeader)
		{
			dbVitVoieCible = pLeader->GetVit(1);
		}
	}

	return dbVitVoieCible;
}


bool IDMCarFollowing::IsImprovedIDM()
{
    return m_bImprovedIDM;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void IDMCarFollowing::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void IDMCarFollowing::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void IDMCarFollowing::serialize(Archive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(AbstractCarFollowing);
    ar & BOOST_SERIALIZATION_NVP(m_bImprovedIDM);
}
