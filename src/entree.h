#pragma once
#ifndef entreeH
#define entreeH

#include "ConnectionPonctuel.h"
#include "usage/SymuViaTripNode.h"

class Tuyau;
class DocTrafic;

// Entree est une classe fille de connection. Il s'agit d'un des deux �lements
// r�seau qu'il est possible de trouver en amont d'un tron�on. Une entr�e est
// responsable de l'injection des v�hicules sur le r�seau

class Entree : public SymuViaTripNode, public ConnectionPonctuel
{
		
public:

        // Constructeurs, destructeurs et assimil�s
	    virtual ~Entree(void) ; // Destructeur
	    Entree(char*, Reseau *pR); 
        Entree() ; // Constructeurs par d�faut

		void SortieTrafic(DocTrafic *pXMLDocTrafic);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};
#endif

