// Game Engine + Entity rendering (via Batch) --> first thing to do
// Game Loop (rpg side view)
// think about entry point (how, some JS will be required :())

// Processing headers
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

// Rendering headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "renderer.h"

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

typedef struct _slime {
  float x, y;
  int frame;
} Slime;

const char *vertex_src =
  "#version 300 es\n"
  "uniform mat4 mvp;\n"
  "layout (location = 0) in vec3 aPos;\n"
  "layout (location = 1) in vec2 aTexCoord;\n"
  "out vec2 TexCoord;\n"
  "void main()\n"
  "{\n"
  "   TexCoord = aTexCoord;\n"
  "   gl_Position = mvp * vec4(aPos, 1.0);\n"
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

GLFWwindow *window = NULL;
Renderer renderer;
Texture slimeText;
float clear_color[3] = {0.5f, 1.0f, 0.2f};
Slime mob[20];
int currentMob = 0;

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

void printMatrix(char row, char col, float *values){
  int j = 0;
  for(int i = 0; i < (row * col); i++){
    if(i != 0 && i % col == 0){
      j++;
      printf("\n");
    }
    printf("%.3f ", values[i%col * 4 + j]);
  }
}

void drawSlime(Renderer *renderer, Texture *texture, float x, float y){
  float sx, sy;

  sx = 17.f/(texture->width);
  sy = 17.f/(texture->height);

  float size = 64.f;

  pushVertex(renderer, (float[]){x, y, 0.0f, 0.0f, 0.0f});
  pushVertex(renderer, (float[]){x + size, y, 0.0f, 0.2f, 0.0f});
  pushVertex(renderer, (float[]){x, y + size, 0.0f, 0.0, 1.0f});

  pushVertex(renderer, (float[]){x, y + size, 0.0f, 0.0f, 1.0f});
  pushVertex(renderer, (float[]){x + size, y, 0.0f, 0.2f, 0.0f});
  pushVertex(renderer, (float[]){x + size, y + size, 0.f, 0.2f, 1.0f});
}

void _loop(){
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); /* wireframe mode */

  for(int i = 0; i < currentMob; i++)
    drawSlime(&renderer, &slimeText, mob[i].x, mob[i].y);

  //drawTriangle(&renderer);
  //drawTriangle2(&renderer);

  flushVertices(&renderer);

  glfwSwapBuffers(window);
  glfwPollEvents();
}

float * genOrthMatrix(float left, float right, float bottom, float top, float znear, float zfar){

  float *m = (float *) calloc(16, sizeof(float));
  
  float rl = 1.0f / (right - left);
  float tb = 1.0f / (top - bottom);
  float fn = -1.0f/ (zfar - znear);

  // column major order
  m[0] = 2.0f * rl;
  m[1 * 4 + 1] = 2.0f * tb;
  m[2 * 4 + 2] = 2.0f * fn;

  m[4 * 3] = - (right + left) * rl;
  m[4 * 3 + 1] = - (top + bottom) * tb;
  m[4 * 3 + 2] = (zfar + znear) * fn;
  m[4 * 3 + 3] = 1.0f;

  return m;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    if(currentMob < 19){
      mob[currentMob].x = frand() * 800;
      mob[currentMob].y = frand() * 600;
      currentMob++;
    }
      

  if(key == GLFW_KEY_A && action == GLFW_PRESS)
    if(currentMob > 0)
      currentMob--;

  printf("currentMob : %d && (%d, %d)\n", currentMob, key, action);
}

int main(){
  srand(0);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(800, 600, "test WasmGL", NULL, NULL);

  glfwMakeContextCurrent(window);

  /* Load the glad process to link functions */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }   
  
  glViewport(0,0,800,600);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  
  createTexture("img/slime.png", &slimeText); //check path ???

  float *matrix = genOrthMatrix(0, 800.f, 600.f, 0, -1.0f, 1.0f);

  renderer = createRenderer(vertex_src, frag_src, 300);

  setTexture(&renderer, slimeText.textureId);
  setMatrix(&renderer, "mvp", matrix);

  glfwSetKeyCallback(window, keyCallback);
  
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
