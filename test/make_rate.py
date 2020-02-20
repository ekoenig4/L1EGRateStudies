import ROOT
from collections import OrderedDict
from L1Trigger.L1EGRateStudies.trigHelpers import make_efficiency_graph, make_rate_hist, setLegStyle, checkDir
import os
from L1Trigger.L1EGRateStudies.trigHelpers import drawCMSString

if not os.path.exists( 'eff_and_rate_roots/' ) : os.makedirs( 'eff_and_rate_roots/' )

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# doTau = True
doTau = False

doEff = True
#doEff = False

doPtEff = True
#doPtEff = False

doRate = True
doRate = False

doRateFirstHalf = True
doRateFirstHalf = False
doRateSecondHalf = True
#doRateSecondHalf = False

c = ROOT.TCanvas('c', 'c', 900, 900)
p = ROOT.TPad('p','p', 0, 0, 1, 1)
p.Draw()
p.cd()

p2Obj = 'jet_pt_calibration'
s2Obj = 'stage2jet_pt'
#s2Obj = 'stage2jet_pt_calibration3'
s2ObjEta = 'stage2jet_eta'
if doTau :
    p2Obj = 'calibPtHH'
    #s2Obj = 'stage2tau_pt'
    s2Obj = 'stage2tau_pt_calibration3'
    s2ObjEta = 'stage2tau_eta'

text = 'Jet' if not doTau else 'Tau'
    

if doEff :
    name = "ggHTT"
    fName = 'output_round2_%s' % name
    date = '20190319'
    base = '/data/truggles/l1CaloJets_'+date+'_r2/'
    universalSaveDir = "/afs/cern.ch/user/t/truggles/www/Phase-II/efficiencies/"+date+"/"+fName+"/"
    checkDir( universalSaveDir )

    f = ROOT.TFile( base+fName+'.root', 'r' )
    t = f.Get('analyzer/tree')
    

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

    """ Pt Eff """
    if doPtEff :
        # Use eta cuts to restrict when doing pT efficiencies
        denom_cut = 'abs(genJet_eta)<1.4'
        #denom_cut = 'abs(genJet_eta)>1.6 && abs(genJet_eta)<2.8'
        axis = [160, 0, 400]
        if doTau :
            axis = [150, 0, 150]
            #axis = [40, 0, 200]

        gP2 = make_efficiency_graph( t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_pt', axis )
        gP22 = make_efficiency_graph( t, denom_cut, p2Obj+' > %i && isoTauHH > 0.5' % pt_cut, 'genJet_pt', axis )
        gS2 = make_efficiency_graph( t, denom_cut, s2Obj+' > %i' % pt_cut, 'genJet_pt', axis )
        gS22 = make_efficiency_graph( t, denom_cut, s2Obj+' > %i && stage2tau_isoBit > 0.5' % pt_cut, 'genJet_pt', axis )

    """ Eta Eff """
    if not doPtEff :
        # Use pt cuts to restrict included objects when doing eta efficiencies
        denom_pt = 40
        denom_cut = '(genJet_pt > %i)' % denom_pt
        axis = [100, -5, 5]
        gP2 = make_efficiency_graph( t, denom_cut, p2Obj+' > %i' % pt_cut, 'genJet_eta', axis )
        gP22 = make_efficiency_graph( t, denom_cut, p2Obj+' > %i && isoTauHH > 0.5' % pt_cut, 'genJet_eta', axis )
        gS2 = make_efficiency_graph( t, denom_cut, s2Obj+' > %i' % pt_cut, 'genJet_eta', axis )
        gS22 = make_efficiency_graph( t, denom_cut, s2Obj+' > %i && stage2tau_isoBit > 0.5' % pt_cut, 'genJet_eta', axis )
    
    gP2.SetMinimum( 0. )
    gP2.SetLineColor(ROOT.kRed)
    gP2.SetLineWidth(2)
    gP22.SetLineColor(ROOT.kBlue)
    gP22.SetLineWidth(2)
    gS2.SetLineColor(ROOT.kBlack)
    gS2.SetLineWidth(2)
    gS22.SetLineColor(ROOT.kBlue)
    gS22.SetLineWidth(2)
    

    #mg = ROOT.TMultiGraph("mg", "L1 %s Efficiency" % text)
    mg = ROOT.TMultiGraph("mg", "")
    mg.Add( gP2 )
    #mg.Add( gP22 )
    mg.Add( gS2 )
    #mg.Add( gS22 )
    mg.SetMinimum( 0. )
    mg.Draw("aplez")
    if doPtEff :
        if doTau :
            mg.GetXaxis().SetTitle("Gen #tau_{h} p_{T} (GeV)")
        else :
            mg.GetXaxis().SetTitle("Gen %s p_{T} (GeV)" % text)
    else :
        if doTau :
            mg.GetXaxis().SetTitle("Gen #tau_{h} #eta")
        else :
            mg.GetXaxis().SetTitle("Gen %s #eta" % text)
    mg.GetYaxis().SetTitle("L1 Efficiency")
    mg.SetMaximum(1.3)
    p.SetGrid()

    cmsString = drawCMSString("#bf{CMS Simulation}  <PU>=200  ggH+qqH, H#rightarrow#tau#tau")
    
    txt = ROOT.TLatex()
    txt.SetTextSize(0.035)
    txt.DrawLatexNDC(.12, .85,  "Baseline:")
    txt.DrawLatexNDC(.12, .81,  "   %s" % denom_cut)
    txt.DrawLatexNDC(.12, .76, "Passing: (Reco p_{T} > %i)" % pt_cut)
    
    #leg = setLegStyle(0.5,0.3,0.9,0.7)
    leg = setLegStyle(0.5,0.72,0.9,0.88)
    leg.AddEntry(gS2, "Phase-I %s" % text,"lpe")
    #leg.AddEntry(gS22, "Phase-I Iso %s" % text,"lpe")
    leg.AddEntry(gP2, "Phase-II %s" % text,"lpe")
    #leg.AddEntry(gP22, "Phase-II Iso %s" % text,"lpe")
    leg.Draw("same")
    c.Update()
    
    
    app = 'ptEff' if doPtEff else 'etaEff_ptDenom%i' % denom_pt
    #c.SaveAs( universalSaveDir + fName + '_Calib_ptThreshold%i_%s_include_S2Iso.png' % (pt_cut, app) )
    c.SaveAs( universalSaveDir + fName + '_Calib_ptThreshold%i_%s.png' % (pt_cut, app) )
    c.SaveAs( universalSaveDir + fName + '_Calib_ptThreshold%i_%s_NoIsoTaus.png' % (pt_cut, app) )
    #c.SaveAs( universalSaveDir + fName + '_Calib_ptThreshold%i_%s_HGCal.png' % (pt_cut, app) )

""" MAKE RATES """
if doRate :

    fName = 'output_round2_minBiasv1'
    fName = 'output_round2_minBiasv1_withCuts'
    date = '20190319'
    base = '/data/truggles/l1CaloJets_'+date+'_r2/'
    universalSaveDir = "/afs/cern.ch/user/t/truggles/www/Phase-II/rates/"+date+"/"+fName+"/"
    checkDir( universalSaveDir )



    f = ROOT.TFile( base+fName+'.root', 'r' )
    print f
    t = f.Get('analyzer/tree')

    # We used cuts to make a slimmed ttree for looping, so need to get nEvents from the
    # original file

    fEvents = ROOT.TFile( base+fName.replace('_withCuts','')+'.root', 'r' )
    print fEvents
    nEvents = fEvents.Get('analyzer/nEvents').Integral()
    #nEvents = f.Get('analyzer/nEvents').Integral()

    # Min and Max eta thresholds for barrel, HGCal, HF rates
    eta_thresholds = OrderedDict()
    if doTau :
        eta_thresholds['all']    = [0., 2.8, ROOT.kBlack]
        eta_thresholds['runII']  = [0., 2.1, ROOT.kBlue]
    else :
        eta_thresholds['all']    = [0., 6.0, ROOT.kBlack]
    eta_thresholds['barrel'] = [0., 1.5, ROOT.kRed]
    #eta_thresholds['hgcal']  = [1.5, 3.0, ROOT.kBlue]
    if not doTau :
        eta_thresholds['hf']     = [3.0, 6.0, ROOT.kGreen+3]

    # nBins, min, max
    x_info = [100, 0, 200]
    
    """ This portion takes a long time, only do it if doRateFirstHalf is True """
    if doRateFirstHalf :
        for name, thresholds in eta_thresholds.iteritems() :
            hP2 = make_rate_hist( nEvents, t, p2Obj, 1.0, 'jet_eta', thresholds[0], thresholds[1], x_info ) 
            hP2.SaveAs( 'eff_and_rate_roots/'+fName+'_'+name+'_Phase-2.root' )
            del hP2
            hP22 = make_rate_hist( nEvents, t, p2Obj, 1.0, 'jet_eta', thresholds[0], thresholds[1], x_info, 'isoTauHH' ) 
            hP22.SaveAs( 'eff_and_rate_roots/'+fName+'_'+name+'_Phase-2_iso.root' )
            del hP22
            hS2 = make_rate_hist( nEvents, t, s2Obj, 1.0, s2ObjEta, thresholds[0], thresholds[1], x_info )
            hS2.SaveAs( 'eff_and_rate_roots/'+fName+'_'+name+'_Stage-2.root' )
            del hS2
            #hS22 = make_rate_hist( nEvents, t, s2Obj, 1.0, s2ObjEta, thresholds[0], thresholds[1], x_info, 'stage2tau_isoBit' )
            #hS22.SaveAs( 'eff_and_rate_roots/'+fName+'_'+name+'_Stage-2_iso.root' )
            #del hS22

    if not doRateSecondHalf :
        print "Skip second half of rate code"
        assert(0)
    
    colors = [ROOT.kBlack, ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kGray+2]
    saveName = 'output_round2_minBiasv1_withCuts'
    x_info_rebin = [100, 0, 160]

    plot_map = OrderedDict()
    plot_map['barrel'] = OrderedDict()
    plot_map['barrel']['Phase-2'] = ['Tau', 0]
    plot_map['barrel']['Stage-2'] = ['Tau', 2]
    plot_map['nominal'] = OrderedDict()
    plot_map['nominal']['Phase-2'] = ['Tau', 0]
    plot_map['nominal']['Stage-2'] = ['Tau', 2]
    plot_map['phase2_All'] = OrderedDict()
    plot_map['phase2_All']['Phase-2'] = ['Tau', 0]
    plot_map['phase2_All']['Phase-2_iso'] = ['IsoTau', 2]

    for plot, samples in plot_map.iteritems() :
        rates = []
        for sample, info in samples.iteritems() :
            cnt = 0
            for name, thresholds in eta_thresholds.iteritems() :

                print sample, info, name
                if plot == 'barrel' and name != 'barrel' : continue
                

                f1 = ROOT.TFile( 'eff_and_rate_roots/'+saveName+'_'+name+'_'+sample+'.root', 'r')
                print f1
                rates.append( f1.Get('cumul') )
                rates[-1].SetDirectory( 0 )
                rates[-1].SetTitle( '%s %s, %s' % (sample.replace('_iso',''), info[0], name) )
                rates[-1].SetName( '%s %s, %s' % (sample.replace('_iso',''), info[0], name) )
                rates[-1].SetLineColor( thresholds[2]+info[1] )
                rates[-1].SetMarkerColor( thresholds[2]+info[1] )
                rates[-1].SetLineWidth( 2 )
                if name == 'all' :
                    rates[-1].SetLineWidth( 4 )
                rates[-1].GetXaxis().SetRangeUser(x_info_rebin[1], x_info_rebin[2])
                cnt += 1
        
        print rates[0]
        if doTau :
            rates[0].GetXaxis().SetTitle("Reco #tau_{h} p_{T} (GeV)" )
        else :
            rates[0].GetXaxis().SetTitle("Reco %s p_{T} (GeV)" % text )
        rates[0].GetYaxis().SetTitle("L1 Rate (kHz)")
        rates[0].SetMaximum( 40000 ) 
        rates[0].SetMinimum( 5 ) 
        rates[0].Draw()

        cmsString = drawCMSString("#bf{CMS Simulation}  <PU>=200  Minimum Bias")

        cnt = 0
        for rate in rates :
            cnt += 1
            if cnt == 1 : continue
            if 'Stage-2' in rate.GetTitle() and 'hgcal' in rate.GetTitle() : continue
            if 'Stage-2' in rate.GetTitle() and 'all' in rate.GetTitle() : continue
            if 'Stage-2' in rate.GetTitle() and 'runII' in rate.GetTitle() : continue
            rate.Draw('SAME')
        
        
        p.SetGrid()
        p.SetLogy()
        p.SetLeftMargin( .13 )
        p.SetBottomMargin( .1 )
        
        
        leg = setLegStyle(0.5,0.55,0.9,0.85)
        for rate in rates :
            if 'Stage-2' in rate.GetTitle() and 'hgcal' in rate.GetTitle() : continue
            if 'Stage-2' in rate.GetTitle() and 'all' in rate.GetTitle() : continue
            if 'Stage-2' in rate.GetTitle() and 'runII' in rate.GetTitle() : continue
            title = rate.GetTitle() if not rate.GetTitle() == "L1 Rates" else "Phase2, All"
            title = title.replace('barrel', '0 < |#eta| < 1.5')
            title = title.replace('Stage-2', 'Phase-I')
            title = title.replace('Phase-2', 'Phase-II')
            title = title.replace('runII', '0 < |#eta| < 2.1')
            title = title.replace('all', '0 < |#eta| < 2.8')
            leg.AddEntry(rate, title,"lpe")
        leg.Draw("same")
        rates[0].SetTitle("")
        c.Update()
        
        
        c.SaveAs( universalSaveDir + fName +  '_Calib_rate_'+plot+'_.png' )



