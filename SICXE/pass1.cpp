#include "tables_and_functions.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;
map<string,int> sect_count;
map<string,vector<string>> extref;
map<string,vector<string>> extdef;
map<string,vector<string>> literal;
string filename;
bool err=0;
bool END=0;
void firstpass()
{
  load_table();
  ifstream inp(filename);
  ofstream intermediate("intermediate"+filename), error("error"+filename),table("table.txt");
  int cnt=0;
  if(inp.is_open() && intermediate.is_open() && table.is_open())
  {
    intermediate<<"ADDRESS\t\tLABEL\t\tOPCODE\t\tOPERAND"<<endl;
    string curr_sect="";
    while(inp)
    {
      vector<string> v;
      string x;
      getline(inp,x);
      //cout<<x<<endl;
      int i=0;
      int n=x.length();
      string y="";
      while(i<n)
      {
        if(x[i]!=' ') y+=x[i];
         else
         {
            if(y.length()!=0)
            {
              v.push_back(y);
              y="";
            }
         }
         i++;
      }
      if(y.length()!=0) v.push_back(y);
      if(v.size()==3)
      {
      //vector size-3-start,resw, resb,word,byte,equ,instruction with label
      //start-locctr initialised, curr sect updated, pushed to map of sects, made external defination
      //resw-check for prev symbol def in its own section, if not, make new symbol, update the vector corresponding to the symbol in the symbtab to show the symbol doesn't exist in the sections before, this is necessary for introduction of a new symbol which wasn't there earlier but might occur multiple times in multiple sections in the future
      //equ-check for prev symbol def in its own section, if not, make new symbol,evaluate expression,check if relative, no modif record, in listing file it is of the form value and then input line instead of location and then input line
      //word-same as resw
      //byte-same as word except increment locctr according to length
      //resb-same as resw,increment locctr according to bytes specified
      //instruction-label in symbtab, handle cases for literal, redef of * not allowed, all other literals default to same value with same name, new literal has to be entered in literal table,address currently undefined, locctr update according to format
        if(v[1]=="START") 
        {
          curr_sect=v[0];
          while(v[2].length()<5) v[2]='0'+v[2];
          locctr=v[2];
          intermediate<<"    "<<"\t\t"<<v[0]<<"\t\t    "<<v[1]<<"\t\t"<<v[2]<<endl;
          sect_count[v[0]]=cnt;
          cnt++;
          struct_symbol s;
          s.value=locctr;
          s.address=locctr;
          s.exists='y';
          s.relative=1;
          SYMBTAB[curr_sect].push_back(s);
          EXTDEF[curr_sect].exists='y';
          EXTDEF[curr_sect].section=curr_sect;
        }
        else if(v[1]=="RESW")
        {
            if(chk_symb(v[0],sect_count[curr_sect]))
            {
              intermediate<<"**********************ERROR*****************************"<<endl;
              error<<"REDEFINITION OF SYMBOL"<<endl;
            }
            else{
          if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=1;
            SYMBTAB[v[0]].push_back(s);
          }
          else{
          while(SYMBTAB[v[0]].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[v[0]].push_back(s);
          }
          SYMBTAB[v[0]][sect_count[curr_sect]].address=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].exists='y';
          SYMBTAB[v[0]][sect_count[curr_sect]].value=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].relative=1;
          }
          intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<"\t\t"<<v[2]<<endl;
          locctr=add(locctr,3*stoi(v[2]));}
        }
        else if(v[1]=="EQU")
          {
            if(chk_symb(v[0],sect_count[curr_sect]))
            {
              intermediate<<"**********************ERROR*****************************"<<endl;
              error<<"REDEFINITION OF SYMBOL"<<endl;
              err=1;
            }
            else{
            pair<vector<string>,string> q=expressionparse(v[2],curr_sect,sect_count[curr_sect],6,0,extref);
            struct_symbol s;
            s.address="?";
            s.exists='y';
            s.value=q.second;
            s.relative=rel(v[2],sect_count[curr_sect]);
            SYMBTAB[v[0]].push_back(s);
            intermediate<<s.value<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<"\t\t"<<v[2]<<endl;}
          }
        else if(v[1]=="WORD")
        {
           if(chk_symb(v[0],sect_count[curr_sect]))
            {
              intermediate<<"**********************ERROR*****************************"<<endl;
              error<<"REDEFINITION OF SYMBOL"<<endl;
              err=1;
            }
            else{
           if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=1;
            SYMBTAB[v[0]].push_back(s);
          }
          else{
           while(SYMBTAB[v[0]].size()<=sect_count[curr_sect])
          {
            struct_symbol s;
            s.address="0";
            s.exists='n';
            //if(v[0]=="MAXLEN") cout<<sect_count[curr_sect]<<endl;
            SYMBTAB[v[0]].push_back(s);
          }
          SYMBTAB[v[0]][sect_count[curr_sect]].address=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].value=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].relative=1;
          SYMBTAB[v[0]][sect_count[curr_sect]].exists='y';}
          intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t   "<<v[1]<<"\t\t"<<v[2]<<endl;
          locctr=add(locctr,3);}
        }
        else if(v[1]=="BYTE") 
        {
          if(chk_symb(v[0],sect_count[curr_sect]))
            {
              intermediate<<"**********************ERROR*****************************"<<endl;
              error<<"REDEFINITION OF SYMBOL"<<endl;
              err=1;
            }
          else{
           if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=1;
            SYMBTAB[v[0]].push_back(s);
          }
          else{
           while(SYMBTAB[v[0]].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[v[0]].push_back(s);
          }
          SYMBTAB[v[0]][sect_count[curr_sect]].address=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].value=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].relative=1;
          SYMBTAB[v[0]][sect_count[curr_sect]].exists='y';}
          intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t   "<<v[1]<<"\t\t"<<v[2]<<endl;
          int q=0;
          if(v[2][0]=='C') q=v[2].length()-3;
          else if(v[2][0]=='X') q=((v[2].length()-3)+1)/2;
          locctr=add(locctr,q);}
        }
        else if(v[1]=="RESB")
        {
          if(chk_symb(v[0],sect_count[curr_sect]))
            {
              intermediate<<"**********************ERROR*****************************"<<endl;
              error<<"REDEFINITION OF SYMBOL"<<endl;
              err=1;
            }
          else{
           if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=1;
            SYMBTAB[v[0]].push_back(s);
          }
          else{
           while(SYMBTAB[v[0]].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[v[0]].push_back(s);
          }
          SYMBTAB[v[0]][sect_count[curr_sect]].address=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].value=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].relative=1;
          SYMBTAB[v[0]][sect_count[curr_sect]].exists='y';}
          intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t   "<<v[1]<<"\t\t"<<v[2]<<endl;
          locctr=add(locctr,stoi(v[2]));}
        }
        else
        {
           if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=1;
            SYMBTAB[v[0]].push_back(s);
          }
          else if(sect_count[curr_sect]!=0){
          while(SYMBTAB[v[0]].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[v[0]].push_back(s);
          }
          SYMBTAB[v[0]][sect_count[curr_sect]].address=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].value=locctr;
          SYMBTAB[v[0]][sect_count[curr_sect]].relative=1;
          SYMBTAB[v[0]][sect_count[curr_sect]].exists='y';
          }
          if(v[2][0]=='=')
          {
            string d=extract(v[2]);
            bool chk=0;
            for(auto it:LITTAB)
            { 
                if(it.first==d) chk=1;
             }
             if(chk && d=="*") 
             {
               intermediate<<"***********ERROR************************"<<endl;
               error<<"* redefined"<<endl;
             }
            if(!chk)
           {
              LITTAB[d].exists='y';
              if(d!="*") LITTAB[d].value=d;
              else  LITTAB[d].value=locctr;
              LITTAB[d].address="?";
              literal[curr_sect].push_back(d);
           }
          }
           intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<"\t\t    "<<v[2]<<endl;
           if(v[1][0]=='+')
           {
            string y;
            y=add(locctr,4);
            locctr=y;
           }
           else{
            int form=OPTAB[v[1]].format;
            string y;
            y=add(locctr,form);
            locctr=y;
           }
        }
      }
      else if(v.size()==2)
      {
         //vector size-2-end,base,extdef,extref,csect,instruction
         //end-all literals with address undefined of curr sect,pushed into symbtable with defined address
         //base-do nothing in pass 1
         //extdef-Update global external def table, add extdef to vector of extdef for curr section
         //extref-add extref to vector of extref for curr section
         //csect-should have added littab updation to this,assumed that ltorg would be written at the end of every section
         //csect-similar to start, locctr=0, sect name in extdef, count of sects updated,all vectors of symbtab updated so that they are of size count
         //instruction-check for literals,update locctr
         if(v[0]=="END") 
         {
          for(int i=0;i<literal[curr_sect].size();i++)
        {
          string z=literal[curr_sect][i];
          if(LITTAB[z].address=="?")
          {
          if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.relative=1;
            s.value=locctr;
            SYMBTAB[z].push_back(s);
          }
          else{
          while(SYMBTAB[z].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[z].push_back(s);
          }
          SYMBTAB[z][sect_count[curr_sect]].address=locctr;
          SYMBTAB[z][sect_count[curr_sect]].value=locctr;
          SYMBTAB[z][sect_count[curr_sect]].relative=1;
          SYMBTAB[z][sect_count[curr_sect]].exists='y';
          }
            LITTAB[z].address=locctr;
            intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<"   *   "<<LITTAB[z].value<<endl;
            if(z[0]=='C') locctr=add(locctr,z.length()-3);
            else if(z[0]=='X') locctr=add(locctr,(z.length()-3)/2);
          }
        }
        intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t    "<<v[1]<<endl;
        END=1;
         }
         else if(v[0]=="BASE") intermediate<<"    "<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<endl;
         else if(v[0]=="EXTDEF" || v[0]=="EXTREF")
         {
           if(v[0]=="EXTDEF")
           {
             string y="";
             for(int i=0;i<v[1].size();i++)
             {
              if(v[1][i]!=',') y+=v[1][i];
              else{
                EXTDEF[y].exists='y';
                EXTDEF[y].section=curr_sect;
                extdef[curr_sect].push_back(y);
                y="";
              }
             }
             EXTDEF[y].exists='y';
             EXTDEF[y].section=curr_sect;
             extdef[curr_sect].push_back(y);
             intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t    "<<v[1]<<endl;
           }
           else if(v[0]=="EXTREF")
           {
             string y="";
             for(int i=0;i<v[1].size();i++)
             {
              if(v[1][i]!=',') y+=v[1][i];
              else{
                extref[curr_sect].push_back(y);
                //cout<<y<<endl;
                y="";
              }
             }
             extref[curr_sect].push_back(y);
             //cout<<y<<endl<<endl;
             intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t    "<<v[1]<<endl;
             //cout<<locctr<<endl;
           }
         }
         else if(v[1]=="CSECT")
         {
          curr_sect=v[0];
          EXTDEF[curr_sect].exists='y';
          EXTDEF[curr_sect].section=curr_sect;
          //cout<<cnt<<endl;
          sect_count[v[0]]=cnt;
          //cout<<curr_sect<<" "<<sect_count[v[0]]<<endl;
          cnt++;
          locctr="00000";
          intermediate<<locctr<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<endl;
          for(auto it:SYMBTAB)
          {
            struct_symbol s;
            s.address="0000";
            s.exists='n';
            s.relative=0;
            s.value="0";
            SYMBTAB[it.first].push_back(s);
          }
          for(int i=0;i<cnt-1;i++)
          {
            struct_symbol s;
            s.address="0000";
            s.exists='n';
            s.relative=0;
            s.value="0";
            SYMBTAB[curr_sect].push_back(s);
          }
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.relative=1;
            s.value=locctr;
            SYMBTAB[curr_sect].push_back(s);
         }
         else{
          if(v[1][0]=='=')
          {
            string d=extract(v[1]);
            bool chk=0;
            for(auto it:LITTAB)
            { 
                if(it.first==d) chk=1;
             }
            if(!chk)
           {
              LITTAB[d].exists='y';
              if(d!="*") LITTAB[d].value=d;
              else  LITTAB[d].value=locctr;
              LITTAB[d].address="?";
              literal[curr_sect].push_back(d);
           }
          }
          if(v[0][0]=='+')
          {
            intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t"<<v[1]<<endl;
            string y;
            y=add(locctr,4);
            locctr=y;
          }
          else{
            intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<"\t\t    "<<v[1]<<endl;
            int form=OPTAB[v[0]].format;
            string y=add(locctr,form);
            locctr=y;
           }
          
           }
      }
      else if(v.size()==1 && v[0]!="LTORG") 
      {
        //vector size-1-rsub instruction
        intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<v[0]<<endl;
        string y=add(locctr,3);
        locctr=y;
      }
      else if(v.size()==1 && v[0]=="LTORG") 
      {
        //ltorg update all undefineded literals push to symbtab
        intermediate<<"LTORG"<<endl;
        for(int i=0;i<literal[curr_sect].size();i++)
        {
          string z=literal[curr_sect][i];
          //cout<<z<<endl;
          if(LITTAB[z].address=="?")
          {
          if(sect_count[curr_sect]==0)
          {
            struct_symbol s;
            s.address=locctr;
            s.exists='y';
            s.value=locctr;
            s.relative=0;
            SYMBTAB[z].push_back(s);
          }
          else{
          while(SYMBTAB[z].size()<cnt)
          {
            struct_symbol s;
            SYMBTAB[z].push_back(s);
          }
          SYMBTAB[z][sect_count[curr_sect]].address=locctr;
          SYMBTAB[z][sect_count[curr_sect]].value=locctr;
          SYMBTAB[z][sect_count[curr_sect]].exists='y';
          SYMBTAB[z][sect_count[curr_sect]].relative=0;
          }
            LITTAB[z].address=locctr;
            intermediate<<locctr<<"\t\t"<<"     "<<"\t\t"<<"   *   "<<LITTAB[z].value<<endl;
            if(z[0]=='C') locctr=add(locctr,z.length()-3);
            else if(z[0]=='X') locctr=add(locctr,(z.length()-3)/2);
          }
        }
      }
    }
  }
}
