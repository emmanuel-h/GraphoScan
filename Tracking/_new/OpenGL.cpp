#include "OpenGL.hpp"

float mixValue = 0.5f;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera myCamera;
glm::mat4 view;
// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat yaw = -90.0f;

GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat aspect = 45.0f;

glm::vec3 cameraPos(0.0f, 0, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

bool keys[1024];

using namespace std;
using namespace glm;
using namespace cv;

GLfloat vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

GLfloat OfPlane[] =
  {
    //first triangle
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,

    //second triangle
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
  };

/************************/
//several cubes, position
glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

/************************/
//Texture
GLfloat texCoords[] = {
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.5f, 1.0f
};

void myOpenGL::InitWindow(){
  //GLFW is used to deal with the context
  //initialize glfw(the window)
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  
  //create the window
  window = glfwCreateWindow(1600, 800, "GraphoScan", nullptr, nullptr);
  if (window == nullptr)
    {
      cout << "Failed to create GLFW window" << endl;
      glfwTerminate();
      return;
    }

  // bind des fonctions d'evenements de la fenetre

  //bind the key call-back function key_callback with the window
  glfwSetKeyCallback(window, key_callback);
  
  //bind the mouse call-back function mouse_callback with the window
  glfwSetCursorPosCallback(window, mouse_callback);
  
  //bind the scroll call-back function scroll_callback with the window
  glfwSetScrollCallback(window, scroll_callback);
  
  //to inform the GLFW to create the window we defined
  glfwMakeContextCurrent(window);
  
  //this should be set GL_TURE for using the modern functions of opencv
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
    {
      cout << "Failed to initialize GLEW" << endl;
      return;
    }
  //call this function to define the window we look from
  glViewport(0, 0, 1600, 800);
}

void myOpenGL::InitVertex() {
  //settings
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  //initialisation du VAO2
  glGenVertexArrays(1, &VAO1);
  glBindVertexArray(VAO1);

  //shader1("vShader.txt", "fShader.txt");

  //vbo de taille 1
  // 0 = vertices
  glGenBuffers(1, &VBO1);

  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, 5*6*6*sizeof(float), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // unbind VertexArray pour utilisation de 2ème VAO
  glBindVertexArray(0);

  //initialisation du VAO2
  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);
  
  //shader2("vShader.txt", "fShader.txt");
  
  //vbo de taille 1
  // 0 = vertices
  glGenBuffers(1, &VBO2);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, 3*3*2*sizeof(float), OfPlane, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
  glEnableVertexAttribArray(0);

//capture the cursor and never lose it
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void myOpenGL::RunGL(const char* filename/*, const char* filename_bg*/) {
  vector<cv::Mat> v1;
  vector<cv::Mat> v2;
  //read the points from file
  ifstream fin;
  ifstream fin_bg;
  fin.open(filename);
  if (!fin.is_open())
    {
      cout << "Cannot open the file " << filename << endl;
      exit(1);
    }
  else
    {
      //lecture du fichier de points 3D
      for (int i = 0; fin.good(); i++)
	{
	  float xyzw[4];
	  fin >> xyzw[0];
	  fin >> xyzw[1];
	  fin >> xyzw[2];
	  fin >> xyzw[3];
	  //sur une ligne de points on prend les 4 valeurs
	  //on les met dans le tableau xyzw

	  //création de matrice 4 lignes 1 col et ajout en fin dans v1
	  cv::Mat M = cv::Mat_<float>(4, 1, xyzw);
	  v1.push_back(M.clone());
	}
    }
  //finde lecture du fichier
  fin.clear();
  fin.close();

  /*fin.open(filename_bg);
    if (!fin.is_open())
    {
    cout << "Cannot open the file " << filename_bg << endl;
    exit(1);
    }
    else
    {
    for (int i = 0; fin.good(); i++)
    {
    float xyzw[4];
    fin >> xyzw[0];
    fin >> xyzw[1];
    fin >> xyzw[2];
    fin >> xyzw[3];

    cv::Mat M = cv::Mat_<float>(4, 1, xyzw);
    v2.push_back(M.clone());
    }
    }
    fin.clear();
    fin.close(); 
  */
  int frameCount = 0;
  int n = 0;

  //Texture2D texture1("wall.jpg");
  //Texture2D texture2("baboon.jpg", 1);
  
  Shader shader1("vShader.txt", "fShader.txt");
  Shader shader2("vShader.txt", "fShader.txt");
  while (!glfwWindowShouldClose(window))
    {
      //we want to handle the events before painting.
      glfwPollEvents();//call this function to check if there are the events, for exemple, the key, are they pressed?
			
      //press key
      do_movement();
			
      //indicate that we want to clear this buffer: GL_COLOR_BUFFER_BIT
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /*Texture1*/
      //texture1.UseTexture2D(shader1.Program, "ourTexture1");
      //glActiveTexture(GL_TEXTURE0);
      //glBindTexture(GL_TEXTURE_2D, texture1);
      //glUniform1i(glGetUniformLocation(shader1.Program, "ourTexture1"), 0);
      /*Texture2*/
      //texture2.UseTexture2D(shader1.Program, "ourTexture2");
      //glActiveTexture(GL_TEXTURE1);
      //glBindTexture(GL_TEXTURE_2D, texture2);
      //glUniform1i(glGetUniformLocation(shader1.Program, "ourTexture2"), 1);

      /************************/
      // ===================
      // Transformation
      // ===================
      //glm::vec3 trans;
      //trans = glm::translate(trans, glm::vec3((GLfloat)sin(glfwGetTime()) / 2, 0.25f, 0.0f));
      //trans = glm::rotate(trans, glm::radians((GLfloat)sin(glfwGetTime()) / 2 * 50.0f), glm::vec3(1.0f, 0.0f, 1.0f));
      //trans = glm::scale(trans, glm::vec3(0.2f, 0.2f, 0.2f));
      /************************/

      
      //run the shader1 program
      shader1.Use();
		
      glUniform1f(glGetUniformLocation(shader1.programID, "mixvalue"), mixValue);
      //glUniform1f(glGetUniformLocation(shader1, "mixvalue"), mixValue);

      
      //GLfloat radius = 10.0f;
      //GLfloat camX = sin(glfwGetTime())*radius;
      //GLfloat camZ = cos(glfwGetTime())*radius;
			
      myView = myCamera.GetViewMatrix();
      //myView = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f,0.0f,0.0f), cameraUp);

      myProjection = myCamera.GetProjectionMatrix(HEIGHT, WIDTH);
      //myProjection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		
      //send view matrix 
      glUniformMatrix4fv(glGetUniformLocation(shader1.programID, "view"), 1, GL_FALSE, glm::value_ptr(myView));
		
      //send projection matrix
      glUniformMatrix4fv(glGetUniformLocation(shader1.programID, "projection"), 1, GL_FALSE, glm::value_ptr(myProjection));
      
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      //if shader program has benn well compiled, run it.
      //when we want to draw something, we should call VAO	
      glBindVertexArray(VAO1);

      /*for (int i = 0; i < v2.size(); i++)
	{
	GLfloat* ptr = v2[i].ptr<float>(0);
	GLfloat scale = 0.01f;
	GLfloat scale_1 = 0.1f;
	GLfloat inv_w = GLfloat(1.0f / ptr[3]);

	//scale *= inv_w;
	glm::vec3 v2 = glm::vec3((GLfloat)ptr[0] * scale_1 *inv_w, (GLfloat)ptr[1] * scale_1*inv_w, (GLfloat)ptr[2] * scale_1*inv_w);
	glm::vec3 v3 = glm::vec3((GLfloat)ptr[0], -(GLfloat)ptr[1], (GLfloat)ptr[2]);
	glm::vec3 v4 = glm::normalize(v2);

	glm::mat4 model;

	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(-1.0f, -1.0f, 0.0f));

	//1
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians((GLfloat)sin(glfwGetTime()) / 2 * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, v2);
	//model = glm::translate(model, glm::vec3((GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime())));
	model = glm::scale(model, glm::vec3(scale, scale, scale));



	//GLfloat* ptr = v2[i].ptr<float>(0);
	//GLfloat scale = 0.01f;
	//GLfloat scale_1 = 0.1f;
	//GLfloat inv_w = GLfloat(1.0f / ptr[3]);

	////scale *= inv_w;
	//glm::vec3 v2 = glm::vec3(-(GLfloat)ptr[0] * scale_1 *inv_w, -(GLfloat)ptr[1] * scale_1*inv_w, (GLfloat)ptr[2] * scale_1*inv_w);
	//glm::vec3 v3 = glm::vec3((GLfloat)ptr[0], -(GLfloat)ptr[1], (GLfloat)ptr[2]);
	//glm::vec3 v4 = glm::normalize(v2);

	//glm::mat4 model;

	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(-1.0f, -1.0f, 0.0f));
	//model = glm::translate(model, v2);
	//model = glm::translate(model, glm::vec3((GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime())));
	//model = glm::scale(model, glm::vec3(scale, scale, scale));

	//send model matrix
	glUniformMatrix4fv(glGetUniformLocation(shader1.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//draw something...
	/******************************/
      //glDrawArrays(GL_TRIANGLES, 0, 36);
      //draw a triangle
      //param 1:the basic graph that we wanna draw
      //param 2:the index of the position of the vertex that we will start with in the vertex table
      //param 3:the number of the vertex that we want to draw
      /*****************************/

      //			}

      for (int i = 0; i < frameCount; i++)
	{
	  if (frameCount >= v1.size())
	    {
	      cout << "frameCount > v1.size." << endl;
	      exit(1);
	    }
	  
	  GLfloat* ptr = v1[i].ptr<float>(0);
	  GLfloat scale = 0.01f;
	  GLfloat scale_1 = 0.01f;
	  GLfloat inv_w = GLfloat(1.0f/ptr[3]);

	  //scale *= inv_w;
	  glm::vec3 v2 = glm::vec3((GLfloat)ptr[0] * scale_1 *inv_w, (GLfloat)ptr[1] * scale_1*inv_w, (GLfloat)ptr[2] * scale_1*inv_w);
	  glm::vec3 v3 = glm::vec3((GLfloat)ptr[0], -(GLfloat)ptr[1], (GLfloat)ptr[2]);
	  glm::vec3 v4 = glm::normalize(v2);
				
	  glm::mat4 model;

	  //model = glm::rotate(model, glm::radians(180.0f), glm::vec3(-1.0f, -1.0f, 0.0f));
					
	  //1
	  model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 1.0f, 0.0f));
	  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	  //model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	  model = glm::translate(model, v2);
	  //model = glm::translate(model, glm::vec3((GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime())));		
	  model = glm::scale(model, glm::vec3(scale, scale, scale));

	  /*GLfloat* ptr = v1[i].ptr<float>(0);
	  GLfloat scale = 0.01f;
	  GLfloat scale_1 = 0.001f;
	  GLfloat inv_w = GLfloat(1.0f / ptr[3]);

	  scale *= inv_w;
	  glm::vec3 v2 = glm::vec3(-(GLfloat)ptr[0] * scale_1 *inv_w, -(GLfloat)ptr[1] * scale_1*inv_w, (GLfloat)ptr[2] * scale_1*inv_w);
	  glm::vec3 v3 = glm::vec3((GLfloat)ptr[0], -(GLfloat)ptr[1], (GLfloat)ptr[2]);
	  glm::vec3 v4 = glm::normalize(v2);

	  glm::mat4 model;

	  model = glm::rotate(model, glm::radians(180.0f), glm::vec3(-1.0f, -1.0f, 0.0f));
	  model = glm::translate(model, v2);
	  model = glm::translate(model, glm::vec3((GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime()), (GLfloat)sin(glfwGetTime())));		
	  model = glm::scale(model, glm::vec3(scale, scale, scale));
	  */
	  
	  //send model matrix
	  glUniformMatrix4fv(glGetUniformLocation(shader1.programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	  //glUniformMatrix4fv(glGetUniformLocation(shader1, "model"), 1, GL_FALSE, glm::value_ptr(model));
	  
	  //draw something...
	  /******************************/
	  glDrawArrays(GL_TRIANGLES, 0, 6*6);
	  //draw a triangle
	  //param 1:the basic graph that we wanna draw
	  //param 2:the index of the position of the vertex that we will start with in the vertex table
	  //param 3:the number of the vertex that we want to draw
	  /*****************************/

	}
      if ((frameCount < v1.size() - 1)){
	frameCount++;
      }
      n++;

	
      //if (frameCount >= v1.size())
      //{
      //	cout << "frameCount > v1.size." << endl;
      //	exit(1);
      //}
      //float* ptr = v1[frameCount].ptr<float>(0);
      //if (n % 5 == 0)
      //	frameCount++;
      //n++;
      ////std::cout << "n = " << n << endl;
      //GLfloat scale = 0.01f;
      //GLfloat scale_1 = 0.001f;
      //GLfloat inv_w = GLfloat(1.0f / ptr[3]);

      ////scale *= inv_w;
      //glm::vec3 v2 = glm::vec3((GLfloat)ptr[0] * scale_1 *inv_w, (GLfloat)ptr[1] * scale_1*inv_w, (GLfloat)ptr[2] * scale_1*inv_w);
      //glm::vec3 v3 = glm::vec3(-(GLfloat)ptr[0], (GLfloat)ptr[1], (GLfloat)ptr[2]);
      //glm::vec3 v4 = glm::normalize(v3);

      //glm::mat4 model;
      //model = glm::translate(model, v2);
      //model = glm::scale(model, glm::vec3(scale, scale, scale));

      ////send model matrix
      //glUniformMatrix4fv(glGetUniformLocation(shader1.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
      //
      ////draw something...
      ///******************************/
      //glDrawArrays(GL_TRIANGLES, 0, 36);
      ////draw a triangle
      ////param 1:the basic graph that we wanna draw
      ////param 2:the index of the position of the vertex that we will start with in the vertex table
      ////param 3:the number of the vertex that we want to draw
      ///*****************************/

      glBindVertexArray(VAO2);

      glm::mat4 model;
      model = glm::rotate(model, glm::radians(180.0f), glm::vec3(-1.0f, -1.0f, 0.0f));

      glUniformMatrix4fv(glGetUniformLocation(shader2.programID, "view"), 1, GL_FALSE, glm::value_ptr(myView));
      
      glUniformMatrix4fv(glGetUniformLocation(shader2.programID, "projection"), 1, GL_FALSE, glm::value_ptr(myProjection));
       
       glUniformMatrix4fv(glGetUniformLocation(shader1.programID, "model"), 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 6);

      glBindVertexArray(0);


      //glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
      //for (GLuint i = 0; i < 10; i++)
      //{
      //	glm::mat4 model;
      //	model = glm::translate(model, cubePositions[i]);
      //	GLfloat angle = 20.0f*i;
      //	if (i % 3 == 0)
      //		angle = glfwGetTime()*25.0f;
      //	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      //	glUniformMatrix4fv(glGetUniformLocation(shader1.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
      //	glDrawArrays(GL_TRIANGLES, 0, 36);
      //}
      //glDrawArrays(GL_TRIANGLES, 0, 36);

      glBindVertexArray(0);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
      //after drawing, unfasten the VAO again
      //this function should be put here, at the end of the loop.
      glfwSwapBuffers(window);//call this function to swap the buffer which is storing the data
    };

  glDeleteVertexArrays(1, &VAO1);
  glDeleteBuffers(1, &VBO1);

  glfwTerminate();//to release the memory
  return;
}

//key call-back function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  
  if (action == GLFW_PRESS)
    {
      keys[key] = true;
    }
  else if (action == GLFW_RELEASE)
    {
      keys[key] = false;
    }
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
  if (firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
  GLfloat xoffset = xpos - lastX;
  GLfloat yoffset = lastY - ypos; //here it's inverse
  lastX = xpos;
  lastY = ypos;
  myCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
  myCamera.ProcessMouseScroll(yoffset);
}

void do_movement()
{
  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  if (keys[GLFW_KEY_W])
    {
      cout << "W" << endl;
      myCamera.ProcessKeyboard(FORWARD, deltaTime);
    }
  if (keys[GLFW_KEY_S])
    {
      cout << "S" << endl;
      myCamera.ProcessKeyboard(BACKWARD, deltaTime);
    }
  if (keys[GLFW_KEY_A])
    {
      cout << "A" << endl;
      myCamera.ProcessKeyboard(LEFT, deltaTime);
    }
  if (keys[GLFW_KEY_D])
    {
      cout << "W" << endl;
      myCamera.ProcessKeyboard(RIGHT, deltaTime);
    }
}
