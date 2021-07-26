#pragma once

//#ifndef __AFXWIN_H__
//	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
//#endif

#include "resource.h"		// symboles principaux

#include <boost/thread/shared_mutex.hpp>

#include <map>

class Reseau;

class CSymubruitApp
{
public:

    class CSymubruitNetworkInstance {
    public:

        CSymubruitNetworkInstance(Reseau * pNetwork);
        ~CSymubruitNetworkInstance();

        Reseau * GetNetwork() const;

        // Mutex utilis�e pour la synchronisation entre le calcul de l'�coulement
        // et le calcul en parall�le de plus courts chemins
        boost::shared_mutex m_ShortestPathsComputeMutex;

    private:
        Reseau * m_pNetwork;
    };

public:

	CSymubruitApp();
    virtual ~CSymubruitApp();

    void DeleteAllNetworks();
    void DeleteNetwork(int networkId);
    void AddNetwork(int networkId, Reseau * pNetwork);
    Reseau * CreateNetwork(bool bUseDefaultNetworkId, int & networkId);

    Reseau * GetNetwork(int networkId);
    CSymubruitNetworkInstance * GetNetworkInstance(int networkId);

// Substitutions
    virtual bool InitInstance();
    virtual int ExitInstance();

    // Mutex utilis�e pour synchroniser les acc�s concurrents � m_mapNetworks;
    boost::shared_mutex m_NetworksAccessMutex;

private:
    // r�seaux en cours d'utilisation
    std::map<int, CSymubruitNetworkInstance*> m_mapNetworks;
};

extern CSymubruitApp theApp;
