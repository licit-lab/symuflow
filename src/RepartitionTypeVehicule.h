#pragma once
#ifndef RepartitionTypeVehiculeH
#define RepartitionTypeVehiculeH

#include "tools.h"

class RepartitionTypeVehicule
{
public:

	RepartitionTypeVehicule();
	~RepartitionTypeVehicule();

    std::deque<TimeVariation<std::vector<std::vector<double> > > > & GetLstCoefficients();

    // Ajout des variations temporelles de r�partition des types de v�hicules
    void AddVariation(const std::vector<std::vector<double> > & coeffs, PlageTemporelle * pPlage);
    void AddVariation(const std::vector<std::vector<double> > & coeffs, double dbDuree);

    // Supprime toutes les r�partitions d�finies
    void Clear();

    // Supprime les r�partitions d�finies depuis l'instant pass� en param�tres
    void ClearFrom(double dbInstant, double dbLag);

    // Effectue le tirage d'un nouveau type de v�hicule
    TypeVehicule* CalculTypeNewVehicule(Reseau * pNetwork, double  dbInstant, int nVoie);

    // Indique s'il existe un coefficient non nul pour un type de v�hicule donn� entre les instants sp�cifi�s
    bool HasVehicleType(Reseau * pNetwork, int indexVehicleType, double dbStartTime, double dbEndTime);

protected:

    // R�cup�ration de la proportion d'un type de v�hicule � un instant donn�
    double GetRepartitionTypeVehicule(double dbInstant, double dbLag, int nTypeVehicule, int nVoie);

protected:

    // Coefficients par type de v�hicule puis par voie
    std::deque<TimeVariation<std::vector<std::vector<double> > > > m_LstCoefficients;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif // RepartitionTypeVehicule