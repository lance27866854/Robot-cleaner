#include <iostream>
#include <fstream>
#define MAX_MAPSIZE 1000
using namespace std;

std::string in_file_name1="floor.data";
std::string in_file_name2="final.path";
std::ifstream in_file1, in_file2;

char Map[MAX_MAPSIZE][MAX_MAPSIZE];
int ROW, COL;

int main(void){
    ///file settings.
    in_file1.open(in_file_name1,std::ios::in);
    in_file2.open(in_file_name2,std::ios::in);
    if(!in_file1||!in_file2){std::cout<<"Something wrong with file.\n";return 1;}

    int battery;
    in_file1>>ROW>>COL>>battery;
    int ri, rj;
    ///get input
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            in_file1>>Map[i][j];
            if(Map[i][j] == 'R'){
                ri = i;
                rj = j;
            }
        }
    }
    int step;
    int b = battery;
    int current_i = ri, current_j = rj;
    in_file2>>step;
    for(int i=0;i<step;i++){
        cout<<"now is at step: "<<i<<"\n";
        if(b == 0){
            cout<<"------------------------\n";
            cout<<"Wrong! Battery = 0.\n";
            cout<<"------------------------\n";
            return 0;
        }
        int go_i, go_j;
        in_file2>>go_i>>go_j;
        if(go_i<0 || go_j<0 || go_i>=ROW || go_j>=COL || Map[go_i][go_j] == '1'||
           (current_i == go_i && current_j == go_j)){
            cout<<"------------------------\n";
            cout<<"Wrong! The step is illegal.\n";
            cout<<"------------------------\n";
            return 0;
        }
        else{
            current_i = go_i;
            current_j = go_j;
            Map[current_i][current_j] = '#';
            b--;
            if(current_i == ri && current_j == rj) b = battery;
        }
    }

    if(current_i != ri || current_j != rj){
        cout<<"------------------------\n";
        cout<<"Wrong! The destination is not R.\n";
        cout<<"------------------------\n";
        return 0;
    }
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(Map[i][j] == '0'){
                cout<<"------------------------\n";
                cout<<"Wrong! Some place is not clean.\n";
                cout<<"------------------------\n";
                return 0;
            }
        }
    }
    cout<<"Complete.";
    return 0;
}
