#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"
#include <string>

int main(){

	co::EVarManager<co::EFloat64> vars;
	std::vector<std::string> names={"Acetic","MO_acetoM","offset","general_decay_rate","aceto_Methanogenesis","auco_weight"};
	std::string dir="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/estebis_downflow_march_cumulative_offset_auco_pso";
	std::vector<co::EFloat64> bounds={co::EFloat64(0.00001),co::EFloat64(1.0),co::EFloat64(0.0001),co::EFloat64(1.0),co::EFloat64(-300),co::EFloat64(300),co::EFloat64(0.000001),co::EFloat64(2),co::EFloat64(0.0001),co::EFloat64(5),co::EFloat64(0.0001),co::EFloat64(5) };

	co::NewtonOptions options;
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua");
	co::ParticleSwarmOptimizer<co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local>> pso(options,evaluator);
		
	pso.run<co::EFloat64>(names,bounds);


}