#include <fstream>
#include <iostream>
#include <queue>
#include <map>
#include <vector>

#define MAX_MAPSIZE 1000

std::string in_file_name="floor.data";
std::string out_file_name="final.path";
char _map[MAX_MAPSIZE][MAX_MAPSIZE];
int ROW, COL;

class Position{
    public:
        friend bool operator == (Position const & a, Position const & b){return (a.row == b.row)&&(a.col == b.col);}
        Position(){row = 0; col = 0;}
        Position(int const & r, int const & c){row = r; col = c;}
        ~Position(){}
        int row;//0~n-1
        int col;//0~n-1
};

class FCR{
    public:
        FCR(const Position & p){
            R = p;
        }
        ~FCR(){}
        void find_path(int battery){
            ///Create ini_path.
            std::vector<Position> ini_path;
            ini_path.push_back(R);
            _paths.push(ini_path);

            while(battery--){//std::cout<<"{"<<battery<<"}\n";
                int s = _paths.size();
                while(s--){
                    std::vector<Position> old_path = _paths.front();
                    _paths.pop();
                    Position p = old_path.back();
                    //down
                    if(p.row < ROW-1 && _map[p.row+1][p.col] != '1'){
                        Position np(p.row+1, p.col);
                        std::vector<Position> new_path = old_path;
                        new_path.push_back(np);
                        if(np == R) _qualified_paths.push(new_path);
                        else _paths.push(new_path);
                    }
                    //up
                    if(p.row > 0 && _map[p.row-1][p.col] != '1'){
                        Position np(p.row-1, p.col);
                        std::vector<Position> new_path = old_path;
                        new_path.push_back(np);
                        if(np == R) _qualified_paths.push(new_path);
                        else _paths.push(new_path);
                    }
                    //left
                    if(p.col < COL-1 && _map[p.row][p.col+1] != '1'){
                        Position np(p.row, p.col+1);
                        std::vector<Position> new_path = old_path;
                        new_path.push_back(np);
                        if(np == R) _qualified_paths.push(new_path);
                        else _paths.push(new_path);
                    }
                    //right
                    if(p.col > 0 && _map[p.row][p.col-1] != '1'){
                        Position np(p.row, p.col-1);
                        std::vector<Position> new_path = old_path;
                        new_path.push_back(np);
                        if(np == R) _qualified_paths.push(new_path);
                        else _paths.push(new_path);
                    }
                }
            }
        }
        void solve(){
            for(int i=0;i<ROW;i++){
                for(int j=0;j<COL;j++){
                    if(_map[i][j]!='0') continue;
                    //qualified path.

                }
            }
        }
        void show_solution(){
            int qs = _qualified_paths.size();
            std::cout<<qs<<"\n";
            while(qs--){
                std::vector<Position> p = _qualified_paths.front();
                _qualified_paths.pop();

                int vs = p.size();
                for(int i=0;i<vs;i++){
                    std::cout<<p[i].row<<" "<<p[i].col<<"\n";
                }
                std::cout<<"-----------------------------\n";
            }
        }

    private:
        std::queue<std::vector<Position>> _paths;
        std::queue<std::vector<Position>> _qualified_paths;
        std::queue<std::vector<Position>> _solutions;
        Position R;
};


int main(void){
    ///file settings.
    std::ifstream in_file;
    std::ofstream out_file;
    in_file.open(in_file_name,std::ios::in);
    out_file.open(out_file_name,std::ios::out);
    if(!in_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///Get input
    int battery;
    Position p;
    in_file>>ROW>>COL>>battery;
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            in_file>>_map[i][j];
            if(_map[i][j] == 'R'){
                p.row =i;
                p.col =j;
            }
        }
    }

    ///FCR declaration.
    FCR fcr(p);
    fcr.find_path(battery);
    //fcr.slove();
    fcr.show_solution();

    return 0;
}
