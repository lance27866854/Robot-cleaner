#include <iostream>
#include <fstream>
#define ROW 1000
#define COL 1000
#define R_X 999
#define R_Y 0
#define BATTERY 1000998
using namespace std;
enum {RIGHT=0, UP, LEFT, DOWN};
char Map[1000][1000];
std::string out_file_name="./106000103/floor.data";
std::ofstream out_file;

int main(void){
    ///file settings.
    out_file.open(out_file_name, std::ios::out);
    if(!out_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///build the map
    out_file<<ROW<<" "<<COL<<" "<<BATTERY<<"\n";
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            Map[i][j]='1';
        }
    }
    Map[R_X][R_Y]='R';
    int _size=ROW;
    int dir=RIGHT;
    int current_i=R_X, current_j=R_Y;
    bool f=0;
    int path=0;
    for(int i=ROW;i>0;i--){
        for(int j=i;j>0;j--){
            if(f==0) f=1;
            else{
                if(dir==RIGHT) current_j++;
                else if(dir==LEFT) current_j--;
                else if(dir==UP) current_i--;
                else current_i++;
                Map[current_i][current_j]='0';
            }
        }
        if(dir==RIGHT) dir = UP;
        else if(dir==LEFT) dir = DOWN;
        else if(dir==UP) dir = LEFT;
        else dir = RIGHT;
        path+=i;
    }
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            out_file<<Map[i][j]<<" ";
        }
        out_file<<"\n";
    }
    std::cout<<(path-1)*2;
    return 0;
}
