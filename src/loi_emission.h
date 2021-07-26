#pragma once
#ifndef loi_emissionH
#define loi_emissionH

#include "SQLSymuvia.h"

namespace boost {
    namespace serialization {
        class access;
    }
}

class Reseau;

// La classe loi d'�mission stocke les lois d'�mission de tous les types de v�hicules
// circulant sur le r�seau
class Loi_emission{
private:
		DBSymuvia		    m_DBLoiEmission;            // Pointeur sur la base de donn�es Loi Emission

        SQLSymuvia			m_DBRevetements;            // Liste des rev�tements
        SQLSymuvia			m_DBAllures;                // Liste des allures
        SQLSymuvia			m_DBTypeVehicules;          // Liste des types de v�hicules
        SQLSymuvia			m_DBVitesses;               // Liste des vitesses
        SQLSymuvia			m_DBVersion;                // Version de la base de donn�es

        SQLSymuvia			m_DBPuissances;             // Liste des puissances*/

        double				m_dbFactPuis;

        bool                m_bLoaded;                  // permet de savoir si la connection � la BDD a �t� charg�e (pour ne pas le refaire
                                                        // � la d�s�rialisation si la connection est d�j� effectu�e)

        Reseau              *m_pNetwork;


public:
        static const int Nb_Octaves;          // nombre d'octaves pris en compte dans la simulation
        static const std::string nom_octave[8]; // tableau contenant les nom des octaves (sert en fait pour l'enregistrement des calcul acoustic au format MatFile)

        Loi_emission();         // constructeur
        Loi_emission(Reseau * pNetwork); // constructeur
        ~Loi_emission();        // destructeur

        void Init();

        std::string     GetVersion();

        bool    LoadDatabase();     // Charge les donn�es de la base

        void    CalculEmissionBruit(double*, double, double, std::string, std::string);        // fonction principale
                // calcul l'emissionde bruti en fonction de param�tres pass�s et des donn�es de la base de donn�es
                // vitesse,, acc�l�ration, nom du revetement, type du vehicule et nom du vehicule


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif


