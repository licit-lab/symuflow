#pragma once
#ifndef SQLNetworkExporterH
#define SQLNetworkExporterH

#include <string>
#include <vector>
#include <map>

class Reseau;
class Connexion;
struct Point;

class SQLNetworkExporter {

public:
    SQLNetworkExporter();
    virtual ~SQLNetworkExporter();

    // �criture d'un script SQL de cr�ation du r�seau dans une base tempus
    bool writeSQL(Reseau * pNetwork);

    // Accesseurs
    const std::map<std::string, std::pair<int,int> > & getTPTypes();

private:

    // Construction de la cha�ne WKT de la polyligne pass�e en param�tres
    std::string GetWKTFromPoints(const std::vector<Point> & lstPoints);

    // Construction de la cha�ne WKT du point correspondant � la connexion pass�e en param�tres
    std::string GetWKTFromConnection(Connexion * pConnexion, Reseau * pNetwork);

private:
    // association entre les noms des types de v�hicules de type TEC et les identifiants en BDD et BTFS associ�s
    std::map<std::string, std::pair<int,int> > m_LstTPTypes;

};

#endif // SQLNetworkExporterH