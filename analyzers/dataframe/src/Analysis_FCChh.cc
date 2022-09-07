#include "FCCAnalyses/Analysis_FCChh.h"
#include "FCCAnalyses/lester_mt2_bisect.h"


#include <iostream>

using namespace AnalysisFCChh;

//truth filter helper functions:
bool AnalysisFCChh::isLep(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 11 || abs(pdg_id) == 13 || abs(pdg_id) == 15){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isLightLep(edm4hep::MCParticleData truth_part) //only electrons or muons, no taus
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 11 || abs(pdg_id) == 13 ){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isNeutrino(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 12 || abs(pdg_id) == 14 || abs(pdg_id) == 16){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isQuark(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) >= 1 && abs(pdg_id) <= 6){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isZ(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 23){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isW(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 24){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isTau(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 15){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isH(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 25){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isb(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) == 5){
		return true;
	}
	else{
		return false;
	}
}

bool AnalysisFCChh::isHadron(edm4hep::MCParticleData truth_part)
{
	auto pdg_id = truth_part.PDG;
	//std::cout << "pdg id of truth part is" << pdg_id << std::endl;
	if (abs(pdg_id) >= 100){
		return true;
	}
	else{
		return false;
	}
}


//check if a truth particle came from a hadron decay, needed to veto taus that come from b-meson decays in the bbtautau samples
bool AnalysisFCChh::isFromHadron(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{

	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		// std::cout << "Found parent of the tau as:" << parent.PDG << std::endl;
		if (abs(parent.PDG) >= 100){
			return true;
		}
	}
	return false;
}

//check if a truth particle had a Higgs as a parent somewhere up the chain
bool AnalysisFCChh::hasHiggsParent(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{

	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		// std::cout << "Found parent of the tau as:" << parent.PDG << std::endl;
		if (isH(parent)){
			return true;
		}
		return hasHiggsParent(parent, parent_ids, truth_particles);
	}

	return false;
}

//check if the immediate parent of a particle is a Higgs
bool AnalysisFCChh::isFromHiggsDirect(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{

	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		// std::cout << "Found parent of the tau as:" << parent.PDG << std::endl;
		if (isH(parent)){
			return true;
		}
	}

	return false;
}

//check if a particle came from a tau that itself came from a Higgs, and not ever from a hadron
bool AnalysisFCChh::isChildOfTauFromHiggs(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{
	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		if (isTau(parent)){
			//veto taus from b-decays
			if (isFromHadron(parent, parent_ids, truth_particles)){
				return false;
			}
			if (hasHiggsParent(parent, parent_ids, truth_particles)){
				return true;
			}
		}
	}
	return false;
}

//check if a particle came from a Z that itself came from a Higgs, and not ever from a hadron
bool AnalysisFCChh::isChildOfZFromHiggs(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{
	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		if (isZ(parent)){
			//veto taus from b-decays
			if (isFromHadron(parent, parent_ids, truth_particles)){
				return false;
			}
			if (hasHiggsParent(parent, parent_ids, truth_particles)){
				return true;
			}
		}
	}
	return false;
}

//check if a particle came from a W that itself came from a Higgs, and not ever from a hadron
bool AnalysisFCChh::isChildOfWFromHiggs(edm4hep::MCParticleData truth_part, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles)
{
	//cannot use the podio getParents fct, need to implement manually:
	auto first_parent_index = truth_part.parents_begin;
	auto last_parent_index = truth_part.parents_end;

	//loop over all parents (usually onle 1, but sometimes more for reasons not understood?):
	for(int parent_i = first_parent_index; parent_i < last_parent_index; parent_i++){
		//first get the index from the parent
		auto parent_MC_index = parent_ids.at(parent_i).index;

		//then go back to the original vector of MCParticles
		auto parent = truth_particles.at(parent_MC_index);

		if (isW(parent)){
			//veto Ws from b-decays
			if (isFromHadron(parent, parent_ids, truth_particles)){
				return false;
			}
			if (hasHiggsParent(parent, parent_ids, truth_particles)){
				return true;
			}
		}
	}
	return false;
}

//check what type the Z decay is: to ll or vv
int AnalysisFCChh::checkZDecay(edm4hep::MCParticleData truth_Z, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles){

	auto first_child_index = truth_Z.daughters_begin;
	auto last_child_index = truth_Z.daughters_end;

	if(last_child_index - first_child_index != 2){
		std::cout << "Error in checkZDecay! Found more or fewer than exactly 2 daughters of a Z boson - this is not expected by code. Need to implement a solution still!"<< std::endl;
		return 0;
	}

	//now get the indices in the daughters vector
	auto child_1_MC_index = daughter_ids.at(first_child_index).index;
	auto child_2_MC_index = daughter_ids.at(last_child_index-1).index;

	// std::cout << "Daughters run from: " << child_1_MC_index << " to " << child_2_MC_index << std::endl;

	//then go back to the original vector of MCParticles
	auto child_1 = truth_particles.at(child_1_MC_index);
	auto child_2 = truth_particles.at(child_2_MC_index);

	if( isLep(child_1) &&  isLep(child_2) ){
		return 1;
	}
	else if ( isNeutrino(child_1) &&  isNeutrino(child_2) ){
		return 2;
	}
	else{
		std::cout << "Found different decay of Z boson than 2 leptons (e or mu), neutrinos or taus! Please check." << std::endl;
		return 0;
	}

}

//check what type the W decay is: to lv or qq
int AnalysisFCChh::checkWDecay(edm4hep::MCParticleData truth_W, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids, ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles){

	auto first_child_index = truth_W.daughters_begin;
	auto last_child_index = truth_W.daughters_end;

	if(last_child_index - first_child_index != 2){
		std::cout << "Error in checkWDecay! Found more or fewer than exactly 2 daughters of a W boson - this is not expected by code. Need to implement a solution still!"<< std::endl;
		return 0;
	}

	//now get the indices in the daughters vector
	auto child_1_MC_index = daughter_ids.at(first_child_index).index;
	auto child_2_MC_index = daughter_ids.at(last_child_index-1).index;

	// std::cout << "Daughters run from: " << child_1_MC_index << " to " << child_2_MC_index << std::endl;

	//then go back to the original vector of MCParticles
	auto child_1 = truth_particles.at(child_1_MC_index);
	auto child_2 = truth_particles.at(child_2_MC_index);

	if( isLep(child_1) &&  isNeutrino(child_2) ){
		return 1;
	}
	else if ( isNeutrino(child_1) &&  isLep(child_2) ){
		return 1;
	}
	else if (isQuark(child_1) && isQuark(child_2)){
		return 2;
	}

	else{
		std::cout << "Found different decay of W boson than lv or qq! Please check." << std::endl;
		std::cout << "PDGID 1 = " << child_1.PDG << " and PDGID 2 = " <<  child_2.PDG << std::endl;
		return 0;
	}

}

//truth filter used to get ZZ(llvv) events from the ZZ(llvv+4l+4v) inclusive signal samples
bool AnalysisFCChh::ZZllvvFilter(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids) 
{	
	// first scan through the truth particles to find Z bosons
	std::vector<edm4hep::MCParticleData> z_list;
	for (auto & truth_part: truth_particles) {
		if (isZ(truth_part)){
			z_list.push_back(truth_part);
		}
		//Tau veto:
		if  (isTau(truth_part)){
			return false;
		}
	}

	//check how many Zs are in event and build the flag:
	// std::cout << "Number of Zs" << z_list.size() << std::endl;
	if (z_list.size() == 2){
		int z1_decay = checkZDecay(z_list.at(0), daughter_ids, truth_particles);
		int z2_decay = checkZDecay(z_list.at(1), daughter_ids, truth_particles);

		int zz_decay_flag = z1_decay + z2_decay;

		//flags are Z(ll) =1 and Z(vv) =2, so flag for llvv is =3 (4l=2, 4v=4)
		if (zz_decay_flag == 3){
			return true; 
		}
		else{
			return false;
		}

	}
	else{
		return false;
	} 
}

//truth filter used to get WW(lvlv) events from the inclusive bbWW samples
bool AnalysisFCChh::WWlvlvFilter(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids, ROOT::VecOps::RVec<podio::ObjectID> parent_ids) 
{	
	// first scan through the truth particles to find Z bosons
	std::vector<edm4hep::MCParticleData> w_list;
	for (auto & truth_part: truth_particles) {
		if (isW(truth_part) && isFromHiggsDirect(truth_part, parent_ids, truth_particles)){
			w_list.push_back(truth_part);
		}
		//Tau veto: - actually probably doesnt work as intended, to revise!!
		// if  (isTau(truth_part)){
			// return false;
		// }
	}

	if (w_list.size() == 2){
		int w1_decay = checkWDecay(w_list.at(0), daughter_ids, truth_particles);
		int w2_decay = checkWDecay(w_list.at(1), daughter_ids, truth_particles);

		int ww_decay_flag = w1_decay + w2_decay;

		//flags are W(lv) =1 and W(qq) =2, so flag for lvlvv is =2 (lvqq=3, 4q=4)
		if (ww_decay_flag == 2){
			return true; 
		}
		else{
			return false;
		}

	}
	else{
		return false;
	} 
}

//find a Z->ll decay on truth level
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getTruthZll(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids)
{	
	//find the Zs
	ROOT::VecOps::RVec<edm4hep::MCParticleData>  z_ll_list;

	for (auto & truth_part: truth_particles) {

		if (isZ(truth_part)){
			if(checkZDecay(truth_part, daughter_ids, truth_particles) == 1){ // check if is Zll decay
				z_ll_list.push_back(truth_part);
			}

		}

	}

	return z_ll_list;
}

//helper functions for reco particles:
TLorentzVector AnalysisFCChh::getTLV_reco(edm4hep::ReconstructedParticleData reco_part){
	TLorentzVector tlv;
	tlv.SetXYZM(reco_part.momentum.x, reco_part.momentum.y, reco_part.momentum.z, reco_part.mass);
	return tlv;
}

//build a MET four momentum:
TLorentzVector AnalysisFCChh::getTLV_MET(edm4hep::ReconstructedParticleData met_object){
	TLorentzVector tlv;
	float met_pt = sqrt(met_object.momentum.x * met_object.momentum.x + met_object.momentum.y * met_object.momentum.y);
	tlv.SetPxPyPzE(met_object.momentum.x, met_object.momentum.y, 0., met_pt);

	//debug:
	// std::cout << "Set MET 4-vector with pT = " << tlv.Pt() << " px = " <<  tlv.Px() << " , py = " << tlv.Py() << " , pz = " << tlv.Pz() << " , E = " << tlv.E()  << " and m = " << tlv.M() << std::endl;

	return tlv;
}

//truth particles
TLorentzVector AnalysisFCChh::getTLV_MC(edm4hep::MCParticleData MC_part){
	TLorentzVector tlv;
	tlv.SetXYZM(MC_part.momentum.x, MC_part.momentum.y, MC_part.momentum.z, MC_part.mass);
	return tlv;
}


ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::merge_pairs(ROOT::VecOps::RVec<RecoParticlePair> pairs){
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> merged_pairs;

	for (auto & pair : pairs){
		TLorentzVector pair_tlv = pair.merged_TLV();

		// //build a edm4hep reco particle from the  pair:
		edm4hep::ReconstructedParticleData pair_particle;
		pair_particle.momentum.x = pair_tlv.Px();
		pair_particle.momentum.y = pair_tlv.Py();
		pair_particle.momentum.z = pair_tlv.Pz();
		pair_particle.mass = pair_tlv.M();

		merged_pairs.push_back(pair_particle);

	}

	return merged_pairs;
}

//select only the first pair in a vector (and retun as vector with size 1, format needed for the rdf)
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::get_first_pair(ROOT::VecOps::RVec<RecoParticlePair> pairs){
	ROOT::VecOps::RVec<RecoParticlePair> first_pair;

	if (pairs.size()){
		first_pair.push_back(pairs.at(0));
	}

	return first_pair;
}

//split the pair again: return only the first particle or second particle in the pairs - needed for getting eg. pT etc of the selected DFOS pair
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::get_first_from_pair(ROOT::VecOps::RVec<RecoParticlePair> pairs){

	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> first_particle;

	if(pairs.size()){
		//sort by pT first: 
		pairs.at(0).sort_by_pT();
		first_particle.push_back(pairs.at(0).particle_1);
	}

	return first_particle;
}

ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::get_second_from_pair(ROOT::VecOps::RVec<RecoParticlePair> pairs){

	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> second_particle;

	if(pairs.size()){
		pairs.at(0).sort_by_pT();
		second_particle.push_back(pairs.at(0).particle_2);
	}

	return second_particle;
}

//count pairs
int AnalysisFCChh::get_n_pairs(ROOT::VecOps::RVec<RecoParticlePair> pairs){
	return pairs.size();
}


//correct function to get jets with a certain tag (can b-tag, c-tag) - check delphes card of sample for which taggers are used
ROOT::VecOps::RVec<bool> AnalysisFCChh::getJet_tag(ROOT::VecOps::RVec<int> index, ROOT::VecOps::RVec<edm4hep::ParticleIDData> pid, ROOT::VecOps::RVec<float> values, int algoIndex){
  ROOT::VecOps::RVec<bool> result;
  for (size_t i = 0; i < index.size(); ++i) {
    auto v = static_cast<unsigned>(values.at(pid.at(index.at(i)).parameters_begin));

    result.push_back((v & (1u << algoIndex)) == (1u << algoIndex));
  }
  return result;
}

//return the full jets rather than the list of tags
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::get_tagged_jets(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> jets, ROOT::VecOps::RVec<int> index, ROOT::VecOps::RVec<edm4hep::ParticleIDData> pid, ROOT::VecOps::RVec<float> tag_values, int algoIndex){
	
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData>  tagged_jets;

	// std::cout << "running AnalysisFCChh::get_tagged_jets() on jet collection of size" << jets.size() << std::endl;

	for (size_t iJet = 0; iJet < jets.size(); ++iJet){
	// for (auto & jet : jets){
		// std::cout << jet.particleIDs_begin << " to " << jet.particleIDs_end << std::endl;
		// get the jet particle id index for the jet
		const auto jetIDIndex = index[jets[iJet].particleIDs_begin];
		// std::cout << "jet index = " << jetIDIndex << std::endl;
		const auto jetID = pid[jetIDIndex];
		// get the tag value
		const auto tag = static_cast<unsigned>(tag_values[jetID.parameters_begin]);
		// std::cout << "Tag = " << tag << std::endl;
		// check if the tag satisfies what we want
    	if (tag & (1 << algoIndex)) {
      		tagged_jets.push_back(jets[iJet]);
    	}
	}

	return tagged_jets;
}

//same for tau tags: they are second entry in the 
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::get_tau_jets(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> jets, ROOT::VecOps::RVec<int> index, ROOT::VecOps::RVec<edm4hep::ParticleIDData> pid, ROOT::VecOps::RVec<float> tag_values, int algoIndex){
	
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData>  tagged_jets;

	// std::cout << "running AnalysisFCChh::get_tagged_jets() on jet collection of size" << jets.size() << std::endl;

	for (size_t iJet = 0; iJet < jets.size(); ++iJet){
	// for (auto & jet : jets){
		// std::cout << jet.particleIDs_begin << " to " << jet.particleIDs_end << std::endl;
		// get the jet particle id index for the jet
		const auto jetIDIndex = index[jets[iJet].particleIDs_begin];
		// std::cout << "jet index = " << jetIDIndex << std::endl;
		const auto jetID = pid[jetIDIndex]; 
		// get the tag value
		const auto tag = static_cast<unsigned>(tag_values[jetID.parameters_end-1]);
		// std::cout << "Tag = " << tag << std::endl;
		// check if the tag satisfies what we want
    	if (tag & (1 << algoIndex)) {
      		tagged_jets.push_back(jets[iJet]);
    	}
	}

	return tagged_jets;
}

//complementary: return the all jets that do not have the requested tag
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::get_untagged_jets(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> jets, ROOT::VecOps::RVec<int> index, ROOT::VecOps::RVec<edm4hep::ParticleIDData> pid, ROOT::VecOps::RVec<float> tag_values, int algoIndex){
	
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData>  untagged_jets;

	// std::cout << "running AnalysisFCChh::get_tagged_jets() on jet collection of size" << jets.size() << std::endl;

	for (size_t iJet = 0; iJet < jets.size(); ++iJet){
	// for (auto & jet : jets){
		// std::cout << jet.particleIDs_begin << " to " << jet.particleIDs_end << std::endl;
		// get the jet particle id index for the jet
		const auto jetIDIndex = index[jets[iJet].particleIDs_begin];
		// std::cout << "jet index = " << jetIDIndex << std::endl;
		const auto jetID = pid[jetIDIndex];
		// get the tag value
		const auto tag = static_cast<unsigned>(tag_values[jetID.parameters_begin]);
		// std::cout << "Tag = " << tag << std::endl;
		// check if the tag satisfies what we want
    	if (!(tag & (1 << algoIndex))) {
      		untagged_jets.push_back(jets[iJet]);
    	}
	}

	return untagged_jets;
}



//select objects that are isolated from the other objects with given dR threshold
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::sel_isolated(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> sel_parts, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> check_parts, float dR_thres){
	
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData>  isolated_parts;

	for (auto & sel_part: sel_parts){
		bool is_isolated = true;
		TLorentzVector sel_part_tlv = getTLV_reco(sel_part);
		// std::cout << "TLV found with pT() =" << sel_part_tlv.Pt() << std::endl;

		//loop over all particles to check against and see if any are within the dR threshold
		for(auto & check_part: check_parts){
			TLorentzVector check_part_tlv = getTLV_reco(check_part);
			float dR_val = sel_part_tlv.DeltaR(check_part_tlv);

			if (dR_val <= dR_thres){is_isolated = false;}

			check_part_tlv.Clear();
		}

		sel_part_tlv.Clear();

		if (is_isolated){isolated_parts.push_back(sel_part);}
	}

	return isolated_parts;
}

// find the lepton pair that likely originates from a Z decay:
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getOSPairs(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> leptons_in){

	ROOT::VecOps::RVec<RecoParticlePair> OS_pairs;

	//need at least 2 leptons in the input
	if (leptons_in.size() < 2){ return OS_pairs; }

	// separate the leptons by charges
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> leptons_pos;
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> leptons_neg;

	for (auto & lep: leptons_in){
		auto charge = lep.charge;
		if (charge > 0){leptons_pos.push_back(lep);}
		else if (charge < 0){leptons_neg.push_back(lep);}

		else{
			std::cout << "Error in function  AnalysisFCChh::getOSPair() - found neutral particle! Function is supposed to be used for electrons or muons only." << std::endl;
		}
	}

	// std::cout << "Found leptons: " << leptons_pos.size() << " pos, " << leptons_neg.size() << " neg." << std::endl;

	// check charges: if don't have one of each, cannot build OS pair and if is only one of each there is no ambiguity
	if (leptons_pos.size() < 1 || leptons_neg.size() < 1){ return OS_pairs; }

	// std::cout << "Have enough leptons to build pair!" << std::endl;

	// build all possible pairs
	// TLorentzVector OS_pair_tlv;

	for (auto & lep_pos: leptons_pos){
		//sum up the momentum components to get the TLV of the OS pair: first the positive one
		// TLorentzVector lep_pos_tlv = getTLV_reco(lep_pos);
		
		for (auto & lep_neg: leptons_neg){
			// TLorentzVector lep_neg_tlv = getTLV_reco(lep_neg);
			// TLorentzVector OS_pair_tlv = lep_pos_tlv+lep_neg_tlv;

			// //build a edm4hep rco particle from the os pair:
			// edm4hep::ReconstructedParticleData OS_pair;
			// OS_pair.momentum.x = OS_pair_tlv.Px();
			// OS_pair.momentum.y = OS_pair_tlv.Py();
			// OS_pair.momentum.z = OS_pair_tlv.Pz();
			// OS_pair.mass = OS_pair_tlv.M();

			//new code: do not merge the pair but store them separately
			RecoParticlePair OS_pair;
			OS_pair.particle_1 = lep_pos;
			OS_pair.particle_2 = lep_neg;

			OS_pairs.push_back(OS_pair);
		} 

	}

	//FOR DEBUG?
	// if (OS_pairs.size() > 1){
	// 	std::cout << "Number of possible OS pairs: " << OS_pairs.size() << std::endl;
	// 	std::cout << "Build from: " << leptons_pos.size() << " pos, " << leptons_neg.size() << " neg." << std::endl;
	// }

	return OS_pairs;

}

//pick the pair that is closest to Z mass:
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getBestOSPair(ROOT::VecOps::RVec<RecoParticlePair> electron_pairs, ROOT::VecOps::RVec<RecoParticlePair> muon_pairs){
	
	ROOT::VecOps::RVec<RecoParticlePair> best_pair;

	// std::cout << "N_elec_pairs = " << electron_pairs.size() << ", N_muon_pairs = " << muon_pairs.size() << std::endl;

	//check if any pairs in input:
	if (electron_pairs.size() == 0 && muon_pairs.size() == 0){
		return best_pair;
	}

	//if only one pair in input, return that one:
	else if ( electron_pairs.size() == 1 && muon_pairs.size() == 0){ 
		best_pair.push_back(electron_pairs.at(0));
		return best_pair;
	}

	else if (electron_pairs.size() == 0 && muon_pairs.size() == 1){
		best_pair.push_back(muon_pairs.at(0));
		return best_pair;

	}

	//if there are mor options, pick the one that is closest to Z mass

	const double Z_mass = 91.1876;

	//make a vector with both electron and muons pairs in it:
	ROOT::VecOps::RVec<RecoParticlePair> all_pairs;
	for (auto & elec_pair : electron_pairs){all_pairs.push_back(elec_pair);}
	for (auto & muon_pair : muon_pairs){all_pairs.push_back(muon_pair);}

	//from Clement's main code: use std::sort on the mass difference
	auto resonancesort = [&] (RecoParticlePair i , RecoParticlePair j) { return (abs( Z_mass - i.merged_TLV().M() ) < abs( Z_mass - j.merged_TLV().M() ) ); };
	// auto resonancesort = [&] (edm4hep::ReconstructedParticleData i ,edm4hep::ReconstructedParticleData j) { return (abs( Z_mass -i.mass)<abs(Z_mass-j.mass)); };
  std::sort(all_pairs.begin(), all_pairs.end(), resonancesort);

  //first one should be the closest one
  best_pair.push_back(all_pairs.at(0));


  return best_pair;

}

//for the bbWW SF analysis: pick the pair that is leading in pTll
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getLeadingPair(ROOT::VecOps::RVec<RecoParticlePair> electron_pairs, ROOT::VecOps::RVec<RecoParticlePair> muon_pairs){
	
	ROOT::VecOps::RVec<RecoParticlePair> best_pair;

	// std::cout << "N_elec_pairs = " << electron_pairs.size() << ", N_muon_pairs = " << muon_pairs.size() << std::endl;

	//check if any pairs in input:
	if (electron_pairs.size() == 0 && muon_pairs.size() == 0){
		return best_pair;
	}

	//if only one pair in input, return that one:
	else if ( electron_pairs.size() == 1 && muon_pairs.size() == 0){ 
		best_pair.push_back(electron_pairs.at(0));
		return best_pair;
	}

	else if (electron_pairs.size() == 0 && muon_pairs.size() == 1){
		best_pair.push_back(muon_pairs.at(0));
		return best_pair;

	}

	//have at least one of each
	//make a vector with both electron and muons pairs in it:
	ROOT::VecOps::RVec<RecoParticlePair> all_pairs;
	for (auto & elec_pair : electron_pairs){all_pairs.push_back(elec_pair);}
	for (auto & muon_pair : muon_pairs){all_pairs.push_back(muon_pair);}

	//take the combined pT to sort
	auto pTll_sort = [&] (RecoParticlePair i , RecoParticlePair j) { return (abs( i.merged_TLV().Pt() ) > abs( j.merged_TLV().Pt() ) ); };
	std::sort(all_pairs.begin(), all_pairs.end(), pTll_sort);

	best_pair.push_back(all_pairs.at(0));

	return best_pair;


}

// build all possible emu OS combinations, for eg tautau and ww analysis
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getDFOSPairs(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> electrons_in, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> muons_in){

	ROOT::VecOps::RVec<RecoParticlePair> DFOS_pairs;

	//need at least 2 leptons in the input
	if (electrons_in.size() < 1 || muons_in.size() < 1){ return DFOS_pairs; }

	//sort the vectors by size, so that the first pair is always the leading
	auto sort_by_pT = [&] (edm4hep::ReconstructedParticleData part_i ,edm4hep::ReconstructedParticleData part_j) { return ( getTLV_reco(part_i).Pt() > getTLV_reco(part_j).Pt() ); };
    std::sort(electrons_in.begin(), electrons_in.end(), sort_by_pT);
    std::sort(muons_in.begin(), muons_in.end(), sort_by_pT);

	//loop over the electrons and make a pair if a muons with opposite charge is found

	for (auto & elec: electrons_in){
		for (auto & muon: muons_in){
			auto total_charge = elec.charge + muon.charge;
			if (total_charge == 0){
				// std::cout << "found DFOS pair!" << std::endl;
				RecoParticlePair DFOS_pair;
				DFOS_pair.particle_1 = elec;
				DFOS_pair.particle_2 = muon;

				DFOS_pairs.push_back(DFOS_pair);
			}
		}
	}			

	//debug
	// if (DFOS_pairs.size() > 1){
	// 	std::cout << "Number of possible DFOS pairs: " << DFOS_pairs.size() << std::endl;
	// 	std::cout << "Build from: " << electrons_in.size() << " electrons, " << muons_in.size() << " muons" << std::endl;
	// }

	return DFOS_pairs;

}

//build all pairs from the input particles -> this returns the pair made of pT leading particles!!!
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getPairs(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particles_in){

	ROOT::VecOps::RVec<RecoParticlePair> pairs;

	//need at least 2 particles in the input
	if (particles_in.size() < 2){ return pairs; }

	//else sort them by pT, and take the only the leading pair
	else {
		auto sort_by_pT = [&] (edm4hep::ReconstructedParticleData part_i ,edm4hep::ReconstructedParticleData part_j) { return ( getTLV_reco(part_i).Pt() > getTLV_reco(part_j).Pt() ); };
    	std::sort(particles_in.begin(), particles_in.end(), sort_by_pT);

    	//old method
		// TLorentzVector tlv_1 = getTLV_reco(particles_in.at(0));
		// TLorentzVector tlv_2 = getTLV_reco(particles_in.at(1));

		// TLorentzVector tlv_pair = tlv_1+tlv_2;

		// edm4hep::ReconstructedParticleData pair;
		// pair.momentum.x = tlv_pair.Px();
		// pair.momentum.y = tlv_pair.Py();
		// pair.momentum.z = tlv_pair.Pz();
		// pair.mass = tlv_pair.M();

		//new method, dont merge the pair
		RecoParticlePair pair;
		pair.particle_1 = particles_in.at(0);
		pair.particle_2 = particles_in.at(1);

		pairs.push_back(pair);

	}

	return pairs;

}


//make the subleading pair, ie. from particles 3 and 4 in pT order
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::getPair_sublead(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particles_in){

	ROOT::VecOps::RVec<RecoParticlePair> pairs;

	//need at least 2 particles in the input
	if (particles_in.size() < 4){ return pairs; }

	//else sort them by pT, and take the only the subleading pair
	else {
		auto sort_by_pT = [&] (edm4hep::ReconstructedParticleData part_i ,edm4hep::ReconstructedParticleData part_j) { return ( getTLV_reco(part_i).Pt() > getTLV_reco(part_j).Pt() ); };
    	std::sort(particles_in.begin(), particles_in.end(), sort_by_pT);


		//new method, dont merge the pair
		RecoParticlePair pair;
		pair.particle_1 = particles_in.at(2);
		pair.particle_2 = particles_in.at(3);

		pairs.push_back(pair);

	}

	return pairs;

}

//calculate the transverse mass ob two objects: massless approximation? 
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mT(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> Z_ll_pair, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){
	
	ROOT::VecOps::RVec<float> mT_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (Z_ll_pair.size() < 1 || MET_obj.size() < 1 ){
		mT_vector.push_back(-999.);
		return mT_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	auto Z_ll = Z_ll_pair.at(0);
	auto MET = MET_obj.at(0);

	// Z_ll is fully reconstructed and regular 4 vector
	TLorentzVector tlv_Zll = getTLV_reco(Z_ll);
	float pT_ll = tlv_Zll.Pt();
	TVector3 vec_pT_ll;
	vec_pT_ll.SetXYZ(Z_ll.momentum.x, Z_ll.momentum.y, 0.  );

	//for MET take the components separately: absolute MET pt and the x and y component in a vector: 
	TLorentzVector tlv_met = getTLV_MET(MET);
	float pT_met = tlv_met.Pt();
	TVector3 vec_pT_met;
	vec_pT_met.SetXYZ(MET.momentum.x, MET.momentum.y, 0.);

	float mT = sqrt(2.*pT_ll*pT_met*( 1-cos( abs( vec_pT_ll.DeltaPhi(vec_pT_met) ) ) ) );


	mT_vector.push_back(mT);

	// std::cout << "Debug mT: mT with old func = " << mT << std::endl;

	return mT_vector;
}

//different definition -> in tests it agreed 100% with previous def, keep for reference
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mT_new(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> vis_part, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){
	
	ROOT::VecOps::RVec<float> mT_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (vis_part.size() < 1 || MET_obj.size() < 1 ){
		mT_vector.push_back(-999.);
		return mT_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	auto visible_particle = vis_part.at(0);
	auto MET = MET_obj.at(0);

	TLorentzVector tlv_vis = getTLV_reco(visible_particle);
	float pT_vis = tlv_vis.Pt();
	TVector3 vec_pT_vis;
	vec_pT_vis.SetXYZ(visible_particle.momentum.x, visible_particle.momentum.y, 0.  );

	//for MET take the components separately: absolute MET pt and the x and y component in a vector: 
	TLorentzVector tlv_met = getTLV_MET(MET);
	float pT_met = tlv_met.Pt();
	TVector3 vec_pT_met;
	vec_pT_met.SetXYZ(MET.momentum.x, MET.momentum.y, 0.);

	float mt_term1 = (pT_vis+pT_met)*(pT_vis+pT_met);
	float mt_term2 = (vec_pT_vis+vec_pT_met).Mag2();

	float mT = sqrt(mt_term1 - mt_term2 );


	mT_vector.push_back(mT);

	// std::cout << "Debug mT: mT with new func = " << mT << std::endl;

	return mT_vector;
}


//pseudo-invariant mass - see CMS paper, PHYS. REV. D 102, 032003 (2020)
ROOT::VecOps::RVec<float> AnalysisFCChh::get_m_pseudo(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> Z_ll_pair, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){

	ROOT::VecOps::RVec<float> m_pseudo_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (Z_ll_pair.size() < 1 || MET_obj.size() < 1 ){
		m_pseudo_vector.push_back(-999.);
		return m_pseudo_vector;
	}

	TLorentzVector tlv_Zll = getTLV_reco(Z_ll_pair.at(0));
	TLorentzVector tlv_MET = getTLV_MET(MET_obj.at(0));

	TLorentzVector tlv_H_pseudo = tlv_Zll+tlv_MET;

	m_pseudo_vector.push_back(tlv_H_pseudo.M());

	return m_pseudo_vector;
}

//pseudo-transverse mass - see CMS paper, PHYS. REV. D 102, 032003 (2020)
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mT_pseudo(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> Z_ll_pair, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){

	ROOT::VecOps::RVec<float> m_pseudo_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (Z_ll_pair.size() < 1 || MET_obj.size() < 1 ){
		m_pseudo_vector.push_back(-999.);
		return m_pseudo_vector;
	}

	TLorentzVector tlv_Zll = getTLV_reco(Z_ll_pair.at(0));
	TLorentzVector tlv_MET = getTLV_MET(MET_obj.at(0));

	TLorentzVector tlv_H_pseudo = tlv_Zll+tlv_MET;

	m_pseudo_vector.push_back(sqrt(tlv_H_pseudo.E()*tlv_H_pseudo.E() - tlv_H_pseudo.Pz()*tlv_H_pseudo.Pz()));

	return m_pseudo_vector;
}

// try the stransverse mass as defined in arXiv:1411.4312
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mT2(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_2, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){
	
	asymm_mt2_lester_bisect::disableCopyrightMessage();

	ROOT::VecOps::RVec<float> m_strans_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (particle_1.size() < 1 || particle_2.size() < 1|| MET_obj.size() < 1 ){
		m_strans_vector.push_back(-999.);
		return m_strans_vector;
	}

	TLorentzVector tlv_vis1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_vis2 = getTLV_reco(particle_2.at(0));
	TLorentzVector tlv_met = getTLV_MET(MET_obj.at(0));

	// std::cout << "Part1 : Compare TLV w. direct for px:" << tlv_vis1.Px() << " vs " << particle_1.at(0).momentum.x << std::endl;
	// std::cout << "Part1 : Compare TLV w. direct for py:" << tlv_vis1.Py() << " vs " << particle_1.at(0).momentum.y << std::endl;

	// std::cout << "Part2 : Compare TLV w. direct for px:" << tlv_vis2.Px() << " vs " << particle_2.at(0).momentum.x << std::endl;
	// std::cout << "Part2 : Compare TLV w. direct for px:" << tlv_vis2.Py() << " vs " << particle_2.at(0).momentum.y << std::endl; 

	// std::cout << "MET : Compare TLV w. direct for px:" << tlv_met.Px() << " vs " << MET_obj.at(0).momentum.x << std::endl;
	// std::cout << "MET : Compare TLV w. direct for px:" << tlv_met.Py() << " vs " << MET_obj.at(0).momentum.y << std::endl; 

	double MT2 =  asymm_mt2_lester_bisect::get_mT2(
           tlv_vis1.M(), tlv_vis1.Px(), tlv_vis1.Py(),
           tlv_vis2.M(), tlv_vis2.Px(), tlv_vis2.Py(),
           tlv_met.Px(), tlv_met.Py(),
           0., 0.);

	// define our inputs:

	m_strans_vector.push_back(MT2);

	return m_strans_vector;
}

// stransverse mass but fixing the two visible to higgs masses
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mT2_125(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_2, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){
	
	asymm_mt2_lester_bisect::disableCopyrightMessage();

	ROOT::VecOps::RVec<float> m_strans_vector; 

	//if one of the input particles is empty, just fill a default value of -999 as mT
	if (particle_1.size() < 1 || particle_2.size() < 1|| MET_obj.size() < 1 ){
		m_strans_vector.push_back(-999.);
		return m_strans_vector;
	}

	TLorentzVector tlv_vis1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_vis2 = getTLV_reco(particle_2.at(0));
	TLorentzVector tlv_met = getTLV_MET(MET_obj.at(0));

	double MT2 =  asymm_mt2_lester_bisect::get_mT2(
           125., tlv_vis1.Px(), tlv_vis1.Py(),
           125., tlv_vis2.Px(), tlv_vis2.Py(),
           tlv_met.Px(), tlv_met.Py(),
           0., 0.);

	// define our inputs:

	m_strans_vector.push_back(MT2);

	return m_strans_vector;
}


//HT2 variable as in ATLAS bblvlv analysis
ROOT::VecOps::RVec<float> AnalysisFCChh::get_HT2(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_2){
	
	ROOT::VecOps::RVec<float> HT2_vector; 	

	if (particle_1.size() < 1 || particle_2.size() < 1){
		HT2_vector.push_back(-999.);
		return HT2_vector;
	}

	TLorentzVector tlv_1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_2 = getTLV_reco(particle_2.at(0));

	//scalar sum
	float HT2 = tlv_1.Pt() + tlv_2.Pt();
	HT2_vector.push_back(HT2);
	return HT2_vector;
}

//HT_w_inv = scalar sum of all pT from objects of a HH->bblvlv decay as used in ATLAS paper for the HT ratio
ROOT::VecOps::RVec<float> AnalysisFCChh::get_HT_wInv(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET, ROOT::VecOps::RVec<RecoParticlePair> ll_pair, ROOT::VecOps::RVec<RecoParticlePair> bb_pair){

	ROOT::VecOps::RVec<float> HT_wInv_vector; 

	if (MET.size() < 1 || ll_pair.size() < 1 || bb_pair.size() < 1){
		HT_wInv_vector.push_back(-999.);
		return HT_wInv_vector;
	}

	//if all objects are there, get the first entry in vector always (should be leading) and take the pTs
	float MET_pT = getTLV_MET(MET.at(0)).Pt();

	float lep1_pT = getTLV_reco(ll_pair.at(0).particle_1).Pt();
	float lep2_pT = getTLV_reco(ll_pair.at(0).particle_2).Pt();

	float b1_pT = getTLV_reco(bb_pair.at(0).particle_1).Pt();
	float b2_pT = getTLV_reco(bb_pair.at(0).particle_2).Pt();

	//and sum ..
	float HT_w_inv = MET_pT+ lep1_pT+ lep2_pT+ b1_pT+ b2_pT;
	HT_wInv_vector.push_back(HT_w_inv);
	return HT_wInv_vector;
}

//get the true HT = scalar sum of only the visible objects, here the bs and the leptons (true HT in contrast to the HT with the MET)
ROOT::VecOps::RVec<float> AnalysisFCChh::get_HT_true(ROOT::VecOps::RVec<RecoParticlePair> ll_pair, ROOT::VecOps::RVec<RecoParticlePair> bb_pair){

	ROOT::VecOps::RVec<float> HT_wInv_vector; 

	if (ll_pair.size() < 1 || bb_pair.size() < 1){
		HT_wInv_vector.push_back(-999.);
		return HT_wInv_vector;
	}

	//if all objects are there, get the first entry in vector always (should be leading) and take the pTs
	float lep1_pT = getTLV_reco(ll_pair.at(0).particle_1).Pt();
	float lep2_pT = getTLV_reco(ll_pair.at(0).particle_2).Pt();

	float b1_pT = getTLV_reco(bb_pair.at(0).particle_1).Pt();
	float b2_pT = getTLV_reco(bb_pair.at(0).particle_2).Pt();

	//and sum ..
	float HT_w_inv = lep1_pT+ lep2_pT+ b1_pT+ b2_pT;
	HT_wInv_vector.push_back(HT_w_inv);
	return HT_wInv_vector;
}

//construct ratio of HT2 and HT_w_inv
ROOT::VecOps::RVec<float> AnalysisFCChh::get_HT2_ratio(ROOT::VecOps::RVec<float> HT2, ROOT::VecOps::RVec<float> HT_wInv){

	ROOT::VecOps::RVec<float> HT2_ratio_vector; 

	if (HT2.size() < 1 || HT_wInv.size() < 1 ){
		HT2_ratio_vector.push_back(-999.);
		return HT2_ratio_vector;
	}

	float HT2_ratio = HT2.at(0)/HT_wInv.at(0);
	HT2_ratio_vector.push_back(HT2_ratio);
	return HT2_ratio_vector;

}

//construct met signifcance as ratio of MET pt and true HT
ROOT::VecOps::RVec<float> AnalysisFCChh::get_MET_significance(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET, ROOT::VecOps::RVec<float> HT_true, bool doSqrt){

	ROOT::VecOps::RVec<float> MET_sig_vector; 

	if (MET.size() < 1 || HT_true.size() < 1 ){
		MET_sig_vector.push_back(-999.);
		return MET_sig_vector;
	}

	float MET_pt = getTLV_MET(MET.at(0)).Pt();

	if (doSqrt){
		MET_sig_vector.push_back(MET_pt/sqrt(HT_true.at(0)));
	}
	else{
		MET_sig_vector.push_back(MET_pt/HT_true.at(0));
	}

	return MET_sig_vector;
}

//helper function which merges two particles into one using the TLVs
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::merge_parts_TLVs(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_2)
{
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> out_vector;

	//if one of the input particles is empty, return an empty vector 
	if (particle_1.size() < 1 || particle_2.size() < 1 ){
		// std::cout << "Warning in AnalysisFCChh::merge_parts_TLVs - one input vector is empty, returning an empty vector." << std::endl;
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_2 = getTLV_reco(particle_2.at(0));

	TLorentzVector tlv_merged = tlv_1+tlv_2;

	edm4hep::ReconstructedParticleData particle_merged;
	particle_merged.momentum.x = tlv_merged.Px();
	particle_merged.momentum.y = tlv_merged.Py();
	particle_merged.momentum.z = tlv_merged.Pz();
	particle_merged.mass = tlv_merged.M();

	out_vector.push_back(particle_merged);

	return out_vector;

}

//same as above, overloaded for MCParticles
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::merge_parts_TLVs(ROOT::VecOps::RVec<edm4hep::MCParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::MCParticleData> particle_2)
{
	ROOT::VecOps::RVec<edm4hep::MCParticleData> out_vector;

	//if one of the input particles is empty, return an empty vector 
	if (particle_1.size() < 1 || particle_2.size() < 1 ){
		// std::cout << "Warning in AnalysisFCChh::merge_parts_TLVs - one input vector is empty, returning an empty vector." << std::endl;
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_MC(particle_1.at(0));
	TLorentzVector tlv_2 = getTLV_MC(particle_2.at(0));

	TLorentzVector tlv_merged = tlv_1+tlv_2;

	edm4hep::MCParticleData particle_merged;
	particle_merged.momentum.x = tlv_merged.Px();
	particle_merged.momentum.y = tlv_merged.Py();
	particle_merged.momentum.z = tlv_merged.Pz();
	particle_merged.mass = tlv_merged.M();

	out_vector.push_back(particle_merged);

	return out_vector;

}

//combine one lepton with one b-jet each, in case of ttbar events this should reconstruct the visible top

//find lb pairs with smallest average 
ROOT::VecOps::RVec<RecoParticlePair> AnalysisFCChh::make_lb_pairing(ROOT::VecOps::RVec<RecoParticlePair> lepton_pair, ROOT::VecOps::RVec<RecoParticlePair> bb_pair){

	ROOT::VecOps::RVec<RecoParticlePair> out_vector;
	RecoParticlePair lb_pair_1;
	RecoParticlePair lb_pair_2;


	//if one of the input particles is empty, return an empty vector
	if (lepton_pair.size() < 1 || bb_pair.size() < 1 ){
		return out_vector;
	}

	//take the separate particles
	TLorentzVector tlv_lepton_1 = getTLV_reco(lepton_pair.at(0).particle_1);
	TLorentzVector tlv_lepton_2 = getTLV_reco(lepton_pair.at(0).particle_2);

	TLorentzVector tlv_bjet_1 = getTLV_reco(bb_pair.at(0).particle_1);
	TLorentzVector tlv_bjet_2 = getTLV_reco(bb_pair.at(0).particle_2);

	//then make the two possible combinations:
	TLorentzVector tlv_l1_b1 = tlv_lepton_1+tlv_bjet_1;
	TLorentzVector tlv_l2_b2 = tlv_lepton_2+tlv_bjet_2;

	TLorentzVector tlv_l1_b2 = tlv_lepton_1+tlv_bjet_2;
	TLorentzVector tlv_l2_b1 = tlv_lepton_2+tlv_bjet_1;

	//calculate the average invariant masses for the two combinations:
	float mlb_comb1 = (tlv_l1_b1.M() + tlv_l2_b2.M() )/2.;
	float mlb_comb2 = (tlv_l1_b2.M() + tlv_l2_b1.M() )/2.;

	// std::cout << "Mlb_comb1: " << mlb_comb1 << std::endl;
	// std::cout << "Mlb_comb2: " << mlb_comb2 << std::endl;

	//the combination with minimum mlb is the one we pick
	if (mlb_comb1 < mlb_comb2){
		lb_pair_1.particle_1 = lepton_pair.at(0).particle_1; 
		lb_pair_1.particle_2 = bb_pair.at(0).particle_1; 

		lb_pair_2.particle_1 = lepton_pair.at(0).particle_2; 
		lb_pair_2.particle_2 = bb_pair.at(0).particle_2; 


	}

	else{

		lb_pair_1.particle_1 = lepton_pair.at(0).particle_1; 
		lb_pair_1.particle_2 = bb_pair.at(0).particle_2; 

		lb_pair_2.particle_1 = lepton_pair.at(0).particle_2; 
		lb_pair_2.particle_2 = bb_pair.at(0).particle_1; 

	}

	out_vector.push_back(lb_pair_1);
	out_vector.push_back(lb_pair_2);
	
	return out_vector;

}

//rather inefficienct, but now get the actually value of mlb again
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mlb_reco(ROOT::VecOps::RVec<RecoParticlePair> lb_pairs){

	ROOT::VecOps::RVec<float> out_vector;

	//there should be two pairs
	if (lb_pairs.size() < 2 ){
		return out_vector;
	}

	TLorentzVector tlv_pair1 = lb_pairs.at(0).merged_TLV();
	TLorentzVector tlv_pair2 = lb_pairs.at(1).merged_TLV();

	float mlb_reco = (tlv_pair1.M() + tlv_pair2.M() )/2.;

	out_vector.push_back(mlb_reco);
	// std::cout << "Mlb : " << mlb_reco << std::endl;

	return out_vector;

}

//do trhe same thing and also add met into it
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mlb_MET_reco(ROOT::VecOps::RVec<RecoParticlePair> lb_pairs, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET){

	ROOT::VecOps::RVec<float> out_vector;

	//there should be two pairs and one met
	if (lb_pairs.size() < 2 || MET.size() < 1){
		return out_vector;
	}

	TLorentzVector tlv_pair1 = lb_pairs.at(0).merged_TLV();
	TLorentzVector tlv_pair2 = lb_pairs.at(1).merged_TLV();
	TLorentzVector tlv_MET = getTLV_MET(MET.at(0));

	float mlb_reco = (tlv_pair1 + tlv_pair2 + tlv_MET ).M()/2.;

	out_vector.push_back(mlb_reco);
	// std::cout << "Mlb : " << mlb_reco << std::endl;

	return out_vector;

}

//calculate the pzetas, following CMS tautau analyses: https://github.com/cardinia/DesyTauAnalysesUL/blob/dev/Common/interface/functions.h#L792-L841

ROOT::VecOps::RVec<float> AnalysisFCChh::get_pzeta_vis(ROOT::VecOps::RVec<RecoParticlePair> lepton_pair){

	ROOT::VecOps::RVec<float> out_vector;

	//there should be one lepton pair
	if (lepton_pair.size() < 1 ){
		return out_vector;
	}

	//get the tlvs of the leptons:
	TLorentzVector tlv_lepton_1 = getTLV_reco(lepton_pair.at(0).particle_1);
	TLorentzVector tlv_lepton_2 = getTLV_reco(lepton_pair.at(0).particle_2);


	//normalize the pT vectors of the leptons to their magnitudes -> make unit vectors, split in x and y components
	float vec_unit_lep1_x = tlv_lepton_1.Px()/tlv_lepton_1.Pt();
    float vec_unit_lep1_y = tlv_lepton_1.Py()/tlv_lepton_1.Pt();

    float vec_unit_lep2_x = tlv_lepton_2.Px()/tlv_lepton_2.Pt();
    float vec_unit_lep2_y = tlv_lepton_2.Py()/tlv_lepton_2.Pt();   

    //the sum of the two unit vectors is the bisector
    float zx = vec_unit_lep1_x + vec_unit_lep2_x;
    float zy = vec_unit_lep1_y + vec_unit_lep2_y; 

    //normalize with magnitude again?
    float modz = sqrt(zx*zx+zy*zy);
    zx = zx/modz;
    zy = zy/modz;

    //build the projection of pTll onto this bisector
    float vis_ll_x = tlv_lepton_1.Px()+tlv_lepton_2.Px();
    float vis_ll_y = tlv_lepton_1.Py()+tlv_lepton_2.Py();

    float pzeta_vis = zx*vis_ll_x+zy*vis_ll_y;

    out_vector.push_back(pzeta_vis);

    // std::cout << "pzeta_vis = " << pzeta_vis << std::endl;

    return out_vector;
}

//for pzeta_miss do the same for bisector, but then project etmiss onot it:
ROOT::VecOps::RVec<float> AnalysisFCChh::get_pzeta_miss(ROOT::VecOps::RVec<RecoParticlePair> lepton_pair, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET){

	ROOT::VecOps::RVec<float> out_vector;

	//there should be one lepton pair and one MET
	if (lepton_pair.size() < 1 || MET.size() < 1){
		return out_vector;
	}

	//get the tlvs of the leptons:
	TLorentzVector tlv_lepton_1 = getTLV_reco(lepton_pair.at(0).particle_1);
	TLorentzVector tlv_lepton_2 = getTLV_reco(lepton_pair.at(0).particle_2);
	TLorentzVector tlv_MET = getTLV_MET(MET.at(0));

	//normalize the pT vectors of the leptons to their magnitudes -> make unit vectors, split in x and y components
	float vec_unit_lep1_x = tlv_lepton_1.Px()/tlv_lepton_1.Pt();
    float vec_unit_lep1_y = tlv_lepton_1.Py()/tlv_lepton_1.Pt();

    float vec_unit_lep2_x = tlv_lepton_2.Px()/tlv_lepton_2.Pt();
    float vec_unit_lep2_y = tlv_lepton_2.Py()/tlv_lepton_2.Pt();   

    //the sum of the two unit vectors is the bisector
    float zx = vec_unit_lep1_x + vec_unit_lep2_x;
    float zy = vec_unit_lep1_y + vec_unit_lep2_y; 

    //normalize with magnitude again?
    float modz = sqrt(zx*zx+zy*zy);
    zx = zx/modz;
    zy = zy/modz;

 	//build the projection of MET onto this bisector
    float pzeta_miss = zx*tlv_MET.Pt()*cos(tlv_MET.Phi())+zy*tlv_MET.Pt()*sin(tlv_MET.Phi());

    out_vector.push_back(pzeta_miss);

    // std::cout << "pzeta_miss = " << pzeta_miss << std::endl;

    return out_vector;
}

//combine the two with a factor applied: CMS tautau uses 0.85
ROOT::VecOps::RVec<float> AnalysisFCChh::get_dzeta(ROOT::VecOps::RVec<float> pzeta_miss, ROOT::VecOps::RVec<float> pzeta_vis, float factor){

	ROOT::VecOps::RVec<float> out_vector;

	//there should be one pzeta each
	if (pzeta_miss.size() < 1 || pzeta_vis.size() < 1){
		return out_vector;
	}

	out_vector.push_back(pzeta_miss.at(0) - factor*pzeta_vis.at(0));

	// std::cout << "dzeta = " << pzeta_miss.at(0) - factor*pzeta_vis.at(0) << std::endl;

	return out_vector;

}

//combine MET with the Zll pair into the HZZ candidate
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::build_HZZ(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> Z_ll_pair, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj){
	
	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> out_vector;

	//if one of the input particles is empty, return an empty vector and a warning
	if (Z_ll_pair.size() < 1 || MET_obj.size() < 1 ){
		// std::cout << "Warning in AnalysisFCChh::build_HZZ - one input vector is empty, returning an empty vector." << std::endl;
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_reco(Z_ll_pair.at(0));
	TLorentzVector tlv_2 = getTLV_MET(MET_obj.at(0));

	TLorentzVector tlv_merged = tlv_1+tlv_2;

	edm4hep::ReconstructedParticleData particle_merged;
	particle_merged.momentum.x = tlv_merged.Px();
	particle_merged.momentum.y = tlv_merged.Py();
	particle_merged.momentum.z = tlv_merged.Pz();
	particle_merged.mass = tlv_merged.M();

	out_vector.push_back(particle_merged);

	return out_vector;

}

//get dR between two objects:
ROOT::VecOps::RVec<float> AnalysisFCChh::get_angularDist(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_2, TString type){

	ROOT::VecOps::RVec<float> out_vector; 

	//if one of the input particles is empty, fill default value
	if (particle_1.size() < 1 || particle_2.size() < 1 ){
		out_vector.push_back(-999.);
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_2 = getTLV_reco(particle_2.at(0));

	if (type.Contains("dR")){
		out_vector.push_back(tlv_1.DeltaR(tlv_2));
	}

	else if (type.Contains("dEta")){
		out_vector.push_back(abs(tlv_1.Eta() -  tlv_2.Eta() ));
	}

	else if (type.Contains("dPhi")){
		out_vector.push_back(tlv_1.DeltaPhi(tlv_2));
	}

	else{
		std::cout << " Error in AnalysisFCChh::get_angularDist - requested unknown type " << type << "Returning default of -999." << std::endl;
		out_vector.push_back(-999.);
	}

	return out_vector;

}

//get angular distances between MET and an object:
ROOT::VecOps::RVec<float> AnalysisFCChh::get_angularDist_MET(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> particle_1, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET_obj, TString type){

	ROOT::VecOps::RVec<float> out_vector; 

	//if one of the input particles is empty, fill default value
	if (particle_1.size() < 1 || MET_obj.size() < 1 ){
		out_vector.push_back(-999.);
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_reco(particle_1.at(0));
	TLorentzVector tlv_2 = getTLV_MET(MET_obj.at(0));

	if (type.Contains("dR")){
		out_vector.push_back(tlv_1.DeltaR(tlv_2));
	}

	else if (type.Contains("dEta")){
		out_vector.push_back(abs(tlv_1.Eta() -  tlv_2.Eta() ));
	}

	else if (type.Contains("dPhi")){
		out_vector.push_back(tlv_1.DeltaPhi(tlv_2));
	}

	else{
		std::cout << " Error in AnalysisFCChh::get_angularDist - requested unknown type " << type << "Returning default of -999." << std::endl;
		out_vector.push_back(-999.);
	}

	return out_vector;

}

//get angular distances between the two particles in a pair:
ROOT::VecOps::RVec<float> AnalysisFCChh::get_angularDist_pair(ROOT::VecOps::RVec<RecoParticlePair> pairs, TString type){

	ROOT::VecOps::RVec<float> out_vector; 

	//if input pairs is empty, fill default value
	if (pairs.size() < 1 ){
		out_vector.push_back(-999.);
		return out_vector;
	}

	//else, for now, just take the first of each, should be the "best" one (by user input) - flexibility to use all combinations is there, to be implemented if needed
	TLorentzVector tlv_1 = getTLV_reco(pairs.at(0).particle_1);
	TLorentzVector tlv_2 = getTLV_reco(pairs.at(0).particle_2);

	if (type.Contains("dR")){
		out_vector.push_back(tlv_1.DeltaR(tlv_2));
	}

	else if (type.Contains("dEta")){
		out_vector.push_back(abs(tlv_1.Eta() -  tlv_2.Eta() ));
	}

	else if (type.Contains("dPhi")){
		out_vector.push_back(tlv_1.DeltaPhi(tlv_2));
	}

	else{
		std::cout << " Error in AnalysisFCChh::get_angularDist - requested unknown type " << type << "Returning default of -999." << std::endl;
		out_vector.push_back(-999.);
	}

	return out_vector;

}

//function which finds truth higgs in the MC particles and selects the one that decays according to requested type (to ZZ or bb here)
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::get_truth_Higgs(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids, TString decay){
	ROOT::VecOps::RVec<edm4hep::MCParticleData>  higgs_list;

	// std::cout << "looking for higgs .." << std::endl;

	for (auto & truth_part: truth_particles) {
		if (isH(truth_part)){
			//check into which particles the Higgs decays:

			auto first_child_index = truth_part.daughters_begin;
			auto last_child_index = truth_part.daughters_end;

			if(last_child_index - first_child_index != 2){
				std::cout << "Error in get_truth_Higgs! Found more or fewer than exactly 2 daughters of a higgs boson - this is not expected by code. Need to implement a solution still!"<< std::endl;
				return higgs_list;
			}

			//now get the indices in the daughters vector
			auto child_1_MC_index = daughter_ids.at(first_child_index).index;
			auto child_2_MC_index = daughter_ids.at(last_child_index-1).index;

			//then go back to the original vector of MCParticles
			auto child_1 = truth_particles.at(child_1_MC_index);
			auto child_2 = truth_particles.at(child_2_MC_index);


			// std::cout << "Found higgs with status = " << h_status << " and children with indices " << child_1.PDG << " , " << child_2.PDG << std::endl;
			if (decay.Contains("ZZ") && isZ(child_1) && isZ(child_2) ){
				higgs_list.push_back(truth_part);
			}

			else if (decay.Contains("bb") && isb(child_1) && isb(child_2)){
				higgs_list.push_back(truth_part);
			}
		}
	}

	return higgs_list;
}

//get the truth flavour of the leptons from taus
ROOT::VecOps::RVec<int> AnalysisFCChh::getTruthLepLepFlavour(ROOT::VecOps::RVec<edm4hep::MCParticleData> leps_from_tau){

	ROOT::VecOps::RVec<int> results_vec;

	if (leps_from_tau.size() != 2){
		std::cout << "Error - running getTruthLepLepFlavour on event which doesn't have exactly two leptons from taus. This isnt the intended usage." << std::endl;
		return results_vec;
	}

	auto pdg_1 = leps_from_tau.at(0).PDG;
	auto pdg_2 = leps_from_tau.at(1).PDG;

	if (abs(pdg_1) == 11 && abs(pdg_2) == 11){
		results_vec.push_back(0);
	}

	else if (abs(pdg_1) == 13 && abs(pdg_2) == 13){
		results_vec.push_back(1);
	}

	else if ( (abs(pdg_1) == 11 && abs(pdg_2) == 13) ||  (abs(pdg_1) == 13 && abs(pdg_2) == 11) ) {
		results_vec.push_back(2);
	}

	//option for taus, needed for checking bbWW
	else if (abs(pdg_1) == 15 || abs(pdg_2) == 15){
		results_vec.push_back(3);
	}

	else{
		std::cout << "Error - found leptons from taus that are neither electrons nor muons" << std::endl;
		results_vec.push_back(-999);
	}

	return results_vec;
}

//take the vector with truth leptons from taus  and pick out the electron or muon
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getTruthEle(ROOT::VecOps::RVec<edm4hep::MCParticleData> leps_from_tau){

	ROOT::VecOps::RVec<edm4hep::MCParticleData> results_vec;

	for (auto & truth_lep: leps_from_tau) {

		// std::cout << "PDG ID " << abs(truth_lep.PDG) << std::endl;

		//electrons
		if (abs(truth_lep.PDG) == 11){
			results_vec.push_back(truth_lep);
		}

	}
	return results_vec;

}

ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getTruthMu(ROOT::VecOps::RVec<edm4hep::MCParticleData> leps_from_tau){

	ROOT::VecOps::RVec<edm4hep::MCParticleData> results_vec;

	for (auto & truth_lep: leps_from_tau) {

		//muons
		if (abs(truth_lep.PDG) == 13){
			results_vec.push_back(truth_lep);
		}

	}
	return results_vec;

}


//find the light leptons (e or mu) that originate from a tau decay (which comes from a higgs, and not a b-meson) using the truth info -> to use as filter for emu tautau evts
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getLepsFromTau(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> parent_ids){
	//test by simply counting first:
	// int counter = 0; 
	ROOT::VecOps::RVec<edm4hep::MCParticleData> leps_list;

	//loop over all truth particles and find light leptons from taus that came from higgs (the direction tau->light lepton as child appears to be missing in the tautau samples)
	for (auto & truth_part: truth_particles) {
		if (isLightLep(truth_part)){
			bool from_tau_higgs = isChildOfTauFromHiggs(truth_part, parent_ids, truth_particles);
			if(from_tau_higgs){
				// counter+=1;
				leps_list.push_back(truth_part);
			}
		}
	}
	// std::cout << "Leps from tau-higgs " << counter << std::endl; 
	return leps_list;
}

//find truth (hadronic) taus, to check the tau veto
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getTruthTauHads(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> daughter_ids, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, TString type){

	ROOT::VecOps::RVec<edm4hep::MCParticleData> tau_list;
	for (auto & truth_part: truth_particles) {
		if (isTau(truth_part)){

			//check also if from Higgs to count only from Higgs ones
			if ( type.Contains("from_higgs") && hasHiggsParent(truth_part, parent_ids, truth_particles) && !isFromHadron(truth_part, parent_ids, truth_particles) ) {
				continue;
			}

			auto first_child_index = truth_part.daughters_begin;
			auto last_child_index = truth_part.daughters_end;

			auto child_1_MC_index = daughter_ids.at(first_child_index).index;
			auto child_2_MC_index = daughter_ids.at(last_child_index-1).index;

			// std::cout << " found tau with children" << std::endl;

			for(int child_i = first_child_index; child_i < last_child_index; child_i++){
				auto child = truth_particles.at(daughter_ids.at(child_i).index);
				// std::cout << "Child ID: " << child.PDG << std::endl;
				if (isHadron(child)){
					// std::cout <<" Is hadronic" << std::endl;
					tau_list.push_back(truth_part);
				}

			}

		}
	}

	return tau_list;
}

//find leptons (including taus?) that came from a H->WW decay
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getLepsFromW(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> parent_ids){
	//test by simply counting first:
	// int counter = 0; 
	ROOT::VecOps::RVec<edm4hep::MCParticleData> leps_list;

	//loop over all truth particles and find light leptons from taus that came from higgs (the direction tau->light lepton as child appears to be missing in the tautau samples)
	for (auto & truth_part: truth_particles) {
		if (isLep(truth_part)){//switch to isLightLep for tau veto!
			bool from_W_higgs = isChildOfWFromHiggs(truth_part, parent_ids, truth_particles);
			if(from_W_higgs){
				// counter+=1;
				leps_list.push_back(truth_part);
			}
		}
	}
	// std::cout << "Leps from tau-higgs " << counter << std::endl; 
	return leps_list;
}

//momentum fraction x for tau decays
ROOT::VecOps::RVec<float> AnalysisFCChh::get_x_fraction(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> visible_particle, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET){
	ROOT::VecOps::RVec<float> results_vec;

	if (visible_particle.size() < 1 || MET.size() < 1){
		results_vec.push_back(-999.);
		return results_vec;
	}	

	//get the components of the calculation
	TLorentzVector met_tlv = getTLV_reco(MET.at(0));
	// TLorentzVector met_tlv = getTLV_MET(MET.at(0));
	TLorentzVector vis_tlv = getTLV_reco(visible_particle.at(0));

	// float x_fraction = vis_tlv.Pt()/(vis_tlv.Pt()+met_tlv.Pt()); // try scalar sum
	float x_fraction = vis_tlv.Pt()/(vis_tlv+met_tlv).Pt(); // vector sum makes more sense?

	// std::cout << " Debug m_col: pT_vis : " << vis_tlv.Pt() << std::endl;
	// std::cout << " Debug m_col: pT_miss : " << met_tlv.Pt() << std::endl;
	// std::cout << " Debug m_col: x with scalar sum: " << x_fraction << std::endl;
	// std::cout << " Debug m_col: x with vector sum: " << vis_tlv.Pt()/(vis_tlv+met_tlv).Pt() << std::endl;

	results_vec.push_back(x_fraction);
	return results_vec;
}

//with truth variables
ROOT::VecOps::RVec<float> AnalysisFCChh::get_x_fraction_truth(ROOT::VecOps::RVec<edm4hep::MCParticleData> visible_particle, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> MET){
	ROOT::VecOps::RVec<float> results_vec;

	if (visible_particle.size() < 1 || MET.size() < 1){
		results_vec.push_back(-999.);
		return results_vec;
	}	

	//get the components of the calculation
	TLorentzVector met_tlv = getTLV_reco(MET.at(0));
	// TLorentzVector met_tlv = getTLV_MET(MET.at(0));
	TLorentzVector vis_tlv = getTLV_MC(visible_particle.at(0));

	// float x_fraction = vis_tlv.Pt()/(vis_tlv.Pt()+met_tlv.Pt()); // try scalar sum
	float x_fraction = vis_tlv.Pt()/(vis_tlv+met_tlv).Pt(); // vector sum makes more sense?

	// std::cout << " Debug m_col: pT_vis truth : " << vis_tlv.Pt() << std::endl;
	// std::cout << " Debug m_col: pT_miss truth : " << met_tlv.Pt() << std::endl;
	// std::cout << " Debug m_col: x truth with vector sum: " << x_fraction << std::endl;
	// std::cout << " Debug m_col: x with vector sum: " << vis_tlv.Pt()/(vis_tlv+met_tlv).Pt() << std::endl;

	results_vec.push_back(x_fraction);
	return results_vec;
}

ROOT::VecOps::RVec<float> AnalysisFCChh::get_mtautau_col(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> ll_pair_merged, ROOT::VecOps::RVec<float> x1, ROOT::VecOps::RVec<float> x2){
	ROOT::VecOps::RVec<float> results_vec;

	//here there is no result if any of the arguments is actually not filled (e.g. no selected pair) OR if one of the xs is 0 or negative (shouldnt happen but better be on the safe side?)
	if (ll_pair_merged.size() < 1 || x1.size() < 1 || x2.size() <1 || x1.at(0) <= 0. || x2.at(0) <= 0.){
		results_vec.push_back(-999.);
		return results_vec;
	}	

	float mtautau_vis = getTLV_reco(ll_pair_merged.at(0)).M(); //check against manual calculation?? 
	float mtautau_col = mtautau_vis/(sqrt(x1.at(0)*x2.at(0)));

	// std::cout << " Debug m_col: mtautau_vis mass = " << mtautau_vis << std::endl;
	// std::cout << " Debug m_col: m_col mass = " << mtautau_col << std::endl;

	results_vec.push_back(mtautau_col);
	return results_vec;

}

//merge the invariant bb mass and the tautau colinear mass for the bbtautau(emu) analysis
ROOT::VecOps::RVec<float> AnalysisFCChh::get_mbbtautau_col(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> bb_pair_merged, ROOT::VecOps::RVec<float> mtautau_col){
	ROOT::VecOps::RVec<float> results_vec;

	//here there is no result if any of the arguments is actually not filled or if the mtautau col is at default value -999.
	if (bb_pair_merged.size() < 1 || mtautau_col.size() < 1 || mtautau_col.at(0) <= 0.){
		results_vec.push_back(-999.);
		return results_vec;
	}	

	float mbb = getTLV_reco(bb_pair_merged.at(0)).M();

	results_vec.push_back(mbb+mtautau_col.at(0));
	return results_vec;

}


//truth matching: find a reco part that matches the truth part within cone of dR_thres
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::find_reco_matched(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_parts_to_match, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> reco_parts_all, float dR_thres){

	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> out_vector;

	//if no iput part, return nothing (didnt input correct event then)
	if (truth_parts_to_match.size() < 1){
		return out_vector;
	}

	//currently only want one particle to match, to not get confused with vectors:
	if (truth_parts_to_match.size() != 1){
		std::cout << "Error! Found more than one truth part in input to find_reco_matched() ! Not intended?" << std::endl;
		// return out_vector;
	}

	//take the TLV of that particle we want to match:
	TLorentzVector truth_part_tlv = getTLV_MC(truth_parts_to_match.at(0));

	//loop over all reco parts and find if there is one within the dr threshold to the truth part
	for (auto & check_reco_part: reco_parts_all){
		TLorentzVector check_reco_part_tlv = getTLV_reco(check_reco_part);
		float dR_val = truth_part_tlv.DeltaR(check_reco_part_tlv);

		if (dR_val <= dR_thres){
			out_vector.push_back(check_reco_part);
		}

		check_reco_part_tlv.Clear();

	}

	return out_vector;

}


//manual implementation of the delphes isolation criterion
ROOT::VecOps::RVec<float> AnalysisFCChh::get_IP_delphes(ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> test_parts, ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> reco_parts_all, float dR_min, float pT_min, bool exclude_light_leps){

	ROOT::VecOps::RVec<float> out_vector;

	if (test_parts.size() < 1){
		return out_vector;
	}

	for (auto & test_part: test_parts){
		//first get the pT of the test particle: 
		TLorentzVector tlv_test_part = getTLV_reco(test_part);
		float pT_test_part = tlv_test_part.Pt();

		float sum_pT = 0;

		//loop over all other parts and sum up pTs if they are within the dR cone and above min pT
		for (auto & reco_part : reco_parts_all){

			//check type of particle first:
			// std::cout << "PDG ID of particle:" << reco_part.m_particleIDUsed << std::endl;

			//exclude electrons and muons 

			TLorentzVector tlv_reco_part = getTLV_reco(reco_part);

			if (tlv_reco_part.Pt() > pT_min && tlv_test_part.DeltaR(tlv_reco_part) < dR_min ){
				sum_pT+= tlv_reco_part.Pt();
			}

			tlv_reco_part.Clear();

		}

		float IP_val = sum_pT/pT_test_part;

		out_vector.push_back(IP_val);

	}

	return out_vector;
}

//index navigation matching reco and MC particle to get pdg id of reco particle, following the ReconstructedParticle2MC::getRP2MC_pdg fuction in base FW
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::filter_lightLeps(ROOT::VecOps::RVec<int> recind,
					  ROOT::VecOps::RVec<int> mcind,
					  ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> reco,
					  ROOT::VecOps::RVec<edm4hep::MCParticleData> mc){

	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData>  out_vector;

	for (auto & reco_index : recind){

		std::cout << "Reco index:" << reco_index << std::endl;
		std::cout << "MC index:" << mcind.at(reco_index) << std::endl;

		//testing:
		auto pdg_id = mc.at(mcind.at(reco_index)).PDG;
		float mass = reco.at(reco_index).mass;

		std::cout << "MC PDG ID:" << pdg_id << std::endl;
		std::cout << "Reco mass:" << mass << std::endl;

	}

	return out_vector;

}

// muon mass: 0.105658
// electron mass: 0.000510999  ? 


//find the neutrinos that originate from a W-decay (which comes from a higgs, and not a b-meson) using the truth info -> to use for truth MET
ROOT::VecOps::RVec<edm4hep::MCParticleData> AnalysisFCChh::getNusFromW(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> parent_ids){

	ROOT::VecOps::RVec<edm4hep::MCParticleData> nus_list;

	//loop over all truth particles and find neutrinos from Ws that came from higgs (the direction tau->light lepton as child appears to be missing in the tautau samples)
	for (auto & truth_part: truth_particles) {
		if (isNeutrino(truth_part)){
			bool from_W_higgs = isChildOfWFromHiggs(truth_part, parent_ids, truth_particles);
			if(from_W_higgs){
				// counter+=1;
				nus_list.push_back(truth_part);
			}
		}
	}
	// std::cout << "Leps from tau-higgs " << counter << std::endl; 
	return nus_list;
}


//get truth met -> return as recoparticle so can use instead of reco met in other ftcs for some checks
ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> AnalysisFCChh::getTruthMETObj(ROOT::VecOps::RVec<edm4hep::MCParticleData> truth_particles, ROOT::VecOps::RVec<podio::ObjectID> parent_ids, TString type){

	ROOT::VecOps::RVec<edm4hep::ReconstructedParticleData> out_vector;

	ROOT::VecOps::RVec<edm4hep::MCParticleData> selected_nus;


	for (auto & truth_part: truth_particles) {
		if (isNeutrino(truth_part)){

			//sum only the neutrinos from different higgs decays if requested
			if ( type.Contains("hww_only") && isChildOfWFromHiggs(truth_part, parent_ids, truth_particles) ) {
				selected_nus.push_back(truth_part);
			}

			else if ( type.Contains("htautau_only") && isChildOfTauFromHiggs(truth_part, parent_ids, truth_particles) ) {
				// std::cout << "getting truth MET from taus only" << std::endl;
				selected_nus.push_back(truth_part);
			}

			else if ( type.Contains("hzz_only") && isChildOfZFromHiggs(truth_part, parent_ids, truth_particles) ) {
				selected_nus.push_back(truth_part);
			}

			else if (type.Contains("all_nu")) {
				selected_nus.push_back(truth_part);
			}

		}

	}

	//sum up
	TLorentzVector tlv_total; 

	for (auto & nu: selected_nus){
		TLorentzVector tlv_nu = getTLV_MC(nu);
		tlv_total += tlv_nu;

	}

	edm4hep::ReconstructedParticleData met_obj;
	met_obj.momentum.x = tlv_total.Px();
	met_obj.momentum.y = tlv_total.Py();
	met_obj.momentum.z = 0.;
	met_obj.mass = 0.;

	// std::cout << "Truth MET from building object: " << sqrt(met_obj.momentum.x*met_obj.momentum.x + met_obj.momentum.y*met_obj.momentum.y) << std::endl;


	out_vector.push_back(met_obj);

	return out_vector;

}

