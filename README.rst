JVT Calibration Testing
=======================

Testing:

- scale factors
- documentation
- that it compiles

See `the JVT twiki <https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JVTCalibration>`_ for more information.

Installing
----------

To install, you will need various dependencies and RootCore::

  rcSetup Base,2.3.41
  git clone https://github.com/UCATLAS/xAODAnaHelpers
  git clone https://github.com/kratsg/JVTCalibrationTesting
  cd xAODAnaHelpers && git checkout 7d8d9f && cd -
  rc checkout_pkg atlasoff/Reconstruction/Jet/JetJvtEfficiency
  rc find_packages
  rc compile

Contributors
============

- Giordon Stark
