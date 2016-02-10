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
#include "xAODAnaHelpers/HelperFunctions.h"

// this is needed to distribute the algorithm to the workers
ClassImp(JVTCalibrationTester)



JVTCalibrationTester :: JVTCalibrationTester (std::string className) :
  Algorithm(className),
  m_JetJvtEfficiency(CxxUtils::make_unique<CP::JetJvtEfficiency>("JetJvtEfficiency"))
{ }

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
  Info("initialize()", "Attempting to configure tool with:\n\tWorkingPoint\t%s\n\tSFFile\t\t%s", m_workingPoint.c_str(), m_sfFile.c_str());
  RETURN_CHECK("JVTCalibrationTester::initialize()", m_JetJvtEfficiency->setProperty("WorkingPoint",m_workingPoint), "Could not set WorkingPoint.");
  RETURN_CHECK("JVTCalibrationTester:initialize()", m_JetJvtEfficiency->setProperty("SFFile",m_sfFile), "Could not set SFFile.");
  RETURN_CHECK("JVTCalibrationTester:initialize()", m_JetJvtEfficiency->initialize(), "Could not initialize the JetJvtEfficiency tool.");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: execute ()
{
  if(m_debug) Info("execute()", "Looking at an event...");
  m_event_totalJets = 0;
  m_event_totalJetsPassed = 0;

  // retrieve input jets
  const xAOD::JetContainer* jets(nullptr);
  RETURN_CHECK("JVTCalibrationTester::execute()", HelperFunctions::retrieve(jets, m_inContainerName, m_event, m_store, m_verbose) ,"Could not retrieve input jet collection.");

  // define how we pass JVT
  auto passJVT = [](const xAOD::Jet& jet, CP::JetJvtEfficiency& jvt_tool) -> bool { return (jet.pt()>60e3 || std::fabs(jet.eta())>2.4 || jvt_tool.passesJvtCut(jet)); };

  // select jets that pass JVT first
  ConstDataVector<xAOD::JetContainer> selected_jets = ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
  for(const auto& jet: *jets){
    // count total number of jets we look at
    m_event_totalJets++;
    m_totalJets++;
    // figure out if we pass or not
    bool pass = passJVT(*jet, *m_JetJvtEfficiency);
    if(m_debug) Info("execute()", "\t%s: Jet with pT %0.4f GeV, eta %0.4f", pass?"PASS":"FAIL", jet->pt()/1e3, std::fabs(jet->eta()));
    if(pass) selected_jets.push_back(jet);
  }

  // compute SF for jets that passed JVT
  for(const auto& jet: selected_jets){
    // count total number of jets that pass JVT
    m_event_totalJetsPassed++;
    m_totalJetsPassed++;
    // get the scale factor and report it
    float scaleFactor(-999.);
    m_JetJvtEfficiency->getEfficiencyScaleFactor(*jet,scaleFactor);
    if(m_debug) Info("execute()", "\tJet with pT %0.4f GeV has scale factor %0.4f", jet->pt()/1e3, scaleFactor);
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JVTCalibrationTester :: postExecute () {
  if(m_debug) Info("execute()", "\tEfficiency of JVT: %0.4f (per-event)", m_event_totalJetsPassed/m_event_totalJets);
  return EL::StatusCode::SUCCESS;
}
EL::StatusCode JVTCalibrationTester :: finalize () {
  if(m_debug) Info("execute()", "Efficiency of JVT: %0.4f (total)", m_totalJetsPassed/m_totalJets);
  return EL::StatusCode::SUCCESS;
}
EL::StatusCode JVTCalibrationTester :: histFinalize () { return EL::StatusCode::SUCCESS; }
