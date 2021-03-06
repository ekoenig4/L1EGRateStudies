import ROOT
import os
from L1Trigger.L1EGRateStudies.trigHelpers import drawCMSString
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)


def make_DM_plot( label1='Gen', label2='Num. L1EGs' ) :
    gen_bin_label_map_mva = {
        1 : '#pi',
        2 : '#pi#pi^{0}s',
        3 : '#pi#pi#pi',
        4 : '#pi#pi#pi#pi^{0}s',
    }
    online_labels = {
        1 : '0 L1EGs',
        2 : '1 L1EGs',
        3 : '2+ L1EGs',
    }
    h_dm = ROOT.TH2D( 'Tau Decay Modes', 'Tau Decay Modes: %s vs. %s;%s #tau_{h} DM;%s #tau_{h} DM' % (label1, label2, label1, label2), 4,0,4,3,0,3 )
    for k, v in gen_bin_label_map_mva.iteritems() :
        h_dm.GetXaxis().SetBinLabel( k, v )
    for k, v in online_labels.iteritems() :
        h_dm.GetYaxis().SetBinLabel( k, v )
    h_dm.GetYaxis().SetTitleOffset( h_dm.GetYaxis().GetTitleOffset() * 2 )
    h_dm.SetDirectory(0)
    return h_dm

def getGenDMCode( n_prongs, n_photons ) :
    if n_prongs == 1 : 
        if n_photons == 0 : return 0
        else : return 1
    if n_prongs == 3 :
        if n_photons == 0 : return 2
        else : return 3
    else : return 0

def getL1DMCode( n_L1EGs ) :
    if n_L1EGs == 0 : return 0
    if n_L1EGs == 1 : return 1
    else : return 2


# Normalize so each row = 100%
def normalize2D( th2 ) :
    for x in range( 1, th2.GetNbinsX()+1 ) :
        colTotal = 0.
        for y in range( 1, th2.GetNbinsY()+1 ) :
            colTotal += th2.GetBinContent( x, y )
        print x, colTotal
        if colTotal == 0. : continue
        for y in range( 1, th2.GetNbinsY()+1 ) :
            th2.SetBinContent( x, y, th2.GetBinContent( x, y ) / colTotal )
    th2.GetZaxis().SetTitleOffset( 1.4 )
    th2.GetZaxis().SetTitle('Fraction of #tau_{h} per Gen DM')

def saveHists( th2, name, saveDir ) :
    ROOT.gStyle.SetOptStat(0)

    ROOT.gStyle.SetPaintTextFormat("4.0f")
    th2.SetTitle("")
    th2.Draw("COLZ TEXT")
    ROOT.gPad.SetLogz()
    ROOT.gPad.SetLeftMargin( .2 )
    ROOT.gPad.SetRightMargin( .2 )

    cmsString = drawCMSString("#bf{CMS Simulation}  <PU>=200  ggH+qqH, H#rightarrow#tau#tau")

    c.SaveAs( saveDir+name+'.png' )
    
    ROOT.gStyle.SetPaintTextFormat("4.2f")
    ROOT.gPad.SetLogz(0)
    
    normalize2D( th2 )
    c.SaveAs( saveDir+name+'_norm.png' )





c = ROOT.TCanvas('c', '', 800, 700)
    
ggH = 'output_round2_HiggsTauTau4v5.root'
ggH = 'output_round2_HiggsTauTauvL1EGsv2.root'
#ggH = 'output_round2_HiggsTauTauvL1EGsv3.root'
#ggH = 'output_round2_HiggsTauTauvL1EGsv4.root'
#ggH = 'output_round2_HiggsTauTauvL1EGsv5.root'
#ggH = 'output_round2_HiggsTauTauvL1EGsv6.root'
ggH = 'output_round2_HiggsTauTauv1.root'
version = ggH.replace('.root','')

base = '/data/truggles/l1CaloJets_20190319_r2/'
#universalSaveDir = "/afs/cern.ch/user/t/truggles/www/Phase-II/"+version+"_GenTauInHGCal/"
universalSaveDir = "/afs/cern.ch/user/t/truggles/www/Phase-II/"+version+"_GenTauInBarrelVMar20/"
if not os.path.exists( universalSaveDir ) : os.makedirs( universalSaveDir )
ggHHTTFile = ROOT.TFile( base+ggH, 'r' )
tree_ggH = ggHHTTFile.Get("analyzer/tree")

h2_loose = make_DM_plot('Gen', 'Number of RECOed L1EGs')
h2_trk = make_DM_plot('Gen', 'Num. L1EGs Trk SS')
h2_standalone = make_DM_plot('Gen', 'Num. L1EGs Standalone SS')

h2_HoverE_leq0p25_loose = make_DM_plot('Gen', 'Number of RECOed L1EGs')
h2_HoverE_leq0p25_trk = make_DM_plot('Gen', 'Num. L1EGs Trk SS')
h2_HoverE_leq0p25_standalone = make_DM_plot('Gen', 'Num. L1EGs Standalone SS')

h2_HoverE_gtr0p25_loose = make_DM_plot('Gen', 'Number of RECOed L1EGs')
h2_HoverE_gtr0p25_trk = make_DM_plot('Gen', 'Num. L1EGs Trk SS')
h2_HoverE_gtr0p25_standalone = make_DM_plot('Gen', 'Num. L1EGs Standalone SS')

cnt = 0
for row in tree_ggH :
    cnt += 1
    if cnt % 10000 == 0 : print cnt
    if row.jet_pt <= 0 : continue
    if row.genJet_pt < 30 : continue
    if abs(row.genJet_eta) > 1.3 : continue

    genDM = getGenDMCode( row.genTau_n_prongs, row.genTau_n_photons )

    h2_loose.Fill( genDM, getL1DMCode( row.l1eg_nL1EGs ) )
    h2_trk.Fill( genDM, getL1DMCode( row.l1eg_nL1EGs_trkMatchSS ) )
    h2_standalone.Fill( genDM, getL1DMCode( row.l1eg_nL1EGs_standaloneSS ) )

    h2_HoverE_leq0p25_loose.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Less0p25 ) )
    h2_HoverE_leq0p25_trk.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Less0p25_trkSS ) )
    h2_HoverE_leq0p25_standalone.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Less0p25_saSS ) )

    h2_HoverE_gtr0p25_loose.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Gtr0p25 ) )
    h2_HoverE_gtr0p25_trk.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Gtr0p25_trkSS ) )
    h2_HoverE_gtr0p25_standalone.Fill( genDM, getL1DMCode( row.n_l1eg_HoverE_Gtr0p25_saSS ) )




saveHists( h2_loose, "dm_loose", universalSaveDir )
saveHists( h2_trk, "dm_trk", universalSaveDir )
saveHists( h2_standalone, "dm_sa", universalSaveDir )


saveHists( h2_HoverE_leq0p25_loose, "dm_HoverE_leq0p25_loose", universalSaveDir )
saveHists( h2_HoverE_leq0p25_trk, "dm_HoverE_leq0p25_trk", universalSaveDir )
saveHists( h2_HoverE_leq0p25_standalone, "dm_HoverE_leq0p25_sa", universalSaveDir )


saveHists( h2_HoverE_gtr0p25_loose, "dm_HoverE_gtr0p25_loose", universalSaveDir )
saveHists( h2_HoverE_gtr0p25_trk, "dm_HoverE_gtr0p25_trk", universalSaveDir )
saveHists( h2_HoverE_gtr0p25_standalone, "dm_HoverE_gtr0p25_sa", universalSaveDir )

