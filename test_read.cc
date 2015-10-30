#include <iostream>
#include <fstream>

using namespace std;

int main(){
  ifstream fs;
  char *buf = new char(1024);
  string st = "/random";

  fs.open(st.c_str());

  while(fs){
    fs.read(buf,1024);
    cout<<"\nRead: "<<buf<<endl;
  }
  fs.close();
  return 0;
}
