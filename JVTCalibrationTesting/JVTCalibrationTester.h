#ifndef JVTCalibrationTesting_JVTCalibrationTester_H
#define JVTCalibrationTesting_JVTCalibrationTester_H

#include "xAODAnaHelpers/Algorithm.h"

// unique pointers
#include <memory>

// EDM includes
#include "JetJvtEfficiency/JetJvtEfficiency.h"

class JVTCalibrationTester : public xAH::Algorithm
{
public:
  /** the working point for the JetJvtCalibration tool (Loose, Medium*, Tight). * = Default **/
  std::string m_workingPoint = "Medium";
  /** the configuration root file to use for calculating scale factors **/
  std::string m_sfFile = "JetJvtEfficiency/JvtSFFile.root";
  /** the input jet collection to use **/
  std::string m_inContainerName = "AntiKt4EMTopoJets";

private:
  /** pointer to the JetJvtEfficiency tool instance **/
  // note: should be called CP::JetJvtEfficiencyTool instead
  std::unique_ptr<CP::JetJvtEfficiency> m_JetJvtEfficiency; //!

public:
  // this is a standard constructor
  JVTCalibrationTester (std::string className = "JVTCalibrationTester");

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(JVTCalibrationTester, 1);
};

#endif
