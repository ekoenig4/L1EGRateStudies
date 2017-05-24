import ROOT
import trigHelpers
from array import array
from ROOT import gStyle, gPad
import CMS_lumi, tdrstyle

def loadHists( file_, histMap = {}, eff=False ) :
    hists = {}
    for h, path in histMap.iteritems() :
        print h, path
        if len( path ) == 2 :
            #print "len 2",path[0],path[1]
            hists[ h ] = file_.Get( path[1] )
            #print hists[ h]
            hists[ h ].SetTitle( path[0] )
        else :
            hists[ h ] = file_.Get( path )
            hists[ h ].SetTitle( h )
        #hists[ h ].SetDirectory( 0 )
    return hists


def drawCMSString( title ) :
    cmsString = ROOT.TLatex(
        gPad.GetAbsXlowNDC()+gPad.GetAbsWNDC()-gPad.GetLeftMargin(),
        gPad.GetAbsYlowNDC()+gPad.GetAbsHNDC()-gPad.GetTopMargin()+0.005,
        title )
    cmsString.SetTextFont(42)
    cmsString.SetTextSize(0.03)
    cmsString.SetNDC(1)
    cmsString.SetTextAlign(31)
    cmsString.Draw()
    return cmsString


def setLegStyle( x1,y1,x2,y2 ) :
    leg = ROOT.TLegend(x1,y1,x2,y2)
    leg.SetBorderSize(0)
    leg.SetLineColor(1)
    leg.SetLineStyle(1)
    leg.SetLineWidth(1)
    leg.SetFillColor(0)
    leg.SetFillStyle(0)
    leg.SetTextFont(42)
    return leg


def draw2DSets(c, tree1, var, cut, title1, tree2, title2, xaxis, xinfo, yaxis, yinfo) :
    print cut
    c.cd(1)
    h1 = ROOT.TH2F("h1", title1, xinfo[0], xinfo[1], xinfo[2], yinfo[0], yinfo[1], yinfo[2])
    tree1.Draw( var + " >> h1", cut )
    h1.GetXaxis().SetTitle( xaxis )
    h1.GetYaxis().SetTitle( yaxis )
    h1.Draw("colz")
    c.cd(2)
    h2 = ROOT.TH2F("h2", title2, xinfo[0], xinfo[1], xinfo[2], yinfo[0], yinfo[1], yinfo[2])
    tree2.Draw( var + " >> h2", cut )
    h2.GetXaxis().SetTitle( xaxis )
    h2.GetYaxis().SetTitle( yaxis )
    h2.Draw("colz")
    c.Print("plots/"+c.GetTitle()+".png")
    del h1
    del h2

def draw2DPtRes( hist, c, name ) :
    c.Clear()
    c.SetCanvasSize(700, 600)
    #c.SetGridx(1)
    #c.SetGridy(1)
    c.SetRightMargin(0.14)
    c.SetTopMargin(0.10)
    #recoGenPtHist.SetTitle("Crystal EG algorithm pT resolution")
    hist.SetTitle("")
    hist.GetYaxis().SetTitle("Relative Error in P_{T} (reco-gen)/gen")
    hist.GetYaxis().SetTitleOffset(1.3)
    hist.SetMaximum(50)
    hist.Draw("colz")
    cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, Single Electron")
    c.Print("plots/"+name+"_reco_gen_pt.png")
    del cmsString


def drawRates( hists, c, ymax, xrange = [0., 0.] ) :
    c.cd()
    colors = [ROOT.kBlack, ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kGray]
    marker_styles = [20, 24, 25, 26, 32, 35]
    c.Clear()
    mg = ROOT.TMultiGraph("mg", c.GetTitle())
    
    graphs = []
    for i, hist in enumerate(hists) :
        graph = ROOT.TGraphErrors( hist )
        graph.SetLineColor( colors[i] )
        graph.SetMarkerColor( colors[i] )
        graph.SetMarkerStyle( marker_styles[i] )
        graph.SetMarkerSize( 0.8 )
        mg.Add( graph )
        graphs.append( graph )
    
    mg.Draw("aplez")
    
    if c.GetLogy() == 0 : # linear
        mg.SetMinimum(0.)
    else :
        mg.SetMinimum(1.)
    
    if ymax != 0. :
        mg.SetMaximum( ymax ) 
    
    #leg = setLegStyle(0.53,0.78,0.95,0.92)
    leg = setLegStyle(0.45,0.6,0.95,0.92)
    for graph in graphs :
        leg.AddEntry(graph, graph.GetTitle(),"lpe")
    leg.Draw("same")
    c.Update()
    
    mg.GetXaxis().SetTitle(hists[0].GetXaxis().GetTitle())
    if xrange[0] != 0. or xrange[1] != 0 :
        mg.GetXaxis().SetRangeUser(xrange[0], xrange[1])
    mg.GetYaxis().SetTitle(hists[0].GetYaxis().GetTitle())
    
    cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, MinBias")
    ROOT.gPad.SetGrid()
    
    c.Print("/afs/cern.ch/user/t/truggles/www/Phase-II/v2p3/"+c.GetName()+".png")


def drawEfficiency( hists, c, ymax, xTitle, xrange = [0., 0.], fit = False, fitHint = [1., 15., 3., 0.]) :
    c.cd()
    colors = [ROOT.kBlack, ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kGray]
    marker_styles = [20, 24, 25, 26, 32]
    lines = [ROOT.kSolid, ROOT.kDashed, ROOT.kDotted]
    c.Clear()
    mg = ROOT.TMultiGraph("mg", c.GetTitle())
    
    graphs = []
    for i, hist in enumerate(hists) :
        #hist.Scale( 1./hist.GetEntries() )
        graph = ROOT.TGraphAsymmErrors( hist )
        graph.SetLineColor( colors[i] )
        graph.SetMarkerColor( colors[i] )
        graph.SetMarkerStyle( marker_styles[i] )
        graph.SetMarkerSize( 0.8 )
        mg.Add( graph )
        graphs.append( graph )
    
    mg.Draw("aplez")

    if c.GetLogy() == 0 : # linear
        mg.SetMinimum(0.)
    else :
        mg.SetMinimum(10.)
    
    if ymax != 0. :
        mg.SetMaximum( ymax ) 
    

    if ( fit and xrange[1] != xrange[0] ) :
       gStyle.SetOptFit(0)
       for j, graph in enumerate(graphs) :
          shape = ROOT.TF1("shape", "[0]/2*(1+TMath::Erf((x-[1])/([2]*sqrt(x))))+[3]*x", xrange[0], xrange[1])
          shape.SetParameters(fitHint[0], fitHint[1], fitHint[2], fitHint[3])
          # Somehow, step size increases each time, have to find a way to control it...
          graph.Fit(shape)
          graph.GetFunction("shape").SetLineColor(graph.GetLineColor())
          graph.GetFunction("shape").SetLineWidth(graph.GetLineWidth()*2)
 
    leg = setLegStyle(0.53,0.80,0.95,0.92)
    for graph in graphs :
        leg.AddEntry(graph, graph.GetTitle(),"lpe")
    leg.Draw("same")
    c.Update()
    
    #mg.GetXaxis().SetTitle(graphs[0].GetXaxis().GetTitle())
    mg.GetXaxis().SetTitle(xTitle)
    if ( xrange[0] != 0. or xrange[1] != 0 ) :
        mg.GetXaxis().SetRangeUser(xrange[0], xrange[1])
    #mg.GetYaxis().SetTitle(graphs[0].GetYaxis().GetTitle())
    mg.GetYaxis().SetTitle("Eff. (L1 Algo./Generated)")
 
    cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, Single Electron")
    #cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, Single Photon")
 
    c.Print(("/afs/cern.ch/user/t/truggles/www/Phase-II/v2p3/"+c.GetName()+".png"))


def draw2DdeltaRHist(hist, c) :
    c.Clear()
    c.cd()
    gStyle.SetOptTitle(0)
    txtSize = 0.028
    margin = 0.08
    histpad_sizeX = 0.6
    histpad_sizeY = 0.7
    hist_pad = ROOT.TPad( c.GetName()+"_hist", "subpad", margin, margin, histpad_sizeX+margin, histpad_sizeY+margin, c.GetFillColor())
    hist_pad.SetMargin(0., 0., 0., 0.)
    hist_pad.Draw()
    xprojection_pad = ROOT.TPad(c.GetName()+"_xprojection", "subpad", margin, histpad_sizeY+margin, histpad_sizeX+margin, 1-margin, c.GetFillColor())
    xprojection_pad.SetMargin(0., 0., 0., 0.)
    xprojection_pad.Draw()
    yprojection_pad = ROOT.TPad(c.GetName()+"_yprojection", "subpad", histpad_sizeX+margin, margin, 1-2.5*margin, histpad_sizeY+margin, c.GetFillColor())
    yprojection_pad.SetMargin(0., 0., 0., 0.)
    yprojection_pad.Draw()
 
    hist.Sumw2()
    hist.Scale(1./hist.Integral())
    xprojection_hist = hist.ProjectionX(hist.GetName(), 1, hist.GetNbinsY(), "e")
    xprojection = ROOT.TGraphErrors(xprojection_hist)
    xprojection.SetLineColor(ROOT.kBlack)
    xprojection.SetMarkerColor(ROOT.kBlack)
    yprojection_hist = hist.ProjectionY(hist.GetName(), 1, hist.GetNbinsX(), "e")
    yprojection = ROOT.TGraphErrors(yprojection_hist.GetXaxis().GetNbins())
    yprojection.SetName(yprojection_hist.GetName()+"_graph")
    yproj_xaxis = yprojection_hist.GetXaxis()
    for i in range(0, yprojection_hist.GetXaxis().GetNbins() ) :
       bin = yproj_xaxis.GetBinCenter(i+1)
       width = yprojection_hist.GetBinWidth(i+1)*gStyle.GetErrorX()
       count = yprojection_hist.GetBinContent(i+1)
       err = yprojection_hist.GetBinError(i+1)
       yprojection.SetPoint(i, count, bin)
       yprojection.SetPointError(i, err, width)
 
    # Draw 2D hist
    hist_pad.cd()
    hist.Draw("col")
    gPad.SetLogz()
    hist.GetXaxis().SetTitle("d#eta(reco-gen)")
    hist.GetYaxis().SetTitle("d#phi(reco-gen)")
    hist.GetYaxis().SetTitleOffset(1.4)
    
    # Fit hist
    shape = ROOT.TF2("2dshape", "[0]*exp(-[2]*(x[0]-[1])**2-[4]*(x[1]-[3])**2-2*[5]*(x[0]-[1])*(x[1]-[3]))", -0.05, 0.05, -0.05, 0.05)
    shape.SetParameters(0.003, 0., 3.769e4, 0., 4.215e4, -1.763e4)
    hist.Fit(shape, "n")
    max_ = shape.GetParameter(0)
    contours = array('d', [])
    contours.append( max_*ROOT.TMath.exp(-4.5))
    contours.append( max_*ROOT.TMath.exp(-2))
    contours.append( max_*ROOT.TMath.exp(-0.5))
    shape.SetContour(3, contours)
    shape.SetNpx(100)
    shape.SetNpy(100)
    shape.SetLineWidth(2)
    shape.SetLineColor(ROOT.kBlack)
    shape.Draw("cont3 same")
    
    # One crystal box
    crystalBox = ROOT.TBox(-0.0173/2, -0.0173/2, 0.0173/2, 0.0173/2)
    crystalBox.SetLineStyle(3)
    crystalBox.SetLineColor(13)
    crystalBox.SetLineWidth(2)
    crystalBox.SetFillStyle(0)
    crystalBox.Draw()
 
    # Draw x projection
    xprojection_pad.cd()
    xprojection.Draw("apez")
    xprojection.GetYaxis().SetNdivisions(0)
    xprojection.GetXaxis().SetRangeUser(hist.GetXaxis().GetBinLowEdge(1), hist.GetXaxis().GetBinUpEdge(hist.GetXaxis().GetNbins()))
    xprojection.GetXaxis().SetLabelSize(0.)
    xprojection.GetYaxis().SetRangeUser(0., 0.22)
    shapeprojX = ROOT.TF1("shapeprojX", "[0]*sqrt(([2]*[4]-[5]**2)/(TMath::Pi()*[2]))*exp(([5]**2-[2]*[4])*(x-[3])**2/[2])", -0.05, 0.05)
    shapeprojX.SetParameters(shape.GetParameters())
    shapeprojX.SetParameter(0, shape.GetParameter(0)/20)
    shapeprojX.SetLineWidth(2)
    shapeprojX.SetNpx(100)
    shapeprojX.SetLineColor(ROOT.kRed)
    shapeprojX.Draw("same")
 
    # Draw y projection
    yprojection_pad.cd()
    yprojection.Draw("apez")
    yprojection.GetXaxis().SetNdivisions(0)
    yprojection.GetXaxis().SetRangeUser(0., 0.2)
    yprojection.GetYaxis().SetRangeUser(hist.GetYaxis().GetBinLowEdge(1), hist.GetYaxis().GetBinUpEdge(hist.GetYaxis().GetNbins()))
    yprojection.GetYaxis().SetLabelSize(0.)
    shapeprojY = ROOT.TF1("shapeprojY", "[0]*sqrt(([2]*[4]-[5]**2)/(TMath::Pi()*[4]))*exp(([5]**2-[2]*[4])*(x-[1])**2/[4])", -0.05, 0.05)
    shapeprojY.SetParameters(shape.GetParameters())
    shapeprojY.SetParameter(0, shape.GetParameter(0)/20)
    shapeprojYpos = array('d', [])
    shapeprojYval = array('d', [])
    for i in range( 0, 101 ) :
       shapeprojYpos.append( 1e-3*i-0.05 )
       shapeprojYval.append( shapeprojY.Eval(shapeprojYpos[i] ) )

    shapeprojYLine = ROOT.TGraph(101, shapeprojYval, shapeprojYpos)
    shapeprojYLine.SetLineColor(ROOT.kRed)
    shapeprojYLine.SetLineWidth(2)
    shapeprojYLine.Draw("l")
 
    # Draw Title
    c.cd()
    if c.GetTitle() != '' :
       title = ROOT.TLatex(margin, 1-margin+0.01, "Crystal-level EG Trigger #DeltaR Distribution")
       title.SetTextSize(0.04)
       title.SetTextFont(42)
       title.SetNDC()
       title.Draw()
 
    # CMS info string
    cmsString = ROOT.TLatex(
       histpad_sizeX+margin-0.005, 
       1-margin-0.005, 
       "CMS Simulation, <PU>=200 bx=25, Single Electron")
       #"CMS Simulation, <PU>=200 bx=25, Single Photon")
    cmsString.SetTextFont(42)
    cmsString.SetTextSize(0.02)
    cmsString.SetNDC(1)
    cmsString.SetTextAlign(33)
    cmsString.Draw()
 
    # Stats
    stats = []
    stats.append( ROOT.TLatex(histpad_sizeX+margin+0.01, histpad_sizeY+margin+0.13, "#mu_#eta = "+format(shape.GetParameter(1), '.2g' )) )
    stats.append( ROOT.TLatex(histpad_sizeX+margin+0.01, histpad_sizeY+margin+0.1,"#mu_#phi = "+format(shape.GetParameter(3), '.2g')) )
    stats.append( ROOT.TLatex(histpad_sizeX+margin+0.01, histpad_sizeY+margin+0.07, "#sigma_#eta#eta = "+format(ROOT.TMath.sqrt(0.5/shape.GetParameter(2)), '.2g')) )
    stats.append( ROOT.TLatex(histpad_sizeX+margin+0.01, histpad_sizeY+margin+0.04, "#sigma_#phi#phi = "+format(ROOT.TMath.sqrt(0.5/shape.GetParameter(4)), '.2g')) )
    stats.append( ROOT.TLatex(histpad_sizeX+margin+0.01, histpad_sizeY+margin+0.01, "#sigma_#eta#phi = "+format(ROOT.TMath.sqrt(-0.5/shape.GetParameter(5)), '.2g')) )
    for i in range( 0, 5 ) :
       stats[i].SetTextSize(txtSize-0.002)
       stats[i].SetTextFont(42)
       stats[i].SetNDC()
       stats[i].Draw()
 
    # Draw palette
    # (not working)
    gPad.Update()
    palette = ROOT.TPaletteAxis(1-2.5*margin+0.01, margin, 1-1.5*margin, histpad_sizeY+margin, hist)
    hist.GetXaxis().SetTitleOffset( .8 )
    hist.GetXaxis().SetLabelSize( txtSize*1.35 )
    hist.GetXaxis().SetTitleSize( txtSize*2 )
    hist.GetYaxis().SetTitleOffset( 1.1 )
    hist.GetYaxis().SetLabelSize( txtSize*1.35 )
    hist.GetYaxis().SetTitleSize( txtSize*2 )
    hist.GetZaxis().SetTitleOffset( 1.4 )
    hist.GetZaxis().SetLabelSize( txtSize )
    hist.GetZaxis().SetTitleSize( txtSize )
    palette.Draw()
    #gPad.SetLogz()
    gPad.Modified()
    gPad.Update()
 
    c.Print("plots/"+c.GetName()+".png")
    
    gStyle.SetOptTitle(1)


def drawDRHists(hists, c, ymax, doFit = False, targetDir = 'plots' ) :
    c.cd()
    colors = [ROOT.kBlack, ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kGray+2]
    marker_styles = [20, 24, 25, 26, 32, 35]
    hs = ROOT.THStack("hs", c.GetTitle())
    maxi = 0.
    for i, hist in enumerate(hists) :
        hist.Sumw2()
        hist.Scale(1./hist.Integral())
        hist.SetLineColor(colors[i])
        hist.SetMarkerColor(colors[i])
        hist.SetMarkerStyle(marker_styles[i])
        hist.SetMarkerSize(0.8)
        if hist.GetMaximum() > maxi : maxi = hist.GetMaximum()
        hs.Add(hist, "ex0 hist")

    c.Clear()
    if c.GetLogy() == 0 : # linear
        hs.SetMinimum(0.)
    if ymax == 0. :
        hs.SetMaximum( maxi * 1.8 )
    elif ymax == -1 :
        hs.SetMaximum( maxi * 1.3 )
    elif ymax != 0. :
        hs.SetMaximum(ymax)
    #hs.SetMinimum(0.0001)
 
    hs.Draw("nostack")
 
    markers = []
    for hist in hists :
        markers.append( hist )
        markers[-1].Draw("psame")
 
    fit = ROOT.TF1("doublegaus", "gaus+gaus(3)", 0., 0.25)
    fit.SetParameters(0.3, 0., 0.003, 0.1, 0., 0.02)
    #hists[0].Fit(fit, "n")
    #fit.Draw("lsame")
 
    #leg = setLegStyle(0.53,0.78,0.95,0.92)
    leg = setLegStyle(0.43,0.72,0.95,0.92)
    for hist in hists :
        leg.AddEntry(hist, hist.GetTitle(),"elp")
    leg.Draw("same")
    c.Update()

    hs.GetXaxis().SetTitle(hists[0].GetXaxis().GetTitle())
    #hs.GetYaxis().SetTitle(hists[0].GetYaxis().GetTitle())
    hs.GetYaxis().SetTitleOffset(1.2)
    hs.GetYaxis().SetTitle("Fraction of Events")
    #hs.GetXaxis().SetRangeUser(0., 0.1)
 
    if "Stage2" in c.GetName() :
        cmsString = drawCMSString("CMS Simulation")
    else :
        cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, Single Electron")
        #cmsString = drawCMSString("CMS Simulation, <PU>=200 bx=25, Min-Bias")
                
    c.Print(targetDir+"/"+c.GetName()+".png")

    # Don't produce CDFs at the moment
    #del markers
    #markers = []
 
    ## Now for integral
    #for  hist in hists :
    #   hs.RecursiveRemove(hist)
    #   intHist = hist.Clone( hist.GetName()+"_cdf" )
    #   integral = 0.
    #   for bin in range(0, intHist.GetNbinsX()+1) :
    #      integral += intHist.GetBinContent(bin)
    #      intHist.SetBinContent(bin, integral)

    #   hs.Add(intHist, "ex0 hist")
    #   markers.append(intHist)

    #hs.SetMaximum(1.2)
    #hs.GetYaxis().SetTitle( "Cumulative "+hs.GetYaxis().GetTitle() )
    #hs.Draw("nostack")
    #for  m in markers : m.Draw("psame")
    #leg.Draw("same")
    #cmsString2 = drawCMSString("CMS Simulation, <PU>=200 bx=25, Single Electron")
    #c.Print( "plots/"+c.GetName()+"_cdf.png" )

    if doFit :
        gStyle.SetOptFit(0)
        # Poorly done hard coding for fit suggestions
        # and fit ranges, sorry
        fitHints = [[.17, -0.025, 0.07],
                    [.08, 0.1, .1 ]]
        fitRanges = [[-.15, .08],
                    [-0.09, .3]]
        fitResults = []
        fitResults.append( ROOT.TLatex(.7, .7, "Gaussian Fits:" ))
        for i, hist in enumerate(hists) :
            shape = ROOT.TF1("shape", "gaus(0)", fitRanges[i][0], fitRanges[i][1])
            shape.SetParameters(fitHints[i][0], fitHints[i][1], fitHints[i][2])
            hist.Fit(shape, "R")
            hist.GetFunction("shape").SetLineColor(hist.GetLineColor())
            hist.GetFunction("shape").SetLineWidth(hist.GetLineWidth()*2)

            fitResult = hist.GetFunction("shape")
            fitResults.append( ROOT.TLatex(.7, .66-i*.09, "#mu: "+format(fitResult.GetParameter(1), '.2g')))
            fitResults.append( ROOT.TLatex(.7, .62-i*.09, "#sigma: "+format(ROOT.TMath.sqrt(.5*abs(fitResult.GetParameter(2))), '.2g')))
        for i in range( len(fitResults) ) :
            fitResults[i].SetTextSize(0.045)
            fitResults[i].SetTextFont(42)
            fitResults[i].SetNDC()
            if i > 2 : fitResults[i].SetTextColor(ROOT.kRed)
            fitResults[i].Draw()

        c.Print( "plots/"+c.GetName()+"_fit.png" )

    c.Clear()
 
def simple1D( name, tree, iii, var, info, cut="" ) :
    h = ROOT.TH1F("%i" % iii[0], name+' '+var+';'+var, info[0], info[1], info[2])
    tree.Draw( var + " >> %i" % iii[0], cut )
    iii[0] += 1
    return h
    


if __name__ == '__main__' :
    
    ROOT.gROOT.SetBatch(True)
    gStyle.SetOptStat(0)
    gStyle.SetTitleFont(42, "p")
    gStyle.SetTitleColor(1)
    gStyle.SetTitleTextColor(1)
    gStyle.SetTitleFillColor(10)
    gStyle.SetTitleFontSize(0.05)
    gStyle.SetTitleFont(42, "XYZ")
    gStyle.SetLabelFont(42, "XYZ")

    
    ratesMap = {
        'L1EGamma Crystal' : 'analyzer/dyncrystalEG_rate',
        'L1EGamma Crystal Track' : 'analyzer/dyncrystalEG_track_rate',
        'L1EGamma Crystal Photon' : 'analyzer/dyncrystalEG_phoWindow_rate',
        'Stage-2 L1EG' : 'analyzer/stage2EG_rate',
        #'Stage-2 L1EG Iso' : 'analyzer/stage2EG_iso_rate',}

    effMap = {
        'newAlgEtaHist' : ('L1EGamma Crystal', 'analyzer/divide_dyncrystalEG_efficiency_eta_by_gen_eta'),
        'newAlgTrkEtaHist' : ('L1EGamma Crystal - Trk Match', 'analyzer/divide_dyncrystalEG_efficiency_track_eta_by_gen_eta'),
        #'newAlgEtaHist' : ('L1EGamma Crystal', 'analyzer/divide_dyncrystalEG_efficiency_track_eta_by_gen_eta'),
        'newAlgPtHist' : ('L1EGamma Crystal', 'analyzer/divide_dyncrystalEG_efficiency_pt_by_gen_pt'),
        'newAlgTrkPtHist' : ('L1EGamma Crystal - Trk Match', 'analyzer/divide_dyncrystalEG_efficiency_track_pt_by_gen_pt'),
        'newAlgPhotonPtHist' : ('L1EGamma Crystal - Photon', 'analyzer/divide_dyncrystalEG_efficiency_phoWindow_pt_by_gen_pt'),
        #'newAlgPtHist' : ('L1EGamma Crystal', 'analyzer/divide_dyncrystalEG_efficiency_track_pt_by_gen_pt'),
        'Stage2PtHist' : ('Stage-2 L1EG', 'analyzer/divide_stage2EG_efficiency_pt_by_gen_pt'),
        #'Stage2IsoPtHist' : ('Stage-2 L1EG - Iso', 'analyzer/divide_stage2EG_efficiency_iso_pt_by_gen_pt'),
        'Stage2EtaHist' : ('Stage-2 L1EG', 'analyzer/divide_stage2EG_efficiency_eta_by_gen_eta'),
        #'Stage2IsoEtaHist' : ('Stage-2 L1EG - Iso', 'analyzer/divide_stage2EG_efficiency_iso_eta_by_gen_eta'),
        'stage2DRHist' : ('Stage-2 Crystal', 'analyzer/stage2EG_deltaR'),
        'stage2DEtaHist' : ('Stage-2 Crystal', 'analyzer/stage2EG_deta'),
        'stage2DPhiHist' : ('Stage-2 Crystal', 'analyzer/stage2EG_dphi'),
        'stage2GenRecoPtHist' : ('Stage-2 Crystal', 'analyzer/stage2_1d_reco_gen_pt'),
        'newAlgDRHist' : ('L1EGamma Crystal', 'analyzer/dyncrystalEG_deltaR'),
        'newAlgDEtaHist' : ('L1EGamma Crystal', 'analyzer/dyncrystalEG_deta'),
        'newAlgDPhiHist' : ('L1EGamma Crystal', 'analyzer/dyncrystalEG_dphi'),
        'newAlgGenRecoPtHist' : ('L1EGamma Crystal', 'analyzer/1d_reco_gen_pt'),
    }
    
    date = 'v3p2'
    date = 'v2p3'

    singleE = 'r2_phase2_singleElectron_%s.root' % date
    singlePho = 'r2_phase2_singlePhoton_%s.root' % date
    minBias = 'r2_phase2_minBias_%s.root' % date

    rateFile = ROOT.TFile( minBias, 'r' )
    effFile = ROOT.TFile( singleE, 'r' )
    effPhoFile = ROOT.TFile( singlePho, 'r' )

    effHistsKeys = trigHelpers.getKeysOfClass( effFile, "analyzer", "TGraphAsymmErrors")
    
    hists = loadHists( rateFile, ratesMap )
    effHists = loadHists( effFile, effMap )
    #newAlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_dyncrystalEG_threshold*_reco_pt")
    #for h in newAlgRecoPtHists : h.SetTitle("Crystal Algorithm")
    newAlgGenPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_dyncrystalEG_threshold*_gen_pt")
    for h in newAlgGenPtHists : h.SetTitle("L1EG Crystal Algo")
    stage2GenPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_stage2EG_threshold*_gen_pt")
    for h in stage2GenPtHists : h.SetTitle("Stage-2 Algorithm")
    #oldAlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_SLHCL1ExtraParticles:EGamma_threshold*_reco_pt")
    #for h in oldAlgRecoPtHists : h.SetTitle("Original L2 Algorithm")
    #dynAlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_SLHCL1ExtraParticlesNewClustering:EGamma_threshold*_reco_pt")
    #for h in dynAlgRecoPtHists : h.SetTitle("Tower Algorithm 2")
    #run1AlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_l1extraParticles:All_threshold*_reco_pt")
    #for h in run1AlgRecoPtHists : h.SetTitle("Run 1 Alg.")
    #crystalAlgPtHist = effFile.Get("analyzer/divide_L1EGammaCrystalsProducer:EGammaCrystal_efficiency_pt_by_gen_pt")
    #crystalAlgPtHist.SetTitle("Crystal Trigger (prod.)")
    #crystalAlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_L1EGammaCrystalsProducer:EGammaCrystal_threshold*_reco_pt")
    #for h in crystalAlgRecoPtHists : h.SetTitle("Crystal Algorithm")
    #crystalAlgGenPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_L1EGammaCrystalsProducer:EGammaCrystal_threshold*_gen_pt")
    #for h in crystalAlgGenPtHists : h.SetTitle("L1EGamma Crystal")
    #UCTAlgRecoPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_l1extraParticlesUCT:All_threshold*_reco_pt")
    #for h in UCTAlgRecoPtHists : h.SetTitle("Phase 1 TDR")
    #UCTAlgGenPtHists = trigHelpers.loadObjectsMatchingPattern( effFile, "analyzer", effHistsKeys, "divide_l1extraParticlesUCT:All_threshold*_gen_pt")
    #for h in UCTAlgGenPtHists : h.SetTitle("Phase 1 TDR")
    
    crystal_tree = effFile.Get("analyzer/crystal_tree")
    rate_tree = rateFile.Get("analyzer/crystal_tree")
    ''' Do 2D color plots 1st b/c of TDR style '''
    # TDR Style does not play well with 2D color plots
    # 1) 2D delta Eta vs delta Phi plot
    dynCrystal2DdeltaRHist = effFile.Get("analyzer/dyncrystalEG_2DdeltaR_hist")
    c = ROOT.TCanvas('c', 'c', 800, 700)
    #c.SetCanvasSize(800, 700)
    c.SetName("dyncrystalEG_2D_deltaR")
    #c.SetTitle("#Delta R Distribution Fit")
    c.SetTitle("")
#XXX    draw2DdeltaRHist(dynCrystal2DdeltaRHist, c)
 
    # 2) 2D pt resolution vs. gen pt
    recoGenPtHist = effFile.Get("analyzer/reco_gen_pt")
    draw2DPtRes( recoGenPtHist, c, "L1EG_Xtal" )
    stage2RecoGenPtHist = effFile.Get("analyzer/stage2_reco_gen_pt")
    draw2DPtRes( stage2RecoGenPtHist, c, "Stage-2" )

    ''' Track to cluster reco resolution '''
    c.SetCanvasSize(1200,600)
    c.Divide(2)
    # 3) 2D position resolution vs. reco pt
    # dEta
    #cut = "((cluster_iso<2 && abs(trackDeltaPhi)<0.02) || cluster_pt > 20)"
    #cut = "((cluster_iso<2 && abs(trackDeltaPhi)<0.02 && abs(trackDeltaEta)<0.015) || (cluster_pt > 20 && abs(trackDeltaEta)<0.015) || cluster_pt > 30)"
    cut = ""
    showerShapes = "(-0.921128 + 0.180511*TMath::Exp(-0.0400725*cluster_pt)>(-1)*(e2x5/e5x5))"
    Isolation = "((0.990748 + 5.64259*TMath::Exp(-0.0613952*cluster_pt))>cluster_iso)"
    deltaR = "((trackDeltaR<0.1))"
    deltaRgtr = "((trackDeltaR>0.1))"
    cut = showerShapes+"*"+Isolation
    #cut = showerShapes+"*"+Isolation+"*"+deltaR
    title1 = "L1EGamma Crystal (Electrons)"
    title2 = "L1EGamma Crystal (Fake)"

    lotsOf2DPlots = False
    #lotsOf2DPlots = True
    if lotsOf2DPlots :
        # Tmp Photo Stuff
        showerShapes = "(-0.903606 + 0.0248551*TMath::Exp(-0.196083*cluster_pt)>(-1)*(e2x5/e5x5))"
        Isolation = "((1.43074 + 3.14748*TMath::Exp(-0.188215*cluster_pt))>cluster_iso)"
        #cut = showerShapes+"*"+Isolation+"*(cluster_pt>8 || trackPt>8)"
        trkMatch="( (2.15946 + 0.684997 * cluster_pt) > trackPt)"
        cut = showerShapes+"*"+Isolation+"*"+trkMatch
        fPho = ROOT.TFile('egTriggerPhoEff.root','r')
        crystal_tree = fPho.Get('analyzer/crystal_tree')
        # End tmp photon stuff

        var = "zVertexEnergy:abs(trackZ-zVertex)"
        xaxis = "dZ (L1Trk, Trk Vtx)"
        xinfo = [80, 0., 20.]
        yaxis = "z Vertex #Sigma P_{T} (GeV)"
        yinfo = [50, 0, 100]
        c.SetTitle("trkBasedDZvsZVtxEnergy")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_pt:trackDeltaEta"
        xaxis = "d#eta (L1Trk, L1EG Crystal)"
        xinfo = [80, -0.05, 0.05]
        yaxis = "Cluster P_{T} (GeV)"
        yinfo = [50, 0, 50]
        c.SetTitle("trkDEta2D_Pt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackDeltaPhi:cluster_pt"
        yaxis = "d#phi (L1Trk, L1EG Crystal)"
        yinfo = [80, -0.5, 0.5]
        xaxis = "Cluster P_{T} (GeV)"
        xinfo = [50, 0, 50]
        c.SetTitle("clusterPtVtrkDPhi")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackDeltaEta:cluster_pt"
        yaxis = "d#eta (L1Trk, L1EG Crystal)"
        yinfo = [80, -0.5, 0.5]
        xaxis = "Cluster P_{T} (GeV)"
        xinfo = [50, 0, 50]
        c.SetTitle("clusterPtVtrkDEta")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackDeltaR:cluster_pt"
        yaxis = "#Delta R (L1Trk, L1EG Crystal)"
        yinfo = [80, -0.5, 0.5]
        xaxis = "Cluster P_{T} (GeV)"
        xinfo = [50, 0, 50]
        c.SetTitle("clusterPtVtrkDRold")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "TMath::Sqrt(trackDeltaEta*trackDeltaEta + trackDeltaPhi*trackDeltaPhi):cluster_pt"
        yaxis = "#Delta R (L1Trk, L1EG Crystal)"
        yinfo = [80, -0.5, 0.5]
        xaxis = "Cluster P_{T} (GeV)"
        xinfo = [50, 0, 50]
        c.SetTitle("clusterPtVtrkDRnew")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_hovere:trackDeltaEta"
        yaxis = "Cluster H/E"
        yinfo = [50, 0, 10]
        xaxis = "d#eta (Trk - L1)"
        xinfo = [50, -1., 1.]
        c.SetTitle("trkDEta2D_HoverE")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_iso:trackDeltaEta"
        yaxis = "Cluster Isolation (GeV)"
        yinfo = [50, 0, 25]
        c.SetTitle("trkDEta2D_Iso")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackChi2:trackDeltaEta"
        yaxis = "Track Chi2"
        yinfo = [50, 0, 300]
        c.SetTitle("trkDEta2D_trkChi2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        # dPhi
        var = "cluster_pt:trackDeltaPhi"
        title1 = "L1EGamma Crystal (Electrons)"
        title2 = "L1EGamma Crystal (Fake)"
        xaxis = "d#phi (L1Trk, L1EG Crystal)"
        xinfo = [80, -1., 1.]
        yaxis = "Cluster P_{T} (GeV)"
        yinfo = [50, 0, 50]
        c.SetTitle("trkDPhi2D_Pt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_hovere:trackDeltaPhi"
        yaxis = "Cluster H/E"
        yinfo = [50, 0, 10]
        c.SetTitle("trkDPhi2D_HoverE")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_iso:trackDeltaPhi"
        yaxis = "Cluster Isolation (GeV)"
        yinfo = [50, 0, 25]
        c.SetTitle("trkDPhi2D_Iso")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackChi2:trackDeltaPhi"
        yaxis = "Track Chi2"
        yinfo = [50, 0, 300]
        c.SetTitle("trkDPhi2D_trkChi2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackDeltaEta:trackDeltaPhi"
        yaxis = "Track d#eta"
        yinfo = [50, -1., 1.]
        c.SetTitle("trkDPhi2D_trkDEta")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "(trackPt-cluster_pt)/trackPt:cluster_pt"
        xaxis = "Cluster P_{T} (GeV)"
        yaxis = "P_{T} Resolution (trk-cluster)/trk"
        xinfo = [50, 0., 50.]
        yinfo = [50, -5., 2.]
        c.SetTitle("clusterPtVptRes")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackDeltaR:(trackPt-cluster_pt)/trackPt"
        yaxis = "Track #delta R"
        xaxis = "P_{T} Resolution (trk-cluster)/trk"
        yinfo = [50, 0., .5]
        xinfo = [50, -5., 2.]
        c.SetTitle("trkDR2D_ptRes")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_iso:cluster_hovere"
        yaxis = "Cluster Iso (GeV)"
        xaxis = "Cluster H/E"
        yinfo = [50, 0., 10.]
        xinfo = [70, 0., 7.]
        c.SetTitle("clusterIsoVclusterHoverE")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackPt:cluster_pt"
        xaxis = "Cluster P_{T} (GeV)"
        yaxis = "Track P_{T} (GeV)"
        xinfo = [50, 0., 50.]
        yinfo = [52, 0., 52.]
        c.SetTitle("clusterPtVTrkPt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_hovere:cluster_pt"
        xaxis = "Cluster P_{T} (GeV)"
        yaxis = "Cluster H/E"
        xinfo = [50, 0., 50.]
        yinfo = [50, 0., 15.]
        c.SetTitle("clusterPtVHoverE")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "cluster_iso:cluster_pt"
        xaxis = "Cluster P_{T} (GeV)"
        yaxis = "Cluster Iso (GeV)"
        xinfo = [50, 0., 50.]
        yinfo = [50, 0., 10.]
        c.SetTitle("clusterPtVClusterIso")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "((pt.5/pt.1 + pt.2)):cluster_pt"
        yaxis = "Ratio of crystal energies 1"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 20.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalRatios1")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "(1./(pt.5/pt.1 + pt.2)):cluster_pt"
        yaxis = "Ratio of crystal energies 2"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 10.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalRatios2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "pt.1:cluster_pt"
        yaxis = "Crystal Energy 1 (GeV)"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 50.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalEnergy1")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "pt.2:cluster_pt"
        yaxis = "Crystal Energy 2 (GeV)"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 50.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalEnergy2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "pt.3:cluster_pt"
        yaxis = "Crystal Energy 3 (GeV)"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 50.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalEnergy3")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "pt.4:cluster_pt"
        yaxis = "Crystal Energy 4 (GeV)"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 50.]
        xinfo = [60, 0., 60.]
        c.SetTitle("clusterPtVCrystalEnergy4")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "bremStrength:cluster_pt"
        yaxis = "BremStrength"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [50, 0., 1.1]
        xinfo = [60, 0., 50.]
        c.SetTitle("clusterPtVBremStrength")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "bremStrength:cluster_iso"
        yaxis = "BremStrength"
        xaxis = "Cluster Iso (GeV)"
        yinfo = [50, 0., 1.1]
        xinfo = [60, 0., 10.]
        c.SetTitle("clusterIsoVBremStrength")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "bremStrength:cluster_hovere"
        yaxis = "BremStrength"
        xaxis = "Cluster H/E"
        yinfo = [50, 0., 1.1]
        xinfo = [60, 0., 5.]
        c.SetTitle("clusterHoEVBremStrength")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackIsoConePtSum:trackIsoConeTrackCount"
        yaxis = "Iso Cone P_{T} Sum (GeV)"
        xaxis = "Iso Cone Track Count"
        yinfo = [500, 0., 60.]
        xinfo = [30, 0., 15.]
        c.SetTitle("trkIsoConePtSumVTrkIsoConeTrkCnt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackIsoConeTrackCount/trackPt:cluster_pt"
        yaxis = "Iso Cone Track Count/ Track P_{T}"
        xaxis = "Cluster P_{T} (GeV)"
        yinfo = [50, 0., 1.5]
        xinfo = [50, 0., 50.]
        c.SetTitle("trkIsoConeTrkCountOverTrackPtVClusterPt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackIsoConePtSum/trackPt:cluster_pt"
        yaxis = "Iso Cone P_{T} Sum/ Track P_{T}"
        xaxis = "Cluster P_{T} (GeV)"
        yinfo = [50, 0., 10.]
        xinfo = [50, 0., 50.]
        c.SetTitle("trkIsoConePtSumOverTrackPtVClusterPt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackIsoConePtSum/trackPt:trackIsoConeTrackCount"
        yaxis = "Iso Cone P_{T} Sum/ Track P_{T}"
        xaxis = "Iso Cone Track Count"
        yinfo = [50, 0., 10.]
        xinfo = [30, 0., 15.]
        c.SetTitle("trkIsoConePtSumOverTrackPtVTrkIsoConeTrkCnt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "abs(trackRInv):cluster_pt"
        yaxis = "abs( Track Curvature )"
        xaxis = "Cluster p_{T} (GeV)"
        yinfo = [100, 0., 0.007]
        xinfo = [50, 0., 50.]
        c.SetTitle("trkRInvVPt")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "abs(trackRInv):((trackPt-cluster_pt)/trackPt)"
        yaxis = "abs( Track Curvature )"
        xaxis = "P_{T} Res"
        yinfo = [100, 0., 0.007]
        xinfo = [50, -10., 2.]
        c.SetTitle("trkRInvVPtRes")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        cut = "cluster_pt < 25"
        var = "abs(trackRInv):((trackPt-cluster_pt)/trackPt)"
        yaxis = "abs( Track Curvature )"
        xaxis = "P_{T} Res"
        yinfo = [100, 0., 0.007]
        xinfo = [50, -10., 2.]
        c.SetTitle("trkRInvVPtRes0to25")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        cut = "cluster_pt > 25"
        var = "abs(trackRInv):((trackPt-cluster_pt)/trackPt)"
        yaxis = "abs( Track Curvature )"
        xaxis = "P_{T} Res"
        yinfo = [100, 0., 0.007]
        xinfo = [50, -10., 2.]
        c.SetTitle("trkRInvVPtRes25to50")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        #cut = ""
        var = "abs(trackZ-gen_z):cluster_pt"
        yaxis = "dZ(trk-Gen) (cm)"
        xaxis = "cluster P_{T} (GeV)"
        yinfo = [100, 0., 20.]
        xinfo = [50, 0., 50.]
        c.SetTitle("clusterPtVDZtrkGen")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "abs(trackZ-zVertex):cluster_pt"
        yaxis = "dZ(trk-PV) (cm)"
        xaxis = "cluster P_{T} (GeV)"
        yinfo = [100, 0., 20.]
        xinfo = [50, 0., 50.]
        c.SetTitle("clusterPtVDZtrkPV")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "abs(gen_z-zVertex):cluster_pt"
        yaxis = "dZ(gen-PV) (cm)"
        xaxis = "cluster P_{T} (GeV)"
        yinfo = [100, 0., 20.]
        xinfo = [50, 0., 50.]
        c.SetTitle("clusterPtVDZgenPV")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "bremStrength:trackChi2"
        yaxis = "bremStr"
        xaxis = "track chi 2"
        yinfo = [100, 0., 1.1]
        xinfo = [50, 0., 100.]
        c.SetTitle("bremStrVTrkChi2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)

        var = "trackChi2:cluster_pt"
        yaxis = "track chi 2"
        xaxis = "cluster P_{T} (GeV)"
        yinfo = [100, 0., 110.]
        xinfo = [50, 0., 50.]
        c.SetTitle("clusterPtVChi2")
        draw2DSets(c, crystal_tree, var, cut, title1, rate_tree, title2, xaxis, xinfo, yaxis, yinfo)




    c.Clear()


 
    # Offline reco pt
    #offlineRecoHist = ROOT.TH2F("offlineRecoHist", "Offline reco to gen. comparisonGen. pT (GeV)(reco-gen)/genCounts", 60, 0., 50., 60, -0.5, 0.5)
    #crystal_tree.Draw("(reco_pt-gen_pt)/gen_pt:gen_pt >> offlineRecoHist", "reco_pt>0", "colz")
    #c.SetLogy(0)
    #offlineRecoHist.Draw("colz")
    #c.Print("plots/offlineReco_vs_gen.png")
    #c.Clear()
 
#XXX    recoGenPtHist.SetTitle("Crystal EG algorithm pT resolution")
#XXX    # oldAlgrecoGenPtHist = (TH2F *) effFile.Get("analyzer/SLHCL1ExtraParticles:EGamma_reco_gen_pt")
#XXX    oldAlgrecoGenPtHist = effFile.Get("analyzer/l1extraParticlesUCT:All_reco_gen_pt")
#XXX    oldAlgrecoGenPtHist.SetTitle("Tower EG alg. momentum error")
#XXX    oldAlgrecoGenPtHist.GetYaxis().SetTitle("Relative Error (reco-gen)/gen")
#XXX    oldAlgrecoGenPtHist.SetMaximum(50)
#XXX    oldAlgrecoGenPtHist.SetLineColor(ROOT.kRed)
#XXX    c.SetCanvasSize(1200,600)
#XXX    c.Divide(2,1)
#XXX    c.cd(1)
#XXX    gPad.SetGridx(1)
#XXX    gPad.SetGridy(1)
#XXX    recoGenPtHist.Draw("colz")
#XXX    recoGenPtHist.GetYaxis().SetTitleOffset(1.4)
#XXX    c.cd(2)
#XXX    gPad.SetGridx(1)
#XXX    gPad.SetGridy(1)
#XXX    oldAlgrecoGenPtHist.Draw("colz")
#XXX    oldAlgrecoGenPtHist.GetYaxis().SetTitleOffset(1.4)
#XXX    c.Print("plots/reco_gen_pt.png")


    del c
    tdrstyle.setTDRStyle()
    gStyle.SetOptStat(0)

    
    c = ROOT.TCanvas('c', 'c', 800, 600)
    c.SetLogy(1)
    #c.SetGridx(1)
    #c.SetGridy(1)
    #gStyle.SetGridStyle(2)
    #gStyle.SetGridColor(ROOT.kGray+1)
    
    ''' RATE SECTION '''    
    xrange = [0., 60.]
    # Calo-based L1EG Rates
    c.SetName('dyncrystalEG_rate')
    c.SetTitle('')
    toDraw = [ hists['Stage-2 L1EG'], hists['L1EGamma Crystal'], ]
    drawRates( toDraw, c, 40000., xrange)

    # Photon
    c.SetName('dyncrystalEG_photon_rate')
    c.SetTitle('')
    toDraw = [ hists['Stage-2 L1EG'], hists['L1EGamma Crystal'], hists['L1EGamma Crystal Photon']]
    drawRates( toDraw, c, 40000., xrange)

    # Track
    c.SetName('dyncrystalEG_track_rate')
    c.SetTitle('')
    toDraw = [ hists['Stage-2 L1EG'], hists['L1EGamma Crystal'], hists['L1EGamma Crystal Track']]
    drawRates( toDraw, c, 40000., xrange)

    # All 
    c.SetName('dyncrystalEG_all_rate')
    c.SetTitle('')
    toDraw = [ hists['Stage-2 L1EG'], hists['L1EGamma Crystal'], hists['L1EGamma Crystal Track'], hists['L1EGamma Crystal Photon']]
    drawRates( toDraw, c, 40000., xrange)

    # Stage-2 rate solo
    c.SetName('stage2_rate')
    xrange = [0., 80.]
    toDraw = [ hists['Stage-2 L1EG'],]
    drawRates( toDraw, c, 40000., xrange)
    
    #c.SetName('dyncrystalEG_rate_UW')
    #c.SetTitle('EG Rates (UW only)')
    #toDraw = [ hists['L1EGamma Crystal'], hists['Phase 1 TDR'], hists['LLR Alg.'] ]
    #drawRates( toDraw, c, 40000., xrange)
    
    ''' EFFICIENCY SECTION '''
    xrange = [0., 100.]
    c.SetLogy(0)
    c.SetName("dyncrystalEG_efficiency_eta")
    c.SetTitle("EG Efficiencies")
    drawEfficiency([effHists['Stage2EtaHist'], effHists['newAlgEtaHist']], c, 1.3, "Gen #eta", [-3.,3.] , False, [-2.5, 2.5])
    c.SetName("dyncrystalEG_efficiency_track_eta")
    c.SetTitle("EG Efficiencies")
    drawEfficiency([effHists['Stage2EtaHist'], effHists['newAlgEtaHist'], effHists['newAlgTrkEtaHist']], c, 1.3, "Gen #eta", [-3.,3.] , False, [-2.5, 2.5])

    c.SetName("dyncrystalEG_efficiency_all_pt")
    c.SetTitle("")
    drawEfficiency([effHists['Stage2PtHist'], effHists['newAlgPtHist'], effHists['newAlgTrkPtHist'], effHists['newAlgPhotonPtHist']], c, 1.3, "Gen P_{T} (GeV)", xrange, True, [0.9, 2., 1., 0.])
    c.SetName("dyncrystalEG_efficiency_pt")
    c.SetTitle("")
    drawEfficiency([effHists['Stage2PtHist'], effHists['newAlgPtHist']], c, 1.3, "Gen P_{T} (GeV)", xrange, True, [0.9, 2., 1., 0.])
    c.SetName("dyncrystalEG_efficiency_track_pt")
    c.SetTitle("")
    drawEfficiency([effHists['Stage2PtHist'], effHists['newAlgPtHist'], effHists['newAlgTrkPtHist']], c, 1.3, "Gen P_{T} (GeV)", xrange, True, [0.9, 2., 1., 0.])
    c.SetName("dyncrystalEG_efficiency_photon_pt")
    c.SetTitle("")
    drawEfficiency([effHists['Stage2PtHist'], effHists['newAlgPtHist'], effHists['newAlgPhotonPtHist']], c, 1.3, "Gen P_{T} (GeV)", xrange, True, [0.9, 2., 1., 0.])

    # Map of possible pt values from file with suggested fit function params
    possiblePts = {'20' : [0.9, 20., 1., 0.], '30' : [0.95, 30., 1., 0.], '40': [0.95, 16., 1., 0.]}
    for crystalPt in newAlgGenPtHists :
        print crystalPt
        toPlot = []
        for s2 in stage2GenPtHists :
            print s2
            for pt in possiblePts.keys() :
                if pt in crystalPt.GetName() and pt in s2.GetName() :
                    print pt, crystalPt.GetName(), s2.GetName()
                    toPlot.append( crystalPt )
                    toPlot.append( s2 )
                    c.SetName("dyncrystalEG_threshold"+pt+"_efficiency_gen_pt")
                    drawEfficiency( toPlot, c, 1.3, "Gen P_{T} (GeV)", xrange, True, possiblePts[pt])


    ''' POSITION RECONSTRUCTION '''
    # Delta R Stuff
    c.SetGridx(0)
    c.SetGridy(0)
    c.SetName("dyncrystalEG_deltaR")
    c.SetTitle("")
    tDir = '/afs/cern.ch/user/t/truggles/www/Phase-II/v2p3'
    drawDRHists([effHists['newAlgDRHist'], effHists['stage2DRHist']], c, 0., False, tDir)

    # Delta Eta / Phi
    c.SetName("dyncrystalEG_deltaEta")
    drawDRHists([effHists['newAlgDEtaHist'], effHists['stage2DEtaHist']], c, 0., False, tDir)
    c.SetName("dyncrystalEG_deltaPhi")
    drawDRHists([effHists['newAlgDPhiHist'], effHists['stage2DPhiHist']], c, 0., False, tDir)
    c.SetName("dyncrystalEG_1D_pt_res")
    drawDRHists([effHists['newAlgGenRecoPtHist'], effHists['stage2GenRecoPtHist']], c, 0., False, tDir)

#XXX    """ 81X Check """
#XXX    Check81x = False
#XXX    if Check81x :
#XXX        f81x = ROOT.TFile('effTest.root','r')
#XXX        #f81x = ROOT.TFile('effTestMassInTeV.root','r')
#XXX        tree81X = f81x.Get('analyzer/crystal_tree')
#XXX        dEta81X = f81x.Get('analyzer/dyncrystalEG_deta')
#XXX        dEta81X.SetTitle('L1 EGamma Crystals 81X')
#XXX        dPhi81X = f81x.Get('analyzer/dyncrystalEG_dphi')
#XXX        dPhi81X.SetTitle('L1 EGamma Crystals 81X')
#XXX        dEta62X = effHists['newAlgDEtaHist'].Clone()
#XXX        dEta62X.SetTitle('L1 EGamma Crystals 62X')
#XXX        dPhi62X = effHists['newAlgDPhiHist'].Clone()
#XXX        dPhi62X.SetTitle('L1 EGamma Crystals 62X')
#XXX        c.SetName("dyncrystalEG_deltaEta_81X_Check")
#XXX        drawDRHists([dEta62X, dEta81X], c, 0.)
#XXX        c.SetName("dyncrystalEG_deltaPhi_81X_Check")
#XXX        drawDRHists([dPhi62X, dPhi81X], c, 0.)
#XXX        tree62XdEta = ROOT.TH1F("tree62XdEta", "L1 EGamma Crystal 62X Gen p_{T} #in[30,40];d#eta(Gen - L1)", 80, -0.2, 0.2)
#XXX        crystal_tree.Draw("(gen_eta - eta) >> tree62XdEta","(gen_pt > 30 && gen_pt < 40)")
#XXX        tree81XdEta = ROOT.TH1F("tree81XdEta", "L1 EGamma Crystal 81X, Gen p_{T} = 35;d#eta(L1 - Reco)", 80, -0.2, 0.2)
#XXX        tree81X.Draw("(deltaEta) >> tree81XdEta")
#XXX        c.SetName("dyncrystalEG_deltaEta_81X_CheckPt35")
#XXX        drawDRHists([tree62XdEta, tree81XdEta], c, 0.)
#XXX        tree62XdPhi = ROOT.TH1F("tree62XdPhi", "L1 EGamma Crystal 62X, Gen p_{T} #in[30,40];d#phi(Reco - L1)", 80, -0.1, 0.1)
#XXX        #crystal_tree.Draw("(gen_phi - phi) >> tree62XdPhi")
#XXX        crystal_tree.Draw("(deltaPhi) >> tree62XdPhi","(gen_pt > 30 && gen_pt < 40)")
#XXX        tree81XdPhi = ROOT.TH1F("tree81XdPhi", "L1 EGamma Crystal 81X, Gen p_{T} = 35;d#phi(L1 - Reco)", 60, -0.1, 0.1)
#XXX        tree81X.Draw("(deltaPhi) >> tree81XdPhi")
#XXX        c.SetName("dyncrystalEG_deltaPhi_81X_CheckPt35")
#XXX        drawDRHists([tree62XdPhi, tree81XdPhi], c, 0.4)
#XXX
#XXX    """ Stage 2 Comparisons """
#XXX    CheckStage2 = False
#XXX    if CheckStage2 :
#XXX        tree81XdEta2 = ROOT.TH1F("tree81XdEta2", "L1 EGamma Crystal 81X, Gen p_{T} = 35;d#eta(L1 - Reco)", 120, -0.15, 0.15)
#XXX        tree81X.Draw("(deltaEta) >> tree81XdEta2")
#XXX        tree81XdPhi2 = ROOT.TH1F("tree81XdPhi2", "L1 EGamma Crystal 81X, Gen p_{T} = 35;d#phi(L1 - Reco)", 120, -0.15, 0.15)
#XXX        tree81X.Draw("(deltaPhi) >> tree81XdPhi2")
#XXX        c.SetName("dyncrystalEG_deltaEta_Stage2Comp")
#XXX        dEtaStage2 = ROOT.TFile('dEta.root','r')
#XXX        stage2dEta = dEtaStage2.Get('dEta')
#XXX        stage2dEta.SetTitle('Stage-2 Level 1 EGamma Algo.')
#XXX        tree81XdEta2.SetTitle('Phase-2 Level 1 EGamma Crystal Algo.')
#XXX        drawDRHists([tree81XdEta2, stage2dEta], c, 0.4)
#XXX        c.SetName("dyncrystalEG_deltaPhi_Stage2Comp")
#XXX        dPhiStage2 = ROOT.TFile('dPhi.root','r')
#XXX        stage2dPhi = dPhiStage2.Get('dPhi')
#XXX        stage2dPhi.SetTitle('Stage-2 Level 1 EGamma Algo.')
#XXX        tree81XdPhi2.SetTitle('Phase-2 Level 1 EGamma Crystal Algo.')
#XXX        drawDRHists([tree81XdPhi2, stage2dPhi], c, 0.4)
#XXX    
#XXX
#XXX    DoPUStuff = False
#XXX    if DoPUStuff :
#XXX        fPU1 = ROOT.TFile('egTriggerRates45_p2.root','r')
#XXX        tPU1 = fPU1.Get('analyzer/crystal_tree')
#XXX        puAll = ROOT.TH1F("puAll", "13x113 PU Energy Total;#Sigma p_{T} (GeV)", 65, 0., 65)
#XXX        pu0 = ROOT.TH1F("pu0", "13x113 PU Energy < 0.5 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        pu5 = ROOT.TH1F("pu5", "13x113 PU Energy 0.5 - 1 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        pu1 = ROOT.TH1F("pu1", "13x113 PU Energy 1 - 2 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        pu2 = ROOT.TH1F("pu2", "13x113 PU Energy 2 - 3 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        pu3 = ROOT.TH1F("pu3", "13x113 PU Energy 3 - 4 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        pu4 = ROOT.TH1F("pu4", "13x113 PU Energy 4 - 5 GeV Hits;#Sigma p_{T} (GeV)", 50, 0., 50)
#XXX        tPU1.Draw("ecalPUtoPt >> puAll")
#XXX        tPU1.Draw("ecalPUtoPt0to500 >> pu0")
#XXX        tPU1.Draw("ecalPUtoPt500to1 >> pu5")
#XXX        tPU1.Draw("ecalPUtoPt1to2 >> pu1")
#XXX        tPU1.Draw("ecalPUtoPt2to3 >> pu2")
#XXX        tPU1.Draw("ecalPUtoPt3to4 >> pu3")
#XXX        tPU1.Draw("ecalPUtoPt4to5 >> pu4")
#XXX        c.SetName("puStudy_ECAL_PU_range")
#XXX        c.SetLogy()
#XXX        drawDRHists([pu0, pu5, pu1, pu2, pu3, pu4], c, 10)
#XXX        c.SetName("puStudy_ECAL_PU_Tot")
#XXX        c.SetLogy(0)
#XXX        drawDRHists([puAll,], c, .12)
#XXX
#XXX    doPhotonComp = True
#XXX    if doPhotonComp :
#XXX        # FROZEN
#XXX        showerShapesF = "(-0.896501 + 0.181135*TMath::Exp(-0.0696926*cluster_pt)>(-1)*(e2x5/e5x5))"
#XXX        IsolationF = "((1.0614 + 5.65869*TMath::Exp(-0.0646173*cluster_pt))>cluster_iso)"
#XXX        cut_ss_cIso = showerShapesF+"*"+IsolationF+"*(cluster_pt>20)"
#XXX        fPho = ROOT.TFile('egTriggerPhoEff.root','r')
#XXX        tPho = fPho.Get('analyzer/crystal_tree')
#XXX        crystal_tree = effFile.Get("analyzer/crystal_tree")
#XXX        min_ = 0.
#XXX        max_ = 1.2
#XXX        tmpAry=[60,min_,max_]
#XXX        varList = [
#XXX'e1x1/e1x2','e1x1/e2x1','e1x1/e2x2','e1x1/e2x3','e1x1/e2x5','e1x1/gen_energy','e1x1/e3x5',
#XXX            'e2x1/e1x2','e2x1/e2x2','e2x1/e2x3','e2x1/e2x5','e2x1/gen_energy','e2x1/e3x5',
#XXX                        'e1x2/e2x2','e1x2/e2x3','e1x2/e2x5','e1x2/gen_energy','e1x2/e3x5',
#XXX                                    'e2x2/e2x3','e2x2/e2x5','e2x2/gen_energy','e2x2/e3x5',
#XXX                                                'e2x3/e2x5','e2x3/gen_energy','e2x3/e3x5',
#XXX                                                            'e2x5/gen_energy','e2x5/e3x5',
#XXX        ]
#XXX        cnt = [0]
#XXX        for var in varList :
#XXX            h1 = simple1D( 'Photon', tPho, cnt, var, tmpAry, cut_ss_cIso )
#XXX            h2 = simple1D( 'Electron', crystal_tree, cnt, var, tmpAry, cut_ss_cIso )
#XXX            h3 = simple1D( 'Min-Bias', rate_tree, cnt, var, tmpAry, cut_ss_cIso )
#XXX            c.SetName("photonElecDiff"+var.replace('/','_'))
#XXX            drawDRHists([h1,h2,h3], c, 0. )
#XXX
#XXX
#XXX    c.SetName("dyncrystalEG_deltaEta")
#XXX    drawDRHists([effHists['newAlgDEtaHist'], effHists['UCTAlgDEtaHist']], c, 0.)
#XXX#    #c.SetName("dyncrystalEG_deltaPhi_UW")
#XXX#    #drawDRHists([effHists['newAlgDPhiHist'], effHists['UCTAlgDPhiHist'], effHists['dynAlgDPhiHist']], c, 0., [-0.5, 0.5])
#XXX#    drawDRHists([effHists['newAlgDPhiHist'], effHists['UCTAlgDPhiHist'] ], c, 0.5)
#XXX    # Draw L1EG Crystal dEta, dPhi with track matching
#XXX#    c.SetLogy(0)
#XXX#
#XXX#    cut15 = "cluster_pt > 15"
#XXX#    cut30 = "cluster_pt > 30"
#XXX#    c.SetName("dyncrystalEG_trkDeltaEtaFake")
#XXX#    trkDEtaF = ROOT.TH1F("trkDEtaF", "L1EGamma Crystal - Fakes", 80, -0.05, 0.05)
#XXX#    rate_tree.Draw("trackDeltaEta >> trkDEtaF")
#XXX#    trkDEtaF2 = ROOT.TH1F("trkDEtaF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, -0.05, 0.05)
#XXX#    rate_tree.Draw("trackDeltaEta >> trkDEtaF2", cut30)
#XXX#    trkDEtaF.GetXaxis().SetTitle("d#eta (L1Trk, L1EG Crystal)")
#XXX#    #drawDRHists( [trkDEtaF,], c, -1 )
#XXX#    c.SetName("dyncrystalEG_trkDeltaEta")
#XXX#    trkDEta = ROOT.TH1F("trkDEta", "L1EGamma Crystal", 80, -0.05, 0.05)
#XXX#    crystal_tree.Draw("trackDeltaEta >> trkDEta")
#XXX#    trkDEta2 = ROOT.TH1F("trkDEta2", "L1EGamma Crystal gtr 30p_{T}", 80, -0.05, 0.05)
#XXX#    crystal_tree.Draw("trackDeltaEta >> trkDEta2", cut30)
#XXX#    trkDEta.GetXaxis().SetTitle("d#eta (L1Trk, L1EG Crystal)")
#XXX#    #drawDRHists( [trkDEta,], c, -1 )
#XXX#    c.SetName("dyncrystalEG_trkDeltaEtaComp")
#XXX#    drawDRHists( [trkDEta, trkDEta2, trkDEtaF, trkDEtaF2], c, .2 )
#XXX#    del trkDEta, trkDEta2, trkDEtaF, trkDEtaF2
#XXX#    c.SetName("dyncrystalEG_trkDeltaEtaGen")
#XXX#    trkDEta = ROOT.TH1F("trkDEta", "L1EGamma Crystal", 80, -0.05, 0.05)
#XXX#    crystal_tree.Draw("trackEta-gen_eta >> trkDEta")
#XXX#    trkDEta.GetXaxis().SetTitle("d#eta (L1Trk, Gen Particle)")
#XXX#    drawDRHists( [trkDEta,], c, -1 )
#XXX#    del trkDEta
#XXX#
#XXX#    c.SetName("dyncrystalEG_trkDeltaPhiFake")
#XXX#    trkDPhiF = ROOT.TH1F("trkDPhiF", "L1EGamma Crystal - Fakes", 80, -0.2, 0.2)
#XXX#    rate_tree.Draw("trackDeltaPhi >> trkDPhiF")
#XXX#    trkDPhiF2 = ROOT.TH1F("trkDPhiF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, -0.2, 0.2)
#XXX#    rate_tree.Draw("trackDeltaPhi >> trkDPhiF2", cut30)
#XXX#    trkDPhiF.GetXaxis().SetTitle("d#phi (L1Trk, L1EG Crystal)")
#XXX#    #drawDRHists( [trkDPhiF,], c, -1 )
#XXX#    c.SetName("dyncrystalEG_trkDeltaPhi")
#XXX#    trkDPhi = ROOT.TH1F("trkDPhi", "L1EGamma Crystal", 80, -0.2, 0.2)
#XXX#    crystal_tree.Draw("trackDeltaPhi >> trkDPhi")
#XXX#    trkDPhi2 = ROOT.TH1F("trkDPhi2", "L1EGamma Crystal gtr 30p_{T}", 80, -0.2, 0.2)
#XXX#    crystal_tree.Draw("trackDeltaPhi >> trkDPhi2", cut30)
#XXX#    trkDPhi.GetXaxis().SetTitle("d#phi (L1Trk, L1EG Crystal)")
#XXX#    #drawDRHists( [trkDPhi,], c, -1 )
#XXX#    c.SetName("dyncrystalEG_trkDeltaPhiComp")
#XXX#    drawDRHists( [trkDPhi,trkDPhi2,trkDPhiF,trkDPhiF2], c, .55 )
#XXX#    del trkDPhi, trkDPhi2, trkDPhiF, trkDPhiF2
#XXX#
#XXX#    c.SetName("dyncrystalEG_trkDeltaPhiGen")
#XXX#    trkDPhi = ROOT.TH1F("trkDPhi", "L1EGamma Crystal", 80, -0.2, 0.2)
#XXX#    crystal_tree.Draw("trackPhi-gen_phi >> trkDPhi")
#XXX#    trkDPhi.GetXaxis().SetTitle("d#phi (L1Trk, Gen Particle)")
#XXX#    drawDRHists( [trkDPhi,], c, -1 )
#XXX#    del trkDPhi
#XXX#
#XXX#    # delta R
#XXX#    trkDRF = ROOT.TH1F("trkDRF", "L1EGamma Crystal - Fakes", 80, 0., 0.25)
#XXX#    rate_tree.Draw("trackDeltaR >> trkDRF")
#XXX#    trkDRF2 = ROOT.TH1F("trkDRF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 0.25)
#XXX#    rate_tree.Draw("trackDeltaR >> trkDRF2", cut30)
#XXX#    trkDR = ROOT.TH1F("trkDR", "L1EGamma Crystal", 80, 0., 0.25)
#XXX#    crystal_tree.Draw("trackDeltaR >> trkDR")
#XXX#    trkDR2 = ROOT.TH1F("trkDR2", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 0.25)
#XXX#    crystal_tree.Draw("trackDeltaR >> trkDR2", cut30)
#XXX#    trkDR.GetXaxis().SetTitle("#Delta R (L1Trk, L1EG Crystal)")
#XXX#    c.SetName("dyncrystalEG_trkDeltaRComp")
#XXX#    drawDRHists( [trkDR,trkDR2,trkDRF,trkDRF2], c, .40 )
#XXX#    del trkDR, trkDR2, trkDRF, trkDRF2
#XXX#
#XXX#    c.SetName("dyncrystalEG_trkDeltaPt")
#XXX#    trkDPt = ROOT.TH1F("trkDPt", "L1EGamma Crystal", 80, -1., 1.)
#XXX#    crystal_tree.Draw("(trackPt-cluster_pt)/trackPt >> trkDPt")
#XXX#    trkDPt.GetXaxis().SetTitle("#delta P_{T} (L1Trk - L1EG Crystal)/L1Trk")
#XXX#    #drawDRHists( [trkDPt,], c, -1 )
#XXX#    del trkDPt
#XXX#    # do this one two ways once .../trk Pt other .../cluster_pt
#XXX#    trkDPtF = ROOT.TH1F("trkDPtF", "L1EGamma Crystal - Fakes", 80, -2., 5.)
#XXX#    rate_tree.Draw("(trackPt-cluster_pt)/cluster_pt >> trkDPtF")
#XXX#    trkDPtF2 = ROOT.TH1F("trkDPtF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, -2., 5.)
#XXX#    rate_tree.Draw("(trackPt-cluster_pt)/cluster_pt >> trkDPtF2", cut30)
#XXX#    trkDPt = ROOT.TH1F("trkDPt", "L1EGamma Crystal", 80, -2., 5.)
#XXX#    crystal_tree.Draw("(trackPt-cluster_pt)/cluster_pt >> trkDPt")
#XXX#    trkDPt2 = ROOT.TH1F("trkDPt2", "L1EGamma Crystal gtr 30p_{T}", 80, -2., 5.)
#XXX#    crystal_tree.Draw("(trackPt-cluster_pt)/cluster_pt >> trkDPt2", cut30)
#XXX#    trkDPt.GetXaxis().SetTitle("P_{T} Resolution (GeV)  (L1Trk - L1EG Crystal)/L1EG Crystal")
#XXX#    c.SetName("dyncrystalEG_trkDeltaClusterPtOverClusterComp")
#XXX#    drawDRHists( [trkDPt,trkDPt2,trkDPtF,trkDPtF2], c, .6 )
#XXX#    del trkDPt, trkDPt2, trkDPtF, trkDPtF2
#XXX#    c.SetName("dyncrystalEG_trkDeltaPtFakes")
#XXX#    trkDPtF = ROOT.TH1F("trkDPtF", "L1EGamma Crystal - Fakes", 80, -6., 2.)
#XXX#    rate_tree.Draw("(trackPt-cluster_pt)/trackPt >> trkDPtF")
#XXX#    trkDPtF2 = ROOT.TH1F("trkDPtF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, -6., 2.)
#XXX#    rate_tree.Draw("(trackPt-cluster_pt)/trackPt >> trkDPtF2", cut30)
#XXX#    trkDPt = ROOT.TH1F("trkDPt", "L1EGamma Crystal", 80, -6., 2.)
#XXX#    crystal_tree.Draw("(trackPt-cluster_pt)/trackPt >> trkDPt")
#XXX#    trkDPt.GetXaxis().SetTitle("P_{T} Resolution (GeV)  (L1Trk - L1EG Crystal)/L1Trk")
#XXX#    trkDPt2 = ROOT.TH1F("trkDPt2", "L1EGamma Crystal gtr 30p_{T}", 80, -6., 2.)
#XXX#    crystal_tree.Draw("(trackPt-cluster_pt)/trackPt >> trkDPt2", cut30)
#XXX#    trkDPtF.GetXaxis().SetTitle("#delta P_{T} (L1Trk - L1EG Crystal)/L1Trk")
#XXX#    #drawDRHists( [trkDPtF,], c, -1 )
#XXX#    c.SetName("dyncrystalEG_trkDeltaPtComp")
#XXX#    drawDRHists( [trkDPt,trkDPt2,trkDPtF,trkDPtF2], c, .6 )
#XXX#    del trkDPt, trkDPt2, trkDPtF, trkDPtF2
#XXX#    c.SetName("dyncrystalEG_trkDeltaPtGen")
#XXX#    trkDPt = ROOT.TH1F("trkDPt", "L1EGamma Crystal", 80, -1., 1.)
#XXX#    crystal_tree.Draw("(trackPt-gen_pt)/trackPt >> trkDPt")
#XXX#    trkDPt.GetXaxis().SetTitle("#delta P_{T} (L1Trk - Gen Particle)/L1Trk")
#XXX#    #drawDRHists( [trkDPt,], c, -1 )
#XXX#    del trkDPt
#XXX#
#XXX#    # Iso Comparisons
#XXX#    # Iso / clusterPt
#XXX#    clusterIso = ROOT.TH1F("clusterIso", "L1EGamma Crystal", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/cluster_pt >> clusterIso")
#XXX#    clusterIso2 = ROOT.TH1F("clusterIso2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/cluster_pt >> clusterIso2", cut15)
#XXX#    clusterIso3 = ROOT.TH1F("clusterIso3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/cluster_pt >> clusterIso3", cut30)
#XXX#    clusterIsoF = ROOT.TH1F("clusterIsoF", "L1EGamma Crystal - Fakes", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/cluster_pt >> clusterIsoF")
#XXX#    clusterIsoF2 = ROOT.TH1F("clusterIsoF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/cluster_pt >> clusterIsoF2", cut15)
#XXX#    clusterIsoF3 = ROOT.TH1F("clusterIsoF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/cluster_pt >> clusterIsoF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoOverPtComp")
#XXX#    clusterIso.GetXaxis().SetTitle("Cluster Isolation/Cluster P_{T}")
#XXX#    drawDRHists( [clusterIso,clusterIso2,clusterIso3,clusterIsoF,clusterIsoF2,clusterIsoF3], c, .6 )
#XXX#    del clusterIso, clusterIso2, clusterIso3, clusterIsoF, clusterIsoF2, clusterIsoF3
#XXX#    # Iso / trackPt
#XXX#    clusterIso = ROOT.TH1F("clusterIso", "L1EGamma Crystal", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/trackPt >> clusterIso")
#XXX#    clusterIso2 = ROOT.TH1F("clusterIso2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/trackPt >> clusterIso2", cut15)
#XXX#    clusterIso3 = ROOT.TH1F("clusterIso3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 1.)
#XXX#    crystal_tree.Draw("cluster_iso/trackPt >> clusterIso3", cut30)
#XXX#    clusterIsoF = ROOT.TH1F("clusterIsoF", "L1EGamma Crystal - Fakes", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/trackPt >> clusterIsoF")
#XXX#    clusterIsoF2 = ROOT.TH1F("clusterIsoF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/trackPt >> clusterIsoF2", cut15)
#XXX#    clusterIsoF3 = ROOT.TH1F("clusterIsoF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 1.)
#XXX#    rate_tree.Draw("cluster_iso/trackPt >> clusterIsoF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoOverTrackPtComp")
#XXX#    clusterIso.GetXaxis().SetTitle("Cluster Isolation/Track P_{T}")
#XXX#    drawDRHists( [clusterIso,clusterIso2,clusterIso3,clusterIsoF,clusterIsoF2,clusterIsoF3], c, .6 )
#XXX#    del clusterIso, clusterIso2, clusterIso3, clusterIsoF, clusterIsoF2, clusterIsoF3
#XXX#    # Normal Isolation
#XXX#    clusterIso = ROOT.TH1F("clusterIso", "L1EGamma Crystal", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso >> clusterIso")
#XXX#    clusterIso2 = ROOT.TH1F("clusterIso2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso >> clusterIso2", cut15)
#XXX#    clusterIso3 = ROOT.TH1F("clusterIso3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso >> clusterIso3", cut30)
#XXX#    clusterIsoF = ROOT.TH1F("clusterIsoF", "L1EGamma Crystal - Fakes", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso >> clusterIsoF")
#XXX#    clusterIsoF2 = ROOT.TH1F("clusterIsoF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso >> clusterIsoF2", cut15)
#XXX#    clusterIsoF3 = ROOT.TH1F("clusterIsoF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso >> clusterIsoF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoComp")
#XXX#    clusterIso.GetXaxis().SetTitle("Cluster Isolation (GeV)")
#XXX#    drawDRHists( [clusterIso,clusterIso2,clusterIso3,clusterIsoF,clusterIsoF2,clusterIsoF3], c, .3 )
#XXX#    del clusterIso, clusterIso2, clusterIso3, clusterIsoF, clusterIsoF2, clusterIsoF3
#XXX#    # Isolation using trackPt
#XXX#    clusterIso = ROOT.TH1F("clusterIso", "L1EGamma Crystal", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIso")
#XXX#    clusterIso2 = ROOT.TH1F("clusterIso2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIso2", cut15)
#XXX#    clusterIso3 = ROOT.TH1F("clusterIso3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIso3", cut30)
#XXX#    clusterIsoF = ROOT.TH1F("clusterIsoF", "L1EGamma Crystal - Fakes", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIsoF")
#XXX#    clusterIsoF2 = ROOT.TH1F("clusterIsoF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIsoF2", cut15)
#XXX#    clusterIsoF3 = ROOT.TH1F("clusterIsoF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("cluster_iso*cluster_pt/trackPt >> clusterIsoF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoTrackPtComp")
#XXX#    clusterIso.GetXaxis().SetTitle("Cluster Isolation w/ Track P_{T} (GeV)")
#XXX#    drawDRHists( [clusterIso,clusterIso2,clusterIso3,clusterIsoF,clusterIsoF2,clusterIsoF3], c, .3 )
#XXX#    del clusterIso, clusterIso2, clusterIso3, clusterIsoF, clusterIsoF2, clusterIsoF3
#XXX#
#XXX#    # H/E Comparisons
#XXX#    # H/E / Cluster PT
#XXX#    clusterHoE = ROOT.TH1F("clusterHoE", "L1EGamma Crystal", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/cluster_pt >> clusterHoE")
#XXX#    clusterHoE2 = ROOT.TH1F("clusterHoE2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/cluster_pt >> clusterHoE2", cut15)
#XXX#    clusterHoE3 = ROOT.TH1F("clusterHoE3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/cluster_pt >> clusterHoE3", cut30)
#XXX#    clusterHoEF = ROOT.TH1F("clusterHoEF", "L1EGamma Crystal - Fakes", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/cluster_pt >> clusterHoEF")
#XXX#    clusterHoEF2 = ROOT.TH1F("clusterHoEF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/cluster_pt >> clusterHoEF2", cut15)
#XXX#    clusterHoEF3 = ROOT.TH1F("clusterHoEF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/cluster_pt >> clusterHoEF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterHoEOverPtComp")
#XXX#    clusterHoE.GetXaxis().SetTitle("(Cluster H/E)/Cluster P_{T} (GeV^{-1})")
#XXX#    drawDRHists( [clusterHoE,clusterHoE2,clusterHoE3,clusterHoEF,clusterHoEF2,clusterHoEF3], c, .5 )
#XXX#    del clusterHoE, clusterHoE2, clusterHoE3, clusterHoEF, clusterHoEF2, clusterHoEF3
#XXX#    # H/E / Track PT
#XXX#    clusterHoE = ROOT.TH1F("clusterHoE", "L1EGamma Crystal", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/trackPt >> clusterHoE")
#XXX#    clusterHoE2 = ROOT.TH1F("clusterHoE2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/trackPt >> clusterHoE2", cut15)
#XXX#    clusterHoE3 = ROOT.TH1F("clusterHoE3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., .2)
#XXX#    crystal_tree.Draw("cluster_hovere/trackPt >> clusterHoE3", cut30)
#XXX#    clusterHoEF = ROOT.TH1F("clusterHoEF", "L1EGamma Crystal - Fakes", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/trackPt >> clusterHoEF")
#XXX#    clusterHoEF2 = ROOT.TH1F("clusterHoEF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/trackPt >> clusterHoEF2", cut15)
#XXX#    clusterHoEF3 = ROOT.TH1F("clusterHoEF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., .2)
#XXX#    rate_tree.Draw("cluster_hovere/trackPt >> clusterHoEF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterHoEOverTrackPtComp")
#XXX#    clusterHoE.GetXaxis().SetTitle("(Cluster H/E)/Track P_{T} (GeV^{-1})")
#XXX#    drawDRHists( [clusterHoE,clusterHoE2,clusterHoE3,clusterHoEF,clusterHoEF2,clusterHoEF3], c, .5 )
#XXX#    del clusterHoE, clusterHoE2, clusterHoE3, clusterHoEF, clusterHoEF2, clusterHoEF3
#XXX#    clusterHoE = ROOT.TH1F("clusterHoE", "L1EGamma Crystal", 80, 0., 5.)
#XXX#    crystal_tree.Draw("cluster_hovere >> clusterHoE")
#XXX#    clusterHoE2 = ROOT.TH1F("clusterHoE2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 5.)
#XXX#    crystal_tree.Draw("cluster_hovere >> clusterHoE2", cut15)
#XXX#    clusterHoE3 = ROOT.TH1F("clusterHoE3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 5.)
#XXX#    crystal_tree.Draw("cluster_hovere >> clusterHoE3", cut30)
#XXX#    clusterHoEF = ROOT.TH1F("clusterHoEF", "L1EGamma Crystal - Fakes", 80, 0., 5.)
#XXX#    rate_tree.Draw("cluster_hovere >> clusterHoEF")
#XXX#    clusterHoEF2 = ROOT.TH1F("clusterHoEF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 5.)
#XXX#    rate_tree.Draw("cluster_hovere >> clusterHoEF2", cut15)
#XXX#    clusterHoEF3 = ROOT.TH1F("clusterHoEF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 5.)
#XXX#    rate_tree.Draw("cluster_hovere >> clusterHoEF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterHoEComp")
#XXX#    clusterHoE.GetXaxis().SetTitle("Cluster H/E")
#XXX#    drawDRHists( [clusterHoE,clusterHoE2,clusterHoE3,clusterHoEF,clusterHoEF2,clusterHoEF3], c, .35 )
#XXX#    del clusterHoE, clusterHoE2, clusterHoE3, clusterHoEF, clusterHoEF2, clusterHoEF3
#XXX#
#XXX#    # Track Iso Cone
#XXX#    clusterIsoConeNumTrks = ROOT.TH1F("clusterIsoConeNumTrks", "L1EGamma Crystal", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrks")
#XXX#    clusterIsoConeNumTrks2 = ROOT.TH1F("clusterIsoConeNumTrks2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrks2", cut15)
#XXX#    clusterIsoConeNumTrks3 = ROOT.TH1F("clusterIsoConeNumTrks3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrks3", cut30)
#XXX#    clusterIsoConeNumTrksF = ROOT.TH1F("clusterIsoConeNumTrksF", "L1EGamma Crystal - Fakes", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrksF")
#XXX#    clusterIsoConeNumTrksF2 = ROOT.TH1F("clusterIsoConeNumTrksF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrksF2", cut15)
#XXX#    clusterIsoConeNumTrksF3 = ROOT.TH1F("clusterIsoConeNumTrksF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConeTrackCount >> clusterIsoConeNumTrksF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoConeNumTrksComp")
#XXX#    clusterIsoConeNumTrks.GetXaxis().SetTitle("Iso Cone Num Trks")
#XXX#    drawDRHists( [clusterIsoConeNumTrks,clusterIsoConeNumTrks2,clusterIsoConeNumTrks3,clusterIsoConeNumTrksF,clusterIsoConeNumTrksF2,clusterIsoConeNumTrksF3], c, 1. )
#XXX#    del clusterIsoConeNumTrks, clusterIsoConeNumTrks2, clusterIsoConeNumTrks3, clusterIsoConeNumTrksF, clusterIsoConeNumTrksF2, clusterIsoConeNumTrksF3
#XXX#    clusterIsoConePtSum = ROOT.TH1F("clusterIsoConePtSum", "L1EGamma Crystal", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSum")
#XXX#    clusterIsoConePtSum2 = ROOT.TH1F("clusterIsoConePtSum2", "L1EGamma Crystal gtr 15p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSum2", cut15)
#XXX#    clusterIsoConePtSum3 = ROOT.TH1F("clusterIsoConePtSum3", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 10.)
#XXX#    crystal_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSum3", cut30)
#XXX#    clusterIsoConePtSumF = ROOT.TH1F("clusterIsoConePtSumF", "L1EGamma Crystal - Fakes", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSumF")
#XXX#    clusterIsoConePtSumF2 = ROOT.TH1F("clusterIsoConePtSumF2", "L1EGamma Crystal - Fakes gtr 15p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSumF2", cut15)
#XXX#    clusterIsoConePtSumF3 = ROOT.TH1F("clusterIsoConePtSumF3", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 10.)
#XXX#    rate_tree.Draw("trackIsoConePtSum >> clusterIsoConePtSumF3", cut30)
#XXX#    c.SetName("dyncrystalEG_clusterIsoConePtSumComp")
#XXX#    clusterIsoConePtSum.GetXaxis().SetTitle("Iso Cone P_{T} Sum (GeV)")
#XXX#    drawDRHists( [clusterIsoConePtSum,clusterIsoConePtSum2,clusterIsoConePtSum3,clusterIsoConePtSumF,clusterIsoConePtSumF2,clusterIsoConePtSumF3], c, .1 )
#XXX#    del clusterIsoConePtSum, clusterIsoConePtSum2, clusterIsoConePtSum3, clusterIsoConePtSumF, clusterIsoConePtSumF2, clusterIsoConePtSumF3
#XXX#
#XXX#    # Shower Shape vars
#XXX#    E2x5 = ROOT.TH1F("E2x5", "L1EGamma Crystal", 80, 0., 60.)
#XXX#    crystal_tree.Draw("e2x5 >> E2x5")
#XXX#    E2x52 = ROOT.TH1F("E2x52", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 60.)
#XXX#    crystal_tree.Draw("e2x5 >> E2x52", cut30)
#XXX#    E2x5F = ROOT.TH1F("E2x5F", "L1EGamma Crystal - Fakes", 80, 0., 60.)
#XXX#    rate_tree.Draw("e2x5 >> E2x5F")
#XXX#    E2x5F2 = ROOT.TH1F("E2x5F2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 60.)
#XXX#    rate_tree.Draw("e2x5 >> E2x5F2", cut30)
#XXX#    c.SetName("dyncrystalEG_E2x5Comp")
#XXX#    E2x5.GetXaxis().SetTitle("Energy 2x5 Crystals (GeV)")
#XXX#    drawDRHists( [E2x5,E2x52,E2x5F,E2x5F2], c, .4 )
#XXX#    del E2x5, E2x52, E2x5F, E2x5F2
#XXX#    E5x5 = ROOT.TH1F("E5x5", "L1EGamma Crystal", 80, 0., 60.)
#XXX#    crystal_tree.Draw("e5x5 >> E5x5")
#XXX#    E5x52 = ROOT.TH1F("E5x52", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 60.)
#XXX#    crystal_tree.Draw("e5x5 >> E5x52", cut30)
#XXX#    E5x5F = ROOT.TH1F("E5x5F", "L1EGamma Crystal - Fakes", 80, 0., 60.)
#XXX#    rate_tree.Draw("e5x5 >> E5x5F")
#XXX#    E5x5F2 = ROOT.TH1F("E5x5F2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 60.)
#XXX#    rate_tree.Draw("e5x5 >> E5x5F2", cut30)
#XXX#    c.SetName("dyncrystalEG_E5x5Comp")
#XXX#    E5x5.GetXaxis().SetTitle("Energy 5x5 Crystals (GeV)")
#XXX#    drawDRHists( [E5x5,E5x52,E5x5F,E5x5F2], c, .4 )
#XXX#    del E5x5, E5x52, E5x5F, E5x5F2
#XXX#    ShowerShape = ROOT.TH1F("ShowerShape", "L1EGamma Crystal", 80, 0., 1.)
#XXX#    crystal_tree.Draw("(e2x5/e5x5) >> ShowerShape")
#XXX#    ShowerShape2 = ROOT.TH1F("ShowerShape2", "L1EGamma Crystal gtr 30p_{T}", 80, 0., 1.)
#XXX#    crystal_tree.Draw("(e2x5/e5x5) >> ShowerShape2", cut30)
#XXX#    ShowerShapeF = ROOT.TH1F("ShowerShapeF", "L1EGamma Crystal - Fakes", 80, 0., 1.)
#XXX#    rate_tree.Draw("(e2x5/e5x5) >> ShowerShapeF")
#XXX#    ShowerShapeF2 = ROOT.TH1F("ShowerShapeF2", "L1EGamma Crystal - Fakes gtr 30p_{T}", 80, 0., 1.)
#XXX#    rate_tree.Draw("(e2x5/e5x5) >> ShowerShapeF2", cut30)
#XXX#    c.SetName("dyncrystalEG_E2x5OverE5x5Comp")
#XXX#    ShowerShape.GetXaxis().SetTitle("Energy (2x5)/(5x5)")
#XXX#    drawDRHists( [ShowerShape,ShowerShape2,ShowerShapeF,ShowerShapeF2], c, .4 )
#XXX#    del ShowerShape, ShowerShape2, ShowerShapeF, ShowerShapeF2
#XXX#
#XXX#    # Track Pt comparisons for no EG matching for highest pt track (dR < 10) and
#XXX#    # normal matching
#XXX#    trackPtHist = ROOT.TH1F("trackPtHist", "L1 Trk - EG Matched", 55, 0., 55)
#XXX#    crystal_tree.Draw("trackPt >> trackPtHist","")
#XXX#    trackPtHist10 = ROOT.TH1F("trackPtHist10", "L1 Trk - EG Matched, P_{T}>10", 55, 0., 55)
#XXX#    crystal_tree.Draw("trackPt >> trackPtHist10","trackPt>10")
#XXX#    trackFile = ROOT.TFile('egTriggerRateTracks.root','r')
#XXX#    print trackFile.ls()
#XXX#    trackTree = trackFile.Get('analyzer/crystal_tree')
#XXX#    print trackTree.ls()
#XXX#    trackPtHistF = ROOT.TH1F("trackPtHistF", "L1 Trk - No Match", 55, 0., 55.)
#XXX#    trackTree.Draw("trackPt >> trackPtHistF","")
#XXX#    trackPtHistF10 = ROOT.TH1F("trackPtHistF10", "L1 Trk - No Match, P_{T}>10", 55, 0., 55.)
#XXX#    trackTree.Draw("trackPt >> trackPtHistF10","trackPt>10")
#XXX#    print trackPtHistF.Integral()
#XXX#    c.SetName("dyncrystalEG_trackPtEGMatchVNoMatch")
#XXX#    trackPtHist.GetXaxis().SetTitle("Track P_{T} (GeV)")
#XXX#    c.SetLogy(1)
#XXX#    #drawDRHists( [trackPtHist,trackPtHist10,trackPtHistF,trackPtHistF10], c, 10. )
#XXX#    drawDRHists( [trackPtHist,trackPtHistF], c, 10. )
#XXX#    c.SetLogy(0)
#XXX#    del trackPtHist, trackPtHist10, trackPtHistF, trackPtHistF10
#XXX#
#XXX#
#XXX#    # Back to DeltaR stuff
#XXX#    #newAlgDRCutsHist = ROOT.TH1F("newAlgDRCutsHist", "L1EGamma Crystal", 50, 0., .25)
#XXX#    #crystal_tree.Draw("deltaR >> newAlgDRCutsHist", "passed && gen_pt > 20.", "goff")
#XXX#    #c.SetName("dyncrystalEG_deltaR_ptcut")
#XXX#    #drawDRHists([newAlgDRCutsHist], c, 0.)
#XXX#
#XXX#
#XXX#    ''' PT RECONSTRUCTION: (reco-gen) / reco '''
#XXX    #c.SetName("dyncrystalEG_RecoGenPt_UW")
#XXX    #drawDRHists([effHists['newAlgGenRecoPtHist'], effHists['UCTAlgGenRecoPtHist'], effHists['dynAlgGenRecoPtHist']], c, 0., [-1., 1.])
#XXX    c.SetName("dyncrystalEG_RecoGenPt")
#XXX    effHists['newAlgGenRecoPtHist'].GetXaxis().SetTitle("P_{T} (reco-gen)/gen")
#XXX    drawDRHists([effHists['newAlgGenRecoPtHist'], effHists['UCTAlgGenRecoPtHist']], c, 0., True)
#XXX#    
#XXX#
#XXX# 
#XXX#    c.Clear()
#XXX    #brem_dphi = ROOT.TH2F("brem_dphi", "d#phi(uslE+lslE)/clusterEnergy", 50, -0.1, 0.1, 50, 0, 1)
#XXX    #crystal_tree.Draw("(uslE+lslE)/cluster_energy : deltaPhi >> brem_dphi", "passed && cluster_pt > 10", "goff")
#XXX    #brem_dphi.Draw("colz")
#XXX    #c.Print("plots/brem_dphi_hist.png")
#XXX 
#XXX
