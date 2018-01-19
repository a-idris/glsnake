#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>

#include "game_logic.h"
#include "snake.h"


const int GRID_SIZE = 20; //default grid size
const float midW = GRID_SIZE / 2.0f, midH = GRID_SIZE / 2.0f;
const float INIT_VIEWPORT_WIDTH = 512.0f;
const float INIT_VIEWPORT_HEIGHT = 652.0f;

float text_scale = 1.0f; //scale factor for drawing text 

//initialise game
Game game (GRID_SIZE);

//grid display list handle
unsigned int g_grid = 0; 

//only render if window is visible
bool render = true, paused = false, dead = false, instructions = false;

//camera controls
float camera_xoffset = midW;
float camera_zoffset = -2.5f; // change initial value = 0 + CONST
float camera_delta = 0.5f;

//time vars
long start_time, total_time, last_time;

//initial viewing perspective 
perspective_t current_perspective = PERSPECTIVE;

//snake material
const material_t snake_mat = {
	{0.55f, 0.86f, 0.55f, 1.0f},
	{0.63f, 0.92f, 0.43f, 1.0f}, 
	{1.0f, 1.0f, 1.0f, 1.0f},
	30.0f
};

const material_t dead_snake_mat = {
	{0.85f, 1.0f, 0.85f, 1.0f},
	{0.83f, 1.0f, 0.83f, 1.0f}, 
	{1.0f, 1.0f, 1.0f, 1.0f},
	30.0f
};

//food material
const material_t food_mat = {
	{ 0.18f, 0.01f, 0.01f, 0.55f },
	{0.61f, 0.04f, 0.04f, 0.55f },
	{0.73f, 0.63f, 0.63f, 0.55f },
	77.0f
};


void draw_hud();
void draw_text(const float, const float, const std::string &);
void init_lights();
void orthographic_vv();
void perspective_vv();
void set_material(const material_t &);


void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//set camera depending on current perspective
	// grid is in xz plane in +x, -z quadrant)
	if (current_perspective == ORTHOGRAPHIC) {
		gluLookAt(midW, GRID_SIZE, -midH,
				  midW, 0, -midH,
				  0, 0, -1);		
	} else if (current_perspective == PERSPECTIVE) {
		gluLookAt(camera_xoffset, GRID_SIZE * 1.2f, camera_zoffset,
				  midW, 0, -midH,
				  0, 1, 0);
	} else {
		//POV
		SnakeNode head = game.get_head();
		//adjust head coords so starting in the middle of the block		
		float head_x = head.get_x() + 0.5f;
		float head_y = head.get_y() + 0.5f;

		coord_t heading_direction = head.get_direction();
		float dir_x = heading_direction.x * 0.5f;
		float dir_y = heading_direction.y * 0.5f;

		//put camera on 'edge' of head, looking in the direction of the heading direction
		gluLookAt(head_x + dir_x, 0.5, -(head_y + dir_y) ,
				  head_x + 2 * dir_x, 0.5, -(head_y + 2 * dir_y),
				  0, 1, 0);
	}

	//disable lighting to draw grid
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glCallList(g_grid);
	glEnable(GL_LIGHTING);


	glPushMatrix();
		glScalef(1.0f, 1.0f, -1.0f); //reflect in xy plane ( since grid is in +x, -z quadrant)
		glTranslatef(0.5f, 0.5f, 0.5f); // translate cube so its near left corner is at 0,0,0 

		//draw snake blocks

		if (dead) {
			set_material(dead_snake_mat);
		} else {
			set_material(snake_mat);
		}		

		std::vector<coord_t> snake_blocks = game.get_snake_coords();
		std::vector<coord_t>::iterator it = snake_blocks.begin();
		while (it != snake_blocks.end()) {
			coord_t block = *it;
			// draw the x,y game coordinates in the xz plane using translation
			glPushMatrix();
			glTranslatef(block.x, 0.0f, block.y);;
			glutSolidCube(1);
			glPopMatrix();

			it++;
		}

		//draw food if it is available
		if (game.food_active()) {
			set_material(food_mat);
			glPushMatrix();
			coord_t food_pos = game.get_food();
			glTranslatef(food_pos.x, 0.0f, food_pos.y);
			glutSolidSphere(0.5, 32, 32);
			glPopMatrix();
		}

	glPopMatrix();

	//draw game info overlay 
	draw_hud();
	
	glutSwapBuffers(); 

}


unsigned int make_grid(int w, int h) {

	unsigned int handle = glGenLists(1);

	glNewList(handle, GL_COMPILE);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
	for (size_t columns = 0; columns <= w; columns++) {
		glVertex3f(columns, 0.0f, 0.0f);		
		glVertex3f(columns, 0.0f, -h);		
	}

	for (int rows = 0; rows <= h; rows++) {
		glVertex3f(0.0f, 0.0f, -rows);		
		glVertex3f(w, 0.0f, -rows);		
	}
	glEnd();
	
	glEndList();

	return handle;
} 


void set_material(const material_t& mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
}

void draw_hud() {
	//get viewport dimensions
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();

		gluOrtho2D(0, width, 0, height);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();			

			glDisable(GL_LIGHTING);
			//enclosing container
			int container1_width = static_cast<int>(width * 0.2f);
			int container1_height = static_cast<int>(height * 0.15f);

			//position first container in top left
			glTranslatef(0, height - container1_height, 0);

			glPushMatrix();
				glColor3f(0.6f, 0.0f, 0.2f);

				//time
				float secs = (int) (total_time / 100.0f) / 10.0f;
				std::ostringstream convert_t;
				convert_t << secs;
				std::string secs_str (convert_t.str()); 
				std::string time = "time: " +secs_str + "s";
				draw_text(20.0f, container1_height * 0.7f, time); 

				//score
				std::ostringstream convert_s;
				convert_s << game.get_score();
				std::string score_str = convert_s.str(); 
				std::string score = "score: " + score_str; 
				draw_text(20.0f, container1_height * 0.7f - 30.0f * text_scale, score);

				//difficulty
				std::ostringstream convert_d;
				convert_d << game.get_difficulty();
				std::string difficulty_str = convert_d.str(); 
				std::string difficulty = "difficulty: " + difficulty_str; 
				draw_text(20.0f, container1_height * 0.7f - 60.0f * text_scale, difficulty);

				//draw instructions
				int container2_width = static_cast<int>(width * 0.50f);
				int container2_height = static_cast<int>(height * 0.15f);
				glTranslatef(width - container2_width, 0.0f, 0.0f);

/*				glBegin(GL_LINE_LOOP);
					glVertex3f(0.0f, 0.0f, 0.0f);
					glVertex3f(container2_width, 0.0f, 0.0f);
					glVertex3f(container2_width, container2_height, 0.0f);
					glVertex3f(0.0f, container2_height, 0.0f);
				glEnd();*/
				if (!dead) {
					if (!instructions) {
						std::string instruction_str = "i to toggle instructions";
						draw_text(0.0f, container2_height * 0.7, instruction_str); 
					} else {
						std::string instructions [] = { 
							"arrow keys to move", 
							"spacebar to pause",
							"'v/V' to change velocity",
							"'wasd' to move camera",
							"'p' to cycle camera", 
							"'q' to quit"
						};
						int num_instructions = 6;

						for (int i = 0; i < num_instructions; i++) {
							//gaps between lines also scale
							draw_text(10.0f, container2_height * 0.8f - (20.0f * text_scale * i), instructions[i]);
						}				
					}
				} else {
					std::string death_text1 = "You died.";
					std::string death_text2 = "Play again? (y/n)";
					draw_text(0.0f, container2_height * 0.7, death_text1); 
					draw_text(0.0f, container2_height * 0.7 - 30.0f * text_scale, death_text2); 
				}	

			glPopMatrix();
			glEnable(GL_LIGHTING);
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void draw_text(const float x, const float y, const std::string & text)
{
	float scale = 0.15f * text_scale; //scale by text_scale (dependent on viewport dimensions)

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(x, y, 0.1f);
		glScalef(scale, scale, 1.0f);
		glLineWidth(1.5f);
		for (size_t i = 0; i < text.length(); i++) {
   			glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
		}
		glLineWidth(1.0f);
	glPopMatrix();
}

long get_time() {
	//return time in millis. 
	timespec tp;
	//CLOCK_MONOTONIC_RAW doesn't return wall time but not necessary
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp); 
	long milis = tp.tv_sec * 1000;
	milis += static_cast<long>(tp.tv_nsec / 1000000.0f);
	return milis; 
}

void init()
{
	glClearColor(0.027f, 0.125f, 0.277f, 0.0f); 

	// make grid display list
	g_grid = make_grid(GRID_SIZE, GRID_SIZE);

	//initialise in perspective view mode
	perspective_vv();
	init_lights();
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	//start time
	start_time = get_time();
	total_time = start_time;
	last_time = start_time;
}

void orthographic_vv() {
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	float half_GRID_SIZE = GRID_SIZE / 2.0f;
	//enough room for grid and game info
	glOrtho(-half_GRID_SIZE - 1, half_GRID_SIZE + 1, -half_GRID_SIZE - 1, half_GRID_SIZE + 5, GRID_SIZE - 2, GRID_SIZE + 2);
}

void perspective_vv() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//aspect ratio ~5:4
	gluPerspective(60.0f, 0.8f, 0.01f, GRID_SIZE * 2.0f);
}

void init_lights() {
	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {1.0, 0.9, 0.9, 1.0};
	float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	// fix the light above the centre of the grid
	float light_position[] = {0.0, GRID_SIZE * 0.9, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// enable lighting and turn on the light0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void idle()
{
	long timeMs = get_time();
	long time_elapsed = timeMs - last_time; //get time elapsed since last call
	last_time = timeMs;

	total_time = timeMs - start_time; //get total gameplay time to display on screen
	
	//game advances its state by the time_elapsed, and returns alive state of snake
	dead = !game.update(time_elapsed); 

	//stop updating game state if dead
	if (dead) {
		glutIdleFunc(NULL);
	}

	if (render) {
		glutPostRedisplay();
	} 

}

void reset() {
	//reset camera controls and time
	camera_xoffset = midW;
	camera_zoffset = 0.0f; 
	start_time = get_time();
	total_time = start_time;
	last_time = start_time;
}


void visibility(int vis)
{
	//render iff window is visible
	render = (vis == GLUT_VISIBLE) ? true : false;
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	if (current_perspective == ORTHOGRAPHIC) {
		orthographic_vv();
	} else {
		//for PERSPECTIVE and POV
		perspective_vv();
	}

	//scale text size
	float w_ratio = w / INIT_VIEWPORT_WIDTH;
	float h_ratio = h / INIT_VIEWPORT_HEIGHT;
	text_scale = (w_ratio + h_ratio) / 2;
}

void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': 
			exit(1); // quit!
		case 'w': 
			camera_zoffset -= camera_delta; break;
		case 'a':
			camera_xoffset -= camera_delta; break;
		case 's': 
			camera_zoffset += camera_delta; break;
		case 'd':
			camera_xoffset += camera_delta; break;
		case 'p':
			//cycle through perspectives 
			current_perspective = static_cast<perspective_t>((current_perspective + 1) % PERSPECTIVES_COUNT);
			if (current_perspective == ORTHOGRAPHIC) {
				orthographic_vv();
			} else {
				//for PERSPECTIVE and POV
				perspective_vv();
			}
			break;
		case ' ':
			//pause, if alive
			if (!dead) {
				if (!paused) {
					glutIdleFunc(NULL);
				} 
				else {
					//update time correctly
					long curr_time = get_time();
					long gap = curr_time - last_time;
					last_time = curr_time;
					start_time += gap;
					glutIdleFunc(idle);
				}
				paused = !paused;
			}
			break;
		case 'y':
			if (dead) {
				//reset time and other vars
				reset();
				game.reset();
				//reinstall the idle func
				glutIdleFunc(idle);
			}
			break;
		case 'n':
			if (dead) {
				exit(1);
			}
			break;
		case 'v':
			game.decrease_difficulty();
			break;
		case 'V':
			game.increase_difficulty();
			break;
		case 'i':
			instructions = !instructions;
	}

	glutPostRedisplay(); // force a redraw
}


void special(int key, int, int)
{
	//if the game is paused don't accept input to change direction
	if (paused) {
		return;
	}
	// handle special keys
	coord_t direction = { 0, 0 };
	switch (key)
	{
		case GLUT_KEY_LEFT: 
			direction.x = -1;
			game.change_direction(direction); 
			break;
		case GLUT_KEY_RIGHT: 
			direction.x = 1;
			game.change_direction(direction); 
			break;
		case GLUT_KEY_UP: 
			direction.y = 1;
			game.change_direction(direction); 
			break;
		case GLUT_KEY_DOWN: 
			direction.y = -1;
			game.change_direction(direction); 
			break; 
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH); 
	glutInitWindowSize(INIT_VIEWPORT_WIDTH * 2, INIT_VIEWPORT_HEIGHT * 2); 
	glutInitWindowPosition(50, 50); 
	glutCreateWindow("Snake"); 

	glutDisplayFunc(display); 
	glutKeyboardFunc(keyboard); 
	glutSpecialFunc(special); 
	glutVisibilityFunc(visibility); 
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	init(); 
	glutMainLoop(); 

	return 0; 
}
