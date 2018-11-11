#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#define MAX_MAPSIZE 1000
#define WALL '1'
#define ROAD '0'
#define BASE 'R'

std::string in_file_name="floor.data";
std::string out_file_name="final.path";

char _map[MAX_MAPSIZE][MAX_MAPSIZE];
bool _visited[MAX_MAPSIZE][MAX_MAPSIZE];
int ROW, COL;
enum Direction{UP, DOWN, RIGHT, LEFT};
typedef std::pair<int, int> Position;
typedef std::pair<int, Position> InPosition;

class Edge{
    public:
        Edge(){}
        Edge(const Position& U, const Position& V, const int& W){
            u = U;
            v = V;
            w = W;
        }
        ~Edge(){}
        friend std::ostream & operator<<(std::ostream & os, const Edge & e){
            os<<"<"<<e.u.first<<", "<<e.u.second<<"> ";
            os<<"<"<<e.v.first<<", "<<e.v.second<<"> ";
            os<<"["<<e.w<<"]\n";
            return os;
        }

    private:
        Position u, v;
        int w;
};

class FCR{
    public:
        FCR(const Position & p){
            R = p;
            step = 0;
        }
        ~FCR(){}
        void BuildGraph(){
            std::vector<Edge> edges;
            std::queue<InPosition> nodes;

            ///Initialization.
            //first = row, second = col
            InPosition m1{UP, {R.first-1, R.second}};
            InPosition m2{DOWN, {R.first+1, R.second}};
            InPosition m3{RIGHT, {R.first, R.second+1}};
            InPosition m4{LEFT, {R.first, R.second-1}};
            nodes.push(m1);
            nodes.push(m2);
            nodes.push(m3);
            nodes.push(m4);

            ///BFS
            while(!nodes.empty()){
                InPosition ip = nodes.front();
                int dir = ip.first;
                Position p = ip.second;
                nodes.pop();
                //extend.
                if(p.first<ROW&&p.first>=0&&p.second<COL&&p.second>=0){
                    if(!_visited[p.first][p.second]&&_map[p.first][p.second] == ROAD){
                        //four case.
                        Position d1{p.first-1, p.second};//UP
                        Position d2{p.first+1, p.second};//DOWN
                        Position d3{p.first, p.second+1};//RIGHT
                        Position d4{p.first, p.second-1};//LEFT

                        if(dir == UP){//std::cout<<"UP\n";
                            Edge e(d2,p,1);
                            edges.push_back(e);
                            nodes.push({UP,d1});nodes.push({RIGHT,d3});nodes.push({LEFT,d4});
                        }
                        else if(dir == DOWN){
                            Edge e(d1,p,1);
                            edges.push_back(e);
                            nodes.push({DOWN,d2});nodes.push({RIGHT,d3});nodes.push({LEFT,d4});
                        }
                        else if(dir == RIGHT){
                            Edge e(d4,p,1);
                            edges.push_back(e);
                            nodes.push({UP,d1});nodes.push({DOWN,d2});nodes.push({RIGHT,d3});
                        }
                        else{
                            Edge e(d3,p,1);
                            edges.push_back(e);
                            nodes.push({UP,d1});nodes.push({DOWN,d2});nodes.push({LEFT,d4});
                        }
                    }
                }
                _visited[p.first][p.second] = 1;
            }
            int c = 0;
            for(auto it = edges.begin();it != edges.end();it++){
                std::cout<<*it;
                c++;
            }
            std::cout<<c<<"\n";
        }
        void solve(){
        }
        void show_solution(){

        }

    private:
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
                p.first =i;
                p.second =j;
            }
        }
    }

    ///FCR declaration.
    FCR fcr(p);
    fcr.BuildGraph();
    //fcr.solve();
    //fcr.show_solution();

    return 0;
}
