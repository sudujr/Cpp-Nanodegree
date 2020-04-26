#include <string>

#include "format.h"

using std::string;
using std::to_string;
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
     int hours = (seconds / 3600);
     int lessthanhr = seconds % 3600;
     string sign = ":";
     int minutes = (lessthanhr/60);
     int second = (lessthanhr % 60);
     string output = to_string(hours) + sign + to_string(minutes) + sign + to_string(second);
     return output; 
     
}
