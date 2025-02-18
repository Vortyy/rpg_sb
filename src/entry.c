#include<stdio.h>
#include<stdlib.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

/*
 * ceci est un test pour voir ce que ca donne
 * */
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glViewport(0, 0,  width, height);
}

int main(int argc, char **argv){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
  if(window == NULL){
    printf("Failed to create window");
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("Failed to initialize GLAD\n");
    return -1;
  }  
  
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  while(!glfwWindowShouldClose(window)){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}
