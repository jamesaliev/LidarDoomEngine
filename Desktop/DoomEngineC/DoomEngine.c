#include <math.h>
#include <stdio.h>
#include <stdlib.h>    // <--- For rand()
#include <GL/glut.h> 
#include <windows.h>

#define res        4   
#define SW         160*res
#define SH         120*res
#define SW2        (SW/2)
#define SH2        (SH/2)
#define pixelScale 8/res
#define GLSW       (SW*pixelScale)
#define GLSH       (SH*pixelScale)
#define numSect 3
#define numWall 12
#define MOUSE_SENSITIVITY_X 0.2f
#define MOUSE_SENSITIVITY_Y 0.2f

static int centerX = 0;
static int centerY = 0;
static int isJumping = 0;        
static float jumpVelocity = 0;   
static const float GRAVITY = 0.5f;
static const float JUMP_STRENGTH = 5.0f;
static const int GROUND_LEVEL = 40;
static int mouseInsideWindow = 0;
static int crosshairColor = 0;
static int pickBuffer[SW][SH];


typedef struct 
{
 int fr1,fr2;           
}time; time T;

typedef struct 
{
 int w,s,a,d;           
 int sl,sr;             
 int m;                 
}keys; keys K;

typedef struct {
	float cos[360];
	float sin[360];
} math; math M;

typedef struct {
	int x;
	int y;
	int z;
	int a;
	int l;
} player; player P;

typedef struct {
	int x1, y1;
	int x2, y2;
	int c;
} walls; walls W[30];

typedef struct {
	int ws, we;
	int z1, z2;
	int d;
	int c1, c2;
	int surf[SW];
	int surface;
} sectors; sectors S[30];

void pixel(int x,int y, int c)
{
 int rgb[3];
 if(c==0){  rgb[0]=255; rgb[1]=255; rgb[2]=  0;}  //Yellow	
 if(c==1){  rgb[0]=160; rgb[1]=160; rgb[2]=  0;}  //Yellow darker	
 if(c==2){  rgb[0]=  0; rgb[1]=255; rgb[2]=  0;}  //Green	
 if(c==3){  rgb[0]=  0; rgb[1]=160; rgb[2]=  0;}  //Green darker	
 if(c==4){  rgb[0]=  0; rgb[1]=255; rgb[2]=255;}  //Cyan	
 if(c==5){  rgb[0]=  0; rgb[1]=160; rgb[2]=160;}  //Cyan darker
 if(c==6){  rgb[0]=160; rgb[1]=100; rgb[2]=  0;}  //Brown	
 if(c==7){  rgb[0]=110; rgb[1]= 50; rgb[2]=  0;}  //Brown darker
 if(c==8){  rgb[0]=  0; rgb[1]= 60; rgb[2]=130;}  //Background 
 if(c==9){  rgb[0]= 31; rgb[1]= 31; rgb[2]= 31;}  //Dark gray
 if(c==10){ rgb[0]=  0; rgb[1]=  0; rgb[2]=  0;}  //Black
 if(c==11){ rgb[0]=  255; rgb[1]=  0; rgb[2]=  0;} //RED
 if(c==12){ rgb[0]=  0; rgb[1]=  255; rgb[2]=  0;} //BLUE
 if(c==13){ rgb[0]=  0; rgb[1]=  0; rgb[2]=  255;} //GREEN
 if(c==14){ rgb[0]=255; rgb[1]=255; rgb[2]=255; } // White
 glColor3ub(rgb[0],rgb[1],rgb[2]);
 glBegin(GL_POINTS);
 glVertex2i(x*pixelScale+2,y*pixelScale+2);
 glEnd();
}

void mouseMove(int mx, int my)
{
    if (!mouseInsideWindow) {
        return;
    }
    if (mx == centerX && my == centerY) {
        return;
    }
    {
        int dx = mx - centerX;
        int dy = my - centerY;
        P.a += (int)(dx * MOUSE_SENSITIVITY_X);
        P.l += (int)(dy * MOUSE_SENSITIVITY_Y);
        if (P.a < 0)   P.a += 360;
        if (P.a >= 360) P.a -= 360;
        if (P.l > 80)   P.l = 80;
        if (P.l < -80)  P.l = -80;
    }
    glutWarpPointer(centerX, centerY);
}

void mouseEnterLeave(int state)
{
    if (state == GLUT_ENTERED) {
        mouseInsideWindow = 1;
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(centerX, centerY);
    }
    else if (state == GLUT_LEFT) {
        mouseInsideWindow = 0;
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}

void movePlayer()
{
    if(K.a == 1 && K.m == 0) { P.a -= 2; if (P.a < 0) { P.a += 360; }}  
    if(K.d == 1 && K.m == 0) { P.a += 2; if (P.a > 359) { P.a -= 360; }}
 
    int dx = M.sin[P.a] * 10.0;
    int dy = M.cos[P.a] * 10.0;

    if(K.w == 1 && K.m == 0) { P.x += dx; P.y += dy; }
    if(K.s == 1 && K.m == 0) { P.x -= dx; P.y -= dy; }
    if(K.sr == 1) { P.x -= dy; P.y += dx; }
    if(K.sl == 1) { P.x += dy; P.y -= dx; }

    if (P.z > GROUND_LEVEL || isJumping) {
        P.z += jumpVelocity; 
        jumpVelocity -= GRAVITY; 
        if (P.z <= GROUND_LEVEL) {
            P.z = GROUND_LEVEL;
            isJumping = 0;
            jumpVelocity = 0;
        }
    }
    if (K.m == 1 && !isJumping) {
        isJumping = 1;  
        jumpVelocity = JUMP_STRENGTH; 
    }
}

void clearBackground() 
{
	
 int x,y;
 for(y=0;y<SH;y++)
 { 
  for(x=0;x<SW;x++){ pixel(x,y,10); pickBuffer[x][y] = -1; } //clear background color (9=dark gray)
 }	
}

void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2)
{
	float da = *y1;
	float db = y2;
	float d = da - db;
	if (d == 0) { d = 1; }
	float s = da/(da - db);
	*x1 = *x1 + s*(x2 - (*x1));
	*y1 = *y1 + s*(y2 - (*y1));
	if (*y1 == 0) { *y1 = 1; }
	*z1 = *z1 + s*(z2 - (*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int c, int s, int wallIndex)
{
	int x;
	int y;
	
	int dyb = b2 - b1;
	int dyt = t2 - t1;
	
	int dx = x2 - x1; if (dx == 0) { dx = 1; }
	
	int xs = x1;
	
	if (x1 < 1) { x1 = 1;} 
	if (x2 < 1) { x2 = 1;} 
	if (x1 > SW - 1) { x1 = SW - 1;} 
	if (x2 > SW - 1) { x2 = SW - 1;} 
	
	for (x = x1; x < x2; x++) {
		int y1 = dyb * (x - xs + 0.5)/dx + b1;
		int y2 = dyt * (x - xs + 0.5)/dx + t1;
		
		if (y1 < 1) { y1 = 1;} 
		if (y2 < 1) { y2 = 1;} 
		if (y1 > SH - 1) { y1 = SH - 1;} 
		if (y2 > SH - 1) { y2 = SH - 1;} 
		
		// Surfaces
		if (S[s].surface == 1) {
			// Save the bottom line
			S[s].surf[x] = y1;
			continue;
		}
		if (S[s].surface == 2) {
			// Save the top line
			S[s].surf[x] = y2;
			continue;
		}
		if (S[s].surface == -1) { 
			for (y = S[s].surf[x]; y < y1; y++) {
				// Instead of drawing every pixel, draw a dot only sometimes
				if ((rand() % 6) == 0) {
					pixel(x, y, S[s].c1);
				}
			}
		}
		if (S[s].surface == -2) {
			for (y = y2; y < S[s].surf[x]; y++) {
				if ((rand() % 6) == 0) {
					pixel(x, y, S[s].c2);
				}
			}
		}
		for (y = y1; y < y2; y++) {
			//if ((rand() % 6) == 0) {
				pixel(x, y, c);
				pickBuffer[x][y] = wallIndex;
			//}
		}
	}
}

int dist(int x1, int y1, int x2, int y2)
{
	int distance = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	return distance;
}

void draw3D()
{
	int s, w;
	int wx[4], wy[4], wz[4];
	float CS = M.cos[P.a];
	float SN = M.sin[P.a];
	int loop;

	// Bubble sort by sector distance
	for (s = 0; s < numSect - 1; s++) {
		for (w = 0; w < numSect - s - 1; w++) {
			if (S[w].d < S[w+1].d) {
				sectors st = S[w];
				S[w] = S[w+1];
				S[w+1] = st;
			}
		}
	}
	
	// Draw sectors
	for (s = 0; s < numSect; s++) {
		S[s].d = 0;
		if (P.z < S[s].z1) {
			S[s].surface = 1;
		}
		else if (P.z > S[s].z2) {
			S[s].surface = 2;
		}
		else {
			S[s].surface = 0;
		}
		
		for (loop = 0; loop < 2; loop++) {
			for (w = S[s].ws; w < S[s].we; w++) {
				int x1 = W[w].x1 - P.x;
				int y1 = W[w].y1 - P.y;
				int x2 = W[w].x2 - P.x;
				int y2 = W[w].y2 - P.y;
				
				if (loop == 0) {
					int swp = x1; x1 = x2; x2 = swp;
					swp = y1; y1 = y2; y2 = swp;
				}
				wx[0] = x1*CS - y1*SN;
				wx[1] = x2*CS - y2*SN;
				wx[2] = wx[0];
				wx[3] = wx[1];
				
				wy[0] = y1*CS + x1*SN;
				wy[1] = y2*CS + x2*SN;
				wy[2] = wy[0];
				wy[3] = wy[1];
				
				S[s].d += dist(0, 0, (wx[0]+wx[1])/2, (wy[0]+wy[1])/2);

				wz[0] = S[s].z1 - P.z + ((P.l*wy[0])/32.0);
				wz[1] = S[s].z1 - P.z + ((P.l*wy[1])/32.0);
				wz[2] = wz[0] + S[s].z2;
				wz[3] = wz[1] + S[s].z2;
				
				if (wy[0] < 1 && wy[1] < 1) { continue; }
				
				if (wy[0] < 1) {
					clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]);
					clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]);
				}
				if (wy[1] < 1) {
					clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]);
					clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]);
				}
				
				wx[0] = wx[0]*200/wy[0] + SW2; wy[0] = wz[0]*200/wy[0] + SH2;
				wx[1] = wx[1]*200/wy[1] + SW2; wy[1] = wz[1]*200/wy[1] + SH2;
				wx[2] = wx[2]*200/wy[2] + SW2; wy[2] = wz[2]*200/wy[2] + SH2;
				wx[3] = wx[3]*200/wy[3] + SW2; wy[3] = wz[3]*200/wy[3] + SH2;

				drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], W[w].c, s, w);

			}
			S[s].d /= (S[s].we - S[s].ws);
			S[s].surface *= -1;
		}
	}
}

void drawCrosshair() {
    int centerX = GLSW / 2;
    int centerY = GLSH / 2;
    int outerRadius = 8;
    int innerRadius = 6;
    int numSegments = 100; // Smoothness of the circle

    if (crosshairColor == 0) {
        glColor3ub(255, 255, 255); 
    } else {
        glColor3ub(255, 0, 0);
    }

    glBegin(GL_TRIANGLE_STRIP);
    int i;
    for (i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;
        float xOuter = centerX + cos(angle) * outerRadius;
        float yOuter = centerY + sin(angle) * outerRadius;
        float xInner = centerX + cos(angle) * innerRadius;
        float yInner = centerY + sin(angle) * innerRadius;
        glVertex2f(xOuter, yOuter);
        glVertex2f(xInner, yInner);
    }
    glEnd();
}

void resetCrosshairColor(int value) {
    crosshairColor = 0; // Reset to white
    glutPostRedisplay(); // Trigger a redraw
}      
        
void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		crosshairColor = 0;
	}
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    	crosshairColor = 1; // Turn the crosshair red
        //glutTimerFunc(200, resetCrosshairColor, 0); // Reset color after 200ms
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Scale mouse coordinates to match the pickBuffer resolution
        int sx = (x * SW) / windowWidth;
        int sy = ((windowHeight - y) * SH) / windowHeight;

        // Ensure coordinates are within bounds
        if (sx >= 0 && sx < SW && sy >= 0 && sy < SH) {
            int wallIndex = pickBuffer[sx][sy];
            if (wallIndex >= 0) {
                W[wallIndex].c = 14; // Set wall color to white
                glutPostRedisplay(); // Redraw the scene
            }
        }
    }
}

void display() {
    glViewport(0, 0, GLSW, GLSH); // Match the OpenGL viewport to the window
    if (T.fr1 - T.fr2 >= 33) {    // ~30 FPS
        clearBackground();
        movePlayer();
        draw3D();
        drawCrosshair();
        T.fr2 = T.fr1;
        glutSwapBuffers();
    }
    T.fr1 = glutGet(GLUT_ELAPSED_TIME);
    glutPostRedisplay();
}


void KeysDown(unsigned char key, int x, int y)
{
    if(key == 'w') { K.w = 1; }
    if(key == 's') { K.s = 1; }
    if(key == ',') { K.a = 1; }
    if(key == '.') { K.d = 1; }
    if(key == 'a') { K.sr = 1; }
    if(key == 'd') { K.sl = 1; }
    if(key == ' ') { K.m = 1; }
}

void KeysUp(unsigned char key, int x, int y)
{
    if(key == 'w') { K.w = 0; }
    if(key == 's') { K.s = 0; }
    if(key == ',') { K.a = 0; }
    if(key == '.') { K.d = 0; }
    if(key == 'a') { K.sr = 0; }
    if(key == 'd') { K.sl = 0; }
    if(key == ' ') { K.m = 0; }
}
int loadSectors[] = 
{
    // Small present
    0, 4, 0, 20, 2, 11,  // Small box (red, green)
    // Medium present
    4, 8, 0, 30, 4, 12,  // Medium box (cyan, darker cyan)
    // Large present
    8, 12, 0, 40, 6, 13, // Large box (brown, darker brown)
};

int loadWalls[] = 
{
    // Small present
    -60, 0,  -40, 0,   11,  // Bottom
    -40, 0,  -40, 20,  11,  // Right
    -40, 20, -60, 20,  11,  // Top
    -60, 20, -60, 0,   11,  // Left

    // Medium present
    -20, 0,   20, 0,   12,  // Bottom
    20, 0,    20, 30,  12,  // Right
    20, 30,  -20, 30,  12,  // Top
    -20, 30, -20, 0,   12,  // Left

    // Large present
    40, 0,   80, 0,   13,  // Bottom
    80, 0,   80, 40,  13,  // Right
    80, 40,  40, 40,  13,  // Top
    40, 40,  40, 0,   13,  // Left
};


void init()
{
    centerX = (int)(GLSW / 2);
    centerY = (int)(GLSH / 2);
    glutPassiveMotionFunc(mouseMove);
    glutMotionFunc(mouseMove);
    glutEntryFunc(mouseEnterLeave);

	int x;
	for (x = 0; x < 360; x++) {
		M.cos[x] = cos(x/180.0*M_PI);
		M.sin[x] = sin(x/180.0*M_PI);
	}
	
	P.x = 70;
	P.y = -110;
	P.z = 100;
	P.a = 0;
	P.l = 0;
	
	int s, w, v1=0, v2=0;
	for (s = 0; s < numSect; s++) {
		S[s].ws = loadSectors[v1+0];
		S[s].we = loadSectors[v1+1];
		S[s].z1 = loadSectors[v1+2];
		S[s].z2 = loadSectors[v1+3] - loadSectors[v1+2];
		S[s].c1 = loadSectors[v1+4];
		S[s].c2 = loadSectors[v1+5];
		v1 += 6;
		for (w = S[s].ws; w < S[s].we; w++) {
			W[w].x1 = loadWalls[v2+0];
			W[w].y1 = loadWalls[v2+1];
			W[w].x2 = loadWalls[v2+2];
			W[w].y2 = loadWalls[v2+3];
			W[w].c  = loadWalls[v2+4];
			v2 += 5;
		}
	}
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowPosX = (screenWidth - GLSW) / 2;
    int windowPosY = (screenHeight - GLSH) / 2;

    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(GLSW, GLSH);

    glutCreateWindow("Lidar Doom Engine w/ Mouse");
    glPointSize(pixelScale);
    gluOrtho2D(0, GLSW, 0, GLSH);

    init();
    glutDisplayFunc(display); // Register display callback
    glutKeyboardFunc(KeysDown);
    glutKeyboardUpFunc(KeysUp);
    glutMouseFunc(mouseClick); // Register mouse click callback

    // Optional: seed rand() for different patterns each time
    srand(12345);

    glutMainLoop();
    return 0;
}
