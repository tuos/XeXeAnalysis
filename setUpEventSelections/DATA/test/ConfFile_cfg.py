import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20) )
#process.MessageLogger.cerr.FwkReport.reportEvery = 100
#process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
'root://cmsxrootd.fnal.gov//store/hidata/XeXeRun2017/HIMinimumBias1/AOD/PromptReco-v1/000/304/906/00000/CEF3F710-BDB1-E711-9652-02163E019BB5.root'
    )
)

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '92X_dataRun2_Prompt_Candidate_forXeXe_v1', '')

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
#phfCoincFilter2
process.load("HeavyIonsAnalysis.Configuration.hfCoincFilter_cff")
process.hfPosFilter2 = process.hfPosFilter.clone(minNumber=cms.uint32(2))
process.hfNegFilter2 = process.hfNegFilter.clone(minNumber=cms.uint32(2))
process.hfCoincFilter2 = cms.Sequence(
    process.towersAboveThreshold 
    + process.hfPosTowers 
    + process.hfNegTowers 
    + process.hfPosFilter2 
    + process.hfNegFilter2
)
#all the event selections
process.eventSelections = cms.Sequence(
        process.primaryVertexFilter
        + process.NoScraping
        + process.hfCoincFilter2
)

#Centrality
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
   cms.PSet(record = cms.string("HeavyIonRcd"),
      tag = cms.string("CentralityTable_HFtowers200_DataXeXe_eff942_run2v9313x02_offline"),
      connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
      label = cms.untracked.string("HFtowersCymbal5Ev8")
   ),
])
process.load('RecoHI.HiCentralityAlgos.HiCentrality_cfi')
process.hiCentrality.produceHFhits = False
process.hiCentrality.produceHFtowers = True
process.hiCentrality.produceEcalhits = False
process.hiCentrality.produceZDChits = False
process.hiCentrality.produceETmidRapidity = True
process.hiCentrality.producePixelhits = False
process.hiCentrality.produceTracks = True
process.hiCentrality.producePixelTracks = False
process.hiCentrality.reUseCentrality = False
process.hiCentrality.srcTracks = cms.InputTag("generalTracks")
process.hiCentrality.srcVertex = cms.InputTag("offlinePrimaryVertices")
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")
process.centralityBin.nonDefaultGlauberModel = cms.string("Cymbal5Ev8")

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

process.p = cms.Path(process.hltHIL1MinimumBiasHF_OR_SinglePixelTrack * process.eventSelections * process.hiCentrality * process.centralityBin * process.demo)

