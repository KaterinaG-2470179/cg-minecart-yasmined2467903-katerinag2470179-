#pragma once

#include "Bezier.h"
#include "Model.h"

class Animating {
public:
	Animating(Bezier& bezier, Model& model, float speed);

	void update(float timeDiff);
	void draw(Shader& shader, glm::mat4& view, glm::mat4& proj);

private:
	Bezier m_bezier;
	Model m_model;

	float m_speed;
	float m_dist;

	glm::mat4 model_matrix;
	glm::vec3 last_traj = glm::vec3(0, 0, 1); //we use this in update
};