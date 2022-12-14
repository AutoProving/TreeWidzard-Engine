#include "Flags.h"
Flags::Flags() {
	content.insert({"PrintStates", 0});		// print all states
	content.insert({"LoopTime", 0});		// time for loop
	content.insert({"CreateStateTime", 0}); // time to create a state
	content.insert({"StateSize", 0});		// prints size of state
	content.insert({"Premise", 0});
	content.insert({"NoBFSDAG", 0});
	content.insert({"WriteToFiles", 0});
}

float Flags::get(std::string s)
{
    if(content.find(s)!=content.end()){
        return content[s];
    }else{
        return -1;
    }
}

void Flags::add_flag(std::string s, float value) {
	auto it = content.find(s);
	if(it==content.end()){
        content.insert({s,value});
    }else{
	    content.erase(s);
        content.insert({s,value});
    }
}
