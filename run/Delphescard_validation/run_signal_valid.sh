#! /bin/bash

#run the analysis.py directly for each of the four signal samples:

python analysis.py -i /eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbbb_tester_new_card.root -o /eos/user/b/bistapf/FCChh_EvtGen/FCCAnalysis_ntuples/
python analysis.py -i /eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbWW_tester_new_card.root -o /eos/user/b/bistapf/FCChh_EvtGen/FCCAnalysis_ntuples/
python analysis.py -i /eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbtautau_tester_new_card.root -o /eos/user/b/bistapf/FCChh_EvtGen/FCCAnalysis_ntuples/
python analysis.py -i /eos/user/b/bistapf/FCChh_EvtGen/pwp8_pp_hh_5f_hhbbyy_tester_new_card.root -o /eos/user/b/bistapf/FCChh_EvtGen/FCCAnalysis_ntuples/