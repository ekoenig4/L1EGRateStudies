import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('REPR',eras.Phase2C4_trigger)
 
# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2023D35Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D35_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.MessageLogger.categories = cms.untracked.vstring('L1CaloJets', 'FwkReport')
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
   reportEvery = cms.untracked.int32(1)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50) )

process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(),
   #fileNames = cms.untracked.vstring('/store/mc/PhaseIIMTDTDRAutumn18DR/NeutrinoGun_E_10GeV/FEVT/PU200_103X_upgrade2023_realistic_v2-v1/40000/DA20A045-9075-4240-BC0E-FBFAB6F65484.root'),
   dropDescendantsOfDroppedBranches=cms.untracked.bool(False),
   inputCommands = cms.untracked.vstring(
                    "keep *",
                    "drop l1tEMTFHitExtras_simEmtfDigis_CSC_HLT",
                    "drop l1tEMTFHitExtras_simEmtfDigis_RPC_HLT",
                    "drop l1tEMTFTrackExtras_simEmtfDigis__HLT",
                    "drop l1tEMTFHit2016Extras_simEmtfDigis_CSC_HLT",
                    "drop l1tEMTFHit2016Extras_simEmtfDigis_RPC_HLT",
                    "drop l1tEMTFHit2016s_simEmtfDigis__HLT",
                    "drop l1tEMTFTrack2016Extras_simEmtfDigis__HLT",
                    "drop l1tEMTFTrack2016s_simEmtfDigis__HLT",
                    "drop l1tHGCalTowerMapBXVector_hgcalTriggerPrimitiveDigiProducer_towerMap_HLT",
                    "drop PCaloHits_g4SimHits_EcalHitsEB_SIM",
                    "drop PCaloHits_g4SimHits_HGCHitsEE_SIM",
                    "drop HGCalDetIdHGCSampleHGCDataFramesSorted_mix_HGCDigisEE_HLT",

   )
)

# ---- Global Tag :
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_upgrade2023_realistic_v2', '') 


# Add HCAL Transcoder
process.load('SimCalorimetry.HcalTrigPrimProducers.hcaltpdigi_cff')
process.load('CalibCalorimetry.CaloTPG.CaloTPGTranscoder_cfi')


process.L1simulation_step = cms.Path(process.SimL1Emulator)

### Based on: L1Trigger/L1TCommon/test/reprocess_test_10_4_0_mtd5.py
### This code is a portion of what is imported and excludes the 'schedule' portion
### of the two lines below.  It makes the test script run!
### from L1Trigger.Configuration.customiseUtils import L1TrackTriggerTracklet
### process = L1TrackTriggerTracklet(process)
process.load('L1Trigger.TrackFindingTracklet.L1TrackletTracks_cff')
process.L1TrackTriggerTracklet_step = cms.Path(process.L1TrackletTracksWithAssociators)





# --------------------------------------------------------------------------------------------
#
# ----    Produce the L1EGCrystal clusters using Emulator

process.load('L1Trigger.L1CaloTrigger.L1EGammaCrystalsEmulatorProducer_cfi')
process.L1EGammaClusterEmuProducer.ecalTPEB = cms.InputTag("simEcalEBTriggerPrimitiveDigis","","REPR")


# ----------------------------------------------------------------------------------------------
# 
# Analyzer starts here


process.analyzer = cms.EDAnalyzer("L1TowerAnalyzer",
    HcalTpEtMin = cms.double(0.5), # Default is 0.5 GeV
    EcalTpEtMin = cms.double(0.5), # Default is 0.5 GeV
    HGCalHadTpEtMin = cms.double(0.25), # Default is 0.5 GeV
    HGCalEmTpEtMin = cms.double(0.25), # Default is 0.5 GeV
    HFTpEtMin = cms.double(0.5), # Default is 0.5 GeV
    puThreshold = cms.double(5.0), # Default is 5 GeV
    puThresholdL1eg = cms.double(2.0), # Default is 4 GeV
    puThresholdHcalMin = cms.double(1.0), # Default is 3 GeV
    puThresholdHcalMax = cms.double(2.0), # Default is 3 GeV
    puThresholdEcalMin = cms.double(0.75), # Default is 2 GeV
    puThresholdEcalMax = cms.double(1.5), # Default is 2 GeV
    puThresholdHGCalEMMin = cms.double(1.25), # Default is 1 GeV
    puThresholdHGCalEMMax = cms.double(1.75), # Default is 1.5 GeV
    puThresholdHGCalHadMin = cms.double(0.75), # Default is 0.5 GeV
    puThresholdHGCalHadMax = cms.double(1.25), # Default is 1 GeV
    puThresholdHFMin = cms.double(10.0), # Default is 4 GeV
    puThresholdHFMax = cms.double(15.0), # Default is 10.0 GeV
    debug = cms.bool(False),
    #debug = cms.untracked.bool(True),
    vertexTag = cms.InputTag("g4SimHits","","SIM"),
    trackingVertexInitTag = cms.InputTag("mix","InitialVertices","HLT"),
    l1CaloTowers = cms.InputTag("L1EGammaClusterEmuProducer","L1CaloTowerCollection","REPR"),
    L1CrystalClustersInputTag = cms.InputTag("L1EGammaClusterEmuProducer", "L1EGXtalClusterEmulator", "REPR"),
    L1HgcalTowersInputTag = cms.InputTag("hgcalTowerProducer","HGCalTowerProcessor","REPR"),
    hcalDigis = cms.InputTag("simHcalTriggerPrimitiveDigis"),
)

process.pL1Objs = cms.Path( 
    process.L1EGammaClusterEmuProducer *
    process.analyzer
)



process.TFileService = cms.Service("TFileService", 
   fileName = cms.string( "output.root" ), 
   closeFileFast = cms.untracked.bool(True)
)



#dump_file = open("dump_file.py", "w")
#dump_file.write(process.dumpPython())


