// Entity rendering (via Batch) --> first thing to do
// Game Loop (rpg side view)
// think about entry point (how, some JS will be required :()
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Rendering headers
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>
//#include <emscripten.h>
//#include <emscripten/console.h>

// Processing headers
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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
  "precision mediump float;"
  "out vec4 FragColor;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D img;\n"
  "void main()\n"
  "{\n"
    "FragColor = texture(img, TexCoord);\n"
  "}\0";

float vertices[15] = {
  // position        //texture
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
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
    //  emscripten_console_log(buffer);
  }

  return shader_id;
}

void gen_bind_texture(unsigned int text, unsigned char *data, int width, int height){

}
  
GLFWwindow *window = NULL;
float clear_color[3] = {0.5f, 1.0f, 0.2f};
unsigned int shader_program;
unsigned int VAO;
unsigned int TEXT;

extern void randomize_bg_color();

/*
EMSCRIPTEN_KEEPALIVE void randomize_bg_color(){
  float v;
  for(int i = 0; i < 3; i++){
    v = frand();
    clear_color[i] = v;
  }
}
*/

void _loop(){
  glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  glBindTexture(GL_TEXTURE_2D, TEXT);

  glUseProgram(shader_program);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);

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

  unsigned vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_src);
  unsigned frag_shader = load_shader(GL_FRAGMENT_SHADER, frag_src);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);

  char str[512];
  int success;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shader_program, 512, NULL, str);
    //emscripten_console_log(str);
  }
  
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  //TEXTURE
  glActiveTexture(GL_TEXTURE0);

  int width, height, nr_channels;
  unsigned char *data = stbi_load("src/wall.jpg", &width, &height, &nr_channels, 0);
  if(!data)
    //emscripten_console_log("error while loading");
    printf("error");
  else
  {
    char buffer[100];
    sprintf(buffer, "image loaded : (%d, %d, %d)", width, height, nr_channels);
    printf("%s\n", buffer);
    //emscripten_console_log(buffer);
  }

  glGenTextures(1, &TEXT);
  
  glBindTexture(GL_TEXTURE_2D, TEXT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  unsigned int textUni = glGetUniformLocation(shader_program, "img");
  glUniform1i(textUni, 0);
  
  //emscripten_set_main_loop(_loop, -1, 1);

  while(!glfwWindowShouldClose(window)){
    _loop();
  }

  glfwTerminate();
  return 0;
}
