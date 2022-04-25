//  Created by thuzmh on 16/7/20.
//  Copyrigt thuzmh, Hsiao-Lun Wang All rights reserved.

#include <errno.h>
#include <signal.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <time.h>
#include <numeric> //get the sum of the data in a vector
#include "circuit.h"
#define hostID 0
#define INF 999
using namespace std;

float CIRCUIT::libDelay(unsigned ID, bool ris)
{
    int intDelay;
    float cap = gatelist[ID]->cap, theDelay;
    unsigned i,type = gatelist[ID]->type - 2;
    
    for(i=0; i<7; i++) if(index[i] > cap) break;
    if(ris)
    {
        if(i==0) theDelay = rising[type][0];
        else if(i==7) theDelay = rising[type][6];
        else theDelay = rising[type][i-1] + ( rising[type][i]-rising[type][i-1] ) * (cap-index[i-1]) / (index[i]-index[i-1]);
    }
    else
    {
        if(i==0) theDelay = falling[type][0];
        else if(i==7) theDelay = falling[type][6];
        else theDelay = falling[type][i-1] + ( falling[type][i]-falling[type][i-1] ) * (cap-index[i-1]) / (index[i]-index[i-1]);
    }	
    intDelay = int(theDelay*100.0);
    return intDelay;
}
void CIRCUIT::gateDelay(unsigned delay_type)
{
    unsigned i, j, who;
    float cap;
    if(delay_type==UNIT)
    { for(i=inNum+outNum; i<gateNum; i++) gatelist[i]->rise_delay = gatelist[i]->rise_delay = 1.0; }
    else if (delay_type==FANOUT)
    { for(i=inNum+outNum; i<gateNum; i++) gatelist[i]->rise_delay = gatelist[i]->rise_delay = gatelist[i]->fanoutNum + gatelist[i]->fanoutNum + 10; }
    else if (delay_type==LIB || delay_type==LIB2)
    {    //cout<<"circuit:50\n"<<endl;
        for(i=1; i<=gateNum; i++)
        {    //cout<<"gateNum "<< i <<endl;
            //cout<<"totoalnum"<< gateNum-i<<endl;
            who = topo[gateNum-i];
            //cout<<"who"<< who <<endl;
            switch(gatelist[who]->type)
            {
                case T_BUFF: cap = 0.002295; break;
                case T_NOT:  cap = 0.003543; break;
                case T_AND:  cap = 0.001925; break;
                case T_NAND: cap = 0.004120; break;
                case T_OR:   cap = 0.002469; break;
                case T_NOR:  cap = 0.004398; break;
                default: cap = 0.002295;
            }
            for(j=0; j<gatelist[who]->faninNum; j++)
            { gatelist[gatelist[who]->fanin_arr[j]]->cap += cap;
                //cout<<"who："<< who<<endl;
            }
        }
        //cout<<"circuit:69"<<endl;
        float ind[7]={0.00035, 0.021, 0.0385, 0.084, 0.147, 0.231, 0.3115},
        /*	ris[6][7]={{0.058429, 0.154015, 0.233371, 0.438815, 0.722959, 1.101685, 1.464589},
         {0.026516, 0.120965, 0.200257, 0.405724, 0.689867, 1.068599, 1.431505},
         {0.083615, 0.183546, 0.263102, 0.468877, 0.753191, 1.132012, 1.494964},
         {0.031883, 0.127281, 0.206766, 0.412383, 0.696586, 1.075342, 1.438261},
         {0.065263, 0.161547, 0.240986, 0.446600, 0.730800, 1.109557, 1.472478},
         {0.041322, 0.183475, 0.301784, 0.607982, 1.031287, 1.595432, 2.135987}},
         fall[6][7]={{0.080733, 0.144402, 0.187918, 0.297753, 0.449225, 0.651116, 0.844572},
         {0.017116, 0.065008, 0.107102, 0.216467, 0.367865, 0.569716, 0.763154},
         {0.102516, 0.173053, 0.218648, 0.329728, 0.481348, 0.683272, 0.876742},
         {0.021720, 0.077380, 0.126169, 0.252918, 0.428354, 0.662250, 0.886393},
         {0.120981, 0.197519, 0.245830, 0.359520, 0.511701, 0.713703, 0.907198},
         {0.021135, 0.069864, 0.112028, 0.221439, 0.372857, 0.574719, 0.768160}};
         */
        ris[6][7]={{0.069975, 0.164839, 0.244184, 0.449767, 0.733984, 1.112753, 1.475679},
            {0.042294, 0.138571, 0.217664, 0.423177, 0.707420, 1.086222, 1.449168},
            {0.095398, 0.194742, 0.274248, 0.480066, 0.764429, 1.143288, 1.506259},
            {0.048513, 0.144237, 0.223716, 0.429608, 0.714028, 1.092923, 1.455912},
            {0.077740, 0.173297, 0.252745, 0.458457, 0.742735, 1.121539, 1.484479},
            {0.058400, 0.197566, 0.315782, 0.622307, 1.045885, 1.610212, 2.150863}},
        fall[6][7]={{0.099433, 0.162980, 0.206508, 0.316340, 0.467813, 0.669709, 0.863172},
            {0.020378, 0.081669, 0.123690, 0.233021, 0.384408, 0.586254, 0.779689},
            {0.120674, 0.191027, 0.236641, 0.347734, 0.499356, 0.701282, 0.894753},
            {0.025964, 0.092110, 0.140866, 0.267597, 0.443032, 0.676928, 0.901072},
            {0.137021, 0.213456, 0.261764, 0.375462, 0.527647, 0.729651, 0.923148},
            {0.026827, 0.085867, 0.128031, 0.237477, 0.388914, 0.590786, 0.784234}};
        
        for(i=0; i<6; i++){index[i]=ind[i];
        
            for(j=0; j<7; j++)
            {
                rising[i][j]  = ris[i][j];
                falling[i][j] = fall[i][j];
            }
        }
        for(i=inNum+outNum+in_latch+out_latch; i<gateNum; i++)
        {
            //gatelist[i].delay  = int(100*libDelay(i, 0));
            //gatelist[i].delay1 = int(100*libDelay(i, 1));
            
            gatelist[i]->rise_delay  =libDelay(i, 0);
            gatelist[i]->fall_delay =libDelay(i, 1);
            /*if(delay_type==LIB2)
             {
             if(gatelist[i].delay > gatelist[i].delay1) gatelist[i].delay1 = gatelist[i].delay;
             else gatelist[i].delay = gatelist[i].delay1;
             }*/
            // by alan wang
            /*switch(gatelist[i].type)
             {
             case T_BUFF: gatelist[i].delay1 = gatelist[i].delay1*1.1; gatelist[i].delay = gatelist[i].delay*1.3; break;
             case T_NOT:  gatelist[i].delay1 = gatelist[i].delay1; gatelist[i].delay = gatelist[i].delay*1.1; break;
             case T_AND:  gatelist[i].delay1 = gatelist[i].delay1*1.2; gatelist[i].delay = gatelist[i].delay*1.2; break;
             case T_NAND: gatelist[i].delay1 = gatelist[i].delay1*1.1; gatelist[i].delay = gatelist[i].delay; break;
             case T_OR:   gatelist[i].delay1 = gatelist[i].delay1; gatelist[i].delay = gatelist[i].delay*1.2; break;
             case T_NOR:  gatelist[i].delay1 = gatelist[i].delay1*1.3; gatelist[i].delay = gatelist[i].delay*1.1; break;
             default: gatelist[i].delay = 0;
             }*/	
            
            if(gatelist[i]->rise_delay > gatelist[i]->fall_delay) gatelist[i]->fall_delay = gatelist[i]->rise_delay;
                else gatelist[i]->rise_delay = gatelist[i]->fall_delay;
            }	
        }
    for(unsigned i=0;i<inNum+outNum+out_latch+in_latch;i++){
        gatelist[i]->rise_delay =0;
        gatelist[i]->fall_delay =0;
    }
    //cout<<"delay is finish.\n";
}
void CIRCUIT::topological()
{
	unsigned i,now;
	stack <unsigned> sorter;
    //unsigned gateNum = (unsigned)gatelist.size();
	unsigned * faninNum;
	topo  = new unsigned[gateNum];
    
	faninNum = new unsigned[gateNum];	
	for (i = 0; i < gateNum ; i++)
	{
		faninNum[i] = gatelist[i]->faninNum;
		gatelist[i]->level = 0;
		//if(gatelist[i]->type == INPUT||gatelist[i]->type == LATCH_IN)
			//faninNum[i] = 0;
	}
    //cout<<"inNum="<<inNum<<endl;
    //cout<<"in_latch="<<in_latch<<endl;
	for (i = 0; i < gateNum ; i++)
	{
		if(!faninNum[i])
		{
			sorter.push(i);
			gatelist[i]->level = 0;
			//topo[i] = i;
		}
	}
	unsigned out_gate;
	i=0;
	while(sorter.size())
	{
		now = sorter.top();
        //cout<<"the choice="<<now<<endl;
		sorter.pop();
		topo[i] = now;
		i++;
        //cout<<"gatelist "<<now<<"'s fanoutNum="<<gatelist[now]->fanoutNum<<endl;
		for(unsigned j = 0; j < gatelist[now]->fanoutNum;j++)
		{
            //cout<<"fanout_arr="<<gatelist[now]->fanout_arr[j]<<endl;
			out_gate = gatelist[now]->fanout_arr[j];
            //cout<<"out_gate="<<out_gate<<" and faninnum_outgate="<<faninNum[out_gate]<<endl;
			faninNum[out_gate]--;
			gatelist[out_gate]->level = max(gatelist[out_gate]->level,gatelist[now]->level+1);
			if(faninNum[out_gate]==0)
				sorter.push(out_gate);
		}
	}
    //cout<<gateNum;
	if(i != gateNum)
	{   
		cerr<<"There are "<<gateNum-i<<" gates not in topo.";
        exit(0);
        
	}	
}
//determine the value of Dsv
void CIRCUIT::forward_STA(unsigned delay_type)
{
	unsigned i,now;
	stack <unsigned> visitor;
	unsigned * faninNum;
	unsigned gateNum = (unsigned)gatelist.size();
	faninNum = new unsigned[gateNum];	
	for (i = 0; i < gateNum ; i++)
	{
		faninNum[i] = gatelist[i]->faninNum;
		gatelist[i] -> Dsv = 0;
	}
	for(i = 0; i < gateNum; i++)
	{
        if(!gatelist[i]->faninNum)
            visitor.push(i);
	}
	unsigned out_gate;
	while(visitor.size())
	{
		now = visitor.top();
		visitor.pop();
		for(unsigned j = 0; j <gatelist[now]->fanoutNum; j++)
		{
			out_gate = gatelist[now]->fanout_arr[j];
			faninNum[out_gate]--;
			gatelist[out_gate]->Dsv = max(gatelist[out_gate]->Dsv,gatelist[now]->Dsv + gatelist[out_gate]->rise_delay);
			if(faninNum[out_gate]==0)
				visitor.push(out_gate);
		}
	}
}
//determine the value of Dvt
void CIRCUIT::backward_STA(unsigned delay_type)
{
	unsigned i,now;
	stack <unsigned> visitor;
	unsigned * fanoutNum;
	unsigned gateNum = (unsigned)gatelist.size();
	fanoutNum = new unsigned[gateNum];	
	for (i = 0; i < gateNum ; i++)
	{
		fanoutNum[i] = gatelist[i]->fanoutNum;
		gatelist[i] -> Dvt = 0;
	}
	for(i = 0; i < gateNum; i++)
	{
		if(!gatelist[i]->fanoutNum)
            visitor.push(i);
	}
	unsigned in_gate;
	while(visitor.size())
	{
		now = visitor.top();
		visitor.pop();
		for(unsigned j = 0; j <gatelist[now]->faninNum; j++)
		{
			in_gate = gatelist[now]->fanin_arr[j];
			fanoutNum[in_gate]--;
			gatelist[in_gate]->Dvt = max(gatelist[in_gate]->Dvt,gatelist[now]->Dvt + gatelist[now]->rise_delay);
			if(fanoutNum[in_gate]==0)
				visitor.push(in_gate);
		}
	}
    //for(i=0;i<gatelist.size();i++)
        //gatelist[i]->D2 = gatelist[i]->D2 - gatelist[i]->delay;
}
void CIRCUIT::cal_g_t_ei_id()
{
	stack <unsigned> visitor;
	int i;
    cout<<"number of outputs is "<<out_latch+outNum<<endl;
    cout<<"in num "<<inNum<<endl;
    cout<<"out num "<<outNum<<endl;
    cout<<"in latch "<<in_latch<<endl;
    cout<<"out latch "<<out_latch<<endl;
    int repeat[gateNum];
	float phi_1_phase = big_phi_1 + small_phi_1;
	float phi_2_phase = big_phi_2 + small_phi_2;
        for(unsigned j = 0 ;j<gateNum;j++) repeat[j] = -1;   
	while(!visitor.empty()) visitor.pop();
    cout<<"finish visitor's pop"<<endl;
	for( i = (int)(inNum + in_latch); i<(int)(inNum + in_latch + outNum + out_latch);i++)
	{
		unsigned now;
        int repeat2[gateNum];
        for(unsigned j = 0 ;j<gateNum;j++) repeat2[j] = -1;
		if(visitor.size()!=0){cerr<<"cal_get_t_eid: wrong input size"<<endl;}
		if(repeat[i] == -1)
        {
            visitor.push(i);
            repeat[i]=0;
            
        }
        
		while(visitor.size())//do this every level
		{
            //cout<<visitor.size()<<endl;

			now = visitor.top();
            cout<<gatelist[now]->name<<endl;
			visitor.pop();
            if(gatelist[now]->faninNum==0 || gatelist[now]->type==INPUT || gatelist[now]->type == SLAVE_LATCH) { if(repeat[now]!=i){repeat[now] = i; gatelist[i]->g_t_ei_id.push_back(now);} continue;}

			for(unsigned j = 0; j< gatelist[now]->faninNum; j++)
			{
				unsigned in_of_now = gatelist[now]->fanin_arr[j];
				
              
				//this expression remains to be checked
				if((max(gatelist[in_of_now]->Dsv, phi_1_phase)+gatelist[now]->Dvt+gatelist[now]->rise_delay ) >(phi_1_phase+phi_2_phase))
				{
					if(repeat[now] != i && (int)now!=i){repeat[now] = i; gatelist[i]->g_t_ei_id.push_back(now); break;}
				}
				else {
					if(repeat[now] != i)
                    {
                        if(repeat[in_of_now] == -1)
                        {
                            visitor.push(in_of_now);
                            repeat[in_of_now]=0;
                        }
                    }
				}
			}
		}
	}
     
}

//define the retiming region using the contraints of D1, D2
//V_-1 = 0, Vr = 1, V_0 = 2

void CIRCUIT::retiming_reg()
{
    for(unsigned i=inNum+in_latch+outNum+out_latch; i< gateNum ; i++)
    {
        if(gatelist[i]->Dsv>big_phi_1+small_phi_2+big_phi_2)
        {
            if(gatelist[i]->Dvt>big_phi_1+small_phi_1+big_phi_2)
                cout<<"period error! too small, change that!"<<endl;
            else
                gatelist[i]->region_type = V_ZERO;
        }
        else if(gatelist[i]->Dvt>big_phi_1+big_phi_2+small_phi_1)
            gatelist[i]->region_type = V_MINUS_ONE;
        else
            gatelist[i]->region_type = V_RETIMING;
        
    }
	
	for(unsigned i=0;i<inNum+in_latch;i++){
		gatelist[i]->region_type = V_MINUS_ONE;
	}
	for(unsigned i=inNum+in_latch;i<outNum+out_latch+inNum+in_latch;i++){ gatelist[i]->region_type = V_ZERO;}
}

void CIRCUIT::create_gate_node(){
    // create host node
    if(nodelist.size()!=0) nodelist.clear();
    node *host = new node;
    host->ID =hostID;
    host->type =HOST;
    node_init(host);
    host->gateID =-1;
    nodelist.push_back(host);
    
    //create gate nodes
    for(unsigned i=0; i< gatelist.size(); i++){
        node * new_node = new node;
        new_node->ID = (unsigned)nodelist.size();
        new_node->type = GATE;
        new_node->gateID = i;
        if(gatelist[i]->fanoutNum == 0)
            new_node->edge_mult_by = 0;
        else
            new_node->edge_mult_by = (1.0)/(float)((gatelist[i])->fanoutNum);
        nodelist.push_back(new_node);
        node_init(new_node);
        gatelist[i]->gate_node_ID= new_node->ID;
        
        if(gatelist[i]->fanoutNum>1){
            node * pseudo = new node;
            pseudo->ID = (unsigned)nodelist.size();
            pseudo->type = PSEUDO_M;
            pseudo->gateID =i;
            pseudo->edge_mult_by = (1.0)/(float)((gatelist[i])->fanoutNum);
            node_init(pseudo);
            nodelist.push_back(pseudo);
            gatelist[i]->pseudo_node_ID= pseudo->ID;
        }
    }
   
    /*for input nodes*/
    for(unsigned i=0;i<inNum+in_latch; i++){
        create_edge(1,hostID , gatelist[i]->gate_node_ID,0);
        for(unsigned j=0;j<gatelist[i]->fanout_arr.size(); j++){
            unsigned out_ind = gatelist[gatelist[i]->fanout_arr[j]]->gate_node_ID;
            
            if(gatelist[i]->fanout_arr.size()==1){
                create_edge( 0,gatelist[i]->gate_node_ID , out_ind,1);}
            else { create_edge( 0,gatelist[i]->gate_node_ID , out_ind,1.0/(float)(gatelist[i]->fanout_arr.size()));}
            if(gatelist[i]->fanout_arr.size()>1){
                int pseudo_node_ind= gatelist[i]->pseudo_node_ID;
                create_edge( 0, out_ind, pseudo_node_ind,1.0/(float)(gatelist[i]->fanout_arr.size()));
            }
        }
    }
    for(unsigned i=inNum+in_latch;i<inNum+in_latch+outNum+out_latch;i++){
        create_edge(0, gatelist[i]->gate_node_ID, hostID,0);
    }
    
    for(unsigned i=inNum+in_latch+outNum+out_latch;i<gatelist.size(); i++){
        for(unsigned j=0;j<gatelist[i]->fanout_arr.size(); j++){
            int out_ind = gatelist[gatelist[i]->fanout_arr[j]]->gate_node_ID;
            if(gatelist[i]->fanout_arr.size()==1){
                create_edge( 0,gatelist[i]->gate_node_ID , out_ind,1);}
            else {create_edge( 0,gatelist[i]->gate_node_ID , out_ind,1.0/(float)(gatelist[i]->fanout_arr.size()));}
            if(gatelist[i]->fanout_arr.size()>1){
                int pseudo_node_ind= gatelist[i]->pseudo_node_ID;
                create_edge( 0, out_ind, pseudo_node_ind,1.0/(float)(gatelist[i]->fanout_arr.size()));
            }
        }
    }
}
/* V2 */
void CIRCUIT::create_psuedo_node(){
    
    if(nodelist.size()==0) { cerr<<"wrong procedure"<<endl; exit(0);}
    for(unsigned i=inNum+in_latch;i<inNum+in_latch+outNum+out_latch;i++){
        if(gatelist[i]->g_t_ei_id.size()>0){
            node  *new_pseudo = new node;
            new_pseudo->ID =(unsigned)nodelist.size();
            new_pseudo->type =PSEUDO_T;
            new_pseudo->gateID= i;
            node_init(new_pseudo);
            nodelist.push_back(new_pseudo);
            gatelist[i]->t_ei_id= new_pseudo->ID;
            for(unsigned j=0;j<gatelist[i]->g_t_ei_id.size() ; j++){
                int g_node = gatelist[(gatelist[i]->g_t_ei_id[j])]->gate_node_ID;
                create_edge( 0,g_node, new_pseudo->ID,0);
            }
            create_edge( 0,new_pseudo->ID,hostID, (float)-edl_cost);
        }
    }
}
void CIRCUIT::set_upper_lower_bound(){
    
    for(unsigned i=0;i<gatelist.size();i++){
        if(gatelist[i]->region_type==V_MINUS_ONE){
            if(i<inNum+in_latch|| i>=inNum+in_latch+outNum+out_latch)create_edge( -1,gatelist[i]->gate_node_ID,hostID, 0);
            if(i>=inNum+in_latch)create_edge( 1,hostID,gatelist[i]->gate_node_ID, 0);
        }
        else if(gatelist[i]->region_type==V_ZERO){
            if(i<inNum+in_latch|| i>=inNum+in_latch+outNum+out_latch)create_edge( 0,gatelist[i]->gate_node_ID,hostID, 0);
            if(i>=inNum+in_latch)create_edge( 0,hostID        ,gatelist[i]->gate_node_ID, 0);
        }
        else if(gatelist[i]->region_type==V_RETIMING){
            if(i<inNum+in_latch|| i>=inNum+in_latch+outNum+out_latch)create_edge( 0,gatelist[i]->gate_node_ID,hostID, 0);
            if(i>=inNum+in_latch)create_edge( 1,hostID        ,gatelist[i]->gate_node_ID, 0);
        }
    }
}
void CIRCUIT::create_edge(float cost, unsigned from, unsigned to,float demand){
    EDGE e; e.cost = cost, e.from =from; e.to=to; 
    nodelist[to]->demand+= demand; nodelist[to]->in_degree++;      nodelist[to]->fanin_node.push_back(e);
    nodelist[from]->demand-=demand; nodelist[from]->out_degree++;  nodelist[from]->fanout_node.push_back(e);
}
void CIRCUIT::node_init(node *new_node){
    if(new_node==NULL){ cerr<<"new node should not be null"<<endl; exit(1);}
    new_node->in_degree =new_node->out_degree=0;
    new_node->fanout_node.clear(); new_node->fanin_node.clear();
    new_node->region_type = V_RETIMING;
    new_node->demand = 0;
}


void  CIRCUIT::output_node_graph(char* constraint_name,float t)
{

    ofstream out(constraint_name);
    float check_demand =0;
    if(out.is_open())
    {
        unsigned Edge_num = 0;
        out<<t<<endl;
        for(unsigned i = 0;i<nodelist.size();i++)
            Edge_num = Edge_num + nodelist[i]->in_degree;
        out<<nodelist.size()<<" "<<Edge_num<<endl;
        for(unsigned i = 0;i<nodelist.size();i++)
        {
            float temp_demand = (round(nodelist[i]->demand*1000))/1000;
	        out<<temp_demand<<" ";
            check_demand+= temp_demand;
          /*I if(nodelist[i]->demand>0.0001 || nodelist[i]->demand<-0.0001)
           {
	     out<<i+1<<"   "<<nodelist[i]->demand<<endl;
             
           }
	   else {
		out<<i+1<<"   "<<0<<endl;
           }*/
        }
        out<<endl<<endl;
       

        for(unsigned i = 0;i<nodelist.size();i++)
        {
            for(int j = 0;j<nodelist[i]->in_degree;j++)
                out<<nodelist[i]->fanin_node[j].to+1<<" "<<nodelist[i]->fanin_node[j].from+1<<" "<<nodelist[i]->fanin_node[j].cost<<endl;
        }

    }

      if(check_demand!=0){
	       cerr<< "total demand is not zero"<<endl; exit(1);
      }  
    
}


void CIRCUIT::get_r_u(char* flow_info_name,char* answer_name,int cal_ori)
{
    clock_t t1,t2;
    t1 =clock();
    ifstream infile;
    //vector<edge> data;
    //vector<int> from;
    //vector<int> to;
    vector<int> r_u;
    int v;
    float t;
    infile.open(flow_info_name);
    if(!infile.is_open())
    {
        cerr<<"error in opening the file!"<<endl;
        exit(1);
    }
    unsigned count = 0;
    infile>>t;
    while(!infile.eof())
    {
        infile >>v;
        r_u.push_back(-v);
        count++;
    }

    for (unsigned i = 0; i< r_u.size(); i++)
    {
        cout<<"retiming is "<<r_u[i]<<"  for node "<<i<<endl;
    }
    //The caculate the total cost
    //node_type = 1, pseudo node T node_type = 3, gate node
    vector<unsigned> pseudo;
    for(unsigned i = 0;i< nodelist.size();i++)
    {
        if(nodelist[i]->type==1)
            pseudo.push_back(i);
        cout<<i<<"multiby"<<nodelist[i]->edge_mult_by<<endl;
    }
    vector <unsigned> pseudo_visited;
    for(unsigned i = 0; i < r_u.size();i++)
    {
        pseudo_visited.push_back(0);
    }
    float total_COST = 0;
    int latch_count = 0;
    int edl_optimal_count = 0;
    for(unsigned i = 0; i < nodelist.size();i++)
    {
        for(unsigned j =0 ;j < nodelist[i]->out_degree;j++)
        {
            cout<<"node "<<i<<"'s out_degree is "<<nodelist[i]->out_degree<<endl;
            if(nodelist[i]->type==GATE&&nodelist[nodelist[i]->fanout_node[j].to]->type==GATE)
            {
                total_COST = total_COST + (float)(r_u[nodelist[i]->fanout_node[j].to]- r_u[i])*nodelist[i]->edge_mult_by ;
                //if((float)(r_u[nodelist[i]->fanout_node[j].to]- r_u[i])*nodelist[i]->edge_mult_by !=0)
                  //  latch_count++;
                //cout<<total_COST<<endl;
            }
            else if(nodelist[i]->type==GATE&&nodelist[nodelist[i]->fanout_node[j].to]->type==PSEUDO_M)
            {
                total_COST = total_COST + (float)(r_u[nodelist[i]->fanout_node[j].to]- r_u[i])*nodelist[nodelist[i]->fanout_node[j].to]->edge_mult_by ;
                cout<<total_COST<<endl;
            }
            else if(nodelist[nodelist[i]->fanout_node[j].to]->type==1&&pseudo_visited[nodelist[i]->fanout_node[j].to]==0)
            {
                total_COST = total_COST + (r_u[nodelist[i]->fanout_node[j].to]+1)*edl_cost ;
                if(r_u[nodelist[i]->fanout_node[j].to]==-1)
                    edl_optimal_count++;
                pseudo_visited[nodelist[i]->fanout_node[j].to]=1;
                cout<<total_COST<<endl;
            }
        }
    }
    if(cal_ori==0)
        for(int i =(int)(in_latch+inNum); i < (int)(in_latch + inNum + outNum + out_latch);i++)
            if(gatelist[i]->g_t_ei_id.size()==0)
                total_COST = total_COST + edl_cost;
    total_COST = total_COST + outNum + out_latch;
    cout<<"num of pseudo nodes is "<<pseudo.size()<<endl;
    //cout<<"total_cost is "<<total_COST<<endl;
    if(cal_ori==1)
    {
        //total_COST = total_COST + (float)(out_latch+outNum)*edl_cost;
        for(int i =(int)(in_latch+inNum); i < (int)(in_latch + inNum + outNum + out_latch);i++)
        {
            int edl_need = 0;
            //int temp = 0;
            if(gatelist[i]->g_t_ei_id.size()==0)
            {
                //total_COST = total_COST + edl_cost; //need EDL
                //temp = 1;
                edl_need = 1;
            }
            for(int j = 0; j< gatelist[i]->g_t_ei_id.size();j++)
            {
                if(r_u[gatelist[i]->g_t_ei_id[j]]==0)
                    edl_need = 1;
            }
            if(edl_need==1)
                total_COST = total_COST + edl_cost;
            else
                edl_optimal_count++;
            
        }
    }
    //write the cost into a new file
    t2 = clock();
    float diff = ((float)t2-(float)t1)/(float)CLOCKS_PER_SEC;
    float running_time = t+diff;
    ofstream outfile;
    outfile.open(answer_name,std::ofstream::out | std::ofstream::app);
    if(!infile.is_open())
    {
        cerr<<"error in opening the output file!"<<endl;
        exit(1);
    }
    outfile<<flow_info_name<<"\t";
    outfile<<total_COST<<"\t";
	outfile<<edl_optimal_count <<"\t";
    //outfile<<"edl_optimal_number is "<<edl_optimal_count<<"\t";
    latch_count = round(total_COST - (out_latch+outNum-edl_optimal_count)*edl_cost)-out_latch-outNum;
	outfile<<latch_count<<"\t";
	outfile<<outNum + out_latch<<"\t";
    outfile<<running_time<<endl;
    //outfile<<"the number of latch in the circuit is "<<latch_count<<endl;
    //outfile<<"out_put number is "<<outNum + out_latch<<endl;
    outfile.close();
}
