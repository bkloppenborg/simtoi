/*
 * main.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <string>

using namespace std;

void init(void);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void idle(void);
int main(int argc, char** argv);
void CheckOpenGLError(string function_name);

// OpenGL initialization routines:
void initFrameBuffer(void);
void initFrameBufferDepthBuffer(void);
void initFrameBufferTexture(void);


#endif /* MAIN_H_ */
