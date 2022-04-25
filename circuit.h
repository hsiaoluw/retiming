#ifndef CIRCUIT_H
#define CIRCUIT_H
#define INITSIZE 3
#include <stdlib.h>
#include <math.h>
using namespace std;
enum REGION_TYPE{V_MINUS_ONE=0, V_ZERO=2, V_RETIMING=1};
enum GATE_TYPE {INPUT=0, OUTPUT=1, SLAVE_LATCH=11, MASTER_LATCH=12, T_BUFF=2, T_NOT=3, T_AND=4, T_NAND=5, T_OR=6, T_NOR=7, T_XOR=8, T_0=9, T_1=10, T_UN= 13};
enum LIB_TYPE  {L_BUFF=0, L_NOT=1, L_AND=2, L_NAND=3, L_OR=4, L_NOR=5};
enum GATE_VALUE {V0=0,V1=1,VU=2,VE=3,VX=4};
enum IN_CIRCUIT_TYPE  {BENCH=0,BLIF=1};
enum OUT_CIRCUIT_TYPE {CNF=0, NONE=2};
enum DELAY_TYPE       {UNIT=0, FANOUT=1, LIB=2, LIB2=3};
enum ACCURACY_TYPE    {EXACT=0,ABOUT=1, CHECK=2, FPATH=3, FLOAT=4};
enum NODE_TYPE        {HOST=0, PSEUDO_T=1, PSEUDO_M=2, GATE=3};
#include <vector>
typedef struct EDGE{ float cost; unsigned from; unsigned to;} edge;
typedef struct GATE{
    char * name;
    char ** faninName;//faninNames
    int gateID;
    unsigned faninSize;
    unsigned pseudo_node_ID;//fan_out pseudo node
    unsigned gate_node_ID;
    unsigned type, level;
    float cap;
    unsigned fanoutNum, faninNum;
    float rise_delay,fall_delay;//rise and fall delay
    vector<unsigned> fanout_arr;//fanout_IDs
    vector<unsigned> fanin_arr;//fanin_IDs
    float Dsv, Dvt;
    unsigned region_type;//region_type
    // for output latches
    vector<unsigned> g_t_ei_id;//only for output, it is empty for master latches which must be EDL
    unsigned t_ei_id;//master pseudo node
    float edge_mult_by;
    
    /*int  r_id; */
    /*int  cost;*/
} gate;
typedef struct NODE{
    unsigned ID;
    float demand; // out - in
    float edge_mult_by;//fanout_edge_beta
    unsigned region_type;//region_type
    unsigned gateID;
    int in_degree;
    int out_degree;
    int type;
    vector<EDGE> fanout_node;//fanout_edge
    vector<EDGE> fanin_node;//fanin_edge
} node;

template<class T>
T * resize(T * mylist, unsigned &size)
{
    T * templist;
    if(!size){ size=INITSIZE; mylist = new T[size]; return mylist;}
    templist = new T[size*2];
    for(unsigned g=0; g<size; g++){ templist[g] = mylist[g]; }
    size *=2;
    return templist;
}

class CIRCUIT
{
private:
public:
     //circuit
	unsigned inNum, outNum, in_latch, out_latch, gateNum;
	vector<gate *> gatelist;	
	vector<node *> nodelist;
    unsigned int* topo;//contains the gate ID of topo sort
	double big_phi_1, small_phi_1, big_phi_2, small_phi_2;
    float edl_cost;
    float index[7], rising[6][7], falling[6][7];// from lib
	//graph
	void loadCircuit(char* fileName);
	void pushFanin(unsigned g, char * name);
	void loadBlif(char* fileName);
	
	//===== circuit.cc ===== 
	/*delay type*/
    void gateDelay(unsigned delay_type);
    /*complete data structure*/
	void  topological();
	void  forward_STA(unsigned delay_type);	
	void  backward_STA(unsigned delay_type);
	void  cal_g_t_ei_id();//calculate cut set
    void  retiming_reg();//calculate the retiming region
	/*creating node */
    void  create_edge(float cost, unsigned from, unsigned to,float demand);
    void  node_init(node* new_node);
    void  set_upper_lower_bound();
    void  create_psuedo_node();//create master psuedo node
	void  create_gate_node();
    float libDelay(unsigned ID, bool ris);//void  create_psuedo_t();
	/* node graph*/
	void  output_node_graph(char* constraint_name,float t);//output Simplex LP
    //data processing
    void  get_r_u(char* flow_info_name,char* answer_name,int cal_ori);//final cost
};



#endif