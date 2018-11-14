#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#define MAX_MAPSIZE 1000
#define WALL '1'
#define ROAD '0'
#define BASE 'R'

std::string in_file_name="floor.data";
std::string out_file_name="final.path";
char Map[MAX_MAPSIZE][MAX_MAPSIZE];
bool Visited[MAX_MAPSIZE][MAX_MAPSIZE];
int StepTable[MAX_MAPSIZE*2];
int ROW, COL;

enum Direction{UP=0, DOWN, RIGHT, LEFT};
typedef std::pair<int, int> Position;
typedef std::pair<int, Position> StepPosition;
typedef std::pair<bool, Position> VistPosition;

class FCR{
    public:
        FCR(const Position & p){
            R = p;
            step = 0;
            max_cnt = 0;
        }
        ~FCR(){}
        void BuildGraph(){
            std::queue<StepPosition> nodes;
            ///Initialization.
            //first = row, second = col
            StepPosition m1{1, {R.first-1, R.second}};//UP
            StepPosition m2{1, {R.first+1, R.second}};//DOWN
            StepPosition m3{1, {R.first, R.second+1}};//RIGHT
            StepPosition m4{1, {R.first, R.second-1}};//LEFT
            nodes.push(m1);
            nodes.push(m2);
            nodes.push(m3);
            nodes.push(m4);

            while(!nodes.empty()){
                //nodes
                StepPosition sp = nodes.front();
                Position p = sp.second;
                int cnt = sp.first;
                nodes.pop();

                if(p.first<ROW && p.first>=0 && p.second<COL && p.second>=0){
                    if(Map[p.first][p.second] == ROAD && Visited[p.first][p.second] == 0){
                        //visit it.
                        Visited[p.first][p.second] = 1;
                        StepTable[cnt]++;
                        if(cnt > max_cnt) max_cnt = cnt;

                        //for nodes_vec.
                        nodes_vec.push_back({false, p});
                        nodes_step_vec.push_back(cnt);

                        //four case.
                        StepPosition d1{cnt+1, {p.first-1, p.second}};//UP
                        StepPosition d2{cnt+1, {p.first+1, p.second}};//DOWN
                        StepPosition d3{cnt+1, {p.first, p.second+1}};//RIGHT
                        StepPosition d4{cnt+1, {p.first, p.second-1}};//LEFT
                        nodes.push(d1);
                        nodes.push(d2);
                        nodes.push(d3);
                        nodes.push(d4);
                    }
                }
            }
            StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
                //std::cout<<StepTable[i]<<" ";
            }
        }
        void Find_route(Position u, Position v){
            ///find the path u to v.

        }
        void Move(){

        }
        void solve(){
            /*for(auto it = nodes_vec.rbegin();it!= nodes_vec.rend();it++){
                std::cout<<it->second.first<<" "<<it->second.second<<"\n";
            }*/
            int solve_index = nodes_vec.size()-1;
            while(solve_index >= 0){//for every unvisited node.
                //to see if we have been to this node.
                if(nodes_vec[solve_index].first == true){solve_index--;continue;}
                //pick nodes[solve_index] as the destination.
                //select two scenario of traversal, the more unvisited nodes gets , better the path is.
                //We can get the value of nodes[i](specific steps),
                //from the index of StepTable[i-1](include) to StepTable[i](exclude).
                //we start from the last element, we go back and find two different path.
                std::vector<Position> route;
                //if the node's step is 9, we start from 8.
                int choice_index = solve_index;
                int cnt = nodes_step_vec[choice_index]-1;
                std::cout<<nodes_vec[choice_index].second.first<<" "<<nodes_vec[choice_index].second.second<<"\n";

                for(int i=cnt;i>0;i--){//8~1
                    Map[nodes_vec[choice_index].second.first][nodes_vec[choice_index].second.second] = '&';
                    int next_choice_index;
                    for(int j=StepTable[i-1];j<StepTable[i];j++){
                        //choose the best(not visited) adjacent node.
                        if(can_reach(choice_index, j)){
                            next_choice_index = j;
                            if(!nodes_vec[j].first){
                                nodes_vec[j].first = true;
                                break;
                            }
                        }
                    }
                    choice_index = next_choice_index;
                    route.push_back(nodes_vec[choice_index].second);
                }
                solve_index--;
                for(auto it = route.begin();it!= route.end();it++){std::cout<<it->first<<" "<<it->second<<"\n";}
                std::cout<<"----------------------------------------\n";
                //break;
            }
            for(int i=0;i<ROW;i++){
                for(int j=0;j<COL;j++){
                    std::cout<<Map[i][j]<<" ";
                }
                std::cout<<"\n";
            }
        }
        bool can_reach(int i, int j){
            ///This function help to determine
            ///if the elements of nodes_vec of index i and index j is adjacent to each other.
            Position u = nodes_vec[i].second;
            Position v = nodes_vec[j].second;
            if((u.first-v.first == 1 || v.first-u.first == 1) && v.second == u.second) return true;
            else if((u.second-v.second == 1 || v.second-u.second == 1) && v.first == u.first) return true;
            else return false;
        }
        void show_solution(){
        }
     private:
        std::vector<VistPosition> nodes_vec;
        std::vector<int> nodes_step_vec;
        Position R;
        int step;
        int max_cnt;
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
            in_file>>Map[i][j];
            Visited[i][j] = 0;
            if(Map[i][j] == BASE){
                p.first =i;
                p.second =j;
            }
        }
    }
     ///FCR declaration.
    FCR fcr(p);
    fcr.BuildGraph();
    fcr.solve();
    //fcr.show_solution();
     return 0;
}