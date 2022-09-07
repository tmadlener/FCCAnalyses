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

			#Tau-jets, here also 3 WP

			#LOOSE WP
			.Define("tau_tagged_jets_loose", "AnalysisFCChh::get_tau_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 0)") #bit 0 = loose WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_n(tau_tagged_jets_loose)")
			.Define("px_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_px(tau_tagged_jets_loose)")
			.Define("py_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_py(tau_tagged_jets_loose)")
			.Define("pz_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_pz(tau_tagged_jets_loose)")
			.Define("E_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_e(tau_tagged_jets_loose)")
			.Define("pT_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_pt(tau_tagged_jets_loose)")
			.Define("eta_tau_jets_loose", "FCCAnalyses::ReconstructedParticle::get_eta(tau_tagged_jets_loose)")

			#MEDIUM WP
			.Define("tau_tagged_jets_medium", "AnalysisFCChh::get_tau_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 1)") #bit 1 = medium WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_n(tau_tagged_jets_medium)")
			.Define("px_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_px(tau_tagged_jets_medium)")
			.Define("py_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_py(tau_tagged_jets_medium)")
			.Define("pz_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_pz(tau_tagged_jets_medium)")
			.Define("E_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_e(tau_tagged_jets_medium)")
			.Define("pT_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_pt(tau_tagged_jets_medium)")
			.Define("eta_tau_jets_medium", "FCCAnalyses::ReconstructedParticle::get_eta(tau_tagged_jets_medium)")


			#TIGHT WP
			.Define("tau_tagged_jets_tight", "AnalysisFCChh::get_tau_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 2)") #bit 2 = tight WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
			.Define("n_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_n(tau_tagged_jets_tight)")
			.Define("px_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_px(tau_tagged_jets_tight)")
			.Define("py_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_py(tau_tagged_jets_tight)")
			.Define("pz_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_pz(tau_tagged_jets_tight)")
			.Define("E_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_e(tau_tagged_jets_tight)")
			.Define("pT_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_pt(tau_tagged_jets_tight)")
			.Define("eta_tau_jets_tight", "FCCAnalyses::ReconstructedParticle::get_eta(tau_tagged_jets_tight)")

			########################################### ELECTRONS ########################################### 
			#all
			.Alias("Electron0", "Electron#0.index")
			.Define("electrons",  "FCCAnalyses::ReconstructedParticle::get(Electron0, ReconstructedParticles)")
			.Define("n_electrons",  "FCCAnalyses::ReconstructedParticle::get_n(electrons)")
			.Define("px_electrons",  "FCCAnalyses::ReconstructedParticle::get_px(electrons)")
			.Define("py_electrons",  "FCCAnalyses::ReconstructedParticle::get_py(electrons)")
			.Define("pz_electrons",  "FCCAnalyses::ReconstructedParticle::get_pz(electrons)")
			.Define("E_electrons",  "FCCAnalyses::ReconstructedParticle::get_e(electrons)")
			.Define("pT_electrons",  "FCCAnalyses::ReconstructedParticle::get_pt(electrons)")
			.Define("eta_electrons",  "FCCAnalyses::ReconstructedParticle::get_eta(electrons)")

			#isolated from b-jets - using the medium WP only for now!!
			.Define("electrons_iso", "AnalysisFCChh::sel_isolated(electrons, b_tagged_jets_medium)")
			.Define("n_electrons_iso", "FCCAnalyses::ReconstructedParticle::get_n(electrons_iso)")
			.Define("px_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_px(electrons_iso)")
			.Define("py_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_py(electrons_iso)")
			.Define("pz_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_pz(electrons_iso)")
			.Define("E_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_e(electrons_iso)")
			.Define("pT_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_pt(electrons_iso)")
			.Define("eta_electrons_iso",  "FCCAnalyses::ReconstructedParticle::get_eta(electrons_iso)")

			#selection: isolated and above the pT threshold
			.Define("selected_electrons", "FCCAnalyses::ReconstructedParticle::sel_pt(10.)(electrons_iso)")
			.Define("n_electrons_sel", "FCCAnalyses::ReconstructedParticle::get_n(selected_electrons)") 
			.Define("px_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_px(selected_electrons)")
			.Define("py_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_py(selected_electrons)")
			.Define("pz_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_pz(selected_electrons)")
			.Define("E_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_e(selected_electrons)")
			.Define("pT_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_pt(selected_electrons)")
			.Define("eta_electrons_sel",  "FCCAnalyses::ReconstructedParticle::get_eta(selected_electrons)")

			########################################### MUONS ########################################### 
			# all
			.Alias("Muon0", "Muon#0.index")
			.Define("muons",  "FCCAnalyses::ReconstructedParticle::get(Muon0, ReconstructedParticles)") 
			.Define("n_muons",  "FCCAnalyses::ReconstructedParticle::get_n(muons)")
			.Define("px_muons",  "FCCAnalyses::ReconstructedParticle::get_px(muons)")
			.Define("py_muons",  "FCCAnalyses::ReconstructedParticle::get_py(muons)")
			.Define("pz_muons",  "FCCAnalyses::ReconstructedParticle::get_pz(muons)")
			.Define("E_muons",  "FCCAnalyses::ReconstructedParticle::get_e(muons)")
			.Define("pT_muons",  "FCCAnalyses::ReconstructedParticle::get_pt(muons)")
			.Define("eta_muons",  "FCCAnalyses::ReconstructedParticle::get_eta(muons)")

			#isolated from b-jets - using the medium WP only for now!!
			.Define("muons_iso", "AnalysisFCChh::sel_isolated(muons, b_tagged_jets_medium)")
			.Define("n_muons_iso", "FCCAnalyses::ReconstructedParticle::get_n(muons_iso)")
			.Define("px_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_px(muons_iso)")
			.Define("py_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_py(muons_iso)")
			.Define("pz_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_pz(muons_iso)")
			.Define("E_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_e(muons_iso)")
			.Define("pT_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_pt(muons_iso)")
			.Define("eta_muons_iso",  "FCCAnalyses::ReconstructedParticle::get_eta(muons_iso)")

			#selection: isolated and above the pT threshold
			.Define("selected_muons", "FCCAnalyses::ReconstructedParticle::sel_pt(10.)(muons_iso)")
			.Define("n_muons_sel", "FCCAnalyses::ReconstructedParticle::get_n(selected_muons)") 
			.Define("px_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_px(selected_muons)")
			.Define("py_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_py(selected_muons)")
			.Define("pz_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_pz(selected_muons)")
			.Define("E_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_e(selected_muons)")
			.Define("pT_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_pt(selected_muons)")
			.Define("eta_muons_sel",  "FCCAnalyses::ReconstructedParticle::get_eta(selected_muons)")


			########################################### PHOTONS ########################################### 
			.Alias("Photon0", "Photon#0.index") 
			.Define("photons",  "FCCAnalyses::ReconstructedParticle::get(Photon0, ReconstructedParticles)") 
			.Define("n_photons",  "FCCAnalyses::ReconstructedParticle::get_n(photons)") 
			.Define("px_photons",  "FCCAnalyses::ReconstructedParticle::get_px(photons)")
			.Define("py_photons",  "FCCAnalyses::ReconstructedParticle::get_py(photons)")
			.Define("pz_photons",  "FCCAnalyses::ReconstructedParticle::get_pz(photons)")
			.Define("E_photons",  "FCCAnalyses::ReconstructedParticle::get_e(photons)")
			.Define("pT_photons",  "FCCAnalyses::ReconstructedParticle::get_pt(photons)")
			.Define("eta_photons",  "FCCAnalyses::ReconstructedParticle::get_eta(photons)")

			#isolated from b-jets - using the medium WP only for now!!
			.Define("photons_iso", "AnalysisFCChh::sel_isolated(photons, b_tagged_jets_medium)")
			.Define("n_photons_iso", "FCCAnalyses::ReconstructedParticle::get_n(photons_iso)")
			.Define("px_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_px(photons_iso)")
			.Define("py_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_py(photons_iso)")
			.Define("pz_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_pz(photons_iso)")
			.Define("E_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_e(photons_iso)")
			.Define("pT_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_pt(photons_iso)")
			.Define("eta_photons_iso",  "FCCAnalyses::ReconstructedParticle::get_eta(photons_iso)")

			#selection: isolated and above the pT threshold
			.Define("selected_photons", "FCCAnalyses::ReconstructedParticle::sel_pt(20.)(photons_iso)")
			.Define("n_photons_sel", "FCCAnalyses::ReconstructedParticle::get_n(selected_photons)") 
			.Define("px_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_px(selected_photons)")
			.Define("py_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_py(selected_photons)")
			.Define("pz_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_pz(selected_photons)")
			.Define("E_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_e(selected_photons)")
			.Define("pT_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_pt(selected_photons)")
			.Define("eta_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_eta(selected_photons)")


			########################################### MET ########################################### 
			.Define("MET", "FCCAnalyses::ReconstructedParticle::get_pt(MissingET)")
			.Define("MET_x", "FCCAnalyses::ReconstructedParticle::get_px(MissingET)")
			.Define("MET_y", "FCCAnalyses::ReconstructedParticle::get_py(MissingET)")
			.Define("MET_phi", "FCCAnalyses::ReconstructedParticle::get_phi(MissingET)")

			########################################### EVENT WIDE KINEMATIC VARIABLES########################################### 

			#H(bb) system - using the medium WP b-jets only for now!! -> if the events has < 2 bjets these variables do not get filled!
			.Define("bb_pairs_unmerged", "AnalysisFCChh::getPairs(b_tagged_jets_medium)") #currently gets only leading pT pair, as a RecoParticlePair
			#get angles between the two bs:
			.Define("dPhi_bb", "AnalysisFCChh::get_angularDist_pair(bb_pairs_unmerged, TString(\"dPhi\")) ")
			.Define("dEta_bb", "AnalysisFCChh::get_angularDist_pair(bb_pairs_unmerged, TString(\"dEta\")) ")
			.Define("dR_bb", "AnalysisFCChh::get_angularDist_pair(bb_pairs_unmerged, TString(\"dR\")) ")

			#then merge the bb pair into one object and get its kinematic properties
			.Define("bb_pairs", "AnalysisFCChh::merge_pairs(bb_pairs_unmerged)") #merge into one object to access inv masses etc
			.Define("m_bb", "FCCAnalyses::ReconstructedParticle::get_mass(bb_pairs)")
			.Define("px_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_px(bb_pairs)")
			.Define("py_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_py(bb_pairs)")
			.Define("pz_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_pz(bb_pairs)")
			.Define("E_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_e(bb_pairs)")
			.Define("pT_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_pt(bb_pairs)")
			.Define("eta_Hbb_cand",  "FCCAnalyses::ReconstructedParticle::get_eta(bb_pairs)")

			#H(yy) if it exists, if there are no 2 selected photons, doesnt get filled 
			.Define("yy_pairs_unmerged", "AnalysisFCChh::getPairs(selected_photons)")
			.Define("yy_pairs", "AnalysisFCChh::merge_pairs(yy_pairs_unmerged)")
			.Define("m_yy", "FCCAnalyses::ReconstructedParticle::get_mass(yy_pairs)")

			#H(tau_h, tau_h) if it exists, if there are no 2 tau-jets, doesnt get filled -> using medium WP only !!
			.Define("tauhtauh_pairs_unmerged", "AnalysisFCChh::getPairs(tau_tagged_jets_medium)")
			.Define("tauhtauh_pairs", "AnalysisFCChh::merge_pairs(tauhtauh_pairs_unmerged)")
			.Define("m_tauhtauh", "FCCAnalyses::ReconstructedParticle::get_mass(tauhtauh_pairs)")

			#H(WW->emu+MET) if it exists,  if there is no emu pair, doesnt get filled 
			.Define("OS_emu_pairs", "AnalysisFCChh::getDFOSPairs(selected_electrons, selected_muons)") #returns all possible pairs sorted by pT of the elecs and muons
			.Define("n_OS_emu_pairs", "AnalysisFCChh::get_n_pairs(OS_emu_pairs)")

			##select the leading pT DFOS pair only
			.Define("OS_emu_pair_lead", "AnalysisFCChh::get_first_pair(OS_emu_pairs)")
			.Define("H_2l_cand", "AnalysisFCChh::merge_pairs(OS_emu_pair_lead)")
			.Define("m_ll", "FCCAnalyses::ReconstructedParticle::get_mass(H_2l_cand)")
			.Define("dPhi_ll_MET", "AnalysisFCChh::get_angularDist_MET(H_2l_cand, MissingET, TString(\"dPhi\")) ")


			)

		########################################### OUTPUT BRANCHES FOR NTUPLE  ########################################### 
		# select branches for output file
		branchList = ROOT.vector('string')()

		for branchName in [
			# Jets:
			"n_jets", "px_jets", "py_jets", "pz_jets", "E_jets", "pT_jets", "eta_jets",
			"n_jets_sel", "px_jets_sel", "py_jets_sel", "pz_jets_sel", "E_jets_sel", "pT_jets_sel", "eta_jets_sel",
			# B-jets:
			"n_b_jets_loose", "px_b_jets_loose", "py_b_jets_loose", "pz_b_jets_loose", "E_b_jets_loose", "pT_b_jets_loose", "eta_b_jets_loose",
			"n_b_jets_medium", "px_b_jets_medium", "py_b_jets_medium", "pz_b_jets_medium", "E_b_jets_medium", "pT_b_jets_medium", "eta_b_jets_medium",
			"n_b_jets_tight", "px_b_jets_tight", "py_b_jets_tight", "pz_b_jets_tight", "E_b_jets_tight", "pT_b_jets_tight", "eta_b_jets_tight",
			# Tau-jets:
			"n_tau_jets_loose", "px_tau_jets_loose", "py_tau_jets_loose", "pz_tau_jets_loose", "E_tau_jets_loose", "pT_tau_jets_loose", "eta_tau_jets_loose",
			"n_tau_jets_medium", "px_tau_jets_medium", "py_tau_jets_medium", "pz_tau_jets_medium", "E_tau_jets_medium", "pT_tau_jets_medium", "eta_tau_jets_medium",
			"n_tau_jets_tight", "px_tau_jets_tight", "py_tau_jets_tight", "pz_tau_jets_tight", "E_tau_jets_tight", "pT_tau_jets_tight", "eta_tau_jets_tight",
			# Electrons:
			"n_electrons", "px_electrons", "py_electrons", "pz_electrons", "E_electrons", "pT_electrons", "eta_electrons",
			"n_electrons_iso", "px_electrons_iso", "py_electrons_iso", "pz_electrons_iso", "E_electrons_iso", "pT_electrons_iso", "eta_electrons_iso",
			"n_electrons_sel", "px_electrons_sel", "py_electrons_sel", "pz_electrons_sel", "E_electrons_sel", "pT_electrons_sel", "eta_electrons_sel",
			# Muons:
			"n_muons", "px_muons", "py_muons", "pz_muons", "E_muons", "pT_muons", "eta_muons",
			"n_muons_iso", "px_muons_iso", "py_muons_iso", "pz_muons_iso", "E_muons_iso", "pT_muons_iso", "eta_muons_iso",
			"n_muons_sel", "px_muons_sel", "py_muons_sel", "pz_muons_sel", "E_muons_sel", "pT_muons_sel", "eta_muons_sel",
			# Photons:
			"n_photons", "px_photons", "py_photons", "pz_photons", "E_photons", "pT_photons", "eta_photons",
			"n_photons_iso", "px_photons_iso", "py_photons_iso", "pz_photons_iso", "E_photons_iso", "pT_photons_iso", "eta_photons_iso",
			"n_photons_sel", "px_photons_sel", "py_photons_sel", "pz_photons_sel", "E_photons_sel", "pT_photons_sel", "eta_photons_sel",
			# ETMiss:
			"MET","MET_x", "MET_y", "MET_phi",
			# Hbb decay:
			"m_bb", "px_Hbb_cand", "py_Hbb_cand", "pz_Hbb_cand", "E_Hbb_cand", "pT_Hbb_cand", "eta_Hbb_cand", "dPhi_bb", "dEta_bb", "dR_bb",
			#Hyy decay:
			"m_yy",
			#H(tau_h, tau_h) decay:
			"m_tauhtauh",
			#H(WW->emu+MET) decay:
			"n_OS_emu_pairs", "m_ll", "dPhi_ll_MET",

			]:
			branchList.push_back(branchName)

		#write tree to file	
		df_out.Snapshot("events", self.outname, branchList)



#main function
if __name__ == "__main__":

	default_input_tester = "/eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbWW_tester_new_card.root" #locally produced bbWW tester
	# default_input_tester = "/eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbtautau_tester_new_card.root" #locally produced bbtautau tester
	# default_input_tester = "/eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbyy_tester_new_card.root" #locally produced bbyy tester
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