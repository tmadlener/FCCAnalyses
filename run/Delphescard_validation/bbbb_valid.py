from collections import namedtuple
import ROOT
import argparse
import os 

ROOT.gROOT.SetBatch()
ROOT.gStyle.SetOptTitle(0)

def makePlots(dict_of_vars, sel_cutstring, input_filepath, out_dir_base, out_format = ".png", do_log_y=False):

	if not os.path.exists(out_dir_base):
		os.mkdir(out_dir_base)

	rdf = ROOT.RDataFrame("events", input_filepath)

	if sel_cutstring:
		rdf = rdf.Filter(sel_cutstring) #apply the selection

	if not rdf:
		print("Empty file for:", input_filepath, " Exiting.")
		return

	for plot_name, plot in dict_of_vars.items():
		print("Plotting:", plot_name)

		has_variable_binning = False
		if not isinstance(plot.nbins, int):
			has_variable_binning = True
			hist_binEdges = array("d", plot.nbins)
			hist_nBins = len(plot.nbins)-1
			model = ROOT.RDF.TH1DModel(plot_name+"_model_hist", plot_name, hist_nBins, hist_binEdges)
		else:
			model = ROOT.RDF.TH1DModel(plot_name+"_model_hist", plot_name, plot.nbins, plot.xmin, plot.xmax)

		tmp_hist = rdf.Histo1D(model, plot.name)
		if not tmp_hist.GetEntries():
			print("Empty histogram for:", input_filepath, " Exiting.")
			return

		tmp_hist.GetXaxis().SetTitle(plot.label)
		tmp_hist.GetYaxis().SetTitle("Raw MC events")
		tmp_hist.SetFillColor(ROOT.kCyan+2)
		tmp_hist.SetLineColor(ROOT.kCyan+2)

		#write out
		if sel_cutstring == "":
			filename = "bbbb_noSel_"+plot_name+out_format
		else:
			filename = "bbbb_withSel_"+plot_name+out_format
		fileout = os.path.join(out_dir_base, filename)

		canvas = ROOT.TCanvas("canvas", "canvas", 800, 800) 
		tmp_hist.Draw("hist same")
		canvas.SaveAs(fileout)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Plot distributions for validation of new signal files")
	parser.add_argument('--input', '-i', metavar="INPUTDIR", dest="inDir", required=True, help="Path to the input directory")
	parser.add_argument('--outdir', '-o', metavar="OUTPUTDIR", dest="outDir", required=True, help="Output directory.")
	args = parser.parse_args()

	##setup for bbbb:
	infilepath = os.path.join(args.inDir, "FCChh_EvtGen_pwp8_pp_hh_5f_hhbbbb_tester_new_card.root")

	#use a custim namedntuple to transfer the plotting info
	PlotSpecs = namedtuple('PlotSpecs', ['name', 'xmin', 'xmax', 'label', 'nbins'])


	#plot some vars without any selection:
	bbbb_plot_vars_nosel = {
		"n_b_jets_loose":PlotSpecs(name="n_b_jets_loose", xmin=0., xmax=10., label="n_b_jets_loose", nbins=10),
		"n_b_jets_medium":PlotSpecs(name="n_b_jets_medium", xmin=0., xmax=10., label="n_b_jets_medium", nbins=10),
		"n_b_jets_tight":PlotSpecs(name="n_b_jets_tight", xmin=0., xmax=10., label="n_b_jets_tight", nbins=10),
	}

	sel_cuts_empty = ""
	makePlots(bbbb_plot_vars_nosel, sel_cuts_empty, infilepath, args.outDir)

	##apply selection to be able to plot event wide kinematic variables

	sel_cuts = "n_b_jets_medium == 4"


	bbbb_plot_vars = {
		"m_bb":PlotSpecs(name="m_bb", xmin=80., xmax=200., label="m_{bb} (lead) [GeV]", nbins=60),
		"m_bb_sub":PlotSpecs(name="m_bb_sub", xmin=80., xmax=200., label="m_{bb} (sublead)  [GeV]", nbins=60),
		"pT_b_jets_medium":PlotSpecs(name="pT_b_jets_medium", xmin=0., xmax=500., label="p_{T} sel. b-jets [GeV]", nbins=75), #plots the pT of all 4 selected bs into one hist!
		"eta_b_jets_medium":PlotSpecs(name="eta_b_jets_medium", xmin=0., xmax=5., label="#eta sel. pb-jets", nbins=20), #plots the eta of all 4 selected bs into one hist!
	}

	makePlots(bbbb_plot_vars, sel_cuts, infilepath, args.outDir)

#python bbbb_valid.py -i /eos/user/b/bistapf/FCChh_EvtGen/FCCAnalysis_ntuples/ -o ./bbbb_plots/