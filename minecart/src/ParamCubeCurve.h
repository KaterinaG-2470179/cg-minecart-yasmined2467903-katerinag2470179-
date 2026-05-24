#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>


class ParamCubeCurve {
    public:
        ParamCubeCurve(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int steps);

        void AddPoint(glm::vec3 point);
        void ClearPoints();
        int PointCount() const;
        glm::vec3 GetPoint(int index) const;
        const std::vector<glm::vec3>& GetPoints() const;

    protected:
        glm::mat4 m_base;
        glm::mat4x3 m_geometric;
        int m_steps;

        std::vector<glm::vec3> m_points;

        glm::vec3 Evaluate(double t);
};