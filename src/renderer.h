#ifndef RENDERER_H
#define RENDERER_H

// Inner reason that we need to use glad.h
// Cannot just link gl.so (HINT: WINDOWS is shitty)
// there: https://stackoverflow.com/questions/34662134/why-is-opengl-designed-in-a-way-that-the-actual-functions-have-to-be-loaded-manu
#include <glad/glad.h>
#include <stdlib.h>

/* var for information about glFunctions */
#define BUFF_INFO_SIZE 1024
#define VERTEX_SIZE 5

#define GL_UNIFORM_MATRIX(fctName, location, values) glUniformMatrix ## fctName ## fv\
  (location, 1, GL_FALSE, values)

typedef struct _Vertex {
  float buffer[VERTEX_SIZE];
} Vertex;

typedef struct _Renderer {
  GLuint shaderProgr;
  GLuint texture;

  GLuint vao;
  GLuint vbo;

  int vertex_capacity;
  int vertex_count;
  
  Vertex *vertices;
  float *mvp;
} Renderer;

void logInfo(char *str);

Renderer createRenderer(const char *vertex, const char *frag, int vertex_capacity);
void pushVertex(Renderer *renderer, Vertex vertex);
void setTexture(Renderer *renderer, GLuint textureId);
void setMatrix(Renderer *renderer, const char *fctName, const char *uniformName, float *values);
void flushVertices(Renderer *renderer);

void createTexture(const char *src, GLuint *texture);

#endif //RENDERER_H
