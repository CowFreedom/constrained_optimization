#include <iostream>
#include "parameter_estimation.h"
#include "parameters.h"


int main(){
	co::EVar<co::EFloat64> v_acetic(0.002,0.0000000001,200);
	co::EVar<co::EFloat64> v_general_decay_rate(0.0005,0.0000000001,200);
	co::EVarManager<co::EFloat64> vars;

	vars.add("Acetic",v_acetic);
	vars.add("general_decay_rate",v_general_decay_rate);
	std::string path="D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/estebis_downflow_pe";
	co::run_parameter_estimator(path,"",vars);
	//co::EFloat<double>(5);
	//std::cout<<var.min;
}