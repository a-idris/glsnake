#include <cmath>
#include <stddef.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#include <unistd.h>

#include "game_logic.h"

//Game func implementations

Game::Game(int grid_size) : grid_size(grid_size), score(0), MIN_VELOCITY(1), MAX_VELOCITY(31) {
	// blocks / sec
	velocity = 10; 
	// amount by which velocity will be increased / decreased. set s.t. 10 difficulty levels
	difficulty_delta = (MAX_VELOCITY - MIN_VELOCITY) / 10; 

	total_time = 0;
	//time for a snake node to traverse one block = distance / velocity (converted to milliseconds). 
	block_time = static_cast<long>(1.0f / velocity * 1000); 
	//time until food is next set
	food_time = 1000;
	//keep track of time between block / food times to be able to update appropriately
	block_ongoing_time = 0;
	food_ongoing_time = 0;
	//seed the rng
	srand(time(NULL));
	//put snake in the middle of the grid initially
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); 
}

Game::~Game() {
	delete snake;
}

void Game::reset() {
	//free the memory of the old snake
	delete snake;

	score = 0;
	total_time = 0;
	block_ongoing_time = 0;
	food_ongoing_time = 0;
	food_time = 0;
	// food.eat(); 	
	int midpoint = static_cast<int>(grid_size / 2.0f);
	snake = new Snake(midpoint, midpoint); 
}

bool Game::update(long time_elapsed) {
	total_time += time_elapsed;
	block_ongoing_time += time_elapsed;
	food_ongoing_time += time_elapsed;

	if (block_ongoing_time >= block_time) {
		//threshold has passed, reset
		block_ongoing_time -= block_time;

		//move snake by 1 square
		snake->update(); 
		
		std::vector<coord_t> snake_coords = snake->get_snake_coords();
		coord_t head = snake->get_head()->get_coords();

		//check snake collision w/ itself or if the head is out of bounds. only need to check head because other blocks are old positions of head 
		if (snake->has_collision() || out_of_bounds(head)) {
			//return alive=false
			return false;
		}

		if (food.is_active(total_time)) {
			coord_t food_pos = food.get_coords();
			if (head == food_pos) {
				// points given proportional to the time taken to eat the food
				int points = food.eat(total_time); 
				score += points;
				//add another SnakeNode to the end of the snake
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

			//choose when next to respawn the food as the expiry date of the active food + some random amount
			int rand_amount = (rand() % 2500) + 1000;
			food_ongoing_time = 0;  
			food_time = time_active + rand_amount;
		}
	}

	return true;
}	

SnakeNode Game::get_head() {
	return *(snake->get_head());
}

std::vector<coord_t> Game::get_snake_coords() {
	return snake->get_snake_coords();
}

coord_t Game::get_food() {
	return food.get_coords();
}

bool Game::food_active() {
	//need to pass the time for food to decide 
	return food.is_active(total_time);

}

int Game::get_difficulty() {
	return (velocity - MIN_VELOCITY) / difficulty_delta;
}

int Game::increase_difficulty() {
	velocity = std::min(velocity + difficulty_delta, MAX_VELOCITY);
	block_time = static_cast<long>(1.0f / velocity * 1000);
	return velocity;
}

int Game::decrease_difficulty() {
	velocity = std::max(velocity - difficulty_delta, MIN_VELOCITY);
	block_time = static_cast<long>(1.0f / velocity * 1000);
	return velocity;
}

void Game::change_direction(const coord_t & direction) {
	//delegate to snake
	snake->enqueue_direction(direction);
}

//check if coordinate is out of bounds of grid
bool Game::out_of_bounds(const coord_t & coord) {
	return coord.x < 0 || coord.x >= grid_size || coord.y < 0 || coord.y >= grid_size;
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
	//iterate through vector, free each element
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
	//iterate through, updating each SnakeNode

	//for head, direction = retrieve and process input direction received from user
	//for the other nodes, set direction to be the prev direction of the next block

	std::vector<SnakeNode *>::iterator it = snake_nodes.begin();

	SnakeNode * head = *it;

	//process input direction. if input queue is empty, direction stays the same 
	if (!directions.empty()) {
		coord_t direction = directions.front();
		directions.pop();
		coord_t curr_direction = head->get_direction();
		//prevent head from turning inwards into its body
		if (snake_nodes.size() > 2 && direction.x + curr_direction.x == 0 && direction.y + curr_direction.y == 0) {
			//if the direction is in the opposite direction of current direction (e.g. {1,0} and {-1,0}), skip it
			//next direction guaranteed to be different by the enqueue_direction func
			if (!directions.empty()) {
				direction = directions.front();
				directions.pop();
				head->set_direction(direction);
			}
		} else {
			head->set_direction(direction);
		}
	}

	//save tail details. in case need to append, will use these details. need to save after updating head direction in case head = tail
	last_tail = snake_nodes.back()->clone();

	head->update();
	it++;
	
	//after each node is updated, set its direction to the direction of the previous node so that it follows in the same path
	coord_t prev_direction = head->get_direction();
	while (it != snake_nodes.end()) {
		SnakeNode * snode = *it;
		coord_t curr_direction = snode->get_direction();
		snode->update();
		snode->set_direction(prev_direction);
		prev_direction = curr_direction;
		it++;
	}
}

void Snake::append() {
	SnakeNode * node = new SnakeNode(last_tail);
	snake_nodes.push_back(node);
}

void Snake::enqueue_direction(const coord_t & direction) {
	if (!directions.empty()) {
		coord_t last_dir = directions.back();
		//don't duplicate directions - not meaningful info
		if (last_dir == direction)
			return;
	}
	directions.push(direction);
}

//functor for collision ops
struct collision {
	private:
		coord_t collidor; 
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
	//check if any of the body nodes collide with the head coords  
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
	direction = copy.get_direction();
}

SnakeNode SnakeNode::clone() {
	return SnakeNode(x, y, direction);
}

coord_t SnakeNode::get_coords() {
	coord_t coords = {x, y};
	return coords;
}

void SnakeNode::set_direction(const coord_t & direction_vector) {

	if (std::abs(direction_vector.x) + std::abs(direction_vector.y) != 1) {
		std::cerr << "(" << direction_vector.x << ", " << direction_vector.y <<  ")" << std::endl; 
		throw std::invalid_argument("direction must be unit vector");
	}
	direction = direction_vector;
}

void SnakeNode::update() {
	//advance the node by 1 in the stored direction
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
	
	//time active: from 3-7 seconds
	long time_active = (rand() % 4000) + 3000;
	time_set = curr_time; 
	time_expires = curr_time + time_active;

	return time_active;
}

int Food::eat(long time) {
	//eating deactivates the food
	eaten = true;
	float proportion = float(time - time_set) / (time_expires - time_set);  
	//range 5-25, the faster it's eaten the higher the score
	float additional = 20 - static_cast<int>((proportion * 20) + 0.5);
	return 5 + additional;
}

bool Food::is_active(long time) {
	return time <= time_expires && !eaten;
}