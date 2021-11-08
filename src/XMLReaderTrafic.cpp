#include "stdafx.h"
#include "XMLReaderTrafic.h"

#include "SystemUtil.h"
#include "Xerces/XMLReaderSymuvia.h"
#include "Logger.h"

#include <xercesc/dom/DOMException.hpp>

XERCES_CPP_NAMESPACE_USE


XMLReaderTrafic::XMLReaderTrafic(Logger* pFS)
{
	m_XmlReaderTraj = NULL;
	m_XmlReaderVeh = NULL;
	m_XmlReaderSimuCell = NULL;

    pFicSimulation = pFS;
}

XMLReaderTrafic::~XMLReaderTrafic()
{
    if(m_XmlReaderVeh)
        m_XmlReaderVeh->Close();
	delete m_XmlReaderVeh;
	m_XmlReaderVeh = NULL;

    if(m_XmlReaderTraj)
        m_XmlReaderTraj->Close();
	delete m_XmlReaderTraj;
	m_XmlReaderTraj = NULL;
        
    if(m_XmlReaderSimuCell)
        m_XmlReaderSimuCell->Close();
	delete m_XmlReaderSimuCell;
	m_XmlReaderSimuCell = NULL;
}

// Retourne la valeur d'un attribut pour la trajectoire courante
std::string XMLReaderTrafic::GetValueAttrFromTrajectoire(const std::string & ssAttr)
{		
	return m_XmlReaderTraj->GetAttribute(ssAttr);
}

// Retourne la valeur d'un attribut pour la cellule de simulation courante
std::string XMLReaderTrafic::GetValueAttrFromSimuCell(const std::string & ssAttr)
{		
	return m_XmlReaderSimuCell->GetAttribute(ssAttr);
}

// Initialisation
bool XMLReaderTrafic::Init(const std::string & strFilename, std::string & sTimeExecTraf)
{	
    bool bOK = true;
    try
    {
        m_strFilename = strFilename;

        m_XmlReaderVeh = XmlReaderSymuvia::Create( strFilename);
        m_XmlReaderTraj = XmlReaderSymuvia::Create( strFilename);
        m_XmlReaderSimuCell = XmlReaderSymuvia::Create( strFilename); 

        // Lecture de l'horodatage du lancement de la simulation trafic
        m_XmlReaderVeh->ReadToFollowing( "SIMULATION" );
		sTimeExecTraf = m_XmlReaderVeh->GetAttribute("time");

	    // On place les curseurs au bon endroit
	    bOK = bOK & m_XmlReaderVeh->ReadToFollowing( "VEHS" );
        bOK = bOK & m_XmlReaderTraj->ReadToFollowing( "INST" );
        bOK = bOK & m_XmlReaderSimuCell->ReadToFollowing( "INST" );
    }
	catch(DOMException *e)
    {
        char * exMsg = XMLString::transcode(e->getMessage());
		(*pFicSimulation) << exMsg << std::endl;
        XMLString::release(&exMsg);
        return false;
    }

    return bOK;
}

// Routine permettant la lecture des trajectoires
int XMLReaderTrafic::ReadNextTrajectoire(double dbInstant, int nID)
{
    try
    {
        if( nID == -1)  // Premi�re fois du pas de temps courant...
        {
            // Est-on au bon endroit par rapport � l'instant trait� ?
			const std::string & sVal = m_XmlReaderTraj->GetAttribute("val");
			if (fabs(atof(sVal.c_str()) - dbInstant) > 0.1 )
                return -1;            
 
            if( !m_XmlReaderTraj->ReadToFollowing( "TRAJS" ))
                    return -1;
            
			m_XmlReaderTraj->Read();   		// On se positionne sur le noeud TRAJ 				
        }

        // On passe � l'�l�ment suivant
		if( nID > -1)
			m_XmlReaderTraj->Read();      

        if(m_XmlReaderTraj->Name != "TRAJ")
        {
            nID = -1;
            m_XmlReaderTraj->ReadToFollowing( "INST" );  // On se positionne sur l'instant suivant
        }
        else
        {
			nID = atoi(m_XmlReaderTraj->GetAttribute("id").c_str()); // et le curseur est bien positionn�
        }
    }
	catch (DOMException *e)
    {
        char * exMsg = XMLString::transcode(e->getMessage());
		(*pFicSimulation) << exMsg << std::endl;
        XMLString::release(&exMsg);
        return -1;
    }

	return nID;
}

// Lecture des donn�es du noeud <VEHS>
void XMLReaderTrafic::ReadVehicules()
{
    // Au premier passage, lecture des donn�es de l'ensemble des v�hicules
    if(m_mapVehData.size() == 0)
    {
        XmlReaderSymuvia* XmlReaderSelectedVeh = XmlReaderSymuvia::Create( m_strFilename);
        XmlReaderSelectedVeh->ReadToFollowing( "VEHS" );

        XmlReaderSelectedVeh->Read(); // Passer au premier "VEH"

        while( !XmlReaderSelectedVeh->eof && XmlReaderSelectedVeh->GetAttribute("id").compare(""))
        {
            // r�cup�ration des donn�es
            std::string vehID = XmlReaderSelectedVeh->GetAttribute("id");
            VehTraficData data;
            data.type = XmlReaderSelectedVeh->GetAttribute("type");
            data.entree = XmlReaderSelectedVeh->GetAttribute("entree");
            data.sortie = XmlReaderSelectedVeh->GetAttribute("sortie");
            data.instC = XmlReaderSelectedVeh->GetAttribute("instC");
            data.instE = XmlReaderSelectedVeh->GetAttribute("instE");
            data.instS = XmlReaderSelectedVeh->GetAttribute("instS");
            data.dstParcourue = XmlReaderSelectedVeh->GetAttribute("dstParcourue");
            m_mapVehData[vehID] = data;
            

		    XmlReaderSelectedVeh->Read(); // passage au v�hicule suivant
        }

        delete XmlReaderSelectedVeh;
    }
}

// Retourne la valeur d'un attribut pour un v�hicule 
VehTraficData & XMLReaderTrafic::GetAttrFromVehicule(const std::string & id)
{		        
	return m_mapVehData[id];
}

// Routine de lecture des cellules au cours de la simulation
int XMLReaderTrafic::ReadNextSimuCell(double dbInstant, int nID)
{
    try
    {
        if( nID == -1)  // Premi�re fois...
        {
            // Est-on au bon endroit par rapport � l'instant trait� ?
			if( fabs(atof(m_XmlReaderSimuCell->GetAttribute("val").c_str()) - dbInstant) > 0.00001)
                return -1;
            
                if( !m_XmlReaderSimuCell->ReadToFollowing( "SGTS" ))
                    return -1;
        }
    

        // On passe � l'�l�ment suivant
        m_XmlReaderSimuCell->Read();  

        if(m_XmlReaderSimuCell->Name != "SGT")
        {
            nID = -1;
            m_XmlReaderSimuCell->ReadToFollowing( "INST" );  // On se positionne sur l'instant suivant
        }
        else
        {
			nID = atoi(m_XmlReaderSimuCell->GetAttribute("id").c_str()); // et le curseur est bien positionn�
        }
    }
	catch(DOMException *e)
    {
        char * exMsg = XMLString::transcode(e->getMessage());
		(*pFicSimulation) << exMsg << std::endl;
        XMLString::release(&exMsg);
        return -1;
    }

	return nID;
}
