// Author: Talal Malas
// Version 1.0.0.1

//******************************** Header Files Included **************************************//

#include <windows.h>
#include <gl\glut.h>
#include <gl\glui.h>
#include <math.h>												// needed for trigonometric functions (sin, cos, tan)

//*********************************** Global Variables ****************************************//

char *helptext = "Solar System 1.0 is an OpenGL application that implements a pseudo-solar system by applying transformations (translations and rotations) on object matrices. GLUI is used to provide a friendlier user interface and some effects such as lighting are used. The interface is pretty self explanatory and for fine-tuning, you can use the keyboard to manipulate views and the mouse drag function for custom translations.";
char *keybtext = "The keyboard keys are used are\n   Translation in x-direction: q & w\n   Translation in y-direction: a & s\n   Translation in z-direction: z & x\n   Rotation in x-direction: e & r\n   Rotation in y-direction: d & f\n   Rotation in z-direction: c & v.\n\n Thank you for using Solar System 1.0. -Talal Malas";

// GLUI Control Variables
#define ENABLE_ID		300
#define DISABLE_ID		301
#define SHOW_ID			302
#define HIDE_ID			303
#define RADIOGROUP_ID	304
#define ABOUT_ID		305
#define HELP_ID			306

// Planet Color definitions (with alpha)
typedef GLfloat planetcolor[4];
planetcolor color[] = {	{1.0, 1.0, 0.0,1.0}, {1.0,0.0,0.0,1.0}, {1.0,0.5,0.5 ,1.0},
						{0.0, 0.0, 1.0,1.0}, {1.0,0.0,0.0,1.0}, {0.9,0.7,0.15,1.0},
						{0.85,0.96,0.1,1.0}, {0.5,0.0,1.0,1.0}, {0.0,0.8,0.5 ,1.0},
						{0.5 ,0.5 ,1.0,1.0 },{1.0,1.0,1.0,1.0}, {0.5,0.0,1.0 ,0.0} };

// Planet Color definitions (without alpha)
typedef GLfloat dcolor[3];
dcolor vcolor[] = {	{1.0 , 1.0 , 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.5, 0.5 },
					{0.0 , 0.0 , 1.0}, {1.0, 1.0, 1.0}, {0.9, 0.7, 0.15},
					{0.85, 0.96, 0.1}, {0.5, 0.0, 1.0}, {0.0, 0.8, 0.5 },
					{0.5 , 0.5 , 1.0}, {1.0, 1.0, 1.0}};

// Planet Information
float radius[] =	{ 0.500, 0.010, 0.024, 0.026, 0.014, 0.286,  0.241,  0.102,  0.099,  0.005 }; // Planet's radius (1 * 10^9 m)
float distance[] =	{ 0.000, 0.579, 1.082, 1.496, 2.279, 7.784, 14.294, 28.750, 45.043, 59.001 }; // Distance from the sun (100 * 10^9 m)
float rotate[] =	{ 30.50, 58.65, 243.0, 0.997, 1.026, 9.800, 10.200, 17.900, 19.100,  6.390 }; // Rotation of planet around its axis (in earth days)
float revolve[] =	{ 0.100, 0.241, 0.615, 1.000, 1.881, 11.86, 29.460, 84.009, 164.01, 247.70 }; // Revolution of planet around the sun (in earth years)
float oblique[] =	{ 0.000, 0.000, 178.0, 23.40, 25.00, 3.080, 26.070, 97.900, 29.600, 122.50 }; // Obliquity ~ tilt of axis of rotation (in degrees)
float incline[] =	{ 0.000, 7.000, 3.390, 0.000, 1.850, 1.300,  2.490,  0.770,  1.770, 17.150 }; // Orbit Inclination (in degrees)
char *planet_list[] = { "Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto" };

// Viewport variables for panning, zooming, etc..
GLdouble aspect = 0.0;											// Aspect Ratio of viewport
float zoom_x = 0.0, zoom_y = 0.0, zoom_z = -2.5;
float rot_x = -90, rot_y = 0.0, rot_z = 0.0, zRot = 0.1;
float lightposx = 0.0, lightposy = 0.0, lightposz = 1.0;
float xstart, ystart;
float Talal = 0.01;

// Static & GLUI control variables
const int M_PI = 3.1415;
int main_window;
int group_int;
int planet_names_int,
	planet_paths_int,
	planet_origins_int,
	lighting_int,
	grid_int;

// Pointers to the GLUI windows and controls
GLUI *glui, *glui2;
GLUI_Panel      *panel_main, *panel_main2, *panel_models;
GLUI_Checkbox   *checkbox_names, *checkbox_paths, *checkbox_origins, *checkbox_light, *checkbox_grid;
GLUI_Spinner    *spinner_speed;
GLUI_RadioGroup *radio_group;
GLUI_Rotation	*rot_view;
GLUI_Translation *trans_xy, *trans_x, *trans_y;

// GLUI Live variables
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

// GLUT/GLUI Callbacks
void MyOpenGLInit();										// used for opengl specific initialization
void InitializeGLUI();										// used for GLUI specific initialization
void myDisplay();											// This is the function that does the actual drawing
void myResize(int width, int height);						// This function is called by GLUT whenever the user changes the size of the window
void myKeyboard(unsigned char key, int x, int y);			// This function is called by GLUT whenever a keyboard button is pressed
void myMouse(int button, int state, int x, int y);			// This function is called by GLUT whenever a mouse button is pressed
void myMotion(int x, int y);								// This function is called by GLUT whenever a mouse is moved
void myGlutMenu( int value);
void control_cb(int control);
void myIdle();

//*********************************** Function Main *******************************************//

void main(int argc, char **argv)
{
	// GLUT Initialization
	glutInit(&argc, argv);									// initialize GLUT
	glutInitWindowSize(900, 600);							// initialize window size
	glutInitWindowPosition(100, 100);						// initialize window position
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// use RGB colors, double buffering, depth testing
	main_window = glutCreateWindow("Solar System Simulation");	// create the window with title "Solar System Simulation"

	// GLUT Callback Registration
	glutDisplayFunc(myDisplay);								// give GLUT a pointer to the Dislpay function
	GLUI_Master.set_glutReshapeFunc( myResize );			// give GLUT a pointer to the Resize function
	GLUI_Master.set_glutKeyboardFunc( myKeyboard );			// give GLUT a pointer to the Keyboard function
	GLUI_Master.set_glutMouseFunc( myMouse );				// give GLUT a pointer to the Mouse function
	glutMotionFunc(myMotion);								// give GLUT a pointer to the Motion function
	MyOpenGLInit();											// to do my own initialization
	InitializeGLUI();										// to do my own GLUI initialization
	GLUI_Master.set_glutIdleFunc( myIdle );					// register the idle callback with GLUI, *not* with GLUT
	glutMainLoop();											// starts the Program (this function never returns, any code after it will not be executed)
}	

//*********************************** OpenGL Initialization ***********************************//

void MyOpenGLInit()
{
	glClearColor(0.00f, 0.00f, 0.00f, 0.5f);				// set the background color to black

	for(int i=0; i<10; i++)
		rotate[i] = rotate[i] * 24;

	MessageBox( NULL, helptext, "Solar System Help", NULL );
	MessageBox( NULL, keybtext, "Keyboard Help", NULL );
	// Shading properties
	GLfloat light_ambient[] =	{0.5, 0.5, 0.5, 1.0};
	GLfloat light_diffuse[] =	{1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[] =	{0.5, 0.5, 0.5, 1.0};
    GLfloat light_position[] =	{0.0, 0.0, 0.0, 1.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}

//*********************************** GLUI Initialization ***********************************//

void InitializeGLUI()
{
	// Print-out GLUI Version
	printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );
	
	// Create the side subwindow
	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT );
	
	glui->add_statictext( "Solar System Options" );
	glui->add_separator();

	panel_main = glui->add_panel( "Planet Properties", true );
		
		panel_models = glui->add_panel_to_panel( panel_main, "Shape Modeling", GLUI_PANEL_EMBOSSED);
			radio_group = glui->add_radiogroup_to_panel( panel_models, &group_int);
			glui->add_radiobutton_to_group( radio_group, "Wireframe models" );
			glui->add_radiobutton_to_group( radio_group, "Solid models" );
		
		glui->add_separator_to_panel( panel_main );
		
		checkbox_names = glui->add_checkbox_to_panel( panel_main, "Enable planet names", &planet_names_int );
		checkbox_paths = glui->add_checkbox_to_panel( panel_main, "Enable planet paths", &planet_paths_int );
		checkbox_origins = glui->add_checkbox_to_panel( panel_main, "Enable planet origins", &planet_origins_int );
		
		glui->add_separator_to_panel( panel_main );
		
		spinner_speed  = glui->add_spinner_to_panel( panel_main, "Speed:", GLUI_SPINNER_FLOAT, &Talal);
			spinner_speed->set_float_limits(-10.0f, 10.0f);
			spinner_speed->set_alignment( GLUI_ALIGN_CENTER );
			spinner_speed->set_speed( 0.1 );
		
		glui->add_statictext( " " );

	panel_main2 = glui->add_panel( "Viewing Options", true );
	
		checkbox_light = glui->add_checkbox_to_panel( panel_main2, "Enable planet shading", &lighting_int);
		checkbox_grid = glui->add_checkbox_to_panel( panel_main2, "Show grid at origin", &grid_int);
	
		glui->add_separator_to_panel( panel_main2 );

		glui->add_button_to_panel( panel_main2, "Disable movement", DISABLE_ID, control_cb );
		glui->add_button_to_panel( panel_main2, "Enable movement", ENABLE_ID, control_cb );

		glui->add_separator_to_panel( panel_main2 );

		glui->add_button_to_panel( panel_main2, "Hide panel", HIDE_ID, control_cb );
		glui->add_button_to_panel( panel_main2, "Show panel", SHOW_ID, control_cb );

		glui->add_separator_to_panel( panel_main2 );

		glui->add_button_to_panel( panel_main2, "About..", ABOUT_ID, control_cb );
		glui->add_button_to_panel( panel_main2, "Help", HELP_ID, control_cb );
		glui->add_button_to_panel( panel_main2, "Quit", 0,(GLUI_Update_CB)exit );

	// Link windows to GLUI, and register idle callback
	glui->set_main_gfx_window( main_window );

	// Create the bottom subwindow
	glui2 = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_BOTTOM );
	glui2->set_main_gfx_window( main_window );

	rot_view = glui2->add_rotation( "Tumble/Spin View", view_rotate );
	rot_view->set_spin( 0.98 );
	glui2->add_column( false );

	trans_xy = glui2->add_translation( "Track/Span View", GLUI_TRANSLATION_XY, obj_pos );
	trans_xy->set_speed( .005 );
	glui2->add_column( false );
  
	GLUI_Translation *trans_x = glui2->add_translation( "Track in x-axis", GLUI_TRANSLATION_X, obj_pos );
	trans_x->set_speed( .005 );
	glui2->add_column( false );
  
	GLUI_Translation *trans_y = glui2->add_translation( "Track in y-axis", GLUI_TRANSLATION_Y, &obj_pos[1] );
	trans_y->set_speed( .005 );
	glui2->add_column( false );
  
	GLUI_Translation *trans_z = 
    glui2->add_translation( "Zoom/Dolly View", GLUI_TRANSLATION_Z, &obj_pos[2] );
	trans_z->set_speed( .010 );
}

// GLUI Controls function
void control_cb( int control )
{
	if		( control == ENABLE_ID )	{ glui2->enable(); }
	else if ( control == DISABLE_ID )	{ glui2->disable(); }
	else if ( control == SHOW_ID )		{ glui2->show(); }
	else if ( control == HIDE_ID )		{ glui2->hide(); }
	else if ( control == ABOUT_ID )		{ MessageBox( NULL, "Copyright © Talal Malas 2002.", "About Solar System 1.0.", NULL ); }
	else if ( control == HELP_ID )		{ MessageBox( NULL, helptext, "Solar System Help", NULL );
										  MessageBox( NULL, keybtext, "Keyboard Help", NULL );}
}

//*********************************** Function myDisplay **************************************//

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear the window

	// Grid
	if (grid_int == 1)
	{
		glPushMatrix();
			glTranslatef(zoom_x, zoom_y, zoom_z);			// translate projection in x,y,z space
			glRotatef(rot_x, 1.0f, 0.0f, 0.0f);				// rotate projection around x-axis
			glRotatef(rot_y, 0.0f, 1.0f, 0.0f);				// rotate projection around y-axis
			glRotatef(rot_z, 0.0f, 0.0f, 1.0f);				// rotate projection around z-axis
			
			glTranslatef( obj_pos[0], obj_pos[2], obj_pos[1] );
			glMultMatrixf( view_rotate );
	
			float i = 0, j = 0;
			glBegin(GL_LINES);								// draw the origin & axis
				glLineWidth(0.5);
				glColor3f(0.5f, 0.5f, 0.5f);
				for (i=-60; i<=60; i=i+1.0 ) { glVertex3f(-60.0, i, 0.0); glVertex3f( 60.0, i, 0.0); }
				for (i=-60; i<=60; i=i+1.0 ) { glVertex3f(i, -60.0, 0.0); glVertex3f(i,  60.0, 0.0); }
			glEnd();
		glPopMatrix();
	}

	for (int p=0; p<10; p++)
	{	
		glPushMatrix();
			if (p != 0)
				glRotatef(incline[p], 0.0f, 0.0f, 1.0f);		// orbit inclination (in degrees)

			glTranslatef(zoom_x, zoom_y, zoom_z);				// translate projection in x,y,z space
			glRotatef(rot_x, 1.0f, 0.0f, 0.0f);					// rotate projection around x-axis
			glRotatef(rot_y, 0.0f, 1.0f, 0.0f);					// rotate projection around y-axis
			glRotatef(rot_z, 0.0f, 0.0f, 1.0f);					// rotate projection around z-axis
			
			glTranslatef( obj_pos[0], obj_pos[2], obj_pos[1] );
			glMultMatrixf( view_rotate );

			glColor3fv(vcolor[p]);								// select color

			// draw orbit
			if (p != 0)
			{
                if (planet_paths_int == 1)
				{
					glBegin(GL_LINE_LOOP);
						for (float th = 0.0; th<=2*M_PI; th=th+0.01745)
							glVertex3f(  distance[p]*cos(th), distance[p]*sin(th), 0.0 );
					glEnd();	
				}
			}

			glRotatef(zRot/revolve[p], 0.0f, 0.0f, 1.0f);		// speed of revolution around the sun
			if (p != 0)
			{
				glTranslatef(distance[p], 0.0f, 0.0f);			// planet's distance from the sun
				glRotatef(oblique[p], 1.0f, 0.0f, 0.0f);		// planet's obliquity (tilt of axis of rotation)
				glRotatef(zRot*rotate[p], 0.0f, 0.0f, 1.0f);	// speed of rotation around its axis
			}
		
			if (p == 5 || p == 6)
			{
				glMaterialfv(GL_FRONT, GL_DIFFUSE, color[11]);
				glColor3f(0.5f, 0.0f, 1.0f);
				glutSolidCone((radius[5]+0.1),0.0,50,3);
			}

			glMaterialfv(GL_FRONT, GL_DIFFUSE, color[p]);
			glColor3fv(vcolor[p]);								// select color

			// draw planet
			if (group_int == 0)	glutWireSphere(radius[p], 50, 50);
			else				glutSolidSphere(radius[p], 50, 50);

			if (planet_origins_int == 1)
			{
				glLineWidth(2.0);
				glBegin(GL_LINES);								// draw the origin & axis
					glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(radius[p]*2,0.0f,0.0f); // x-axis
					glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,radius[p]*2,0.0f); // y-axis
					glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,radius[p]*2); // z-axis
				glEnd();
			}
			glLineWidth(0.1);

			if (planet_names_int == 1)
			{
				glMaterialfv(GL_FRONT, GL_DIFFUSE, color[10]);
				glColor3f(1.0f, 1.0f, 1.0f);
				glRasterPos3f(0.0f, 0.0f, 0.0f);
				for( int i=0; i<(int)strlen( planet_list[p] ); i++ )
					glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, planet_list[p][i] );
			}
		glPopMatrix();
	}

	// Moon
	glPushMatrix();
		glRotatef(incline[3], 0.0f, 0.0f, 1.0f);			// orbit inclination (in degrees)

		glTranslatef(zoom_x, zoom_y, zoom_z);				// translate projection in x,y,z space
		glRotatef(rot_x, 1.0f, 0.0f, 0.0f);					// rotate projection around x-axis
		glRotatef(rot_y, 0.0f, 1.0f, 0.0f);					// rotate projection around y-axis
		glRotatef(rot_z, 0.0f, 0.0f, 1.0f);					// rotate projection around z-axis
		
		glTranslatef( obj_pos[0], obj_pos[2], obj_pos[1] );
		glMultMatrixf( view_rotate );
		
		glRotatef(zRot, 0.0f, 0.0f, 1.0f);					// revolve around the sun at an angle
		glTranslatef(distance[3], 0.0f, 0.0f);				// distance of mother planet from the sun
		
		glColor3f(1.0f, 1.0f, 1.0f);
		if (planet_paths_int == 1)
		{
			glBegin(GL_LINE_LOOP);
			for (float th = 0.0; th<=2*M_PI; th=th+0.01745)
				glVertex3f( cos(th)/10, sin(th)/10, 0.0 );
			glEnd();
		}

		glTranslatef((sin(zRot/4)/10), (cos(zRot/4)/10), 0.0f); // rotation + distance
		glRotatef(15, 0.0f, 1.0f, 0.0f);					// moons's obliquity (tilt of axis of rotation)
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color[10]);
		glColor3f(1.0f, 1.0f, 1.0f);

		// draw moon
		if (group_int == 0)	glutWireSphere(0.005, 10, 10);
		else				glutSolidSphere(0.005, 10, 10);
		
		if (planet_origins_int == 1)
		{
			glBegin(GL_LINES);								// draw the origin & axis
				glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.01f,0.0f,0.0f); // x-axis
				glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.01f,0.0f); // y-axis
				glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,0.01f); // z-axis
			glEnd();
		}

		if (planet_names_int == 1)
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, color[10]);
			glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos3f(0.0f, 0.0f, 0.0f);
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, 'M');
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, 'o');
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, 'o');
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, 'n');
		}
	glPopMatrix();	

	if (lighting_int == 1)		glEnable(GL_LIGHTING);		// enable lighting
	else						glDisable(GL_LIGHTING);		// disable lighting

	glFlush();												// draw everything
	glutSwapBuffers();										// swap between back and front buffer
}

//*********************************** Function Callbacks **************************************//

void myGlutMenu( int value ) { myKeyboard( value, 0, 0 ); }

void myResize(int width, int height)
{
	if ( height == 0 ) height = 1;							// divide-by-zero protection
	aspect = (GLdouble)width / (GLdouble)height;			// calculate the window's aspect ratio
	glViewport(0, 0, width, height);						// set the viewport to the whole window
	GLUI_Master.auto_set_viewport();
	glMatrixMode(GL_PROJECTION);							// choose the projection matrix
	glLoadIdentity();										// load the identity matrix into the projection matrix
	gluPerspective(60.0, aspect, 0.1, 100.0);				// define a 3D OpenGL Viewing Projection
	glMatrixMode(GL_MODELVIEW);								// choose the modelview matrix (or transformation matrix)
	glLoadIdentity();										// load the identity matrix into the modelview matrix
	glutPostRedisplay();									// request from GLUT to re-render the image (call myDisplay)
}

// Custom transitions using the keyboard
void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':  zoom_x = zoom_x + 0.1; break; case 'q':  zoom_x = zoom_x - 0.1; break;
		case 's':  zoom_y = zoom_y + 0.1; break; case 'a':  zoom_y = zoom_y - 0.1; break;
		case 'x':  zoom_z = zoom_z + 0.1; break; case 'z':  zoom_z = zoom_z - 0.1; break;
		
		case 'r':  rot_x = rot_x + 1; break; case 'e':  rot_x = rot_x - 1; break;
		case 'f':  rot_y = rot_y + 1; break; case 'd':  rot_y = rot_y - 1; break;
		case 'v':  rot_z = rot_z + 1; break; case 'c':  rot_z = rot_z - 1; break;

	};
	glutPostRedisplay();
}

// Custom Translation in (x,y)
void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		xstart = (float)x;
		ystart = (float)y;
	};
}

// Custom Translation in (x,y)
void myMotion(int x, int y)
{
	zoom_x = zoom_x + (x - xstart) * 0.002;
	zoom_y = zoom_y + (ystart - y) * 0.002;
	glutPostRedisplay();
	xstart = (float)x;
	ystart = (float)y;
}

// Rotation variable incrementation
void myIdle()
{
	zRot = zRot + Talal;
	if ( glutGetWindow() != main_window ) 
		glutSetWindow(main_window);  
	glutPostRedisplay();
}