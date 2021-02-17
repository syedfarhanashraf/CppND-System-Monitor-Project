#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds

string Format::ElapsedTime(long seconds) {
    
   int h, m;
    h = seconds / 3600;
    seconds = seconds % 3600;
    m = seconds / 60;
    seconds = seconds % 60;
    
    // OUTPUT: HH:MM:SS
    return std::to_string(h) + ":" + std::to_string(m) + ":" + std::to_string(seconds);
           
    
}
