#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>


//snake block
//for head, direction = arrow direction (last changed)
//for else, direction = prev direction of next block

/*class Snake {
private:
	SnakeBlock head;
	int length, score;
}*/

// class SnakeBlock {
	
// 	public:
// 		int xpos, ypos;
// 		//vector direction;
// 		SnakeBlock getNextSnakeBlock();
// }

// "SCHEDULE TURN"

/*game: grid, snake.

*/

//decls:
void draw_border(int, int);
void init_lights();
void orthographic_vv();
void perspective_vv();

const int grid_size = 20;
const float midW = grid_size / 2.0f, midH = grid_size / 2.0f;

unsigned int g_grid = 0;

float camera_delta = 0.5f;
float camera_xoffset = midW;
float camera_zoffset = 0.0f; // change initial value = 0 + CONST

enum perspective_t {ORTHOGRAPHIC=0, PERSPECTIVE=1, PERSPECTIVES_COUNT = 2};
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
		gluLookAt(midW, 3, -midH,
				  midW, 0, -midH,
				  0, 0, -1);		
	} else if (current_perspective == PERSPECTIVE) {
		gluLookAt(camera_xoffset, grid_size, camera_zoffset,
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
	glTranslatef(0.5f, 0.5f, -0.5f);


	for (size_t rows = 0; rows <= grid_size; rows++) {
		glPushMatrix();
		glTranslatef(rows / 5.0f, 0.0f, -rows);
		glutSolidCube(1);		
		glPopMatrix();
		glPushMatrix();
		glTranslatef(rows / 5.0f, 0.0f, -rows);		
		glutSolidCube(1);		
		glPopMatrix();
	}

	// glutSolidCube(1);

	glPushMatrix();
	glTranslatef(9.0f, 0.0f, -9.0f);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.0f, 0.0f, 1.0f);
	// glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(19.0f, 0.0f, -19.0f);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(19.0f, 0.0f, -9.0f);
	glutSolidCube(1);
	glPopMatrix();


	glPopMatrix();

	//for each snake block, animate (translate by x,y)
	//animate food, if applicable
	//if (food.up)
	//	food.x, food.y 

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

	for (size_t rows = 0; rows <= h; rows++) {
		glVertex3f(0.0f, 0.0f, -rows);		
		glVertex3f(w, 0.0f, -rows);		
	}
	glEnd();
	
	draw_border(w,h);

	glEndList();

	return handle;
} 

void draw_border(int w, int h) {

}


bool check_alive() {
	//collided with itself
	//collided with wall
	return true;
}


void init()
{

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 

	// make triangle display list
	g_grid = make_grid(grid_size, grid_size);

	perspective_vv();
	init_lights();
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
}

void orthographic_vv() {
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	float half_grid_size = grid_size / 2.0f;
	glOrtho(-half_grid_size - 1, half_grid_size + 1, -half_grid_size - 1, half_grid_size + 1, 1.5, 4);
}

void perspective_vv() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, grid_size * 0.1f, grid_size * 10.0f);
}

void init_lights() {
	float light_ambient[] = {0.1, 0.1, 0.1, 1.0};
	float light_diffuse[] = {0.5, 0.5, 0.5, 1.0};
	float light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	// fix the light above the centre of the grid
	float light_position[] = {0.0, grid_size * 0.9, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// enable lighting and turn on the light0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

//30, 60 fps. change x,ypos proportional to time since last.

// time_t last_time;

// our idle handler
void idle()
{
	// time_passed = time - last_time;
	//change PROPORTIONALLY 

	// update(time_passed);

	// if (render) {
	// 	glutPostRedisplay();
	// }
}

void update(int time_incr) {

}

void visibility(int vis)
{
	std::cerr << "VIS EVENT" << std::endl;
	if (vis==GLUT_VISIBLE)
	{
		//start drawing
		//render = true
	}
	else
	{
		//stop drawing
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
		case 'q': exit(1); // quit!

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
	switch (key)
	{
		case GLUT_KEY_LEFT: 
			// vector = (-1, 0); break;
		case GLUT_KEY_RIGHT: 
			// vector = (1, 0); break;
		case GLUT_KEY_UP: 
			// vector = (0, 1); break;
		case GLUT_KEY_DOWN: 
			// vector = (0, -1); 
		break;
	}

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



