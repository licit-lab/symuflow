#include "stdafx.h"
#include "DocTrafic.h"

#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// S�rialisation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template void DocTrafic::serialize(boost::archive::xml_woarchive & ar, const unsigned int version);
template void DocTrafic::serialize(boost::archive::xml_wiarchive & ar, const unsigned int version);

template<class Archive>
void DocTrafic::serialize(Archive & ar, const unsigned int version)
{
    // rien � sauvegarder dans cette classe pure virtual
}