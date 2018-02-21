import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(200) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#'/store/himc/HINXeXeFall17GS/XeXeMinBias_5p44TeV-AMPT-withmelt/GEN-SIM/93X_mc2017_realistic_v3-v1/10000/0019B4F8-F0F5-E711-BC8D-001E67E6F841.root'
'/store/user/gsfs/XeXeMinBias_5p44TeV-ReggeGribovPartonMC_EposLHC/RECODEBUG_Full_20180119/180121_124727/0000/step3_MinBias_XeXe_9_4_RAW2DIGI_L1Reco_RECO_1.root'
    )
)

process.TFileService = cms.Service('TFileService',
                                   fileName = cms.string('outfile_epos.root')
                                   )

process.demo = cms.EDAnalyzer('MCAnalyzer',
  HepMCCollection = cms.InputTag("generatorSmeared"),
  GenParticles = cms.untracked.InputTag("genParticles"),
  srcTower= cms.InputTag("towerMaker")
)


process.p = cms.Path(process.demo)

