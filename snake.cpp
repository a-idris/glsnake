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

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); 

	//animate game state. vars for xpos, ypos, etc.

	glutSwapBuffers(); 
}


void draw_grid(int x, int y) {


	

	draw_border(x,y);
} 

void draw_border(int x, y) {

}


bool check_alive() {
	//collided with itself
	//collided with wall
}


void init()
{
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho(0, 1000, 0, 1000);
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f); 

	// make triangle display list
	g_the_triangle = make_triangle();
}

// will get which key was pressed and x and y positions if required
void keyboard(unsigned char key, int, int)
{
	std::cerr << "\t you pressed the " << key << " key" << std::endl;

	switch (key)
	{
		case 'q': exit(1); // quit!
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
	}

	glutPostRedisplay(); // force a redraw
}

/*// any special key pressed like arrow keys
void special(int key, int, int)
{
	// handle special keys
	switch (key)
	{
		case GLUT_KEY_LEFT: g_xoffset -= g_offset_step; break;
		case GLUT_KEY_RIGHT: g_xoffset += g_offset_step; break;
		case GLUT_KEY_UP: g_yoffset += g_offset_step; break;
		case GLUT_KEY_DOWN: g_yoffset -= g_offset_step; break;
	}

	glutPostRedisplay(); // force a redraw
}*/


int main(int argc, char* argv[])
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA); 
	glutInitWindowSize(512, 512); 
	glutInitWindowPosition(50, 50); 
	glutCreateWindow("Snake"); 
	glutDisplayFunc(display); 

	// handlers for keyboard input
	glutKeyboardFunc(keyboard); 
	// glutSpecialFunc(special); 

	// visibility
	glutVisibilityFunc(visibility); 

	init(); 
	glutMainLoop(); 

	return 0; 
}



