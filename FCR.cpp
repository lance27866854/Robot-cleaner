#include <fstream>
#include <vector>
#include <queue>

#define MAX_MAPSIZE 1000
#define WALL '1'
#define ROAD '0'
#define BASE 'R'

std::ifstream in_file;
std::ofstream out_file;

char Map[MAX_MAPSIZE][MAX_MAPSIZE];
bool Visited[MAX_MAPSIZE][MAX_MAPSIZE];
int StepTable[MAX_MAPSIZE*MAX_MAPSIZE];
int Row, Col;

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
            type = 0;
        }
        ~Node(){}
        int type;
        std::vector<Position> u, v;
        Node* next;
};

class FCR{
    public:
        FCR(const Position & p){
            R = p;
            step = 0;
            no_port=false;
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
            if(port_num == 0) no_port=true;
            
            for(int i=0;i<port_num;i++){
                Port[i] = nodes_vec[i].second;
            }

            ///5
            for(int i=0;i<16;i++){
                Node* node = new Node(i);
                PortTable.push_back(node);
            }
        }
        void solve(){
            if(no_port) return;
            ///In this function, we will build the ideal paths
            //1.use solve_index refers to the current destination of the ideal path.
            //2.every iteration we first check if this node has been gone through, if so, continue.
            int solve_index = nodes_vec_size-1;
            int times = 0;//how many nodes are there.
            while(solve_index >= 0){
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

            ///Join different paths.
            //First, we look for any nonzero port.
            int first_start_port;
            bool flag = 0;
            for(int i=0;i<port_num;i++){
                if(PortTable[i*5]->type > 0){
                    first_start_port = i*5;
                    flag = 1;
                   break;
                }
            }
            if(!flag){
                for(int i=0;i<port_num;i++){
                    for(int j=0;j<port_num;j++){
                        if(PortTable[i*4+j]->type > 0){
                            first_start_port = i*4+j;
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 1) break;
                }
            }

            //Second, we start a while loop with important variable "last_port".
            //In every iteration, we choose a path from port table and output it.
            //At the end of the iteration, we need find the next last_port with the function "get_next_port".
            //Stop the while-loop if we traverse every paths we stored in PortTable. (use travel_time for counting.)
            int last_port = first_start_port;
            int start_port = last_port / 4;
            int travel_times = 0;
            while(1){
                //go.
                Node *go = PortTable[last_port]->next;
                PortTable[last_port]->type--;
                PortTable[last_port]->next = go->next;
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
            if(no_port) return;
            for(auto it = answer_position.begin();it!=answer_position.end();it++){
                out_file<<it->first<<" "<<it->second<<(it==answer_position.end()-1?"":"\n");
            }
        }
     private:
        std::vector<Node *> PortTable;
        std::vector<Position> answer_position;

        VistPosition *nodes_vec;
        int *nodes_step_vec;

        Position Port[4];
        Position R;

        int nodes_vec_size;
        int nodes_step_vec_size;
        int step;
        int port_num;
        bool no_port;

        ///Tool-Functions.
        int get_next_port(const int& last_port, int& start_port){
            //First, choose the path no need to change.(self loop first).
            int new_out = start_port;
            if(PortTable[new_out*5]->type>0) return new_out*5;//self loop.
            for(int i=0;i<port_num;i++){
                if(PortTable[new_out + i*4]->type>0){
                    start_port = i;
                    return new_out + i*4;
                }
            }

            //case the u, v need to be swapped.
            int idx = -1;
            for(int i=0;i<port_num;i++){
                if(PortTable[i + new_out*4]->type>0){
                    idx = i + new_out*4;
                    break;
                }
            }
            ///After the two operation above,
            ///if we did get the right value, we are going to create a new Node and add it to PortTable.
            if(idx != -1){
                Node *node = PortTable[idx]->next;
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
                //self loop.
                if(PortTable[op*5]->type>0){
                    start_port = op;
                    return op*5;
                }
                for(int i=0;i<port_num;i++){
                    if(PortTable[op + i*4]->type>0){
                        start_port = i;
                        return op + i*4;
                    }
                }
                //reverse.
                for(int i=0;i<port_num;i++){
                    if(PortTable[i + op*4]->type>0){
                        idx = i + op*4;
                        break;
                    }
                }
                Node *node = PortTable[idx]->next;
                node->u.swap(node->v);
                start_port = idx - op*4;
                return idx;
            }
        }
        int revise_path(const int& out){
            ///choose the max port.
            int port_destination[4]={0};
            for(int i=0;i<16;i++){
                int s = i/4;
                port_destination[s] += PortTable[i]->type;
                port_destination[i-s*4] += PortTable[i]->type;
            }
            int max_port=0;
            for(int i=0;i<port_num;i++){
                if(Port[i].first-Port[out].first!=2&&Port[i].first-Port[out].first!=-2&&
                Port[i].second-Port[out].second!=2&&Port[i].second-Port[out].second!=-2){
                    if(port_destination[i]>0){
                        max_port = i;
                        break;
                    }
                }
            }

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
            return max_port;
        }
        void find_routes(const Position& from, const Position& to, bool flag){
            ///Initialization.
            //Map set.
            for(int i=0;i<Row;i++){
                for(int j=0;j<Col;j++){
                    Visited[i][j] = 0;
                    StepTable[i*Row+j] = 0;
                }
            }

            //containers
            nodes_vec = new VistPosition[Row*Col];
            nodes_step_vec = new int[Row*Col];
            nodes_vec_size = 0;
            nodes_step_vec_size = 0;
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
                    if(Map[p.first][p.second] == ROAD && Visited[p.first][p.second] == 0){
                        //visit it.
                        Visited[p.first][p.second] = 1;
                        StepTable[cnt]++;
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
            Node * insert_node = PortTable[n];
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
};

int main(int argc, char *argv[]){
    ///file settings.
    char buffer[30];
    sprintf(buffer, "./%s/floor.data", argv[1]);
    in_file.open(buffer,std::ios::in);
    sprintf(buffer, "./%s/final.path", argv[1]);
    out_file.open(buffer,std::ios::out);

    //if(!in_file || !out_file){std::cout<<"Something wrong with file.\n";return 1;}

    ///Get input and variables.
    int battery;
    Position p;
    in_file>>Row>>Col>>battery;

    for(int i=0;i<Row;i++){
        for(int j=0;j<Col;j++){
            in_file>>Map[i][j];
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
    fcr.out_to_file();
    return 0;
}
