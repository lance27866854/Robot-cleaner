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

char Map[MAX_MAPSIZE][MAX_MAPSIZE];
bool Visited[MAX_MAPSIZE][MAX_MAPSIZE];
int StepTable[MAX_MAPSIZE*MAX_MAPSIZE];
int ROW, COL;

enum Direction{UP=0, DOWN, RIGHT, LEFT};
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
            u.resize(0);
            v.resize(0);
        }
        ~Node(){
            u.~vector();
            v.~vector();
        }
        int type;
        std::vector<Position> u;
        std::vector<Position> v;
        Node* next;
};

class FCR{
    public:
        FCR(const Position & p){
            R = p;
            step = 0;
            max_cnt = 0;
            port_table_size = 0;
        }
        ~FCR(){}
        void BuildGraph(){
            ///This function is for initialization.
            //1.build nodes_vec
            //2.build nodes_step_vec
            //3.build StepTable
            //4.build port

            ///1, 2, 3
            std::queue<StepPosition> nodes;
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
            }

            ///4
            port_num = StepTable[1];
            for(int i=0;i<port_num;i++){
                Port[i] = nodes_vec[i].second;
            }

            ///5
            if(port_num == 1) port_table_size = 1;
            else if(port_num == 2) port_table_size = 3;
            else if(port_num == 3) port_table_size = 6;
            else if(port_num == 4) port_table_size = 10;

            for(int i=0;i<port_table_size;i++){
                Node* node = new Node(i);
                port_table.push_back(node);
            }
        }
        void solve(){
            ///In this function, we will build the ideal paths
            //1.use solve_index refers to the current destination of the ideal path.
            //2.every iteration we first check if this node has been gone through, if so, continue.
            int solve_index = nodes_vec.size()-1;
            int times = 0;//how many nodes are there.
            while(solve_index >= 0){
                if(nodes_vec[solve_index].first == true){solve_index--;continue;}
                //select two scenario of traversal, the more unvisited nodes gets , better the path is.
                Node *node = new Node;
                get_route(solve_index, node->u);
                get_route(solve_index, node->v);
                get_port_number(node);
                build_port_table(node);
                times++;
                solve_index--;
                std::cout<<node->type<<"\n";
            }

            ///Join different paths.
            int first_start_port=0;
            int max_count=port_table[0]->type;
            for(int i=1;i<port_num;i++){
                int idx = convert_list(i, i);
                if(port_table[idx]->type > max_count)
                    first_start_port = idx;
            }
            if(max_count == 0){
                int idx = 0;
                while(idx<port_table_size&&){
                    idx++;
                    max_count = port_table[idx]->type;
                }//if idx = 0 return.
                first_start_port = idx;
            }

            int last_port = first_start_port;
            int travel_times = 0;
            while(1){
                //go!
                Node *go = port_table[last_port]->next;
                port_table[last_port]->type--;
                port_table[last_port]->next = go->next;
                output_solution(go);
                delete go;

                travel_times++;
                if(travel_times == times)break;
                //choose next port.
                last_port = get_next_port(last_port);
            }
        }
        int get_next_port(const int last_port){
            if(port_num == 1) return 0;
            else if(port_num == 2){
                for(int i=0;i<port_num;i++){
                    
                }
            }
        }
        void output_solution(Node *node){
            //show solution and return the next port.
            std::vector<Position> u = node->u;
            std::vector<Position> v = node->v;
            for(auto it = u.rbegin();it != u.rend();it++){
                std::cout<<it->first()<<" "<<it->second<<"\n";
            }
            for(auto it = v.begin();it != v.end();it++){
                std::cout<<it->first()<<" "<<it->second<<"\n";
            }
        }

        //tool functions below.
        void build_port_table(Node* node){
            int n = node->type;
            Node * insert_node = port_table[n];
            insert_node->type = insert_node->type + 1;
            while(insert_node->next != nullptr){
                insert_node = insert_node->next;
            }
            insert_node->next = node;
        }
        void get_port_number(Node *node){
            ///convert the input position and output position to port number.
            Position in = node->u.back();
            Position out = node->v.back();
            int in_port, out_port;
            for(int i=0;i<port_num;i++){
                if(in == Port[i]) in_port = i;
                if(out == Port[i]) out_port = i;
            }
            //if out<in, swap.
            if(in_port > out_port){
                //swap port
                int temp1 = in_port;
                in_port = out_port;
                out_port = temp1;
                //swap vector.
                std::vector<Position> temp2 = node->u;
                node->v = node->u;
                node->u = temp2;
            }
            //in * port_num + out = [in][out]
            node->type = convert_list(in_port, out_port);//0~15
        }
        int convert_list(const int& in, const int& out){
            ///port number encoder.
            if(port_num == 1) return 0;
            else if(port_num == 2){
                return in+out;
            }
            else if(port_num == 3){
                if(in == 0) return out;
                else return in+out+1;
            }
            else{//4
                if(in == 0) return out;
                else if(in == 1) return in+out+2;
                else return in+out+3;
            }
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

            //-----------------<debug>-----------------
            std::cout<<cnt+1<<"----------------------------------------\n";
            for(auto it = route.begin();it != route.end();it++){
                std::cout<<it->first<<" "<<it->second<<"\n";
            }
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
        std::vector<VistPosition> nodes_vec;
        std::vector<int> nodes_step_vec;
        std::vector<Node *> port_table;
        int port_table_size;
        Position Port[4];
        Position R;
        int step;
        int port_num;//how many port are there.
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
