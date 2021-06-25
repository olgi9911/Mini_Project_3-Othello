#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}  // float to int
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
};

int player;
const int SIZE = 8;
const int MAX = INT32_MAX;
const int MIN = INT32_MIN;
const int weight_table[SIZE][SIZE] =
    {
        {500, -25, 10, 5, 5, 10, -25, 500},
        {-25 - 45, 1, 1, 1, 1, -45, -25},
        {10, 1, 3, 2, 2, 3, 1, 10},
        {5, 1, 2, 1, 1, 2, 1, 5},
        {5, 1, 2, 1, 1, 2, 1, 5},
        {10, 1, 3, 2, 2, 3, 1, 10},
        {-25, -45, 1, 1, 1, 1, -45, -25},
        {500, -25, 10, 5, 5, 10, -25, 500},
};
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

class OthelloBoard {
   public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                           Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                           Point(1, -1), Point(1, 0), Point(1, 1)}};
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
        for (Point dir : directions) {
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
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s : discs) {
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
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> new_board) {
        cur_player = player;
        done = false;
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                board[i][j] = new_board[i][j];
    }
    OthelloBoard(const OthelloBoard& state) {  // copy constructor
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = state.board[i][j];
            }
        }
        cur_player = state.cur_player;
        done = state.done;
        next_valid_spots = state.next_valid_spots;
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
        if (!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        //disc_count[cur_player]++;
        //disc_count[EMPTY]--;*/
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
            }
        }
        return true;
    }
};

int weight(OthelloBoard state) {
    int val = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (state.board[i][j] == player)
                val += weight_table[i][j];
            else if (state.board[i][j] == (3 - player))
                val -= weight_table[i][j];
        }
    return val;
}

int set_value(OthelloBoard state) {
    int value = 0;
    if (state.done) {
        return state.disc_count[player] - state.disc_count[3 - player];
    } else {
        value += weight(state);
    }
    return value;
}

int minimax(OthelloBoard state, Point p, int depth) {
    OthelloBoard new_state = state;
    new_state.put_disc(p);
    int val = 0;
    if (state.done || depth == 0) {
        return set_value(new_state);
    } else if (state.cur_player == player) {
        val = MIN;
        for (auto new_p : new_state.next_valid_spots) {
            val = std::max(val, minimax(new_state, new_p, depth - 1));
        }
    } else if (state.cur_player == 3 - player) {
        val = MAX;
        for (auto new_p : new_state.next_valid_spots) {
            val = std::min(val, minimax(new_state, new_p, depth - 1));
        }
    }
    return val;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
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
}

void write_valid_spot(std::ofstream& fout) {
    Point final;
    OthelloBoard state(board);  // construct the cur_state
    int val = 0, greatest = MIN;
    for (auto pt : next_valid_spots) {
        val = minimax(state, pt, 3);
        if (val >= greatest) {
            greatest = val;
            final = pt;
        }
    }
    fout << final.x << " " << final.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}