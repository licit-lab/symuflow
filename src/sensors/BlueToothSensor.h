#pragma once
#ifndef BlueToothSensorH
#define BlueToothSensorH

#include "AbstractSensor.h"

#include <vector>
#include <map>

class PonctualSensor;
class PonctualSensorSnapshot;
class Connexion;
class Tuyau;
class SensorsManager;

struct BlueToothVehSensorData
{
    BlueToothVehSensorData();
    int nbVehs; // nombre de v�hicules d�tect�s pour un mouvement
    std::vector<int> nIDs;  // Identifiants des v�hicules pris en compte pour le mouvement
    std::vector<double> dbInstEntrees; // Instants d'entr�e des v�hicules pris en compte pour le mouvement
    std::vector<double> dbInstSorties; // Instants de sortie des v�hicules pris en compte pour le mouvement

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

/*===========================================================================================*/
/* Structure BlueToothSensorData                                                                 */
/*===========================================================================================*/
struct BlueToothSensorData
{
        std::map<std::string, std::map<std::string, BlueToothVehSensorData> > mapCoeffsDir; // nombre de v�hicule d�tect�s pour chaque mouvement

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};


class BlueToothSensorSnapshot : public AbstractSensorSnapshot
{
public:

    virtual ~BlueToothSensorSnapshot();

	BlueToothSensorData data;  // Donn�es du capteur bluetooth

    std::vector<PonctualSensorSnapshot*> lstCapteursAmontSnapshots;
    std::vector<PonctualSensorSnapshot*> lstCapteursAvalSnapshots;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


class BlueToothSensor : public AbstractSensor
{
public:
    BlueToothSensor();
    BlueToothSensor(SensorsManager * pParentSensorsManager, const std::string & strNom, Connexion *pNode, const std::map<Tuyau*, double> &mapPositions);
    virtual ~BlueToothSensor();

    virtual std::string GetSensorPeriodXMLNodeName() const;

    virtual void CalculInfoCapteur(Reseau * pNetwork, double dbInstant, bool bNewPeriod, double dbInstNewPeriode, boost::shared_ptr<Vehicule> pVeh);

    virtual void AddMesoVehicle(double dbInstant, Vehicule * pVeh, Tuyau * pLink, Tuyau * pDownstreamLink, double dbLengthInLink = DBL_MAX);

    virtual void WriteDef(DocTrafic* pDocTrafic);
    virtual void Write(double dbInstant, Reseau * pReseau, double dbPeriodAgregation, double dbDebut, double dbFin, const std::deque<TraceDocTrafic* > & docTrafics, CSVOutputWriter * pCSVOutput);

    virtual void PrepareNextPeriod();
    
    virtual AbstractSensorSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, AbstractSensorSnapshot * backup);

protected:
    Connexion*                   m_pNode;
    std::vector<PonctualSensor*> lstCapteursAmont;
    std::vector<PonctualSensor*> lstCapteursAval;

	// Informations dynamiques du capteur agr�g�es sur une p�riode finie (non glissante)
	BlueToothSensorData    data;		

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // BlueToothSensorH