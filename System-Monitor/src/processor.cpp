#include "processor.h"
#include "linux_parser.h"
using std::string;
using std::stoi;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    // referred  from https://supportcenter.checkpoint.com/supportcenter/portal?eventSubmit_doGoviewsolutiondetails=&solutionid=sk65143
    string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest;
    string line;
    std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if(stream.is_open()){
        std::getline(stream,line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>steal >> guest ;

    }
    float totaltime = stoi(user) + stoi(nice) +stoi(system) + stoi(idle) + stoi(iowait) + stoi (irq) + stoi(softirq) + stoi(steal) + stoi(guest);
    return (totaltime - stoi(idle)) / totaltime; 
    
    }