#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <ctime>
#include <set>

//forward declared dependencies
class Snake;
class SnakeNode;
class Food;

struct vector_t {
	int x;
	int y;

	//overload operators for use in std::set	
	bool operator==(const vector_t & other) const {
		return other.x == x && other.y == y;
	}

	bool operator<(const vector_t & other) const {
		if (x < other.x) {
			return true;
		} else if (x > other.x) {
			return false;
		}
		else {
			return y < other.y;
		}
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
		bool contains(std::set<vector_t> vectors, vector_t to_find);
	public:
		Game(int grid_size);
		~Game();
		int get_score() const { return score; }
		Snake * get_snake() const { return snake; } 
		vector_t get_food();
		bool food_active();
		void update(long time_elapsed);
		void change_direction(vector_t);
		std::set<vector_t> get_snake_coords(bool include_head=true);
		void start(); //REMOVE
};


class Snake {
	private:
		std::list<SnakeNode> snake_nodes;
	public:
		Snake();
		Snake(int x, int y); 
		~Snake();
		// SnakeNode * get_head() const { return head; }
		// SnakeNode * get_tail() const { return tail; }
		size_t get_length() const { return length; } 
		void update();
		void append();
		void enqueue_direction(vector_t & direction);
};


class SnakeNode {
	private:
		int x, y; //PRIVATE FLOAT, PUBLIC INT? OR 2 TYPES OF XPOS?
		vector_t direction;

		//USE QUEUE OR DEQUE FOR DIRECTION. enqueu from change_direction, on update - dequeue?

		SnakeNode * next;
		SnakeNode * prev;
	public:
		SnakeNode() {};
		SnakeNode(int x, int y);  
		SnakeNode(int x, int y, const vector_t directionVector): x(x), y(y), direction(direction) {} 
		~SnakeNode();
		int get_x() const { return x; }
		int get_y() const { return y; }
		vector_t get_direction() const { return direction; }
		SnakeNode * get_prev() const { return prev; }
		SnakeNode * get_next() const { return next; }
		void set_next(SnakeNode * next) { this->next = next; }
		void set_prev(SnakeNode * prev) { this->prev = prev; }
		void set_direction(vector_t);
		void update();
};


#endif