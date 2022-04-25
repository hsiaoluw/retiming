#include <errno.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <stdio.h>
#include <fstream>
#include "circuit.h"
using namespace std;



//=======================================    Load    ========================================

void ini_gate(gate * tempg){
	if(tempg!=NULL){
    tempg->faninName =NULL;
	tempg->level = 0;
    tempg->faninNum =tempg->fanoutNum=0;
    tempg->Dsv= tempg->Dvt =  0;
    tempg->faninName = new char * [3];
    tempg->faninSize =3;
	tempg->type= T_UN;
    tempg->fanout_arr.clear();
	tempg->fanin_arr.clear();
	tempg->g_t_ei_id.clear();
	}
}
unsigned length(char* buffer)
{
	unsigned i=0;
	while(buffer[i]!=0)	i++;
	return i;
}

void CIRCUIT::loadCircuit(char * fileName)
{
	int i=length(fileName)-1;
	//if(fileName[i]=='h') loadBench(fileName);
	if(fileName[i]=='f') loadBlif(fileName);
	else cout<<"Unknown in_circuit_type.\n";
}

bool remove(char c)
{    return c=='\t'|| c=='\r' || c=='\n' || c=='=' || c==' ' || c==':' || c==',' || c==';' || c == ')' || c =='(' || c =='.'|| c=='\0'; }
bool remove2(char c)
{    return c=='\n' || c==92 || c=='=' || c==' ' || c==':' || c==',' || c==';' || c =='.'; }

bool eq(char* a,char* b) //a is longer
{
	bool flag = true;
	for(unsigned i=0; b[i]!=0; i++)
		if(a[i] != b[i])
			flag = false;
	return flag;
}
char * exact(char* buffer,unsigned start,char end,unsigned& num)
{
	char * name;
	unsigned i=start,j=0;
	name = new char[30];
	while(buffer[i]!=end && buffer[i]!=0 && buffer[i]!=')')
	{
		if(!remove(buffer[i])) name[j++]=buffer[i];
		i++;
	}
	name[j] = 0;
	num = i-start+1;
	return name;
}
void CIRCUIT::pushFanin(unsigned g, char * name)
{
	if(gatelist[g]->faninNum == gatelist[g]->faninSize )
		gatelist[g]->faninName = resize(gatelist[g]->faninName,gatelist[g]->faninSize);
	gatelist[g]->faninName[ gatelist[g]->faninNum ] = name;
	gatelist[g]->faninNum++;
}

void CIRCUIT::loadBlif(char * fileName)
{
	bool skip=false; //no latches
	gate * tempg;
	FILE * infile;
	char buffer[300], * temps;
	unsigned i,g,index,len;
	map < string,unsigned > gateName; // INV, AND, NAND, OR, NOR, XOR, XNOR, BUFF
	map < string,unsigned >::iterator it;
		
	infile  = fopen(fileName,"r");
    if(infile == NULL)	{	cout <<"cannot open the file ["<<fileName<<"]\n"; exit(0);	}
	
	//---------- .inputs G0 G1 G2
	cout<<"start load...\n";	
	
	for(fgets (buffer , 300 , infile); !eq(buffer,(char*)".inputs"); fgets (buffer , 300 , infile))	;
	i=7; len=length(buffer);
	for(gateNum=0,inNum=0; true; gateNum++,inNum++)
	{   
        tempg = new gate; 	ini_gate(tempg);
        tempg->type = INPUT;
		while(remove(buffer[i])){ i++; if(i>=len) break; }
		if(i>=len-2) 
		{
			fgets (buffer , 300 , infile);
			i=0; len=length(buffer);
			if(eq(buffer,(char*)".")) break;
			while(remove(buffer[i])) i++;
		}
		g=0; 
		tempg->name = new char[100];
		while(!remove(buffer[i])) tempg->name[g++] = buffer[i++];
		tempg->name[g]='\0';
		/*if( gateNum == gatelistSize ) gatelist = resize(gatelist, gatelistSize);*/
		gatelist.push_back(tempg);
		gateName.insert( pair<string,int>(tempg->name, gateNum) );
                cout<<"insert input\t"<< tempg->name<< " gateNum"<< gateNum <<endl;
		//delete [] temp->name;	
	}
	in_latch=0;
	for(fgets (buffer , 300 , infile); true; fgets (buffer , 300 , infile))
	{
		if(eq(buffer,(char*)".latch")) break;
		else if(eq(buffer,(char*)".gate")){ skip=true; break; }
	}
	if(!skip)
	{
		for(i=6; true; i=6)
		{
			while(buffer[i]==' ') i++;
			while(buffer[i]!=' ') i++;
			while(buffer[i]==' ') i++;// ____"nonspace" _____
			g=0; 
			tempg = new gate; 	ini_gate(tempg);
            tempg->type = SLAVE_LATCH;
			tempg->name = new char[100];
			while(!remove(buffer[i])) tempg->name[g++] = buffer[i++];
			tempg->name[g]='\0';
			/*if( gateNum == gatelistSize ) gatelist = resize(gatelist, gatelistSize);*/
			gatelist.push_back(tempg);
			gateName.insert( pair<string,int>(tempg->name, gateNum) );	
			cout<<"insert input\t"<< tempg->name<< " gateNum"<< gateNum <<endl;
			gateNum++; in_latch++; 
			fgets (buffer , 300 , infile);
			if(!eq(buffer, (char*)".latch")) break;
		}
	}
	rewind(infile);
	//----------- .outputs G118 G167 G107 G119 G168 G108
	
	for(fgets (buffer , 300 , infile); !eq(buffer,(char*)".outputs"); fgets (buffer , 300 , infile))	;
	i=8; len=length(buffer);
	for(outNum=0; true; gateNum++,outNum++)
	{   
       tempg = new gate; ini_gate(tempg); tempg->type = OUTPUT;
		while(remove(buffer[i])){ i++; if(i>=len) break; }
		if(i>=len-2) 
		{
			fgets (buffer , 300 , infile);
			i=0; len=length(buffer);
			if(eq(buffer,(char*)".")) break;
			while(remove(buffer[i])) i++;
		}
		g=0; 
		tempg->name = new char[100];
		while(!remove(buffer[i])) tempg->name[g++] = buffer[i++];
		tempg->name[g]='\0';

		gatelist.push_back(tempg);
		pushFanin(gateNum, tempg->name);	
		//delete [] tempg->name;
	}
	out_latch=0;
	if(!skip)
	{
		for(fgets (buffer , 300 , infile); !eq(buffer,(char*)".latch"); fgets (buffer , 300 , infile))	;
		for(i=6; true; i=6)
		{
			while(buffer[i]==' ') i++;
			g=0; 
			tempg = new gate; 	ini_gate(tempg);
            tempg->type = MASTER_LATCH;
			tempg->name = new char[100];
			while(!remove(buffer[i])) tempg->name[g++] = buffer[i++];
			tempg->name[g]='\0';
			gatelist.push_back(tempg);
			pushFanin(gateNum, tempg->name);		
			gateNum++; out_latch++;
			fgets (buffer , 300 , infile);
			if(!eq(buffer, (char*)".latch")) break;

		}
	}		
	//.gate OR3   A=G13 B=G12 C=G11 Y=n76
	for(; !eq(buffer,(char*)".gate"); fgets (buffer , 300 , infile))	;
	for(i=6; true; i=6)
	{    tempg = new gate; 	ini_gate(tempg);
		if(eq(buffer,(char*)".gate INV") || eq(buffer,(char*)".gate NOT") || eq(buffer,(char*)".gate inv") || eq(buffer,(char*)".gate not"))
			tempg->type = T_NOT;
		else if(eq(buffer,(char*)".gate AND") || eq(buffer,(char*)".gate and"))
			tempg->type = T_AND;
		else if(eq(buffer,(char*)".gate NAND")|| eq(buffer,(char*)".gate nand"))
			tempg->type = T_NAND;
		else if(eq(buffer,(char*)".gate OR") || eq(buffer,(char*)".gate or"))
			tempg->type = T_OR;
		else if(eq(buffer,(char*)".gate NOR") || eq(buffer,(char*)".gate nor"))
			tempg->type = T_NOR;
		else if(eq(buffer,(char*)".gate BUF") || eq(buffer,(char*)".gate buf"))
			tempg->type = T_BUFF;
		else if(eq(buffer,(char*)".gate XOR") || eq(buffer,(char*)".gate xor"))
			tempg->type = T_XOR;
		else if(eq(buffer,(char*)".gate ZERO") || eq(buffer,(char*)".gate zero"))
			tempg->type = T_0;
		else if(eq(buffer,(char*)".gate ONE") || eq(buffer,(char*)".gate one"))
			tempg->type = T_1;
        else {cerr<<"unknown gate type"<<endl;exit(1);}
		
		tempg->name = new char[100];
		gatelist.push_back(tempg) ;
		
		for(; true; i++)
		{ 
			if(buffer[i] == '=')
			{
				if(buffer[i-1]=='Y' ||buffer[i-1]=='y' ) //gate output
                {			i++;
                    for(g=0; !remove(buffer[i]); g++,i++)	{gatelist[gateNum]->name[g] = buffer[i];}
					gatelist[gateNum]->name[g] = '\0';
					gateName.insert(pair<string,int>(gatelist[gateNum]->name, gateNum));
                    cout<<"insert gate name"<<gatelist[gateNum]->name<<endl;
                    cout<<"number"<<gateNum<<endl;
					break;
				}				
				else
				{
					temps = new char[100];
                    for(g=0,i++; !remove(buffer[i]); g++,i++)	{temps[g] = buffer[i];}
					temps[g] = '\0';
                    cout<<gatelist[gateNum]->name <<"input name "<<temps<<endl;
					pushFanin(gateNum,temps);
				}
			}
		}		
		gateNum++;
        if(gateNum!=gatelist.size()){cerr<<"wrong number of gate"<<endl; exit(1); }
		fgets (buffer , 300 , infile);
		if(eq(buffer, (char*)".end")) break;
	}
	for( g=inNum+in_latch; g<gateNum; g++) //push fanin ID
	{
		/*gatelist[g].fanin = new unsigned[gatelist[g].faninNum];*/
		for(unsigned net=0; net<gatelist[g]->faninNum; net++)
		{	
			index = gateName.find(gatelist[g]->faninName[net])->second;
  cout<<"name:"<<gatelist[g]->name<<endl;
                cout<<"input name index:"<<index<<" input name:"<< gatelist[g]->faninName[net] <<endl;
			if(index<gateNum)
            {
              
                gatelist[g]->fanin_arr.push_back(index);
                gatelist[index]->fanoutNum++;
                gatelist[index]->fanout_arr.push_back(g);
            }
            else { cerr<<"unmatched fanin name"<<endl; exit(1);}
		}	
	}		
	fclose(infile);
}


