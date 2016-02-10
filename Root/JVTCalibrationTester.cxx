#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <JVTCalibrationTesting/JVTCalibrationTester.h>

// make unique pointers
#include<CxxUtils/make_unique.h>

// RETURN_CHECK macro
#include <xAODAnaHelpers/tools/ReturnCheck.h>

// EDM includes
#include "xAODJet/JetContainer.h"

// this is needed to distribute the algorithm to the workers
ClassImp(JVTCalibrationTester)



JVTCalibrationTester :: JVTCalibrationTester (std::string className) : Algorithm(className) { }

EL::StatusCode JVTCalibrationTester :: setupJob (EL::Job& job)
{
  // tell everyone I want to run on xAODs
  job.useXAOD();
  xAOD::Init("JVTCalibrationTester").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: histInitialize ()
{
  // register me with xAODAnaHelpers to keep track of how many of me exist
  RETURN_CHECK("xAH::Algorithm::algInitialize()", xAH::Algorithm::algInitialize(), "");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JVTCalibrationTester :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }


EL::StatusCode JVTCalibrationTester :: initialize ()
{
  // grab pointers to TEvent and TStore objects
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // set up the JetJvtEfficiency tool
  if ( asg::ToolStore::contains<CP::JetJvtEfficiency>("JetJvtEfficiency") ) {
    m_JetJvtEfficiency = asg::ToolStore::get<CP::JetJvtEfficiency>("JetJvtEfficiency");
  } else {
    m_JetJvtEfficiency = CxxUtils::make_unique<CP::JetJvtEfficiency>("JetJvtEfficiency");
  }
  Info("initialize()", "Attempting to configure tool with:\n\tWorkingPoint\t%s\n\tSFFile\t\t\t%s", m_workingPoint.c_str(), m_sfFile.c_str());
  RETURN_CHECK("JVTCalibrationTester::initialize()", m_JetJvtEfficiency->setProperty("WorkingPoint",m_workingPoint), "Could not set WorkingPoint.");
  RETURN_CHECK("JVTCalibrationTester:initialize()", m_JetJvtEfficiency->setProperty("SFFile",m_sfFile), "Could not set SFFile.");
  RETURN_CHECK("JVTCalibrationTester:initialize()", m_JetJvtEfficiency->initialize(), "Could not initialize the JetJvtEfficiency tool.");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: execute ()
{
  // retrieve input jets
  const xAOD::JetContainer* jets(nullptr);
  RETURN_CHECK("JVTCalibrationTester::execute()", HelperFunctions::retrieve(jets, m_inContainerName, m_event, m_store, m_verbose) ,"Could not retrieve input jet collection.");

  // define how we pass JVT
  auto passJVT = [m_JetJvtEfficiency](const xAOD::Jet* jet) -> bool { return (jet->pt()>60e3 || std::fabs(jet->eta())>2.4 || m_JetJvtEfficiency->passesJvtCut(*jet)); };

  // select jets that pass JVT first
  ConstDataVector<xAOD::JetContainer> selected_jets = ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
  for(const auto& jet: *jets){
    bool pass = passJVT(jet);
    if(m_debug) Info("execute()", "%s: Jet with pT %0.2f GeV, eta %0.2f", pass?"PASS":"FAIL", jet->pt()/1e3, std::fabs(jet->eta()));
    if(pass) selected_jets.push_back(jet);
  }

  // compute SF for jets that passed JVT
  for(const auto& jet: selected_jets){
    float scaleFactor(-999.);
    m_JetJvtEfficiency->getEfficiencyScaleFactor(*jet,scaleFactor);
    if(m_debug) Info("execute()", "Jet with pT %0.2f GeV has scale factor %0.2f", jet->pt()/1e3, sf);
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JVTCalibrationTester :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JVTCalibrationTester :: histFinalize () { return EL::StatusCode::SUCCESS; }
