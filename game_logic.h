#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>
#include <queue>

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
		// keep track of whether food is active using time parameters
		long time_set, time_expires;
		bool eaten;

	public:
		Food() {}; 
		int get_x() const { return x; }
		int get_y() const { return y; }
		coord_t get_coords();
		//set the food to be active at the input coordinates for a random duration of time
		long set(int x, int y, long curr_time);
		// eating the food deactivates it and returns the amount of points awarded
		int eat(long curr_time);
		bool is_active(long curr_time);
};

class Game {
	private:
		Snake * snake;
		//one food object throughout the game lifetime; just change its coordinates and active status
		Food food;
		long total_time, block_time, block_ongoing_time, food_time, food_ongoing_time;
		int velocity, grid_size, score, difficulty_delta;
		const int MIN_VELOCITY, MAX_VELOCITY;
		bool out_of_bounds(const coord_t &);
	public:
		Game(int grid_size);
		~Game();
		//update game world by time_elapsed and return alive state
		bool update(long time_elapsed); 
		int get_score() const { return score; }
		//return difficulty in range[0, 10]
		int get_difficulty();		
		SnakeNode get_head();
		std::vector<coord_t> get_snake_coords();
		bool food_active();
		coord_t get_food();
		void change_direction(const coord_t &);
		int increase_difficulty();
		int decrease_difficulty();
		//reset, for after death
		void reset();
};


class SnakeNode {
	private:
		int x, y; 
		coord_t direction;

	public:
		SnakeNode() {};
		SnakeNode(int x, int y);  
		SnakeNode(int x, int y, const coord_t & direction_vector) : x(x), y(y), direction(direction_vector) {}
		SnakeNode(const SnakeNode & copy);
		SnakeNode clone(); 
		int get_x() const { return x; }
		int get_y() const { return y; }
		coord_t get_coords();
		coord_t get_direction() const { return direction; }
		void set_direction(const coord_t &); 
		//advance the snakenode in the direction stored
		void update();
		//check if it has the same coordinates as some other SnakeNode or coord
		bool collides(const SnakeNode &) const;
		bool collides(const coord_t &) const;
};

class Snake {
	private:
		//store the SnakeNodes on the heap with the vector holding the pointers 
		std::vector<SnakeNode *> snake_nodes;
		//store the user input directions in a queue to be able to process them all
		std::queue<coord_t> directions;
		//store the details of the snake_nodes tail in case need to append
		SnakeNode last_tail;
	public:
		Snake();
		Snake(int x, int y);
		~Snake();
		SnakeNode * get_head();
		//iterate through the snake_nodes and update each node.
		void update();
		//append a copy of last_tail to snake_nodes
		void append();
		void enqueue_direction(const coord_t & direction);
		//check if the snake head is colliding with one of its body nodes
		bool has_collision();
		//check whether 
		bool contains(const coord_t &); 
		std::vector<coord_t> get_snake_coords();
};

#endif