import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#'root://cmsxrootd.fnal.gov//store/hidata/XeXeRun2017/HIMinimumBias1/AOD/PromptReco-v1/000/304/906/00000/CEF3F710-BDB1-E711-9652-02163E019BB5.root'
'root://cmsxrootd.fnal.gov//store/hidata/XeXeRun2017/HIMinimumBias2/AOD/13Dec2017-v1/80000/009CE5A0-A8FA-E711-9252-3417EBE2E9BC.root'
    )
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '94X_dataRun2_ReReco_EOY17_v2', '')

#MB triggers
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack = process.hltHighLevel.clone()
process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack.HLTPaths = ["HLT_HIL1MinimumBiasHF_OR_SinglePixelTrack_part*"]
process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack.andOr = cms.bool(True)
process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack.throw = cms.bool(False)

#Event Selections:
#process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
#primaryVertexFilter
process.primaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlinePrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2 && tracksSize >= 2"),
    filter = cms.bool(True), # otherwise it won't filter the events
)
#BeamScrapingFilter
process.NoScraping = cms.EDFilter("FilterOutScraping",
 applyfilter = cms.untracked.bool(True),
 debugOn = cms.untracked.bool(False),
 numtrack = cms.untracked.uint32(10),
 thresh = cms.untracked.double(0.25)
)
#phfCoincFilter3
process.load("HeavyIonsAnalysis.Configuration.hfCoincFilter_cff")
#all the event selections
process.eventSelections = cms.Sequence(
        process.primaryVertexFilter
        + process.NoScraping
        + process.hfCoincFilter3
)

#Centrality
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
   cms.PSet(record = cms.string("HeavyIonRcd"),
      tag = cms.string("CentralityTable_HFtowers200_DataXeXe_eff95_run2v941x02_offline"),
      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
      label = cms.untracked.string("HFtowersEPOSLHC")
   ),
])

#process.load('RecoHI.HiCentralityAlgos.HiCentrality_cfi')
#process.hiCentrality.produceHFhits = False
#process.hiCentrality.produceHFtowers = False
#process.hiCentrality.produceEcalhits = False
#process.hiCentrality.produceZDChits = False
#process.hiCentrality.produceETmidRapidity = False
#process.hiCentrality.producePixelhits = False
#process.hiCentrality.produceTracks = False
#process.hiCentrality.producePixelTracks = False
#process.hiCentrality.reUseCentrality = False
#process.hiCentrality.srcReUse = cms.InputTag("hiCentrality","","RECO")
#process.hiCentrality.srcTracks = cms.InputTag("generalTracks")
#process.hiCentrality.srcVertex = cms.InputTag("offlinePrimaryVertices")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.centralityBin.nonDefaultGlauberModel = cms.string("EPOSLHC")

#output
process.TFileService = cms.Service("TFileService",
                                  fileName=cms.string("flowXeXe_setupTest.root")
)

process.demo = cms.EDAnalyzer('XeXeAnalysis',
   CentralitySrc    = cms.InputTag("hiCentrality"),
   CentralityBinSrc = cms.InputTag("centralityBin","HFtowers"),
   srcTracks = cms.InputTag("generalTracks"),
   srcVertex= cms.InputTag("offlinePrimaryVertices"),
   srcTower= cms.InputTag("towerMaker"),
   UseQuality = cms.bool(True),
   TrackQuality = cms.string('highPurity'),
   vertexZMaxCut = cms.double(15.0),
   trackPtMinCut = cms.double(0.3),
   trackPtMaxCut = cms.double(100.0),
   trackEtaCut = cms.double(2.4),
   ptErrCut = cms.double(0.1),
   dzRelCut = cms.double(3.0),
   dxyRelCut = cms.double(3.0)
)

#process.p = cms.Path(process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack * process.eventSelections * process.hiCentrality * process.centralityBin * process.demo)
process.p = cms.Path(process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack * process.eventSelections * process.centralityBin * process.demo)


