// -*- C++ -*-
//
// Package:    TestXeXeAnalysis/XeXeAnalysis
// Class:      XeXeAnalysis
// 
/**\class XeXeAnalysis XeXeAnalysis.cc TestXeXeAnalysis/XeXeAnalysis/plugins/XeXeAnalysis.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Shengquan Tuo
//         Created:  Thu, 19 Oct 2017 20:38:04 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HeavyIonEvent/interface/Centrality.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <TH1.h>
#include <TH1D.h>
#include <TH2D.h>

using namespace std;
using namespace edm;
using namespace reco;

   const int nCentBin = 9;
   double cBin[nCentBin+1]={0,10,20,30,40,50,60,70,80,100};

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class XeXeAnalysis : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit XeXeAnalysis(const edm::ParameterSet&);
      ~XeXeAnalysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
   edm::Service<TFileService> fs;
   edm::EDGetTokenT<reco::Centrality> CentralityTag_;
   edm::EDGetTokenT<int> CentralityBinTag_;
   edm::EDGetTokenT<TrackCollection> srcTracks_;
   edm::EDGetTokenT<VertexCollection> srcVertex_;
   edm::EDGetTokenT<CaloTowerCollection> srcTower_;
   bool useQuality_;
   reco::TrackBase::TrackQuality trackQuality_;
   double vertexZMaxCut_;
   double trackPtMinCut_;
   double trackPtMaxCut_;
   double trackEtaCut_;
   double ptErrCut_;
   double dzRelCut_;
   double dxyRelCut_;
   TH1D* vxHist;
   TH1D* vyHist;
   TH1D* vzHist;
   TH1D* generalTracksHist;
   TH1D* ptHist;
   TH1D* etaHist;
   TH1D* phiHist;
   TH1D* hHF;
   TH1D* hHFplus;
   TH1D* hHFminus;
   TH2D* hetaVSphi;
   TH1D* hdzOerr;
   TH1D* hd0Oerr;
   TH1D* hpterrOpt;
   TH1D* hCentralityBin;
   TH1D* hHFCentralityObject; 
   TH1D* hTrackCentralityObject; 
   TH1D* hTrackDistribution[nCentBin];
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
XeXeAnalysis::XeXeAnalysis(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   usesResource("TFileService");
edm::Service<TFileService> fs;
CentralityTag_ = consumes<reco::Centrality>(iConfig.getParameter<edm::InputTag>("CentralitySrc"));
CentralityBinTag_= consumes<int>(iConfig.getParameter<edm::InputTag>("CentralityBinSrc"));
srcTracks_ = consumes<TrackCollection>(iConfig.getParameter<edm::InputTag>("srcTracks"));
srcVertex_ = consumes<VertexCollection>(iConfig.getParameter<edm::InputTag>("srcVertex"));
srcTower_ = consumes<CaloTowerCollection>(iConfig.getParameter<edm::InputTag>("srcTower"));
useQuality_ = iConfig.getParameter<bool>("UseQuality");
trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("TrackQuality"));
vertexZMaxCut_ = iConfig.getParameter<double>("vertexZMaxCut");
trackPtMinCut_ = iConfig.getParameter<double>("trackPtMinCut");
trackPtMaxCut_ = iConfig.getParameter<double>("trackPtMaxCut");
trackEtaCut_ = iConfig.getParameter<double>("trackEtaCut");
ptErrCut_ = iConfig.getParameter<double>("ptErrCut");
dzRelCut_ = iConfig.getParameter<double>("dzRelCut");
dxyRelCut_ = iConfig.getParameter<double>("dxyRelCut");
TH1::SetDefaultSumw2();
vxHist = fs->make<TH1D>("vxhist","offlinePrimaryVertices Vx Distribution", 100, -0.5, 0.5);
vyHist = fs->make<TH1D>("vyhist","offlinePrimaryVertices Vy Distribution", 100, -0.5, 0.5);
vzHist = fs->make<TH1D>("vzhist","offlinePrimaryVertices Vz Distribution", 120, -30., 30.);
generalTracksHist = fs->make<TH1D>("generaltrackshist","generalTracks Distribution", 400, 0., 4000.);
ptHist = fs->make<TH1D>("pthist","generalTracks pT Distribution", 100, 0., 10.);
etaHist = fs->make<TH1D>("etahist","generalTracks eta Distribution", 120, -3.0, 3.0);
phiHist = fs->make<TH1D>("phihist","generalTracks phi Distribution", 160, -4.0, 4.0);
hHF = fs->make<TH1D>("hfhist","HF Distribution", 400, 0., 4000.);
hHFplus = fs->make<TH1D>("hfplushist","HF Plus Distribution", 200, 0., 2000.);
hHFminus = fs->make<TH1D>("hfminushist","HF Minus Distribution", 200, 0., 2000.);
hetaVSphi = fs->make<TH2D>("hetavsphi","generalTracks phi VS eta", 120, -3.0, 3.0, 160, -4.0, 4.0);
hdzOerr = fs->make<TH1D>("hdzoerr","dz/dzErr", 100, 0., 10.);
hd0Oerr = fs->make<TH1D>("hd0oerr","d0/d0Err", 100, 0., 10.);
hpterrOpt = fs->make<TH1D>("hpterropt","p_{T}Err/p_{T}", 100, 0., 0.5);
hCentralityBin = fs->make<TH1D>("hcentralitybin","XeXe centrality bin distribution", 201, 0., 201.0);
hHFCentralityObject = fs->make<TH1D>("hhfcentralityobject","HF Distribution from centrality object", 400, 0., 4000.);
hTrackCentralityObject = fs->make<TH1D>("htrackcentralityobject","NtrackOffline Distribution from centrality object", 400, 0., 4000.);
 for(int icent=0; icent<nCentBin; icent++){ 
   hTrackDistribution[icent] = fs->make<TH1D>(Form("htrackdistribution_icent%d",icent),Form("Track Distribution in centrality bin%d",icent),400,0,4000);
 }

}


XeXeAnalysis::~XeXeAnalysis()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
XeXeAnalysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
//Centrality
    edm::Handle<int> cbin_;
    iEvent.getByToken(CentralityBinTag_,cbin_);
    int hiBin = *cbin_;
    hCentralityBin->Fill(hiBin);
    edm::Handle<reco::Centrality> centrality;
    iEvent.getByToken(CentralityTag_, centrality);
    hHFCentralityObject->Fill(centrality->EtHFtowerSum());
    hTrackCentralityObject->Fill(centrality->Ntracks());
    int ibin=-1;
    for(int j=0;j<nCentBin;j++)
      if(hiBin>=2*cBin[j]&&hiBin<2*cBin[j+1])
        ibin=j;
    if(ibin<0 || ibin==nCentBin) return;
    hTrackDistribution[ibin]->Fill(centrality->Ntracks());

//HF
    double etHFtowerSumPlus = 0;
    double etHFtowerSumMinus = 0;
    double etHFtowerSum = 0;
    Handle<CaloTowerCollection> towers;
    iEvent.getByToken(srcTower_,towers);
    for( size_t i = 0; i<towers->size(); ++ i){
       const CaloTower & tower = (*towers)[ i ];
       double eta = tower.eta();
       bool isHF = tower.ietaAbs() > 29;
          if(isHF && eta > 0){
            etHFtowerSumPlus += tower.pt();
          }
          if(isHF && eta < 0){
            etHFtowerSumMinus += tower.pt();
          }
    }
    etHFtowerSum = etHFtowerSumPlus + etHFtowerSumMinus;
    hHF->Fill(etHFtowerSum);
    hHFplus->Fill(etHFtowerSumPlus);
    hHFminus->Fill(etHFtowerSumMinus);

//Vertex
     double vx=-999.;
     double vy=-999.;
     double vz=-999.;
     double vxError=-999.;
     double vyError=-999.;
     double vzError=-999.;
     edm::Handle<VertexCollection> recoVertices;
     iEvent.getByToken(srcVertex_,recoVertices);
     unsigned int daughter = 0;
     int greatestvtx = 0;
     for (unsigned int i = 0 ; i< recoVertices->size(); ++i){
      daughter = (*recoVertices)[i].tracksSize();
      if( daughter > (*recoVertices)[greatestvtx].tracksSize()) greatestvtx = i;
     }
     if(recoVertices->size()>0){
      vx = (*recoVertices)[greatestvtx].position().x();
      vy = (*recoVertices)[greatestvtx].position().y();
      vz = (*recoVertices)[greatestvtx].position().z();
      vxError = (*recoVertices)[greatestvtx].xError();
      vyError = (*recoVertices)[greatestvtx].yError();
      vzError = (*recoVertices)[greatestvtx].zError();
     }
     if( fabs(vz)>vertexZMaxCut_ ) return;
     vxHist->Fill(vx);
     vyHist->Fill(vy);
     vzHist->Fill(vz);
     math::XYZPoint v1(vx,vy, vz);

//Track
     edm::Handle<TrackCollection> tracks;
     iEvent.getByToken(srcTracks_,tracks);
     int nTracks = 0;
     for(unsigned int i = 0 ; i < tracks->size(); ++i){
       const Track& track = (*tracks)[i];
       if ( track.charge() == 0 ) continue;
       if ( !track.quality(trackQuality_) ) continue;
       if ( track.pt() < trackPtMinCut_ ) continue;
       if ( track.pt() > trackPtMaxCut_ ) continue;
       if ( fabs(track.eta()) > trackEtaCut_ ) continue;

       double dxy = -1.* track.dxy(v1);
       double dxyerror=sqrt(track.dxyError()*track.dxyError()+vxError*vyError);
       double dz=track.dz(v1);
       double dzerror=sqrt(track.dzError()*track.dzError()+vzError*vzError);
       hd0Oerr->Fill(fabs(dxy/dxyerror));
       hdzOerr->Fill(fabs(dz/dzerror));
       hpterrOpt->Fill(track.ptError()/track.pt());

       if ( track.ptError()/track.pt() > ptErrCut_ ) continue;
       if ( fabs( dxy/dxyerror ) > dxyRelCut_ ) continue;
       if ( fabs( dz/dzerror ) > dzRelCut_ ) continue;
       ptHist->Fill(track.pt());
       etaHist->Fill(track.eta());
       phiHist->Fill(track.phi());
       nTracks++;
       hetaVSphi->Fill(track.eta(), track.phi());
     }
     generalTracksHist->Fill(nTracks);


#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
XeXeAnalysis::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
XeXeAnalysis::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
XeXeAnalysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(XeXeAnalysis);
