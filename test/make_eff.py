import ROOT
from collections import OrderedDict
from L1Trigger.L1EGRateStudies.trigHelpers import make_efficiency_graph, make_rate_hist, setLegStyle, checkDir
import os
import re
from L1Trigger.L1EGRateStudies.trigHelpers import drawCMSString

if not os.path.exists( 'eff_and_rate_roots/' ) : os.makedirs( 'eff_and_rate_roots/' )

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
    
def MakeEff(fname):
    base,fName = os.path.split(fname.strip('.root'))
    universalSaveDir = "/afs/hep.wisc.edu/home/ekoenig4/public_html/Trigger/Efficiencies/compare/"
    checkDir( universalSaveDir )

    phase2_f = ROOT.TFile( base+'/'+fName+'.root', 'r' )
    phase2_t = phase2_f.Get('analyzer/tree')
    tyler_f = ROOT.TFile( base.replace('phase2','tyler')+'/'+fName+'.root','r')
    tyler_t = tyler_f.Get('analyzer/tree')
    

    # Threshold cuts for passing region
    pt_cut = 100
    #pt_cut = 40
    #pt_cut = 20
    pt_cut = 32
    #pt_cut = 80
    #pt_cut = 150
    #pt_cut = 200
    #pt_cut = 400
    #pt_cut = 100
    #pt_cut = 0

    def PlotEff(gP2,gT2,gS2,xname,denom_cut,pt_cut,app):
        c = ROOT.TCanvas('c', 'c', 900, 900)
        p = ROOT.TPad('p','p', 0, 0, 1, 1)
        p.Draw()
        p.cd()
        gP2.SetMinimum( 0. )
        gP2.SetLineColor(ROOT.kRed)
        gP2.SetLineWidth(2)
        gT2.SetLineColor(ROOT.kBlue)
        gT2.SetLineWidth(2)
        gS2.SetLineColor(ROOT.kBlack)
        gS2.SetLineWidth(2)
        
        mg = ROOT.TMultiGraph("mg", "")
        mg.Add( gP2 )
        mg.Add( gT2 )
        mg.Add( gS2 )
        mg.SetMinimum( 0. )
        mg.Draw("aplez")
        mg.GetXaxis().SetTitle(xname)
        mg.GetYaxis().SetTitle("L1 Efficiency")
        mg.SetMaximum(1.3)
        p.SetGrid()
        
        cmsString = drawCMSString("#bf{CMS Simulation}  <PU>=200  ggH, H#rightarrow#tau#tau")
    
        txt = ROOT.TLatex()
        txt.SetTextSize(0.035)
        txt.DrawLatexNDC(.12, .85,  "Baseline:")
        txt.DrawLatexNDC(.12, .81,  "   %s" % denom_cut)
        txt.DrawLatexNDC(.12, .76, "Passing: (Reco p_{T} > %i)" % pt_cut)
        
        leg = setLegStyle(0.5,0.72,0.9,0.88)
        leg.AddEntry(gS2, "Phase-I Jet","lpe")
        leg.AddEntry(gT2, "Tyler Jet","lpe")
        leg.AddEntry(gP2, "Phase-II Jet","lpe")
        leg.Draw("same")
        c.Update()
        c.SaveAs( universalSaveDir + fName + '_Calib_ptThreshold%i_%s.png' % (pt_cut, app) )
        return
    """ Pt Eff """
    def MakePtEff(p2Obj,s2Obj,axis=[160,0,400],pt_cut=32,denom_cut='abs(genJet_eta)<1.4'):
        # Use eta cuts to restrict when doing pT efficiencies
        # denom_cut = 'abs(genJet_eta)<1.4'
        #denom_cut = 'abs(genJet_eta)>1.6 && abs(genJet_eta)<2.8'
        gP2 = make_efficiency_graph( phase2_t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_pt', axis )
        gT2 = make_efficiency_graph( tyler_t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_pt', axis )
        gS2 = make_efficiency_graph( phase2_t, denom_cut, s2Obj+' > %i' % pt_cut, 'genJet_pt', axis )
        PlotEff(gP2,gT2,gS2,"Gen Jet p_{T} (GeV)",denom_cut,pt_cut,'ptEff')
        return
    """ Eta Eff """
    def MakeEtaEff(p2Obj,s2Obj,axis=[100,-5,5],pt_cut=32,denom_pt=40):
        # Use pt cuts to restrict included objects when doing eta efficiencies
        denom_cut = '(genJet_pt > %i)' % denom_pt
        gP2 = make_efficiency_graph( phase2_t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_eta', axis )
        gT2 = make_efficiency_graph( tyler_t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_eta', axis )
        gS2 = make_efficiency_graph( phase2_t, denom_cut, s2Obj+' > %i' % pt_cut, 'genJet_eta', axis )
        PlotEff(gP2,gT2,gS2,"Gen Jet #eta",denom_cut,pt_cut,'etaEff_ptDenom%i' % denom_pt)
        return

    p2Obj = 'jet_pt_calibration'
    s2Obj = 'stage2jet_pt'
    #s2Obj = 'stage2jet_pt_calibration3'
    s2ObjEta = 'stage2jet_eta'
    MakePtEff(p2Obj,s2Obj,axis=[50,0,400])
    MakeEtaEff(p2Obj,s2Obj,axis=[50,-1.5,1.5])
    return

if __name__ == "__main__":
    import sys
    for fname in sys.argv[1:]:
        MakeEff(fname)
