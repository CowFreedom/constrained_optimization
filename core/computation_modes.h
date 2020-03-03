#pragma once

//Check if windows is installed
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iomanip>
#include <ctime>//optional. Could be removed if we change the way time  values are attained
#include <sstream>//

#elif __APPLE__
	#include <TargetConditionals.h>
    #if TARGET_OS_MAC
        // Other kinds of Mac OS
		#include <stdio.h>
		#include <stdlib.h>
		#include <unistd.h>
		#include <sys/types.h>
		#include <spawn.h>
		#include <sys/wait.h>
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <spawn.h>
	#include <sys/wait.h>
#elif __unix__ // all unices not caught above
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <spawn.h>
	#include <sys/wait.h>
#endif

#include "options.h"
#include "../evaluation_classes/evaluation.h"
#include "filewriter.h"
#include <thread>
//#include <filesystem>

namespace co{
			
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)


extern bool spawn_process_and_wait_to_join(std::string _command, std::string _working_directory); //forward declare function

/*Creates subdirectories. Horrible and inefficient function, should be replaced by filesystem::create_directories in C++17*/
void create_directories(const std::string& s, int start){
	for(int i=start;i<s.size();i++){
		if(s[i]=='/'||i==(s.size()-1)){
			std::string substr=s.substr(0,i+1);
			//std::cout<<substr<<"\n";
			CreateDirectoryA(substr.c_str(),NULL);//create folder in the given path (Windows OS function). second argument are security arguments
		}
	}
}

template<class T>
void evaluate_os(const std::string& folder_path, const std::string& command, typename std::vector<EVarManager<T>>::const_iterator start, size_t n, size_t id, size_t iter, std::string& message){
	Writer<T> writer;
	//std::cout<<"In evaluate os!\n";
	for (int i=0;i<n;i++){
	
		//Create directory
		std::string path=folder_path+"id_"+std::to_string(id)+"/eval_"+std::to_string(i)+"/";
		create_directories(path,0);
		//std::filesystem::create_directories(path);
		//write previous parameters
		//std::cout<<path<<"\n";
		writer.write(path,*(start+i),message);
		
		
		spawn_process_and_wait_to_join(command, path);
		//Spawn process
		
		
	}
	

	
}

bool spawn_process_and_wait_to_join(std::string _command, std::string _working_directory){
	
	LPSTR command=const_cast<LPSTR>(_command.c_str()); //for LPSTR we must lose const qualifier
	std::string dir=_working_directory;
//Create file handle to redirect console output to file
//See https://stackoverflow.com/questions/7018228/how-do-i-redirect-output-to-a-file-with-createprocess
   SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;       
	
    HANDLE h = CreateFile(_T(_working_directory.append("/console_output.log").c_str()),
        FILE_APPEND_DATA,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    //Create Processes
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	
//This is part of the redirection to console ->
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = NULL;
    si.hStdError = h;
    si.hStdOutput = h;
//-<

	DWORD flags = CREATE_NO_WINDOW;

    // Start the child process. 
    if( !CreateProcessA( NULL,   // No module name (use command line)
        command,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,          // Set handle inheritance to FALSE
        flags,              // No creation flags
        NULL,           // Use parent's environment block
        _T(dir).c_str(),           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return false;;
    }

    // Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );	
	return true;
}


class OSEvaluator{
	
	private:
	std::vector<STARTUPINFO> si;
	std::vector<PROCESS_INFORMATION> pi;
	
	public:
	OSEvaluator(int processes, std::string _evaluation_path):si(processes),pi(processes){
		if (processes>0){
		
		//Initialize process information
		for (int i=0;i<si.size();i++){
			ZeroMemory( &(si[i]), sizeof(si[i]) );
			si[i].cb = sizeof(si[i]);
			ZeroMemory( &(pi[i]), sizeof(pi[i]) );
		}
		}
		else{
			std::cerr<<"Error in OSEvaluator!\n";
		}
		
	}
	template<class T>
	void eval(std::string folder_path, typename std::vector<EVarManager<T>>::const_iterator start, size_t n, int id, int iter){
		
  
		std::cout<<"Evaluation Process started with with id: "<<id<<"\n";
		
		//std::cout<<"Created folder: "<<folder_path.c_str()<<"\n";
		create_directories(folder_path,0); //TODO: Replace by std::filesystem of C++17
		
		
		
	//	std::cout<<"Did it work? "<<ret<<"\n";
	}

	
};
/*UNIX VERSIONS*/	
#elif __APPLE__ || __linux__ || __unix__

class OSEvaluator{
	
	private:
	std::vector<pid_t> pid;
	std::vector<int> status;
	
	public:
	OSEvaluator(int processes, std::string _evaluation_path):si(processes),pi(processes){
		if (processes>0){

		}
		else{
			std::cerr<<"Error in OSEvaluator!\n";
		}
		
	}
	template<class T>
	void eval(std::string folder_path, typename std::vector<EVarManager<T>>::const_iterator start, size_t n, int id, int iter){
		
  
		std::cout<<"Evaluation Process started with with id: "<<id<<"\n";
		
		//std::cout<<"Created folder: "<<folder_path.c_str()<<"\n";
		create_directories(folder_path,0); //TODO: Replace by std::filesystem of C++17
		
		
		
	//	std::cout<<"Did it work? "<<ret<<"\n";
	}

#endif
	
	
	// size_t id, size_t iter, std::string& message
	/*M: Computation mode (local, cluster etc.)
	E: EvaluationClass (e.g. BiogasEvaluation)
	T: Float type (EFLoat64 etc)
	Note: I think E and T could be coupled as E contained information about T
	*/
	template<ConfigComputation M, class E, class T>
	class ComputationMode{
		public:
		const ConfigComputation config_computation;
		const ConfigOutput config_output; //determines, if f will be invoked through cmd or function pointers directly
		
		std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& input, std::string message="");
	};
	
	
	template<class E, class T>
	class ComputationMode<ConfigComputation::Local,E,T>{
		
	private: 
	const ConfigComputation config_computation;
	const ConfigOutput config_output; //determines, if f will be invoked through cmd or function pointers directly
	const int thread_count;
	const std::string table_directory;
	E* evaluation_class; //Pointer back to the evaluationclass (e.g. BiogasEvaluation)
	size_t iter=0; //current iteration of the evaluations
	std::string evaluation_path; //path where the evaluations will be stored (if evaluations are written to file)
	

	public:
	
	ComputationMode(E* e, ConfigOutput _config_output, int _thread_count, std::string _table_directory):evaluation_class(e),config_computation(ConfigComputation::Local),config_output(_config_output),table_directory(_table_directory), thread_count(_thread_count){
		std::cout<<"Pointer address inside at creation:"<<evaluation_class<<"\n";
		if (thread_count<=0){
			std::cerr<<"ComputationMode error: Threadcount less than zero"; //TODO Assert statement
			std::cin.get();
		}
		
		//Create timestring consisting of the current time

		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%d-%m-%Y_%Hh-%Mm-%Ss");

		//initialize evaluation path
		evaluation_path=table_directory+"/evaluations/"+oss.str();
		
	}
	
	std::string get_current_evaluation_path() const{
		return evaluation_path;
	}
	
	size_t get_current_iteration() const{
		return iter;
	}
	
	//Evaluate the problem with the given input
	std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& input,std::string message=""){
		std::vector<std::vector<T>> res;
		
		switch(config_output){
			
			/*Choosing this options means:
			*Parameters are written to file
			*Evaluation will take place via an evaluate.lua file.*/

			case ConfigOutput::File:
			{
				std::cout<<"Bin in  eval::ConfigComputation File!\n";
				res= schedule_and_eval_file(input,message,iter,"",true);
				break;
			}
			
		}
		
		std::cout<<"done evaluation\n";
		return res;
	}
	
	//Evaluate a specific iteration and store into the folder name 
	std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& input,std::string& folder_name, int _iter,std::string message=""){
		std::vector<std::vector<T>> res;
		
		switch(config_output){
			
			/*Choosing this options means:
			*Parameters are written to file
			*Evaluation will take place via an evaluate.lua file.*/

			case ConfigOutput::File:
			{
				res= schedule_and_eval_file(input,message,_iter,folder_name,false);
				break;
			}
			
		}
		
		std::cout<<"done specific evaluation\n";
		return res;
	}	
	
	/*Schedulees processes and evaluates them according to OS process methods
	Evaluations are stored in the same order as the input files, i.e. input[i] corresponds to eval[i]
	*/
	std::vector<std::vector<T>> schedule_and_eval_file(const std::vector<EVarManager<T>>& input,std::string& message,int _iter,std::string folder_name,bool increase_iter){
			
				


		
		//Scheduling the processes
		size_t n=input.size(); //number of needed evaluations
		auto start=input.begin();
		auto end=input.end();
		size_t id=0;
		
	//	OSEvaluator os(thread_count,evaluation_path);//Create Processes
		std::vector<std::thread> t(n);//Create the threads
		std::vector<size_t> evals_per_thread(n);
		std::string folder_path=evaluation_path+"/iteration_"+std::to_string(_iter)+"/"+folder_name;
		std::string shell_command="ugshell -ex "+table_directory+"/evaluate.lua";
		std::cout<<"Shell command: "<<shell_command<<"\n";
		if (n<thread_count){
			for (auto& x: input){
				t[id]=std::thread(evaluate_os<T>,folder_path,shell_command, start+id,1,id,_iter,std::ref(message));
				evals_per_thread[id]=1;
				//os.eval<T>(folder_path+std::to_string(id),start+id,1,id,iter);
				id++;
			}
		}
		else{
			size_t m=n/thread_count;
			size_t rem=n%thread_count;
			size_t id=0;
			
			for (size_t j=0;j<thread_count;j++){
				//os.eval<T>(folder_path+std::to_string(id),start+j*m,m,id,iter,message);
				id++;
			}
			
			if (rem != 0){
				//os.eval<T>(folder_path+std::to_string(id),end-rem,rem,id,iter,message);
			}
			
		}
		
		//Wait for all threads to finish
		//std::cout<<"Waiting on join\n";
		for (auto& x:t){
			x.join();
		}
		std::cout<<"Wir fangen an\n";
		//Parse results;
		std::vector<std::vector<T>> result(n);
		std::vector<int> rows(n);
		
		//std::cout<<"Pointer address inside:"<<evaluation_class<<"\n";
		//std::cout<<"Size targettimes inside:"<<(*evaluation_class).target_times.size()<<"\n";

		int ids=0;
		for (int i=0;i<id;i++){
				for (int j=0;j<evals_per_thread[i];j++){
					std::string data_path=folder_path+"id_"+std::to_string(i)+"/eval_"+std::to_string(j)+"/";
					(*evaluation_class).parse(data_path,result[ids]);	
					ids++;
				}
		}	
		if(increase_iter==true){
			iter++;
		}
		return result;
		
	}
	
	
	
	};
	
}