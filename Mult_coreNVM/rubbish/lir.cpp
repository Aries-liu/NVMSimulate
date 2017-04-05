#include<iostream>
#include<fstream>
#include<unistd.h>
#include<time.h>

using namespace std;


bool endswith(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main(){
	ifstream file("file.log");
	string line_r;
	while(1){
	struct timespec time_start,time_end;
	clock_gettime(CLOCK_REALTIME, &time_start);
	getline(file,line_r);
	cout<<line_r<<endl;
	cout<<line_r.length()<<endl;
	clock_gettime(CLOCK_REALTIME, &time_end);
	long time = (100000000-(time_end.tv_nsec-time_start.tv_nsec))/1000;
	usleep(time);
	}
}
