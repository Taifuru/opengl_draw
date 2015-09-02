#include <GL/glew.h>
#include <GL/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

void init();
void display();
void prepare();

/** istedigimiz boyutta frame buffer olustrabiliyoruz */
int const fbo_width = 2048;
int const fbo_height = 2048;

GLuint fb, color, depth;


void write_bmp_header(FILE *f, int w, int h) {
	int filesize = 54 + 3*w*h;
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
}

void save_current_framebuffer(int w, int h) {
	size_t buffer_size = w * h * 3;
	char * rawImageBuffer = (char*)malloc(buffer_size);
	glReadPixels(0, 0, w-1, h-1, GL_RGB, GL_UNSIGNED_BYTE, rawImageBuffer);


	FILE *f = fopen("framebuffer.bmp", "wb");
	assert(f != NULL);

	write_bmp_header(f, w, h);
	fwrite(rawImageBuffer, buffer_size, 1, f);
	fclose(f);
}

/** framebuffer olusturup caydanlik ciziyor.  */
void prepare() {
	static float a=0, b=0, c=0;

	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);


	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0,0, fbo_width, fbo_height);

	glClearColor(1,1,1,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void cizim_yap() {
	printf("cizim_yap\n");


//	glBindTexture(GL_TEXTURE_2D, 0);
//	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glViewport(0,0, fbo_width, fbo_height);


	float r = (rand() % 80 + 20) / 50.0;
	float color1 = (rand() % 100) / 100.0;
	float color2 = (rand() % 100) / 100.0;
	float color3 = (rand() % 100) / 100.0;

	glPushMatrix();

	glLineWidth(30);
	glColor3f(color1, color2, color3);
	glBegin(GL_LINES);
		glVertex2f(0.0, r-1);
		glVertex2f(0.5, r-1);
	glEnd();
	glPopMatrix();



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
    	glBindFramebuffer(GL_FRAMEBUFFER, fb);
    	glViewport(0,0, fbo_width, fbo_height);
    	save_current_framebuffer(fbo_width, fbo_height);
    	glBindFramebuffer(GL_FRAMEBUFFER, 0);
        break;
    case 'a':
    case 'A':
    	flag_cizim_yap = 1;
    	break;
    default:
        ;
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	glutCreateWindow("FBO test");
	glutDisplayFunc(display);
	glutIdleFunc(glutPostRedisplay);

	glewInit();

    glutKeyboardFunc(keyboardCB);

	init();

	prepare();
	//save_current_framebuffer(fbo_width, fbo_height);

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
	static float a=0, b=0, c=0;

	const int win_width  = glutGet(GLUT_WINDOW_WIDTH);
	const int win_height = glutGet(GLUT_WINDOW_HEIGHT);
	const float aspect = (float)win_width/(float)win_height;

	// ######################################################################

	//cizim_yap();

	if (flag_cizim_yap) {
		flag_cizim_yap = 0;
		cizim_yap();
	}

	// ######################################################################

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0,0, win_width, win_height);
	glClearColor(1.,1.,1.,0.);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	glPushMatrix();


	glRotatef(180, 0, 0, 1);


	////////////////////////////////
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, color);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	glBegin( GL_QUADS );
		glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0);
		glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0);
		glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
		glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
	glEnd();

	///////////////////////////////////7

	glBindTexture(GL_TEXTURE_2D, 0);


//	glLineWidth(2.5);
//	glColor3f(1.0, 0, 0);
//	glBegin(GL_LINES);
//		glVertex2f(0.0, 0.0);
//		glVertex2f(0.5, 0);
//	glEnd();


	glPopMatrix();
	glDisable(GL_BLEND);

}

void display()
{
	final();

	glutSwapBuffers();
}
