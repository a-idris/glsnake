#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>
#include <ctime>
#include <time.h>

#include "game_logic.h"
#include "snake.h"


const int GRID_SIZE = 20; //default grid size
const float midW = GRID_SIZE / 2.0f, midH = GRID_SIZE / 2.0f;

//initialise game
Game game (GRID_SIZE);

//grid display list handle
unsigned int g_grid = 0; 

//only render if window is visible
bool render = true, paused = false;

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
	{0.05f, 0.4f, 0.05f, 1.0f},
	{0.0f, 0.6f, 0.0f, 1.0f}, 
	{0.6f, 0.8f, 0.6f, 1.0f},
	30.0f
};

//food material 
const material_t food_mat = {
	{0.3f, 0.2f, 0.2f, 1.0f},
	{0.6f, 0.15f, 0.15f, 1.0f}, 
	{0.8f, 0.6f, 0.6f, 1.0f},
	50.0f
};

//decls
void draw_border(int, int);
void init_lights();
void orthographic_vv();
void perspective_vv();
void set_material(const material_t &);

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (current_perspective == ORTHOGRAPHIC) {
		gluLookAt(midW, GRID_SIZE, -midH,
				  midW, 0, -midH,
				  0, 0, -1);		
	} else if (current_perspective == PERSPECTIVE) {
		gluLookAt(camera_xoffset, GRID_SIZE, camera_zoffset,
				  midW, 0, -midH,
				  0, 1, 0);
	}
	//if pov, camera pos = snake head, look direction = head + direction

	//disable lighting to draw grid
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glCallList(g_grid);
	glEnable(GL_LIGHTING);

	set_material(snake_mat);

	glPushMatrix();
	glScalef(1.0f, 1.0f, -1.0f); //reflect in xy plane ( since grid is in +x, -z quadrant)

	glTranslatef(0.5f, 0.5f, 0.5f); // translate cube so its near left corner is at 0,0,0 

	set_material(snake_mat);
	//draw snake blocks
	
	std::vector<coord_t> snake_blocks = game.get_snake_coords();
	std::vector<coord_t>::iterator it = snake_blocks.begin();
	while (it != snake_blocks.end()) {
		coord_t block = *it;

		// draw the x,y game coordinates in the xz plane using translation
		// std::cout << "cube " << i << std::endl;

		glPushMatrix();
		glTranslatef(block.x, 0.0f, block.y);;
		glutSolidCube(1);
		glPopMatrix();

		it++;
	}

	if (game.food_active()) {
		set_material(food_mat);
		glPushMatrix();
		coord_t food_pos = game.get_food();
		glTranslatef(food_pos.x, 0.0f, food_pos.y);
		glutSolidSphere(0.5, 32, 32);
		glPopMatrix();
	}

	glPopMatrix();

	//score and time
	float secs = (int) (total_time / 100.0f) / 10.0f;

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
	
	// draw_border(w,h);

	glEndList();

	return handle;
} 

void draw_border(int w, int h) {
	float border_height = 0.7;
	glBegin(GL_QUAD_STRIP);
	glVertex3f(0.0f, border_height, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(w, border_height, 0.0f);
	glVertex3f(w, 0.0f, 0.0f);
	glVertex3f(w, border_height, -h);
	glVertex3f(w, 0.0f, -h);
	glVertex3f(0.0f, border_height, -h);
	glVertex3f(0.0f, 0.0f, -h);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, border_height, 0.0f);
	glEnd();
}

void set_material(const material_t& mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
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

	//start the game
	game.start();
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
	gluPerspective(60.0f, 1.0f, GRID_SIZE * 0.4f, GRID_SIZE * 2.0f);
}

void init_lights() {
	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {0.5, 0.5, 0.5, 1.0};
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

//30, 60 fps. change x,ypos proportional to time since last.



// our idle handler
void idle()
{
	long timeMs = get_time();
	long time_elapsed = timeMs - last_time; //get time elapsed since last call
	last_time = timeMs;

	total_time = timeMs - start_time; //get total gameplay time to display on screen
	
	game.update(time_elapsed); 

	if (render) {
		glutPostRedisplay();
	} 
}


void visibility(int vis)
{
	std::cerr << "VIS EVENT" << std::endl;
	if (vis==GLUT_VISIBLE)
	{
		//start drawing
		render = true;
	}
	else
	{
		//stop drawing
		render = false;
	}
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
	std::cerr << "\t you pressed the " << key << " key" << std::endl;

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
	glutInitWindowSize(1012, 1012); 
	// glutInitWindowSize(512, 512); 
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
