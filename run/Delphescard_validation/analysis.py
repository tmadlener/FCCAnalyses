import ROOT
import os
import argparse

#setup of the libraries, following the example:
print ("Load cxx analyzers ... ",)
ROOT.gSystem.Load("libedm4hep")
ROOT.gSystem.Load("libpodio")
ROOT.gSystem.Load("libFCCAnalyses")
# ROOT.gErrorIgnoreLevel = ROOT.kFatal
_edm  = ROOT.edm4hep.ReconstructedParticleData()
_pod  = ROOT.podio.ObjectID()
_fcc  = ROOT.dummyLoader

print ('edm4hep  ',_edm)
print ('podio    ',_pod)
print ('fccana   ',_fcc)


# print ("----> Load cxx analyzers from libFCCAnalyses... ",)
# ROOT.gSystem.Load("libFCCAnalyses")
# ROOT.gErrorIgnoreLevel = ROOT.kFatal
# #Is this still needed?? 01/04/2022 still to be the case
# _fcc  = ROOT.dummyLoader




#analysis class implementing the "event loop"
class analysis():

	#__________________________________________________________
	def __init__(self, inputlist, outname, ncpu, doDebug = False):
		self.outname = outname

		if ".root" not in outname:
			self.outname+=".root"

		if not doDebug:
			ROOT.ROOT.EnableImplicitMT(ncpu)

		self.df = ROOT.RDataFrame("events", inputlist)

		self.do_debug = doDebug

	#__________________________________________________________
	#definition of all variables for ntuples

	def run(self):

		#check only first 100 events, if debugging
		if self.do_debug:
			self.df = self.df.Range(0, 100)

		#filling the requested variables from the EDM4HEP file by defining them as new branches/columns of the RDF
		#the functions to calculate them are defined in the C++ code in FCCAnalyses/analyzers/dataframe
		df_out = (self.df

			########################################### JETS ########################################### 
			# #all jets
			.Define("n_jets",  "FCCAnalyses::ReconstructedParticle::get_n(Jet)")
			.Define("px_jets",  "FCCAnalyses::ReconstructedParticle::get_px(Jet)")
			.Define("py_jets",  "FCCAnalyses::ReconstructedParticle::get_py(Jet)")
			.Define("pz_jets",  "FCCAnalyses::ReconstructedParticle::get_pz(Jet)")
			.Define("E_jets",  "FCCAnalyses::ReconstructedParticle::get_e(Jet)")
			.Define("pT_jets",  "FCCAnalyses::ReconstructedParticle::get_pt(Jet)")
			.Define("eta_jets",  "FCCAnalyses::ReconstructedParticle::get_eta(Jet)")

			# #selected jets above a pT threshold of 20 GeV
			.Define("selected_jets", "FCCAnalyses::ReconstructedParticle::sel_pt(20.)(Jet)") 
			.Define("n_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_n(selected_jets)")
			.Define("px_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_px(selected_jets)")
			.Define("py_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_py(selected_jets)")
			.Define("pz_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_pz(selected_jets)")
			.Define("E_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_e(selected_jets)")
			.Define("pT_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_pt(selected_jets)")
			.Define("eta_jets_sel",  "FCCAnalyses::ReconstructedParticle::get_eta(selected_jets)")

			# #b-tagged jets:
			.Alias("Jet3","Jet#3.index") 

			#LOOSE WP
			.Define("b_tagged_jets_loose", "AnalysisFCChh::get_tagged_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 0)") #bit 0 = loose WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_b_jets_loose", "FCCAnalyses::ReconstructedParticle::get_n(b_tagged_jets_loose)")
			.Define("px_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_px(b_tagged_jets_loose)")
			.Define("py_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_py(b_tagged_jets_loose)")
			.Define("pz_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_pz(b_tagged_jets_loose)")
			.Define("E_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_e(b_tagged_jets_loose)")
			.Define("pT_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_pt(b_tagged_jets_loose)")
			.Define("eta_b_jets_loose",  "FCCAnalyses::ReconstructedParticle::get_eta(b_tagged_jets_loose)")

			#MEDIUM WP
			.Define("b_tagged_jets_medium", "AnalysisFCChh::get_tagged_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 1)") #bit 1 = medium WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_b_jets_medium", "FCCAnalyses::ReconstructedParticle::get_n(b_tagged_jets_medium)")
			.Define("px_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_px(b_tagged_jets_medium)")
			.Define("py_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_py(b_tagged_jets_medium)")
			.Define("pz_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_pz(b_tagged_jets_medium)")
			.Define("E_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_e(b_tagged_jets_medium)")
			.Define("pT_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_pt(b_tagged_jets_medium)")
			.Define("eta_b_jets_medium",  "FCCAnalyses::ReconstructedParticle::get_eta(b_tagged_jets_medium)")

			#TIGHT WP
			.Define("b_tagged_jets_tight", "AnalysisFCChh::get_tagged_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 2)") #bit 2 = tight WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_b_jets_tight", "FCCAnalyses::ReconstructedParticle::get_n(b_tagged_jets_tight)")
			.Define("px_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_px(b_tagged_jets_tight)")
			.Define("py_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_py(b_tagged_jets_tight)")
			.Define("pz_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_pz(b_tagged_jets_tight)")
			.Define("E_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_e(b_tagged_jets_tight)")
			.Define("pT_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_pt(b_tagged_jets_tight)")
			.Define("eta_b_jets_tight",  "FCCAnalyses::ReconstructedParticle::get_eta(b_tagged_jets_tight)")



			)

		########################################### OUTPUT BRANCHES FOR NTUPLE  ########################################### 
		# select branches for output file
		branchList = ROOT.vector('string')()

		for branchName in [
			#Jets:
			"n_jets", "px_jets", "py_jets", "pz_jets", "E_jets", "pT_jets", "eta_jets",
			"n_jets_sel", "px_jets_sel", "py_jets_sel", "pz_jets_sel", "E_jets_sel", "pT_jets_sel", "eta_jets_sel",
			"n_b_jets_loose", "px_b_jets_loose", "py_b_jets_loose", "pz_b_jets_loose", "E_b_jets_loose", "pT_b_jets_loose", "eta_b_jets_loose",
			"n_b_jets_medium", "px_b_jets_medium", "py_b_jets_medium", "pz_b_jets_medium", "E_b_jets_medium", "pT_b_jets_medium", "eta_b_jets_medium",
			"n_b_jets_tight", "px_b_jets_tight", "py_b_jets_tight", "pz_b_jets_tight", "E_b_jets_tight", "pT_b_jets_tight", "eta_b_jets_tight",

			]:
			branchList.push_back(branchName)

		#write tree to file	
		df_out.Snapshot("events", self.outname, branchList)



#main function
if __name__ == "__main__":

	default_input_tester = "/eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbWW_tester_new_card.root" #centrally produced bbWW tester
	default_out_dir = "./"

	#parse input arguments:
	parser = argparse.ArgumentParser(description="Stand-alone analysise code for FCC-hh events of type HH(bbWW)")
	parser.add_argument('--input', '-i', metavar="INPUTFILE", dest="input_file", default=default_input_tester, help="Path to the input file. If not specified, runs over a default tester file.")
	parser.add_argument('--output', '-o', metavar="OUTPUTDIR", dest="out_dir", default=default_out_dir, help="Output directory. If not specified, sets to current directory.")
	parser.add_argument('--flavour', metavar="FLAVOUR", dest="flavour", default="DFOS", help="Flavour of the analysis: Select DFOS(=emu) or SFOS(=ee+mm) lepton pair. Default is DFOS.")
	parser.add_argument('--nCPU', metavar="NCPU", dest="ncpus", default=4, help="Number of cores to use")
	parser.add_argument('--debug', action="store_true", dest="do_debug", help="Run only over first 10 events for testing, also print debug ouput.")
	args = parser.parse_args()

	stopwatch = ROOT.TStopwatch()
	stopwatch.Start()

	#create the output dir, if it doesnt exist yet:
	if not os.path.exists(args.out_dir):
		os.mkdir(args.out_dir)

	#build the name/path of the output file:
	output_file = os.path.join(args.out_dir, args.input_file.split("/")[-2]+"_"+args.input_file.split("/")[-1])

	#now run:
	print("##### Running HH signals analysis for FCC-hh Delphes card validation #####")
	print("Input file: ", args.input_file)
	print("Output file: ", output_file)

	# ncpus = 1 #debug
	# ncpus = 4
	analysis = analysis(args.input_file, output_file, args.ncpus, doDebug= args.do_debug)
	analysis.run()

	stopwatch.Stop()
	stopwatch.Print() 