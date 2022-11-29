processList = {
    # 'p8_ee_ZZ_ecm240':{},#Run over the full statistics from stage1 input file <inputDir>/p8_ee_ZZ_ecm240.root. Keep the same output name as input
    # 'p8_ee_WW_ecm240':{}, #Run over the statistics from stage1 input files <inputDir>/p8_ee_WW_ecm240_out/*.root. Keep the same output name as input
    # 'p8_ee_ZH_ecm240_out':{'output':'MySample_p8_ee_ZH_ecm240'} #Run over the full statistics from stage1 input file <inputDir>/p8_ee_ZH_ecm240_out.root. Change the output name to MySample_p8_ee_ZH_ecm240
    # "pwp8_pp_hh_5f_hhbbyy_tester_new_card":{},
    "noIso_tutorial_output":{},
}

#Mandatory: input directory when not running over centrally produced edm4hep events. 
#It can still be edm4hep files produced standalone or files from a first analysis step (this is the case in this example it runs over the files produced from analysis.py)
# inputDir    = "/eos/user/b/bistapf/FCChh_EvtGen/"
inputDir    = "/afs/cern.ch/user/b/bistapf/TeraScale_FC_tutorial/tutorials/FCChhTutorials/FastSimulation/"



#Optional: output directory, default is local dir
outputDir   = "./outputs/"

#Optional: ncpus, default is 4
nCPUS       = 2

#Optional running on HTCondor, default is False
runBatch    = False

#Mandatory: RDFanalysis class where the use defines the operations on the TTree
class RDFanalysis():

    #__________________________________________________________
    #Mandatory: analysers funtion to define the analysers to process, please make sure you return the last dataframe, in this example it is df2
    def analysers(df):
        import ROOT

        df2 = (df
              .Define("n_jets",  "FCCAnalyses::ReconstructedParticle::get_n(Jet)")
              .Define("px_jets",  "FCCAnalyses::ReconstructedParticle::get_px(Jet)")
              .Define("py_jets",  "FCCAnalyses::ReconstructedParticle::get_py(Jet)")
              .Define("pz_jets",  "FCCAnalyses::ReconstructedParticle::get_pz(Jet)")
              .Define("E_jets",  "FCCAnalyses::ReconstructedParticle::get_e(Jet)")
              .Define("pT_jets",  "FCCAnalyses::ReconstructedParticle::get_pt(Jet)")
              .Define("eta_jets",  "FCCAnalyses::ReconstructedParticle::get_eta(Jet)")

              # .Alias("Jet3","Jet#3.index") 
              # .Define("b_tagged_jets", "AnalysisFCChh::get_tagged_jets(Jet, Jet3, ParticleIDs, ParticleIDs_0, 0)") #bit 0 = loose WP, see: https://github.com/delphes/delphes/blob/master/cards/FCC/scenarios/FCChh_I.tcl
              # .Define("n_b_jets", "FCCAnalyses::ReconstructedParticle::get_n(b_tagged_jets)")
              # .Define("pT_b_jets",  "FCCAnalyses::ReconstructedParticle::get_pt(b_tagged_jets)")
              # .Define("eta_b_jets",  "FCCAnalyses::ReconstructedParticle::get_eta(b_tagged_jets)")

              #selected b-jets:





              #dont understand checking bin by bin -> efficiencies seem very low, missing the isolation on MC side? could run tester without isolation modules 
              #determine photon efficiency: 
              # .Define("MC_p",      "FCCAnalyses::MCParticle::get_p(Particle)")
              .Define("MC_photons", ROOT.MCParticle.sel_pdgID(22, 0),["Particle"])
              # .Define("MC_photons", ROOT.MCParticle.sel_genStatus(22, 0),["MC_photons"])
              # .Define("n_MC_photons",  "FCCAnalyses::MCParticle::get_n(MC_photons)")
              .Define("stable_MC_photons", ROOT.MCParticle.sel_genStatus(1),["MC_photons"])
              # .Define("stable_MC_photons",  "FCCAnalyses::MCParticle::sel_genStatus(MC_photons)")
              # .Define("status_MC_photons",  "FCCAnalyses::MCParticle::get_genStatus(MC_photons)")

              .Define("sel_MC_photons_pT", "FCCAnalyses::MCParticle::sel_pt(1.)(stable_MC_photons)") 

              .Define("n_MC_photons",  "FCCAnalyses::MCParticle::get_n(sel_MC_photons_pT)")
              .Define("pT_MC_photons",  "FCCAnalyses::MCParticle::get_pt(sel_MC_photons_pT)")
              .Define("eta_MC_photons",  "FCCAnalyses::MCParticle::get_eta(sel_MC_photons_pT)")

              #get MC photons: 

              #reco photons:
              .Alias("Photon0", "Photon#0.index") 
              .Define("photons",  "FCCAnalyses::ReconstructedParticle::get(Photon0, ReconstructedParticles)") 
              .Define("n_photons",  "FCCAnalyses::ReconstructedParticle::get_n(photons)")
              .Define("pT_photons",  "FCCAnalyses::ReconstructedParticle::get_pt(photons)")
              .Define("eta_photons",  "FCCAnalyses::ReconstructedParticle::get_eta(photons)")

              #link back to MC -> seg faults
              # .Alias("MCRecoAssociations0", "MCRecoAssociations#0.index")
              # .Alias("MCRecoAssociations1", "MCRecoAssociations#1.index")
              # .Define("reco_matched_MC_photons_indices",  "FCCAnalyses::ReconstructedParticle2MC::getRP2MC_index(MCRecoAssociations0,MCRecoAssociations1,ReconstructedParticles)")
              # .Define("reco_matched_MC_photons_status",  "FCCAnalyses::MCParticle::get_genStatus(Particle[reco_matched_MC_photons_indices])")
              
              # .Define("selected_photons", "FCCAnalyses::ReconstructedParticle::sel_pt(20.)(photons_iso)")
              # .Define("pT_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_pt(selected_photons)")
              # .Define("eta_photons_sel",  "FCCAnalyses::ReconstructedParticle::get_eta(selected_photons)")

              #get MC photons from Higgses -> how?
              # .Define("MC_Higgs", ROOT.MCParticle.sel_pdgID(25, 0),["Particle"])
              # .Define("MC_Higgs_indices",  "FCCAnalyses::MCParticle::get_n(sel_MC_photons_pT)")

              #try muons
              .Alias("Muon0", "Muon#0.index")
              .Define("muons",  "FCCAnalyses::ReconstructedParticle::get(Muon0, ReconstructedParticles)") 
              .Define("n_muons",  "FCCAnalyses::ReconstructedParticle::get_n(muons)")


              .Define("MC_muons", ROOT.MCParticle.sel_pdgID(13, 1),["Particle"])
              .Define("stable_MC_muons", ROOT.MCParticle.sel_genStatus(1),["MC_muons"])
              .Define("sel_MC_muons_pT", "FCCAnalyses::MCParticle::sel_pt(1.)(stable_MC_muons)") 
              .Define("n_MC_muons",  "FCCAnalyses::MCParticle::get_n(sel_MC_muons_pT)")
              .Define("pT_MC_muons",  "FCCAnalyses::MCParticle::get_pt(sel_MC_muons_pT)")
              .Define("eta_MC_muons",  "FCCAnalyses::MCParticle::get_eta(sel_MC_muons_pT)")


               )
        return df2

    #__________________________________________________________
    #Mandatory: output function, please make sure you return the branchlist as a python list.
    def output():
        branchList = [
            "n_jets", "px_jets", "py_jets", "pz_jets", "E_jets", "pT_jets", "eta_jets",
            "n_MC_photons", "n_photons", "pT_MC_photons", "eta_MC_photons", "pT_photons", "eta_photons",
            "n_muons", "n_MC_muons", "pT_MC_muons", "eta_MC_muons",
        ]
        return branchList




