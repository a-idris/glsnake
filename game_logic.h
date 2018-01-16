#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <ctime>

//forward declared dependencies
class Snake;
class SnakeNode;
class Food;

struct vector_t {
	int x;
	int y;
};

class Game {
	private:
		Snake * snake;
		Food * food;
		long start_time, ongoing_time, block_time, block_ongoing_time, food_time, food_ongoing_time;
		float velocity;
		int grid_size, score;
	public:
		Game(int grid_size);
		~Game();
		int get_score() const { return score; }
		Snake * get_snake() const { return snake; } 
		Food * get_food() const { return food; }
		void update(long time_elapsed);
		void change_direction(vector_t);
		void start(long);
};


class Snake {
	private:
		SnakeNode * head;
		SnakeNode * tail;
		size_t length;
	public:
		Snake();
		Snake(int x, int y); 
		~Snake();
		SnakeNode * get_head() const { return head; }
		SnakeNode * get_tail() const { return tail; }
		size_t get_length() const { return length; } 
		void update();
		void append();
		void enqueue_direction(vector_t);
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
		SnakeNode(int x, int y, vector_t directionVector): x(x), y(y), direction(direction) {} 
		~SnakeNode();
		int get_x() const { return x; }
		int get_y() const { return y; }
		vector_t get_direction() const { return direction; }
		SnakeNode * get_prev() const { return prev; }
		SnakeNode * get_next() const { return next; }
		void set_next(SnakeNode * next) { this->next = next; }
		void set_prev(SnakeNode * prev) { this->prev = prev; }
		void set_direction(vector_t&);
		void update();
};


class Food {
	private:
		int x, y, radius;
	public:
		Food() {} 
		int get_x() const { return x; }
		int get_y() const { return y; }
		int get_radius() const { return radius; }
		void set_x(int x);
		void set_y(int y);
		void set_radius(int);
		int set(int, int, int);
};

#endif