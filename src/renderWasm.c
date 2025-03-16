// Game Engine + Entity rendering (via Batch) --> first thing to do
// Game Loop (rpg side view)
// think about entry point (how, some JS will be required :())

#include "renderer.h"

// Processing headers
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

// Rendering headers
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/console.h>

void logInfo(char *str){
  emscripten_console_log(str);
}
#else
void logInfo(char *str){
  printf("%s\n", str);
}
#endif

#define frand() ((float) rand() / (float) (RAND_MAX))

const char *vertex_src =
  "#version 300 es\n"
  "layout (location = 0) in vec3 aPos;\n"
  "layout (location = 1) in vec2 aTexCoord;\n"
  "out vec2 TexCoord;\n"
  "void main()\n"
  "{\n"
  "   TexCoord = aTexCoord;\n"
  "   gl_Position = vec4(aPos, 1.0);\n"
  "}\0";

const char *frag_src =
  "#version 300 es\n"
  "precision mediump float;\n"
  "out vec4 FragColor;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D img;\n"
  "void main()\n"
  "{\n"
  "FragColor = texture(img, TexCoord);\n"
  "}\0";

static void _log(char *str){
  #ifdef __EMSCRIPTEN__
  emscripten_console_log(str);
  #else
  printf("%s\n", str);
  #endif
}

GLFWwindow *window = NULL;
Renderer renderer;
float clear_color[3] = {0.5f, 1.0f, 0.2f};

#ifdef __EMSCRIPTEN__
extern void randomize_bg_color();

EMSCRIPTEN_KEEPALIVE void randomize_bg_color(){
  float v;
  for(int i = 0; i < 3; i++){
    v = frand();
    clear_color[i] = v;
  }
}
#endif

void drawTriangle(Renderer *renderer){
  pushVertex(renderer, (Vertex){.buffer={-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}});
  pushVertex(renderer, (Vertex){.buffer={0.0f, 0.5f, 0.0f, 0.5f, 1.0f}});
  pushVertex(renderer, (Vertex){.buffer={0.5f, -0.5f, 0.0f, 1.0f, 0.0f}});
}

void drawTriangle2(Renderer *renderer){
  pushVertex(renderer, (Vertex){.buffer={-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}});
  pushVertex(renderer, (Vertex){.buffer={0.0f, -0.7f, 0.0f, 0.5f, 1.0f}});
  pushVertex(renderer, (Vertex){.buffer={0.0f, -1.0f, 0.0f, 1.0f, 0.0f}});
}

void _loop(){
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  drawTriangle(&renderer);
  drawTriangle2(&renderer);
  flushVertices(&renderer);

  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main(){
  srand(0);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(800, 600, "test WasmGL", NULL, NULL);

  glfwMakeContextCurrent(window);
  glViewport(0,0,800,600);

  GLuint wallTexture;
  createTexture("img/wall.jpg", &wallTexture);
  
  renderer = createRenderer(vertex_src, frag_src, 300);

  setTexture(&renderer, wallTexture);

  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(_loop, -1, 1);
  #else
  while(!glfwWindowShouldClose(window)){
    _loop();
  }
  #endif

  glfwTerminate();
  return 0;
}
