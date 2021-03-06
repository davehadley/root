############################################################################
# CMakeLists.txt file for building ROOT ntuple package
# @author Jakob Blomer CERN
############################################################################

if(NOT root7)
  return()
endif()

ROOT_STANDARD_LIBRARY_PACKAGE(ROOTNTuple
HEADERS
  ROOT/RColumn.hxx
  ROOT/RColumnElement.hxx
  ROOT/RColumnModel.hxx
  ROOT/REntry.hxx
  ROOT/RField.hxx
  ROOT/RFieldValue.hxx
  ROOT/RNTuple.hxx
  ROOT/RNTupleDescriptor.hxx
  ROOT/RNTupleModel.hxx
  ROOT/RNTupleUtil.hxx
  ROOT/RNTupleView.hxx
  ROOT/RPage.hxx
  ROOT/RPagePool.hxx
  ROOT/RPageStorage.hxx
  ROOT/RPageStorageRoot.hxx
SOURCES
  v7/src/RColumn.cxx
  v7/src/RField.cxx
  v7/src/REntry.cxx
  v7/src/RNTuple.cxx
  v7/src/RNTupleDescriptor.cxx
  v7/src/RNTupleModel.cxx
  v7/src/RPage.cxx
  v7/src/RPagePool.cxx
  v7/src/RPageStorage.cxx
  v7/src/RPageStorageRoot.cxx
LINKDEF
  LinkDef.h
DEPENDENCIES
  RIO
  ROOTVecOps
)

ROOT_ADD_TEST_SUBDIRECTORY(v7/test)

