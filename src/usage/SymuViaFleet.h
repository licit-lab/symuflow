#pragma once
#ifndef SymuViaFleetH
#define SymuViaFleetH

#include "AbstractFleet.h"

#include <deque>

class SymuViaTripNode;

class SymuViaFleet : public AbstractFleet
{
public:
    SymuViaFleet();
    SymuViaFleet(Reseau * pNetwork);
    virtual ~SymuViaFleet();

    // Instanciation de l'objet sp�cifique � la flotte contenant les param�tres d'un v�hicule li�s � celle-ci
    virtual AbstractFleetParameters * CreateFleetParameters();

    // Initialisation de la flotte pour la simulation de trafic
    virtual void InitSimuTrafic(std::deque< TimeVariation<TraceDocTrafic> > & docTrafics);

    // Sorties sp�cifiques � la flotte dans les fichiers r�sultats
    virtual void SortieTrafic(DocTrafic *pXMLDocTrafic);

    // Traitements sp�cifiques � la flotte appel�s lors du FinCalculTrafic
    virtual void FinCalculTrafic(Vehicule * pVeh);

    // Activation d'un v�hicule sur ordre
    virtual boost::shared_ptr<Vehicule> ActivateVehicle(double dbInstant, VehicleToCreate * pVehicleToCreate);

    // Gestion de la terminaison d'une �tape par un v�hicule de la flotte.
    virtual void OnCurrentLegFinished(boost::shared_ptr<Vehicule> pVehicle, VoieMicro * pDestinationEnterLane, double dbInstDestinationReached, double dbInstant, double dbTimeStep);

    // Met � jour le Trip en fonction des tuyaux parcourus
    virtual void SetLinkUsed(double dbInstant, Vehicule * pVeh, Tuyau * pLink);

    // Effectue les op�rations � faire � l'activation d'un v�hicule
    virtual void OnVehicleActivated(boost::shared_ptr<Vehicule> pVeh, double dbInstant);

    // Renvoie une map d'attributs � sortir de fa�on sp�cifique pour un v�hicule une fois la simulation termin�e
    virtual std::map<std::string, std::string> GetOutputAttributesAtEnd(Vehicule * pV);

protected:
    // Activation des v�hicules pour le Trip pass� en param�tres (si besoin)
    virtual std::vector<boost::shared_ptr<Vehicule> > ActivateVehiclesForTrip(double dbInstant, double dbTimeStep, Trip * pTrip);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // SymuViaFleetH