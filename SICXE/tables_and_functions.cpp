#include<iostream>
#include<map>
#include<string>
#include<algorithm>
#include<vector>
#include<cstdint>
using namespace std;

map<char,int> hex_to_int;
map<int,char> int_to_hex;
map<char,string> hex_to_binary;
map<char,char> hex_complement;
struct opcode_struct{
    string opcode;
    int format;
    char exists;
    opcode_struct(){
      opcode="undefined";
      format=0;
      exists='n';
    }
};

struct struct_symbol{
     string address;
     char exists;
     string value;
     bool relative;
     struct_symbol(){
       address="0";
       exists='n';
       relative=0;
       value="?";
     }
};

struct struct_extdef{
     char exists;
     string section;
     struct_extdef(){
       exists='n';
       section="0";
     }
};
struct struct_literal{
    string value;
    string address;
    char exists;
    struct_literal(){
      value="";
      address="?";
      exists='n';
    }
};

struct struct_register{
     char num;
     char exists;
     struct_register(){
       num = 'F';
       exists='n';
     }
};
map<string,opcode_struct> OPTAB;
map<string,struct_register> REGTAB;
map<string,struct_literal> LITTAB;
map<string,vector<struct_symbol>> SYMBTAB;
map<string,struct_extdef> EXTDEF;
map<string,vector<string>> EXTREF;
string locctr="00000";
void load_REGTAB(){
  REGTAB["A"].num='0';
  REGTAB["A"].exists='y';

  REGTAB["X"].num='1';
  REGTAB["X"].exists='y';

  REGTAB["L"].num='2';
  REGTAB["L"].exists='y';

  REGTAB["B"].num='3';
  REGTAB["B"].exists='y';

  REGTAB["S"].num='4';
  REGTAB["S"].exists='y';

  REGTAB["T"].num='5';
  REGTAB["T"].exists='y';

  REGTAB["F"].num='6';
  REGTAB["F"].exists='y';

  REGTAB["PC"].num='8';
  REGTAB["PC"].exists='y';

  REGTAB["SW"].num='9';
  REGTAB["SW"].exists='y';
}

void load_OPTAB(){
  OPTAB["ADD"].opcode="18";
  OPTAB["ADD"].format=3;
  OPTAB["ADD"].exists='y';

  OPTAB["ADDF"].opcode="58";
  OPTAB["ADDF"].format=3;
  OPTAB["ADDF"].exists='y';

  OPTAB["ADDR"].opcode="90";
  OPTAB["ADDR"].format=2;
  OPTAB["ADDR"].exists='y';

  OPTAB["AND"].opcode="40";
  OPTAB["AND"].format=3;
  OPTAB["AND"].exists='y';

  OPTAB["CLEAR"].opcode="B4";
  OPTAB["CLEAR"].format=2;
  OPTAB["CLEAR"].exists='y';

  OPTAB["COMP"].opcode="28";
  OPTAB["COMP"].format=3;
  OPTAB["COMP"].exists='y';

  OPTAB["COMPF"].opcode="88";
  OPTAB["COMPF"].format=3;
  OPTAB["COMPF"].exists='y';

  OPTAB["COMPR"].opcode="A0";
  OPTAB["COMPR"].format=2;
  OPTAB["COMPR"].exists='y';

  OPTAB["DIV"].opcode="24";
  OPTAB["DIV"].format=3;
  OPTAB["DIV"].exists='y';

  OPTAB["DIVF"].opcode="64";
  OPTAB["DIVF"].format=3;
  OPTAB["DIVF"].exists='y';

  OPTAB["DIVR"].opcode="9C";
  OPTAB["DIVR"].format=2;
  OPTAB["DIVR"].exists='y';

  OPTAB["FIX"].opcode="C4";
  OPTAB["FIX"].format=1;
  OPTAB["FIX"].exists='y';

  OPTAB["FLOAT"].opcode="C0";
  OPTAB["FLOAT"].format=1;
  OPTAB["FLOAT"].exists='y';

  OPTAB["HIO"].opcode="F4";
  OPTAB["HIO"].format=1;
  OPTAB["HIO"].exists='y';

  OPTAB["J"].opcode="3C";
  OPTAB["J"].format=3;
  OPTAB["J"].exists='y';

  OPTAB["JEQ"].opcode="30";
  OPTAB["JEQ"].format=3;
  OPTAB["JEQ"].exists='y';

  OPTAB["JGT"].opcode="34";
  OPTAB["JGT"].format=3;
  OPTAB["JGT"].exists='y';

  OPTAB["JLT"].opcode="38";
  OPTAB["JLT"].format=3;
  OPTAB["JLT"].exists='y';

  OPTAB["JSUB"].opcode="48";
  OPTAB["JSUB"].format=3;
  OPTAB["JSUB"].exists='y';

  OPTAB["LDA"].opcode="00";
  OPTAB["LDA"].format=3;
  OPTAB["LDA"].exists='y';

  OPTAB["LDB"].opcode="68";
  OPTAB["LDB"].format=3;
  OPTAB["LDB"].exists='y';

  OPTAB["LDCH"].opcode="50";
  OPTAB["LDCH"].format=3;
  OPTAB["LDCH"].exists='y';

  OPTAB["LDF"].opcode="70";
  OPTAB["LDF"].format=3;
  OPTAB["LDF"].exists='y';

  OPTAB["LDL"].opcode="08";
  OPTAB["LDL"].format=3;
  OPTAB["LDL"].exists='y';

  OPTAB["LDS"].opcode="6C";
  OPTAB["LDS"].format=3;
  OPTAB["LDS"].exists='y';

  OPTAB["LDT"].opcode="74";
  OPTAB["LDT"].format=3;
  OPTAB["LDT"].exists='y';

  OPTAB["LDX"].opcode="04";
  OPTAB["LDX"].format=3;
  OPTAB["LDX"].exists='y';

  OPTAB["LPS"].opcode="D0";
  OPTAB["LPS"].format=3;
  OPTAB["LPS"].exists='y';

  OPTAB["MUL"].opcode="20";
  OPTAB["MUL"].format=3;
  OPTAB["MUL"].exists='y';

  OPTAB["MULF"].opcode="60";
  OPTAB["MULF"].format=3;
  OPTAB["MULF"].exists='y';

  OPTAB["MULR"].opcode="98";
  OPTAB["MULR"].format=2;
  OPTAB["MULR"].exists='y';

  OPTAB["NORM"].opcode="C8";
  OPTAB["NORM"].format=1;
  OPTAB["NORM"].exists='y';

  OPTAB["OR"].opcode="44";
  OPTAB["OR"].format=3;
  OPTAB["OR"].exists='y';

  OPTAB["RD"].opcode="D8";
  OPTAB["RD"].format=3;
  OPTAB["RD"].exists='y';

  OPTAB["RMO"].opcode="AC";
  OPTAB["RMO"].format=2;
  OPTAB["RMO"].exists='y';

  OPTAB["RSUB"].opcode="4F";
  OPTAB["RSUB"].format=3;
  OPTAB["RSUB"].exists='y';

  OPTAB["SHIFTL"].opcode="A4";
  OPTAB["SHIFTL"].format=2;
  OPTAB["SHIFTL"].exists='y';

  OPTAB["SHIFTR"].opcode="A8";
  OPTAB["SHIFTR"].format=2;
  OPTAB["SHIFTR"].exists='y';

  OPTAB["SIO"].opcode="F0";
  OPTAB["SIO"].format=1;
  OPTAB["SIO"].exists='y';

  OPTAB["SSK"].opcode="EC";
  OPTAB["SSK"].format=3;
  OPTAB["SSK"].exists='y';

  OPTAB["STA"].opcode="0C";
  OPTAB["STA"].format=3;
  OPTAB["STA"].exists='y';

  OPTAB["STB"].opcode="78";
  OPTAB["STB"].format=3;
  OPTAB["STB"].exists='y';

  OPTAB["STCH"].opcode="54";
  OPTAB["STCH"].format=3;
  OPTAB["STCH"].exists='y';

  OPTAB["STF"].opcode="80";
  OPTAB["STF"].format=3;
  OPTAB["STF"].exists='y';

  OPTAB["STI"].opcode="D4";
  OPTAB["STI"].format=3;
  OPTAB["STI"].exists='y';

  OPTAB["STL"].opcode="14";
  OPTAB["STL"].format=3;
  OPTAB["STL"].exists='y';

  OPTAB["STS"].opcode="7C";
  OPTAB["STS"].format=3;
  OPTAB["STS"].exists='y';

  OPTAB["STSW"].opcode="E8";
  OPTAB["STSW"].format=3;
  OPTAB["STSW"].exists='y';

  OPTAB["STT"].opcode="84";
  OPTAB["STT"].format=3;
  OPTAB["STT"].exists='y';

  OPTAB["STX"].opcode="10";
  OPTAB["STX"].format=3;
  OPTAB["STX"].exists='y';

  OPTAB["SUB"].opcode="1C";
  OPTAB["SUB"].format=3;
  OPTAB["SUB"].exists='y';

  OPTAB["SUBF"].opcode="5C";
  OPTAB["SUBF"].format=3;
  OPTAB["SUBF"].exists='y';

  OPTAB["SUBR"].opcode="94";
  OPTAB["SUBR"].format=2;
  OPTAB["SUBR"].exists='y';

  OPTAB["SVC"].opcode="B0";
  OPTAB["SVC"].format=2;
  OPTAB["SVC"].exists='y';

  OPTAB["TD"].opcode="E0";
  OPTAB["TD"].format=3;
  OPTAB["TD"].exists='y';

  OPTAB["TIO"].opcode="F8";
  OPTAB["TIO"].format=1;
  OPTAB["TIO"].exists='y';

  OPTAB["TIX"].opcode="2C";
  OPTAB["TIX"].format=3;
  OPTAB["TIX"].exists='y';

  OPTAB["TIXR"].opcode="B8";
  OPTAB["TIXR"].format=2;
  OPTAB["TIXR"].exists='y';

  OPTAB["WD"].opcode="DC";
  OPTAB["WD"].format=3;
  OPTAB["WD"].exists='y';
}

void hex_complement_map()
{
  hex_complement['0']='F';
  hex_complement['1']='E';
  hex_complement['2']='D';
  hex_complement['3']='C';
  hex_complement['4']='B';
  hex_complement['5']='A';
  hex_complement['6']='9';
  hex_complement['7']='8';
  hex_complement['8']='7';
  hex_complement['9']='6';
  hex_complement['A']='5';
  hex_complement['B']='4';
  hex_complement['C']='3';
  hex_complement['D']='2';
  hex_complement['E']='1';
  hex_complement['F']='0';
}
void hex_to_int_map()
{
    hex_to_int['0']=0;
    hex_to_int['1']=1;
    hex_to_int['2']=2;
    hex_to_int['3']=3;
    hex_to_int['4']=4;
    hex_to_int['5']=5;
    hex_to_int['6']=6;
    hex_to_int['7']=7;
    hex_to_int['8']=8;
    hex_to_int['9']=9;
    hex_to_int['A']=10;
    hex_to_int['B']=11;
    hex_to_int['C']=12;
    hex_to_int['D']=13;
    hex_to_int['E']=14;
    hex_to_int['F']=15;

}
void hex_to_binary_map()
{
    hex_to_binary['0']="0000";
    hex_to_binary['1']="0001";
    hex_to_binary['2']="0010";
    hex_to_binary['3']="0011";
    hex_to_binary['4']="0100";
    hex_to_binary['5']="0101";
    hex_to_binary['6']="0110";
    hex_to_binary['7']="0111";
    hex_to_binary['8']="1000";
    hex_to_binary['9']="1001";
    hex_to_binary['A']="1010";
    hex_to_binary['B']="1011";
    hex_to_binary['C']="1100";
    hex_to_binary['D']="1101";
    hex_to_binary['E']="1110";
    hex_to_binary['F']="1111";

}
void int_to_hex_map()
{
    int_to_hex[0]='0';
    int_to_hex[1]='1';
    int_to_hex[2]='2';
    int_to_hex[3]='3';
    int_to_hex[4]='4';
    int_to_hex[5]='5';
    int_to_hex[6]='6';
    int_to_hex[7]='7';
    int_to_hex[8]='8';
    int_to_hex[9]='9';
    int_to_hex[10]='A';
    int_to_hex[11]='B';
    int_to_hex[12]='C';
    int_to_hex[13]='D';
    int_to_hex[14]='E';
    int_to_hex[15]='F';
}

int hex_to_int_conv(string a)
{
  int c=1;
  int ans=0;
  for(int i=a.length()-1;i>=0;i--)
  {
    ans+=c*(hex_to_int[a[i]]);
    c*=16;
  }
  return ans;
  }

string add(string s,int n)
{
    int m=s.length();
    int i=m-1;
    string ans="";
    while(i>=0)
    {
      char b=s[i];
      int x=hex_to_int[b]+n;
      n=x/16;
      x=x%16;
      ans+=int_to_hex[x];
      i--;
      if(i==-1 && n!=0)
      {
        while(n>0)
        {
            ans+=int_to_hex[n%16];
            n=n/16;
        }
      }
    }
    reverse(ans.begin(),ans.end());
    return ans;
}
string add_hex(string a,string b)
{
    if(a=="error" || b=="error") return "error";
    if(b.length()>a.length()) 
    {
        string temp=a;
        a=b;
        b=temp;
    }
    int n=a.length();
    reverse(b.begin(),b.end());
    while(b.length()<n) b+='0';
    reverse(b.begin(),b.end());
    int c=0;
    string ans="";
    for(int i=n-1;i>=0;i--)
    {
        int j=hex_to_int[b[i]];
        int k=hex_to_int[a[i]];
        int y=(j+k+c);
        c=y/16;
        ans+=int_to_hex[y%16];
    }
    if(c!=0) ans+=int_to_hex[c];
    reverse(ans.begin(),ans.end());
    return ans;
}
string hex_to_binary_conv(string s)
{
   string x="";
   for(int i=0;i<s.length();i++)
   {
     x+=hex_to_binary[s[i]];
   }
   return x;
}
string binary_to_hex_conv(string a)
{
  int i=0;
  reverse(a.begin(),a.end());
  while(a.length()%4!=0) a+='0';
  reverse(a.begin(),a.end());
  string ans="";
  while(i<a.length())
  {
    string x="";
    x+=a[i];
    x+=a[i+1];
    x+=a[i+2];
    x+=a[i+3];
    for(auto it:hex_to_binary) 
    {
      if(it.second==x)
      {
        ans+=it.first;
        break;
      }
    }
    i+=4;
  }
  return ans;
}
string int_to_hex_conv(int a)
{
  string x="";
  int n=a;
  if(a<0) a*=-1;
   else if (a==0)
   {
    x+='0';
   }
   while(a>0)
   {
     x+=int_to_hex[a%16];
     a=a/16;
   }
   reverse(x.begin(),x.end());
   if(n<0)
   {
    string sol="";
    n=0;
    while (n<x.length()) {
          sol+= hex_complement[x[n]];
          n++;
      }
      sol=add(sol,1);
      return sol;
   }
   return x;
}
string extract(string s)
{
    int n=s.length();
    int i=1;
    string sol="";
    while(i<n)
    {
       sol+=s[i];
       i++; 
    }
    return sol;
}
string format2_reg(string s)
{
  string r1;
  r1=REGTAB[s].num;
  return r1;
}

void load_table()
{
    load_OPTAB();
    load_REGTAB();
    hex_to_int_map();
    hex_to_binary_map();
    int_to_hex_map();
    hex_complement_map();
}

string format2(string a,string s)
{
  string b="";
  if(a=="SHIFTL" || a=="SHIFTR")
  {
    if(s.length()>=3)
    {
      string x="";
      x+=s[0];
      if(REGTAB[x].exists=='n') b="error";
      else{
        b+=format2_reg(x);
        x="";
        for(int i=2;i<s.length();i++) x+=s[i];
        int p=stoi(x);
        p--;
        if(p<0 || p>15) b="error";
        else b+=int_to_hex[p];
      }
    }
    else b="error";
  }
  else if(s.length()==3)
  {
      string x="";
      x+=s[0];
      if(REGTAB[x].exists=='n') b="error";
      else{
        b+=format2_reg(x);
        x="";
        x+=s[2];
        if(REGTAB[x].exists=='n') b="error";
        else b+=format2_reg(x);
      }
  }
  else if(s.length()<=2)
  {
      if(a=="SVC")
      {
        if(s.length()==2 && stoi(s)>15) b="error";
        else{
         b=int_to_hex[stoi(s)];
        b+='0';}
      }
      else{
        string x="";
        x+=s[0];
        if(REGTAB[x].exists=='n') b="error";
        else{
        b=format2_reg(x);
        b+='0';}
      }
  }
  return b;
}

bool check_num(string s)
{
   for( int i = 0; i < s.length(); i++ ) {
      if( !isdigit( s[i] )) {
         return false;
      }
   }
   return true;
}

//value of a-b (both in hex)
string calculate_relative(string a,string b,int sz)
{
  int y=hex_to_int_conv(b);
  int x=hex_to_int_conv(a);
  //if(x==3 && y==23) cout<<
  x=x-y;
  if(x>=0) 
  {
    string ans=int_to_hex_conv(x);
    while(ans.length()<sz) ans='0'+ans;
    if(ans.length()>sz) return "error";
    return ans;
  }
  else
  {
    string ans=int_to_hex_conv(x);
    while(ans.length()<sz) ans='F'+ans;
    if(ans.length()>sz) return "error";
    return ans;
  }
}

string clean_operand(string a)
{
  string sol="";
  int i=a.length()-3;
  while(i>=0)
  {
    sol+=a[i];
    i--;
  }
  reverse(sol.begin(),sol.end());
  return sol;
}
bool chk_symb(string y,int sect)
{
  for(auto it: SYMBTAB) if(it.first==y && it.second[sect].exists=='y') return 1;
  return 0;
}

bool chk_ext_def(string y)
{
  for(auto it:EXTDEF) if(it.first==y) return 1;
  return 0;
}

bool chk_ext_ref(string y,map<string,vector<string>> extref,string sect)
{
  for(int i=0;i<extref[sect].size();i++) if(extref[sect][i]==y) return 1;
  return 0;
}

bool chk_opt(string y)
{
  for(auto it:OPTAB) if(it.first==y) return 1;
  return 0;
}

string mul(string hex,int n)
{
  int k=hex_to_int_conv(hex);
  k*=n;
  return int_to_hex_conv(k);
}

string div(string hex,int n)
{
  int k=hex_to_int_conv(hex);
  k/=n;
  return int_to_hex_conv(k);
}

pair<vector<string>,string> expressionparse(string y,string sect,int n,int f,bool include_curr_sect,map<string,vector<string>> extref)
{
  vector<string> q;
  string x="";
  char start;
  if(y[0]!='-') start='+';
  else start='-';
  x+=start;
  for(int i=0;i<y.length();i++)
  {
    if(y[i]=='+' || y[i]=='-')
    {
      q.push_back(x);
      x="";
      x+=y[i];
    }
    else x+=y[i];
  }
  q.push_back(x);
  //cout<<endl;
  string ans="0";
  int val=0;
  while(ans.length()<f) ans+='0';
  bool b=0;
  vector<string> mod;
  string k="";
  vector<string> plus;
  vector<string> minus;
  for(int i=0;i<q.size();i++)
  {
     string t=extract(q[i]);
     if(check_num(t)) 
     {
       //cout<<"j"<<t<<endl;
       int p=stoi(t);
       if(q[i][0]=='+') val+=p;
       else val-=p;
     }
     else if(t=="*")
     {
        int u=hex_to_int_conv(locctr);
        if(q[i][0]=='+') val+=u;
        else val-=u;
     }
     else if(chk_symb(t,n))
     {
       bool z=SYMBTAB[t][n].relative;
       int u=hex_to_int_conv(SYMBTAB[t][n].value);
       //cout<<u<<endl;
       string v="";
       if(z)
       {
        if(q[i][0]=='+') plus.push_back(q[i]);
        else minus.push_back(q[i]);
       }
       if(q[i][0]=='-') val-=u;
       else val+=u;
     }
     else
     {
       //cout<<"k"<<t<<endl;
       if(chk_ext_ref(t,extref,sect))
       {
         if(!include_curr_sect) return make_pair(mod,"error");
         string z="";
         z+=q[i];
         mod.push_back(z);
       }
       else 
       {
        //cout<<"k"<<t<<endl;
        return make_pair(mod,"error"); 
       }
     }
  }
  if(include_curr_sect){
  string v="";
  if(plus.size()<minus.size())
  {
    for(int i=plus.size();i<minus.size();i++) mod.push_back(v+'-'+sect);
  }
  else
  {
    for(int i=minus.size();i<plus.size();i++) mod.push_back(v+'+'+sect);
  }}
  //cout<<val<<endl;
  ans=int_to_hex_conv(val);
  while(ans.length()<f)
  {
    if(val>=0) ans='0'+ans;
    else ans='F'+ans;
  }
  return make_pair(mod,ans);
}

int rel(string y,int n)
{
 vector<string> q;
  string x="";
  char start;
  if(y[0]!='-') start='+';
  else start='-';
  x+=start;
  for(int i=0;i<y.length();i++)
  {
    if(y[i]=='+' || y[i]=='-')
    {
      q.push_back(x);
      x="";
      x+=y[i];
    }
    else x+=y[i];
  }
  q.push_back(x);
  int cp=0;
  int cm=0;
  for(int i=0;i<q.size();i++)
  {
     string t=extract(q[i]);
     if(chk_symb(t,n))
     {
       bool z=SYMBTAB[t][n].relative;
       if(z)
       {
          if(q[i][0]=='+') cp++;
          else cm--;
       }
     }
     else if(t=="*") 
     {
       if(q[i][0]=='+') cp++;
       else cm--;
     }
  }
  if(cp!=cm) return 1;
  else return 0;
}

