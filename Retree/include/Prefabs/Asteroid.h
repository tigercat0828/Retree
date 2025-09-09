#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Transform.h"
#include "Core/AssimpObject.h"
#include "Core/ResourceManager.h"
#include "Core/Camera.h"
#include <glad/glad.h>
#include <memory>
#include "Core/ShaderManager.h"
using namespace std;
using glm::mat4;
using GLuint = unsigned int;

// TODO make VAO more clear
class Asteroid {
private:
	GLuint VAO{0}; 
	GLuint VBO{0};
	vector<mat4> matrices;
	AssimpModel rockModel;
	int amount{ 0 };
	float radius{0.0f};
	shared_ptr<Shader> pShader;

public:
	Asteroid() = default;
	Asteroid(int amount, float radius, float offset) {
		
		pShader = ShaderManager::Get("InstancedModel");
		
		this->amount = amount;
		this->radius = radius;

		matrices.reserve(amount);
		ResourceManager::LoadModel("planet/planet.obj");
        ResourceManager::LoadModel("rock/rock.obj");
		rockModel = *ResourceManager::GetModel("rock/rock.obj");

		srand(0);

		for (int i = 0; i < amount; i++) {
			glm::mat4 model(1.0);
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; 
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
			float rotAngle = static_cast<float>((rand() % 360));

			Transform transform;
			transform.position = glm::vec3(x, y, z);
			transform.scale = glm::vec3(scale);
			transform.rotation = Transform::AxisAngleRotate(rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
			matrices.push_back(transform.GetModelMatrix());
		}

		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < rockModel.meshes.size(); i++) {
			unsigned int VAO = rockModel.meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
	void OnDraw(const Camera& cam) {
		pShader->Use();
		pShader->SetMat4("uProjMat", cam.GetProjMat());
        pShader->SetMat4("uViewMat", cam.GetViewMat());
		pShader->SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockModel.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < rockModel.meshes.size(); i++) {
			glBindVertexArray(rockModel.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rockModel.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}
	}
};