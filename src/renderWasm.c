// Rendering headers
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include <SDL/SDL.h>
#include <emscripten.h>

// Processing headers
#include <time.h>
#include <stdlib.h>

#define frand() ((float) rand() / (float) (RAND_MAX))

GLFWwindow *window = NULL;
float clear_color[3] = {0.5f, 1.0f, 0.2f};

extern void randomize_bg_color();

void randomize_bg_color(){
  float v;
  for(int i = 0; i < 3; i++){
    v = frand();
    clear_color[i] = v;
  }
}

void _loop(){
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwSwapBuffers(window);
  glfwPollEvents();
}
  
int main(){
  srand(0);
  
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  
  window = glfwCreateWindow(800, 600, "test WasmGL", NULL, NULL);
  
  glfwMakeContextCurrent(window);
  glViewport(0,0,800,600);

  emscripten_set_main_loop(_loop, -1, 1);
  
  glfwTerminate();
  return 0;
}
