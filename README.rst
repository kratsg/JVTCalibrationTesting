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
  rc checkout_pkg atlasoff/Reconstruction/Jet/JetJvtEfficiency/trunk
  rc find_packages
  rc compile

Running
-------

It is very simple to run this as we use ``xAH_run.py`` to manage that for us. For example, on UChicago's tier-3, I have a local file I can use, so I run::

  xAH_run.py --files /share/t3data3/kratsg/xAODs/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264_tid05419191_00/AOD.05419191._000003.pool.root.1 --config JVTCalibrationTesting/data/config.py --nevents 10 -f direct

which will run locally (``direct``) over 10 events with the configuration `config.py <data/config.py>`_.

Contributors
============

- Giordon Stark
