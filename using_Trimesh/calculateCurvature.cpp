//
//  calculateCurvature.cpp
//  
//
//  Created by Shashank Ranjan on 11/20/14.
//
//

#ifdef __APPLE__
# include <OPENGL/OpenGL.h>
# include <GLUT/Glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif
#include <stdio.h>
#include <iostream>
#include "trimesh2/Trimesh.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

using namespace std;
using namespace trimesh;

TriMesh *m = NULL;
GLfloat *curvatures = NULL;

GLfloat * calculateCurvature(char * filename){
    m = TriMesh::read(filename);
    if (!m)
        exit(1);
    
    m->need_curvatures();
    int nv = m->vertices.size();
    
    GLfloat ** curvs = new GLfloat[nv][2];
    for(int i = 0;i < nv;i++){
        curvs[i][0] = m->curv1[i];
        curvs[i][1] = m->curv2[i];
    }
    
    return curvs;
}

int main(int argc, char **argv){
    char *filename = "pawn.obj";
    curvatures = calculateCurvature(filename);
    //calculateCurvature(filename);
    return 0;
}

