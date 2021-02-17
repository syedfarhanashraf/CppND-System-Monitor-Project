#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
using std::stol;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version ;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() {
    string line;
    string key;
     float memTotal = 1, memFree = 1;
     std::ifstream stream(kProcDirectory + kMeminfoFilename);
     if (stream.is_open()) {
       while (std::getline(stream, line)) {
         std::istringstream linestream(line);
         linestream >> key;
         if (key == "MemTotal:") {
           linestream >> memTotal;
         } else if (key == "MemAvailable:") {
           linestream >> memFree;
           break;
         }
       }
     }
     // return the system memory utilization
     return (memTotal - memFree) / memTotal;
    
}


long LinuxParser::UpTime() {
    
    long uptimeValue;
    string line;
    string s_uptime;
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream uptimeStream(line);
      uptimeStream >> s_uptime;
    }
    uptimeValue = std::stol(s_uptime);
    // return the system uptime
    return uptimeValue;
    
    
}


long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
    
    long totaltimeValue;
    long utime_ = 0;
    long cutime = 0;
    long cstime = 0;
    long stime_ = 0;
    string line;
    string value;
    vector<string> values;
    
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> value) {
        values.push_back(value);
      }
    }
    
    if (std::all_of(values[13].begin(), values[13].end(), isdigit))
      utime_ = stol(values[13]);
    if (std::all_of(values[14].begin(), values[14].end(), isdigit))
      stime_ = stol(values[14]);
    if (std::all_of(values[15].begin(), values[15].end(), isdigit))
      cutime = stol(values[15]);
    if (std::all_of(values[16].begin(), values[16].end(), isdigit))
      cstime = stol(values[16]);

    totaltimeValue = utime_ + cutime + stime_ + cstime;
    //return the number of active jiffies for a PID
    return totaltimeValue / sysconf(_SC_CLK_TCK);
        
}

long LinuxParser::ActiveJiffies() {
    
     auto jiff = CpuUtilization();
    
     long value = stol(jiff[CPUStates::kUser_]) + stol(jiff[CPUStates::kNice_]) +
            stol(jiff[CPUStates::kSystem_]) + stol(jiff[CPUStates::kIRQ_]) +
            stol(jiff[CPUStates::kSoftIRQ_]) +
            stol(jiff[CPUStates::kSteal_]);
    //return the number of active jiffies for the system
     return value;
}


long LinuxParser::IdleJiffies() {
    auto jiff = CpuUtilization();
    //return the number of idle jiffies for the system
    return stol(jiff[CPUStates::kIdle_]) + stol(jiff[CPUStates::kIOwait_]);
    
}


vector<string> LinuxParser::CpuUtilization() {
    string line;
    string cpu_str;
    string value;
    vector<string> jiffs;
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);

      linestream >> cpu_str;

      while (linestream >> value) {
        jiffs.push_back(value);
      }
    }
    //return CPU utilization
    return jiffs;

}


int LinuxParser::TotalProcesses() {
    
    int processValues;
    string key;
    string line;
    
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "processes") {
          linestream >> processValues;
          break;
        }
      }
    }
    //return the total number of processes
    return processValues;
    
}


int LinuxParser::RunningProcesses() {
    int processValues;
    string key;
    string line;
    
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "procs_running") {
          linestream >> processValues;
          break;
        }
      }
    }
    //return the number of running processes
    return processValues;
    
    
}


string LinuxParser::Command(int pid) {

    string commandStr;
    
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    if (stream.is_open()) {
      std::getline(stream, commandStr);
    }
    //return the command associated with a process
    return commandStr;
}


string LinuxParser::Ram(int pid) {
    
    long mem;
    string line;
    string m_Str;
    string key;
  

    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key;
        if (key == "VmSize:") {
          linestream >> mem;
          mem /= 1000;
          m_Str = std::to_string(mem);
          break;
        }
      }
    }
    //return the memory used by a process
    return m_Str;

}


string LinuxParser::Uid(int pid) {
    string line;
    string key;
    string uIdStr;
    
     std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
     if (stream.is_open()) {
       while (std::getline(stream, line)) {
         std::istringstream linestream(line);
         linestream >> key;
         if (key == "Uid:") {
           linestream >> uIdStr;
           break;
         }
       }
     }
    //return the user ID associated with a process
     return uIdStr;
}


string LinuxParser::User(int pid) {

    string uIdStr = Uid(pid);
    string p_id;
    string x;
    string temp_name;
    string line;
    string name = "DEFAULT";
     std::ifstream stream(kPasswordPath);
     if (stream.is_open()) {
       while (std::getline(stream, line)) {
         std::replace(line.begin(), line.end(), ':', ' ');
         std::istringstream linestream(line);

         linestream >> temp_name >> x >> p_id;
         if (p_id == uIdStr) {
             //set name
           name = temp_name;
           break;
         }
       }
     }
    //return the user associated with a process
     return name;

}


long LinuxParser::UpTime(int pid) {
    long timeToStart = 0;
    vector<string> values_;
    
    string line ;
    string value;
     
     std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
     if (stream.is_open()) {
       std::getline(stream, line);
       std::istringstream linestream(line);
       while (linestream >> value) {
         values_.push_back(value);
       }
        long upTimePid = LinuxParser::UpTime() - (timeToStart/sysconf(_SC_CLK_TCK));
         return upTimePid;
     }
    //return the uptime of a process
     return timeToStart;

}
