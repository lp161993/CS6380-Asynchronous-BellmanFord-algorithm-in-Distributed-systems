// Ryan Bagby
// Lakshmi Priyanka Selvaraj

/*
	CS6380 Distributed Computing
	Project 2 Part 2
	We are implementing the asynchronous bellman ford algorithm

*/

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <string>
#include<vector>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

mutex mtx;

queue<pair<int,int>> *vertex_dist;           // push vertex number and distance
int *c_p;
vector<vector<int>> neighbors_weight;                // Set of edges between nodes. //This is where i left off, trying to solve how to declare global 2d arrays
int num_threads;

int num_msgs          = 0;
int round_num;
int delay;
int *num_nghbrs;

std::random_device rd;                                  // Obtains a seed for the random number engine.
std::mt19937 gen(rd());                                 // Standard mersenne_twister_engine seeded with rd().
std::uniform_int_distribution<> distrib(1, 12);

void Asynch_Bellmanford(int vertex_no, int root) {
    
    int round_num      = 0;
	int dist	 	   = 1000; 				//setting the infinity distance to a really high value
	int parent;
	
	bool i_am_root = false;
	pair<int,int> incoming;
	int r;
	
	mtx.lock();
	
	mtx.unlock();
	
	if (vertex_no == root) {
		i_am_root = true;
		parent = -1;
		c_p[vertex_no] = -1;
		dist = 0;					//if the process is the root, set distance to 0
		//cout<<"I am root "<<vertex_no<<endl;
	}

    while (round_num < num_threads-1) {
		
		mtx.lock();
	    cout<<endl;//"I am vertex "<< vertex_no <<" and I am in round number " <<round_num <<endl;
		mtx.unlock();
		//sending distance to neighbors
		
        for (int h = 0; h < num_threads; h++) {
        	int weight = neighbors_weight[vertex_no][h];
        	if(weight>=1)
        	{	
        		/*mtx.lock();
        		cout<<"I am vertex "<< vertex_no <<" and I am in round number "<<round_num <<"weight for neighbor "<<h <<" is "<<weight  <<endl;
        		mtx.unlock();*/
				mtx.lock();
				//cout<<"I am vertex "<< vertex_no <<" and I am in round number " <<round_num <<"and i am sending msg to "<<h <<endl;
				delay = distrib(gen)*100;
        		sleep_for(nanoseconds(delay));
	        	vertex_dist[h].push(make_pair(vertex_no,dist));
	        	num_msgs++;
	        	mtx.unlock();
    		}    	
		}
		
        //perform relaxation step
        /*mtx.lock();
        cout<<"Incoming message of vertex "<< vertex_no <<" and I am in round number "<<round_num<<endl;
        sleep_for(milliseconds(5));
        incoming = vertex_dist[vertex_no].front();
        int a = get<0>(incoming);
        int b = get<1>(incoming);
        cout<<"Neigbhor : "<<a<< " sent distance "<< b <<endl;
        mtx.unlock();*/
        int x =0;
        /*mtx.lock();
        cout<<"I am vertex "<<vertex_no << " and no of neighbors: "<<num_nghbrs[vertex_no]<<endl;
        mtx.unlock();*/
        
        while(x <num_nghbrs[vertex_no])
        {	
	        while(vertex_dist->empty()){
	        	
	        	sleep_for(milliseconds(10));
	        }
	        mtx.lock();
	        incoming = vertex_dist[vertex_no].front();
        	int incoming_id 	= get<0>(incoming);
        	int incoming_dist   = get<1>(incoming); 
        	
        	vertex_dist[vertex_no].pop(); //verify
        	
        	int edge_weight = neighbors_weight[vertex_no][incoming_id];
        	int new_dist = min(dist, incoming_dist+edge_weight);
        	if(new_dist != dist)
        	{
        		parent = incoming_id;
        		dist = new_dist;
        		c_p[vertex_no] = parent;
			}
        	x++;
        	mtx.unlock();
        	
		}
		round_num++;
	}
	
	//So every node has now chosen its parent and the shortesy path. What we are trying to do here is
	// we want to inform their respective parents about the node id through vertex_dist
	//if i have a parent i inform saying, she is my parent
	//We consider the nodes with parents and send a message
	//sleep_for(milliseconds(50));
	/*while(!vertex_dist[vertex_no].empty()){
		mtx.lock();
		cout<<"Vertex "<<vertex_no<<" Children are ";
		incoming= vertex_dist[vertex_no].front();
		cout<< get<0>(incoming)<<" "<<endl;
		vertex_dist[vertex_no].pop();
		mtx.unlock();
	}*/
	sleep_for(milliseconds(25));
	mtx.lock();
	int q_size = sizeof(c_p);
	bool child_flag = false;
	if(!i_am_root){
		cout<<"Vertex number: "<<vertex_no <<" Shortest path to root: "<< dist <<" Parent: "<<parent <<" Children: ";	
	}
	else{
		cout<<"Vertex number: "<<vertex_no <<" Shortest path to root: "<< dist <<" I am root and " <<" Children: ";
	}
	
	for(int h=0; h < num_threads; h++){
		if (vertex_no == c_p[h])
		{	
			child_flag = true;
			cout<<h<<" ";
		}
	}
	if(!child_flag){
		cout<<"None";
	}
	cout<<endl;
	mtx.unlock();
	sleep_for(milliseconds(10));
	mtx.lock();
	if(!i_am_root){
		cout<<vertex_no <<" "<<dist<<" "<<parent<<" ";
	}
	else{
		cout<<vertex_no<<" "<<dist<<" i am root ";
	}
	for(int h=0; h < num_threads; h++){
		if (vertex_no == c_p[h])
		{	
			child_flag = true;
			cout<<h<<" ";
		}
	}
	if(!child_flag){
		cout<<"None";
	}
	cout<<endl;
	mtx.unlock();
        
}

int main(int argc, char* argv[]) {
    int i = 0;
    int data;
    int c;

    ifstream infile;
    infile.open("connectivity2.txt");                                   // This file contains the input data.
    infile >> data;                                             // This initial data contains the number of threads.
    num_threads = data;
	//dist = new int[num_threads];
	vertex_dist  = new queue<pair<int,int>>[num_threads];
	c_p = new int[num_threads];
	//children = new queue<int>[num_threads];

	infile >> data;   
	int root = data;
    int data_collector[num_threads*num_threads] = {};       // Temporary array to collect data from file.

    while (!infile.eof()) {                                     // This collects all remaining data: process id's and connectivity.
        infile >> data;
        data_collector[i++] = data;
    }
    infile.close();
    num_nghbrs = new int[num_threads];
    
    for (i = 0; i < num_threads; i++) {
        num_nghbrs[i] = 0; 
    }
    
	neighbors_weight.resize(num_threads);
	
	for(int h=0;h<num_threads;h++)
	{
		neighbors_weight[h].resize(num_threads);
	}

    c = 0;
    
    for (int k = 0; k < num_threads; k++) {            // Sort out id's from the connectivity data.
        for (int j = 0; j < num_threads; j++) {
                neighbors_weight[k][j]= data_collector[c];//we make a pair of the neighbor number along with its edge weight for a vertex
                if(data_collector[c]>=1){
                   num_nghbrs[k]++;	
				}
				
            	c++;
        	}
    	}
    
    for(int k=0; k<num_threads;k++)
    {
    	for(int j=0;j<num_threads;j++)
    	{
    		cout<<neighbors_weight[k][j] <<" ";
		}
		cout<<endl;
	}
	
	cout<<"-----------------------------------------------------------------------------------------"<<endl;
    
    thread threads[num_threads];
    for (i = 0; i < num_threads; i++) {                     // Create threads.
            threads[i] = thread(Asynch_Bellmanford, i, root);
    }
    for (i = 0; i < num_threads; i++) {
        threads[i].join();
    }
    cout << endl << "Total messages sent = " << num_msgs << endl;

   return 0;
}
