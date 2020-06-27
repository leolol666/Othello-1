#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
 
struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
    Point operator=(const Point& rhs){
		this->x = rhs.x;
        this->y = rhs.y;
        return *this;
	}
};
 
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    OthelloBoard operator=(OthelloBoard &rhs)
    {
        for(int i=0; i<SIZE; i++){
            for(int j=0; j<SIZE; j++){
                this->board[i][j] = rhs.board[i][j];
            }
        }
        this->next_valid_spots = rhs.next_valid_spots;
        this->disc_count[EMPTY] = rhs.disc_count[EMPTY];
        this->disc_count[BLACK] = rhs.disc_count[BLACK];
        this->disc_count[WHITE] = rhs.disc_count[WHITE];
        this->cur_player = rhs.cur_player;
        this->done = rhs.done;
        this->winner = rhs.winner;
        return *this;
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        /*if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) { // no valid spots for both
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }*/
        return true;
    }
 
};
 
int me_player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> input_board;
std::vector<Point> next_valid_spots;
 
std::array<std::array<int, SIZE>, SIZE> weight;
OthelloBoard input;
void read_board(std::ifstream& fin) {
    fin >> me_player;
    input.cur_player = me_player;
    input.disc_count[0] = 0;
    input.disc_count[1] = 0;
    input.disc_count[2] = 0;
 
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> input.board[i][j];
            input.disc_count[input.board[i][j]]++;
        }
    }
}
 
void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
    input.next_valid_spots = next_valid_spots;
}
 
int count_minimax(OthelloBoard root, int step, int preweight, int alpha, int beta)
{
 
    int al = alpha, be = beta;
    if(step>4) return 0;
    if(root.cur_player==me_player){
        int max = -10000;
        if(root.next_valid_spots.size()!=0){
            for(auto p: root.next_valid_spots){
                OthelloBoard next = root;
                next.put_disc(p);
                int my_weight = weight[p.x][p.y] + count_minimax(next, step+1, weight[p.x][p.y], al, be);
                int motion_for_op = next.next_valid_spots.size();
                if(my_weight-motion_for_op * 20>max) max = my_weight-motion_for_op * 20;
                al = (al>max)? al : max;
                if(al>=be) break;
            }
        }
        return max;
    }
    else if(root.cur_player!=me_player){
        int min = 10000;
        if(root.next_valid_spots.size()!=0){
            for(auto p: root.next_valid_spots){
                OthelloBoard next = root;
                next.put_disc(p);
                int op_weight = (-1) * weight[p.x][p.y] + count_minimax(next, step+1, (-1)*weight[p.x][p.y], al, be);
                int motion_for_me = next.next_valid_spots.size();
                if(op_weight + motion_for_me * 20 < min) min = op_weight + motion_for_me * 20;
                be = (be<min)? be : min;
                if(al>=be) break;
            }
        }
        return min;
    }
}
 
void write_valid_spot(std::ofstream& fout) {
    int best_weight = -2700;
    Point best_move;
    if(input.disc_count[0]==1) best_move = input.next_valid_spots[0];
    else{
        int alpha = -2147483647;
        int beta = 2147483647;
        for(auto p:input.next_valid_spots){
            OthelloBoard stack1;
            stack1.put_disc(p);
 
            int max = weight[p.x][p.y] - stack1.next_valid_spots.size() * 10 + count_minimax(stack1, 0, 0, alpha, beta);
            if(max>best_weight){
                best_weight = max;
                best_move = p;
            }
            alpha = (alpha>max)? alpha : max;
            if(alpha>=beta) break;
        }
    }   
    // Remember to flush the output to ensure the last action is written to file.
    fout << best_move.x << " " << best_move.y << std::endl;
    fout.flush();
}
 
int main(int, char** argv) {
    for(int i=0; i<SIZE; i++){
        for(int j=0; j<SIZE; j++){
            if(i==0 || i==7){
                if(j==0 || j==7) weight[i][j] = 1200;
                else if(j==1 || j==6) weight[i][j] = -600;
                else weight[i][j] = 10;
            }
            else if(i==1 || i==6){
                if(j==0 || j==7) weight[i][j] = -600;
                else if(j==1 || j==6) weight[i][j] = -800;
                else weight[i][j] = -50;
            }
            else{
                if(j==0 || j==7) weight[i][j] = 200;
                else if(j==1 || j==6) weight[i][j] = -50;
                else weight[i][j] = 10;
            }
 
        }
    }
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    if(input.board[0][0]==input.cur_player){
        for(int i=1; i<7; i++){
            weight[i][0] = (900-100*i > weight[i][0])? 900-100*i : weight[i][0];
            weight[0][i] = (900-100*i > weight[0][i])? 900-100*i : weight[0][i];
        }
    }
    if(input.board[0][7]==input.cur_player){
        for(int i=1; i<7; i++){
            weight[i][7] = (900-100*i > weight[i][7])? 900-100*i : weight[i][7];
            weight[0][i] = (200+100*i > weight[0][i])? 200+100*i : weight[0][i];
        }
    }
    if(input.board[7][7]==input.cur_player){
        for(int i=1; i<7; i++){
            weight[i][7] = (200+100*i > weight[i][7])? 200+100*i : weight[i][7];
            weight[7][i] = (200+100*i > weight[7][i])? 200+100*i : weight[7][i];
        }
    }
    if(input.board[7][0]==input.cur_player){
        for(int i=1; i<7; i++){
            weight[i][0] = (200+100*i > weight[i][0])? 200+100*i : weight[i][0];
            weight[7][i] = (900-100*i > weight[7][i])? 900-100*i : weight[7][i];
        }
    }
 
 
 
 
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
 
