// -*- C++ -*-
//
// Package:    XeXeAnalysis/MCAnalyzer
// Class:      MCAnalyzer
// 
/**\class MCAnalyzer MCAnalyzer.cc XeXeAnalysis/MCAnalyzer/plugins/MCAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Shengquan Tuo
//         Created:  Wed, 21 Feb 2018 08:35:58 GMT
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

#include <iostream>
#include <string>
#include <fstream>

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "HepMC/GenEvent.h"
#include "HepMC/HeavyIon.h"

// root include file
 #include "TH2.h"
 #include "TFile.h"
 #include "TNtuple.h"

 using namespace std;




//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class MCAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MCAnalyzer(const edm::ParameterSet&);
      ~MCAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------

  edm::Service<TFileService> fs;
  edm::EDGetTokenT<edm::HepMCProduct> HepMCCollection_;
  TH1D* hB;
  TH1D* hNpart;
  TH1D* hNcoll;
  edm::EDGetTokenT<reco::GenParticleCollection> GenParticles_;
  TH1D* hPt;
  TH1D* hEta;
  TH1D* hPhi;
  TH1D* hCharge;
  edm::EDGetTokenT<CaloTowerCollection> srcTower_;
  TH1D* hHF;
  TH2D* hHFvsNpart;

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
MCAnalyzer::MCAnalyzer(const edm::ParameterSet& iConfig):

  HepMCCollection_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("HepMCCollection"))),
  GenParticles_(consumes<reco::GenParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("GenParticles",edm::InputTag("genParticles")))),
  srcTower_(consumes<CaloTowerCollection>(iConfig.getParameter<edm::InputTag>("srcTower")))
{
   //now do what ever initialization is needed
   usesResource("TFileService");

}


MCAnalyzer::~MCAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
MCAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace HepMC;

   double b = -1;
   double npart = -1;
   double ncoll = -1;
   edm::Handle<HepMCProduct> evt;
   iEvent.getByToken(HepMCCollection_, evt);
   const HepMC::HeavyIon* ion = evt->GetEvent()->heavy_ion();
   b = ion->impact_parameter();
   hB->Fill(b);
   npart = ion->Npart_proj() + ion->Npart_targ();
   ncoll = ion->Ncoll();
   hNpart->Fill(npart);
   hNcoll->Fill(ncoll);

   edm::Handle<reco::GenParticleCollection> parts;
   iEvent.getByToken(GenParticles_,parts);
   for(UInt_t i = 0; i < parts->size(); ++i){
      const reco::GenParticle& p = (*parts)[i];
      if (p.status()!=1) continue;
      if (p.pt()<0.3) continue;
      if (fabs(p.eta())>2.4) continue;
      if (p.charge()==0) continue;
      //int pdg_id = p.pdgId();
      float eta = p.eta();
      float phi = p.phi();
      float pt = p.pt();
      int charge = p.charge();
      //cout<<"b="<<b<<", pdgid="<<pdg_id<<", eta="<<eta<<", phi="<<phi<<", pt="<<pt<<", charge="<<charge<<endl;
      hPt->Fill(pt);
      hEta->Fill(eta);
      hPhi->Fill(phi);
      hCharge->Fill(charge);
   }

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
    hHFvsNpart->Fill(npart, etHFtowerSum);

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
MCAnalyzer::beginJob()
{
  TH1D::SetDefaultSumw2();
  hB = fs->make<TH1D>("b","N",2000000,-0.01,19.99);
  hNpart = fs->make<TH1D>("npart","N",300,0,300.0);
  hNcoll = fs->make<TH1D>("ncoll","N",1500,0,1500.0);
  hPt = fs->make<TH1D>("pt","N particles",200,0,10);
  hEta = fs->make<TH1D>("eta","N particles",300,-3,3);
  hPhi = fs->make<TH1D>("phi","N particles",200,-4,4);
  hCharge = fs->make<TH1D>("charge","N particles",60,-3,3);
  hHF = fs->make<TH1D>("hfhist","HF Distribution", 400, 0., 4000.);
  hHFvsNpart = fs->make<TH2D>("hfvsnparthist","HF vs Npart Distribution", 300,0,300.0, 400,0.,4000.);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MCAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MCAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MCAnalyzer);
