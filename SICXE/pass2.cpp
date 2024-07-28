#include "pass1.cpp"
using namespace std;
//objcode for each section, stores  pair of location and objcode
map<string,vector<pair<string,string>>> objcode;
map<string,vector<string>> modification;
//header for each sect,stores its name start and end address, basically everything required for header file
map<string,vector<string>> header;
string first;
void pass2()
{
    ifstream intermediate("intermediate"+filename);
    ofstream listing("listing"+filename),error("error"+filename);
    string curr_sect="";
    if(intermediate.is_open() && listing.is_open() && error.is_open())
    {
        string PC="0000";
        bool base=0;
        string base_relative="0";
        while(intermediate)
        {
            vector<string> v;
            string temp;
            getline(intermediate,temp);
            int j=0;
            int m=temp.length();
            string y="";
            while(j<m)
            {
               if(temp[j]!=' ' && temp[j]!='\t') y+=temp[j];
               else
               {
                 if(y.size()!=0)
                 {
                   v.push_back(y);
                   y="";
                 }
            }
            j++;
            }
            v.push_back(y);
            if(v[0]=="ADDRESS") listing<<temp<<"\t\tOBJECT CODE"<<endl;
            else if(v.size()==1) listing<<v[0]<<endl;  //LTORG
            else if(v.size()==2 && v[0]!="BASE") 
            {
                //instruction with address as first element in v, instruction as second element
                //objcode is pair of objcode along with the term it represents
                //instruction may be format 3 or format 1
                if(!chk_opt(v[1])) 
                {
                    err=1;
                    error<<"Invalid Instruction"<<endl;
                    listing<<"***************************ERROR***********************************"<<endl;
                }
                else{
                if(v[1]=="RSUB") PC=add_hex(PC,"3");
                else PC=add_hex(PC,"1");
                string z=OPTAB[v[1]].opcode;
                if(v[1]=="RSUB") z+="0000";
                listing<<temp<<"\t\t\t\t\t"<<z<<endl;
                objcode[curr_sect].push_back(make_pair(v[0],z));}
            }
            else if(v.size()==2 && v[0]=="BASE") 
            {
                //if base, make variable base relative as the value of the symbol, set bool base to true
                base=1;
                if(!chk_symb(v[1],sect_count[curr_sect]))
                {
                    err=1;
                    error<<"SYMBOL NOT DEFINED"<<endl;
                    listing<<"***************************ERROR***********************************"<<endl;
                    break;
                }
                else 
                {
                    listing<<temp<<endl;
                    base_relative=SYMBTAB[v[1]][sect_count[curr_sect]].address;
                }
            }
            else if(v.size()==3)
            {
                //v size-3 :start,*(literal def),csect,extref,extdef,end,equ,instruction with no label
                //start-header for curr sect updated with the name and starting address, end address will be found later
                //*- literal definition, create obj code corresponding to literal value,update pc, 
                //csect-put last value of locctr as end in header rec of prev section, now update start and name in header for curr sect
                //extref, extdef nothing to be  done
                //end-put finish in finish of curr sect
                //equ-nothing to be done
                //instruction-identify values of n,i,x,b,p,e, check format from optab
                //format 2 instructions-cannot be of immediate or indirect so no # or @ should be there in instruction,check valid regs
                //format 3 instructions: last word in instruction may hav #,@,=(in case of literals), or none
                if(v[1]=="START") 
                {
                    PC=v[2];
                    curr_sect=v[0];
                    listing<<temp<<endl;
                    string start=v[2];
                    string name=v[0];
                    first=v[0];
                    if(name.length()>6) 
                    {
                        err=1;
                        error<<"NAME OF PROGRAM EXCEEDS 6 BYTES"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        while(name.length()<6) name+=' ';
                    }
                    if(start.length()>=6) 
                    {
                        err=1;
                        error<<"INVALID STARTING ADDRESS"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        while(start.length()<6) start='0'+start;
                    }
                    header[curr_sect].push_back(start);
                    header[curr_sect].push_back(name);
                }
                else if(v[1]=="*")
                {
                    if(v[2][0]=='C')
                    {
                        PC=add(PC,v[2].length()-3);
                        string g="";
                        for(int i=2;i<v[2].length()-1;i++)
                            {
                                int k=v[2][i];
                                string h=int_to_hex_conv(k);
                                if(h.length()==1) h='0'+h;
                                g+=h;
                            }
                            listing<<temp<<"\t\t"<<g<<endl;
                            objcode[curr_sect].push_back(make_pair(v[0],g));
                    }
                    else if(v[2][0]=='X')
                    {
                        PC=add(PC,(v[2].length()-3)/2);
                        string g="";
                        for(int i=2;i<v[2].length()-1;i++)
                            {
                                g+=v[2][i];
                            }
                            listing<<temp<<"\t\t"<<g<<endl;
                            objcode[curr_sect].push_back(make_pair(v[0],g));
                    }
                    else{
                        //cout<<v[2]<<endl;
                        PC=add(PC,3);
                        pair<vector<string>,string> p=expressionparse(v[2],curr_sect,sect_count[curr_sect],6,1,extref);
                        listing<<temp<<"\t\t"<<p.second<<endl;
                        objcode[curr_sect].push_back(make_pair(v[0],p.second));
                        if(p.second=="error") 
                        {
                        err=1;
                        error<<"INVALID EXPRESSION3"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        }
                        else{
                        for(int i=0;i<p.first.size();i++) 
                        {
                            string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                            loc+="06";
                            loc+=p.first[i];
                            modification[curr_sect].push_back(loc);
                        }}
                    }
                }
                else if(v[2]=="CSECT")
                {
                    string finish=PC;
                    PC=v[0];
                    listing<<temp<<endl;
                    while(finish.length()<6) finish='0'+finish;
                    header[curr_sect].push_back(finish);
                    curr_sect=v[1];
                    //cout<<curr_sect<<" "<<sect_count[curr_sect]<<endl;
                    string start=v[0];
                    string name=v[1];
                    if(name.length()>6) 
                    {
                        err=1;
                        error<<"NAME OF PROGRAM EXCEEDS 6 BYTES"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        while(name.length()<6) name+=' ';
                    }
                    if(start.length()>6) 
                    {
                        err=1;
                        error<<"INVALID STARTING ADDRESS"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        while(start.length()<6) start='0'+start;
                    }
                    header[curr_sect].push_back(start);
                    header[curr_sect].push_back(name);
                }
                else if(v[1]=="EXTREF" || v[1]=="EXTDEF")
                {
                    listing<<temp<<endl;
                }
                else if(v[1]=="END")
                {
                    listing<<temp<<endl;
                    string finish=v[0];
                    while(finish.length()<6) finish='0'+finish;
                    header[curr_sect].push_back(finish);
                }
                else if(v[2]=="EQU") listing<<temp<<endl;
                else
                {
                  char n,i,x,b,p,e;
                    n='0';
                    i='0';
                    x='0';
                    b='0';
                    p='0';
                    e='0';
                    if(v[1][0]=='+') e='1';
                    if(v[2][0]=='@') n='1';
                    else if(v[2][0]=='#') i='1';
                    else{
                        n='1';
                        i='1';
                    }
                    string a=",X";
                    auto f=v[2].find(a);
                    if(f!=-1) x='1';
                    if(x=='1')
                    {
                        if((n=='0' && i=='1') || (n=='1' && i=='0')) 
                        {
                            err=1;
                            error<<"Incorrect use of index with immediate or indirect"<<endl;
                            listing<<"***************************ERROR***********************************"<<endl;
                            break;
                        }
                    }
                    string z="";
                    string val="";
                    if(e=='1') z=extract(v[1]);
                    else z=v[1];
                    if(!chk_opt(z)) 
                    {
                        err=1;
                        error<<z<<" INVALID INSTRUCTION"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        int q=0;
                        if(n=='0' && i=='1') q=1;
                        else if(n=='1' && i=='0') q=2;
                        else if(n=='1' && i=='1') q=3;
                        if(OPTAB[z].format==2)
                        {
                            
                            if(q==1 || q==2) 
                            {
                                err=1;
                                error<<"INVALID INSTRUCTION"<<endl;
                                listing<<"***************************ERROR***********************************"<<endl;
                                break;
                            }
                            else
 			                {
                            val=OPTAB[z].opcode;
                            string t=format2(OPTAB[z].opcode,v[2]);
                            if(t=="error")
                            {
                                err=1;
                                error<<"INVALID REGISTER"<<endl;
                                listing<<"***************************ERROR***********************************"<<endl;
                                break;
                            }
                            else
                            {
                                val+=t;
                                PC=add(PC,2);
                                listing<<temp<<"\t\t\t\t"<<val<<endl;
                                objcode[curr_sect].push_back(make_pair(v[0],val));
                            }
                            }

                        }
                        else if(OPTAB[z].format==3)
                        {
                            string t;
                            val=add(OPTAB[z].opcode,q);
                            if(e=='1') PC=add_hex(PC,"4");
                            else PC=add_hex(PC,"3");
                            if(v[2][0]=='#' || v[2][0]=='@' || v[2][0]=='=')
                            {
                                t=extract(v[2]);
                                if(check_num(t))
                                {
                                    string val1="";
                                    val1+=x;
                                    val1+=b;
                                    val1+=p;
                                    val1+=e;
                                    string val2=binary_to_hex_conv(val1);
                                    val+=val2;
                                    //cout<<val<<endl;
                                    val1=t;
                                    if(val1.length()>3 && e=='0') val1="error";
                                    else if(val1.length()>5 && e=='1') val1="error";
                                    else
                                    {
                                        if(e=='1')
                                        {
                                            while(val1.size()<5) val1+='0';
                                        }
                                        else 
                                        {
                                            while(val1.size()<3) val1+='0';
                                        }
                                    }
                                    if(val1=="error")
                                    {
                                        err=1;
                                        error<<"INVALID LENGTH OF IMMEDIATE"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                        break; 
                                    }
                                    val+=val1;
                                    listing<<temp<<"\t\t\t"<<val<<endl;
                                    objcode[curr_sect].push_back(make_pair(v[0],val));
                                }
                                else
                                {
                                    bool k=0;
                                    if(e=='1') k=1;
                                    int num=3;
                                    if(k==1) num+=2; 
                                     pair<vector<string>,string> u=expressionparse(t,curr_sect,sect_count[curr_sect],num,k,extref);
                                     if(u.second=="error") 
                                    {
                                        err=1;
                                         error<<"INVALID EXPRESSION"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                     }
                                     else{
                                        if(e=='0')
                                        {
                                            string rel=calculate_relative(u.second,PC,3);
                                            if(rel=="error" || rel.length()>3)
                                            {
                                            if(base)
                                            {
                                                rel=calculate_relative(u.second,base_relative,3);
                                                if(rel=="error" || rel.length()>3)
                                                {
                                                   err=1;
                                                   error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                   listing<<"***************************ERROR***********************************"<<endl;
                                                   break;  
                                                }
                                                else{
                                                    b='1';
                                                }
                                            }
                                            else
                                            {
                                                err=1;
                                                error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                listing<<"***************************ERROR***********************************"<<endl;
                                                break;  
                                            }
                                            }
                                            else p='1';
                                            string val1="";
                                            val1+=x;
                                            val1+=b;
                                            val1+=p;
                                            val1+=e;
                                            val+=binary_to_hex_conv(val1);
                                            //increase_size(rel,3);
                                            val+=rel;
                                            listing<<temp<<"\t\t"<<val<<endl;
                                            objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                        else
                                        {
                                        string val1="";
                                        val1+=x;
                                        val1+=b;
                                        val1+=p;
                                        val1+=e;
                                        val+=binary_to_hex_conv(val1);
                                        val+=u.second;
                                        for(int i=0;i<u.first.size();i++) 
                                        {
                                              string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                                              //increase_size(loc,5);
                                              loc+="05";
                                              loc+=u.first[i];
                                              modification[curr_sect].push_back(loc);
                                         }
        
                                        listing<<temp<<"\t\t"<<val<<endl;
                                        objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if(x=='1') t=clean_operand(v[2]);
                                else t=v[2];
                                //cout<<"SYMB "<<t<<" "<<sect_count[curr_sect]<<endl;
                                    bool k=0;
                                    if(e=='1') k=1;
                                    int num=3;
                                    if(e=='1') num+=2; 
                                    //cout<<t<<num<<k<<endl;
                                     pair<vector<string>,string> u=expressionparse(t,curr_sect,sect_count[curr_sect],num,k,extref);
                                     if(u.second=="error") 
                                    {
                                        err=1;
                                         error<<"INVALID EXPRESSION"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                        
                                     }
                                     else{
                                        if(e=='0')
                                        {
                                            string rel=calculate_relative(u.second,PC,3);
                                            if(rel=="error" || rel.length()>3)
                                            {
                                            if(base)
                                            {
                                                rel=calculate_relative(u.second,base_relative,3);
                                                if(rel=="error" || rel.length()>3)
                                                {
                                                   err=1;
                                                   error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                   listing<<"***************************ERROR***********************************"<<endl;
                                                   break;  
                                                }
                                                else{
                                                    b='1';
                                                }
                                            }
                                            else
                                            {
                                                err=1;
                                                error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                listing<<"***************************ERROR***********************************"<<endl;
                                                break;  
                                            }
                                            }
                                            else p='1';
                                            string val1="";
                                            val1+=x;
                                            val1+=b;
                                            val1+=p;
                                            val1+=e;
                                            val+=binary_to_hex_conv(val1);
                                            //increase_size(rel,3);
                                            //cout<<rel<<endl;
                                            val+=rel;
                                            listing<<temp<<"\t\t"<<val<<endl;
                                            objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                        else
                                        {
                                        string val1="";
                                        val1+=x;
                                        val1+=b;
                                        val1+=p;
                                        val1+=e;
                                        val+=binary_to_hex_conv(val1);
                                        val+=u.second;
                                        for(int i=0;i<u.first.size();i++) 
                                        {
                                              string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                                              //increase_size(loc,5);
                                              loc+="05";
                                              loc+=u.first[i];
                                              modification[curr_sect].push_back(loc);
                                         }
        
                                        listing<<temp<<"\t\t"<<val<<endl;
                                        objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                     }
                            }
                            }

                        }


                    }

  
                }
            else if(v.size()==4)
            {
                if(v[2]=="RESW" || v[2]=="RESB" || v[2]=="BYTE" || v[2]=="WORD")
                {
                    if(v[2]=="WORD") 
                    {
                        if(v[3][0]=='C')
                        {
                        if(v[3].length()!=6) 
                        {
                            err=1;
                            error<<"INVALID TOO BIG WORD"<<endl;
                            listing<<"***************************ERROR***********************************"<<endl;
                            break;  
                        }
                        PC=add(PC,(v[3].length()-3));
                        string g="";
                        for(int i=2;i<v[3].length()-1;i++)
                            {
                                int k=v[3][i];
                                string h=int_to_hex_conv(k);
                                if(h.length()==1) h='0'+h;
                                g+=h;
                            }
                            listing<<temp<<"\t\t"<<g<<endl;
                            objcode[curr_sect].push_back(make_pair(v[0],g));
                        }
                        else if(v[3][0]=='X')
                        {
                        if(v[3].length()!=9) 
                        {
                            err=1;
                            error<<"INVALID TOO BIG WORD"<<endl;
                            listing<<"***************************ERROR***********************************"<<endl;
                            break;  
                        }
                        PC=add(PC,3);
                        string g="";
                        for(int i=2;i<v[3].length()-1;i++) g+=v[3][i];
                        listing<<temp<<"\t\t"<<g<<endl;
                        objcode[curr_sect].push_back(make_pair(v[0],g)); 
                        }
                        else
                        {
                        PC=add(PC,3);
                        pair<vector<string>,string> p=expressionparse(v[3],curr_sect,sect_count[curr_sect],6,1,extref);
                        listing<<temp<<"\t\t"<<p.second<<endl;
                        objcode[curr_sect].push_back(make_pair(v[0],p.second));
                        if(p.second=="error") 
                        {
                        err=1;
                        error<<"INVALID EXPRESSION6"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        }
                        else{
                        for(int i=0;i<p.first.size();i++) 
                        {
                            string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                            //increase_size(loc,6);
                            loc+="06";
                            loc+=p.first[i];
                            modification[curr_sect].push_back(loc);
                        }
                        }
                        }
                    }
                    else if(v[2]=="BYTE")
                    {
                        if(v[3][0]=='X')
                        {
                            PC=add(PC,(v[3].length()-3)/2);
                            string g="";
                            for(int i=2;i<v[3].length()-1;i++) g+=v[3][i];
                            listing<<temp<<"\t\t"<<g<<endl;
                            for(int j=1;j<g.length();j+=2) 
                            {
                                string rep="";
                                rep+=g[j-1];
                                rep+=g[j];
                                string loc=add(v[0],j/2);
                                objcode[curr_sect].push_back(make_pair(loc,rep));
                            }
                        }
                        else if(v[3][0]=='C')
                        {
                            PC=add(PC,(v[3].length()-3));
                            string g="";
                            for(int i=2;i<v[3].length();i++)
                            {
                                int k=v[3][i];
                                string h=int_to_hex_conv(k);
                                if(h.length()==1) h='0'+h;
                                g+=h;
                            }
                            listing<<temp<<"\t\t"<<g<<endl;
                            for(int j=1;j<g.length();j+=2) 
                            {
                                string rep="";
                                rep+=g[j-1];
                                rep+=g[j];
                                string loc=add(v[0],j/2);
                                objcode[curr_sect].push_back(make_pair(loc,rep));
                            }
                        }
                        else 
                        {
                            err=1;
                            error<<"INVALID TYPE OF BYTE"<<endl;
                            listing<<"***************************ERROR***********************************"<<endl;
                            break; 
                        }
                    }
                    else
                    {
                        if(v[2]=="RESB") PC=add(PC,stoi(v[3]));
                        else if(v[2]=="RESW") PC=add(PC,3*stoi(v[3]));
                        listing<<temp<<endl;
                        objcode[curr_sect].push_back(make_pair(v[0],v[2]));
                        //cout<<v[2]<<endl;

                    }
                }
                else if(v[2]=="EQU") listing<<temp<<endl;
                else
                {
                    char n,i,x,b,p,e;
                    n='0';
                    i='0';
                    x='0';
                    b='0';
                    p='0';
                    e='0';
                    if(v[2][0]=='+') e='1';
                    //cout<<v[2]<<" "<<e<<endl;
                    if(v[3][0]=='@') n='1';
                    else if(v[3][0]=='#') i='1';
                    else
                    {
                        n='1';
                        i='1';
                    }
                    string a=",X";
                    auto f=v[3].find(a);
                    if(f!=-1) x='1';
                    if(x=='1')
                    {
                        if((n=='0' && i=='1') || (n=='1' && i=='0')) 
                        {
                            err=1;
                            error<<"Incorrect use of index with immediate or indirect"<<endl;
                            listing<<"***************************ERROR***********************************"<<endl;
                            break;
                        }
                    }
                    string z="";
                    string val="";
                    if(e=='1') z=extract(v[2]);
                    else z=v[2];
                    if(OPTAB[z].exists=='n') 
                    {
                        err=1;
                        error<<z<<" INVALID INSTRUCTION"<<endl;
                        listing<<"***************************ERROR***********************************"<<endl;
                        break;
                    }
                    else
                    {
                        int q=0;
                        if(n=='0' && i=='1') q=1;
                        else if(n=='1' && i=='0') q=2;
                        else if(n=='1' && i=='1') q=3;
                        if(OPTAB[z].format==2)
                        {           
                            if(q==1 || q==2) 
                            {
                                err=1;
                                error<<"INVALID INSTRUCTION"<<endl;
                                listing<<"***************************ERROR***********************************"<<endl;
                                break;
                            }
                            else{
                            val=OPTAB[z].opcode;
                            string t=format2(OPTAB[z].opcode,v[3]);
                            if(t=="error")
                            {
                                err=1;
                                error<<"INVALID REGISTER"<<endl;
                                listing<<"***************************ERROR***********************************"<<endl;
                                break;
                            }
                            else{
                                val+=t;
                                PC=add(PC,2);
                                listing<<temp<<"\t\t\t\t"<<val<<endl;
                                objcode[curr_sect].push_back(make_pair(v[0],val));
                            }
                            }

                        }
                        else if(OPTAB[z].format==3)
                        {
                            string t;
                            val=add(OPTAB[z].opcode,q);
                            if(e=='1') PC=add_hex(PC,"4");
                            else PC=add_hex(PC,"3");
                            if(v[3][0]=='#' || v[3][0]=='@' || v[3][0]=='=')
                            {
                                t=extract(v[3]);
                                if(check_num(t))
                                {
                                    string val1="";
                                    val1+=x;
                                    val1+=b;
                                    val1+=p;
                                    val1+=e;
                                    string val2=binary_to_hex_conv(val1);
                                    val+=val2;
                                    //cout<<val<<endl;
                                    val1=t;
                                    if(val1.length()>3 && e=='0') val1="error";
                                    else if(val1.length()>5 && e=='1') val1="error";
                                    else
                                    {
                                        if(e=='1')
                                        {
                                            while(val1.size()<5) val1='0'+val1;
                                        }
                                        else
                                        {
                                            while(val1.size()<3) val1='0'+val1;
                                        }
                                    }
                                    if(val1=="error")
                                    {
                                        err=1;
                                        error<<"INVALID LENGTH OF IMMEDIATE"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                        break; 
                                    }
                                    val+=val1;
                                    listing<<temp<<"\t\t\t"<<val<<endl;
                                    objcode[curr_sect].push_back(make_pair(v[0],val));
                                }
                                else{
                                    bool k=0;
                                    if(e=='1') k=1;
                                    int num=3;
                                    if(k==1) num+=2; 
                                     pair<vector<string>,string> u=expressionparse(t,curr_sect,sect_count[curr_sect],num,k,extref);
                                     if(u.second=="error") 
                                    {
                                        err=1;

                                         error<<"INVALID EXPRESSION"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                     }
                                     else{
                                        if(e=='0')
                                        {
                                            //if(v[3]=="EOF") cout<<u.second<<" "<<PC<<endl;
                                            string rel=calculate_relative(u.second,PC,3);
                                            if(rel=="error" || rel.length()>3)
                                            {
                                            if(base)
                                            {
                                                rel=calculate_relative(u.second,base_relative,3);
                                                if(rel=="error" || rel.length()>3)
                                                {
                                                   err=1;
                                                   error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                   listing<<"***************************ERROR***********************************"<<endl;
                                                   break;  
                                                }
                                                else{
                                                    b='1';
                                                }
                                            }
                                            else
                                            {
                                                err=1;
                                                error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                listing<<"***************************ERROR***********************************"<<endl;
                                                break;  
                                            }
                                            }
                                            else p='1';
                                            string val1="";
                                            val1+=x;
                                            val1+=b;
                                            val1+=p;
                                            val1+=e;
                                            val+=binary_to_hex_conv(val1);
                                            //increase_size(rel,3);
                                            val+=rel;
                                            listing<<temp<<"\t\t"<<val<<endl;
                                            objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                        else
                                        {
                                        string val1="";
                                        val1+=x;
                                        val1+=b;
                                        val1+=p;
                                        val1+=e;
                                        val+=binary_to_hex_conv(val1);
                                        val+=u.second;
                                        for(int i=0;i<u.first.size();i++) 
                                        {
                                              string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                                              //increase_size(loc,5);
                                              loc+="05";
                                              loc+=u.first[i];
                                              modification[curr_sect].push_back(loc);
                                         }
        
                                        listing<<temp<<"\t\t"<<val<<endl;
                                        objcode[curr_sect].push_back(make_pair(v[0],val));
                                      }
                                     }
                                }
                            }
                            else
                            {
                                    if(x=='1') t=clean_operand(v[3]);
                                    else t=v[3];
                                    bool k=0;
                                    if(e=='1') k=1;
                                    int num=3;
                                    if(e=='1') num+=2; 
                                    //cout<<"err"<<t<<" "<<k<<endl;
                                     pair<vector<string>,string> u=expressionparse(t,curr_sect,sect_count[curr_sect],num,k,extref);
                                     if(u.second=="error") 
                                    {
                                        err=1;
                                         error<<"INVALID SYMBOL"<<endl;
                                        listing<<"***************************ERROR***********************************"<<endl;
                                     }
                                    else{
                                        if(e=='0')
                                        {
                                            string rel=calculate_relative(u.second,PC,3);
                                            if(rel=="error" || rel.length()>3)
                                            {
                                            if(base)
                                            {
                                                rel=calculate_relative(u.second,base_relative,3);
                                                if(rel=="error" || rel.length()>3)
                                                {
                                                   err=1;
                                                   error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                   listing<<"***************************ERROR***********************************"<<endl;
                                                   break;  
                                                }
                                                else{
                                                    b='1';
                                                }
                                            }
                                            else
                                            {
                                                err=1;
                                                error<<"INVALID TOO HIGH RELATIVE"<<endl;
                                                listing<<"***************************ERROR***********************************"<<endl;
                                                break;  
                                            }
                                            }
                                            else p='1';
                                            string val1="";
                                            val1+=x;
                                            val1+=b;
                                            val1+=p;
                                            val1+=e;
                                            val+=binary_to_hex_conv(val1);
                                            val+=rel;
                                            listing<<temp<<"\t\t"<<val<<endl;
                                            objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                        else
                                        {
                                        string val1="";
                                        val1+=x;
                                        val1+=b;
                                        val1+=p;
                                        val1+=e;
                                        val+=binary_to_hex_conv(val1);
                                        val+=u.second;
                                        for(int i=0;i<u.first.size();i++) 
                                        {
                                              string loc=calculate_relative(add(v[0],1),header[curr_sect][0],6);
                                              //increase_size(loc,5);
                                              loc+="05";
                                              loc+=u.first[i];
                                              modification[curr_sect].push_back(loc);
                                         }
        
                                        listing<<temp<<"\t\t"<<val<<endl;
                                        objcode[curr_sect].push_back(make_pair(v[0],val));
                                        }
                                    }
                            }

                        }

                    }

                }
            }
            
        }
        /*if(!END)
        {
            error<<"END NOT DEFINED"<<endl;
            int clk=0;
            while(!END)
            {
              clk++; 
            }
        }*/
        if(!END) error<<"End statement not present"<<endl;
    }
    
}

