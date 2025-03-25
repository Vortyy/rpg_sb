#include "renderer.h"

static char bufferInfo[BUFF_INFO_SIZE]; /* used to display OpenGl log */
static int success;                     /* return status of OpenGl */

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

  glDeleteShader(vertex_shader);
  glDeleteShader(frag_shader);

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
  glBufferData(GL_ARRAY_BUFFER, vertex_capacity * VERTEX_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*) 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * VERTEX_SIZE, (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return (Renderer){
    .shaderProgr = shaderProgr,
    .vao = vao,
    .vbo = vbo,
    .vertex_count = 0,
    .vertex_capacity = vertex_capacity,
    .vertices = (float *) malloc(sizeof(float) * VERTEX_SIZE * vertex_capacity),
    .texture = 0,
  };
}

void flushVertices(Renderer *renderer){
  if(renderer->vertex_count == 0)
    return;

  glUseProgram(renderer->shaderProgr);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->texture);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * VERTEX_SIZE * renderer->vertex_count, renderer->vertices);

  glBindVertexArray(renderer->vao);
  glDrawArrays(GL_TRIANGLES, 0, renderer->vertex_count);

  renderer->vertex_count = 0;
}

void pushVertex(Renderer *renderer, float vertex[VERTEX_SIZE]){
  if(renderer->vertex_count == renderer->vertex_capacity)
    flushVertices(renderer);

  float * vAddr = renderer->vertices + (renderer->vertex_count * VERTEX_SIZE);
  memcpy(vAddr, vertex, sizeof(float) * VERTEX_SIZE);
  
  renderer->vertex_count++;
}

void setTexture(Renderer *renderer, GLuint textureId){
  if(renderer->texture != textureId){
    flushVertices(renderer);
    renderer->texture = textureId;
  }
}

void setMatrix(Renderer *renderer, const char *uniName, float *values){
  flushVertices(renderer);

  //WARNING wrong uniName will be siltlenty ignored by opengl since loc=-1
  glUseProgram(renderer->shaderProgr);
  GLint location = glGetUniformLocation(renderer->shaderProgr, uniName);
  glUniformMatrix4fv(location, 1, GL_FALSE, values);
  
  GLenum err;
  if((err = glGetError()) != GL_NO_ERROR){
    bufferInfo[0] = '\0';
    sprintf(bufferInfo, "error: uniform matrix setting code=0x%x", err);
    logInfo(bufferInfo);
  }
    
  glUseProgram(0);
}

// --------- texture loading ------------//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void createTexture(const char *src, Texture *texture){
  unsigned char *data = stbi_load(src, &texture->width, &texture->height, &texture->nbChannel, 0);
  if(!data)
    logInfo("error while loading texture");
  else
  {
    bufferInfo[0] = '\0'; //resetting bufferInfo
    sprintf(bufferInfo, "image loaded : (%d, %d, %d)", texture->width, texture->height, texture->nbChannel);
    logInfo(bufferInfo);
  }

  glGenTextures(1, &texture->textureId);
  
  glBindTexture(GL_TEXTURE_2D, texture->textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
}
