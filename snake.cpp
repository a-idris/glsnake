#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>
#include <ctime>

//decls:
void draw_border(int, int);
void init_lights();
void orthographic_vv();
void perspective_vv();


const int GRID_SIZE = 20; //default grid size
const float midW = GRID_SIZE / 2.0f, midH = GRID_SIZE / 2.0f;

Game game (GRID_SIZE);

unsigned int g_grid = 0; //grid display list handle

bool render = true;

float camera_delta = 0.5f;
float camera_xoffset = midW;
float camera_zoffset = 0.0f; // change initial value = 0 + CONST

//time vars
clock_t start_time, total_time, time;

enum perspective_t { ORTHOGRAPHIC=0, PERSPECTIVE=1, PERSPECTIVES_COUNT=2 };
perspective_t current_perspective = PERSPECTIVE;

struct material_t {
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
};

//snake material
const material_t snake_mat = {
	{0.05f, 0.4f, 0.05f, 1.0f},
	{0.0f, 0.6f, 0.0f, 1.0f}, 
	{0.6f, 0.8f, 0.6f, 1.0f},
	30.0f
};

//food material 

void set_material(const material_t& mat) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
}

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
	glScalef(1.0f, 1.0f, -1.0f); //reflect in xy plane

	glTranslatef(0.5f, 0.5f, 0.5f); // translate cube so its left corner is at 0,0,0 

	Snake * snake = game::get_snake();

	for (int i = 0, SnakeNode * node = snake->get_head(); i < snake->get_length(); i++, node = node->get_next()) {
		glPushMatrix();
		glTranslatef(node.get_x(), 0.0f, node.get_y());
		glutSolidCube(1);
		glPopMatrix();
	}

	glPopMatrix();

	//for each snake block, animate (translate by x,y)
	//animate food, if applicable
	//if (food.up)
	//	food.x, food.y 

	glutSwapBuffers(); 

	//score and time
	float time = static_cast<int>(total_time / CLOCKS_PER_SEC * 10) / 10.0f; //convert total time to seconds w/ 1 d.p.precision !!!CLOCK / CLOCKS_PER_SEC YIELDS SECS 
	std::cout << time << std::endl;
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
	start_time = clock();
	total_time = start_time;
	time = start_time;

	//start the game
	game::start(time);
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

	time = clock() - time; //get time elapsed since last call
	total_time = clock() - start_time; //get total gameplay time to display on screen
	//change PROPORTIONALLY

	//maybe encode fps bound. while (time_passed > bound) time_passed -= bound; update();
	game::update(time); 

	if (render) {
		glutPostRedisplay();
	} else {
		//sleep(1); ? //sleep max fps bound time diff ^^^
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
	std::cerr << "reshape" << std::endl;
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
			dispose();
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
	}

	glutPostRedisplay(); // force a redraw
}

// any special key pressed like arrow keys
void special(int key, int, int)
{
	// handle special keys
	vector_t direction;
	switch (key)
	{
		case GLUT_KEY_LEFT: 
			direction = { -1, 0 };
			game::change_direction(direction); 
			break;
		case GLUT_KEY_RIGHT: 
			direction = { 1, 0 };
			game::change_direction(direction); 
			break;
		case GLUT_KEY_UP: 
			direction = { 0, 1 };
			game::change_direction(direction); 
			break;
		case GLUT_KEY_DOWN: 
			direction = { 0, -1 };
			game::change_direction(direction); 
			break; 
	}
	//if (direction) game::change_direction(direction);

	glutPostRedisplay(); // force a redraw
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH); 
	glutInitWindowSize(512, 512); 
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
