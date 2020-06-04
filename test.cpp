#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <iostream>
#include <mutex>
#include <atomic>
#define indexx 100
using namespace std;

sem_t semaphore;
sem_t finished_semaphore;
unordered_map<string,mutex> m;

//vector<mutex> m;
int total_eq=0;
int cnt=0;
volatile bool des=true;

struct job{
	string start_value;
	vector<string> var;
	vector<string> numbers;
};

// $1
void calculate_function(vector<job> &job_list,unordered_map<string,int> &rec,atomic<bool>& flag){
   //for(int i=0;i<indexx;i++){
    
    while(flag){
    sem_wait(&semaphore);
    	if(!job_list.empty()){
            job tmp=job_list.front();
            int sum=rec[tmp.numbers.front()];

            for(int i=1;i<tmp.numbers.size();i+=2){
                string fir,sec;
                fir=tmp.numbers[i];
                sec=tmp.numbers[i+1];
                if(fir=="+"){
                    if(sec[0]=='$'){
                        sum=sum+rec[sec];
                    } 
                    else{
                        int string_to_int;
                        stringstream ss;
                        ss<<sec;
                        ss>>string_to_int;
                        sum = sum + string_to_int;
                    } 
                }
                else{
                    if(sec[0]=='$') sum=sum-rec[sec];
                    else{
                        int string_to_int;
                        stringstream ss;
                        ss<<sec;
                        ss>>string_to_int;
                        sum = sum - string_to_int;
                    }
                }
            }
            rec[tmp.start_value]=sum;
            job_list.erase(job_list.begin());
       // }
    sem_post(&finished_semaphore);
    sem_post(&semaphore);
    }

    }
    return ;
}

int main(int argc, char *argv[]){

	int t;
	string equation;
	unordered_map<string,int> rec;
	vector<job> job_list;
    vector<thread> threads;
    atomic<bool> flag(true);

	cin>>t; // number of variables
	sem_init(&semaphore,0,0); // init semaphore
	sem_init(&finished_semaphore,0,0);

 	// construct thread of numbers of argv[1]
	for(int i=1;i<=atoi(argv[1]);i++){
		threads.push_back(thread(calculate_function,ref(job_list),ref(rec),ref(flag)));
	} 

	// init variables
	for(int i=0;i<t;i++){
		int tmp;
		string tmp2="$",tmp3;
		stringstream ss;
		cin>>tmp;
		ss<<i;
		ss>>tmp3;
		tmp2 += tmp3;
		rec[tmp2]=tmp;
		m[tmp2];
	} 

	//read equation
	cin.ignore();
	while(getline(cin,equation) && equation!=""){
		++total_eq;
		job tmp_job;
		stringstream ss;
		string tmp,tmp2;
		ss<<equation;
		ss>>tmp_job.start_value>>tmp; // get varible left = and "="
		while(ss>>tmp2){
			tmp_job.numbers.push_back(tmp2);
			if(tmp2[0]=='$') tmp_job.var.push_back(tmp2);
		}
		job_list.push_back(tmp_job);
	}
	
    sem_post(&semaphore);
    while(cnt!=total_eq){
        sem_wait(&finished_semaphore);
        ++cnt;
    }

	//write in output file
	ofstream fout(argv[2]);
	for(int i=0;i<t;i++){
		stringstream ss;
		string tmp,index="$";
		ss<<i;
		ss>>tmp;
		index+=tmp;
		fout<<index<<" = "<<rec[index]<<endl;
	}
	fout.close();

    //cout<<"here"<<endl;
    des=false;
    flag=false;

    for(int i=0;i<threads.size();i++){
        threads[i].detach();
    }

    //cout<<"here"<<endl;



	return 0;
}