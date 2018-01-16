#include <cmath>
#include <stddef.h>
#include <iostream>

#include <unistd.h>

#include "game_logic.h"

//snake block
//for head, direction = arrow direction (last changed)
//for else, direction = prev direction of next block

// time to traverse one block = distance / velocity. thus increment time, once passed, snap to. update INTEGER coordinates

//Game func implementations

Game::Game(int grid_size) : grid_size(grid_size), score(0) {
	velocity = 2.0f;
	//time for a snake node to move one block at this velocity
	// block distance / velocity (in clocks per sec))
	// block_time = 1 / (2.0f / CLOCKS_PER_SEC); //NEEDS FIXING
	block_time = CLOCKS_PER_SEC; //NEEDS FIXING
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); //put snake in the middle of the grid initially
}

Game::~Game() {
	delete snake;
	delete food;
}

void Game::start(long time) {
	start_time = time;
	block_ongoing_time = 0;
	food_ongoing_time = 0;
}

void Game::update(long time_elapsed) {
	block_ongoing_time += time_elapsed;
	food_ongoing_time += time_elapsed;

	// if (true) {
	if (block_ongoing_time >= block_time) {


		block_ongoing_time -= block_time;

		snake->update(); //move by 1 square
		// usleep(100000);
		
		// int counter = 0;
		// clock_t start = clock();

		// time_t timer1, timer2;
		// time(&timer1);
		// time(&timer2);
		// std::cout << ctime(&timer1) << std::endl;
		// // usleep(5000000);
		// // time(&timer2);
		// double diff = 0;
		// while (diff < 5) {
		// 	// t = clock() - t;
		// 	diff = difftime(timer2, timer1);
		// 	// std::cout << diff << "DIFF" << std::endl;
		// 	// std::cout << ctime(&timer2) << std::endl;
		// 	time(&timer2);

		// 	int j = 10;
		// 	for (int i = 0; i < 3; i++) {
		// 		j *= std::pow(j,i);
		// 	}
		// 	// usleep(1000000);
		// 	// usleep(1000000);
		// 	// std::cout << ++counter << std::endl;

		// }

		// clock_t end = clock();
		
		// std::cout << float(end - start) / CLOCKS_PER_SEC << "SSS" <<diff << std::endl; 


		// vector_t direction = snake->get_head()->get_direction();
		// std::cout << "block_time " << block_time << std::endl;   
		// std::cout << "time elapsed  " << time_elapsed << std::endl;   

		// std::cout << direction.x << "," << direction.y << std::endl;   
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
void Game::change_direction(vector_t direction) {
	// snake->enqueue_direction(direction);
	snake->get_head()->set_direction(direction);
}


//Snake func implementations

Snake::Snake(): length(1) {
	vector_t right = {1, 0};
	head = new SnakeNode(0, 0);
	head->set_direction(right);
	head->set_next(NULL);
	head->set_prev(NULL);
	tail = head;
}

Snake::Snake(int x, int y) : length(1) {
	vector_t right = {1, 0};
	head = new SnakeNode(x, y);
	head->set_direction(right);
	head->set_next(NULL);
	head->set_prev(NULL);
	tail = head;
}

Snake::~Snake() {
	delete head; //will cascade, since SnakeNode will call delete on prev and next SnakeNodes
}

void Snake::update() {
	//iterate through, update xpos, ypos based on directions etc

	SnakeNode * snode = head;
	for (int i = 0; i < length; i++) {
		//update x and y by the direction vector 
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

//SnakeNode func implementations


SnakeNode::SnakeNode(int x, int y) : x(x), y(y) {
	vector_t right = {1, 0};
	direction = right;
	// no need to set next and prev, set in member initialisation list
}

SnakeNode::~SnakeNode() {
	//if (next) delete next;
	delete next;
	delete prev;
}

void SnakeNode::set_direction(vector_t & direction_vector) {
	//error check

	// if queue.peek == vector 
		// return;

	if (std::abs(direction_vector.x) + std::abs(direction_vector.y) != 1) {
		throw std::invalid_argument("direction vector must be of length 1 and corresponding to one of the cardinal directions");
	}
	direction = direction_vector;
}

// void SnakeNode::set_next(SnakeNode * next) { 
// 	this->next = next; 
// }

// void SnakeNode::set_prev(SnakeNode * prev) { 
// 	this->prev = prev; 
// }

void SnakeNode::update() {
	x += direction.x;
	y += direction.y;
}

// Food func implementations
void Food::set_x(int x) {
	this->x = x;
}

void Food::set_y(int y) {
	this->y = y;
}

void Food::set_radius(int radius) {
	this->radius = radius;
}

int Food::set(int x, int y, int radius) {
	this->x = x;
	this->y = y;
	this->radius = radius;

	// return 5 + std::math
	return 0;
}
