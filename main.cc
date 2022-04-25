//
//  main.cpp
//  circuit
//
//  Created by thuzmh on 16/7/20.
//  Copyrigt thuzmh, Hsiao-Lun Wang All rights reserved.
//
// This version has been uploaded on the small-white-whale's github
#include <iostream>
#include "circuit.h"
#include <math.h>
#include <time.h>
#include <string.h>
#define window 0.3
#define EDL 0.5
using namespace std;
//mode =0 , cal constraint
//mode =1, cal optimal value from flow
int mode =-1;
int simple_test=0;
int delay_type= LIB;
int cal_ori=0;
float EDL_cost =EDL;
float window_size =window;
clock_t  t1,t2;
void usage(){
	cerr<<"wrong command"<<endl;
	cerr<<"For calculating constraints for SAS: ./code_constraint [blif circuit name] constraint [output constraints name for CPLEX]" <<endl;
	cerr<<"For calculating the optimal value  : ./code_constraint [blif circuit name] optimal    [input flow txt from CPLEX] [output answer txt] " <<endl;
	cerr<<" use [unit] to set delay type unit delay, [lib] to set delay type to library delay" <<endl;
	cerr<<" use [window 0.3] to set 30% window or [window 0.4] to set 40% window ..." <<endl;
	cerr<<" use [EDL_cost 1] to set the edl cost saved while changing 1 edl to normal latch" <<endl;
	exit(1);
}
int main(int argc,char * argv[]) {
    CIRCUIT C;
    
    char * constraint_name = NULL;
    char * flow_info_name=NULL;
    char * answer_name=NULL;
	char * circuit_name=NULL;
    t1 = clock();
	
	/*command parser*/
	for(int i=1; argv[i]!=NULL; i++)
	{			
		//===== [delay_model] =====
		if(i==1)                             { circuit_name = argv[i];}
		else if((!strcmp(argv[i],"constraint"))|| (!strcmp(argv[i],"constraints"))) {mode=0;constraint_name=argv[i+1]; i++; }
		else if(!strcmp(argv[i],"optimal")){mode=1; flow_info_name=argv[i+1];answer_name=argv[i+2];i+=2;}
		else if(!strcmp(argv[i],"unit"))  { delay_type   = UNIT;    }  
		else if(!strcmp(argv[i],"lib"))   { delay_type   = LIB;     }	//44nm lib, use 0/1-specified TCF!
		else if(!strcmp(argv[i],"lib2"))  { delay_type   = LIB2;    } //d = max{dr,df}
		else if(!strcmp(argv[i],"window")){ window_size =(float)atof(argv[i+1]);  i++;}	
		else if(!strcmp(argv[i],"edl_cost")){ EDL_cost =(float)atof(argv[i+1]);  i++;}	
 	        else if(!strcmp(argv[i],"test")) { simple_test=1;}
		else if(!strcmp(argv[i],"ori"))  { cal_ori=1;}
		else {usage();}
	}
	if(mode==-1){ usage();}
	C.loadBlif(circuit_name);
        cout<<"inNum "<< C.inNum<<" outNum "<<C.outNum<< " inlatch "<<C.in_latch<<" outlatch "<<C.out_latch <<endl;
        cout<<C.gateNum<<endl;
        C.topological();
        C.edl_cost = EDL_cost;

      if(simple_test){
	    for(int i = 0;i<C.gateNum;i++)
	    {
		if(C.gatelist[i]->type==SLAVE_LATCH||C.gatelist[i]->type==MASTER_LATCH||C.gatelist[i]->type==INPUT||C.gatelist[i]->type==OUTPUT)
		    C.gatelist[i]->rise_delay = 0;
		else
		    C.gatelist[i]->rise_delay = 0.8;
	    }
		C.gatelist[5]->rise_delay=2;
	}
  
     else{  C.gateDelay(delay_type);}
     C.forward_STA(0);
     C.backward_STA(0);
     for(int i = 0;i<C.gateNum;i++)
     {
        cout<<"gate name"<<C.gatelist[i]->name<<endl;
        cout<<"gate "<<i<<"'s fanin_num is"<<C.gatelist[i]->fanin_arr.size()<<endl;
        for(int j = 0;j<C.gatelist[i]->fanin_arr.size();j++)
        {
            cout<<"their ID is: "<<C.gatelist[C.gatelist[i]->fanin_arr[j]]->name<<endl;
        }
        cout<<"  "<<i<<"   Dsv= "<<C.gatelist[i]->Dsv<<endl;
        cout<<"  "<<i<<"   Dvt= "<<C.gatelist[i]->Dvt<<endl;
    }
    float max_time = 0;
    for(unsigned i = 0; i< C.gateNum;i++){if(C.gatelist[i]->Dsv+C.gatelist[i]->Dvt > max_time)max_time = C.gatelist[i]->Dsv+C.gatelist[i]->Dvt;}
    if(!simple_test){
	    float piece = max_time/(1+window_size);
	    C.big_phi_1 = piece * window;
	    C.small_phi_1 = piece * (0.5-window_size);
	    C.big_phi_2 = C.big_phi_1;
	    C.small_phi_2 = C.small_phi_1;
     }else{
	    C.big_phi_2 =1;
	    C.big_phi_1 =1;
	    C.small_phi_2 =1;
	    C.small_phi_1 =1;
	}
    C.cal_g_t_ei_id();//so slow
    for(int i = 0; i< C.gateNum;i++)
    {
        if(C.gatelist[i]->type==OUTPUT||C.gatelist[i]->type==MASTER_LATCH)
        {
            cout<<C.gatelist[i]->g_t_ei_id.size()<<endl;
            for (int j = 0; j < C.gatelist[i]->g_t_ei_id.size();j++)
                cout<<"cal_g_t_ei_id of "<<C.gatelist[i]->name<<"_"<<j<<" is "<<C.gatelist[C.gatelist[i]->g_t_ei_id[j]]->name<<endl;
        }
    }
    //judge the retming type
    C.retiming_reg();
    /*cout<<"-------------retiming_type--------------------------------------------"<<endl;
    for (int i =0;i<C.gatelist.size();i++)
        cout<<i<<"'s retming type is"<<C.gatelist[i]->retiming_type<<endl;
    cout<<"-------------retiming_type--------------------------------------------"<<endl;*/


    C.create_gate_node();
    //cout<<"finish gate"<<endl;
    cout<<cal_ori<<" cal_ori"<<endl;
    if(cal_ori!=1) C.create_psuedo_node();
    //cout<<"finish psuedo"<<endl;
    C.set_upper_lower_bound();
    t2 = clock();
    float diff = ((float)t2-(float)t1)/(float)CLOCKS_PER_SEC;
    
//write file!!!!
   if(mode==0) C.output_node_graph(constraint_name,diff);
//get the result!!!!
   if(mode==1)  C.get_r_u(flow_info_name,answer_name,cal_ori);
    
    return 0;
}
