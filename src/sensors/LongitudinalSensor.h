#pragma once
#ifndef LongitudinalSensorH
#define LongitudinalSensorH

#include "AbstractSensor.h"

#include <map>

class Tuyau;

/*===========================================================================================*/
/* Structure DataCapteurLongitudinal                                                         */
/*===========================================================================================*/
class LongitudinalSensorData : public AbstractSensorSnapshot
{
public:
    std::map<std::pair<int, int>, int>   mapNbChgtVoie;  // Map du nombre de v�hicule ayant chang� de voie dans la zone d�finie par le capteur
                                                         // d�taill� couple de voie par couple de voie

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

class LongitudinalSensor : public AbstractSensor
{
public:
    LongitudinalSensor();
    LongitudinalSensor(const std::string & strName, Tuyau * pTuyau, double dbStartPos, double dbEndPos);
    virtual ~LongitudinalSensor();

    virtual std::string GetSensorPeriodXMLNodeName() const;

    void InitData();

    void AddChgtVoie(Tuyau * pTuyau, double dbPosition, int nVoieOrigine, int nVoieDestination);

    virtual void CalculInfoCapteur(Reseau * pNetwork, double dbInstant, bool bNewPeriod, double dbInstNewPeriode, boost::shared_ptr<Vehicule> pVeh);

    virtual void WriteDef(DocTrafic* pDocTrafic);
    virtual void Write(double dbInstant, Reseau * pReseau, double dbPeriodAgregation, double dbDebut, double dbFin, const std::deque<TraceDocTrafic* > & docTrafics, CSVOutputWriter * pCSVOutput);

    virtual void PrepareNextPeriod();

    double GetSensorLength();

    Tuyau * GetTuyau();
    double GetStartPosition();
    double GetEndPosition();

    virtual AbstractSensorSnapshot * TakeSnapshot();
    virtual void Restore(Reseau * pNetwork, AbstractSensorSnapshot * backup);

protected:

    Tuyau*      m_pTuyau;
    double      dbPositionDebut;    // Positionnement en m du d�but du capteur (� partir du d�but du tuyau
    double      dbPositionFin;      // Positionnement en m de la fin du capteur (� partir du d�but du tuyau)

	// Informations dynamiques du capteur agr�g�es sur une p�riode finie (non glissante)
	LongitudinalSensorData	data;		

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
};

#endif // LongitudinalSensorH