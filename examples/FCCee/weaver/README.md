# Quick tour

## stage 1: produce event based tree
```
fccanalysis run stage1.py --output stage1_cc.root --files-list /eos/experiment/fcc/ee/generation/DelphesEvents/pre_fall2022_training/IDEA/p8_ee_ZH_Znunu_Hcc_ecm240/events_663529174.root
```

## stage 2: produce jet based tree
```
g++ -o stage2 stage2.cpp `root-config --cflags --libs` -Wall
./stage2 stage1_cc.root stage2_cc.root 0 10000
```
## run all stages in one go:
```
python all_stages.py --fsplit 0.9
```

## run inference
```
fccanalysis run analysis_inference.py --output inference.root --files-list /eos/experiment/fcc/ee/generation/DelphesEvents/pre_fall2022_training/IDEA/p8_ee_ZH_Znunu_Hcc_ecm240/events_663529174.root
```



# Preparation of dataset
## Generated samples
The samples used are stored in the directory `/eos/experiment/fcc/ee/generation/DelphesEvents/pre_fall2022_training/IDEA/` .
The events were simulated using Delphes. 
The processes considered are $e^+ e^- \to Z(\to \nu \nu) H(\to aa)$ with $j = u,d,b,c,s,g$.
For the processes $j =  u,d,b,c,s$ samples of $\sim 10^6$ events were produced (i.e. $2 \times 10^6$ jets per sample), for $j = g$ $\sim 2 \times 10^6$ (i.e. $2 \times 10^6$ jets per sample).
Beamspot of 20 um size on Y-axis and 600 um on Z-axis was set.
Tre tree containing the events in the input .root file is called _events_ .


## Description and usage
During the training we want ParticleNet to learn to identify a jet from its properties. This means that each entry of the training dataset should contain the properties of one jet (and of its constituents) which are significant for the discrimination only; furthermore, these properties should be organized in a format accessible to ParticleNet: *arrays*.

However, the samples generated
* have a per-event structure, 
* each event contains way more information than the needed one for the training,
* each event is saved in edm4hep format 

So, before performing the training three actions are required:
1. read the generated samples in edm4hep format(through fccanalysis),
2. extract/compute the features of interest (through fccanalysis),
3. produce the ntuples (one per class) containing the interesting features.

In our case, the first two actions are executed by `stage1.py` and the third by `stage2.cpp` .
Since we are interested in the final ntuple, these two codes are executed jointly by `all_stages.py`, optimizing the times through the usage of multiprocessing.
So the production of the training dataset from the generated samples is performed in two steps, which in the folloing will be referred to as _stage1_ and _stage2_.
Even though the joint action of the two steps, an intermediate file is produced by _stage1_, which will be saved in the ouptut directory, together with the final ntuples but with a recognizable name.

For what concerns time: 
* _stage1_ takes $\sim 3-4$ minutes per $10^6$ events (run on 8 cpus);
* _stage2_ takes $\sim 5$ minutes per $10^6$ events (run on 1 cpu).

For what concerns memory usage:
* intermediate files weight $\sim 4$ Gb per $10^6$ events; 
* final files weight $\sim 4$ Gb per $10^6$ events;

In our case the directory containing the intermediate and final files weights $\sim 50$ Gb .
We notice that the intermediate files could be deleted after the production of the final ntuples.

### Stage1 : `stage1.py`
All the namespaces used are defined and developed inside the folder `analyzers`.
In particular, in JetConstituentsUtils we developed functions to compute the constituents features and modified ReconstructedParticle2Track in order to return the value $-9$ for particles (neutral) nothaving a track (the value was chosen arbitrarily, could be changed).

As said, in this stage basically the initial edm4hep files are read and the interesting features are computed. Furthermore, in our version, the clustering is done explicitly. 

In the initial tree each entry corresponds to an event.

Let's go through the code.
1. explicit clustering. The clustering is done explicitly by the following lines:
```
            #===== CLUSTERING

            #define the RP px, py, pz and e
            .Define("RP_px",          "ReconstructedParticle::get_px(ReconstructedParticles)")
            .Define("RP_py",          "ReconstructedParticle::get_py(ReconstructedParticles)")
            .Define("RP_pz",          "ReconstructedParticle::get_pz(ReconstructedParticles)")
            .Define("RP_e",           "ReconstructedParticle::get_e(ReconstructedParticles)")
            
            #build pseudo jets with the RP
            .Define("pseudo_jets",    "JetClusteringUtils::set_pseudoJets(RP_px, RP_py, RP_pz, RP_e)")
            #run jet clustering with all reconstructed particles. ee_genkt_algorithm, R=1.5, inclusive clustering, E-scheme
            .Define("FCCAnalysesJets_ee_genkt", "JetClustering::clustering_ee_genkt(1.5, 0, 0, 0, 0, -1)(pseudo_jets)")
            #get the jets out of the struct
            .Define("jets_ee_genkt",           "JetClusteringUtils::get_pseudoJets(FCCAnalysesJets_ee_genkt)")
            #get the jets constituents out of the struct
            .Define("jetconstituents_ee_genkt","JetClusteringUtils::get_constituents(FCCAnalysesJets_ee_genkt)")
```
In the initial tree, all the particles measured in one event are saved in one entry of the branch _ReconstructedParticles_ in a                                                                 `ROOT::VecOps::RVec<ReconstructedParticleData>`.
The line `.Define("RP_px",          "ReconstructedParticle::get_px(ReconstructedParticles)")` takes this branch and for all entries computes px of each particle; the output of this call is a branch called _RP_px_ containing an `RVec<float>` per each event.

The jet clustering is performed using the 4-momenta of the reconstructed particles. This operation returns two outputs: 
  - `jets_ee_genkt` : `RVec< fastjet::Pseudojet >` ; `Pseudojet` methods and attributes allow to access the overall jet properties (implemented in `JetClusteringUtils.cc`).
  - `jetconstituents_ee_genkt` : `RVec< RVec < int > >` , which is a vector of vectors of integer labels which were assigned to the particles during the clustering by the function `JetClusteringUtils::set_pseudoJets` : 
```
std::vector<fastjet::PseudoJet> JetClusteringUtils::set_pseudoJets(ROOT::VecOps::RVec<float> px, 
					       ROOT::VecOps::RVec<float> py, 
					       ROOT::VecOps::RVec<float> pz, 
					       ROOT::VecOps::RVec<float> e) {
  std::vector<fastjet::PseudoJet> result;
  unsigned index = 0;
  for (size_t i = 0; i < px.size(); ++i) {
    result.emplace_back(px.at(i), py.at(i), pz.at(i), e.at(i));
    result.back().set_user_index(index);
    ++index;
  }
  return result;
}
``` 
and that are now used to associate the particles to the belonging jet. In fact, by calling
```
.Define("JetsConstituents", "JetConstituentsUtils::build_constituents_cluster(ReconstructedParticles, jetconstituents_ee_genkt)") #build jet constituents
```
we create an RVec::< RVec < ReconstructedParticleData > > in which the first index _i_ runs over the jets identified in the event and the second _j_ over the particles belonging to the _i_-th jet :
```
rv::RVec<FCCAnalysesJetConstituents> build_constituents_cluster(const rv::RVec<edm4hep::ReconstructedParticleData>& rps,
								    const std::vector<std::vector<int>>& indices) {
      //std::cout << "... Building jets-constituents after clustering " << std::endl;
      rv::RVec<FCCAnalysesJetConstituents> jcs;
      for (const auto& jet_index : indices) {
	FCCAnalysesJetConstituents jc;
	for(const auto& const_index : jet_index) {
	  jc.push_back(rps.at(const_index));
	}
	jcs.push_back(jc);
      }
      return jcs;
    }	
```
IMPORTANT: this function associates properly jet-constituents because the labels are set to the particles following the order of the particles as they present in the initial entry; so the index coincides with the particle position in the input branch.

The goodness of the association particles-jets was proven by plotting the histograms of the residuals $P^\mu_{jet}-\sum{P^{\mu}_{constituents}})/P^\mu_{jet}$ with $P^\mu = (E, \vec{p})$. Actually the residuals were computed on other 4 quantities which are in one to one relation with 4-momenta: $E, p_t, \phi, \theta$.
	
	PLOTS!!!

The structure RVec::< RVec < type > > is the key data structure for treating jets constituents in an event and will be mantained also when computing the features of the constituents for this stage.
Let's see an example of a function computing a feature of the constituents of the jets in one event (the same structure is mantained for other functions):
```
rv::RVec<FCCAnalysesJetConstituentsData> get_erel_log_cluster(const rv::RVec<fastjet::PseudoJet>& jets,
								  const rv::RVec<FCCAnalysesJetConstituents>& jcs) {
      rv::RVec<FCCAnalysesJetConstituentsData> out;
      for (size_t i = 0; i < jets.size(); ++i) {   // external loop: first index (i) running over the jets in the event
        auto& jet_csts = out.emplace_back();      //in the ext. loop we fill the external vector with vectors 
						  //(one per jet)
        float e_jet = jets.at(i).E();
        auto csts = get_jet_constituents(jcs, i);
        for (const auto& jc : csts) {                         //second index (j) running over the constituents
							      //of the i-th jet     
          float val = (e_jet > 0.) ? jc.energy / e_jet : 1.;
          float erel_log = float(std::log10(val));
          jet_csts.emplace_back(erel_log);                    //in the int. loop we fill the internal vectors (jets)
	  						      //with constituents data
        }
      }
      return out;
    }
```
Notice: the functions are written considering one event, then the processing of all the events in the tree is performed by `fccanalysis run` command (see `produceTrainingTrees_mp.py`).

The computed features were also validated by comparing them with the ones obtained in a previous study by Michele Selvaggi and Loukas Gouskos (samples spring2021). We obtained the following plots.
	
	PLOTS!!!

At the end of this stage we have a tree in which each entry is an event; the features of the jets are saved as `RVec < float > ` and the features of the constituents of the jets are saved as `RVec < RVec < float > >` ; the return type is actually a pointer to these structures. We still need to rearrange the structure from a per-event tree of pointers to RVec to a per-jet tree of arrays (an ntuple).

#### What if implicit clustering ?
	...

### Stage_ntuple : `stage2.cpp`
The main goal of this stage is to rearrange the tree obtained in _stage1_ to a per-jet format, but other tasks are accomplished:
* setting the flags of the class which the jets belong to;
* checking the number of events actually considered is the wanted one;
* there is a $\sim 30\%$ cases in which the clustering returns more than 2 jets, and $\sim$ few per million cases in which less than 2 jets are returned; so in the first case just the two higher energy jets are considered, while in the second case no jet is considered; a count of this events is printed to stdout.
	
`stage2.cpp` takes 4 arguments: `USAGE: ./stage [root_inFileName] [root_outFileName] N_i N_f`
1. [root_inFileName] : path to input file in the form `path_to_stage1file/stage1_infilename` , 
2. [root_outFileName] : path to output file in the form `path_to_outputdir/outfilename` ,
3. N_i : index of the event from where start reading the tree ,
4. N_f : index of the event ehrtr to stop reading the tree .

Our choices are implemented in the app `all_stages.py`, so will be explained in the next section.
Now, let's go through the code.

###### Loop structure

The general structure of the loop is:
```
loop : events {
	...
	loop : jets {
		...
		loop : constituents {
			...
		}
	ntuple.Fill()  
	}
}
```

The position of ntuple.Fill() inside the loop structure determines the per-jet structure.
We modified this basic structure:
* we insert where to start looping by `N_i` and how many events to consider by `Nevents_Max = N_f - N_i`;
* we loop over the events from N_i to the end of the tree, but introduce an external counter `saved_events_counts` which grows only if the event has been saved; this is the reliable counter! In fact, if the event is "strange" we don't save any jet, we skip to the next event. The loop breaks when `saved_events_counts == Nevents_Max` if the loop has not reached the end of the input file. Let's study different cases:
    - If `nentries - N_i < Nevents_Max` $\implies$ in the file there are less events than required; no error produced, but in stdout will be printed the actual number of saved events; 
    - If `nentries - N_i > Nevents_Max` $\implies$ in the file there are exactly $Nevents_{Max}$; 
    - If `nentries - N_i = Nevents_Max` but there are strange events I will have less saved events, no error, but I know from stdout.

Are considered "strange" events those in which the clustering returns `njets < 2`; in that case we skip the loop. If `njets >= 2` the loop is performed on the first two jets only (the ones having more ENERGY, they're ordered in stage1; the successives not expected: leak in clustering).

```
int N_i = atoi(argv[3]); //where to start reading the tree
int N_f = atoi(argv[4]); 
int Nevents_Max = N_f - N_i;  //number of events to be saved
int saved_events_counts = 0; //counts the number of events actually saved
int nentries = ev->GetEntries(); //total number of events in the tree

for(int i = N_i+1; i < nentries; ++i) { // Loop over the events 
	ev->GetEntry(i);
    	njet = nJets;
    
    	if (njet < 2) {   //exclude the events with less than two jets
      		continue ;
    	}
		     
	for(int j=0; j < 2; ++j) {   //Loop over the jets inside the i-th event
	//we only take the first two jets (the ones having more ENERGY, they're ordered in stage1),
	//the third not expected: leak in clustering
	
		...
	
		nconst = (count_Const->at(j));
	
		for(int k = 0; k < nconst; ++k){ //Loop over the constituents of the j-th jet in the i-th event
			...
		}
		ntuple->Fill();	
	}
	saved_events_counts += 1; //we count the num of events saved
	if (saved_events_counts == Nevents_Max) { //interrupt the loop if Nevents_max events have already been saved
		break;
	}
}
```
	
	
###### Translating
As an example, we take one jet feature (stored as `RVec < float> *` in the per-event tree) and one constituent feature (`RVec < RVec < float> > *`) and follow them through the code.
 
```
//Setting variables for reading
int nJets; //number of jets in the event
int nconst = 0; //number of constituents of the jets
ROOT::VecOps::RVec<float> *Jets_e=0;
ROOT::VecOps::RVec<ROOT::VecOps::RVec<float> > *JetsConstituents_e = 0;

ev->SetBranchAddress("njet", &nJets);
ev->SetBranchAddress("nconst", &count_Const);
ev->SetBranchAddress("Jets_e", &Jets_e);
ev->SetBranchAddress("JetsConstituents_e", &JetsConstituents_e);	

int njet = 0;
int nconst = 0;	
double recojet_e; 
float pfcand_e[1000] = {0.}; //here we initialize wit a large size

ntuple->Branch("nconst", &nconst, "nconst/I");
ntuple->Branch("recojet_e", &recojet_e);
ntuple->Branch("pfcand_e", pfcand_e, "pfcand_e[nconst]/F");

loop : i over events
	loop : j over jets
	
		recojet_e = (*Jets_e)[j];       //Pointer usage
		nconst = (count_Const->at(j));
	
		loop : k over constituents
			pfcand_e[k] = (JetsConstituents_e->at(j))[k]; //k-th element of the j-th vector 
								      //pointed by JetsConstituents_e
```

In the ntuple, a jet overall property will be just a `double`.
Since we don't know a priori how many constituents the jet has, we initialize a constituent feature as an array with a number of elements larger than possible number of constituents, to be sure that we succeed in reading all of them from the input file (this array works as a temporary "home" before being sent to the output file); we initialize all the values to 0. When we create the branch of the ntuple (`ntuple->Branch("pfcand_e", pfcand_e, "pfcand_e[nconst]/F")`), we pass as the size of the entry the actual number of constituents `nconst` which has already been read into another branch (during the loop), in order not to save all the padding zeros contained in the local array.  
	
###### Setting the flags
We read the name and take the character in the name corresponding to the class (in this case last letter before .root); we fix the flag in the beginning and never change it anymore; since it is pointing to the ntuple branch, everytime I call `ntuple.Fill()` the same value will be added to the branch.
```
std::string infileName(argv[1]);
char flavour = infileName[infileName.length()-6];

float is_q = 0.;
float is_b = 0.;
float is_c = 0.;
float is_s = 0.;
float is_g = 0.;

if (flavour == 'q') {is_q = 1.;}
if (flavour == 'b') {is_b = 1.;}
if (flavour == 'c') {is_c = 1.;}
if (flavour == 's') {is_s = 1.;}
if (flavour == 'g') {is_g = 1.;}
if (flavour == 't') {is_t = 1.;}

ntuple->Branch("pfcand_isMu", pfcand_isMu, "pfcand_isMu[nconst]/F");
ntuple->Branch("pfcand_isEl", pfcand_isEl, "pfcand_isEl[nconst]/F");
ntuple->Branch("pfcand_isChargedHad", pfcand_isChargedHad, "pfcand_isChargedHad[nconst]/F");
ntuple->Branch("pfcand_isGamma", pfcand_isGamma, "pfcand_isGamma[nconst]/F");
ntuple->Branch("pfcand_isNeutralHad", pfcand_isNeutralHad, "pfcand_isNeutralHad[nconst]/F");

loop : events {
	...
	loop: jets {
		...
		loop: constituents {
			...
		}
	ntuple.Fill() 
	}
}	
```

