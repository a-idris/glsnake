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


class Game {
	private:
		Snake snake;
		Food food;
		const float velocity;
		int grid_size;
		int score;
		clock_t ongoing_time;
	public:
		Game(int grid_size);
		void update(int timeSlice);
		void change_direction(vector_t);
}

Game::Game(int grid_size) : grid_size(grid_size), score(0), velocity(2.0f) {
	int midPoint = static_cast<int>(grid_size / 2.0f);
	snake(midPoint, midPoint);
	ongoing_time = clock();
}

void Game::update(int timeSlice) {
	ongoing += timeSlice; // redundant

	if (ongoing >= food_scheduled) {
		food.spawn();
		food_scheduled = false;
	}

	snake.update(timeSlice, velocity);

	if (!snake.isAlive()) {

	} else {
		//snake hasn't collided with itself. check walls
		//bounds checks
	}

	//schedule, animate food
	if (!food_scheduled) {
		int rand_amount = (rand() % 5000) + 5000;  
		scheduled = ongoing + rand_amount;
		food_scheduled = true; //NEED 2 VARS 
	}

	//if snap to (== head arrived at dest square, then do direction change logic (iterate thru snake and set direction to next.direction))
}

// "SCHEDULE TURN" / ENQUEUE>?!
void change_direction(vector_t direction) {
	snake.get_head().set_direction(direction);
}

class Snake {
	private:
		SnakeNode head;
		size_t length;
	public:
		Snake();
		Snake(int x, int y); 
		SnakeNode get_head() const { return head; }
		void update(int, float);
		void append();
}

Snake::Snake() length(1), head(0, 0) {
	vector_t right = {1, 0};
	head.set_direction(right); //unneccessary
	//nulls
	head.set_before(null);
	head.set_ahead(null);
}

Snake::Snake(int x, int y) : head(x, y) {
	head.set_before(null);
	head.set_ahead(null);
}

void Snake::update(int timeSlice, float velocity) {
	//iterate through, update xpos, ypos based on directions etc
}

void append() {
	length++;

	//get tail
	SnakeNode tmp = head;
	while (tmp.get_ahead() != null) {
		tmp = tmp.get_ahead; //CHANGE TO GET_NEXT
	}

	float x = tmp.get_x();
	float y = tmp.get_y();
	vector_t direction = tmp.get_direction();
	//add to end
	SnakeNode snake_node (x, y, direction);
	snake_node.set_before(tmp); //OR SET_AHEAD
}

class SnakeNode {
	private:
		float x, y; //PRIVATE FLOAT, PUBLIC INT? OR 2 TYPES OF XPOS?
		vector_t direction;
		SnakeNode before;
		SnakeNode ahead;
	public:
		SnakeNode() {};
		SnakeNode(float x, float y): x(x), y(y);  
		SnakeNode(float x, float y, vector_t directionVector): x(x), y(y), direction(direction) {} 
		float get_x() const { return x; }
		float get_y() const { return y; }
		float get_direction() const { return direction; }
		SnakeNode get_ahead() const { return SnakeNode; }
		SnakeNode get_before() const { return SnakeNode; }
		void set_direction(vector_t);
		void set_before(SnakeNode before) { this.before = before; }
		void set_ahead(SnakeNode ahead) { this.ahead = ahead; }
}

SnakeNode::SnakeNode(float x, float y) : x(x), y(y) {
	vector_t right = {1, 0};
	direction = right;
	// no need to set before and ahead, set in member initialisation list
}

void SnakeNode::set_direction(vector_t direction_vector) {
	//error check
	if (direction_vector.x + direction_vector.y != 1) {
		throw std::invalid_argument("direction vector must be of length 1 and corresponding to one of the cardinal directions");
	}
	direction = direction_vector;
}

void set_before(SnakeNode before) { 
	this.before = before; 
}

void set_ahead(SnakeNode ahead) { 
	this.ahead = ahead; 
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
