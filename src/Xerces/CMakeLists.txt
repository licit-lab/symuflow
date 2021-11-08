set(XERCES_HEADERS
  DOMDocumentSymuvia.h
  DOMLSSerializerSymu.hpp
  MemoryManagerSymuvia.hpp
  XMLReaderSymuvia.h
  XMLUtil.h
  XercesString.h)


set(XERCES_SRC
  DOMDocumentSymuvia.cpp
  DOMLSSerializerSymu.cpp
  MemoryManagerSymuvia.cpp
  XMLReaderSymuvia.cpp
  XMLUtil.cpp
  XercesString.cpp)

appendingfiles(SYMUFLOW_SRC XERCES_SRC)

install(FILES ${XERCES_HEADERS} DESTINATION include/SymuFlow/Xerces)
