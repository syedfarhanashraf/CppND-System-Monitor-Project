#include <unistd.h>
#include <cctype>
#include <sstream>
#include <fstream>
#include "linux_parser.h"
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;


// Constructor -------------------
Process::Process(int pid) :  pid_ (pid) {
  
}

int Process::Pid() const {
    // TODO: Return this process's ID
    return pid_;
    
}

float Process::CpuUtilization()const {
    // TODO: Return this process's CPU utilization
  return float(LinuxParser::ActiveJiffies(pid_)) / float(LinuxParser::UpTime());
    
}

string Process::Command() const{
    // TODO: Return the command that generated this process

    return LinuxParser::Command(pid_);
    
}

// TODO: Return this process's memory utilization
string Process::Ram()const {
    // TODO: Return the command that generated this process in string

    return LinuxParser::Ram(pid_);
    
}

// TODO: Return the user (name) that generated this process
string Process::User() const{
    // TODO: Return the user (name) that generated this process

    return LinuxParser::User(pid_);
    
}


long int Process::UpTime()const {
   // TODO: Return the age of this process (in seconds)

    return LinuxParser::UpTime(pid_);
    
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    // less then operator comparison
    return CpuUtilization() < a.CpuUtilization();
    
}
