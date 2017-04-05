#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <boost/regex.hpp>
#include <unistd.h>
#include <time.h>

using namespace std;

#define WDELAY 1.3
#define RDELAY 0.2

const string fpath_r_RW = "ReadWrite.log";
const string fpath_r_EV =  "unc_h.log";
const string fpath_w = "/sys/kernel/kobject_NVM/nvm";
const string fsyn = "syncount.log";

static long writeCount[10]={0};
static long readCount[10]={0};
static long long delay[10]={0};
static long eviction =0;
static double syncount = 0;
static boost::regex re(",");

static ifstream fp_r_rw;
static ifstream fp_r_ev;
static ofstream fp_w;
static ofstream fp_wsyn;

bool endswith(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void get_rw(int coreNUM)
{
    std::ios::sync_with_stdio(false);
    int coreOnLine=coreNUM; 
    while(coreOnLine>0){
        int count =2;
        while(count>0){
            string line_r;
            getline(fp_r_rw,line_r);
	    //cout<<line_r<<endl;
            if(endswith(line_r,"dram")||endswith(line_r,")")){
                int core_number=coreNUM-coreOnLine;
                char *p;
                char *tokenptr = strtok_r(const_cast<char*>(line_r.c_str()), " ", &p);
                char *cou;
                for (int i = 0; i < 4; i++){
                    cou = tokenptr;
                    tokenptr = strtok_r(NULL, " ", &p);
                }
                string num=cou;
                if (count==2)
                    writeCount[core_number]=stol(boost::regex_replace(num,re,""));
                else
                    readCount[core_number]=stol(boost::regex_replace(num,re,""));
                count--;
                continue;
                }
            else if(!endswith(line_r,"events")){
                    int x = - line_r.length();
                    fp_r_rw.seekg(x, ios::cur);
		    //sleep(1);
                }
        }
        coreOnLine--;
    }

}

void get_evc()
{
    std::ios::sync_with_stdio(false);
    int count=2;
    while(count>0){
        string line_r;
        getline(fp_r_ev,line_r);
	cout<<line_r<<endl;
        if(endswith(line_r,"remote_0")){
            char *p;
            char *tokenptr = strtok_r(const_cast<char*>(line_r.c_str()), " ", &p);
            char *cou;
            for (int i = 0; i < 2; i++){
                cou = tokenptr;
                tokenptr = strtok_r(NULL, " ", &p);
            }
            string num=cou;
            eviction=stol(boost::regex_replace(num,re,""));
            count--;
            continue;
        }
        else if(endswith(line_r,"remote_1")){
            count--;
            continue;
        }
        else if(!endswith(line_r,"events")){
            int x = - line_r.length();
            fp_r_rw.seekg(x, ios::cur);
	    //usleep(10);
        }

    }

}

void count_delay(int coreNUM)
{
    long through_count = 0;
    for(int i=0;i<coreNUM;i++)
        through_count+=writeCount[i];
    long dirty_eviction = eviction-through_count;
    for(int i=0;i<coreNUM;i++){
        if (dirty_eviction>0)
            delay[i]=(writeCount[i] * WDELAY + readCount[i] * RDELAY + dirty_eviction * WDELAY / coreNUM );
        else
            delay[i] = readCount[i] * RDELAY;
	//cout<<"write = "<<writeCount[i]<<"---"<<"read = "<<readCount[i]<<endl;
	//cout<<"eviction = "<<eviction<<"---"<<"dirty_eviction = "<<dirty_eviction<<endl;
	//cout<<"delay "<<i<<" "<<delay[i]<<endl<<endl;
    }
}

void sendop(int coreNUM)
{
    std::ios::sync_with_stdio(false);
    for(int i=0;i<coreNUM;i++){
        fp_w<<i<<delay[i]<<endl;
    }
	cout<<"sebdop done "<<endl;


}

int main(int argc, char *argv[])
{
    fp_r_rw.open(fpath_r_RW,ios::in|ios::app);
    if(!fp_r_rw.is_open())
        cout<<"rw_fail open."<<endl;
    fp_r_ev.open(fpath_r_EV);
    if(!fp_r_rw.is_open())
        cout<<"evc_fail open."<<endl;
    fp_w.open(fpath_w);
    if(!fp_w.is_open())
        cout<<"w_fail open."<<endl;
    fp_wsyn.open(fsyn);
    if(!fp_wsyn.is_open())
        cout<<"syn_fail open."<<endl;

    int coreNUM = atoi(argv[1]);
    struct timespec time_start,time_end;

    while(1){
	clock_gettime(CLOCK_REALTIME, &time_start);
	//cout << "in loop"<<endl;
        get_rw(coreNUM);
	//cout<<"rw done"<<endl;
        get_evc();
	//cout<<"evc done"<<endl;
        count_delay(coreNUM);
	//cout<<"count delay done"<<endl;
        sendop(coreNUM);
        syncount+=0.1;
        fp_wsyn<<syncount<<endl;
	clock_gettime(CLOCK_REALTIME, &time_end);
	//long time = (150000000-(time_end.tv_nsec-time_start.tv_nsec))/1000;
	sleep(0.1);
    }
}
