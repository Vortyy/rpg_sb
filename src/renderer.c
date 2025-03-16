#include "renderer.h"

char bufferInfo[BUFF_INFO_SIZE];
int success;

GLuint loadShader(GLuint type, const char *src){
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &src, NULL);
  glCompileShader(shaderId);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

  if(!success){
    glGetShaderInfoLog(shaderId, 1024, NULL, bufferInfo);
    logInfo(bufferInfo);
  }

  return shaderId;
}

GLuint shaderProgram(const char *vertex, const char *frag){
  GLuint vertex_shader = loadShader(GL_VERTEX_SHADER, vertex);
  GLuint frag_shader = loadShader(GL_FRAGMENT_SHADER, frag);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertex_shader);
  glAttachShader(shaderProgram, frag_shader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shaderProgram, 1024, NULL, bufferInfo);
    logInfo(bufferInfo);
  }

  return shaderProgram;
}

Renderer createRenderer(const char *vertex, const char *frag, int vertex_capacity){
  GLuint shaderProgr = shaderProgram(vertex, frag);
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_capacity * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return (Renderer){
    .shaderProgr = shaderProgr,
    .vao = vao,
    .vbo = vbo,
    .vertex_count = 0,
    .vertex_capacity = vertex_capacity,
    .vertices = malloc(sizeof(Vertex) * vertex_capacity),
    .texture = 0,
    .mvp = 0,
  };
}

void flushVertices(Renderer *renderer){
  if(renderer->vertex_count == 0)
    return;

  glUseProgram(renderer->shaderProgr);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->texture);

  glUniformMatrix4fv(glGetUniformLocation(renderer->shaderProgr, "mvp"), 1, GL_FALSE, renderer->mvp);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->vertex_count, renderer->vertices);

  glBindVertexArray(renderer->vao);
  glDrawArrays(GL_TRIANGLES, 0, renderer->vertex_count);

  renderer->vertex_count = 0;
}

void pushVertex(Renderer *renderer, Vertex vertex){
  if(renderer->vertex_count == renderer->vertex_capacity)
    flushVertices(renderer);

  *(renderer->vertices + renderer->vertex_count) = vertex;
  renderer->vertex_count++;
}

void setTexture(Renderer *renderer, GLuint textureId){
  if(renderer->texture != textureId){
    flushVertices(renderer);
    renderer->texture = textureId;
  }
}

void setMatrix(Renderer *renderer, const char *fctName, const char *uniformName, float *values){
   flushVertices(renderer);
   renderer->mvp = values;
}

// --------- texture loading ------------//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void createTexture(const char *src, GLuint *texture){
  int width, height, nr_channels;
  unsigned char *data = stbi_load(src, &width, &height, &nr_channels, 0);
  if(!data)
    logInfo("error while loading texture");
  else
  {
    bufferInfo[0] = '\0'; //resetting bufferInfo
    sprintf(bufferInfo, "image loaded : (%d, %d, %d)", width, height, nr_channels);
    logInfo(bufferInfo);
  }

  glGenTextures(1, texture);
  
  glBindTexture(GL_TEXTURE_2D, *texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
}
