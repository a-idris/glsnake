#include <cmath>
#include <stddef.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include <unistd.h>

#include "game_logic.h"

//Game func implementations

Game::Game(int grid_size) : grid_size(grid_size), score(0) {
	//difficulty d/D: increment/decrement velocity
	// blocks / sec
	velocity = 10.0f; 
	//time for a snake node to traverse one block = distance / velocity (converted to milliseconds). 
	block_time = 1 / velocity * 1000; 
	//put snake in the middle of the grid initially
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); 
}

Game::~Game() {
	delete snake;
}

//reset
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

	if (block_ongoing_time >= block_time) {

		block_ongoing_time -= block_time;

		//move by 1 square
		snake->update(); 
		
		//get new snake coords
		std::vector<coord_t> snake_coords = snake->get_snake_coords();
		coord_t head = snake->get_head()->get_coords();
		// std::cout << "head direction" << snake->get_head()->get_direction().x << ", " << snake->get_head()->get_direction().y << std::endl;

		//check snake collision w/ itself or if the head is of bounds. only need to check head because other blocks are old positions of head 
		if (snake->has_collision() || out_of_bounds(head)) {
			//death logic
			std::cout << "dead" << std::endl;
			//stop animation.
			//reset vals.
			//display text overlay. play again / quit.
			// pass death back to display? to change key handlers? or just is_alive func
		}

		if (food.is_active(total_time)) {
			coord_t food_pos = { food.get_x(), food.get_y() };
			if (head == food_pos) {
				std::cout << "ATE" << std::endl;
				food.eat(); //add time parameter. use for proportional score to give
				snake->append();
			}
		}

		//timer for food spawn
		if (food_ongoing_time >= food_time) {
			//generate random coordinates for the food that don't collide with the snake
			coord_t food_pos = {0, 0};
			do {
				food_pos.x = rand() % grid_size;
				food_pos.y = rand() % grid_size;
			} while (snake->contains(food_pos));

			long time_active = food.set(food_pos.x, food_pos.y, total_time);

			//choose next when to respawn the food
			int rand_amount = (rand() % 3000) + 2000;
			// int rand_amount = (rand() % 5000) + 5000;
			food_ongoing_time = 0;  
			food_time = time_active + rand_amount;
			// food_time = 3000;
		}
	}
}	

bool Game::out_of_bounds(const coord_t & coord) {
	return coord.x < 0 || coord.x >= grid_size || coord.y < 0 || coord.y >= grid_size;
}

// "SCHEDULE TURN" / ENQUEUE>?!
void Game::change_direction(coord_t & direction) {
	// snake->enqueue_direction(direction);
	snake->get_head()->set_direction(direction);
}

std::vector<coord_t> Game::get_snake_coords() {
	return snake->get_snake_coords();
}

coord_t Game::get_food() {
	return food.get_coords();
}

bool Game::food_active() {

	bool val = food.is_active(total_time);
	// std::cout << val << std::endl;
	return val;
}

//Snake func implementations

Snake::Snake() {
	coord_t right = {1, 0};
	SnakeNode * head = new SnakeNode(0, 0, right);
	snake_nodes.push_back(head);	
}

Snake::Snake(int x, int y) {
	coord_t right = {1, 0};
	SnakeNode * head = new SnakeNode(x, y, right);
	snake_nodes.push_back(head);	
}

Snake::~Snake() {
	//iterate through vector, deleting each element
	std::vector<SnakeNode *>::iterator it = snake_nodes.begin();
	while (it != snake_nodes.end()) {
		SnakeNode * ptr = *it; 
		delete ptr;
		it++;
	}
}

SnakeNode * Snake::get_head() {
	return snake_nodes.front();
}

void Snake::update() {
	//iterate through, update xpos, ypos based on directions etc
	//save tail data, to set the new node if need to append.


	last_tail = snake_nodes.back()->clone();

	//for head, direction = arrow direction (last changed)
	//for else, direction = prev direction of next block

	std::vector<SnakeNode *>::iterator it = snake_nodes.begin();

	//head
	SnakeNode * head = *it;
	coord_t pos = head->get_coords();
	head->update();

	it++;
	coord_t prev_direction = head->get_direction();
	while (it != snake_nodes.end()) {
		SnakeNode * snode = *it;
		coord_t curr_direction = snode->get_direction();
		// std::cout << "Snake::update curr_dir = (" << curr_direction.x << ", " << curr_direction.y <<  ")" << std::endl; 
		snode->update();
		snode->set_direction(prev_direction);
		prev_direction = curr_direction;
		it++;
	}


}

void Snake::append() {
	// SnakeNode node = last_tail.clone();
	SnakeNode * node = new SnakeNode(last_tail);
	snake_nodes.push_back(node);
}

void Snake::enqueue_direction(const coord_t & direction) {
	return;
}

//functor for collision ops
struct collision {
	private:
		coord_t collidor; //MAY NEED TO PREINIT
	public:
		collision(const coord_t & collidor): collidor(collidor) {};
		bool operator () (SnakeNode * node) {
			return node->collides(collidor);
		}
};

bool Snake::has_collision() {
	//has a collision if any of the non-head blocks collide with the head coordinates
	coord_t head = get_head()->get_coords();
	//advance the beginning iterator by 1 to skip head node
	std::vector<SnakeNode *>::iterator begin_it = snake_nodes.begin();
	begin_it++;  
	std::vector<SnakeNode *>::iterator it = std::find_if(begin_it, snake_nodes.end(), collision(head));
	return it != snake_nodes.end();
}

bool Snake::contains(const coord_t & coords) {
	//returns true if one of the nodes' coords match the parameter
	std::vector<SnakeNode *>::iterator it = std::find_if(snake_nodes.begin(), snake_nodes.end(), collision(coords));
	return it != snake_nodes.end();
}

std::vector<coord_t> Snake::get_snake_coords() {
	std::vector<coord_t> snake_coords;
	snake_coords.reserve(snake_coords.size()); 
	
	std::vector<SnakeNode *>::iterator it = snake_nodes.begin();
	int i = 0;
	while (it != snake_nodes.end()) {
		SnakeNode * node_ptr = *it;
		snake_coords.push_back(node_ptr->get_coords());
		it++;
	}
	return snake_coords;
}

//SnakeNode func implementations

SnakeNode::SnakeNode(int x, int y) : x(x), y(y) {
	coord_t right = {1, 0};
	direction.x = right.x;
	direction.y = right.y;
}

SnakeNode::SnakeNode(const SnakeNode & copy) {
	x = copy.get_x();
	y = copy.get_y();
	coord_t dir = copy.get_direction();
	direction.x = dir.x;
	direction.y = dir.y;
}

SnakeNode SnakeNode::clone() {
	//SAME REF
	coord_t dir = { direction.x, direction.y };
	return SnakeNode(x, y, dir);
}

coord_t SnakeNode::get_coords() {
	coord_t coords = {x, y};
	return coords;
}

void SnakeNode::set_direction(coord_t direction_vector) {
	//error check

	// if queue.peek == vector 
		// return;

	if (std::abs(direction_vector.x) + std::abs(direction_vector.y) != 1) {
		std::cout << "(" << direction_vector.x << ", " << direction_vector.y <<  ")" << std::endl; 
		throw std::invalid_argument("direction vector must be of length 1 and corresponding to one of the cardinal directions");
	}

	direction = direction_vector;
	// direction.x = direction_vector.x;
	// direction.y = direction_vector.y;
	// std::cout << "SnakeNode::set_direction = (" << direction.x << ", " << direction.y <<  ")" << std::endl; 
}

void SnakeNode::update() {
	x += direction.x;
	y += direction.y;
}

bool SnakeNode::collides(const SnakeNode & other) const{
	return x == other.x && y == other.y;
}

bool SnakeNode::collides(const coord_t & coord) const {
	return x == coord.x && y == coord.y;
}

// Food func implementations
coord_t Food::get_coords() {
	coord_t coords = {x,y};
	return coords;
}

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
	return time <= time_expires && !eaten;
}

void Food::eat() {
	eaten = true;
}