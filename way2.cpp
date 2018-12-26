#include <fstream>
#include <iostream>
#include <queue>
#include <list>
#include <vector>

#define MAX_MAPSIZE 1000
#define WALL '1'
#define FLOOR '0'
#define BASE 'R'
#define DU_NOTATION '@'
#define GO_NOTATION 'X'

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
            step = 0;
        }
        ~FCR(){}
        void find_path(int battery){
            ///Create ini_path.
            std::vector<Position> ini_path;
            ini_path.push_back(R);
            _paths.push(ini_path);

            for(int i=1;i<=battery;i++){//std::cout<<"{"<<battery<<"}\n";
                int s = _paths.size();
                while(s--){
                    std::vector<Position> old_path = _paths.front();
                    _paths.pop();
                    Position p = old_path.back();
                    //down
                    if(p.row < ROW-1 && _map[p.row+1][p.col] != WALL){
                        Position np(p.row+1, p.col);//examine if np in path.
                        int _count=0;
                        for(int j=0;j<i;j++) if(old_path[j] == np) _count++;
                        if(_count == 0 || _count == 1){
                            std::vector<Position> new_path = old_path;
                            new_path.push_back(np);
                            if(np == R) _qualified_paths.push_back(new_path);
                            else _paths.push(new_path);
                        }
                    }
                    //up
                    if(p.row > 0 && _map[p.row-1][p.col] != WALL){
                        Position np(p.row-1, p.col);
                        int _count=0;
                        for(int j=0;j<i;j++) if(old_path[j] == np) _count++;
                            if(_count == 0 || _count == 1){
                            std::vector<Position> new_path = old_path;
                            new_path.push_back(np);
                            if(np == R) _qualified_paths.push_back(new_path);
                            else _paths.push(new_path);
                        }
                    }
                    //left
                    if(p.col < COL-1 && _map[p.row][p.col+1] != WALL){
                        Position np(p.row, p.col+1);
                        int _count=0;
                        for(int j=0;j<i;j++) if(old_path[j] == np) _count++;
                        if(_count == 0 || _count == 1){
                            std::vector<Position> new_path = old_path;
                            new_path.push_back(np);
                            if(np == R) _qualified_paths.push_back(new_path);
                            else _paths.push(new_path);
                        }
                    }
                    //right
                    if(p.col > 0 && _map[p.row][p.col-1] != WALL){
                        Position np(p.row, p.col-1);
                        int _count=0;
                        for(int j=0;j<i;j++) if(old_path[j] == np) _count++;
                        if(_count == 0 || _count == 1){
                            std::vector<Position> new_path = old_path;
                            new_path.push_back(np);
                            if(np == R) _qualified_paths.push_back(new_path);
                            else _paths.push(new_path);
                        }
                    }
                }
            }
        }
        void solve(){
            ///Choose the path that through more 0s.
            while(1){
                int s = _qualified_paths.size();
                int _counting = 0;
                auto it = _qualified_paths.begin();

                ///Find the most efficient path in qualified path.
                for(auto i = it;i != _qualified_paths.end();i++){
                    std::vector<Position> test_path = *i;
                    std::vector<Position> duplicate;
                    int ts = test_path.size();
                    int new_counting = 0;
                    for(int j=1;j<ts;j++){
                        Position np = test_path[j];
                        if(_map[np.row][np.col] == FLOOR){
                            new_counting++;
                            _map[np.row][np.col] = DU_NOTATION;
                            duplicate.push_back(np);
                        }
                    }
                    int ds = duplicate.size();
                    for(int j=0;j<ds;j++){Position dp = duplicate[j];_map[dp.row][dp.col] = FLOOR;}
                    if(new_counting > _counting){_counting = new_counting;it = i;}
                }
                if(_counting == 0) break;

                ///Go through this path
                std::vector<Position> ideal_path = *it;
                s = ideal_path.size();
                for(int i=1;i<s;i++){
                    Position np = ideal_path[i];
                    _map[np.row][np.col] = GO_NOTATION;
                    step++;
                }
                _solutions.push(ideal_path);
                _qualified_paths.erase(it);
            }
        }
        void show_solution(){
            std::cout<<step<<"\n";
            int s = _solutions.size();
            while(s--){
                std::vector<Position> p = _solutions.front();
                _solutions.pop();
                int ps = p.size();
                for(int i=1;i<ps;i++){
                    std::cout<<p[i].row<<" "<<p[i].col<<"\n";
                }
            }
        }

    private:
        std::queue<std::vector<Position>> _paths;
        std::list<std::vector<Position>> _qualified_paths;
        std::queue<std::vector<Position>> _solutions;
        Position R;
        int step;
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
            if(_map[i][j] == BASE){
                p.row =i;
                p.col =j;
            }
        }
    }

    ///FCR declaration.
    FCR fcr(p);
    fcr.find_path(battery);
    fcr.solve();
    fcr.show_solution();

    return 0;
}
