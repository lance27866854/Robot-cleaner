#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#define MAX_MAPSIZE 1000
#define WALL '1'
#define ROAD '0'
#define BASE 'R'

std::string in_file_name="./106000103/floor.data";
std::string out_file_name="./106000103/final.path";
std::ifstream in_file;
std::ofstream out_file;

typedef std::pair<int, int> Position;
typedef std::pair<int, Position> StepPosition;
typedef std::pair<bool, Position> VistPosition;

class Node{
    public:
        Node(){
            next = nullptr;
        }
        Node(int i){
            next = nullptr;
            type = 0;//for count
        }
        ~Node(){}
        int type;
        std::vector<Position> u;
        std::vector<Position> v;
        Node* next;
};

class FCR{
    public:
        FCR(const Position & p, char* _map, const int& row, const int& col){
            R = p;
            step = 0;
            Map = _map;
            Row = row;
            Col = col;
        }
        ~FCR(){}
        void BuildGraph(){
            ///This function is for initialization.
            //1.build nodes_vec
            //2.build nodes_step_vec
            //3.build StepTable
            //4.build port
            //5.build port table

            ///1, 2, 3
            find_routes(R, {0, 0}, 1);

            ///4
            port_num = StepTable[1];
            for(int i=0;i<port_num;i++){
                Port[i] = nodes_vec[i].second;
            }

            ///5
            for(int i=0;i<16;i++){
                Node* node = new Node(i);
                port_table.push_back(node);
            }//0~15
        }
        void solve(){
            ///In this function, we will build the ideal paths
            //1.use solve_index refers to the current destination of the ideal path.
            //2.every iteration we first check if this node has been gone through, if so, continue.
            int solve_index = nodes_vec_size-1;
            int times = 0;//how many nodes are there.
            while(solve_index >= 0){//std::cout<<solve_index<<"\n";
                if(nodes_vec[solve_index].first == true){solve_index--;continue;}
                //select two scenario of traversal, the more unvisited nodes gets , better the path is.
                Node *node = new Node;
                get_route(solve_index, node->u);
                get_route(solve_index, node->v);
                build_port_table(node);
                step += nodes_step_vec[solve_index]*2;
                times++;
                solve_index--;
            }
            delete []nodes_vec;
            delete []nodes_step_vec;
            delete []StepTable;

            ///Join different paths.
            //initial condition.
            //First, we look for any nonzero port.
            int first_start_port;
            bool flag = 0;
            for(int i=0;i<port_num;i++){
                for(int j=0;j<port_num;j++){
                    if(port_table[i*4+j]->type > 0){
                        first_start_port = i*4+j;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1) break;
            }

            //Second, we start a while loop with important variable "last_port".
            //In every iteration, we choose a path from port table and output it.
            //At the end of the iteration, we need find the next last_port with the function "get_next_port".
            //Stop the while-loop if we traverse every paths we stored in port_table. (use travel_time for counting.)
            int last_port = first_start_port;
            int start_port = last_port / 4;
            int travel_times = 0;
            while(1){
                //go.
                Node *go = port_table[last_port]->next;
                port_table[last_port]->type--;
                port_table[last_port]->next = go->next;
                get_solution_position(go->v, 1, 0);//vector, reverse, base
                get_solution_position(go->u, 0, 1);

                //stop condition.
                travel_times++;
                if(travel_times == times)break;

                //choose next port.
                last_port = get_next_port(last_port, start_port);
            }
        }

        void out_to_file(){
            out_file<<step<<"\n";
            for(auto it = answer_position.begin();it!=answer_position.end();it++)
                out_file<<it->first<<" "<<it->second<<"\n";
        }

        ///Tool-Functions.
        int get_next_port(const int& last_port, int& start_port){
            //in = last_port / 4 = new_out
            //First, choose the path no need to change.(self loop first).
            int new_out = start_port;
            if(port_table[new_out*5]->type>0) return new_out*5;//self loop.
            for(int i=0;i<port_num;i++){
                if(port_table[new_out + i*4]->type>0){
                    start_port = i;
                    return new_out + i*4;
                }
            }

            //case the u, v need to be swapped.
            int idx = -1;
            for(int i=0;i<port_num;i++){
                if(port_table[i + new_out*4]->type>0){
                    idx = i + new_out*4;
                    break;
                }
            }
            ///After the two operation above,
            ///if we did get the right value, we are going to create a new Node and add it to Port_table.
            if(idx != -1){
                Node *node = port_table[idx]->next;
                node->u.swap(node->v);
                start_port = idx - new_out*4;
                return idx;
            }
            ///if not, we should find a alternative way.
            //That is, we are going to use BFS to search the Map again,
            //hoping to get a new port with some additional paths.
            else{
                //call revise_path to get new port.
                int op = revise_path(new_out);
                //repeat the examination steps above.
                if(port_table[op*5]->type>0){
                    start_port = op;
                    return op*5;//self loop.
                }
                for(int i=0;i<port_num;i++){
                    if(port_table[op + i*4]->type>0){
                        start_port = i;
                        return op + i*4;
                    }
                }
                //reverse.
                for(int i=0;i<port_num;i++){
                    if(port_table[i + op*4]->type>0){
                        idx = i + op*4;
                        break;
                    }
                }
                Node *node = port_table[idx]->next;
                node->u.swap(node->v);
                start_port = idx - op*4;
                return idx;
            }
        }
        int revise_path(const int& out){
            ///choose the max port.
            int port_destination[4]={0};
            for(int i=0;i<4;i++){
                for(int j=0;j<4;j++){
                    port_destination[j] += port_table[4*j+i]->type;
                    port_destination[i] += port_table[4*i+j]->type;
                }
            }
            int max_port=0;
            for(int i=0;i<4;i++) if(port_destination[i]>max_port) max_port = i;

            Position from = Port[out];
            Position to = Port[max_port];
            //new nodes_vec, nodes_step_vec, StepTable.
            find_routes(from, to, 0);

            std::vector<Position> route;
            int choice_index = nodes_vec_size-1;
            int cnt = nodes_step_vec[choice_index]-1;
            step += (cnt+3);
            for(int i=cnt;i>0;i--){
                int next_choice_index;
                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    //choose any can_reach adjacent node.
                    if(can_reach(choice_index, j)){
                        next_choice_index = j;
                        break;
                    }
                }
                route.push_back(nodes_vec[choice_index].second);
                choice_index = next_choice_index;
            }
            route.push_back(nodes_vec[choice_index].second);
            route.push_back(from);

            get_solution_position(route, 1, 1);
            delete []nodes_vec;
            delete []nodes_step_vec;
            delete []StepTable;
            return max_port;
        }
        void find_routes(const Position& from, const Position& to, bool flag){
            ///Initialization.
            //containers
            nodes_vec = new VistPosition[Row*Col];
            nodes_step_vec = new int[Row*Col];
            nodes_vec_size = 0;
            nodes_step_vec_size = 0;
            //map class
            Visited = new bool[Row*Col];
            StepTable = new int[Row*Col];

            for(int i=0;i<Row;i++){
                for(int j=0;j<Col;j++){
                    Visited[i*Row + j] = 0;
                    StepTable[i*Row + j] = 0;
                }
            }
            int max_cnt = 0;

            ///In this function, we will do things below:
            //1.build nodes_vec.
            //2.build nodes_step_vec.
            //3.build StepTable.
            //PS. the flag means that if this is initialization mode.

            std::queue<StepPosition> nodes;
            StepPosition m1{1, {from.first-1, from.second}};//UP
            StepPosition m2{1, {from.first+1, from.second}};//DOWN
            StepPosition m3{1, {from.first, from.second+1}};//RIGHT
            StepPosition m4{1, {from.first, from.second-1}};//LEFT
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

                if(p.first<Row && p.first>=0 && p.second<Col && p.second>=0){
                    if(Map[p.first*Row+p.second] == ROAD && Visited[p.first*Row+p.second] == 0){
                        //visit it.
                        Visited[p.first*Row+p.second] = 1;
                        StepTable[cnt] = StepTable[cnt]+1;
                        if(cnt > max_cnt) max_cnt = cnt;//max step.

                        //for nodes_vec.
                        nodes_vec[nodes_vec_size++] = {false, p};
                        nodes_step_vec[nodes_step_vec_size++] = cnt;
                        if(flag == 0 && p == to){break;}

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
            }
            //delete
            delete []Visited;
        }

        void get_solution_position(const std::vector<Position>& vec, bool _reverse, bool _base){
            if(_reverse){
                for(auto it = vec.rbegin();it != vec.rend();it++){
                    answer_position.push_back(*it);
                }
            }
            else{
                for(auto it = vec.begin()+1;it != vec.end();it++){
                    answer_position.push_back(*it);
                }
            }
            if(_base){
                answer_position.push_back(R);
            }
        }

        void build_port_table(Node* node){
            ///convert the input position and output position to port number.
            //from v to u.
            Position from = node->v.back();
            Position to = node->u.back();
            int in_port, out_port;
            for(int i=0;i<port_num;i++){
                if(from == Port[i]) out_port = i;
                if(to == Port[i]) in_port = i;
            }
            node->type = in_port*4 + out_port;//0~15

            ///set the node to the port table.
            int n = node->type;
            Node * insert_node = port_table[n];
            insert_node->type = insert_node->type + 1;
            while(insert_node->next != nullptr){
                insert_node = insert_node->next;
            }
            insert_node->next = node;
        }

        void get_route(const int& solve_index, std::vector<Position>& route){
            ///Two input in this function, choice_index and route.
            ///The former is the destination of this traversal.
            ///The latter is the container of this ideal route.
            ///After operation, we will get a path stored in std::vector<Position> route.
            //1.We can get the value of nodes[i](specific steps),
            //2.from the index of StepTable[i-1](include) to StepTable[i](exclude).

            //use choice_index to represent the index of the chosen node.
            int choice_index = solve_index;
            //cnt is the steps need to be gone through to reach this point.
            int cnt = nodes_step_vec[choice_index]-1;
            for(int i=cnt;i>0;i--){
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
                route.push_back(nodes_vec[choice_index].second);
                choice_index = next_choice_index;
            }
            route.push_back(nodes_vec[choice_index].second);
        }

        bool can_reach(int i, int j){
            ///This function help to determine if the elements of nodes_vec of index i and index j is adjacent to each other.
            Position u = nodes_vec[i].second;
            Position v = nodes_vec[j].second;
            if((u.first-v.first == 1 || v.first-u.first == 1) && v.second == u.second) return true;
            else if((u.second-v.second == 1 || v.second-u.second == 1) && v.first == u.first) return true;
            else return false;
        }

     private:
        std::vector<Node *> port_table;
        std::vector<Position> answer_position;

        VistPosition *nodes_vec;
        int *nodes_step_vec;
        char *Map;

        bool *Visited;
        int *StepTable;

        Position Port[4];
        Position R;

        int Row, Col;
        int nodes_vec_size;
        int nodes_step_vec_size;
        int step;
        int port_num;//how many port are there.
};

int main(void){
    ///start clock.
    LARGE_INTEGER t1, t2, ts;
    QueryPerformanceFrequency(&ts);
    QueryPerformanceCounter(&t1);

    ///file settings.
    in_file.open(in_file_name,std::ios::in);
    out_file.open(out_file_name,std::ios::out);
    if(!in_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///Get input and variables.
    int battery;
    int ROW, COL;
    Position p;
    in_file>>ROW>>COL>>battery;
    char *Map = new char[ROW*COL];//[MAX_MAPSIZE][MAX_MAPSIZE];

    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            in_file>>Map[i*ROW+j];
            if(Map[i*ROW+j] == BASE){
                p.first =i;
                p.second =j;
            }
        }
    }

    ///FCR declaration.
    std::cout<<"%";
    FCR fcr(p, Map, ROW, COL);
    std::cout<<"%";
    fcr.BuildGraph();
    std::cout<<"%";
    fcr.solve();
    std::cout<<"%";
    fcr.out_to_file();
    std::cout<<"%";

    delete []Map;
    QueryPerformanceCounter(&t2);
    std::cout<<"Execution Time : "<<(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart)<<" s."<<"\n";
    return 0;
}
