#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <ctime>
#include <vector>

//forward declared dependencies
class Snake;
class SnakeNode;
class Food;

struct coord_t {
	int x;
	int y;

	bool operator==(const coord_t & other) const {
 		return other.x == x && other.y == y;
 	}
};

class Food {
	private:
		int x, y;
		long time_expires;
		bool eaten;

	public:
		Food() {}; 
		int get_x() const { return x; }
		int get_y() const { return y; }
		coord_t get_coords();
		long set(int x, int y, long curr_time);
		bool is_active(long curr_time);
		void eat();
};

class Game {
	private:
		Snake * snake;
		Food food;
		long total_time, block_time, block_ongoing_time, food_time, food_ongoing_time;
		float velocity;
		int grid_size, score;
		bool out_of_bounds(const coord_t &);
	public:
		Game(int grid_size);
		~Game();
		void start(); //REMOVE
		void update(long time_elapsed);
		int get_score() const { return score; }
		bool food_active();
		coord_t get_food();
		std::vector<coord_t> get_snake_coords();
		void change_direction(coord_t &);
};


class SnakeNode {
	private:
		int x, y; 
		coord_t direction;

		//USE QUEUE OR DEQUE FOR DIRECTION. enqueu from change_direction, on update - dequeue?
	public:
		SnakeNode() {};
		SnakeNode(int x, int y);  
		SnakeNode(int x, int y, coord_t direction_vector) : x(x), y(y), direction(direction_vector) {}
		SnakeNode(const SnakeNode & copy);
		SnakeNode clone(); 
		int get_x() const { return x; }
		int get_y() const { return y; }
		coord_t get_coords();
		coord_t get_direction() const { return direction; }
		void set_direction(coord_t); //&
		void update();
		bool collides(const SnakeNode &) const;
		bool collides(const coord_t &) const;
};

class Snake {
	private:
		std::vector<SnakeNode *> snake_nodes;
		SnakeNode last_tail;
	public:
		Snake();
		Snake(int x, int y);
		~Snake();
		SnakeNode * get_head();
		void update();
		void append();
		bool has_collision();
		bool contains(const coord_t &); 
		void enqueue_direction(const coord_t & direction);
		std::vector<coord_t> get_snake_coords();
};

#endif