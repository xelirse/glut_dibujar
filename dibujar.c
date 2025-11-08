// dibujar_pincel_tubo_final.c
// Compilar: gcc dibujar_pincel_tubo_final.c -o dibujar_pincel_tubo_final -lGL -lGLU -lglut
#include <GL/glut.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_POINTS 200000

int winW=800, winH=600;
bool drawing=false;
float colorR=0.0f, colorG=0.0f, colorB=0.0f;
float lineWidth=20.0f;

int px[MAX_POINTS];
int py[MAX_POINTS];
bool penDown[MAX_POINTS];
int pointCount=0;

// ------------------- Añadir punto -------------------
void addPoint(int x,int y,bool cont){
    if(pointCount>=MAX_POINTS) return;
    px[pointCount]=x;
    py[pointCount]=y;
    penDown[pointCount]=cont;
    pointCount++;
}

// ------------------- Interpolación para suavidad -------------------
void addInterpolatedPoint(int x,int y){
    if(pointCount==0){
        addPoint(x,y,false);
        return;
    }
    int lastX=px[pointCount-1];
    int lastY=py[pointCount-1];
    float dx=x-lastX;
    float dy=y-lastY;
    float dist=sqrtf(dx*dx+dy*dy);
    if(dist<0.5f) return;
    int steps=(int)(dist/(lineWidth/4.0f))+1;
    for(int i=1;i<=steps;i++){
        float t=(float)i/steps;
        int ix=(int)(lastX+dx*t);
        int iy=(int)(lastY+dy*t);
        addPoint(ix,iy,true);
    }
}

// ------------------- Círculo -------------------
void drawRoundPoint(float cx,float cy,float radius){
    int segments=20;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=segments;i++){
        float angle=2.0f*M_PI*i/segments;
        glVertex2f(cx+cos(angle)*radius,cy+sin(angle)*radius);
    }
    glEnd();
}

// ------------------- Dibuja un segmento con extremos redondeados -------------------
void drawSegment(int x0,int y0,int x1,int y1){
    float dx=x1-x0, dy=y1-y0;
    float len=sqrtf(dx*dx+dy*dy);
    if(len==0) return;
    dx/=len; dy/=len;
    float nx=-dy*(lineWidth/2.0f);
    float ny= dx*(lineWidth/2.0f);

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(x0+nx,y0+ny);
    glVertex2f(x0-nx,y0-ny);
    glVertex2f(x1+nx,y1+ny);
    glVertex2f(x1-nx,y1-ny);
    glEnd();

    drawRoundPoint(x0,y0,lineWidth/2.0f);
    drawRoundPoint(x1,y1,lineWidth/2.0f);
}

// ------------------- Dibujar todo el path -------------------
void drawTube(int start,int end){
    if(end <= start) return;          // no hay puntos
    if(end-start == 1){               // solo un punto
        drawRoundPoint(px[start], py[start], lineWidth/2.0f);
        return;
    }
    for(int i=start;i<end-1;i++){
        drawSegment(px[i], py[i], px[i+1], py[i+1]);
    }
}

// ------------------- Display -------------------
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(colorR,colorG,colorB);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

    int start=0;
    while(start<pointCount){
        int end=start+1;
        while(end<pointCount && penDown[end]) end++;
        drawTube(start,end);
        start=end;
    }

    glutSwapBuffers();
}

// ------------------- Eventos -------------------
void reshape(int w,int h){
    winW=w; winH=h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);
    glMatrixMode(GL_MODELVIEW);
}

void mouseButton(int button,int state,int x,int y){
    int gy=winH-y;
    if(button==GLUT_LEFT_BUTTON){
        if(state==GLUT_DOWN){
            drawing=true;
            addPoint(x,gy,false); // primer punto visible
            glutPostRedisplay();
        }else if(state==GLUT_UP){
            drawing=false;
        }
    }else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN){
        pointCount=0; // limpiar todo
        glutPostRedisplay();
    }
}

void mouseMotion(int x,int y){
    if(drawing){
        int gy=winH-y;
        addInterpolatedPoint(x,gy);
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key,int x,int y){
    switch(key){
        case 27: case 'q': case 'Q': exit(0); break;
        case 'c': case 'C': pointCount=0; break;
        case '1': colorR=colorG=colorB=0; break;
        case '2': colorR=1;colorG=colorB=0; break;
        case '3': colorR=0;colorG=0.8;colorB=0; break;
        case '4': colorR=0;colorG=0;colorB=1; break;
        case '+': lineWidth+=1.0f; break;
        case '-': if(lineWidth>1.0f) lineWidth-=1.0f; break;
    }
    glutPostRedisplay();
}

void initGL(){
    glClearColor(1,1,1,1);
}

// ------------------- Main -------------------
int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("Pincel tipo tubo vectorial");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
