#include <GL/glew.h>
#include <GL/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

void init();
void display();
void prepare();

int mouseX = 0;
int mouseY = 0;
int mouseLeftDown = 0;
int screenWidth = 630;
int screenHeight = 354;

/** istedigimiz boyutta frame buffer olustrabiliyoruz */
int const fbo_width = 2048;
int const fbo_height = 2048;

GLuint fb, color, depth;

GLuint texture_background;
GLuint texture_old_draw;

void save_image_to_file(GLuint fb, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0,0, width, height);


    // Image Writing
    unsigned char * imageData = (unsigned char *) malloc( width * height * 4 );
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FILE *file;
    // open texture data
    file = fopen("draw.data", "w");
    if (file == NULL)
        return ;

    // read texture data
    fwrite(imageData, width * height * 4, 1, file);
    fclose(file);

	free(imageData);
	printf("saving successful\n");
}

GLuint raw_texture_load_4(const char *filename, int width, int height)
{
    GLuint texture;
    unsigned char *data;
    FILE *file;

    // open texture data
    file = fopen(filename, "rb");
    if (file == NULL)
        return 0;

    // allocate buffer
    data = (unsigned char*) malloc(width * height * 4);

    // read texture data
    fread(data, width * height * 4, 1, file);
    fclose(file);

    // allocate a texture name
    glGenTextures(1, &texture);

    // select our current texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // texture should tile
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // build our texture mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // free buffer
    free(data);

    return texture;
}

/** framebuffer olusturup caydanlik ciziyor.  */
void prepare() {

	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0,0, fbo_width, fbo_height);

	glClearColor(1,1,1,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	texture_old_draw = raw_texture_load_4("draw.data", fbo_width, fbo_height);
	if (texture_old_draw != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_old_draw);

		glPushMatrix();
		glBegin( GL_QUADS );
			glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
			glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
			glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
			glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
		glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
	}


}

float onceki_px, onceki_py;

void cizim_yap() {
	if(mouseLeftDown == 1) {

	printf("cizim_yap\n");

//	glBindTexture(GL_TEXTURE_2D, color);
//	glEnable(GL_TEXTURE_2D);

	float r = (rand() % 80 + 20) / 50.0;
	float color1 = (rand() % 100) / 100.0;
	float color2 = (rand() % 100) / 100.0;
	float color3 = (rand() % 100) / 100.0;

	//printf("color1 %f\ncolor2 %f\ncolor3 %f\n", color1, color2, color3);

	glPushMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0,0, fbo_width, fbo_height);

	//glLineWidth(30);

	//glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(color1, color2, color3);
	glRotatef( 180, 1.0f, 0.0f, 0.0f );


		const int win_width  = glutGet(GLUT_WINDOW_WIDTH);
		const int win_height = glutGet(GLUT_WINDOW_HEIGHT);
		printf("x: %d -- y: %d\n", mouseX, mouseY);
		float px = ((float)mouseX / win_width) * 2 - 1;
		float py = ((float)mouseY / win_height) * -2 + 1;

		glLineWidth(30);

		glBegin(GL_LINES);
			glVertex2f(onceki_px, onceki_py);
			glVertex2f(px, py);
		glEnd();

		printf("%f,%f - %f,%f\n", onceki_px, onceki_py, px, py);


		onceki_px = px;
		onceki_py = py;




	// cizgi cizme
//	glBegin(GL_LINES);
//		glVertex2f(0.0, -0.5);
//		glVertex2f(0.5, -0.5);
//	glEnd();


	glPopMatrix();

	//glClearColor(1, 1, 1, 0);



	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

}

int flag_cizim_yap = 0;

void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        exit(0);
        break;

    case 's':
    case 'S':
    	save_image_to_file(fb, fbo_width, fbo_height);
        break;
    case 'a':
    case 'A':
    	flag_cizim_yap = 1;
    	break;
    default:
        ;
    }
    //glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			const int win_width  = glutGet(GLUT_WINDOW_WIDTH);
			const int win_height = glutGet(GLUT_WINDOW_HEIGHT);
			float px = ((float)x / win_width) * 2 - 1;
			float py = ((float)y / win_height) * -2 + 1;
			onceki_px = px;
			onceki_py = py;
			mouseLeftDown = 1;
			//printf("mouseLeftDown %d\n", mouseLeftDown);
		}
		else {
			mouseLeftDown = 0;
		}
	}
	//cizim_yap();
}

void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown == 1)
    {
        mouseX = x;
        mouseY = y;
        cizim_yap();
    }
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	glutCreateWindow("FBO test");
	glutDisplayFunc(display);
	glutIdleFunc(glutPostRedisplay);
	glutInitWindowSize(screenWidth, screenHeight);

	glewInit();

    glutKeyboardFunc(keyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);

	init();

	prepare();
	//save_current_framebuffer(fbo_width, fbo_height);

	texture_background = raw_texture_load_4("mini.data", 630, 354);

	glutMainLoop();

	return 0;
}

void CHECK_FRAMEBUFFER_STATUS()
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch(status) {
	case GL_FRAMEBUFFER_COMPLETE:
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
	/* choose different formats */
		break;

	default:
		/* programming error; will fail on all hardware */
		fputs("Framebuffer Error\n", stderr);
		exit(-1);
	}
}

float const light_dir[]={1,1,1,0};
float const light_color[]={1,0.95,0.9,1};

void init()
{
	glGenFramebuffers(1, &fb);
	glGenTextures(1, &color);
	glGenRenderbuffers(1, &depth);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glBindTexture(GL_TEXTURE_2D, color);
	glTexImage2D(	GL_TEXTURE_2D,
			0,
			GL_RGBA,
			fbo_width, fbo_height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

	CHECK_FRAMEBUFFER_STATUS();
}

float donus_hizi = 10;

/** nasil yaptigini bilmiyorum fakat bir sekilde prepare fonksiyonunda cizilen caydanligi
	4 kere cizip hareket ettiriyor */
void final() {

	const int win_width  = glutGet(GLUT_WINDOW_WIDTH);
	const int win_height = glutGet(GLUT_WINDOW_HEIGHT);

	// ######################################################################

	if (flag_cizim_yap) {
		flag_cizim_yap = 0;
		cizim_yap();
	}

	// ######################################################################

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0, win_width, win_height);

	glClearColor(1.,1.,1.,0.);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



	/* slayt resmi */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glPushMatrix();
	//glRotatef( 180, 0.0f, 0.0f, 1.0f );
	glRotatef( 180, 1.0f, 0.0f, 0.0f );


	glBegin( GL_QUADS );
		glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
		glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
		glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);




	////////////////////////////////
	/* cizim resmi */
	glColor3f(1, 1, 1); // arka plana beyaz saydam rek
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, color);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glRotatef( 180, 1.0f, 0.0f, 0.0f );
	glBegin( GL_QUADS );
		glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
		glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
		glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	///////////////////////////////////7



}

void display()
{
	final();

	glutSwapBuffers();
}
