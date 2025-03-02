// Rendering headers
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
#include <SDL/SDL.h>
#include <emscripten.h>
#include <emscripten/console.h>

// Processing headers
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define frand() ((float) rand() / (float) (RAND_MAX))

const char *vertex_src =
  "#version 300 es\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main()\n"
  "{\n"
  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}\0";

const char *frag_src =
  "#version 300 es\n"
  "precision mediump float;"
  "out vec4 FragColor;\n"
  "void main()\n"
  "{\n"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}\0";

float vertices[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.5f, 0.0f
};

static unsigned load_shader(int type, const char *src){
  unsigned shader_id = glCreateShader(type);
  glShaderSource(shader_id, 1, &src, NULL);
  glCompileShader(shader_id);

  int success;
  char info_log[512];
  char buffer[1024];
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

  if(!success){
    glGetShaderInfoLog(shader_id, 512, NULL, info_log);
    sprintf(buffer, "error shader compilation: %s", info_log);
    emscripten_console_log(buffer);
  }

  emscripten_console_log("compilation shader succeed");

  return shader_id;
}
  
GLFWwindow *window = NULL;
float clear_color[3] = {0.5f, 1.0f, 0.2f};
unsigned int shader_prog = -1;
unsigned int VAO = -1;

extern void randomize_bg_color();

EMSCRIPTEN_KEEPALIVE void randomize_bg_color(){
  float v;
  for(int i = 0; i < 3; i++){
    v = frand();
    clear_color[i] = v;
  }
}

void _loop(){
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shader_prog);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);

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

  unsigned vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_src);
  unsigned frag_shader = load_shader(GL_FRAGMENT_SHADER, frag_src);

  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, vertex_shader);
  glAttachShader(shader_prog, frag_shader);
  glLinkProgram(shader_prog);

  char str[512];
  int success;
  glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shader_prog, 512, NULL, str);
    emscripten_console_log(str);
  }

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  emscripten_set_main_loop(_loop, -1, 1);

  glfwTerminate();
  return 0;
}
