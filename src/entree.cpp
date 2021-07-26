#include "stdafx.h"
#include "entree.h"

#include "tuyau.h"
#include "SystemUtil.h"
#include "DocTrafic.h"
#include "usage/Position.h"

//---------------------------------------------------------------------------
// Constructeurs, destructeurs et assimil�s
//---------------------------------------------------------------------------

// Destructeurs
Entree::~Entree()
{    
}

// Constructeur par d�faut
Entree::Entree() : SymuViaTripNode(), ConnectionPonctuel()
{
}

// Constructeurs normal de l'entr�e
Entree::Entree(char _Nom[256], Reseau *pR): SymuViaTripNode(_Nom, pR),
    ConnectionPonctuel(_Nom, pR)
{
    // La position de sortie du TripNode vers le r�seau est l'entr�e elle-m�me
    m_OutputPosition.SetConnection(this);
}



//================================================================
	void Entree::SortieTrafic
//----------------------------------------------------------------
// Fonction  : Module de sortie des donn�es trafic de l'entr�e
// Version du: 16/11/2009
// Historique: 16/11/2009 (C.B�carie - Tinea)
//             Cr�ation
//================================================================
(
	DocTrafic *pXMLDocTrafic
)
{
	int nVeh = 0;

	if( m_LstTuyAv.size()==0 )
		return;

	if( !m_LstTuyAv.front() )
		return;

	for(int nVoie = 0; nVoie < m_LstTuyAv.front()->getNb_voies(); nVoie++)	
	{
		nVeh += (int) m_mapVehEnAttente[m_LstTuyAv.front()].find( nVoie )->second.size();		// Comptage des v�hicules en attente
		if( m_mapVehPret[m_LstTuyAv.front()].find( nVoie )->second != -1)						// Ajout du v�hicule en attente mais pr�t
			nVeh++;
	}

	pXMLDocTrafic->AddSimuEntree(SymuViaTripNode::GetID(), nVeh);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void Entree::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void Entree::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void Entree::serialize(Archive & ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SymuViaTripNode);
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ConnectionPonctuel);
}