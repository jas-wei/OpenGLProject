#include <iostream>

// Must be in this order
#include <glad/glad.h> 
#include <glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom header files
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "stb_image.h"

float mixValue = 0.0f;

// Camera variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400;
float lastY = 300;

bool firstMouse = true;
float deltaTime = 0.0f;
float prevFrame = 0.0f;



// Refreshes viewport to match frame buffer size (usually window dimentions)
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Initializes last x and y mouse pos 
	if (firstMouse)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Calculate x and y offset
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProccessMouseScroll(yoffset);
}


//Checks if escape is pressed and closes window
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//firstMouse = true;
	}
		

	// Texture mixing
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
		mixValue += 0.1;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
		mixValue -= 0.1;

	// Keyboard Camera Movements
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}


void loadTexture(int textureID, int textureUnitOffset, const char* texturePath, unsigned int TextureOption, int sourceFormat, int sourceDataType) {

	glActiveTexture(GL_TEXTURE0 + textureUnitOffset); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, textureID);			  // place the texture at textureID into the currently active texture unit

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image data into an array and populates ints tracking the image's width, height, and number of colour channels
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

	if (data) {
		// Binds image data to the current texture target
		// Params: Texture target, mip level, format, width, height, 0, source format, source datatype, image data
		glTexImage2D(GL_TEXTURE_2D, 0, sourceFormat, width, height, 0, sourceFormat, sourceDataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture data" << '\n';
	}
	
	stbi_image_free(data);
}


int main()
{
	// Starts up the GLFW library
	glfwInit();


	///////// WINDOW SETUP ///////////


	// Defining settings for the future window
	// Major means number before the decimal and minor means the number after the decimal (ex. OpenGL 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request the OpenGL Core Profile (modern OpenGL)

	// Specify dimentions and name of the window 
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Use GLAD to load OpenGL function pointers from the driver
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	// Creates the viewport (different from GLFW window)
	glViewport(0, 0, 800, 600);

	// Registers framebuffer_size_callback function to resize the viewport whenever the framebuffer size changes
	// Like a wrapper, the function you pass in glfwSetFramebufferSizeCallback must always have *window, width, and height params to work
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	///////// VERTEX DATA ///////////


	float vertices[] = {
		// positions              // tex coords

		// Front (+Z)
		-0.5f,-0.5f, 0.5f,   0.0f,0.0f, //0
		 0.5f,-0.5f, 0.5f,   1.0f,0.0f, //1
		 0.5f, 0.5f, 0.5f,   1.0f,1.0f, //2
		-0.5f, 0.5f, 0.5f,   0.0f,1.0f, //3

		// Back (-Z)
		 0.5f,-0.5f,-0.5f,   0.0f,0.0f, //4
		-0.5f,-0.5f,-0.5f,   1.0f,0.0f, //5
		-0.5f, 0.5f,-0.5f,   1.0f,1.0f, //6
		 0.5f, 0.5f,-0.5f,   0.0f,1.0f, //7

		 // Left (-X)
		 -0.5f,-0.5f,-0.5f,   0.0f,0.0f, //8
		 -0.5f,-0.5f, 0.5f,   1.0f,0.0f, //9
		 -0.5f, 0.5f, 0.5f,   1.0f,1.0f, //10
		 -0.5f, 0.5f,-0.5f,   0.0f,1.0f, //11

		 // Right (+X)
		  0.5f,-0.5f, 0.5f,   0.0f,0.0f, //12
		  0.5f,-0.5f,-0.5f,   1.0f,0.0f, //13
		  0.5f, 0.5f,-0.5f,   1.0f,1.0f, //14
		  0.5f, 0.5f, 0.5f,   0.0f,1.0f, //15

		  // Bottom (-Y)
		  -0.5f,-0.5f,-0.5f,   0.0f,0.0f, //16
		   0.5f,-0.5f,-0.5f,   1.0f,0.0f, //17
		   0.5f,-0.5f, 0.5f,   1.0f,1.0f, //18
		  -0.5f,-0.5f, 0.5f,   0.0f,1.0f, //19

		  // Top (+Y)
		  -0.5f, 0.5f, 0.5f,   0.0f,0.0f, //20
		   0.5f, 0.5f, 0.5f,   1.0f,0.0f, //21
		   0.5f, 0.5f,-0.5f,   1.0f,1.0f, //22
		  -0.5f, 0.5f,-0.5f,   0.0f,1.0f  //23
	};

	unsigned int indices[] = {
		// Front
		0,1,2,
		2,3,0,

		// Back
		4,5,6,
		6,7,4,

		// Left
		8,9,10,
		10,11,8,

		// Right
		12,13,14,
		14,15,12,

		// Bottom
		16,17,18,
		18,19,16,

		// Top
		20,21,22,
		22,23,20
	};

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

	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO); // params: number of VAOs to generate, array of VAOs
	glGenBuffers(1, &VBO); // params: number of buffers to generate, array of buffers
	glGenBuffers(1, &EBO);

	// Bind the VAO so the following VBO/EBO and attribute setup is stored in it
	glBindVertexArray(VAO);
	
	// Create the vertex buffer, a memory block that stores all of the vertex info (pos, uvs, etc) in an alternating pattern per vertex
	// Bind it to be activly used, and copy vertex data into it (so far only pos)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an element buffer object, a memory block that stores indices that tell OpenGL which vertices to reuse
	// Bind it to be activly used, and copy indice;s data into it 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Create a vertex array object, a memory block that stores how vertex attributes should be read from the VBO
	// glVertexAttribPointer assigns the attribute mapping (pos, uvs, etc) for the actively bound VAO 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // params: location, vector size, datatype, isNormalized, stride, offset
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float))); // params: location, vector size, datatype, isNormalized, stride, offset
	glEnableVertexAttribArray(1);

	//test


	///////// SHADERS ///////////


	Shader ourShader("shader.vs", "shader.fs");


	///////// TEXTURES ///////////

	Texture tex1, tex2 = Texture();
	
	tex1.bind(0);
	tex1.loadTexture("textures/container.jpg", GL_REPEAT, GL_RGB, GL_UNSIGNED_BYTE);

	tex2.bind(1);
	tex2.loadTexture("textures/cat.png", GL_REPEAT, GL_RGB, GL_UNSIGNED_BYTE);

	ourShader.use(); // don't forget to activate the shader before setting uniforms!  
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);


	///////// TRANSFORMATIONS ///////////
	

	// Setting cursor to be invisible for that window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Configuring input callbacks
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scrollCallback);



	///////// RENDER LOOP ///////////

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Clear the color and depth buffers, filling color buffer with a placeholder teal
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update the green value with respect to time
		float timeValue = glfwGetTime();
		float currentFrame = timeValue;
		deltaTime = currentFrame - prevFrame;
		prevFrame = currentFrame;

		// Set dynamic values to  uniforms via the custom shader class
		ourShader.use();
		ourShader.setFloat("positionOffset", sin(timeValue));
		ourShader.setFloat("timeValue", timeValue);
		ourShader.setFloat("mixValue", mixValue);

		glm::mat4 viewMatrix;
		viewMatrix = camera.GetViewMatrix();

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.zoom), (float)800 / (float)600, 0.1f, 100.0f);

		// Send data to the uniforms
		ourShader.use();
		unsigned int modelMatrixLocation = glGetUniformLocation(ourShader.ID, "modelMatrix");
		unsigned int viewMatrixLocation = glGetUniformLocation(ourShader.ID, "viewMatrix");
		unsigned int projectionMatrixLocation = glGetUniformLocation(ourShader.ID, "projectionMatrix");
		
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// Set texture units to active and bind them to the active texture
		tex1.bind(0);
		tex2.bind(1);

		// Draw cube from vertex element
		glBindVertexArray(VAO);
		for (int i = 0; i < 10; i++) {

			// Establish model matrix within the redner loop, since it updates
			float angle = 20 * i;
			glm::mat4 modelMatrix = glm::mat4(1.0f); //identity matrix
			modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
			modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(timeValue) * angle, {1.0f, 0.3f, 0.5f});
			
			// Send new model matrix to model matrix's uniform
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		
		glBindVertexArray(0); //Remove binding

		glfwSwapBuffers(window); // using double buffer prevents flickering
		glfwPollEvents(); //check for events
	}

	glfwTerminate();
	
	return 0;
}


