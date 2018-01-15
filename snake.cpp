#ifdef __APPLE__
#include <GLUT/glut.h> 
#else
#include <GL/glut.h> 
#endif

#include <stddef.h>
#include <iostream>

//turn on unseen geometry so that the unseen polygons of the snake aren't drawn

//snake block
//for head, direction = arrow direction (last changed)
//for else, direction = prev direction of next block


/*game: grid, snake.

*/

//decls:
void draw_border(int, int);
void orthographic_vv();
void perspective_vv();

const int grid_size = 20;

unsigned int g_grid = 0; 

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// gluLookAt(0, 3, 0,
	// 		  0, 0, 0,
	// 		  0, 0, -1);

	gluLookAt(0, grid_size, grid_size / 2.0f,
			  0, 0, 0,
			  0, 1, 0);

	//animate game state. vars for xpos, ypos, etc.
	glCallList(g_grid);

	glutSolidCube(1);

	glutSwapBuffers(); 
}


unsigned int make_grid(int w, int h) {


	//translate -midW, midH

	float midW = w / 2.0f;
	float midH = h / 2.0f;

	unsigned int handle = glGenLists(1);

	glNewList(handle, GL_COMPILE);
	// glMatrixMode(GL_MODELVIEW);
	// glColor3f(1.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
	for (size_t columns = 0; columns <= w; columns++) {
		glVertex3f(columns - midW, 0.0f, -midH);		
		glVertex3f(columns - midW, 0.0f, midH);		
	}
	// glColor3f(0.0f, 0.0f, 1.0f);

	for (size_t rows = 0; rows <= h; rows++) {
		glVertex3f(-midW, 0.0f, rows - midH);		
		glVertex3f(midW, 0.0f, rows - midH);		
	}
	glEnd();
	
	draw_border(w,h);

	glEndList();

	return handle;
} 

void draw_border(int w, int h) {

}

void draw_cube() {

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
}

void orthographic_vv() {
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	float half_grid_size = grid_size / 2.0f;
	glOrtho(-half_grid_size - 1, half_grid_size + 1, -half_grid_size - 1, half_grid_size + 1, 3, 4);
}

void perspective_vv() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0f, grid_size * 0.4f, grid_size * 2.0f);
}

// our idle handler
void idle()
{
	//static int count = 0;
	//std::cerr << "\t idle handler called..." << ++count << std::endl;

	//time handling?

	glutPostRedisplay(); // uncomment if you change any drawing state
}

void visibility(int vis)
{
	std::cerr << "VIS EVENT" << std::endl;
	if (vis==GLUT_VISIBLE)
	{
		//start drawing
	}
	else
	{
		//stop drawing
	}
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	std::cerr << "reshape" << std::endl;
	perspective_vv();
}

// will get which key was pressed and x and y positions if required
void keyboard(unsigned char key, int, int)
{
	std::cerr << "\t you pressed the " << key << " key" << std::endl;

	switch (key)
	{
		case 'q': exit(1); // quit!

		//WASD for camera movement?

		case 'w': 
			// vector = (0, 1); break;
		case 'a':
			// vector = (-1, 0); break;
		case 's': 
			// vector = (0, -1); break;
		case 'd':
			// vector = (1, 0); break;
		case 'p':
			//switch perspective (change camera vars)
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



