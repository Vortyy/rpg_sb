#ifndef RENDERER_H
#define RENDERER_H

// Inner reason that we need to use glad.h
// Cannot just link gl.so (HINT: WINDOWS is shitty)
// there: https://stackoverflow.com/questions/34662134/why-is-opengl-designed-in-a-way-that-the-actual-functions-have-to-be-loaded-manu
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* var for information about glFunctions */
#define BUFF_INFO_SIZE 1024
#define VERTEX_SIZE 5

typedef struct _Renderer {
  GLuint shaderProgr;
  GLuint texture;

  GLuint vao;
  GLuint vbo;

  int vertex_capacity;
  int vertex_count;
  
  float *vertices;
} Renderer;

void logInfo(char *str);

Renderer createRenderer(const char *vertex, const char *frag, int vertex_capacity);
void pushVertex(Renderer *renderer, float vertex[VERTEX_SIZE]);
void setTexture(Renderer *renderer, GLuint textureId);
void setMatrix(Renderer *renderer, const char *uniName, float *values);
void flushVertices(Renderer *renderer);

void createTexture(const char *src, GLuint *texture);

#endif //RENDERER_H
