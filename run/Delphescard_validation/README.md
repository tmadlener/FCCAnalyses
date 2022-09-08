#FCCAnalyses scripts to produce ntuples from the EDM4HEP files produced for the validation of new Delphes card 

## Setting up 

Check out this fork of the FCCAnalyses framework and switch to the branch called "FCChh_HH_analyses". Then compile the code as described in the main readme: 

```shell
source ./setup.sh
mkdir build install
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make install
cd ..
```

Recompiling is necessary when you make changes to the C++ based code in `/analyzers/dataframe/`. 

To run the Delphes card validation examples, switch to `/run/Delphescard_validation/`.

## Structure of the framework 
The FCCAnalyses framework makes use of RDataframe to allow for fast and multi-threaded analyses of the EDM4HEP events. It consists of C++ libraries found in `/analyzers/dataframe/` which implement all the work, e.g. actually accessing the EDM4HEP event info, such as lepton's 4-vector, and calculating other kinematic variables from it, such as an invariant mass. More details on how the EDM4HEP file is actually structured and how the reading works can be found in [this readme](https://github.com/HEP-FCC/FCCAnalyses/blob/basicexamples/examples/basics/README.md). 

## Running the examples
Different python scripts are then used to steer the different steps of the example analysis:
1. `analysis.py` contains the class `analysis` where all the variables to write out to the ntuple are defined. To run this locally on one of the test files just use:
	`python analysis.py -i <input_file> -o <out_dir>`. The script `run_signal_valid.sh` runs the analysis of all signal testers. 
2. `<signalname>_valid.py` then applies a simple filter for each signal, e.g. requiring two b-jets and two photons for bbyy, and produces plots of the testing variables - extra plots from existing branches of the ntuple can be added here. 

Please note that the examples here are simplified versions compared to and out-of-synch with the main FW repo. 

##Adding new variables to the ntuples 
To add a new variable/branch to the output ntuples check if a C++ function to calculate it exists, e.g. the stransverse mass mT2 calculation is available in `Analysis_FCChh`. If it does you just need to add a new line to the `analysis.py` RDF definition block, e.g. `Define("mT2", "AnalysisFCChh::get_mT2(<particles1>, <particles2>, <MET>)")`.

If it doesn't you can add the required calculation to the `Analysis_FCChh` (or make an additional C++ library) and recompile. 