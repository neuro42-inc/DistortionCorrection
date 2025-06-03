#include "defines.h"
#include "DRBUDDI.h"
#include "DRBUDDI_parserBase.h"
#include "registration_settings.h"
#include "TORTOISE.h"


int main(int argc,char *argv[])
{
    TORTOISE t;
    DRBUDDI_PARSERBASE *parser= new DRBUDDI_PARSERBASE(argc,argv,1);

    std::string TORTOISE_loc = executable_path(argv[0]);         //get the location of the called executable
    std::string executable_folder= fs::path(TORTOISE_loc).parent_path().string(); //to access the settings folder

    int nc=1;
    if(parser->getNumberOfCores()!=0)
    {
        nc=parser->getNumberOfCores();
        std::cout<<"The number of threads read from command line:  "<<nc<<std::endl;
    }
    else
    {
        const char* env_p = std::getenv("OMP_NUM_THREADS");
        if(env_p)
        {
            nc=atoi(env_p)-1;
            std::cout<<"The number of threads from OMP_NUM_THREADS: "<<nc<<std::endl;
        }
        else
        {
            nc=(int)(getNCores() *0.5)-1;
            std::cout<<"The number of threads to use in DRBUDDI could not be automatically determined. "<<std::endl;
            std::cout<<"It is currently set to: " <<nc<<std::endl;
            std::cout<<"Please set the OMP_NUM_THREADS environmental variable to change this."<<std::endl;
        }
    }


/*
    int nc=1;
    if(fs::exists(system_settings_file))
    {
        json system_settings_json;
        std::ifstream system_settings_stream(system_settings_file);
        system_settings_stream >> system_settings_json;
        system_settings_stream.close();
        float perc=system_settings_json["PercentOfCpuCoresToUse"];
        nc=(int)(getNCores() *perc)-1;
        std::cout<<"The number of threads read from system_Settings file: "<<nc<<std::endl;
    }
    else
    {
        const char* env_p = std::getenv("OMP_NUM_THREADS");
        if(env_p)
        {
            nc=atoi(env_p)-1;
            std::cout<<"The number of threads from OMP_NUM_THREADS: "<<nc<<std::endl;
        }
        else
        {
            nc=(int)(getNCores() *0.5)-1;
            std::cout<<"The number of threads to use in DRBUDDI could not be automatically determined. "<<std::endl;
            std::cout<<"It is currently set to: " <<nc<<std::endl;
            std::cout<<"Please set the OMP_NUM_THREADS environmental variable to change this."<<std::endl;
        }
    }
*/

    if(nc==0)
        nc=1;
    itk::MultiThreaderBase::SetGlobalDefaultNumberOfThreads( nc);
    omp_set_num_threads(nc);
    if(parser->getDisableITKThreads())
    {
        itk::MultiThreaderBase::SetGlobalDefaultNumberOfThreads( 1 );
    }
    TORTOISE::SetNAvailableCores(nc);

    std::string up_name = parser->getUpInputName();
    std::string json_name= parser->getUpJSonName();
    if(json_name=="")
    {
        std::cout<<"The Json name is required for DRBUDDI. Exiting.."<<std::endl;
        return EXIT_FAILURE;
    }

    std::string down_name = parser->getDownInputName();
    std::vector<std::string> structural_names = parser->getStructuralNames();

    // json file used to indicate the PhaseEncodingDirection in which there is a large distortion
    json my_json;
    std::ifstream json_file(json_name);
    json_file >> my_json;
    json_file.close();


    RegistrationSettings::get().setValue<int>("DRBUDDI_DWI_bval_tensor_fitting",parser->getDWIBvalue());

    std::cout<<"myDRBUDDI Initialize.."<<std::endl;
    DRBUDDI myDRBUDDI(up_name,down_name,structural_names,my_json);

    std::cout<<"Start myDRBUDDI.SetParser().."<<std::endl;
    myDRBUDDI.SetParser(parser);

    std::cout<<"Start myDRBUDDI.Process().."<<std::endl;
    myDRBUDDI.Process();
}
