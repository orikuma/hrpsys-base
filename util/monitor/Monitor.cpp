#include <rtm/CorbaNaming.h>
#include "Monitor.h"
#include "util/OpenRTMUtil.h"
#include "GLmodel.h"

static int threadMain(void *arg)
{
    Monitor *monitor = (Monitor *)arg;
    while(monitor->run());
}

Monitor::Monitor(CORBA::ORB_var orb, const std::string &i_hostname,
                 int i_port, int i_interval) :
    m_orb(orb),
    m_flagExit(false),
    m_rhCompName("RobotHardware0"),
    m_shCompName("StateHolder0"),
    m_interval(i_interval)
{
    char buf[128];
    sprintf(buf, "%s:%d", i_hostname.c_str(), i_port);
    RTC::CorbaNaming naming(orb, buf);
    m_naming = CosNaming::NamingContext::_duplicate(naming.getRootContext());
}

void Monitor::start()
{
    m_thread = SDL_CreateThread(threadMain, (void *)this);
}

void Monitor::stop()
{
    m_flagExit = true;
    SDL_WaitThread(m_thread, NULL);
}

bool Monitor::run()
{
    // RobotHardwareService
    if (CORBA::is_nil(m_rhService)){
        try{
            CosNaming::Name name;
            name.length(1);
            name[0].id = CORBA::string_dup(m_rhCompName.c_str());
            name[0].kind = CORBA::string_dup("rtc");
            CORBA::Object_var obj = m_naming->resolve(name);
            RTC::RTObject_var rtc = RTC::RTObject::_narrow(obj);
            RTC::ExecutionContextList_var eclist = rtc->get_owned_contexts();
            for(CORBA::ULong i=0; i < eclist->length(); ++i){
                eclist[i]->activate_component(rtc);
            }
            const char *ior = getServiceIOR(rtc, "RobotHardwareService");
            m_rhService = OpenHRP::RobotHardwareService::_narrow(m_orb->string_to_object(ior));
        }catch(...){
        }
    }
    // StateHolderService
    if (CORBA::is_nil(m_shService)){
        try{
            CosNaming::Name name;
            name.length(1);
            name[0].id = CORBA::string_dup(m_shCompName.c_str());
            name[0].kind = CORBA::string_dup("rtc");
            CORBA::Object_var obj = m_naming->resolve(name);
            RTC::RTObject_var rtc = RTC::RTObject::_narrow(obj);
            const char *ior = getServiceIOR(rtc, "StateHolderService");
            m_shService = OpenHRP::StateHolderService::_narrow(m_orb->string_to_object(ior));
        }catch(...){
        }
    }

    bool stateUpdate = false;
    if (!CORBA::is_nil(m_rhService)){
        OpenHRP::RobotHardwareService::RobotState_var rs;
        try{
            m_rhService->getStatus(rs);
            m_rstate.state = rs;
            stateUpdate = true;
        }catch(...){
            std::cout << "exception in getStatus()" << std::endl;
            m_rhService = NULL;
        }
    }

    if (!CORBA::is_nil(m_shService)){
        OpenHRP::StateHolderService::Command_var com;
        try{
            m_shService->getCommand(com); 
            if (m_rstate.state.command.length() != com->jointRefs.length()){
                m_rstate.state.command.length(com->jointRefs.length());
            }
            memcpy(m_rstate.state.command.get_buffer(),
                   com->jointRefs.get_buffer(),
                   sizeof(double)*com->jointRefs.length());
            stateUpdate = true;
        }catch(...){
            std::cout << "exception in getCommand()" << std::endl;
            m_shService = NULL;
        }
    }


    if (stateUpdate) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        m_rstate.time = tv.tv_sec + tv.tv_usec/1e6; 
        GLscene::getInstance()->addState(m_rstate);
    }
    usleep(1000*m_interval);

    return !m_flagExit;
}

bool Monitor::isConnected()
{
    return !CORBA::is_nil(m_rhService);
}