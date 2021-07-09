#include <files.hpp>
#include <model.hpp>
#include <cam.hpp>

const u32 SCR_WIDTH  = 1280;
const u32 SCR_HEIGHT = 720;
const f32 ASPECT     = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

Cam* cam;

f32  deltaTime  = 0.0f;
f32  lastFrame  = 0.0f;
bool wireframe  = false;

void processInput(GLFWwindow* window) {
	//Increase movement velocity when SHIFT is pressed down
	f32 temp = cam->speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cam->speed *=2;
	//Shot to asterioids with space 
	//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		//cam->speed *=2;

	//Use arrow keys to move
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam->processKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cam->processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam->processKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cam->processKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || 
		glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
	//Return to normal velocity then 
	cam->speed = temp;
}
void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	} else {
		cam->stopPov();
	}
}
void scroll_callback(GLFWwindow*, f64, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

i32 main() {
	srand(time(NULL));
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Cubito");
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	cam = new Cam({0.0f, 0.0f, 0.0f});
	cam->speed = 7.5f;

	Files* files = new Files("bin", "resources/textures", "resources/objects");

	Shader* shader = new Shader(files, "shader.vert", "shader.frag");
	Shader* rockShader = new Shader(files, "rock.vert", "rock.frag");
	Model*  planet = new Model(files, "planet/planet.obj");
	Model*  rock = new Model(files, "rock/rock.obj");

	i32 limit_sup_x = 15, limit_inf_x = 0,
		limit_sup_y = 2, limit_inf_y = -2,
		limit_sup_z = 0, limit_inf_z = -25;

	u32 amount = 100;
	glm::mat4* models = new glm::mat4[amount];
	srand(glfwGetTime());
	//f32 radius = 25.0f;
	f32 offset = 2.f;
	for (u32 i = 0; i < amount; ++i) {
		glm::mat4 model = glm::mat4(1.0f);
		f32 x = rand() % limit_sup_x,
			y = rand() % (limit_sup_y-limit_inf_y+1) - 2,
			z = rand() % (-limit_inf_z) + 10;

		model = glm::translate(model, {x*offset, y*offset, -z*offset});
		model = glm::scale(model, glm::vec3(0.5));

		/*
		f32 angle = (f32)i / (f32)amount * 360.0f;
		f32 displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 x = sin(angle) * radius + displacement;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 y = displacement * 0.4f;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 z = cos(angle) * radius + displacement;
		model = glm::translate(model, {x, y, z});

		f32 scale = (rand() % 10) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		f32 rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, {0.4f, 0.6f, 0.8f});
		*/

		models[i] = model;
	}
	u32 buffer;
	const u32 smat4 = sizeof(glm::mat4);
	const u32 svec4 = sizeof(glm::vec4);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * smat4, &models[0], GL_STATIC_DRAW);

	for (u32 i = 0; i < rock->meshes.size(); ++i) {
		glBindVertexArray(rock->meshes[i].Vao);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, smat4, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, smat4, (void*)svec4);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, smat4, (void*)(2*svec4));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, smat4, (void*)(3*svec4));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	glm::vec3 lightPos   = glm::vec3(8.0f, 0.f, -1.f);
	glm::vec3 lightColor = glm::vec3(1.0f, 0.f,  0.f);

	glEnable(GL_DEPTH_TEST);

	srand(time(NULL));
	float dx=(rand()%10+1)/10.f, dy=(rand()%10+1)/10.f, dz=-(rand()%10+1)/10.f;;
	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;



		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE: GL_FILL);

		shader->use();

		//Check limit WALLS
		if (lightPos.x+dx> limit_sup_x *offset || lightPos.x+dx < limit_inf_x *offset)
			dx *= -1.f;
		if (lightPos.y+dy> limit_sup_y *offset || lightPos.y+dy < limit_inf_y *offset)
			dy *= -1.f;
		if (lightPos.z+dz> limit_sup_z *offset || lightPos.z+dz < limit_inf_z *offset)
			dz *= -1.f;

		f32 light_speed = 9.0f;
		lightPos.x += dx * deltaTime*light_speed;
		lightPos.y += dy * deltaTime*light_speed;
		lightPos.z += dz * deltaTime*light_speed;

		//std::cout << dx << dy << dz << std::endl;

		shader->setVec3("xyz", lightPos);
		shader->setVec3("xyzColor", lightColor);
		shader->setVec3("xyzView", cam->pos);
		glm::mat4 proj = glm::perspective(cam->zoom, ASPECT, 0.1f, 1000.0f);
		shader->setMat4("proj", proj);
		shader->setMat4("view", cam->getViewM4());

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
		//f32 scale = (rand() % 10) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(0.05));
		shader->setMat4("model", model);
		planet->Draw(shader);

		rockShader->use();
		rockShader->setMat4("proj", proj);
		rockShader->setMat4("view", cam->getViewM4());
		rockShader->setI32("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock->textures_loaded[0].id);
		for (u32 i = 0; i < rock->meshes.size(); ++i) {
			glBindVertexArray(rock->meshes[i].Vao);
			glDrawElementsInstanced(GL_TRIANGLES, rock->meshes[i].indices.size(),
					GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete cam;
	delete shader;
	delete planet;
	delete[] models;

	return 0;
}

/* vim: set tabstop=4:softtabstop=2:shiftwidth=2:noexpandtab */

