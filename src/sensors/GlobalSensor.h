#pragma once
#ifndef GlobalSensorH
#define GlobalSensorH

#include "AbstractSensor.h"
#include "tools.h"

#include <map>


class Vehicule;

struct VehData
{
    VehData();

    //double dbInstCreation;		// instant de cr�ation du v�hicule (si -1, ant�rieure � la p�riode courante)
	double dbInstSuppres;		// instant de sortie du r�seau du v�hicule (si -1, post�rieur � la p�riode courante)
	//double dbDstParcourue;		// Distance parcourue au cours de la p�riode
	double dbDstCumParcPrev;	// Distance cumul�e parcourue depuis sa cr�ation et jusqu'� la p�riode pr�c�dente

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

/*===========================================================================================*/
/* Structure GlobalSensorData                                                                 */
/*===========================================================================================*/
struct GlobalSensorData
{
    std::map<boost::shared_ptr<Vehicule>, VehData, LessVehiculePtr<Vehicule> > mapVehs; // maps des v�hicules yant circul� sur le r�seau durant la p�riode d'agr�gation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};


struct GlobalSensorSnapshot : public AbstractSensorSnapshot
{
	std::map<int, VehData> mapVehs;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


class GlobalSensor: public AbstractSensor
{
public:
    GlobalSensor();
	virtual ~GlobalSensor();

    virtual std::string GetSensorPeriodXMLNodeName() const;

    virtual void CalculInfoCapteur(Reseau * pNetwork, double dbInstant, bool bNewPeriod, double dbInstNewPeriode, boost::shared_ptr<Vehicule> pVeh);

    virtual void AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink = DBL_MAX);

    virtual void WriteDef(DocTrafic* pDocTrafic);
    virtual void Write(double dbInstant, Reseau * pReseau, double dbPeriodAgregation, double dbDebut, double dbFin, const std::deque<TraceDocTrafic* > & docTrafics, CSVOutputWriter * pCSVOutput);

    virtual void PrepareNextPeriod();

    virtual AbstractSensorSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, AbstractSensorSnapshot * backup);

private:
    void ProcessVehicle(boost::shared_ptr<Vehicule> pV);

protected:
	// Informations dynamiques du capteur agr�g�es sur une p�riode finie (non glissante)
	GlobalSensorData    data;		

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};


#endif // GlobalSensorH