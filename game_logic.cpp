#include <cmath>
#include <stddef.h>
#include <iostream>
#include <cstdlib>

#include <unistd.h>

#include "game_logic.h"

//snake block
//for head, direction = arrow direction (last changed)
//for else, direction = prev direction of next block

// time to traverse one block = distance / velocity. thus increment time, once passed, snap to. update INTEGER coordinates

//Game func implementations

Game::Game(int grid_size) : grid_size(grid_size), score(0) {
	velocity = 5.0f; // blocks / sec
	//time for a snake node to move one block at this velocity
	block_time = 1 / velocity * 1000; //convert to miliseconds 
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); //put snake in the middle of the grid initially
}

Game::~Game() {
	delete snake;
}

void Game::start() {
	total_time = 0;
	block_ongoing_time = 0;
	food_ongoing_time = 0;
	food_time = 1000;
}

void Game::update(long time_elapsed) {
	total_time += time_elapsed;
	block_ongoing_time += time_elapsed;
	food_ongoing_time += time_elapsed;

	// if (true) {
	if (block_ongoing_time >= block_time) {


		block_ongoing_time -= block_time;

		snake->update(); //move by 1 square
		
		//get new snake coords
		std::set<vector_t> snake_coords = get_snake_coords();

		//collision check
		bool died = false;
		vector_t head = { snake->get_head()->get_x(), snake->get_head()->get_y() };
		//check collision
		if (contains(get_snake_coords(false), head)) {
			//death logic
			died = true;
		} else if (head.x < 0 || head.x >= grid_size || head.y < 0 || head.y >= grid_size) {
			died = true;
		}

		if (died) {
			std::cout << "dead" << std::endl;
			//stop animation.
			//reset vals.
			//display text overlay. play again / quit.
			// pass death back to display? to change key handlers? or just is_alive func
		}

		if (food.is_active(total_time)) {
			vector_t food_pos = { food.get_x(), food.get_y() };
			if (head == food_pos) {
				std::cout << "ATE" << std::endl;
				food.eat();
				snake->append();
				// return;
			}
		}

		//timer for food spawn
		if (food_ongoing_time >= food_time) {
			//schedule food
			//set food time randomly
			vector_t food_pos = {0, 0};
			
			do {
				food_pos.x = rand() % grid_size;
				food_pos.y = rand() % grid_size;
			} while(contains(snake_coords, food_pos));


			long time_active = food.set(food_pos.x, food_pos.y, total_time);

			//choose next when to respawn the food
			int rand_amount = (rand() % 3000) + 2000;
			// int rand_amount = (rand() % 5000) + 5000;
			food_ongoing_time = 0;  
			// food_time = 3000;
			food_time = time_active + rand_amount;
		}
	}
}	


bool Game::contains(std::set<vector_t> vectors, vector_t to_find) {
	std::set<vector_t>::iterator it = vectors.find(to_find);
	return it != vectors.end();
}

 std::set<vector_t> Game::get_snake_coords(bool include_head) {
 	//return coordinates of snake as set of vector_t coordinates
	std::set<vector_t> coords; //initial capacity = snake length
	SnakeNode * snode = snake->get_head();
	size_t length = snake->get_length();
	for (int i = include_head ? 0 : 1; i < length; i++) {
		vector_t coord = {snode->get_x(), snode->get_y()};
		coords.insert(coord);
		snode = snode->get_next();
	}
	return coords;
}

// "SCHEDULE TURN" / ENQUEUE>?!
void Game::change_direction(vector_t direction) {
	// snake->enqueue_direction(direction);
	snake->get_head()->set_direction(direction);
}

vector_t Game::get_food() {
	vector_t food_pos = { food.get_x(), food.get_y() };
			// std::cout << food_pos.x << "," << food_pos.y << std::endl;
	return food_pos;
}

bool Game::food_active() {

	bool val = food.is_active(total_time);
	// std::cout << val << std::endl;
	return val;
}

//Snake func implementations

Snake::Snake(): length(1) {
	vector_t right = {1, 0};
	SnakeNode head (0, 0, right);
	head->set_next(NULL);
	head->set_prev(NULL);
	tail = head;
}

Snake::Snake(int x, int y) : length(1) {
	vector_t right = {1, 0};
	head = new SnakeNode(x, y, right);
	head->set_next(NULL);
	head->set_prev(NULL);
	tail = head;
}

Snake::~Snake() {
	SnakeNode * snode = head;
	SnakeNode * tmp;
	while (snode != NULL) {
		tmp = snode->get_next();
		delete snode;
		snode = tmp;
	}

	// delete head; //will cascade, since SnakeNode will call delete on prev and next SnakeNodes
}

void Snake::update() {
	//iterate through, update xpos, ypos based on directions etc

	SnakeNode * snode = tail;
	for (int i = 0; i < length; i++) {
		//update x and y by the direction vector 
		std::cout << i << ": this=" << this << ", prev=" << snode->get_prev() << ", next=" << snode->get_next() << std::endl; 

		snode->update();

		if (snode == head)
			continue;
		//all the non head blocks get their new direction from the direction of the previous snake node
		SnakeNode * prev = snode->get_prev();
		vector_t direction = prev->get_direction();
		snode->set_direction(direction);
		snode = snode->get_next();
	}
}

void Snake::append() {

	//UPDATE AND THEN APPEND, BUT SAVE TAIL VALS BEFORE CHANGING ITS DIRECTION. TAIL_DIRECTION VAR. THEN CAN CALC PROPER X AND Y VALS FOR NEW NODE

	//get tail values
	float x = tail->get_x() - 1;
	float y = tail->get_y() - 1;
	vector_t direction = tail->get_direction();
	std::cout << "ASD" << x << std::endl;

	//add to end	
	SnakeNode node (x, y, direction);
	std::cout << "ASD" << x << std::endl;
	node.set_prev(tail);
	node.set_next(NULL);
	tail->set_next(&node);


	tail = &node;
	length++;

	SnakeNode * snode = head;
	std::cout << "head: (" << snode->get_x() << "," << snode->get_y() <<")"<<std::endl;
	snode = head->get_next();
	std::cout << "head+1: (" << snode->get_x() << "," << snode->get_y() <<")"<<std::endl;

	snode = tail;
	std::cout << "tail: (" << tail->get_x() << "," << tail->get_y() <<")"<<std::endl;
	snode = tail->get_prev();
	std::cout << "tail-1: (" << snode->get_x() << "," << snode->get_y() <<")"<<std::endl;


}

void Snake::enqueue_direction(vector_t & direction) {
	return;
}

//SnakeNode func implementations


SnakeNode::SnakeNode(int x, int y) : x(x), y(y) {
	vector_t right = {1, 0};
	direction = right;
	// no need to set next and prev, set in member initialisation list
}

SnakeNode::~SnakeNode() {
	//if (next) delete next;
	std::cout << "next=" << next << "prev" << prev << std::endl;
	if (next) {
		// delete next;
	}
	if (prev) { 
		// delete prev;
	}
}

void SnakeNode::set_direction(vector_t direction_vector) {
	//error check

	// if queue.peek == vector 
		// return;

	if (std::abs(direction_vector.x) + std::abs(direction_vector.y) != 1) {
		std::cout << "(" << direction_vector.x << ", " << direction_vector.y <<  ")" << std::endl; 
		throw std::invalid_argument("direction vector must be of length 1 and corresponding to one of the cardinal directions");
	}
	direction = direction_vector;
}

void SnakeNode::update() {
	x += direction.x;
	y += direction.y;
}

// Food func implementations
long Food::set(int x, int y, long curr_time) {
	eaten = false; //restore active status

	this->x = x;
	this->y = y;
	
	//time active: from 4-7 seconds
	// long time_active = (rand() % 3000) + 1000; 
	long time_active = (rand() % 3000) + 4000; 
	time_expires = curr_time + time_active;

	return time_active;
}

bool Food::is_active(long time) {
	return time < time_expires && !eaten;
}

void Food::eat() {
	eaten = true;
}
