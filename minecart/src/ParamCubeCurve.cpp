#include "ParamCubeCurve.h"

ParamCubeCurve::ParamCubeCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int steps) : m_geometric(p0, p1, p2, p3), m_base(1.0f), m_steps(steps) {}

glm::vec3 ParamCubeCurve::Evaluate(double t) {
    glm::vec4 T(t*t*t, t*t, t, 1.0);
    glm::vec4 tM = m_base * T;
    return glm::vec3(m_geometric * tM);
}

void ParamCubeCurve::AddPoint(glm::vec3 point) {
    m_points.push_back(point);
}

void ParamCubeCurve::ClearPoints() {
    m_points.clear();
}

int ParamCubeCurve::PointCount() const {
    return (int)m_points.size();
}

glm::vec3 ParamCubeCurve::GetPoint(int index) const {
    return m_points[index];
}

const std::vector<glm::vec3>& ParamCubeCurve::GetPoints() const {
    return m_points;
}
