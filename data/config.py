from xAH_config import xAH_config
c = xAH_config()

import itertools

jets = "AntiKt4EMTopoJets"
workingPoint = "Medium" # Loose, Medium, Tight
systematicName = "Nominal" # All, Nominal, JvtEfficiencyDown, JvtEfficiencyUp

''' Set up all the algorithms '''
c.setalg("BasicEventSelection", {"m_name": "BasicEventSelection",
                                 "m_applyGRLCut": False,
                                 "m_cleanPowheg": False,
                                 "m_doPUreweighting": False,
                                 "m_applyPrimaryVertexCut": False,
                                 "m_applyEventCleaningCut": False,
                                 "m_applyCoreFlagsCut": False,
                                 "m_applyTriggerCut": False,
                                 "m_useMetaData": False,
                                 "m_checkDuplicatesData": False,
                                 "m_checkDuplicatesMC": False})

selectedJets = "{0:s}Selected".format(jets)

c.setalg("JetSelector", {"m_name": "Select{0:s}".format(jets),
                         "m_inContainerName": jets,
                         "m_outContainerName": selectedJets,
                         "m_decorateSelectedObjects": False,
                         "m_createSelectedContainer": True,
                         "m_cleanJets": False,
                         "m_pT_min": 20.e3,
                         "m_pT_max": 60.e3})

c.setalg("JVTCalibrationTester", {"m_name": "TestJVTCalibrations",
                                  "m_inContainerName": selectedJets,
                                  "m_workingPoint": workingPoint,
                                  "m_systName": systematicName,
                                  "m_debug": True})
