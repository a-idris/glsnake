#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>
#include <cmath>
#include <ctime>

//snake block
//for head, direction = arrow direction (last changed)
//for else, direction = prev direction of next block

// time to traverse one block = distance / velocity. thus increment time, once passed, snap to. update INTEGER coordinates


class Game {
	private:
		Snake * snake;
		Food food;
		clock_t start_time, ongoing_time, block_time, block_ongoing_time, food_time, food_ongoing_time;
		float velocity;
		int grid_size, score;
	public:
		Game(int grid_size);
		~Game();
		int get_score() const { return score; }
		Snake * get_snake() const { return snake; } 
		void update(clock_t time_elapsed);
		void change_direction(vector_t);
}

Game::Game(int grid_size) : grid_size(grid_size), score(0) {
	velocity = 2.0f;
	//time for a snake node to move one block at this velocity
	// block distance / velocity (in clocks per sec))
	block_time = 1 / 2.0f * CLOCKS_PER_SEC; //NEEDS FIXING
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); //put snake in the middle of the grid initially
}

Game::~Game() {
	delete snake;
}

void Game::start(clock_t time) {
	start_time = time;
	ongoing_time = 0;
}

void Game::update(clock_t time_elapsed) {
	block_ongoing_time += time_elapsed;
	food_ongoing_time += time_elapsed;

	if (block_ongoing_time >= block_time) {
		block_ongoing_time -= block_time;
		snake->update(); //move by 1 square

/*		if (!snake.isAlive()) {

		} else {
			//snake hasn't collided with itself. check walls
			//bounds checks
		}

		check_alive();
		check_food();*/
	} 

	//timer for food spawn
/*	if (food_ongoing_time >= food_time) {
		//schedule food
		//set food time randomly
		food.spawn(x,y,radius,time_duration);

		//ENCODE FOOD == ACTIVE. PERHAPS TIME PARAMETERS TO ENCODE.

		int rand_amount = (rand() % 5000) + 5000;  
		food_time = food.get_time() + rand_amount;
	}*/

	//if snap to (== head arrived at dest square, then do direction change logic (iterate thru snake and set direction to next.direction))
}

// "SCHEDULE TURN" / ENQUEUE>?!
void change_direction(vector_t direction) {
	snake.enqueue_direction(direction);
	// snake.get_head().set_direction(direction);
}

class Snake {
	private:
		SnakeNode * head;
		SnakeNode * tail;
		size_t length;
	public:
		Snake();
		Snake(int x, int y); 
		SnakeNode * get_head() const { return head; }
		SnakeNode * get_tail() const { return tail; }
		void update();
		void append();
}

Snake::Snake() length(1), head(0, 0) {
	vector_t right = {1, 0};
	head = new SnakeNode(0, 0, right);
	head->set_next(NULL);
	head->set_prev(NULL);
	tail = head;
}

Snake::Snake(int x, int y) : head(x, y) {
	head.set_next(null);
	head.set_prev(null);
}

Snake::~Snake() {
	delete head; 
}

void Snake::update() {
	//iterate through, update xpos, ypos based on directions etc

	for (int i = 0, SnakeNode * snode = head; i < length; i++, snode = snode->get_next()) {
		vector_t direction = snode->get_direction();
		snode->set_x(snode->get_x() + direction.x);
		snode->set_y(snode->get_y() + direction.y);
		if (snode == head)
			continue;
		//all the non head blocks get their new direction from the direction of the previous snake node
		SnakeNode * prev = snode.get_prev();
		snode.set_direction(prev->get_direction());
	}
}

void append() {

	//UPDATE AND THEN APPEND, BUT SAVE TAIL VALS BEFORE CHANGING ITS DIRECTION. TAIL_DIRECTION VAR. THEN CAN CALC PROPER X AND Y VALS FOR NEW NODE

	//get tail values
	float x = tail->get_x();
	float y = tail->get_y();
	vector_t direction = tail->get_direction();

	//add to end	
	SnakeNode * node = new SnakeNode(x, y, direction);
	node->set_prev(tail);
	node->set_next(NULL);
	tail->set_next(node);

	tail = node;
	length++;
}

class SnakeNode {
	private:
		int x, y; //PRIVATE FLOAT, PUBLIC INT? OR 2 TYPES OF XPOS?
		vector_t direction;

		//USE QUEUE OR DEQUE FOR DIRECTION. enqueu from change_direction, on update - dequeue?

		SnakeNode * next;
		SnakeNode * prev;
	public:
		SnakeNode() {};
		SnakeNode(int x, int y): x(x), y(y);  
		SnakeNode(int x, int y, vector_t directionVector): x(x), y(y), direction(direction) {} 
		~SnakeNode();
		int get_x() const { return x; }
		int get_y() const { return y; }
		vector_t get_direction() const { return direction; }
		SnakeNode get_prev() const { return SnakeNode; }
		SnakeNode get_next() const { return SnakeNode; }
		void set_next(SnakeNode * next) { this.next = next; }
		void set_prev(SnakeNode * prev) { this.prev = prev; }
		void set_direction(vector_t&);


}

SnakeNode::SnakeNode(int x, int y) : x(x), y(y) {
	vector_t right = {1, 0};
	direction = right;
	// no need to set next and prev, set in member initialisation list
}

SnakeNode::~SnakeNode() {
	delete next;
	delete prev;
}

void SnakeNode::set_direction(vector_t & direction_vector) {
	//error check

	if queue.peek == vector 
		return;

	if (direction_vector.x + direction_vector.y != 1) {
		throw std::invalid_argument("direction vector must be of length 1 and corresponding to one of the cardinal directions");
	}
	direction = direction_vector;
}

void set_next(SnakeNode * next) { 
	this.next = next; 
}

void set_prev(SnakeNode * prev) { 
	this.prev = prev; 
}

struct vector_t {
	int x;
	int y;
};

class Food {
	private:
		float x, y, radius;
	public:
		Food() {} 
		float get_x() const { return x; }
		float get_y() const { return y; }
		float get_radius() const { return radius; }
		void set_x(float);
		void set_y(float);
		void set_radius(float);
		int set(float, float, float);
}

void Food::set_x(float x) {
	this.x = x;
}

void Food::set_y(float y) {
	this.y = y;
}

void Food::set_radius(float radius) {
	this.radius = radius;
}

float Food::set(float x, float y, float radius) {
	this.x = x;
	this.y = y;
	this.radius = radius;

	return 5 + std::math
}
