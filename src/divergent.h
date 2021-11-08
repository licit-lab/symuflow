#pragma once
#ifndef divergentH
#define divergentH

#include "ConnectionPonctuel.h"
#include <string>

class Tuyau;

class Divergent :  public ConnectionPonctuel
{
private:

    // Variables caract�ristiques du divergent

    Tuyau*          m_pTuyauAmont;              // Fictif pour un giratoire 

    // Variables de simulation
private:

public:
    // Constructeurs, destructeurs et assimil�s
    virtual ~Divergent(); // Destructeur
    Divergent() ; // Constructeur par d�faut
	Divergent(std::string strID,  Reseau *pReseau);

    // Fonctions relatives � la simulation des divergents    
    void            Init();
    void            Init(Tuyau *pTAm, Tuyau *pTAvP, Tuyau *pTAvS);

    void            CorrigeAcceleration(double dbInst);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};

#endif
