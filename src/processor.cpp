#include "processor.h"

#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    
    // add new supporting variables
    long total_old;
    long total_new;
    long active_new;
    long idle_old;
    long idle_new;
    
    total_new = LinuxParser::Jiffies();
    active_new = LinuxParser::ActiveJiffies();
    idle_new = LinuxParser::IdleJiffies();

    total_old = total_;
    idle_old =  idle_;
     
    // update
    Update(idle_new, active_new, total_new);

    float totalCalculation = float(total_new) - float(total_old);
    float idleCalculation = float(idle_new) - float(idle_old);

    float utilizationValue = (totalCalculation - idleCalculation) / totalCalculation;
    // return the value
    return utilizationValue;
    
    
}
void Processor::Update(long idle, long active, long total)
{
    //set Values
  idle_ = idle;
  active_ = active;
  total_ = total;
}
