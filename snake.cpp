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

//initialise game
Game game (GRID_SIZE);

//grid display list handle
unsigned int g_grid = 0; 

//only render if window is visible
bool render = true, paused = false, dead = false;

//camera controls
float camera_xoffset = midW;
float camera_zoffset = 0.0f; // change initial value = 0 + CONST
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

// const material_t snake_mat = { 
// 	{0.135f, 0.2225f, 0.1575f, 0.95f},
// 	{0.54f, 0.89f, 0.63f, 0.95f},
// 	{0.316228f, 0.316228f, 0.316228f, 0.95f},
// 	12.8
// };

// const material_t snake_mat ={{ 0.0f,0.05f,0.0f,1.0f },
// 	{ 0.4f,0.5f,0.4f,1.0f},
// 	{0.04f,0.7f,0.04f,1.0f },
// 	10.0f
// };

const material_t dead_snake_mat = {
	{0.35f, 0.7f, 0.35f, 1.0f},
	{0.5f, 0.95f, 0.5f, 1.0f}, 
	{0.7f, 0.9f, 0.7f, 1.0f},
	30.0f
};

//food material 
// const material_t food_mat = {
// 	{0.3f, 0.2f, 0.2f, 1.0f},
// 	{0.6f, 0.15f, 0.15f, 1.0f}, 
// 	{0.8f, 0.6f, 0.6f, 1.0f},
// 	50.0f
// };

const material_t food_mat = {
	{ 0.24725f, 0.1995f, 0.0745f, 1.0f },
	{0.75164f, 0.60648f, 0.22648f, 1.0f },
	{0.628281f, 0.555802f, 0.366065f, 1.0f },
	51.2f
};

//decls
void draw_hud();
void draw_death();
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
		gluLookAt(camera_xoffset, GRID_SIZE, camera_zoffset,
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
	if (dead) {
		draw_death();
	} else {
		draw_hud();
	}

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

		//position in top right
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();			

			glDisable(GL_LIGHTING);
			//enclosing box
			// glColor3f(0.8f, 0.8f, 0.8f);
			glColor3f(1.0f, 1.0f, 0.0f);

			int box_width = static_cast<int>(width * 0.3f);
			int box_height = static_cast<int>(height * 0.2f);

			glTranslatef(width - box_width, height - box_height, 0);

			glBegin(GL_LINE_LOOP);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(box_width, 0.0f, 0.0f);
				glVertex3f(box_width, box_height, 0.0f);
				glVertex3f(0.0f, box_height, 0.0f);
			glEnd();

			//score and time


			glPushMatrix();
				glColor3f(1.0f, 0.0f, 0.0f);


				float secs = (int) (total_time / 100.0f) / 10.0f;
				std::ostringstream convert_t;
				convert_t << secs;
				std::string secs_str (convert_t.str()); 
				std::string time = "time:" +secs_str + "s";
				draw_text(10.0f, box_height * 0.8f, time);

				std::ostringstream convert_s;
				convert_s << game.get_score();
				std::string score_str = convert_s.str(); 
				std::string score = "score: " + score_str; 
				draw_text(10.0f, box_height * 0.8f - 30.0f, score);

				std::string instructions [] = { 
					"arrow keys to move", 
					"spacebar to pause",
					"'wasd' to move camera",
					"'p' to cycle camera", 
					"'q' to quit"
				};
				int num_instructions = 5;

				for (int i = 0; i < num_instructions; i++) {
					draw_text(10.0f, box_height * 0.8f - (60.0f + 30*i), instructions[i]);
				}				

			glPopMatrix();


			glEnable(GL_LIGHTING);
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void draw_death() {
	//get viewport dimensions
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();

		gluOrtho2D(0, width, 0, height);

		//position in top right
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();			

			glDisable(GL_LIGHTING);
			//enclosing box
			// glColor3f(0.8f, 0.8f, 0.8f);
			glColor3f(1.0f, 1.0f, 0.0f);

			int box_width = static_cast<int>(width * 0.3f);
			int box_height = static_cast<int>(height * 0.2f);

			glTranslatef((width - box_width) / 2.0f, (height - box_height) / 2.0f, 0);

			glBegin(GL_POLYGON);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(box_width, 0.0f, 0.0f);
				glVertex3f(box_width, box_height, 0.0f);
				glVertex3f(0.0f, box_height, 0.0f);
			glEnd();

			//score and time


			glPushMatrix();
				glColor3f(1.0f, 0.0f, 0.0f);

				std::string death_text = "You're dead kiddo. Play again? (y/n)";
				draw_text(10.0f, box_height - 50.0f, death_text);

				// float secs = (int) (total_time / 100.0f) / 10.0f;
				// std::ostringstream convert_t;
				// convert_t << secs;
				// std::string secs_str (convert_t.str()); 
				// std::string time = "time:" +secs_str + "s";
				// draw_text(10.0f, box_height * 0.8f, time);

				// std::ostringstream convert_s;
				// convert_s << game.get_score();
				// std::string score_str = convert_s.str(); 
				// std::string score = "score: " + score_str; 
				// draw_text(10.0f, box_height * 0.8f - 30.0f, score);

				// std::string instructions [] = { 
				// 	"arrow keys to move", 
				// 	"spacebar to pause",
				// 	"'wasd' to move camera",
				// 	"'p' to cycle camera", 
				// 	"'q' to quit"
				// };
				// int num_instructions = 5;

				// for (int i = 0; i < num_instructions; i++) {
				// 	draw_text(10.0f, box_height * 0.8f - (60.0f + 30*i), instructions[i]);
				// }				

			glPopMatrix();


			glEnable(GL_LIGHTING);
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//adapted from lab5
void draw_text(const float x, const float y, const std::string & text)
{
	const float scale = 0.25f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(x, y, 0.1f);
		glScalef(scale, scale, 1.0f);
		for (size_t i = 0; i < text.length(); i++) {
   			glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
		}
	glPopMatrix();
}


long get_time() {
	timespec tp;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
	// std::cout << ctime(&(tp.tv_sec)) << std::endl;
	long milis = tp.tv_sec * 1000;
	milis += static_cast<long>(tp.tv_nsec / 1000000.0f);
	return milis; 
}

void init()
{

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

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
	glOrtho(-half_GRID_SIZE - 1, half_GRID_SIZE + 1, -half_GRID_SIZE - 1, half_GRID_SIZE + 1, GRID_SIZE - 2, GRID_SIZE + 2);
}

void perspective_vv() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0f, 0.01f, GRID_SIZE * 2.0f);
}

void init_lights() {
	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {1.0, 0.9, 0.9, 1.0};
	float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	// float light_ambient[] = {0.5, 0.5, 0.5, 1.0};
	// float light_diffuse[] = {0.0, 1.0, 0.0, 1.0};
	// float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
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
	
	dead = !game.update(time_elapsed); 

	if (dead) {
		glutIdleFunc(NULL);
	}

	if (render) {
		glutPostRedisplay();
	} 

}

void reset() {
	camera_xoffset = midW;
	camera_zoffset = 0.0f; // change initial value = 0 + CONST
	start_time = get_time();
	total_time = start_time;
	last_time = start_time;
}


void visibility(int vis)
{
	std::cerr << "VIS EVENT" << std::endl;
	//render iff window is visible
	render = (vis == GLUT_VISIBLE) ? true : false;
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	if (current_perspective == ORTHOGRAPHIC) {
		orthographic_vv();
	} else {
		perspective_vv();
	}
}

// will get which key was pressed and x and y positions if required
void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
		case 'q': 
			exit(1); // quit!

		//ADD BOUNDS TO CAMERA MOVEMENT E.G. MIN(MAX(0, CAMERA_XOFFSET), GRID_SIZE)

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
				perspective_vv();
			}
			break;
		case ' ':
			if (!dead) {
				if (!paused) {
					glutIdleFunc(NULL);
				} 
				else {
					//need to save time ellapsed etc. and update when resuming
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

	}

	glutPostRedisplay(); // force a redraw
}

// any special key pressed like arrow keys
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
	// glutInitWindowSize(1012, 1012); 
	glutInitWindowSize(512, 512); 
	glutInitWindowPosition(50, 50); 
	glutCreateWindow("Snek"); 
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
