# CMakeLists.txt for the ROOT tutorials programs.
# Author: Pere Mato, 25/10/2010
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(tutorials)
find_package(ROOT REQUIRED)

if(DEFINED ROOT_SOURCE_DIR)  # Testing using the binary tree
  set(ROOT_root_CMD root.exe)
  if(NOT MSVC)  # Ignore environment on Windows
    set(ROOT_environ PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
                     ${ld_library_path}=${CMAKE_BINARY_DIR}/lib:$ENV{${ld_library_path}}
                     ROOTSYS=${CMAKE_BINARY_DIR}
                     PYTHONPATH=${CMAKE_BINARY_DIR}/lib:$ENV{PYTHONPATH})
  else()
    set(ROOT_environ ROOTSYS=${CMAKE_BINARY_DIR}
                     PYTHONPATH=${CMAKE_BINARY_DIR}/bin;$ENV{PYTHONPATH})
  endif()
else()                       # testing using an installation
  include(${ROOT_USE_FILE})
  if(DEFINED ROOT_CONFIG_EXECUTABLE) #---If ROOT was built with the classic configure/make---
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules)
    include(RootNewMacros)
    set(ROOT_root_CMD root.exe)
  endif()
  enable_testing()
endif()

#---Copy the CTestCustom.cmake file into the build directory--------
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

#---Provide a rootlogon.C file in the current build directory that
#   will affect the way we run all tutorials.
#   This overwrites the existing rootlogon.C and rootalias.C in the
#   tutorials directory which is copied to the build area.
#-------------------------------------------------------------------
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/rootlogon.C "{
  // Needed by ACLiC to use the current dicrectory for scratch area
  gSystem->SetBuildDir(\".\", kTRUE);
  gROOT->SetWebDisplay(\"batch\");
}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/rootalias.C "")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/rootlogoff.C "{}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.rootrc "
Proof.Sandbox: /tmp/proof
Rint.History:  .root_hist
")

#---Definition of the helper function--------------------------------
function(ROOT_ADD_TUTORIAL macrofile rc)
  string(REPLACE ".C" "" name ${macrofile})
  string(REPLACE "/" "-" name ${name})
  ROOT_ADD_TEST(tutorial-${name} COMMAND ${ROOT_root_CMD} -b -l -n -q ${CMAKE_CURRENT_SOURCE_DIR}/${macrofile}
                PASSRC ${rc} FAILREGEX "Error in" "error:" LABELS tutorial)
endfunction()

#---Tutorials disabled depending on the build components-------------
if(NOT ROOT_minuit2_FOUND)
  set(minuit2_veto fit/fit2dHist.C fit/fit2dHist.C
                   fit/fitCircle.C fit/minuit2FitBench2D.C
                   fit/minuit2FitBench2D.C fit/minuit2FitBench.C
                   fit/minuit2FitBench.C fit/minuit2GausFit.C
                   fit/minuit2GausFit.C fit/NumericalMinimization.C
                   fit/combinedFit.C fit/TestBinomial.C
                   fit/fitNormSum.C fit/vectorizedFit.C
                   tutorials/roostats/rs_bernsteinCorrection.C)
endif()

if(NOT ROOT_roofit_FOUND)
  set(roofit_veto  fit/RoofitDemo.C
                   roofit/*.C roostats/*.C histfactory/*.C
                   roofit/*.py)
endif()

if(NOT ROOT_unuran_FOUND)
  set(unuran_veto  math/testrandom.C unuran/unuranDemo.C unuran/unuranFoamTest.C
                   math/multidimSampling.C)
endif()

if(NOT ROOT_xml_FOUND)
  set(xml_veto  xml/*.C
                histfactory/*.C    # histfactory requires xml
                unfold/*.C)        # unfold requires xml
endif()

if(NOT XROOTD_FOUND)
  set(xrootd_veto dataframe/df101_h1Analysis.C
                  dataframe/df102_NanoAODDimuonAnalysis.C
                  dataframe/df103_NanoAODHiggsAnalysis.C)
endif()

# variables identifying the package must have the package name  in lower case (it corresponds to the CMake option name)
if(NOT ROOT_r_FOUND)
  set(r_veto  r/*.C)
endif()

# FIXME: Temporary workaround for runtime_cxxmodule
if (FIXME_TEMPORARILY_EXCLUDED_FOR_RUNTIME_CXXMODULES)
  set(runtime_cxxmodules_veto r/Function.C)
endif()

if(WIN32)
  set(histfactory_veto histfactory/*.C
                       roostats/StandardFrequentistDiscovery.C) # histfactory doesn't work on Windows
else()
  set(histfactory_veto histfactory/makeExample.C)
endif()


if(NOT ROOT_fitsio_FOUND)
  set(fitsio_veto  fitsio/*.C)
endif()

if(NOT ROOT_mathmore_FOUND)
  set(mathmore_veto
      math/quasirandom.C
      math/exampleMultiRoot.C
      math/Bessel.C
      math/LegendreAssoc.C
      math/Legendre.C
      math/mathmoreIntegration.C
      math/tStudent.C
      math/normalDist.C)
endif()

if(NOT ROOT_fftw3_FOUND)
  set(fftw3_veto roofit/rf208_convolution.C
                 roofit/rf210_angularconv.C
                 roofit/rf211_paramconv.C
                 roofit/rf512_wsfactory_oper.C
                 fft/FFT.C
                 fit/fitConvolution.C)
endif()

if(NOT ROOT_opengl_FOUND)
  set(opengl_veto tree/staff.C
                  gl/*.C)
endif()

if(NOT GRAPHVIZ_FOUND)
  set(gviz_veto graphs/graphstruct.C)
endif()

if(NOT TBB_FOUND AND NOT builtin_tbb)
  set(tbb_veto  multicore/mtbb*.C)
endif()

if(NOT ROOT_imt_FOUND)
  set(imt_veto multicore/imt*.C multicore/mt*.C)
else()
  if(MSVC)
    #---Multiproc is not supported on Windows
    set(imt_veto ${imt_veto} multicore/mp*.C multicore/mtbb201_parallelHistoFill.C)
  endif()
endif()

if(ROOT_CLASSIC_BUILD)
  set(classic_veto multicore/mp104_*.C multicore/mp105_*.C)
endif()

#---These ones requires a display to run-----------------------------
set(gui_veto fit/fitpanel_playback.C
             cocoa/*.C
             geom/building.C geom/cheongwadae.C geom/geom*.C geom/lego.C geom/robot.C geom/south_gate.C geom/station*.C geom/tank.C
             gl/glViewerExercise.C gl/glViewerLOD.C gl/gviz3d.C gl/nucleus.C gl/viewer3DLocal.C gl/viewer3DMaster.C
             gui/*.C
             hist/exec1.C
             hist/exec2.C
             hist/tprofile2polyRealistic.C
             hist/tprofile2polyRealisticModuleError.C
             image/*.C
             graphics/psview.C graphics/gtime.C
             graphics/graph_edit_playback.C
             roostats/ModelInspector.C
             tree/tvdemo.C
             eve/*.C)

if (NOT ROOT_tmva_FOUND)
  set(tmva_veto tmva/*.C tmva/*.py tmva/envelope/*.C tmva/keras/*.C tmva/keras/*.py)
else()
  #---These do not need to run for TMVA
  set(tmva_veto tmva/createData.C)
endif()

if (NOT ROOT_pythia6_FOUND)
  set(pythia_veto pythia/pythiaExample.C)
endif()
if (NOT ROOT_pythia8_FOUND)
  set(pythia_veto ${pythia_veto} pythia/pythia8.C)
else()
  if("$ENV{PYTHIA8}" STREQUAL "")
    get_filename_component(pythia8dir "${PYTHIA8_INCLUDE_DIR}" DIRECTORY)
    list(APPEND ROOT_environ PYTHIA8=${pythia8dir})
  endif()
  if("$ENV{PYTHIA8DATA}" STREQUAL "" AND PYTHIA8_DATA)
    list(APPEND ROOT_environ PYTHIA8DATA=${PYTHIA8_DATA})
  endif()
endif()

if(root7)
  set(root7_veto dataframe/df013_InspectAnalysis.C
                 v7/fitpanel.cxx
                 v7/fitpanel6.cxx
      )
endif()

if(NOT root7)
  set(root7_veto v7/ntuple/ntpl001_staff.C
                 v7/ntuple/ntpl002_vector.C
                 v7/ntuple/ntpl003_lhcbOpenData.C
                 v7/ntuple/ntpl004_dimuon.C)
endif()

if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  set(bits32_veto dataframe/*.C graphs/timeSeriesFrom*.C v7/ntuple/ntpl004_dimuon.C)
endif()

#---These ones are disabled !!! ------------------------------------
set(extra_veto
  htmlex.C
  rootalias.C          # Helper macro
  rootlogon.C          # Helper macro
  rootlogoff.C         # Helper macro
  rootmarks.C          # Instrumentation. Not a standalone tutorial
  memstat/memstatExample.C   # Instrumentation. Not a standalone tutorial
  multicore/mp_H1_lambdas.C # not a tutorial; used by mp104_processH1.C et al.
  html/*.C
  net/*.C
  proof/*.C
  sql/*.C
  tree/hsimpleProxy.C # A driver uses this macro which cannot be executed directly
  tree/tree0.C
  tree/tree2a.C
  tree/tree4.C
  roostats/rs401d_FeldmanCousins.C  # Takes too much time
  histfactory/ModifyInterpolation.C
  tree/copytree2.C
  tree/copytree3.C
  tree/copytree.C
  tree/h1analysis*.C # these are not a tutorial but classes used in run_h1analysis.C
  tree/h1chain.C
  http/*.C
  eve7/*.C
  r/rootlogon.C)

set(all_veto hsimple.C
             geom/geometry.C
             ${extra_veto}
             ${gui_veto}
             ${minuit2_veto}
             ${roofit_veto}
             ${unuran_veto}
             ${xml_veto}
             ${fitsio_veto}
             ${tmva_veto}
             ${mathmore_veto}
             ${fftw3_veto}
             ${opengl_veto}
             ${gviz_veto}
             ${r_veto}
             ${runtime_cxxmodules_veto}
             ${histfactory_veto}
             ${tbb_veto}
             ${imt_veto}
             ${classic_veto}
             ${pythia_veto}
             ${root7_veto}
             ${bits32_veto}
             ${xrootd_veto}
             )

file(GLOB_RECURSE tutorials RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.C)
if(root7 AND webgui)
  file(GLOB_RECURSE tutorials_v7 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} v7/*.cxx)
  list(APPEND tutorials ${tutorials_v7})
endif()
file(GLOB tutorials_veto RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${all_veto})

list(LENGTH tutorials total)
list(LENGTH tutorials_veto veto)
message(STATUS "${veto}/${total} tutorials have been vetoed for various reasons")

list(REMOVE_ITEM tutorials ${tutorials_veto})

#---Special return code------------------------------------------------
set(returncode_1 fit/fit2a.C fit/graph2dfit.C
                 graphics/arrow.C
                 graphics/crown.C graphics/diamond.C
                 graphics/earth.C graphics/ellipse.C
                 graphics/pavetext.C
                 graphics/tmathtext.C graphics/tmathtext2.C
                 graphs/timeonaxis.C
                 graphs/timeonaxis2.C
                 graphs/timeonaxis3.C
                 graphs/exclusiongraph.C
                 graphs/graphstruct.C
                 hist/ContourList.C
                 hist/hstack.C
                 hist/hbars.C
                 hist/th2polyBoxes.C
                 hist/statsEditing.C
                 hist/cumulative.C
                 hist/hlabels1.C
                 hist/hlabels2.C
                 tree/h1analysis.C
                 math/chi2test.C
                 r/SimpleFitting.C)
#---Dependencies------------------------------------------------------
set(unfold-testUnfold5d-depends tutorial-unfold-testUnfold5c)
set(unfold-testUnfold5c-depends tutorial-unfold-testUnfold5b)
set(unfold-testUnfold5b-depends tutorial-unfold-testUnfold5a)
set(unfold-testUnfold7d-depends tutorial-unfold-testUnfold7c)
set(unfold-testUnfold7c-depends tutorial-unfold-testUnfold7b)
set(unfold-testUnfold7b-depends tutorial-unfold-testUnfold7a)
set(xml-xmlreadfile-depends tutorial-xml-xmlnewfile)
set(roofit-rf503_wspaceread-depends tutorial-roofit-rf502_wspacewrite)
set(histfactory-example-depends tutorial-roostats-CreateExampleFile)
set(io-readCode-depends tutorial-io-importCode)
set(fit-fit1-depends tutorial-hist-fillrandom)
set(fit-myfit-depends tutorial-fit-fitslicesy)
set(foam-foam_demopers-depends tutorial-foam-foam_demo)
set(tree-staff-depends  tutorial-tree-cernbuild)
set(tree-cernstaff-depends  tutorial-tree-cernbuild)
set(hist-hbars-depends  tutorial-tree-cernbuild)
set(benchmarks-depends tutorial-hsimple
                       tutorial-fit-fit1
                       tutorial-fit-myfit
                       tutorial-hist-h1draw
                       tutorial-hist-FirstContour
                       tutorial-geom-na49view
                       tutorial-tree-ntuple1
                       tutorial-tree-spider
                       tutorial-io-hadd
                       tutorial-io-loopdir
                       tutorial-io-copyFiles)
set(geom-na49view-depends tutorial-geom-geometry)
set(multicore-mt102_readNtuplesFillHistosAndFit-depends tutorial-multicore-mt101_fillNtuples)
set(multicore-mp102_readNtuplesFillHistosAndFit-depends tutorial-multicore-mp101_fillNtuples)
set(multicore-mp105_processEntryList-depends tutorial-multicore-mp104_processH1)

#--many roostats tutorials depending on having creating the file first with histfactory and example_combined_GaussExample_model.root
foreach(tname  ModelInspector OneSidedFrequentistUpperLimitWithBands StandardBayesianMCMCDemo StandardBayesianNumericalDemo
               StandardFeldmanCousinsDemo  StandardFrequentistDiscovery StandardHistFactoryPlotsWithCategories StandardHypoTestDemo
               StandardHypoTestInvDemo StandardProfileInspectorDemo StandardTestStatDistributionDemo
               OneSidedFrequentistUpperLimitWithBands TwoSidedFrequentistUpperLimitWithBands StandardProfileLikelihoodDemo)
  set(roostats-${tname}-depends tutorial-roostats-CreateExampleFile)
endforeach()

#--dependency for TMVA tutorials
set (tmva-TMVAClassificationApplication-depends tutorial-tmva-TMVAClassification)
set (tmva-TMVAClassificationCategory-depends tutorial-tmva-TMVAClassification)
set (tmva-TMVAClassificationCategoryApplication-depends tutorial-tmva-TMVAClassificationCategory)
set (tmva-TMVAMulticlass-depends tutorial-tmva-TMVAMultipleBackgroundExample)
set (tmva-TMVAMulticlassApplication-depends tutorial-tmva-TMVAMulticlass)
set (tmva-TMVARegressionApplication-depends tutorial-tmva-TMVARegression)


#---Loop over all tutorials and define the corresponding test---------

#---Define the primordial tutorials-----------------------------------
ROOT_ADD_TEST(tutorial-hsimple COMMAND ${ROOT_root_CMD} -b -l -n -q ${CMAKE_CURRENT_SOURCE_DIR}/hsimple.C
                PASSRC 255 FAILREGEX "Error in" "error:" LABELS tutorial)
ROOT_ADD_TEST(tutorial-geom-geometry COMMAND ${ROOT_root_CMD} -b -l -n -q ${CMAKE_CURRENT_SOURCE_DIR}/geom/geometry.C
                FAILREGEX "Error in" "error:" LABELS tutorial)

#---Loop over all tutorials and define the corresponding test---------
foreach(t ${tutorials})
  list(FIND returncode_1 ${t} index)
  if(index EQUAL -1)
    set(rc 0)
  else()
    set(rc 255)
  endif()
  string(REPLACE ".C" "" tname ${t})
  string(REPLACE "/" "-" tname ${tname})

   # These tests on ARM64 need much more than 20 minutes - increase the timeout
   if(ROOT_ARCHITECTURE MATCHES arm64)
     set(thisTestTimeout 2400) # 40m
   else()
     set(thisTestTimeout 1200) # 20m
   endif()

  ROOT_ADD_TEST(tutorial-${tname}
                COMMAND ${ROOT_root_CMD} -b -l -q ${CMAKE_CURRENT_SOURCE_DIR}/${t}${${tname}-aclic}
                PASSRC ${rc} FAILREGEX "Error in <" ": error:" "segmentation violation" "FROM HESSE     STATUS=FAILED"
                LABELS tutorial
                DEPENDS tutorial-hsimple ${${tname}-depends}
                ENVIRONMENT ${ROOT_environ}
                TIMEOUT ${thisTestTimeout})
endforeach()


#---Python tutorials-----------------------------------------------------
if(ROOT_python_FOUND)
  find_package(PythonInterp REQUIRED)

  file(GLOB pytutorials RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */*.py)
  set(pyveto pyroot/demo.py         # requires GUI
             pyroot/fit1_py.py      # not a tutorial
             pyroot/gui_ex.py       # requires GUI
             pyroot/mrt.py          # not really a tutorial
             pyroot/na49geomfile.py # ????
             pyroot/na49visible.py  # ????
             pyroot/parse_CSV_file_with_TTree_ReadStream.py # not a tutorial
             pyroot/numberEntry.py  # requires GUI
             histfactory/example.py # not a tutorial
             histfactory/makeQuickModel.py # not a tutorial
             eve/lineset.py         # requires GUI
             sql/sqlcreatedb.py     # same as the C++ case
             sql/sqlfilldb.py       # same as the C++ case
             sql/sqlselect.py       # same as the C++ case
             )
  if(NOT ROOT_mathmore_FOUND)
    list(APPEND pyveto math/Legendre.py)
    list(APPEND pyveto math/Bessel.py)
    list(APPEND pyveto math/tStudent.py)
  endif()

 if(NOT XROOTD_FOUND)
   list(APPEND pyveto dataframe/df102_NanoAODDimuonAnalysis.py)
 endif()

  list(REMOVE_ITEM pytutorials ${pyveto})

  if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    set(bits32_veto_py dataframe/*.py)
    file(GLOB bits32_veto_py RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} dataframe/*.py)
    list(REMOVE_ITEM pytutorials ${bits32_veto_py})
    list(REMOVE_ITEM pytutorials pyroot/pyroot002_TTreeAsMatrix.py)
  endif()

  #---Python tutorials dependencies--------------------------------------
  set(pyroot-ntuple1-depends tutorial-pyroot-hsimple-py)
  set(pyroot-h1draw-depends tutorial-pyroot-hsimple-py)
  set(pyroot-benchmarks-depends tutorial-pyroot-hsimple-py
                                tutorial-pyroot-fit1-py
                                tutorial-pyroot-na49view-py
                                tutorial-pyroot-h1draw-py
                                tutorial-pyroot-ntuple1-py)
  set(pyroot-fit1-depends tutorial-pyroot-fillrandom-py)
  set(pyroot-na49view-depends tutorial-pyroot-geometry-py)
  set(roofit-rf104_classfactory-depends tutorial-roofit-rf104_classfactory) #Race condition

  #---Tutorials expected to fail in PyROOT experimental
  set(pyexp_fail tutorial-dataframe-df002_dataModel-py
                 tutorial-dataframe-df003_profiles-py
                 tutorial-dataframe-df010_trivialDataSource-py
                 tutorial-dataframe-df011_ROOTDataSource-py
                 tutorial-dataframe-df014_CSVDataSource-py
                 tutorial-dataframe-df016_vecOps-py
                 tutorial-dataframe-df017_vecOpsHEP-py
                 tutorial-dataframe-df026_AsNumpyArrays-py
                 tutorial-dataframe-df102_NanoAODDimuonAnalysis-py
                 tutorial-math-Legendre-py
                 tutorial-math-tStudent-py
                 tutorial-pyroot-benchmarks-py
                 tutorial-pyroot-geometry-py
                 tutorial-pyroot-na49view-py
                 tutorial-pyroot-pyroot002_TTreeAsMatrix-py
                 tutorial-pyroot-shapes-py
                 tutorial-pyroot-staff-py
                 tutorial-roofit-rf106_plotdecoration-py
                 tutorial-roofit-rf315_projectpdf-py
                 tutorial-roofit-rf402_datahandling-py
                 tutorial-roofit-rf610_visualerror-py
                 tutorial-vecops-vo004_SortAndSelect-py
                 tutorial-vecops-vo005_Combinations-py)

  if(${PYTHON_VERSION_MAJOR} GREATER_EQUAL 3)
    # These tests only fail in Py3 with PyROOT experimental
    list(APPEND pyexp_fail tutorial-pyroot-hsimple-py)
  endif()

  foreach(t ${pytutorials})
    list(FIND returncode_1 ${t} index)
    if(index EQUAL -1)
      set(rc 0)
    else()
      set(rc 255)
    endif()

    string(REPLACE ".py" "" tname ${t})
    string(REPLACE "/" "-" tname ${tname})

    set(tutorial_name tutorial-${tname}-py)

    list(FIND pyexp_fail ${tutorial_name} index)
    if(index EQUAL -1)
      set(py_will_fail "")
    else()
      set(py_will_fail ${PYTESTS_WILLFAIL})
    endif()

    ROOT_ADD_TEST(${tutorial_name}
                COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${t} -b
                PASSRC ${rc} FAILREGEX "Error in" ": error:" "segmentation violation"
                LABELS tutorial
                DEPENDS ${${tname}-depends}
                ENVIRONMENT ${ROOT_environ}
                ${py_will_fail})
  endforeach()
endif()
