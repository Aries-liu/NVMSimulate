#include<iostream>
#include<fstream>
#include<unistd.h>

using namespace std;


bool endswith(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main(){
	ofstream file("file.log");
	string line_r;
	while(1){
	file<<"lallalaa"<<endl;
	sleep(0.1);
	}
}
