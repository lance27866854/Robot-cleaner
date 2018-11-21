#include <iostream>
#include <fstream>
using namespace std;

std::string in_file_name="./106000103/final.path";
std::ifstream in_file;

int main(void){
    ///file settings.
    in_file.open(in_file_name,std::ios::in);
    if(!in_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///start examine
    int _step;
    int _count=0;
    int i, j;

    in_file>>_step;
    while(in_file>>i>>j){_count++;}

    if(_step != _count){
        cout<<"------------------------\n";
        cout<<"Wrong! step problem.\n";
        cout<<"------------------------\n";
        return 1;
    }
    cout<<"Complete.";
    return 0;
}
