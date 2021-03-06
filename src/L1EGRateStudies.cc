// -*- C++ -*-
//
// Package:    L1EGRateStudies
// Class:      L1EGRateStudies
// 
/**\class L1EGRateStudies L1EGRateStudies.cc L1Trigger/L1EGRateStudies/src/L1EGRateStudies.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Nick Smith
//         Created:  Mon Mar 10 13:27:23 CDT 2014
// $Id$
//
//


// system include files
#include <memory>
#include <array>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TMath.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/Phase2L1CaloTrig/interface/L1EGCrystalCluster.h"
#include "DataFormats/Phase2L1CaloTrig/src/classes.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"

#include "FastSimulation/BaseParticlePropagator/interface/BaseParticlePropagator.h"
//#include "FastSimulation/Particle/interface/ParticleTable.h"

//#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
//#include "DataFormats/L1TrackTrigger/interface/TTPixelTrack.h"
//#include "L1Trigger/L1EGRateStudies/interface/L1TkElectronTrackMatchAlgo.h"
#include "L1Trigger/L1CaloTrigger/interface/L1TkElectronTrackMatchAlgo.h"

//track trigger data formats
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTCluster.h"
#include "DataFormats/L1TrackTrigger/interface/TTStub.h"
#include "DataFormats/L1TrackTrigger/interface/TTTrack.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTClusterAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTStubAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTTrackAssociationMap.h"
#include "Geometry/Records/interface/StackedTrackerGeometryRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

// Track PV
#include "DataFormats/L1TrackTrigger/interface/L1TkPrimaryVertex.h"


#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "FastSimulation/Particle/interface/RawParticle.h"

// ECAL TPs
#include "SimCalorimetry/EcalEBTrigPrimProducers/plugins/EcalEBTrigPrimProducer.h"
#include "DataFormats/EcalDigi/interface/EcalEBTriggerPrimitiveDigi.h"
// slimmed Electrons
#include "DataFormats/PatCandidates/interface/Electron.h"

// Stage2
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/L1Candidate.h"


//
// class declaration
//
class L1EGRateStudies : public edm::EDAnalyzer {
   typedef std::vector< TTTrack < Ref_Phase2TrackerDigi_ >> L1TkTrackCollectionType;
   typedef BXVector<l1t::EGamma> EGammaBxCollection;
   typedef std::vector<l1t::EGamma> EGammaCollection;

   public:
      explicit L1EGRateStudies(const edm::ParameterSet&);
      ~L1EGRateStudies();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      //virtual void endRun(edm::Run const&, edm::EventSetup const&);
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // -- user functions
      void integrateDown(TH1F *);
      void fill_tree(const l1slhc::L1EGCrystalCluster& cluster);
      // Don't use these, they require uncorrected pT and H/E
      //bool cluster_passes_base_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      //bool cluster_passes_l1tkMatch_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      //bool cluster_passes_stage2_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      //bool cluster_passes_95_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      //bool cluster_passes_90_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      bool cluster_passes_track_cuts(const l1slhc::L1EGCrystalCluster& cluster, float trackDeltaR) const;
      bool cluster_passes_track_variable_cuts(const l1slhc::L1EGCrystalCluster& cluster, float trackDeltaR, float deltaRMax ) const;
      //bool cluster_passes_photon_cuts(const l1slhc::L1EGCrystalCluster& cluster) const;
      bool checkTowerExists(const l1slhc::L1EGCrystalCluster &cluster, const EcalTrigPrimDigiCollection &tps) const;
      //void checkRecHitsFlags(const l1slhc::L1EGCrystalCluster &cluster, const EcalTrigPrimDigiCollection &tps, const EcalRecHitCollection &ecalRecHits) const;
      void doTrackMatching(const l1slhc::L1EGCrystalCluster& cluster, edm::Handle<L1TkTrackCollectionType> l1trackHandle);
      size_t getBestTrack(double dR_cut, const reco::Candidate::PolarLorentzVector &p4, edm::Handle<L1TkTrackCollectionType> l1trackHandle);
      
      // ----------member data ---------------------------
      bool doEfficiencyCalc;
      bool useOfflineClusters;
      bool debug;
      bool useEndcap;
      bool doTracking;
      bool isPhoton;
      
      double genMatchDeltaRcut;
      double genMatchRelPtcut;
      double trackDeltaRMax;
      
      int eventCount;

      // Fit function to scale L1EG Crystal Pt to Stage-2
      //TF1 ptAdjustFunc = TF1("ptAdjustFunc", "(([0] + [1]*TMath::Exp(-[2]*x))*(1./([3] + [4]*TMath::Exp(-[5]*x))))");
      TF1 ptAdjustFuncPhaseII = TF1("ptAdjustFunc", "([0] + [1]*TMath::Exp(-[2]*x)) * ([3] + [4]*TMath::Exp(-[5]*x))");
      TF1 ptAdjustFuncStage2 = TF1("ptAdjustFunc2", "[0] + [1]*TMath::Exp(-[2]*x)");

      //std::vector<edm::InputTag> L1EGammaInputTags;
      edm::InputTag L1CrystalClustersInputTag;
      edm::InputTag offlineRecoClusterInputTag;

      edm::EDGetTokenT<l1slhc::L1EGCrystalClusterCollection> crystalClustersToken_;
      l1slhc::L1EGCrystalClusterCollection crystalClusters;
      edm::Handle<l1slhc::L1EGCrystalClusterCollection> crystalClustersHandle;      

      edm::EDGetTokenT<reco::GenParticleCollection> genCollectionToken_;
      reco::GenParticleCollection genParticles;
      //iEvent.getByLabel("genParticles", genParticleHandle);
      edm::Handle<reco::GenParticleCollection> genParticleHandle;

      //edm::EDGetTokenT<EcalEBTrigPrimDigiCollection> ecalTPEBToken_;
      //edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitEBToken_;
      //edm::EDGetTokenT<EcalRecHitCollection> ecalRecHitEEToken_;

      edm::EDGetTokenT<L1TkTrackCollectionType> L1TrackInputToken_;
      edm::EDGetTokenT<l1t::L1TkPrimaryVertexCollection> L1TrackPVToken_;

      edm::EDGetTokenT<std::vector<pat::Electron>> offlineRecoClusterToken_;
      edm::Handle<std::vector<pat::Electron>> offlineRecoClustersHandle;
      //edm::EDGetTokenT<reco::SuperClusterCollection> offlineRecoClusterToken_;
      //edm::Handle<reco::SuperClusterCollection> offlineRecoClustersHandle;

      // Stage2 Digis
      edm::EDGetTokenT<BXVector<l1t::EGamma> > stage2egToken1_;
            
      int nHistBins, nHistEtaBins;
      double histLow;
      double histHigh;
      double histetaLow;
      double histetaHigh;
      // For checking the effect of the barrel gap
      //float etaEffBinning[18] = 
      //    {-2.,-1.75,-1.5,-1.25,-1.,-.75,-.5,-.25,-.02,.02,.25,.5,.75,1.,1.25,1.5,1.75,2.};
      //int nEtaVarBins = 17;
      float etaEffBinning[17] = 
          {-2.,-1.75,-1.5,-1.25,-1.,-.75,-.5,-.25,0.0,.25,.5,.75,1.,1.25,1.5,1.75,2.};
      int nEtaVarBins = 16;

      TH1F * efficiency_denominator_hist;
      TH1F * efficiency_denominator_trk_gen_hist_0p3;
      TH1F * efficiency_denominator_trk_gen_hist_0p1;
      TH1F * efficiency_denominator_trk_gen_hist_0p05;
      TH1F * efficiency_denominator_eta_hist;
      TH1F * efficiency_denominator_reco_hist;

      TH1F * dyncrystal_efficiency_hist;
      TH1F * dyncrystal_efficiency_hist_open;
      TH1F * dyncrystal_efficiency_hist_loose;
      TH1F * dyncrystal_efficiency_hist_stage2;
      TH1F * dyncrystal_efficiency_hist_95;
      TH1F * dyncrystal_efficiency_hist_90;
      TH1F * dyncrystal_efficiency_hist_stage2_reco10;
      TH1F * dyncrystal_efficiency_hist_95_reco10;
      TH1F * dyncrystal_efficiency_hist_90_reco10;
      TH1F * dyncrystal_efficiency_track_hist;
      TH1F * dyncrystal_efficiency_trackl1match_hist;
      TH1F * dyncrystal_efficiency_trackl1matchOnly_hist;
      TH1F * dyncrystal_efficiency_track_gen_match_hist_0p3;
      TH1F * dyncrystal_efficiency_track_gen_match_hist_0p1;
      TH1F * dyncrystal_efficiency_track_gen_match_hist_0p05;
      TH1F * dyncrystal_efficiency_phoWindow_hist;
      std::map<double, TH1F *> dyncrystal_efficiency_reco_hists; // Turn-on thresholds
      std::map<double, TH1F *> dyncrystal_efficiency_reco_adj_hists; // Turn-on thresholds
      std::map<double, TH1F *> dyncrystal_efficiency_gen_hists; // Turn-on thresholds
      TH1F * dyncrystal_efficiency_bremcut_hist;
      TH1F * dyncrystal_efficiency_eta_hist;
      TH1F * dyncrystal_efficiency_track_eta_hist;
      TH1F * dyncrystal_efficiency_phoWindow_eta_hist;
      TH1F * dyncrystal_deltaR_hist;
      TH1F * dyncrystal_deltaR_bremcut_hist;
      TH1F * dyncrystal_deta_hist;
      TH1F * dyncrystal_dphi_hist;
      TH1F * dyncrystal_dphi_bremcut_hist;
      TH1F * dyncrystal_rate_hist;
      TH1F * dyncrystal_track_rate_hist;
      TH1F * dyncrystal_phoWindow_rate_hist;
      TH1F * dyncrystal_trackl1match_rate_hist;
      TH1F * dyncrystal_trackl1matchOnly_rate_hist;
      TH1F * dyncrystal_rate_adj_hist;
      TH1F * dyncrystal_rate_adj_hist_stage2;
      //TH1F * dyncrystal_rate_adj_hist_95;
      TH1F * dyncrystal_rate_adj_hist_90;
      TH1F * dyncrystal_track_rate_adj_hist;
      TH1F * dyncrystal_phoWindow_rate_adj_hist;
      TH2F * dyncrystal_2DdeltaR_hist;

      TH1F * stage2_efficiency_hist;
      //TH1F * stage2_efficiency_iso_hist;
      std::map<double, TH1F *> stage2_efficiency_reco_hists; // Turn-on thresholds
      std::map<double, TH1F *> stage2_efficiency_gen_hists; // Turn-on thresholds
      std::map<double, TH1F *> stage2_efficiency_gen_hists_calib; // Turn-on thresholds, Stage-2 calibrated to Gen
      TH1F * stage2_efficiency_bremcut_hist;
      TH1F * stage2_efficiency_eta_hist;
      //TH1F * stage2_efficiency_iso_eta_hist;
      TH1F * stage2_deltaR_hist;
      TH1F * stage2_deltaR_bremcut_hist;
      TH1F * stage2_deta_hist;
      TH1F * stage2_dphi_hist;
      TH1F * stage2_dphi_bremcut_hist;
      TH1F * stage2_rate_hist;
      TH1F * stage2_rate_hist_calib;
      //TH1F * stage2_iso_rate_hist;
      TH2F * stage2_2DdeltaR_hist;
      TH2F * stage2_reco_gen_pt_hist;
      TH2F * stage2_reco_gen_pt_hist2;
      TH2F * stage2_reco_gen_pt_hist3;
      TH2F * stage2_reco_gen_pt_hist4;
      TH1F * stage2_reco_gen_pt_1dHist;
      TH1F * stage2_reco_gen_pt_1dHist_calib;

      //std::map<std::string, TH1F *> EGalg_efficiency_hists;
      //std::map<std::string, std::map<double, TH1F *>> EGalg_efficiency_reco_hists;
      //std::map<std::string, std::map<double, TH1F *>> EGalg_efficiency_gen_hists;
      //std::map<std::string, TH1F *> EGalg_efficiency_eta_hists;
      //std::map<std::string, TH1F *> EGalg_deltaR_hists;
      //std::map<std::string, TH1F *> EGalg_deta_hists;
      //std::map<std::string, TH1F *> EGalg_dphi_hists;
      //std::map<std::string, TH1F *> EGalg_rate_hists;
      //std::map<std::string, TH2F *> EGalg_2DdeltaR_hists;
      //std::map<std::string, TH2F *> EGalg_reco_gen_pt_hists;
      //std::map<std::string, TH1F *> EGalg_reco_gen_pt_1dHists;

      // EcalRecHits flags
      TH1I * RecHitFlagsTowerHist;
      TH1I * RecHitFlagsNoTowerHist;

      // Crystal pt stuff
      TTree * crystal_tree;
      struct {
         double run;
         double lumi;
         double event;
         float hcal_dR0p05;
         float hcal_dR0p075;
         float hcal_dR0p1;
         float hcal_dR0p125;
         float hcal_dR0p15;
         float hcal_dR0p2;
         float hcal_dR0p3;
         float hcal_dR0p4;
         float hcal_dR0p5;
         float hcal_dR0p05_nTowers;
         float hcal_dR0p075_nTowers;
         float hcal_dR0p1_nTowers;
         float hcal_dR0p125_nTowers;
         float hcal_dR0p15_nTowers;
         float hcal_dR0p2_nTowers;
         float hcal_dR0p3_nTowers;
         float hcal_dR0p4_nTowers;
         float hcal_dR0p5_nTowers;
         std::array<float, 6> crystal_pt;
         int   crystalCount;
         float cluster_pt;
         float cluster_pt_adj;
         float cluster_ptPUCorr;
         float stage2_pt;
         float stage2_pt_calib;
         float cluster_preCalibratedPt;
         float cluster_energy;
         float eta;
         float phi;
         float hovere;
         float iso;
         float bremStrength;
         bool passedBase = false;
         bool passedPhoton = false;
         bool passedTrack = false;
         bool electronWP98 = false;
         bool photonWP80 = false;
         bool electronWP90 = false;
         bool looseL1TkMatchWP = false;
         bool stage2matchEff = false;
         float e2x2;
         float e2x5;
         float e3x5;
         float e5x5;
         float deltaR = 0.;
         float deltaPhi = 0.;
         float deltaEta = 0.;
         float gen_trk_deltaR_0p3 = 999.;
         float gen_trk_deltaR_0p1 = 999.;
         float gen_trk_deltaR_0p05 = 999.;
         float gen_pt = 0.;
         float gen_z = 999.;
         float gen_eta = 0.;
         float gen_phi = 0.;
         float gen_energy = 0.;
         float gen_charge = 0.;
         float E_gen = 0.;
         float denom_pt = 0.;
         float reco_pt = 0.;
         float reco_eta = -99.;
         float reco_phi = -99.;
         int   nthCandidate = -1;
         bool  endcap = false;
         float uslPt = 0.;
         float lslPt = 0.;
         float corePt = 0.;
         float ecalPUtoPt = 0.;
         float E_core = 0.;
         float phiStripContiguous0;
         float phiStripOneHole0;
         float phiStripContiguous3p;
         float phiStripOneHole3p;
         float trackDeltaR;
         float trackZ;
         float trackEta;
         float trackPhi;
         float trackDeltaPhi;
         float trackDeltaEta;
         float trackMomentum;
         float trackPt;
         float trackHighestPt;
         float trackHighestPtEta;
         float trackHighestPtPhi;
         float trackHighestPtChi2;
         float trackHighestPtCutChi2;
         float trackHighestPtCutChi2Eta;
         float trackHighestPtCutChi2Phi;
         float trackHighestPtCutChi2Chi2;
         float trackRInv;         
         float trackChi2;
         float trackIsoConeTrackCount;
         float trackIsoConePtSum;
         //float trackPUTrackPtGlobalDiffZ;
         //float trackPUTrackPtGlobalDiffZandPt;
         //float trackPUTrackPtGlobalSameZ;
         //float trackPUTrackPtGlobalAll;
         //float trackPUTrackPt13x113DiffZ;
         //float trackPUTrackPt13x113DiffZandPt;
         //float trackPUTrackPt13x113SameZ;
         //float trackPUTrackPt13x113All;
         //float trackPUTrackPt3x5DiffZ;
         //float trackPUTrackPt3x5DiffZandPt;
         //float trackPUTrackPt3x5SameZ;
         //float trackPUTrackPt3x5All;
         //float trackPUTrackPtECalIsoConeDiffZ;
         //float trackPUTrackPtECalIsoConeDiffZandPt;
         //float trackPUTrackPtECalIsoConeSameZ;
         //float trackPUTrackPtECalIsoConeAll;
         //float trackPUTrackPtTkIsoConeDiffZ;
         //float trackPUTrackPtTkIsoConeDiffZandPt;
         //float trackPUTrackPtTkIsoConeSameZ;
         //float trackPUTrackPtTkIsoConeAll;
         //float trackPUTrackCntGlobalDiffZ;
         //float trackPUTrackCntGlobalDiffZandPt;
         //float trackPUTrackCntGlobalSameZ;
         //float trackPUTrackCntGlobalAll;
         //float trackPUTrackCnt13x113DiffZ;
         //float trackPUTrackCnt13x113DiffZandPt;
         //float trackPUTrackCnt13x113SameZ;
         //float trackPUTrackCnt13x113All;
         //float trackPUTrackCnt3x5DiffZ;
         //float trackPUTrackCnt3x5DiffZandPt;
         //float trackPUTrackCnt3x5SameZ;
         //float trackPUTrackCnt3x5All;
         //float trackPUTrackCntECalIsoConeDiffZ;
         //float trackPUTrackCntECalIsoConeDiffZandPt;
         //float trackPUTrackCntECalIsoConeSameZ;
         //float trackPUTrackCntECalIsoConeAll;
         //float trackPUTrackCntTkIsoConeDiffZ;
         //float trackPUTrackCntTkIsoConeDiffZandPt;
         //float trackPUTrackCntTkIsoConeSameZ;
         //float trackPUTrackCntTkIsoConeAll;
         float zVertex;
         float zVertexEnergy;
      } treeinfo;

      // (pt_reco-pt_gen)/pt_gen plot
      TH2F * reco_gen_pt_hist;
      TH2F * reco_gen_pt_hist2;
      TH2F * reco_gen_pt_hist3;
      TH2F * reco_gen_pt_hist4;
      TH2F * reco_gen_pt_adj_hist;
      TH2F * reco_gen_pt_adj_hist2;
      TH2F * reco_gen_pt_adj_hist3;
      TH1F * l1eg_gen_pt_1dHist;
      TH1F * l1eg_gen_pt_adj_1dHist;
      TH1F * reco_gen_pt_1dHist;
      TH1F * recoSC_gen_pt_1dHist;
      TH1F * l1eg_reco_pt_1dHist;
      //TH1F * l1eg_recoSC_pt_1dHist;

      // dphi vs. brem
      TH2F * brem_dphi_hist;
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
L1EGRateStudies::L1EGRateStudies(const edm::ParameterSet& iConfig) :
   doEfficiencyCalc(iConfig.getUntrackedParameter<bool>("doEfficiencyCalc", false)),
   useOfflineClusters(iConfig.getUntrackedParameter<bool>("useOfflineClusters", false)),
   debug(iConfig.getUntrackedParameter<bool>("debug", false)),
   useEndcap(iConfig.getUntrackedParameter<bool>("useEndcap", false)),
   doTracking(iConfig.getUntrackedParameter<bool>("doTracking", false)),
   isPhoton(iConfig.getUntrackedParameter<bool>("isPhoton", false)),
   genMatchDeltaRcut(iConfig.getUntrackedParameter<double>("genMatchDeltaRcut", 0.1)),
   genMatchRelPtcut(iConfig.getUntrackedParameter<double>("genMatchRelPtcut", 0.5)),
   trackDeltaRMax(iConfig.getUntrackedParameter<double>("trackDeltaRMax", 0.05)),
   crystalClustersToken_(consumes<l1slhc::L1EGCrystalClusterCollection>(iConfig.getParameter<edm::InputTag>("L1CrystalClustersInputTag"))),
   genCollectionToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))),
   L1TrackInputToken_(consumes<L1TkTrackCollectionType>(iConfig.getParameter<edm::InputTag>("L1TrackInputTag"))),
   L1TrackPVToken_(consumes<l1t::L1TkPrimaryVertexCollection>(iConfig.getParameter<edm::InputTag>("L1TrackPrimaryVertexTag"))),
   offlineRecoClusterToken_(consumes<std::vector<pat::Electron>>(iConfig.getParameter<edm::InputTag>("OfflineRecoClustersInputTag"))),
   stage2egToken1_(consumes<BXVector<l1t::EGamma>>(iConfig.getParameter<edm::InputTag>("Stage2EG1Tag"))),
   //ecalTPEBToken_(consumes<EcalEBTrigPrimDigiCollection>(iConfig.getParameter<edm::InputTag>("ecalTPEB"))),
   //ecalRecHitEBToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ecalRecHitEB"))),
   //ecalRecHitEEToken_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ecalRecHitEE"))),
   nHistBins(iConfig.getUntrackedParameter<int>("histogramBinCount", 10)),
   nHistEtaBins(iConfig.getUntrackedParameter<int>("histogramEtaBinCount", 20)),
   histLow(iConfig.getUntrackedParameter<double>("histogramRangeLow", 0.)),
   histHigh(iConfig.getUntrackedParameter<double>("histogramRangeHigh", 50.)),
   histetaLow(iConfig.getUntrackedParameter<double>("histogramRangeetaLow", -2.5)),
   histetaHigh(iConfig.getUntrackedParameter<double>("histogramRangeetaHigh", 2.5))
{
   eventCount = 0;

   // Fit parameters measured on 28 May 2017, using 500 MeV threshold for ECAL TPs
   // working in CMSSW 920
   // Adjustments to be applied to reco cluster pt
   //ptAdjustFunc.SetParameter( 0, 1.062166 );
   //ptAdjustFunc.SetParameter( 1, 0.298738 );
   //ptAdjustFunc.SetParameter( 2, 0.038971 );
   //ptAdjustFunc.SetParameter( 3, 0.977781 );
   //ptAdjustFunc.SetParameter( 4, -0.054748 );
   //ptAdjustFunc.SetParameter( 5, 0.044248 );
   ptAdjustFuncPhaseII.SetParameter( 0, 1.06 );
   ptAdjustFuncPhaseII.SetParameter( 1, 0.273 );
   ptAdjustFuncPhaseII.SetParameter( 2, 0.0411 );
   ptAdjustFuncPhaseII.SetParameter( 3, 1.00 );
   ptAdjustFuncPhaseII.SetParameter( 4, 0.567 );
   ptAdjustFuncPhaseII.SetParameter( 5, 0.288 );
   ptAdjustFuncStage2.SetParameter( 0, 0.968 );
   ptAdjustFuncStage2.SetParameter( 1, -0.312 );
   ptAdjustFuncStage2.SetParameter( 2, 0.0729 );

   //L1EGammaInputTags = iConfig.getParameter<std::vector<edm::InputTag>>("L1EGammaInputTags");
   //L1EGammaInputTags.push_back(edm::InputTag("l1extraParticles:All"));
   //L1EGammaInputTags.push_back(edm::InputTag("l1extraParticlesUCT:All"));
   //L1CrystalClustersInputTag = iConfig.getParameter<edm::InputTag>("L1CrystalClustersInputTag");

   edm::Service<TFileService> fs;
   
   // If using offline-reco clusters, label dR & related hists appropriately
   std::string drLabel("(Gen-Reco);Counts");
   if ( useOfflineClusters ) drLabel = "(vs. Offline Reco.);Counts";

   // Make a set of histograms to fill, depending on if we are doing rate or efficiency
   if ( doEfficiencyCalc )
   {
      auto thresholds = iConfig.getUntrackedParameter<std::vector<int>>("turnOnThresholds");
      //offlineRecoClusterInputTag = iConfig.getParameter<edm::InputTag>("OfflineRecoClustersInputTag");

      dyncrystal_efficiency_hist = fs->make<TH1F>("dyncrystalEG_efficiency_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_open = fs->make<TH1F>("dyncrystalEG_efficiency_open_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_loose = fs->make<TH1F>("dyncrystalEG_efficiency_loose_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_stage2 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_stage2", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_95 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_95", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_90 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_90", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_stage2_reco10 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_stage2_reco10", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_95_reco10 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_95_reco10", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_hist_90_reco10 = fs->make<TH1F>("dyncrystalEG_efficiency_pt_90_reco10", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_track_hist = fs->make<TH1F>("dyncrystalEG_efficiency_track_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_trackl1match_hist = fs->make<TH1F>("dyncrystalEG_efficiency_trackl1match_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_trackl1matchOnly_hist = fs->make<TH1F>("dyncrystalEG_efficiency_trackl1matchOnly_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_track_gen_match_hist_0p3 = fs->make<TH1F>("dyncrystalEG_efficiency_track_gen_match_pt_0p3", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_track_gen_match_hist_0p1 = fs->make<TH1F>("dyncrystalEG_efficiency_track_gen_match_pt_0p1", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_track_gen_match_hist_0p05 = fs->make<TH1F>("dyncrystalEG_efficiency_track_gen_match_pt_0p05", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_phoWindow_hist = fs->make<TH1F>("dyncrystalEG_efficiency_phoWindow_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_bremcut_hist = fs->make<TH1F>("dyncrystalEG_efficiency_bremcut_pt", "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      dyncrystal_efficiency_eta_hist = fs->make<TH1F>("dyncrystalEG_efficiency_eta", "Dynamic Crystal Trigger;Gen. #eta;Efficiency", nEtaVarBins, etaEffBinning);
      dyncrystal_efficiency_track_eta_hist = fs->make<TH1F>("dyncrystalEG_efficiency_track_eta", "Dynamic Crystal Trigger;Gen. #eta;Efficiency", nEtaVarBins, etaEffBinning);
      dyncrystal_efficiency_phoWindow_eta_hist = fs->make<TH1F>("dyncrystalEG_efficiency_phoWindow_eta", "Dynamic Crystal Trigger;Gen. #eta;Efficiency", nEtaVarBins, etaEffBinning);
      // Implicit conversion from int to double
      for(int threshold : thresholds)
      {
         dyncrystal_efficiency_reco_hists[threshold] = fs->make<TH1F>(("dyncrystalEG_threshold"+std::to_string(threshold)+"_efficiency_reco_pt").c_str(), "Dynamic Crystal Trigger;Offline reco. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
         dyncrystal_efficiency_reco_adj_hists[threshold] = fs->make<TH1F>(("dyncrystalEG_threshold"+std::to_string(threshold)+"_efficiency_reco_adj_pt").c_str(), "Dynamic Crystal Trigger;Offline reco. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
         dyncrystal_efficiency_gen_hists[threshold] = fs->make<TH1F>(("dyncrystalEG_threshold"+std::to_string(threshold)+"_efficiency_gen_pt").c_str(), "Dynamic Crystal Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      }
      dyncrystal_deltaR_hist = fs->make<TH1F>("dyncrystalEG_deltaR", ("Dynamic Crystal Trigger;#Delta R "+drLabel).c_str(), 100, 0., genMatchDeltaRcut);
      dyncrystal_deltaR_bremcut_hist = fs->make<TH1F>("dyncrystalEG_deltaR_bremcut", ("Dynamic Crystal Trigger;#Delta R "+drLabel).c_str(), 100, 0., genMatchDeltaRcut);
      dyncrystal_deta_hist = fs->make<TH1F>("dyncrystalEG_deta", ("Dynamic Crystal Trigger;d#eta "+drLabel).c_str(), 100, -0.25, 0.25);
      dyncrystal_dphi_hist = fs->make<TH1F>("dyncrystalEG_dphi", ("Dynamic Crystal Trigger;d#phi "+drLabel).c_str(), 100, -0.25, 0.25);
      dyncrystal_dphi_bremcut_hist = fs->make<TH1F>("dyncrystalEG_dphi_bremcut", ("Dynamic Crystal Trigger;d#phi "+drLabel).c_str(), 50, -0.1, 0.1);
      dyncrystal_2DdeltaR_hist = fs->make<TH2F>("dyncrystalEG_2DdeltaR_hist", "Dynamic Crystal Trigger;d#eta;d#phi;Counts", 50, -0.05, 0.05, 50, -0.05, 0.05);

      // Make Stage 2 hists
      stage2_efficiency_hist = fs->make<TH1F>("stage2EG_efficiency_pt", "Stage-2 Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      //stage2_efficiency_iso_hist = fs->make<TH1F>("stage2EG_efficiency_iso_pt", "Stage-2 Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      stage2_efficiency_bremcut_hist = fs->make<TH1F>("stage2EG_efficiency_bremcut_pt", "Stage-2 Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      stage2_efficiency_eta_hist = fs->make<TH1F>("stage2EG_efficiency_eta", "Stage-2 Trigger;Gen. #eta;Efficiency", nEtaVarBins, etaEffBinning);
      //stage2_efficiency_iso_eta_hist = fs->make<TH1F>("stage2EG_efficiency_iso_eta", "Stage-2 Trigger;Gen. #eta;Efficiency", nEtaVarBins, etaEffBinning);
      // Implicit conversion from int to double
      for(int threshold : thresholds)
      {
         stage2_efficiency_reco_hists[threshold] = fs->make<TH1F>(("stage2EG_threshold"+std::to_string(threshold)+"_efficiency_reco_pt").c_str(), "Stage-2 Trigger;Offline reco. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
         stage2_efficiency_gen_hists[threshold] = fs->make<TH1F>(("stage2EG_threshold"+std::to_string(threshold)+"_efficiency_gen_pt").c_str(), "Stage-2 Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
         stage2_efficiency_gen_hists_calib[threshold] = fs->make<TH1F>(("stage2EG_threshold"+std::to_string(threshold)+"_calib_efficiency_gen_pt").c_str(), "Stage-2 Trigger;Gen. pT (GeV);Efficiency", nHistBins, histLow, histHigh);
      }
      stage2_deltaR_hist = fs->make<TH1F>("stage2EG_deltaR", ("Stage-2 Trigger;#Delta R "+drLabel).c_str(), 100, 0., genMatchDeltaRcut);
      stage2_deltaR_bremcut_hist = fs->make<TH1F>("stage2EG_deltaR_bremcut", ("Stage-2 Trigger;#Delta R "+drLabel).c_str(), 100, 0., genMatchDeltaRcut);
      stage2_deta_hist = fs->make<TH1F>("stage2EG_deta", ("Stage-2 Trigger;d#eta "+drLabel).c_str(), 100, -0.25, 0.25);
      stage2_dphi_hist = fs->make<TH1F>("stage2EG_dphi", ("Stage-2 Trigger;d#phi "+drLabel).c_str(), 100, -0.25, 0.25);
      stage2_dphi_bremcut_hist = fs->make<TH1F>("stage2EG_dphi_bremcut", ("Stage-2 Trigger;d#phi "+drLabel).c_str(), 50, -0.1, 0.1);
      stage2_2DdeltaR_hist = fs->make<TH2F>("stage2EG_2DdeltaR_hist", "Stage-2 Trigger;d#eta;d#phi;Counts", 50, -0.05, 0.05, 50, -0.05, 0.05);
      stage2_reco_gen_pt_hist = fs->make<TH2F>("stage2_reco_gen_pt", "Stage-2;Gen. pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      stage2_reco_gen_pt_hist2 = fs->make<TH2F>("stage2_reco_gen_pt2", "Stage-2;Reco pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      stage2_reco_gen_pt_hist3 = fs->make<TH2F>("stage2_reco_gen_pt3", "Stage-2;Reco pT (GeV);gen/reco;Counts", 100, 0., 100., 100, 0.0, 2.0); 
      stage2_reco_gen_pt_hist4 = fs->make<TH2F>("stage2_reco_gen_pt4", "Stage-2;Calibrated Reco pT (GeV);gen/calib. reco;Counts", 100, 0., 100., 100, 0.0, 2.0); 
      stage2_reco_gen_pt_1dHist = fs->make<TH1F>("stage2_1d_reco_gen_pt", "Stage-2;(reco-gen)/gen;Counts", 100, -1., 1.); 
      stage2_reco_gen_pt_1dHist_calib = fs->make<TH1F>("stage2_1d_reco_gen_pt_calib", "Stage-2;(reco calib.-gen)/gen;Counts", 100, -1., 1.); 

      //for(auto& inputTag : L1EGammaInputTags)
      //{
      //   const std::string &name = inputTag.encode();
      //   EGalg_efficiency_hists[name] = fs->make<TH1F>((name+"_efficiency_pt").c_str(), (name+";Gen. pT (GeV);Efficiency").c_str(), nHistBins, histLow, histHigh);
      //   EGalg_efficiency_eta_hists[name] = fs->make<TH1F>((name+"_efficiency_eta").c_str(), (name+";Gen. #eta;Efficiency").c_str(), nEtaVarBins, etaEffBinning);
      //   // Implicit conversion from int to double
      //   for(int threshold : thresholds)
      //   {
      //      EGalg_efficiency_reco_hists[name][threshold] = fs->make<TH1F>((name+"_threshold"+std::to_string(threshold)+"_efficiency_reco_pt").c_str(), (name+";Offline reco. pT (GeV);Efficiency").c_str(), nHistBins, histLow, histHigh);
      //      EGalg_efficiency_gen_hists[name][threshold] = fs->make<TH1F>((name+"_threshold"+std::to_string(threshold)+"_efficiency_gen_pt").c_str(), (name+";Gen. pT (GeV);Efficiency").c_str(), nHistBins, histLow, histHigh);
      //   }
      //   EGalg_deltaR_hists[name] = fs->make<TH1F>((name+"_deltaR").c_str(), (name+";#Delta R "+drLabel).c_str(), 50, 0., genMatchDeltaRcut);
      //   EGalg_deta_hists[name] = fs->make<TH1F>((name+"_deta").c_str(), (name+";d#eta "+drLabel).c_str(), 100, -0.25, 0.25);
      //   EGalg_dphi_hists[name] = fs->make<TH1F>((name+"_dphi").c_str(), (name+";d#phi "+drLabel).c_str(), 100, -0.25, 0.25);
      //   EGalg_2DdeltaR_hists[name] = fs->make<TH2F>((name+"_2DdeltaR").c_str(), ";d#eta;d#phi;Counts", 50, -0.05, 0.05, 50, -0.05, 0.05);
      //   EGalg_reco_gen_pt_hists[name] = fs->make<TH2F>((name+"_reco_gen_pt").c_str(), (name+";Gen. pT (GeV);(reco-gen)/gen;Counts").c_str(), 40, 0., 50., 40, -0.3, 0.3); 
      //   EGalg_reco_gen_pt_1dHists[name] = fs->make<TH1F>((name+"_1d_reco_gen_pt").c_str(), (name+";(reco-gen)/gen;Counts").c_str(), 100, -1., 1.); 
      //}

      reco_gen_pt_hist = fs->make<TH2F>("reco_gen_pt" , "EG relative momentum error;Gen. pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      reco_gen_pt_hist2 = fs->make<TH2F>("reco_gen_pt2" , "EG relative momentum error;Reco pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      reco_gen_pt_hist3 = fs->make<TH2F>("reco_gen_pt3" , "EG relative momentum error;Reco pT (GeV);gen/reco;Counts", 100, 0., 100., 100, 0., 2.0); 
      reco_gen_pt_hist4 = fs->make<TH2F>("reco_gen_pt4" , "EG relative momentum error;Reco pT (GeV);gen/reco;Counts", 100, 0., 100., 100, 0., 2.0); 
      reco_gen_pt_adj_hist = fs->make<TH2F>("reco_gen_pt_adj" , "EG relative momentum error;Gen. pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      reco_gen_pt_adj_hist2 = fs->make<TH2F>("reco_gen_pt_adj2" , "EG relative momentum error;Reco pT (GeV);(reco-gen)/gen;Counts", 100, 0., 100., 100, -0.5, 0.5); 
      reco_gen_pt_adj_hist3 = fs->make<TH2F>("reco_gen_pt_adj3" , "EG relative momentum error;Reco pT (GeV);gen/reco;Counts", 100, 0., 100., 100, 0.0, 2.0); 
      l1eg_gen_pt_1dHist = fs->make<TH1F>("1d_l1eg_gen_pt" , "EG relative momentum error;(l1eg-gen)/gen;Counts", 100, -1., 1.); 
      l1eg_gen_pt_adj_1dHist = fs->make<TH1F>("1d_l1eg_gen_pt_adj" , "EG relative momentum error;(l1eg-gen)/gen;Counts", 100, -1., 1.); 
      reco_gen_pt_1dHist = fs->make<TH1F>("1d_reco_gen_pt" , "EG relative momentum error;(reco-gen)/gen;Counts", 100, -1., 1.); 
      l1eg_reco_pt_1dHist = fs->make<TH1F>("1d_l1eg_reco_pt" , "EG relative momentum error;(l1eg-reco)/reco;Counts", 100, -1., 1.); 
      //l1eg_recoSC_pt_1dHist = fs->make<TH1F>("1d_l1eg_recoSC_pt" , "EG relative momentum error;(l1eg-recoSC)/recoSC;Counts", 100, -1., 1.); 
      brem_dphi_hist = fs->make<TH2F>("brem_dphi_hist" , "Brem. strength vs. d#phi;Brem. Strength;d#phi;Counts", 40, 0., 2., 40, -0.05, 0.05); 

      efficiency_denominator_hist = fs->make<TH1F>("gen_pt", "Gen. pt;Gen. pT (GeV); Counts", nHistBins, histLow, histHigh);
      efficiency_denominator_trk_gen_hist_0p3 = fs->make<TH1F>("gen_pt_trk_match_0p3", "Gen. pt;Gen. pT (GeV); Counts", nHistBins, histLow, histHigh);
      efficiency_denominator_trk_gen_hist_0p1 = fs->make<TH1F>("gen_pt_trk_match_0p1", "Gen. pt;Gen. pT (GeV); Counts", nHistBins, histLow, histHigh);
      efficiency_denominator_trk_gen_hist_0p05 = fs->make<TH1F>("gen_pt_trk_match_0p05", "Gen. pt;Gen. pT (GeV); Counts", nHistBins, histLow, histHigh);
      efficiency_denominator_reco_hist = fs->make<TH1F>("reco_pt", "Offline reco. pt;Gen. pT (GeV); Counts", nHistBins, histLow, histHigh);
      efficiency_denominator_eta_hist = fs->make<TH1F>("gen_eta", "Gen. #eta;Gen. #eta; Counts", nEtaVarBins, etaEffBinning);
   }
   else
   {
      dyncrystal_rate_hist = fs->make<TH1F>("dyncrystalEG_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_track_rate_hist = fs->make<TH1F>("dyncrystalEG_track_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_phoWindow_rate_hist = fs->make<TH1F>("dyncrystalEG_phoWindow_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_rate_adj_hist = fs->make<TH1F>("dyncrystalEG_adj_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_rate_adj_hist_stage2 = fs->make<TH1F>("dyncrystalEG_adj_rate_stage2" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      //dyncrystal_rate_adj_hist_95 = fs->make<TH1F>("dyncrystalEG_adj_rate_95" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_rate_adj_hist_90 = fs->make<TH1F>("dyncrystalEG_adj_rate_90" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_trackl1match_rate_hist = fs->make<TH1F>("dyncrystalEG_adj_rate_trackl1match" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_trackl1matchOnly_rate_hist = fs->make<TH1F>("dyncrystalEG_adj_rate_trackl1matchOnly" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_track_rate_adj_hist = fs->make<TH1F>("dyncrystalEG_track_adj_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      dyncrystal_phoWindow_rate_adj_hist = fs->make<TH1F>("dyncrystalEG_phoWindow_adj_rate" , "Dynamic Crystal Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      stage2_rate_hist = fs->make<TH1F>("stage2EG_rate" , "Stage-2 Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      stage2_rate_hist_calib = fs->make<TH1F>("stage2EG_rate_calib" , "Stage-2 Trigger;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      //stage2_iso_rate_hist = fs->make<TH1F>("stage2EG_iso_rate" , "Stage-2 Trigger Iso;ET Threshold (GeV);Rate (kHz)", nHistBins, histLow, histHigh);
      //for(auto& inputTag : L1EGammaInputTags)
      //{
      //   const std::string &name = inputTag.encode();
      //   EGalg_rate_hists[name] = fs->make<TH1F>((name+"_rate").c_str() , (name+";ET Threshold (GeV);Rate (kHz)").c_str(), nHistBins, histLow, histHigh);
      //}
   }
   RecHitFlagsTowerHist = fs->make<TH1I>("recHitFlags_tower", "EcalRecHit status flags when tower exists;Flag;Counts", 20, 0, 19);
   RecHitFlagsNoTowerHist = fs->make<TH1I>("recHitFlags_notower", "EcalRecHit status flags when tower exists;Flag;Counts", 20, 0, 19);

   crystal_tree = fs->make<TTree>("crystal_tree", "Crystal cluster individual crystal pt values");
   crystal_tree->Branch("run", &treeinfo.run);
   crystal_tree->Branch("lumi", &treeinfo.lumi);
   crystal_tree->Branch("event", &treeinfo.event);
   crystal_tree->Branch("hcal_dR0p05", &treeinfo.hcal_dR0p05);
   crystal_tree->Branch("hcal_dR0p075", &treeinfo.hcal_dR0p075);
   crystal_tree->Branch("hcal_dR0p1", &treeinfo.hcal_dR0p1);
   crystal_tree->Branch("hcal_dR0p125", &treeinfo.hcal_dR0p125);
   crystal_tree->Branch("hcal_dR0p15", &treeinfo.hcal_dR0p15);
   crystal_tree->Branch("hcal_dR0p2", &treeinfo.hcal_dR0p2);
   crystal_tree->Branch("hcal_dR0p3", &treeinfo.hcal_dR0p3);
   crystal_tree->Branch("hcal_dR0p4", &treeinfo.hcal_dR0p4);
   crystal_tree->Branch("hcal_dR0p5", &treeinfo.hcal_dR0p5);
   crystal_tree->Branch("hcal_dR0p05_nTowers", &treeinfo.hcal_dR0p05_nTowers);
   crystal_tree->Branch("hcal_dR0p075_nTowers", &treeinfo.hcal_dR0p075_nTowers);
   crystal_tree->Branch("hcal_dR0p1_nTowers", &treeinfo.hcal_dR0p1_nTowers);
   crystal_tree->Branch("hcal_dR0p125_nTowers", &treeinfo.hcal_dR0p125_nTowers);
   crystal_tree->Branch("hcal_dR0p15_nTowers", &treeinfo.hcal_dR0p15_nTowers);
   crystal_tree->Branch("hcal_dR0p2_nTowers", &treeinfo.hcal_dR0p2_nTowers);
   crystal_tree->Branch("hcal_dR0p3_nTowers", &treeinfo.hcal_dR0p3_nTowers);
   crystal_tree->Branch("hcal_dR0p4_nTowers", &treeinfo.hcal_dR0p4_nTowers);
   crystal_tree->Branch("hcal_dR0p5_nTowers", &treeinfo.hcal_dR0p5_nTowers);
   crystal_tree->Branch("passedBase", &treeinfo.passedBase);
   crystal_tree->Branch("electronWP98", &treeinfo.electronWP98);
   crystal_tree->Branch("electronWP90", &treeinfo.electronWP90);
   crystal_tree->Branch("looseL1TkMatchWP", &treeinfo.looseL1TkMatchWP);
   crystal_tree->Branch("photonWP80", &treeinfo.photonWP80);
   crystal_tree->Branch("stage2matchEff", &treeinfo.stage2matchEff);
   crystal_tree->Branch("passedPhoton", &treeinfo.passedPhoton);
   crystal_tree->Branch("passedTrack", &treeinfo.passedTrack);
   crystal_tree->Branch("pt", &treeinfo.crystal_pt, "1:2:3:4:5:6");
   crystal_tree->Branch("crystalCount", &treeinfo.crystalCount);
   crystal_tree->Branch("cluster_pt", &treeinfo.cluster_pt);
   crystal_tree->Branch("cluster_pt_adj", &treeinfo.cluster_pt_adj);
   crystal_tree->Branch("cluster_ptPUCorr", &treeinfo.cluster_ptPUCorr);
   crystal_tree->Branch("cluster_preCalibratedPt", &treeinfo.cluster_preCalibratedPt);
   crystal_tree->Branch("cluster_energy", &treeinfo.cluster_energy);
   crystal_tree->Branch("stage2_pt", &treeinfo.stage2_pt);
   crystal_tree->Branch("stage2_pt_calib", &treeinfo.stage2_pt_calib);
   crystal_tree->Branch("eta", &treeinfo.eta);
   crystal_tree->Branch("phi", &treeinfo.phi);
   crystal_tree->Branch("cluster_hovere", &treeinfo.hovere);
   crystal_tree->Branch("cluster_iso", &treeinfo.iso);
   crystal_tree->Branch("bremStrength", &treeinfo.bremStrength);
   crystal_tree->Branch("e2x2", &treeinfo.e2x2);
   crystal_tree->Branch("e2x5", &treeinfo.e2x5);
   crystal_tree->Branch("e3x5", &treeinfo.e3x5);
   crystal_tree->Branch("e5x5", &treeinfo.e5x5);
   crystal_tree->Branch("deltaR", &treeinfo.deltaR);
   crystal_tree->Branch("deltaPhi", &treeinfo.deltaPhi);
   crystal_tree->Branch("deltaEta", &treeinfo.deltaEta);
   crystal_tree->Branch("gen_trk_deltaR_0p3", &treeinfo.gen_trk_deltaR_0p3);
   crystal_tree->Branch("gen_trk_deltaR_0p1", &treeinfo.gen_trk_deltaR_0p1);
   crystal_tree->Branch("gen_trk_deltaR_0p05", &treeinfo.gen_trk_deltaR_0p05);
   crystal_tree->Branch("gen_pt", &treeinfo.gen_pt);
   crystal_tree->Branch("gen_z", &treeinfo.gen_z);
   crystal_tree->Branch("gen_eta", &treeinfo.gen_eta);
   crystal_tree->Branch("gen_phi", &treeinfo.gen_phi);
   crystal_tree->Branch("gen_energy", &treeinfo.gen_energy);
   crystal_tree->Branch("gen_charge", &treeinfo.gen_charge);
   crystal_tree->Branch("E_gen", &treeinfo.E_gen);
   crystal_tree->Branch("denom_pt", &treeinfo.denom_pt);
   crystal_tree->Branch("reco_pt", &treeinfo.reco_pt);
   crystal_tree->Branch("reco_eta", &treeinfo.reco_eta);
   crystal_tree->Branch("reco_phi", &treeinfo.reco_phi);
   crystal_tree->Branch("nthCandidate", &treeinfo.nthCandidate);
   crystal_tree->Branch("endcap", &treeinfo.endcap);
   crystal_tree->Branch("uslPt", &treeinfo.uslPt);
   crystal_tree->Branch("lslPt", &treeinfo.lslPt);
   crystal_tree->Branch("corePt", &treeinfo.corePt);
   crystal_tree->Branch("ecalPUtoPt", &treeinfo.ecalPUtoPt);
   crystal_tree->Branch("E_core", &treeinfo.E_core);
   crystal_tree->Branch("phiStripContiguous0", &treeinfo.phiStripContiguous0);
   crystal_tree->Branch("phiStripOneHole0", &treeinfo.phiStripOneHole0);
   crystal_tree->Branch("phiStripContiguous3p", &treeinfo.phiStripContiguous3p);
   crystal_tree->Branch("phiStripOneHole3p", &treeinfo.phiStripOneHole3p);
   crystal_tree->Branch("trackDeltaR", &treeinfo.trackDeltaR);
   crystal_tree->Branch("trackZ", &treeinfo.trackZ);
   crystal_tree->Branch("trackEta", &treeinfo.trackEta);
   crystal_tree->Branch("trackPhi", &treeinfo.trackPhi);
   crystal_tree->Branch("trackDeltaPhi", &treeinfo.trackDeltaPhi);
   crystal_tree->Branch("trackDeltaEta", &treeinfo.trackDeltaEta);
   crystal_tree->Branch("trackMomentum", &treeinfo.trackMomentum);
   crystal_tree->Branch("trackPt", &treeinfo.trackPt);
   crystal_tree->Branch("trackHighestPt", &treeinfo.trackHighestPt);
   crystal_tree->Branch("trackHighestPtEta", &treeinfo.trackHighestPtEta);
   crystal_tree->Branch("trackHighestPtPhi", &treeinfo.trackHighestPtPhi);
   crystal_tree->Branch("trackHighestPtChi2", &treeinfo.trackHighestPtChi2);
   crystal_tree->Branch("trackHighestPtCutChi2", &treeinfo.trackHighestPtCutChi2);
   crystal_tree->Branch("trackHighestPtCutChi2Eta", &treeinfo.trackHighestPtCutChi2Eta);
   crystal_tree->Branch("trackHighestPtCutChi2Phi", &treeinfo.trackHighestPtCutChi2Phi);
   crystal_tree->Branch("trackHighestPtCutChi2Chi2", &treeinfo.trackHighestPtCutChi2Chi2);
   crystal_tree->Branch("trackRInv", &treeinfo.trackRInv);
   crystal_tree->Branch("trackChi2", &treeinfo.trackChi2);
   crystal_tree->Branch("trackIsoConeTrackCount", &treeinfo.trackIsoConeTrackCount);
   crystal_tree->Branch("trackIsoConePtSum", &treeinfo.trackIsoConePtSum);
   //crystal_tree->Branch("trackPUTrackPt13x113DiffZ", &treeinfo.trackPUTrackPt13x113DiffZ);
   //crystal_tree->Branch("trackPUTrackPt13x113DiffZandPt", &treeinfo.trackPUTrackPt13x113DiffZandPt);
   //crystal_tree->Branch("trackPUTrackPt13x113SameZ", &treeinfo.trackPUTrackPt13x113SameZ);
   //crystal_tree->Branch("trackPUTrackPt13x113All", &treeinfo.trackPUTrackPt13x113All);
   //crystal_tree->Branch("trackPUTrackPt3x5DiffZ", &treeinfo.trackPUTrackPt3x5DiffZ);
   //crystal_tree->Branch("trackPUTrackPt3x5DiffZandPt", &treeinfo.trackPUTrackPt3x5DiffZandPt);
   //crystal_tree->Branch("trackPUTrackPt3x5SameZ", &treeinfo.trackPUTrackPt3x5SameZ);
   //crystal_tree->Branch("trackPUTrackPt3x5All", &treeinfo.trackPUTrackPt3x5All);
   //crystal_tree->Branch("trackPUTrackPtECalIsoConeDiffZ", &treeinfo.trackPUTrackPtECalIsoConeDiffZ);
   //crystal_tree->Branch("trackPUTrackPtECalIsoConeDiffZandPt", &treeinfo.trackPUTrackPtECalIsoConeDiffZandPt);
   //crystal_tree->Branch("trackPUTrackPtECalIsoConeSameZ", &treeinfo.trackPUTrackPtECalIsoConeSameZ);
   //crystal_tree->Branch("trackPUTrackPtECalIsoConeAll", &treeinfo.trackPUTrackPtECalIsoConeAll);
   //crystal_tree->Branch("trackPUTrackPtTkIsoConeDiffZ", &treeinfo.trackPUTrackPtTkIsoConeDiffZ);
   //crystal_tree->Branch("trackPUTrackPtTkIsoConeDiffZandPt", &treeinfo.trackPUTrackPtTkIsoConeDiffZandPt);
   //crystal_tree->Branch("trackPUTrackPtTkIsoConeSameZ", &treeinfo.trackPUTrackPtTkIsoConeSameZ);
   //crystal_tree->Branch("trackPUTrackPtTkIsoConeAll", &treeinfo.trackPUTrackPtTkIsoConeAll);
   //crystal_tree->Branch("trackPUTrackCnt13x113DiffZ", &treeinfo.trackPUTrackCnt13x113DiffZ);
   //crystal_tree->Branch("trackPUTrackCnt13x113DiffZandPt", &treeinfo.trackPUTrackCnt13x113DiffZandPt);
   //crystal_tree->Branch("trackPUTrackCnt13x113SameZ", &treeinfo.trackPUTrackCnt13x113SameZ);
   //crystal_tree->Branch("trackPUTrackCnt13x113All", &treeinfo.trackPUTrackCnt13x113All);
   //crystal_tree->Branch("trackPUTrackCnt3x5DiffZ", &treeinfo.trackPUTrackCnt3x5DiffZ);
   //crystal_tree->Branch("trackPUTrackCnt3x5DiffZandPt", &treeinfo.trackPUTrackCnt3x5DiffZandPt);
   //crystal_tree->Branch("trackPUTrackCnt3x5SameZ", &treeinfo.trackPUTrackCnt3x5SameZ);
   //crystal_tree->Branch("trackPUTrackCnt3x5All", &treeinfo.trackPUTrackCnt3x5All);
   //crystal_tree->Branch("trackPUTrackCntECalIsoConeDiffZ", &treeinfo.trackPUTrackCntECalIsoConeDiffZ);
   //crystal_tree->Branch("trackPUTrackCntECalIsoConeDiffZandPt", &treeinfo.trackPUTrackCntECalIsoConeDiffZandPt);
   //crystal_tree->Branch("trackPUTrackCntECalIsoConeSameZ", &treeinfo.trackPUTrackCntECalIsoConeSameZ);
   //crystal_tree->Branch("trackPUTrackCntECalIsoConeAll", &treeinfo.trackPUTrackCntECalIsoConeAll);
   //crystal_tree->Branch("trackPUTrackCntTkIsoConeDiffZ", &treeinfo.trackPUTrackCntTkIsoConeDiffZ);
   //crystal_tree->Branch("trackPUTrackCntTkIsoConeDiffZandPt", &treeinfo.trackPUTrackCntTkIsoConeDiffZandPt);
   //crystal_tree->Branch("trackPUTrackCntTkIsoConeSameZ", &treeinfo.trackPUTrackCntTkIsoConeSameZ);
   //crystal_tree->Branch("trackPUTrackCntTkIsoConeAll", &treeinfo.trackPUTrackCntTkIsoConeAll);
   crystal_tree->Branch("zVertex", &treeinfo.zVertex);
   crystal_tree->Branch("zVertexEnergy", &treeinfo.zVertexEnergy);
}


L1EGRateStudies::~L1EGRateStudies()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called for each event  ------------
void
L1EGRateStudies::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   eventCount++;

   // electron candidates
   //std::map<std::string, l1extra::L1EmParticleCollection> eGammaCollections;
   //for(const auto& inputTag : L1EGammaInputTags)
   //{
   //   if (inputTag.encode().compare("l1extraParticles:All") == 0) continue;
   //   if (inputTag.encode().compare("l1extraParticlesUCT:All") == 0) continue;
   //   edm::Handle<l1extra::L1EmParticleCollection> handle;
   //   iEvent.getByLabel(inputTag, handle);
   //   if ( handle.product() == nullptr )
   //      std::cout << "There is no product of type " << inputTag.encode() << std::endl;
   //   else
   //      eGammaCollections[inputTag.encode()] = *handle.product();

   //   // Special case: Run 1, UCT alg. iso/niso are exclusive, we want to make inclusive EGamma available too
   //   if (inputTag.encode().find("l1extraParticlesUCT") != std::string::npos)
   //   {
   //      auto& collection = eGammaCollections["l1extraParticlesUCT:All"];
   //      collection.insert(begin(collection), begin(*handle.product()), end(*handle.product()));
   //   }
   //   else if (inputTag.encode().find("l1extraParticles") != std::string::npos)
   //   {
   //      auto& collection = eGammaCollections["l1extraParticles:All"];
   //      collection.insert(begin(collection), begin(*handle.product()), end(*handle.product()));
   //   }
   //}

   // electron candidate extra info from Sacha's algorithm
   //l1slhc::L1EGCrystalClusterCollection crystalClusters;
   //edm::Handle<l1slhc::L1EGCrystalClusterCollection> crystalClustersHandle;      
   //iEvent.getByLabel(L1CrystalClustersInputTag,crystalClustersHandle);
   iEvent.getByToken(crystalClustersToken_,crystalClustersHandle);
   crystalClusters = (*crystalClustersHandle.product());

   // Generator info (truth)
   //edm::Handle<reco::GenParticleCollection> genParticleHandle;
   //reco::GenParticleCollection genParticles;
   iEvent.getByToken(genCollectionToken_,genParticleHandle);
   genParticles = *genParticleHandle.product();
   std::sort(begin(genParticles), end(genParticles), [](reco::GenParticle& a, reco::GenParticle& b){return a.pt() > b.pt();});
   //if (abs(genParticles[0].pdgId()) != 11) return;
   reco::GenParticle& firstGen = genParticles[0];
   reco::GenParticle& bestGen = genParticles[0];
   int cnt = 0;
   for (reco::GenParticle& genP : genParticles ) {
      //std::cout << cnt << " Gen pT: " << genParticles[0].pt() << std::endl;
      if (genP.isPromptFinalState()) {
         //std::cout << cnt << " Gen pT PROMPT: " << genParticles[0].pt() << std::endl;
         bestGen = genP;
         //std::cout << cnt << " Gen pT PROMPT: " << bestGen.pt() << "   pdg Id " << bestGen.pdgId() << "   Charge: " << bestGen.charge() << std::endl;
         break;
      }
      ++cnt;
   }

   //if ( fabs(bestGen.pdgId()) != 11) {
   //     std::cout << "Event without electron as best gen.  Gen pdgId was: " << bestGen.pdgId() <<std::endl;
   //     return;
   //}

   if (firstGen.pt() != bestGen.pt()) std::cout << "   ---   Found better genP [0] = " << firstGen.pt() << " vs. best = " << bestGen.pt() << "\n" << std::endl;



   // L1 Tracks
   edm::Handle<L1TkTrackCollectionType> l1trackHandle;
   iEvent.getByToken(L1TrackInputToken_, l1trackHandle);

   // L1 Track PV
   edm::Handle<l1t::L1TkPrimaryVertexCollection> l1PrimaryVertexHandle;
   iEvent.getByToken(L1TrackPVToken_, l1PrimaryVertexHandle);

   if (doTracking) {
      if ( l1PrimaryVertexHandle.isValid() )
      {
         l1t::L1TkPrimaryVertexCollection vertices = *l1PrimaryVertexHandle.product();
         double vertexEnergy = vertices[0].getSum();
         double z = vertices[0].getZvertex();
         treeinfo.zVertex = z;
         treeinfo.zVertexEnergy = vertexEnergy;
         if (debug) std::cout << " - Vertex Position: " << z << " vertex energy: " << vertexEnergy << std::endl;
      }
      else std::cout << "No valid primary vertices" << std::endl;
   } // end doTracking with PV


   // Stage-2
   edm::Handle<BXVector<l1t::EGamma>> stage2eg1Handle;
   iEvent.getByToken(stage2egToken1_, stage2eg1Handle);
   EGammaBxCollection stage2BXEGs;
   EGammaCollection stage2EGs;
   if ( stage2eg1Handle.isValid() )
   {
      stage2BXEGs = *stage2eg1Handle.product();
      for(auto& eg : stage2BXEGs) {
         stage2EGs.push_back( eg );
      } 
      std::sort(begin(stage2EGs), end(stage2EGs), [](l1t::EGamma& a, l1t::EGamma& b){return a.pt() > b.pt();});
   }
   else std::cout << "No valid stage2 EGs (2)" << std::endl;


   // Trigger tower info (trigger primitives)
   //edm::Handle<EcalTrigPrimDigiCollection> tpH;
   //iEvent.getByLabel(edm::InputTag("ecalDigis:EcalTriggerPrimitives"), tpH);
   //EcalTrigPrimDigiCollection triggerPrimitives = *tpH.product();

   // EcalRecHits for looking at flags in the cluster seed crystal
   //edm::Handle<EcalEBTrigPrimDigiCollection> pcalohits;
   //edm::Handle<EcalRecHitCollection> pcalohits;
   //iEvent.getByLabel("ecalRecHit","EcalRecHitsEB",pcalohits);
   //iEvent.getByToken(ecalTPEBToken_,pcalohits);
   //iEvent.getByToken(ecalRecHitEBToken_,pcalohits);
   //EcalRecHitCollection ecalRecHits = *pcalohits.product();
   //EcalEBTrigPrimDigiCollection ecalRecHits = *pcalohits.product();

   // Record the standards
   treeinfo.run = iEvent.eventAuxiliary().run();
   treeinfo.lumi = iEvent.eventAuxiliary().luminosityBlock();
   treeinfo.event = iEvent.eventAuxiliary().event();

   // Sort clusters so we can always pick highest pt cluster matching cuts
   std::sort(begin(crystalClusters), end(crystalClusters), [](const l1slhc::L1EGCrystalCluster& a, const l1slhc::L1EGCrystalCluster& b){return a.pt() > b.pt();});
   // also sort old algorithm products
   //for(auto& collection : eGammaCollections)
   //   std::sort(begin(collection.second), end(collection.second), [](const l1extra::L1EmParticle& a, const l1extra::L1EmParticle& b){return a.pt() > b.pt();});
   
   int clusterCount = 0;
   if ( doEfficiencyCalc )
   {
      reco::Candidate::PolarLorentzVector trueElectron;
      float reco_electron_pt = 0.;
      float reco_electron_eta = -99.;
      float reco_electron_phi = -99.;
      
      // Get offline cluster info
      //iEvent.getByToken(offlineRecoClusterToken_, offlineRecoClustersHandle);
      //std::vector<pat::Electron> offlineRecoClusters = *offlineRecoClustersHandle.product();

      // Find the cluster corresponding to generated electron
      //double trueElectron_SC_pt = -99;
      bool offlineRecoFound = false;
      //for(auto& cluster : offlineRecoClusters)
      //{
      //   reco::Candidate::PolarLorentzVector p4;
      //   p4.SetPt(cluster.pt());
      //   p4.SetEta(cluster.eta());
      //   p4.SetPhi(cluster.phi());
      //   p4.SetM(0.);
      //   if ( reco::deltaR(p4, bestGen.polarP4()) < 0.1
      //       && fabs(p4.pt() - bestGen.pt()) < genMatchRelPtcut*bestGen.pt() )
      //   {
      //      if ( useOfflineClusters )
      //         trueElectron = p4;
      //      reco_electron_pt = p4.pt();
      //      reco_electron_eta = p4.eta();
      //      reco_electron_phi = p4.phi();
      //      offlineRecoFound = true;
      //      if (debug) std::cout << "Gen.-matched pBarrelCorSuperCluster: pt " 
      //               << cluster.energy()/std::cosh(cluster.eta()) 
      //               << " eta " << cluster.eta() 
      //               << " phi " << cluster.phi() << std::endl;
      //      if (debug) std::cout << "Cluster pt - Gen pt / Gen pt = " << (reco_electron_pt-bestGen.pt())/bestGen.pt() << std::endl;
      //      if (debug) std::cout << "Reco Electron pt: " << reco_electron_pt << std::endl;
      //      if (debug) std::cout << "Gen Electron pt: " << bestGen.pt() << std::endl;
      //      trueElectron_SC_pt = cluster.superCluster()->rawEnergy()*sin(trueElectron.theta());
      //      break;
      //   }
      //}
      if ( useOfflineClusters && !offlineRecoFound )
      {
         // if we can't offline reconstruct the generated electron, 
         // it might as well have not existed.
         eventCount--;
         return;
      }

      if ( !useOfflineClusters )
      {
         // Get the particle position upon entering ECal
         RawParticle particle(bestGen.p4());
         particle.setVertex(bestGen.vertex().x(), bestGen.vertex().y(), bestGen.vertex().z(), 0.);
         //particle.setID(bestGen.pdgId());
         // Skip setID requires some external libraries working well that
         // define HepPDT::ParticleID
         // in the end, setID sets the mass and charge of our particle.
         // Try doing this by hand for the moment

         // Electrons... mainly
         particle.setMass(.511); // MeV
         int pdgId = bestGen.pdgId();
         if (pdgId > 0) {
            particle.setCharge( -1.0 ); }
         if (pdgId < 0) {
            particle.setCharge( 1.0 ); }

         // Charged pions
         if (pdgId == -211 || pdgId == 211) {
               particle.setCharge( bestGen.charge() );
               particle.setMass(139.6); // MeV
         }

         // Photon
         if (isPhoton) {
            particle.setMass(0.0);
            particle.setCharge( 0.0 );
         }
         BaseParticlePropagator prop(particle, 0., 0., 4.);
         BaseParticlePropagator start(prop);
         prop.propagateToEcalEntrance();
         if(prop.getSuccess()!=0)
         {
            trueElectron = reco::Candidate::PolarLorentzVector(prop.E()*sin(prop.vertex().theta()), prop.vertex().eta(), prop.vertex().phi(), 0.);
            if ( debug ) std::cout << "Propogated genParticle to ECal, position: " << prop.vertex() << " momentum = " << prop.momentum() << std::endl;
            if ( debug ) std::cout << "                       starting position: " << start.vertex() << " momentum = " << start.momentum() << std::endl;
            if ( debug ) std::cout << "                    genParticle position: " << bestGen.vertex() << " momentum = " << bestGen.p4() << std::endl;
            if ( debug ) std::cout << "       old pt = " << bestGen.pt() << ", new pt = " << trueElectron.pt() << std::endl;
         }
         else
         {
            // something failed?
            if ( debug ) std::cout << "Taking defaul, non-propagated gen object" << std::endl;
            trueElectron = bestGen.polarP4();
         }
      }

      // Only one electron is produced in singleElectron files
      // we look for that electron in the reconstructed data within some deltaR cut,
      // and some relative pt error cut
      // and if we find it, it goes in the numerator
      // but only if in the barrel!
      if ( !useEndcap && fabs(trueElectron.eta()) > 1.479 )
      {
         eventCount--;
         return;
      }

      efficiency_denominator_hist->Fill(trueElectron.pt());
      // Only fill eta efficiencies if gen pt > 20
      if (trueElectron.pt()>20 && trueElectron.pt()<100)
         efficiency_denominator_eta_hist->Fill(trueElectron.eta());

      // Only fill if gen is matched to a track
      size_t best_trk_index_0p3 = getBestTrack( 0.3, trueElectron, l1trackHandle);
      size_t best_trk_index_0p1 = getBestTrack( 0.1, trueElectron, l1trackHandle);
      size_t best_trk_index_0p05 = getBestTrack( 0.05, trueElectron, l1trackHandle);
      double genTrkDeltaR_0p3 = 999;
      double genTrkDeltaR_0p1 = 999;
      double genTrkDeltaR_0p05 = 999;
      // Check there was a returned track
      // Get the track associated with the index
      // 9999 is the defaul index, if it is still 9999, no track was matched
      // deltaR 0p3
      if (best_trk_index_0p3 != 9999) {
        efficiency_denominator_trk_gen_hist_0p3->Fill(trueElectron.pt());
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> trk_ptr(l1trackHandle, best_trk_index_0p3);
        genTrkDeltaR_0p3 = L1TkElectronTrackMatchAlgo::deltaR(L1TkElectronTrackMatchAlgo::calorimeterPosition(trueElectron.phi(), trueElectron.eta(), trueElectron.energy()), trk_ptr);
      }
      treeinfo.gen_trk_deltaR_0p3 = genTrkDeltaR_0p3;
      // deltaR 0.1
      if (best_trk_index_0p1 != 9999) {
        efficiency_denominator_trk_gen_hist_0p1->Fill(trueElectron.pt());
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> trk_ptr(l1trackHandle, best_trk_index_0p1);
        genTrkDeltaR_0p1 = L1TkElectronTrackMatchAlgo::deltaR(L1TkElectronTrackMatchAlgo::calorimeterPosition(trueElectron.phi(), trueElectron.eta(), trueElectron.energy()), trk_ptr);
      }
      treeinfo.gen_trk_deltaR_0p1 = genTrkDeltaR_0p1;
      // deltaR 0.05
      if (best_trk_index_0p05 != 9999) {
        efficiency_denominator_trk_gen_hist_0p05->Fill(trueElectron.pt());
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> trk_ptr(l1trackHandle, best_trk_index_0p05);
        genTrkDeltaR_0p05 = L1TkElectronTrackMatchAlgo::deltaR(L1TkElectronTrackMatchAlgo::calorimeterPosition(trueElectron.phi(), trueElectron.eta(), trueElectron.energy()), trk_ptr);
      }
      treeinfo.gen_trk_deltaR_0p05 = genTrkDeltaR_0p05;

      treeinfo.gen_pt = bestGen.pt();
      treeinfo.gen_z = bestGen.vz();
      treeinfo.gen_eta = bestGen.eta();
      treeinfo.gen_phi = bestGen.phi();
      treeinfo.gen_energy = bestGen.energy();
      treeinfo.gen_charge = bestGen.charge();
      treeinfo.E_gen = bestGen.pt()*cosh(bestGen.eta());
      treeinfo.denom_pt = trueElectron.pt();
      if ( fabs(trueElectron.eta()) > 1.479 )
         treeinfo.endcap = true;
      else
         treeinfo.endcap = false;

      if ( offlineRecoFound ) {
         treeinfo.reco_pt = reco_electron_pt;
         treeinfo.reco_eta = reco_electron_eta;
         treeinfo.reco_phi = reco_electron_phi;
         efficiency_denominator_reco_hist->Fill(reco_electron_pt);
      }
      else
      {
         treeinfo.reco_pt = 0.;
      }
      //std::cout << "   ---!!!--- L1EG Size: " << crystalClusters.size() << std::endl;
      if ( crystalClusters.size() > 0 )
      {
         auto bestCluster = *std::min_element(begin(crystalClusters), end(crystalClusters), [trueElectron](const l1slhc::L1EGCrystalCluster& a, const l1slhc::L1EGCrystalCluster& b){return reco::deltaR(a, trueElectron) < reco::deltaR(b, trueElectron);});
         bool clusterFound = false;
         bool bestClusterUsed = false;
         bool openEffFilled = false;
         bool looseEffFilled = false;
         for(const auto& cluster : crystalClusters)
         {
            clusterCount++;
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               clusterFound = true;
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;
               bestClusterUsed = true;
               if ( debug ) std::cout << "using cluster dr = " << reco::deltaR(cluster, trueElectron) << std::endl;
               if (doTracking) doTrackMatching(cluster, l1trackHandle);
               treeinfo.nthCandidate = clusterCount;
               treeinfo.deltaR = reco::deltaR(cluster, trueElectron);
               treeinfo.deltaPhi = reco::deltaPhi(cluster, trueElectron);
               treeinfo.deltaEta = trueElectron.eta()-cluster.eta();
               
               fill_tree(cluster);
               //checkRecHitsFlags(cluster, triggerPrimitives, ecalRecHits);

               // Open selection essentially getting L1EG Reco eff
               if (!openEffFilled) {
                  openEffFilled = true;
                  dyncrystal_efficiency_hist_open->Fill(trueElectron.pt());
               }
               if (!looseEffFilled && cluster.looseL1TkMatchWP() ) {
                  looseEffFilled = true;
                  dyncrystal_efficiency_hist_loose->Fill(trueElectron.pt());
               }

               if ( cluster.standaloneWP() )
               {
                  dyncrystal_efficiency_hist->Fill(trueElectron.pt());
                  if (trueElectron.pt()>20 && trueElectron.pt()<100)
                     dyncrystal_efficiency_eta_hist->Fill(trueElectron.eta());

                  if ( cluster_passes_track_cuts(cluster, treeinfo.trackDeltaR) ) {
                     dyncrystal_efficiency_track_hist->Fill(trueElectron.pt());
                     if (trueElectron.pt()>20 && trueElectron.pt()<100)
                        dyncrystal_efficiency_track_eta_hist->Fill(trueElectron.eta());
                     // Fill this if there was a match with the L1Tracks to gen particle
                     if (best_trk_index_0p3 != 9999)
                        dyncrystal_efficiency_track_gen_match_hist_0p3->Fill(trueElectron.pt());
                     if (best_trk_index_0p1 != 9999)
                        dyncrystal_efficiency_track_gen_match_hist_0p1->Fill(trueElectron.pt());
                     if (best_trk_index_0p05 != 9999)
                        dyncrystal_efficiency_track_gen_match_hist_0p05->Fill(trueElectron.pt());
                  }

                  if ( cluster.photonWP80() ) {
                     dyncrystal_efficiency_phoWindow_hist->Fill(trueElectron.pt());
                     if (trueElectron.pt()>20 && trueElectron.pt()<100)
                        dyncrystal_efficiency_phoWindow_eta_hist->Fill(trueElectron.eta());
                  }

                  if ( offlineRecoFound )
                  {
                     for(auto& pair : dyncrystal_efficiency_reco_hists)
                     {
                        // (threshold, histogram)
                        if (cluster.pt() > pair.first)
                           pair.second->Fill(reco_electron_pt);
                     }
                  }
                  for(auto& pair : dyncrystal_efficiency_reco_adj_hists)
                  {
                     // (threshold, histogram)
                     if ( cluster.pt() > pair.first)
                        pair.second->Fill(trueElectron.pt());
                  }
                  for(auto& pair : dyncrystal_efficiency_gen_hists)
                  {
                     // (threshold, histogram)
                     if (cluster.pt() > pair.first)
                        pair.second->Fill(trueElectron.pt());
                  }
                  dyncrystal_deltaR_hist->Fill(reco::deltaR(cluster, trueElectron));
                  dyncrystal_deta_hist->Fill(trueElectron.eta()-cluster.eta());
                  dyncrystal_dphi_hist->Fill(reco::deltaPhi(cluster.phi(), trueElectron.phi()));
                  if ( cluster.bremStrength() < 0.2 )
                  {
                     dyncrystal_efficiency_bremcut_hist->Fill(trueElectron.pt());
                     dyncrystal_deltaR_bremcut_hist->Fill(reco::deltaR(cluster, trueElectron));
                     dyncrystal_dphi_bremcut_hist->Fill(reco::deltaPhi(cluster.phi(), trueElectron.phi()));
                  }
                  dyncrystal_2DdeltaR_hist->Fill(trueElectron.eta()-cluster.eta(), reco::deltaPhi(cluster, trueElectron));

                  reco_gen_pt_hist->Fill( trueElectron.pt(), (cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  reco_gen_pt_hist2->Fill( cluster.pt(), (cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  reco_gen_pt_hist3->Fill( cluster.pt(), trueElectron.pt()/cluster.pt() );
                  reco_gen_pt_hist4->Fill( cluster.GetExperimentalParam("preCalibratedPt"), trueElectron.pt()/cluster.GetExperimentalParam("preCalibratedPt") );
                  reco_gen_pt_adj_hist->Fill( trueElectron.pt(), ( cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  reco_gen_pt_adj_hist2->Fill( cluster.pt(), (cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  reco_gen_pt_adj_hist3->Fill( cluster.pt(), trueElectron.pt() / cluster.pt() );
                  l1eg_reco_pt_1dHist->Fill( (cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  //l1eg_recoSC_pt_1dHist->Fill( (cluster.pt() - trueElectron_SC_pt)/trueElectron_SC_pt );
                  l1eg_gen_pt_1dHist->Fill( (cluster.pt() - bestGen.pt())/bestGen.pt() );
                  reco_gen_pt_1dHist->Fill( (reco_electron_pt - bestGen.pt())/bestGen.pt() );
                  l1eg_gen_pt_adj_1dHist->Fill( (cluster.pt() - trueElectron.pt())/trueElectron.pt() );
                  brem_dphi_hist->Fill( cluster.bremStrength(), reco::deltaPhi(cluster, trueElectron) );
                  break;
               }
            } // end passes Pt and dR match
         }
         if ( clusterFound && !bestClusterUsed )
         {
            std::cerr << "Found a cluster but it wasn't the best so I lost efficiency!" << std::endl;
         }

         // Adding the new non-overlapping WPs for efficiency and rate
         double compThreshold = 10.0;
         // Stage-2 efficiency match
         for(const auto& cluster : crystalClusters)
         {
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;

               if ( cluster.stage2effMatch() )
               {
                  dyncrystal_efficiency_hist_stage2->Fill(trueElectron.pt());
                  if ( ( cluster.pt() ) > compThreshold)
                     dyncrystal_efficiency_hist_stage2_reco10->Fill(trueElectron.pt());
                  break;
               }
            } // end passes Pt and dR match
         }
         // 95% efficiency plateau
         for(const auto& cluster : crystalClusters)
         {
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;

               //if ( cluster_passes_95_cuts(cluster) )
               //{
               //   dyncrystal_efficiency_hist_95->Fill(trueElectron.pt());
               //   if ( cluster.pt() > compThreshold)
               //      dyncrystal_efficiency_hist_95_reco10->Fill(trueElectron.pt());
               //   break;
               //}
            } // end passes Pt and dR match
         }
         // 90% efficiency plateau
         for(const auto& cluster : crystalClusters)
         {
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;

               if ( cluster.electronWP90() )
               {
                  dyncrystal_efficiency_hist_90->Fill(trueElectron.pt());
                  if ( cluster.pt() > compThreshold)
                     dyncrystal_efficiency_hist_90_reco10->Fill(trueElectron.pt());
                  break;
               }
            } // end passes Pt and dR match
         }
         // L1Track match + different Iso and shower shape cuts
         for(const auto& cluster : crystalClusters)
         {
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;

               if ( cluster.looseL1TkMatchWP() && cluster_passes_track_variable_cuts(cluster, treeinfo.trackDeltaR, trackDeltaRMax ) )
               {
                  dyncrystal_efficiency_trackl1match_hist->Fill(trueElectron.pt());
                  break;
               }
            } // end passes Pt and dR match
         }
         // L1Track match + nothing else
         for(const auto& cluster : crystalClusters)
         {
            if ( reco::deltaR(cluster, trueElectron) < genMatchDeltaRcut
                 && fabs(cluster.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
            {
               if ( cluster.eta() != bestCluster.eta() || cluster.phi() != bestCluster.phi() ) // why don't I have a comparison op
                  continue;

               if ( cluster_passes_track_cuts(cluster, treeinfo.trackDeltaR) )
               {
                  dyncrystal_efficiency_trackl1matchOnly_hist->Fill(trueElectron.pt());
                  break;
               }
            } // end passes Pt and dR match
         }

      }

      for(const auto& EGCandidate : stage2EGs)
      {
         if ( reco::deltaR(EGCandidate.polarP4(), trueElectron) < genMatchDeltaRcut &&
              fabs(EGCandidate.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
         {
            stage2_efficiency_hist->Fill(trueElectron.pt());
            if (trueElectron.pt()>20 && trueElectron.pt()<100)
               stage2_efficiency_eta_hist->Fill(trueElectron.eta());
            //if (EGCandidate.hwIso() > 0.) {
            //   stage2_efficiency_iso_hist->Fill(trueElectron.pt());
            //   stage2_efficiency_iso_eta_hist->Fill(trueElectron.eta());
            //}
            if ( offlineRecoFound )
            {
               for(auto& pair : stage2_efficiency_reco_hists)
               {
                  // (threshold, histogram)
                  if (EGCandidate.pt() > pair.first)
                     pair.second->Fill(reco_electron_pt);
               }
            }
            for(auto& pair : stage2_efficiency_gen_hists)
            {
               // (threshold, histogram)
               if (EGCandidate.pt() > pair.first)
                  pair.second->Fill(trueElectron.pt());
            }
            // Stage2 pT calibrated to Gen
            for(auto& pair : stage2_efficiency_gen_hists_calib)
            {
               // (threshold, histogram)
               if ( (EGCandidate.pt() * ptAdjustFuncStage2.Eval( EGCandidate.pt() ) ) > pair.first)
                  pair.second->Fill(trueElectron.pt());
            }
            stage2_deltaR_hist->Fill(reco::deltaR(EGCandidate.polarP4(), trueElectron));
            stage2_deta_hist->Fill(trueElectron.eta()-EGCandidate.eta());
            stage2_dphi_hist->Fill(reco::deltaPhi(EGCandidate.phi(), trueElectron.phi()));
            stage2_reco_gen_pt_hist->Fill( trueElectron.pt(), (EGCandidate.pt() - trueElectron.pt())/trueElectron.pt() );
            stage2_reco_gen_pt_hist2->Fill( EGCandidate.pt(), (EGCandidate.pt() - trueElectron.pt())/trueElectron.pt() );
            stage2_reco_gen_pt_hist3->Fill( EGCandidate.pt(), trueElectron.pt()/EGCandidate.pt() );
            stage2_reco_gen_pt_hist4->Fill( (EGCandidate.pt() * ptAdjustFuncStage2.Eval( EGCandidate.pt() ) ), trueElectron.pt()/(EGCandidate.pt() * ptAdjustFuncStage2.Eval( EGCandidate.pt() ) ) );
            stage2_reco_gen_pt_1dHist->Fill( (EGCandidate.pt() - trueElectron.pt())/trueElectron.pt() );
            stage2_reco_gen_pt_1dHist_calib->Fill( ( (EGCandidate.pt() * ptAdjustFuncStage2.Eval( EGCandidate.pt() ) ) - trueElectron.pt())/trueElectron.pt() );
            stage2_2DdeltaR_hist->Fill(trueElectron.eta()-EGCandidate.eta(), reco::deltaPhi(EGCandidate, trueElectron));


            // Also fill 2 vars in TTree
            treeinfo.stage2_pt = EGCandidate.pt();
            treeinfo.stage2_pt_calib = EGCandidate.pt() * ptAdjustFuncStage2.Eval( EGCandidate.pt() );
            break;
         }
      }
      
      
      //for(const auto& eGammaCollection : eGammaCollections)
      //{
      //   const std::string &name = eGammaCollection.first;
      //   for(const auto& EGCandidate : eGammaCollection.second)
      //   {
      //      if ( reco::deltaR(EGCandidate.polarP4(), trueElectron) < genMatchDeltaRcut &&
      //           fabs(EGCandidate.pt()-trueElectron.pt())/trueElectron.pt() < genMatchRelPtcut )
      //      {
      //         if ( debug ) std::cout << "Filling hists for EG Collection: " << name << std::endl;
      //         EGalg_efficiency_hists[name]->Fill(trueElectron.pt());
      //         EGalg_efficiency_eta_hists[name]->Fill(trueElectron.eta());
      //         if ( offlineRecoFound )
      //         {
      //            for(auto& pair : EGalg_efficiency_reco_hists[name])
      //            {
      //               // (threshold, histogram)
      //               if (EGCandidate.pt() > pair.first)
      //                  pair.second->Fill(reco_electron_pt);
      //            }
      //         }
      //         for(auto& pair : EGalg_efficiency_gen_hists[name])
      //         {
      //            // (threshold, histogram)
      //            if (EGCandidate.pt() > pair.first)
      //               pair.second->Fill(trueElectron.pt());
      //         }
      //         EGalg_deltaR_hists[name]->Fill(reco::deltaR(EGCandidate.polarP4(), trueElectron));
      //         EGalg_deta_hists[name]->Fill(trueElectron.eta()-EGCandidate.eta());
      //         EGalg_dphi_hists[name]->Fill(reco::deltaPhi(EGCandidate.phi(), trueElectron.phi()));
      //         EGalg_reco_gen_pt_hists[name]->Fill( trueElectron.pt(), (EGCandidate.pt() - trueElectron.pt())/trueElectron.pt() );
      //         EGalg_reco_gen_pt_1dHists[name]->Fill( (EGCandidate.pt() - trueElectron.pt())/trueElectron.pt() );
      //         EGalg_2DdeltaR_hists[name]->Fill(trueElectron.eta()-EGCandidate.eta(), reco::deltaPhi(EGCandidate, trueElectron));
      //         break;
      //      }
      //   }
      //}
   }
   else // !doEfficiencyCalc
   {
      // L1EG Crystal Algo
      bool filledBasicCuts = false;
      bool filledTrackMatch = false;
      bool filledPhotonTag = false;
      bool filledLeadCand = false;
      bool filledStage2Eff = false;
      //bool filled95Eff = false;
      bool filled90Eff = false;
      bool filledL1TrkMatch = false;
      bool filledL1TrkMatchOnly = false;
      for(const auto& cluster : crystalClusters)
      {
         if ( !useEndcap && fabs(cluster.eta()) >= 1.479 ) continue;
         clusterCount++;

         // Only fill this once, the first time through
         if (!filledLeadCand) {
            filledLeadCand = true;
            treeinfo.nthCandidate = clusterCount;
            if ( fabs(cluster.eta()) > 1.479 )
               treeinfo.endcap = true;
            else
               treeinfo.endcap = false;
            if (doTracking) doTrackMatching(cluster, l1trackHandle);
            fill_tree(cluster);
            //checkRecHitsFlags(cluster, triggerPrimitives, ecalRecHits);
         }

         if ( cluster.standaloneWP() )
         {

            if (!filledBasicCuts) {
               filledBasicCuts = true;
               dyncrystal_rate_hist->Fill(cluster.pt());
               dyncrystal_rate_adj_hist->Fill( cluster.pt());
            }

            if ( cluster_passes_track_cuts(cluster, treeinfo.trackDeltaR) && (!filledTrackMatch) ) {
               filledTrackMatch = true;
               dyncrystal_track_rate_hist->Fill(cluster.pt());
               dyncrystal_track_rate_adj_hist->Fill(cluster.pt());
            }

            if ( cluster.photonWP80() && (!filledPhotonTag) ) {
               filledPhotonTag = true;
               dyncrystal_phoWindow_rate_hist->Fill(cluster.pt());
               dyncrystal_phoWindow_rate_adj_hist->Fill(cluster.pt());
            }
         }
         if ( cluster.stage2effMatch() )
         {
            if (!filledStage2Eff) {
               filledStage2Eff = true;
               dyncrystal_rate_adj_hist_stage2->Fill(cluster.pt());
            }
         }
         //if ( cluster_passes_95_cuts(cluster) )
         //{
         //   if (!filled95Eff) {
         //      filled95Eff = true;
         //      dyncrystal_rate_adj_hist_95->Fill(cluster.pt());
         //   }
         //}
         if ( cluster.electronWP90() )
         {
            if (!filled90Eff) {
               filled90Eff = true;
               dyncrystal_rate_adj_hist_90->Fill(cluster.pt());
            }
         }
         if ( cluster.looseL1TkMatchWP() && cluster_passes_track_variable_cuts(cluster, treeinfo.trackDeltaR, trackDeltaRMax) )
         {
            if (!filledL1TrkMatch) {
               filledL1TrkMatch = true;
               dyncrystal_trackl1match_rate_hist->Fill(cluster.pt());
            }
         }
         if ( cluster_passes_track_cuts(cluster, treeinfo.trackDeltaR) )
         {
            if (!filledL1TrkMatchOnly) {
               filledL1TrkMatchOnly = true;
               dyncrystal_trackl1matchOnly_rate_hist->Fill(cluster.pt());
            }
         }
      }

      // Stage-2
      if ( useEndcap )
      {
         auto& highestEGCandidate = stage2EGs[0];
         stage2_rate_hist->Fill(highestEGCandidate.pt());
         stage2_rate_hist_calib->Fill( (highestEGCandidate.pt() * ptAdjustFuncStage2.Eval( highestEGCandidate.pt() ) ) );
         //for(const auto& eg : stage2EGs) {
         //   if (eg.hwIso() > 0.) {
         //      stage2_iso_rate_hist->Fill(eg.pt());
         //      break;
         //   }
         //}
      }
      else // !useEndcap
      {
         // Can't assume the highest candidate is in the barrel
         for(const auto& candidate : stage2EGs)
         {
            if ( fabs(candidate.eta()) < 1.479 )
            {
               stage2_rate_hist->Fill(candidate.pt());
               stage2_rate_hist_calib->Fill( (candidate.pt() * ptAdjustFuncStage2.Eval( candidate.pt() ) ) );
               break;
            }
         }
         //for(const auto& candidate : stage2EGs)
         //{
         //   if ( fabs(candidate.eta()) < 1.479 && candidate.hwIso() > 0. )
         //   {
         //      stage2_iso_rate_hist->Fill(candidate.pt());
         //      break;
         //   }
         //}
      }

      //for(const auto& eGammaCollection : eGammaCollections)
      //{
      //   const std::string &name = eGammaCollection.first;
      //   if ( eGammaCollection.second.size() == 0 ) continue;
      //   if ( useEndcap )
      //   {
      //      auto& highestEGCandidate = eGammaCollection.second[0];
      //      EGalg_rate_hists[name]->Fill(highestEGCandidate.pt());
      //   }
      //   else // !useEndcap
      //   {
      //      // Can't assume the highest candidate is in the barrel
      //      for(const auto& candidate : eGammaCollection.second)
      //      {
      //         if ( fabs(candidate.eta()) < 1.479 )
      //         {
      //            EGalg_rate_hists[name]->Fill(candidate.pt());
      //            break;
      //         }
      //      }
      //   }
      //}
   }
}


// ------------ method called once each job just before starting event loop  ------------
void 
L1EGRateStudies::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
L1EGRateStudies::endJob() 
{
   // Rate or efficiency study?
   if ( !doEfficiencyCalc )
   {
      // We currently have a rate pdf, we want cdf, so we integrate (downward in pt is inclusive)
      // We normalize to 30MHz as this will be the crossing rate of filled bunches in SLHC
      // (in parallel processing mode, fill dummy hist with event counts so they can be added later)
      edm::Service<TFileService> fs;
      TH1F* event_count = fs->make<TH1F>("eventCount", "Event Count", 1, -1, 1);
      event_count->SetBinContent(1, eventCount);
      integrateDown(dyncrystal_rate_hist);
      integrateDown(dyncrystal_track_rate_hist);
      integrateDown(dyncrystal_phoWindow_rate_hist);
      integrateDown(dyncrystal_rate_adj_hist);
      integrateDown(dyncrystal_rate_adj_hist_stage2);
      //integrateDown(dyncrystal_rate_adj_hist_95);
      integrateDown(dyncrystal_rate_adj_hist_90);
      integrateDown(dyncrystal_trackl1match_rate_hist);
      integrateDown(dyncrystal_trackl1matchOnly_rate_hist);
      integrateDown(dyncrystal_track_rate_adj_hist);
      integrateDown(dyncrystal_phoWindow_rate_adj_hist);
      integrateDown(stage2_rate_hist);
      integrateDown(stage2_rate_hist_calib);
      //integrateDown(stage2_iso_rate_hist);
      //for(auto& hist : EGalg_rate_hists)
      //{
      //   integrateDown(hist.second);
      //}
   }
}

// ------------ method called when starting to processes a run  ------------
void 
L1EGRateStudies::beginRun(edm::Run const& run, edm::EventSetup const& es)
{
   //edm::ESHandle<HepPDT::ParticleDataTable> pdt;
   //es.getData(pdt);
   //if ( !ParticleTable::instance() ) ParticleTable::instance(&(*pdt));
}

// ------------ method called when ending the processing of a run  ------------
/*
void 
L1EGRateStudies::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
L1EGRateStudies::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
L1EGRateStudies::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
L1EGRateStudies::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

// ------------ user methods (ncsmith)
void 
L1EGRateStudies::integrateDown(TH1F * hist) {
   // integral includes overflow and underflow bins
   double integral=0.;
   for(int i=hist->GetNbinsX()+1; i>=0; i--)
   {
      integral += hist->GetBinContent(i);
      hist->SetBinContent(i, integral);
   }
}

void
L1EGRateStudies::fill_tree(const l1slhc::L1EGCrystalCluster& cluster) {
   for(Size_t i=0; i<treeinfo.crystal_pt.size(); ++i)
   {
      treeinfo.crystal_pt[i] = cluster.GetCrystalPt(i);
   }
   // As of 28 May 2018 cluster_pt is post-calibration
   treeinfo.cluster_pt = cluster.pt(); // Brem corrected
   treeinfo.cluster_pt_adj = cluster.GetExperimentalParam("preCalibratedPt") * 
        ( ptAdjustFuncPhaseII.Eval( cluster.GetExperimentalParam("preCalibratedPt") ) ); // Brem corrected
   printf("Get pT %.2f,  Precalibrated %.2f,  EDProd Calib %.2f,   L1Rate Calib %.2f", 
        treeinfo.gen_pt,
        cluster.GetExperimentalParam("preCalibratedPt"),
        cluster.pt(),
        treeinfo.cluster_pt_adj);
   treeinfo.cluster_ptPUCorr = cluster.PUcorrPt(); // Brem & PU corrected
   treeinfo.cluster_preCalibratedPt = cluster.GetExperimentalParam("preCalibratedPt");
   treeinfo.corePt = cluster.GetExperimentalParam("uncorrectedPt"); // 3x5 Pt
   treeinfo.E_core = cluster.GetExperimentalParam("uncorrectedE"); // 3x5 Energy
   //treeinfo.ecalPUtoPt = cluster.GetExperimentalParam("ecalPUEnergyToPt");
   treeinfo.crystalCount = cluster.GetExperimentalParam("crystalCount");
   treeinfo.cluster_energy = cluster.energy();
   treeinfo.eta = cluster.eta();
   treeinfo.phi = cluster.phi();
   treeinfo.hovere = cluster.hovere();
   treeinfo.iso = cluster.isolation();
   treeinfo.bremStrength = cluster.bremStrength();
   treeinfo.electronWP98 = cluster.electronWP98();
   treeinfo.photonWP80 = cluster.photonWP80();
   treeinfo.electronWP90 = cluster.electronWP90();
   treeinfo.looseL1TkMatchWP = cluster.looseL1TkMatchWP();
   treeinfo.stage2matchEff = cluster.stage2effMatch();
   treeinfo.passedBase = cluster.standaloneWP();
   treeinfo.passedPhoton = cluster.photonWP80();
   treeinfo.hcal_dR0p05 = cluster.GetExperimentalParam("hcal_dR0p05");
   treeinfo.hcal_dR0p075 = cluster.GetExperimentalParam("hcal_dR0p075");
   treeinfo.hcal_dR0p1 = cluster.GetExperimentalParam("hcal_dR0p1");
   treeinfo.hcal_dR0p125 = cluster.GetExperimentalParam("hcal_dR0p125");
   treeinfo.hcal_dR0p15 = cluster.GetExperimentalParam("hcal_dR0p15");
   treeinfo.hcal_dR0p2 = cluster.GetExperimentalParam("hcal_dR0p2");
   treeinfo.hcal_dR0p3 = cluster.GetExperimentalParam("hcal_dR0p3");
   treeinfo.hcal_dR0p4 = cluster.GetExperimentalParam("hcal_dR0p4");
   treeinfo.hcal_dR0p5 = cluster.GetExperimentalParam("hcal_dR0p5");
   treeinfo.hcal_dR0p05_nTowers = cluster.GetExperimentalParam("hcal_dR0p05_nTowers");
   treeinfo.hcal_dR0p075_nTowers = cluster.GetExperimentalParam("hcal_dR0p075_nTowers");
   treeinfo.hcal_dR0p1_nTowers = cluster.GetExperimentalParam("hcal_dR0p1_nTowers");
   treeinfo.hcal_dR0p125_nTowers = cluster.GetExperimentalParam("hcal_dR0p125_nTowers");
   treeinfo.hcal_dR0p15_nTowers = cluster.GetExperimentalParam("hcal_dR0p15_nTowers");
   treeinfo.hcal_dR0p2_nTowers = cluster.GetExperimentalParam("hcal_dR0p2_nTowers");
   treeinfo.hcal_dR0p3_nTowers = cluster.GetExperimentalParam("hcal_dR0p3_nTowers");
   treeinfo.hcal_dR0p4_nTowers = cluster.GetExperimentalParam("hcal_dR0p4_nTowers");
   treeinfo.hcal_dR0p5_nTowers = cluster.GetExperimentalParam("hcal_dR0p5_nTowers");
   treeinfo.e2x2 = cluster.e2x2();
   treeinfo.e2x5 = cluster.e2x5();
   treeinfo.e3x5 = cluster.e3x5();
   treeinfo.e5x5 = cluster.e5x5();
   treeinfo.uslPt = cluster.GetExperimentalParam("upperSideLobePt");
   treeinfo.lslPt = cluster.GetExperimentalParam("lowerSideLobePt");
   treeinfo.phiStripContiguous0 = cluster.GetExperimentalParam("phiStripContiguous0");
   treeinfo.phiStripOneHole0 = cluster.GetExperimentalParam("phiStripOneHole0");
   treeinfo.phiStripContiguous3p = cluster.GetExperimentalParam("phiStripContiguous3p");
   treeinfo.phiStripOneHole3p = cluster.GetExperimentalParam("phiStripOneHole3p");
   // Gen and reco pt get filled earlier
   crystal_tree->Fill();
}

//bool
//L1EGRateStudies::cluster_passes_base_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   // return true;
//   
//   // Currently this producer is optimized based on cluster isolation and shower shape
//   // the previous H/E cut has been removed for the moment.
//   // The following cut is based off of what was shown in the Phase-2 meeting
//   // 23 May 2017.  Only the barrel is considered.
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      //std::cout << "Starting passing check" << std::endl;
//      float cluster_pt = cluster.pt();
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      float clusterE5x5 = cluster.GetExperimentalParam("E5x5");
//      float cluster_iso = cluster.isolation();
//      bool passIso = false;
//      bool passShowerShape = false;
//      
//      // 250 MeV option
//      //if ( ( 0.94 + 0.11 * TMath::Exp( -0.11 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	  //passShowerShape = true; }
//      //if ( (( -0.27 + 1.5 * TMath::Exp( -0.013 * cluster_pt )) > cluster_iso ) ) {
//      //    passIso = true; }
//      //if ( passShowerShape && passIso ) {
//      //    //std::cout << " --- Passed!" << std::endl;
//	  //    return true; }
//
//	  // 500 MeV
//      if ( ( 0.94 + 0.052 * TMath::Exp( -0.044 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	  passShowerShape = true; }
//      if ( cluster_pt < 80 ) {
//         if ( ( 0.85 + -0.0080 * cluster_pt ) > cluster_iso ) passIso = true;
//      }
//      if ( cluster_pt >= 80 ) { // do flat line extension of isolation cut
//         if ( 0.21 > cluster_iso ) passIso = true;
//      }
//      if ( passShowerShape && passIso ) {
//          //std::cout << " --- Passed!" << std::endl;
//	      return true; }
//   }
//   return false;
//}
//
//bool
//L1EGRateStudies::cluster_passes_stage2_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      float cluster_pt = cluster.pt();
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      float clusterE5x5 = cluster.GetExperimentalParam("E5x5");
//      float cluster_iso = cluster.isolation();
//      float cluster_hovere = cluster.hovere();
//      bool passIso = false;
//      bool passShowerShape = false;
//      bool passHoverE = false;
//
//      // Stage-2 Matching
//      
//      if ( ( 0.94 + 0.04 * TMath::Exp( -0.02 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	     passShowerShape = true; }
//      if ( ( 0.22 + 1.4 * TMath::Exp( -0.08 * cluster_pt ) > cluster_iso ) ) {
//	     passIso = true; }
//      if ( ( 0.27 + 2.7 * TMath::Exp( -0.07 * cluster_pt ) > cluster_hovere ) ) {
//	     passHoverE = true; }
//
//      if ( passShowerShape && passIso && passHoverE ) {
//	      return true; }
//   }
//   return false;
//}
//
//bool
//L1EGRateStudies::cluster_passes_l1tkMatch_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      float cluster_pt = cluster.pt();
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      float clusterE5x5 = cluster.GetExperimentalParam("E5x5");
//      float cluster_iso = cluster.isolation();
//      bool passIso = false;
//      bool passShowerShape = false;
//
//      // 95% plateau
//      if ( ( 0.944 + -0.65 * TMath::Exp( -0.4 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	     passShowerShape = true; }
//      if ( ( 0.38 + 1.9 * TMath::Exp( -0.05 * cluster_pt ) > cluster_iso ) ) {
//	     passIso = true; }
//
//      if ( passShowerShape && passIso ) {
//	      return true; }
//
//   }
//   return false;
//}
//
//bool
//L1EGRateStudies::cluster_passes_95_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      float cluster_pt = cluster.pt();
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      float clusterE5x5 = cluster.GetExperimentalParam("E5x5");
//      float cluster_iso = cluster.isolation();
//      bool passIso = false;
//      bool passShowerShape = false;
//
//      // 95% plateau
//      if ( ( 0.95 + 0.03 * TMath::Exp( -0.05 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	     passShowerShape = true; }
//      if ( ( 0.085 + 1.9 * TMath::Exp( -0.05 * cluster_pt ) > cluster_iso ) ) {
//	     passIso = true; }
//
//      if ( passShowerShape && passIso ) {
//	      return true; }
//
//   }
//   return false;
//}
//
//bool
//L1EGRateStudies::cluster_passes_90_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      float cluster_pt = cluster.pt();
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      float clusterE5x5 = cluster.GetExperimentalParam("E5x5");
//      float cluster_iso = cluster.isolation();
//      float cluster_hovere = cluster.hovere();
//      bool passIso = false;
//      bool passShowerShape = false;
//      bool passHoverE = false;
//
//      // 90% plateau
//      if ( ( 0.95 + 0.043 * TMath::Exp( -0.055 * cluster_pt ) < (clusterE2x5 / clusterE5x5)) ) {
//	     passShowerShape = true; }
//      if ( ( 0.067 + 1.6 * TMath::Exp( -0.055 * cluster_pt ) > cluster_iso ) ) {
//	     passIso = true; }
//      if ( ( 0.26 + 4.4 * TMath::Exp( -0.089 * cluster_pt ) > cluster_hovere ) ) {
//	     passHoverE = true; }
//
//      if ( passShowerShape && passIso && passHoverE ) {
//	      return true; }
//   }
//   return false;
//}

bool
L1EGRateStudies::cluster_passes_track_variable_cuts(const l1slhc::L1EGCrystalCluster& cluster, float trackDeltaR, float deltaRMax) const {
   // return true;
   
   // Add track cut
   if ( fabs(cluster.eta()) < 1.479 )
   {
      //std::cout << "Starting passing check" << std::endl;
      if ( trackDeltaR < deltaRMax ) {
         return true; }
   }
   return false;
}

bool
L1EGRateStudies::cluster_passes_track_cuts(const l1slhc::L1EGCrystalCluster& cluster, float trackDeltaR) const {
   // return true;
   
   // Add track cut
   if ( fabs(cluster.eta()) < 1.479 )
   {
      //std::cout << "Starting passing check" << std::endl;
      if ( trackDeltaR < 0.05 ) {
         return true; }
   }
   return false;
}

//bool
//L1EGRateStudies::cluster_passes_photon_cuts(const l1slhc::L1EGCrystalCluster& cluster) const {
//   // return true;
//   
//   // Add track cut
//   if ( fabs(cluster.eta()) < 1.479 )
//   {
//      float clusterE2x2 = cluster.GetExperimentalParam("E2x2");
//      float clusterE2x5 = cluster.GetExperimentalParam("E2x5");
//      if ( clusterE2x2/clusterE2x5 > 0.96 - 0.0003 * cluster.pt() ) {
//         return true; }
//   }
//   return false;
//}

bool
L1EGRateStudies::checkTowerExists(const l1slhc::L1EGCrystalCluster &cluster, const EcalTrigPrimDigiCollection &tps) const {
   for (const auto& tp : tps)
   {
      if ( tp.id() == ((EBDetId) cluster.seedCrystal()).tower() )
      {
         if ( tp.compressedEt() > 0 )
         {
            return true;
         }
         break;
      }
   }
   return false;
}

//void
//L1EGRateStudies::checkRecHitsFlags(const l1slhc::L1EGCrystalCluster &cluster, const EcalTrigPrimDigiCollection &tps, const EcalRecHitCollection &ecalRecHits) const {
//   if ( cluster.standaloneWP() )
//   {
//      if ( debug ) std::cout << "Event (pt = " << cluster.pt() << ") passed cuts, ";
//      if ( debug && checkTowerExists(cluster, tps) )
//         std::cout << "\x1B[32mtower exists!\x1B[0m" << std::endl;
//      else if ( debug )
//         std::cout << "\x1B[31mtower does not exist!\x1B[0m" << std::endl;
//      if ( debug ) std::cout << "Here are the cluster seed crystal reco flags:" << std::endl;
//      for(auto hit : ecalRecHits)
//      {
//         if( hit.id() == cluster.seedCrystal() )
//         {
//            const std::map<int, std::string> flagDefs {
//               { EcalRecHit::kGood, "channel ok, the energy and time measurement are reliable" },
//               { EcalRecHit::kPoorReco, "the energy is available from the UncalibRecHit, but approximate (bad shape, large chi2)" },
//               { EcalRecHit::kOutOfTime, "the energy is available from the UncalibRecHit (sync reco), but the event is out of time" },
//               { EcalRecHit::kFaultyHardware, "The energy is available from the UncalibRecHit, channel is faulty at some hardware level (e.g. noisy)" },
//               { EcalRecHit::kNoisy, "the channel is very noisy" },
//               { EcalRecHit::kPoorCalib, "the energy is available from the UncalibRecHit, but the calibration of the channel is poor" },
//               { EcalRecHit::kSaturated, "saturated channel (recovery not tried)" },
//               { EcalRecHit::kLeadingEdgeRecovered, "saturated channel: energy estimated from the leading edge before saturation" },
//               { EcalRecHit::kNeighboursRecovered, "saturated/isolated dead: energy estimated from neighbours" },
//               { EcalRecHit::kTowerRecovered, "channel in TT with no data link, info retrieved from Trigger Primitive" },
//               { EcalRecHit::kDead, "channel is dead and any recovery fails" },
//               { EcalRecHit::kKilled, "MC only flag: the channel is{ EcalRecHit::killed in the real detector" },
//               { EcalRecHit::kTPSaturated, "the channel is in a region with saturated TP" },
//               { EcalRecHit::kL1SpikeFlag, "the channel is in a region with TP with sFGVB = 0" },
//               { EcalRecHit::kWeird, "the signal is believed to originate from an anomalous deposit (spike) " },
//               { EcalRecHit::kDiWeird, "the signal is anomalous, and neighbors another anomalous signal  " },
//               { EcalRecHit::kHasSwitchToGain6, "at least one data frame is in G6" },
//               { EcalRecHit::kHasSwitchToGain1, "at least one data frame is in G1" }
//            };
//            for(auto& flag : flagDefs)
//            {
//               if ( hit.checkFlag(flag.first) )
//               {
//                  if ( flag.first == EcalRecHit::kGood && debug ) std::cout << "\x1B[32m";
//                  if ( debug ) std::cout << "    " << flag.second << std::endl;
//                  if ( flag.first == EcalRecHit::kGood && debug ) std::cout << "\x1B[0m";
//
//                  if ( checkTowerExists(cluster, tps) )
//                     RecHitFlagsTowerHist->Fill(flag.first);
//                  else
//                     RecHitFlagsNoTowerHist->Fill(flag.first);
//               }
//            }
//         }
//      }
//   }
//}


void
L1EGRateStudies::doTrackMatching(const l1slhc::L1EGCrystalCluster& cluster, edm::Handle<L1TkTrackCollectionType> l1trackHandle)
{
  // track matching stuff
  // match to closes track up to delta R = 0.3
  // then match to the highest pt track < 0.3
  double min_track_dr = 999.;
  double max_track_pt = 0.;
  double max_track_pt_all_tracks = 0.;
  double max_track_pt_all_tracksEta = 0.;
  double max_track_pt_all_tracksPhi = 0.;
  double max_track_pt_all_tracksChi2 = 0.;
  double max_track_pt_all_chi2_cut = 0.;
  double max_track_pt_all_chi2_cutEta = 0.;
  double max_track_pt_all_chi2_cutPhi = 0.;
  double max_track_pt_all_chi2_cutChi2 = 0.;
  double matched_z = 999.;
  //edm::Ptr<TTTrack<Ref_PixelDigi_>> matched_track;
  bool foundMatchedTrack = false;
  edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> matched_track;
  if ( l1trackHandle.isValid() )
  {
     for(size_t track_index=0; track_index<l1trackHandle->size(); ++track_index)
     {
        //edm::Ptr<TTTrack<Ref_PixelDigi_>> ptr(l1trackHandle, track_index);
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> ptr(l1trackHandle, track_index);
        double pt = ptr->getMomentum().perp();

        // Don't consider tracks with pt < 2 for studies, might be increased to 3 later
        if (pt < 2.) continue;

        // Record the highest pt track per event
        // And, highest pt passing chi2 cut
        // to see if brem is an issues for mis-matched tracks
        double chi2 = ptr->getChi2();
        if (pt > max_track_pt_all_tracks) {
          max_track_pt_all_tracks = pt;
          max_track_pt_all_tracksEta = ptr->getMomentum().eta();
          max_track_pt_all_tracksPhi = ptr->getMomentum().phi();
          max_track_pt_all_tracksChi2 = chi2;}
        if (pt > max_track_pt_all_chi2_cut ) { // && chi2 < 100) {
          max_track_pt_all_chi2_cut = pt;
          max_track_pt_all_chi2_cutEta = ptr->getMomentum().eta();
          max_track_pt_all_chi2_cutPhi = ptr->getMomentum().phi();
          max_track_pt_all_chi2_cutChi2 = chi2;}
        

        // L1 Tracks are considered mis-measured if pt > 50
        // Therefore pt -> 50 if pt > 50
        // Only consider tracks if chi2 < 100
        //double dr = .1;
        double dr = L1TkElectronTrackMatchAlgo::deltaR(L1TkElectronTrackMatchAlgo::calorimeterPosition(cluster.phi(), cluster.eta(), cluster.energy()), ptr);
        //if (pt > 50.) pt = 50;
        // Choose closest track until dR < 0.3
        if ( dr < min_track_dr && min_track_dr > 0.3 ) // && chi2 < 100. )
        {
           min_track_dr = dr;
           max_track_pt = pt;
           matched_track = ptr;
           foundMatchedTrack = true;
           matched_z = ptr->getPOCA().z();
        }
        // If dR < 0.3, choose highest pt track
        else if ( dr < 0.3 && pt > max_track_pt ) // && chi2 < 100. )
        {
           min_track_dr = dr;
           max_track_pt = pt;
           matched_track = ptr;
           foundMatchedTrack = true;
           matched_z = ptr->getPOCA().z();
        }
     }
     float isoConeTrackCount = 0.; // matched track will be in deltaR cone
     float isoConePtSum = 0.;
     for(size_t track_index=0; track_index<l1trackHandle->size(); ++track_index)
     {
        //edm::Ptr<TTTrack<Ref_PixelDigi_>> ptr(l1trackHandle, track_index);
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> ptr(l1trackHandle, track_index);
        // don't double count the matched_track
        if ( ptr == matched_track ) {
          continue;
        }
        // Don't consider tracks with pt < 2 for studies, might be increased to 3 later
        double pt = ptr->getMomentum().perp();
        if (pt < 2.) continue;

        // Track Isolation cuts have been updated based on the recommendations here:
        // https://twiki.cern.ch/twiki/bin/viewauth/CMS/L1TrackTriggerObjects62X#Matching_L1EG_objects_with_L1Tra
        // Inner dR cone of 0.03, outer dR cone 0.2, momentum at least 2GeV
        // L1 Tracks are considered mis-measured if pt > 50
        // Therefore pt -> 50 if pt > 50
        // Only consider tracks if chi2 < 100
        // Only consider iso tracks from within dZ < 0.6
        double dr_2 = 99.;
        if (foundMatchedTrack) {
           dr_2 = reco::deltaR(ptr->getMomentum(), matched_track->getMomentum());}
        //double chi2 = ptr->getChi2();
        //if (pt > 50.) pt = 50;
        double this_z = ptr->getPOCA().z();
        double deltaZ = abs(matched_z - this_z);
        if ( dr_2 < 0.2 && dr_2 > 0.03 && pt > 2. && deltaZ < 0.6 ) // chi2 < 100 && deltaZ < 0.6 )
        {
          isoConeTrackCount++;
          isoConePtSum += pt;
        }
     }
//
//     // Trying a track-based PU estimation for abs(dEta) <= 13*0.0173 && abs(dPhi) <= 113*0.0173
//     // using the same window as calo based PU
//     float PUTrackPtGlobalDiffZ = 0.;
//     float PUTrackPtGlobalDiffZandPt = 0.;
//     float PUTrackPtGlobalSameZ = 0.;
//     float PUTrackPtGlobalAll = 0.;
//     float PUTrackPt13x113DiffZ = 0.;
//     float PUTrackPt13x113DiffZandPt = 0.;
//     float PUTrackPt13x113SameZ = 0.;
//     float PUTrackPt13x113All = 0.;
//     float PUTrackPt3x5DiffZ = 0.;
//     float PUTrackPt3x5DiffZandPt = 0.;
//     float PUTrackPt3x5SameZ = 0.;
//     float PUTrackPt3x5All = 0.;
//     float PUTrackPtECalIsoConeDiffZ = 0.;
//     float PUTrackPtECalIsoConeDiffZandPt = 0.;
//     float PUTrackPtECalIsoConeSameZ = 0.;
//     float PUTrackPtECalIsoConeAll = 0.;
//     float PUTrackPtTkIsoConeDiffZ = 0.;
//     float PUTrackPtTkIsoConeDiffZandPt = 0.;
//     float PUTrackPtTkIsoConeSameZ = 0.;
//     float PUTrackPtTkIsoConeAll = 0.;
//     float PUTrackCntGlobalDiffZ = 0.;
//     float PUTrackCntGlobalDiffZandPt = 0.;
//     float PUTrackCntGlobalSameZ = 0.;
//     float PUTrackCntGlobalAll = 0.;
//     float PUTrackCnt13x113DiffZ = 0.;
//     float PUTrackCnt13x113DiffZandPt = 0.;
//     float PUTrackCnt13x113SameZ = 0.;
//     float PUTrackCnt13x113All = 0.;
//     float PUTrackCnt3x5DiffZ = 0.;
//     float PUTrackCnt3x5DiffZandPt = 0.;
//     float PUTrackCnt3x5SameZ = 0.;
//     float PUTrackCnt3x5All = 0.;
//     float PUTrackCntECalIsoConeDiffZ = 0.;
//     float PUTrackCntECalIsoConeDiffZandPt = 0.;
//     float PUTrackCntECalIsoConeSameZ = 0.;
//     float PUTrackCntECalIsoConeAll = 0.;
//     float PUTrackCntTkIsoConeDiffZ = 0.;
//     float PUTrackCntTkIsoConeDiffZandPt = 0.;
//     float PUTrackCntTkIsoConeSameZ = 0.;
//     float PUTrackCntTkIsoConeAll = 0.;
//     //int trkCnt = 0;
//     for(size_t track_index=0; track_index<l1trackHandle->size(); ++track_index)
//     {
//       //edm::Ptr<TTTrack<Ref_PixelDigi_>> ptr(l1trackHandle, track_index);
//       edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> ptr(l1trackHandle, track_index);
//
//       // Cleaning section
//       // don't double count the matched_track
//       if ( ptr == matched_track ) continue;
//       // Don't consider tracks with pt < 2 for studies
//       // Don't consider track with pt > 5 b/c they aren't PU
//       double pt = ptr->getMomentum().perp();
//       double chi2 = ptr->getChi2();
//       //if (pt < 2. || pt > 5.) continue;
//       //trkCnt++;
//       //std::cout << trkCnt << " - Pt: " << pt << " Chi2: " << chi2 << std::endl;
//       if (pt < 2.) continue;
//       if (pt > 50.) pt = 50;
//       if (chi2 > 100.) continue;
//
//       // Categories
//       // 1. ECal PU Window 13x113
//       // 2. ECal Iso Window: 27x27
//       // 3. Trk Iso Window: dr < 0.2
//       // 4. Signal Region 3x5
//       // 5. Global!
//        
//       // Reject tracks not matching any of these areas
//       float trackDEta = L1TkElectronTrackMatchAlgo::deltaEta(L1TkElectronTrackMatchAlgo::calorimeterPosition(cluster.phi(), cluster.eta(), cluster.energy()), ptr);
//       float trackDPhi = L1TkElectronTrackMatchAlgo::deltaPhi(L1TkElectronTrackMatchAlgo::calorimeterPosition(cluster.phi(), cluster.eta(), cluster.energy()), ptr);
//
//       // Additional vars for following categories
//       float dr_2 = 99.;
//       if (foundMatchedTrack) {
//          dr_2 = reco::deltaR(ptr->getMomentum(), matched_track->getMomentum());}
//       float this_z = ptr->getPOCA().z();
//       float dz = abs(matched_z - this_z);
//
//       // Whole detector
//       PUTrackPtGlobalAll += pt;
//       PUTrackCntGlobalAll++;
//       if (dz < 0.6) {
//          PUTrackPtGlobalSameZ += pt;
//          PUTrackCntGlobalSameZ++;
//       }
//       if (dz > 0.6) {
//          PUTrackPtGlobalDiffZ += pt;
//          PUTrackCntGlobalDiffZ++;
//       }
//       if (dz > 0.6 && pt < 5.) {
//          PUTrackPtGlobalDiffZandPt += pt;
//          PUTrackCntGlobalDiffZandPt++;
//       }
//
//    // Going from integral crystal indices to track distance that could hit the farther edge of a crystal
//    if (fabs(trackDEta) > 13.5*0.0173 ) continue; // ECal Iso is widest in Eta, this cut is == dR > 0.23
//    if (fabs(trackDPhi) > 56.5*0.0173 ) continue; // ECal PU is widest in Phi
//
//    // Now many categories
//    // 13x113 ECal PU Region
//    if (fabs(trackDEta) < 6.5*0.0173 && fabs(trackDPhi) < 56.5*0.0173) {
//             PUTrackPt13x113All += pt;
//             PUTrackCnt13x113All++;
//        if (dz < 0.6) {
//                 PUTrackPt13x113SameZ += pt;
//                 PUTrackCnt13x113SameZ++;
//        }
//        if (dz > 0.6) {
//                 PUTrackPt13x113DiffZ += pt;
//                 PUTrackCnt13x113DiffZ++;
//        }
//        if (dz > 0.6 && pt < 5.) {
//                 PUTrackPt13x113DiffZandPt += pt;
//                 PUTrackCnt13x113DiffZandPt++;
//        }
//    } // end 13x113 ECal PU Region
//        
//    // 3x5 Cluster Core
//    if (fabs(trackDEta) < 1.5*0.0173 && fabs(trackDPhi) < 2.5*0.0173) {
//             PUTrackPt3x5All += pt;
//             PUTrackCnt3x5All++;
//        if (dz < 0.6) {
//                 PUTrackPt3x5SameZ += pt;
//                 PUTrackCnt3x5SameZ++;
//        }
//        if (dz > 0.6) {
//                 PUTrackPt3x5DiffZ += pt;
//                 PUTrackCnt3x5DiffZ++;
//        }
//        if (dz > 0.6 && pt < 5.) {
//                 PUTrackPt3x5DiffZandPt += pt;
//                 PUTrackCnt3x5DiffZandPt++;
//        }
//    } // end 3x5 Cluster Core
//        
//    // ECal Iso Cone
//    if (fabs(trackDEta) < 13.5*0.0173 && fabs(trackDPhi) < 13.5*0.0173) {
//             PUTrackPtECalIsoConeAll += pt;
//             PUTrackCntECalIsoConeAll++;
//        if (dz < 0.6) {
//                 PUTrackPtECalIsoConeSameZ += pt;
//                 PUTrackCntECalIsoConeSameZ++;
//        }
//        if (dz > 0.6) {
//                 PUTrackPtECalIsoConeDiffZ += pt;
//                 PUTrackCntECalIsoConeDiffZ++;
//        }
//        if (dz > 0.6 && pt < 5.) {
//                 PUTrackPtECalIsoConeDiffZandPt += pt;
//                 PUTrackCntECalIsoConeDiffZandPt++;
//        }
//    } // end ECal Iso Cone
//        
//    // Track Iso Cone
//    if (dr_2 < 0.2) {
//             PUTrackPtTkIsoConeAll += pt;
//             PUTrackCntTkIsoConeAll++;
//        if (dz < 0.6) {
//                 PUTrackPtTkIsoConeSameZ += pt;
//                 PUTrackCntTkIsoConeSameZ++;
//        }
//        if (dz > 0.6) {
//                 PUTrackPtTkIsoConeDiffZ += pt;
//                 PUTrackCntTkIsoConeDiffZ++;
//        }
//        if (dz > 0.6 && pt < 5.) {
//                 PUTrackPtTkIsoConeDiffZandPt += pt;
//                 PUTrackCntTkIsoConeDiffZandPt++;
//        }
//    } // end Track Iso Cone
//     } // end PU Tracks


     if (foundMatchedTrack) {
        treeinfo.trackRInv = matched_track->getRInv();
        treeinfo.trackChi2 = matched_track->getChi2();
        treeinfo.trackZ = matched_track->getPOCA().z();
        treeinfo.trackDeltaPhi = L1TkElectronTrackMatchAlgo::deltaPhi(L1TkElectronTrackMatchAlgo::calorimeterPosition(cluster.phi(), cluster.eta(), cluster.energy()), matched_track);
        treeinfo.trackDeltaEta = L1TkElectronTrackMatchAlgo::deltaEta(L1TkElectronTrackMatchAlgo::calorimeterPosition(cluster.phi(), cluster.eta(), cluster.energy()), matched_track);
        treeinfo.trackEta = matched_track->getMomentum().eta();
        treeinfo.trackPhi = matched_track->getMomentum().phi();
        treeinfo.trackMomentum = matched_track->getMomentum().mag();
        treeinfo.trackRInv = matched_track->getRInv();
        treeinfo.trackChi2 = matched_track->getChi2();
     }
     else {
        treeinfo.trackRInv = -99.;
        treeinfo.trackChi2 = -99.;
        treeinfo.trackZ = -99.;
        treeinfo.trackDeltaPhi = -99.;
        treeinfo.trackDeltaEta = -99.;
        treeinfo.trackEta = -99.;
        treeinfo.trackPhi = -99.;
        treeinfo.trackMomentum = -99.;
        treeinfo.trackRInv = -99.;
        treeinfo.trackChi2 = -99.;
     }
     treeinfo.trackDeltaR = min_track_dr;
     treeinfo.trackPt = max_track_pt;
     treeinfo.trackHighestPt = max_track_pt_all_tracks;
     treeinfo.trackHighestPtEta = max_track_pt_all_tracksEta;
     treeinfo.trackHighestPtPhi = max_track_pt_all_tracksPhi;
     treeinfo.trackHighestPtChi2 = max_track_pt_all_tracksChi2;
     treeinfo.trackHighestPtCutChi2 = max_track_pt_all_chi2_cut;
     treeinfo.trackHighestPtCutChi2Eta = max_track_pt_all_chi2_cutEta;
     treeinfo.trackHighestPtCutChi2Phi = max_track_pt_all_chi2_cutPhi;
     treeinfo.trackHighestPtCutChi2Chi2 = max_track_pt_all_chi2_cutChi2;
     treeinfo.trackIsoConeTrackCount = isoConeTrackCount;
     treeinfo.trackIsoConePtSum = isoConePtSum;
     //treeinfo.passedTrack = (cluster_passes_track_cuts(cluster, min_track_dr) && cluster.standaloneWP());
     //treeinfo.trackPUTrackPtGlobalDiffZ = PUTrackPtGlobalDiffZ;
     //treeinfo.trackPUTrackPtGlobalDiffZandPt = PUTrackPtGlobalDiffZandPt;
     //treeinfo.trackPUTrackPtGlobalSameZ = PUTrackPtGlobalSameZ;
     //treeinfo.trackPUTrackPtGlobalAll = PUTrackPtGlobalAll;
     //treeinfo.trackPUTrackPt13x113DiffZ = PUTrackPt13x113DiffZ;
     //treeinfo.trackPUTrackPt13x113DiffZandPt = PUTrackPt13x113DiffZandPt;
     //treeinfo.trackPUTrackPt13x113SameZ = PUTrackPt13x113SameZ;
     //treeinfo.trackPUTrackPt13x113All = PUTrackPt13x113All;
     //treeinfo.trackPUTrackPt3x5DiffZ = PUTrackPt3x5DiffZ;
     //treeinfo.trackPUTrackPt3x5DiffZandPt = PUTrackPt3x5DiffZandPt;
     //treeinfo.trackPUTrackPt3x5SameZ = PUTrackPt3x5SameZ;
     //treeinfo.trackPUTrackPt3x5All = PUTrackPt3x5All;
     //treeinfo.trackPUTrackPtECalIsoConeDiffZ = PUTrackPtECalIsoConeDiffZ;
     //treeinfo.trackPUTrackPtECalIsoConeDiffZandPt = PUTrackPtECalIsoConeDiffZandPt;
     //treeinfo.trackPUTrackPtECalIsoConeSameZ = PUTrackPtECalIsoConeSameZ;
     //treeinfo.trackPUTrackPtECalIsoConeAll = PUTrackPtECalIsoConeAll;
     //treeinfo.trackPUTrackPtTkIsoConeDiffZ = PUTrackPtTkIsoConeDiffZ;
     //treeinfo.trackPUTrackPtTkIsoConeDiffZandPt = PUTrackPtTkIsoConeDiffZandPt;
     //treeinfo.trackPUTrackPtTkIsoConeSameZ = PUTrackPtTkIsoConeSameZ;
     //treeinfo.trackPUTrackPtTkIsoConeAll = PUTrackPtTkIsoConeAll;
     //treeinfo.trackPUTrackCntGlobalDiffZ = PUTrackCntGlobalDiffZ;
     //treeinfo.trackPUTrackCntGlobalDiffZandPt = PUTrackCntGlobalDiffZandPt;
     //treeinfo.trackPUTrackCntGlobalSameZ = PUTrackCntGlobalSameZ;
     //treeinfo.trackPUTrackCntGlobalAll = PUTrackCntGlobalAll;
     //treeinfo.trackPUTrackCnt13x113DiffZ = PUTrackCnt13x113DiffZ;
     //treeinfo.trackPUTrackCnt13x113DiffZandPt = PUTrackCnt13x113DiffZandPt;
     //treeinfo.trackPUTrackCnt13x113SameZ = PUTrackCnt13x113SameZ;
     //treeinfo.trackPUTrackCnt13x113All = PUTrackCnt13x113All;
     //treeinfo.trackPUTrackCnt3x5DiffZ = PUTrackCnt3x5DiffZ;
     //treeinfo.trackPUTrackCnt3x5DiffZandPt = PUTrackCnt3x5DiffZandPt;
     //treeinfo.trackPUTrackCnt3x5SameZ = PUTrackCnt3x5SameZ;
     //treeinfo.trackPUTrackCnt3x5All = PUTrackCnt3x5All;
     //treeinfo.trackPUTrackCntECalIsoConeDiffZ = PUTrackCntECalIsoConeDiffZ;
     //treeinfo.trackPUTrackCntECalIsoConeDiffZandPt = PUTrackCntECalIsoConeDiffZandPt;
     //treeinfo.trackPUTrackCntECalIsoConeSameZ = PUTrackCntECalIsoConeSameZ;
     //treeinfo.trackPUTrackCntECalIsoConeAll = PUTrackCntECalIsoConeAll;
     //treeinfo.trackPUTrackCntTkIsoConeDiffZ = PUTrackCntTkIsoConeDiffZ;
     //treeinfo.trackPUTrackCntTkIsoConeDiffZandPt = PUTrackCntTkIsoConeDiffZandPt;
     //treeinfo.trackPUTrackCntTkIsoConeSameZ = PUTrackCntTkIsoConeSameZ;
     //treeinfo.trackPUTrackCntTkIsoConeAll = PUTrackCntTkIsoConeAll;
     if ( debug ) std::cout << "Track dr: " << min_track_dr << ", chi2: " << treeinfo.trackChi2 << ", dp: " << (treeinfo.trackMomentum-cluster.energy())/cluster.energy() << std::endl;
  }
}

size_t
L1EGRateStudies::getBestTrack(double dR_cut, const reco::Candidate::PolarLorentzVector &p4, edm::Handle<L1TkTrackCollectionType> l1trackHandle)
{
  // track matching stuff
  // match to the highest pt track < 0.3
  // if no track within dR < 0.3, return null track
  size_t matched_track_index = 9999;
  if ( l1trackHandle.isValid() )
  {
     double max_track_pt = 0.;
     edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> matched_track;
     for(size_t track_index=0; track_index<l1trackHandle->size(); ++track_index)
     {
        edm::Ptr<TTTrack<Ref_Phase2TrackerDigi_>> ptr(l1trackHandle, track_index);
        double pt = ptr->getMomentum().perp();

        // Don't consider tracks with pt < 2 for studies, might be increased to 3 later
        if (pt < 2.) continue;

        // Record the highest pt track per event
        // And, highest pt passing chi2 cut
        // to see if brem is an issues for mis-matched tracks
        //double chi2 = ptr->getChi2();
        
        // L1 Tracks are considered mis-measured if pt > 50
        // Therefore pt -> 50 if pt > 50
        // Only consider tracks if chi2 < 100
        //double dr = .1;
        double dr = L1TkElectronTrackMatchAlgo::deltaR(L1TkElectronTrackMatchAlgo::calorimeterPosition(p4.phi(), p4.eta(), p4.energy()), ptr);
        //if (pt > 50.) pt = 50;
        // If dR < 0.3, choose highest pt track
        if ( dr < dR_cut && pt > max_track_pt ) // && chi2 < 100. )
        {
           max_track_pt = pt;
           matched_track = ptr;
           matched_track_index = track_index;
        }
     } // end track loop
  } // end isValid

  return matched_track_index;
}



//define this as a plug-in
DEFINE_FWK_MODULE(L1EGRateStudies);
