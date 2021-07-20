#ifndef _REPMOTIF_H__
#define _REPMOTIF_H__

class SymuViaTripNode;
class CMotif;

namespace boost {
    namespace serialization {
        class access;
    }
}

#include <vector>
#include <map>

//! Classe d�finissant un coefficient pour un couple de motifs de d�placement
class CMotifCoeff
{

public:
    //! Default constructor
    CMotifCoeff(void);
    //! Constructor
    CMotifCoeff(CMotif* pOrigin, CMotif* pDest, double dbCoeff);
    //! Destructor
    virtual ~CMotifCoeff(void);

public:
    CMotif * GetMotifOrigine() const;
    CMotif * GetMotifDestination() const;

    double getCoeff() const;
    void setCoeff(double dbCoeff);

private:

    CMotif * m_pOriginMotif;
    CMotif * m_pDestinationMotif;
    double   m_dbCoeff;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // S�rialisation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

//! Classe d�finissant une r�partition de motifs de d�placement
class CRepMotifDest
{

public:
    //! Default constructor
    CRepMotifDest(void);
    //! Destructor
    virtual ~CRepMotifDest(void);

public:

    void setDestination(SymuViaTripNode * pDest);
    SymuViaTripNode * getDestination() const;

    bool addMotifCoeff(const CMotifCoeff & motifCoeff);
    std::vector<CMotifCoeff> & getCoeffs();

private:

    // Destination (optionnelle) associ�e � la r�partition des motifs. Si NULL, la r�partition
    // est la m�me pour toutes les destinations
    SymuViaTripNode * m_pDestination;

    // Liste des couples de motifs OD pour lesquels on a un coefficient non nul
    std::vector<CMotifCoeff> m_LstCoeffs;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // S�rialisation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

//! Classe d�finissant une liste de r�partitions de motifs de d�placement pour diff�rentes destinations
class CRepMotif
{

public:
    //! Default constructor
    CRepMotif(void);
    //! Destructor
    virtual ~CRepMotif(void);

public:

    bool addRepMotifDest(const CRepMotifDest & repMotifDest);

    CRepMotifDest * getRepMotifDest(SymuViaTripNode * pDestination);

    std::map<SymuViaTripNode*, CRepMotifDest> & getMapRepMotifs() { return m_mapRepMotifs; }

private:

    // Map des r�partition des motifs en fonction de la destination
    std::map<SymuViaTripNode*, CRepMotifDest> m_mapRepMotifs;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // S�rialisation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif // _REPMOTIF_H__