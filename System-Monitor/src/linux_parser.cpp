#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
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
        int pid = stoi(filename);  string line, value;
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key , value;
  float memtotal;
  float memfree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value;
    if(key == "MemTotal:"){
      memtotal = stof(value);
    } else if (key == "MemFree:"){
      memfree = stof(value);
      }
    }
  return (memtotal - memfree)/memtotal;
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  string value;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    }
  return std::stol(value); 
  }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  long activejiffies;
  vector <string> jiffieslist;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0 ; i < 22 ; i++ ){
      linestream >> value;
      jiffieslist.push_back(value);
    }
    activejiffies = std::stol(jiffieslist[13]) + std::stol(jiffieslist[14]) + std::stol(jiffieslist[15]) + std::stol(jiffieslist[16]) ;

  }
  
   return activejiffies; 
   }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  
  string line, value;
  long activejiffies;
  vector <string> jiffieslist = LinuxParser::CpuUtilization();
  for (auto data : jiffieslist){
    activejiffies += stol(data);
  }
  activejiffies -= (LinuxParser::IdleJiffies() + stol(jiffieslist.back()));
  
  return activejiffies; }  
// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector <string> util;
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    for(int i =0 ;i < 10; i++) {
      linestream >> value;
      util.push_back(value);
    }
  }
  
  return util; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value;
    if(key ==  "processes")
      break;
    }
  return stoi(value); 
  }  
  
// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value;
    if(key ==  "procs_running")
      break;
    }
  return stoi(value); 
  }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    break;
  }
  return line; 
  }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value;
    if(key == "VmSize:")
      break; 
   }
  return to_string(stoi(value)/1024); 
  }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
      if(key == "Uid:"){ 
        linestream >> value;
        break;
      }
  }
  return value; 
  }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user, uid, x;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    std::getline(stream, line);
    std::replace(line.begin(),line.end(), ':', ' ');
    std::istringstream linestream(line);
    while(linestream >> user >> x >> uid)
      if(uid == LinuxParser::Uid(pid))
        break;
    return user;
    }
  }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  
  string line, value;
  vector <string> uptimelist;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0 ; i < 22 ; i++ ){ linestream >> value; }  
    }
  return stol(value)/sysconf(_SC_CLK_TCK); 
  }