#pragma once
#ifndef connectionPonctuelH
#define connectionPonctuelH

//#define NB_ELT_MAX 10

#include "Connection.h"

#include <boost/shared_ptr.hpp>

#include <deque>

class Reseau;
class Vehicule;
class VoieMicro;

// La classe connexion est la classe m�re de toutes les liaisons entre les tron�ons.
// Ses fils sont entr�es, sorties et r�partiteurs

class ConnectionPonctuel : public Connexion
{
protected:

    std::deque<boost::shared_ptr<Vehicule> > m_LstInsVeh;        // Liste des v�hicules suceptibles de traverser la connexion au cours du pas de temps (d�cel� en appliquant la loi de poursuite classique)

public:

        // Constructeurs, destructeurs et assimil�s
	    virtual ~ConnectionPonctuel(void); // Destructeur
        ConnectionPonctuel(void); // constructeur par d�faut
		ConnectionPonctuel(std::string, Reseau *pReseau); // constructeur normal

        bool Init( XERCES_CPP_NAMESPACE::DOMNode *pXmlNodeCnx, Logger *pOfLog);

        void        AddInsVeh(boost::shared_ptr<Vehicule> pV){ m_LstInsVeh.push_back(pV); }
        void        DelInsVeh(boost::shared_ptr<Vehicule> pV);

        void        ClearLstInsVeh(){m_LstInsVeh.clear();}

        void        CopyFrom( ConnectionPonctuel *pCnx);

		void	    InitSimuTrafic(){}

        virtual bool ComputeTraffic(boost::shared_ptr<Vehicule> pV, VoieMicro * pUpstreamLane, double dbInstant, double dbPasTemps, double &dbMinimizedGoalDistance) {return false;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif
