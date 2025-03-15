#ifndef TEST_H
#define TEST_H

#include <GLES3/gl3.h>
#include <stdlib.h>

/* var for information about glFunctions */
#define BUFF_INFO_SIZE 1024
#define VERTEX_SIZE 5

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
} Renderer;

void logInfo(char *str);

Renderer createRenderer(const char *vertex, const char *frag, int vertex_capacity);
void pushVertex(Renderer *renderer, Vertex vertex);
void setTexture(Renderer *renderer, GLuint textureId);
void flushVertices(Renderer *renderer);

void createTexture(const char *src, GLuint *texture);

#endif
