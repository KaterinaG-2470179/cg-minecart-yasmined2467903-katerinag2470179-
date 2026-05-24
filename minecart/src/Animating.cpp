#include "Animating.h"

#include <glm/gtc/matrix_transform.hpp>

Animating::Animating(Bezier& bezier, Model& model, float speed) : m_bezier(bezier), m_model(model), m_speed(speed), m_dist(0.0f), model_matrix(1.0f)
{}

void Animating::update(float timeDiff)
{
    //calculate the total distance we've moved since previous frame, if our calculated distance is bigger than our total arclen then we reset it to 0 to restart
    m_dist += m_speed * timeDiff;
    if (m_dist > m_bezier.GetTotalLen()) { m_dist = 0.0f; }

    //get cart position by getting the corresponding t from our table with our distance ( see tFromArcLen function for explanation)
    float t = m_bezier.tFromArcLen(m_dist);
    glm::vec3 cartPos = m_bezier.GetPointAt(t);

    //to calculate the trajectory we take a next point not far away from our current one, for safety we take minimum in case that our new point distance is larger than total arclen
    float distNext = glm::min(m_dist + 0.05f, m_bezier.GetTotalLen());
    float tNext = m_bezier.tFromArcLen(distNext);
    glm::vec3 nextPos = m_bezier.GetPointAt(tNext);         //basically calculate the distance of our next point first (minimum), then we get the corresponding t with that distance and then we collect the point with the GetPointAt

    //then we find the trajectory we need, we do a floating point comparison (we check if our 2 points have enough distance between them so we can normalize and get our trajectory)
    glm::vec3 trajectory;
    if (glm::length(nextPos - cartPos) > 0.000001f) { 
        trajectory = glm::normalize(nextPos - cartPos);         //get the trajectory of the current point to next one, we normalize so that len is exactly 1.0
        last_traj = trajectory;
    } 
    else { 
        trajectory = last_traj;                        // if dist not big enough (that being 10^-6) we just use our previous saved trajectory instead because we consider the 2 points the same 
    } 

    //lastly we need to build an axis system (X Y Z on graph), first we just make our AXIS Y for now (we recalculate ir later)
    glm::vec3 axisY = glm::vec3(0, 1, 0);

    //then we need to calculate the X axis by finding the the vector that stands straight on both our trajectory and our Y axis (that being X)
    glm::vec3 axisX = glm::normalize(glm::cross(axisY, trajectory));

    //lastly we have to refix our Y axis so that all our 3 axis' are standing straight on top of each other (L) -> from GRam schmidt
    glm::vec3 correctedY = glm::cross(trajectory, axisX);

    //finally we update our model matrix so that we cna use it later when we need to draw our object at its new location
    model_matrix[0] = glm::vec4(axisX, 0.0f);           //x-as
    model_matrix[1] = glm::vec4(correctedY, 0.0f);      //y-as
    model_matrix[2] = glm::vec4(trajectory, 0.0f);      //z-as
    model_matrix[3] = glm::vec4(cartPos, 1.0f);         //model position
}

void Animating::draw(ShaderProgram& shader, glm::mat4& view, glm::mat4& proj)
{
    shader.use();
    shader.setMat4("model", model_matrix);
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);
    m_model.Draw(shader);
}