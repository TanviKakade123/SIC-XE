#include "pass2.cpp"
using namespace std;

int main()
{
  cout<<"**The input_File and Executable should be in the same folder**"<<endl<<endl;
  cout<<"Enter name of the input_File:";
  cin>>filename;
    firstpass();
    pass2();
    ofstream objprogram("objprog"+filename);
    if(!err)
    {
    for(auto j:sect_count)
    {
    string sect=j.first;
    string start=header[sect][0];
    string finish=header[sect][2];
    string name=header[sect][1];
    string len=calculate_relative(finish,start,6);
    //HEADER
    objprogram<<"H"<<name<<"^"<<start<<"^"<<len<<endl;
    //DEFINE RECORD
    if(extdef[sect].size()!=0){
    string y="D";
    for(int i=0;i<extdef[sect].size();i++)
    {
        if(y.length()==73)
        {
            objprogram<<y<<endl;
            y="D";
            y+='^';
            y+=extdef[sect][i];
            string z=calculate_relative(SYMBTAB[extdef[sect][i]][j.second].address,start,6);
            z+='^';
            y+=z;
        }
        else{
        y+='^';
        y+=extdef[sect][i];
        string z=calculate_relative(SYMBTAB[extdef[sect][i]][j.second].address,start,6);
        y+=z;
        }

    }
    objprogram<<y<<endl;}
    //REFERENCE RECORD
    if(extref[sect].size()!=0)
    {
    string y="R";
    for(int i=0;i<extref[sect].size();i++)
    {
         if(y.length()==73)
        {
            objprogram<<y<<endl;
            y="R";
            y+=extref[sect][i];
        }
        else 
        {
            y+='^';
            y+=extref[sect][i];
        }
    }
    objprogram<<y<<endl;
    }
    //TEXT RECORD
    int num=0;
    int i=0;
    int n=objcode[sect].size();
    string y;
    y="T^";
    y+=start;
    y+="^";
    y+="  ";
    bool last_res=0;
    while(i<n)
    {
        if((objcode[sect][i].second=="RESW" || objcode[sect][i].second=="RESB") && !last_res)
        {
            num=num/2;
            string z=int_to_hex_conv(num);
            if(z.length()==1) z='0'+z;
            y[9]=z[0];
            y[10]=z[1];
            objprogram<<y<<endl;
            last_res=1;
            num=60;
        }
        else if(objcode[sect][i].second!="RESW" && objcode[sect][i].second!="RESB")
        {
        num+=objcode[sect][i].second.length();
        if(num>60)
        {
            if(!last_res)
            {
            num-=objcode[sect][i].second.length();
            num=num/2;
            string z=int_to_hex_conv(num);
            if(z.length()==1) z='0'+z;
            y[9]=z[0];
            y[10]=z[1];
            objprogram<<y<<endl;
            }
            y="T^0";
            y+=objcode[sect][i].first;
            y+="^";
            y+="  ";
            y+="^";
            y+=objcode[sect][i].second;
            num=objcode[sect][i].second.length();
            last_res=0;
        }
        else
        {
            y+="^";
            y+=objcode[sect][i].second;
            last_res=0;
        }
        }
        i++;
    }
    if(!last_res) 
    {
        num=num/2;
        string z=int_to_hex_conv(num);
        if(z.length()==1) z='0'+z;
        y[9]=z[0];
        y[10]=z[1];
        objprogram<<y<<endl;
    }
    //MODIFICATION RECORD
    for(int i=0;i<modification[sect].size();i++)
    {
        string loc=modification[sect][i];
        objprogram<<"M"<<"^"<<loc<<endl;
    }
    //END RECORD
    objprogram<<"E";
    if(sect==first) objprogram<<"^"<<start;
    objprogram<<endl;
    cout<<"FINISHED PASS 1 AND PASS 2 SUCCESSFULLY"<<endl;
    }
    }
    else objprogram<<"***************************ERROR***********************************"<<endl;
}
