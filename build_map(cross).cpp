#include <iostream>
#include <fstream>
#define ROW 50
#define COL 50
#define R_X 4
#define R_Y 4
#define BATTERY 4000
using namespace std;

std::string out_file_name="floor.data";
std::ofstream out_file;

int main(void){
    ///file settings.
    out_file.open(out_file_name, std::ios::out);
    if(!out_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///build the map
    out_file<<ROW<<" "<<COL<<" "<<BATTERY<<"\n";
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(i == R_X && j == R_Y) out_file<<"R ";
            else if((i == R_X-1 || i == R_X+1 || j == R_Y-1 ||j == R_Y+1) && !(i==1 || i==ROW-1 || j==1 || j==COL-1 || i==R_X || j==R_Y))
                out_file<<"1 ";
            else out_file<<"0 ";
        }
        out_file<<"\n";
    }

    return 0;
}
