#include <iostream>
#include "Bezier.h"

Bezier::Bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int steps, Bezier::IterationMethod method) : ParamCubeCurve(p0, p1, p2, p3, steps), m_method(method) {
}

void Bezier::iterate() {
    ClearPoints();

    switch (m_method) {
        case IterationMethod::BRUTEFORCE:
            Bruteforce(m_steps);
            break;
        case IterationMethod::FORWARDDIFFERENCE:
            ForwardDifference(m_steps);
            break;
        default:
            std::cerr << "Method not implemented" << std::endl;
    }
}

Bezier::Bezier() : ParamCubeCurve(glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3(), 0), m_method(IterationMethod::BRUTEFORCE) {
    m_base = glm::mat4(
        -1,  3, -3,  1,
         3, -6,  3,  0,
        -3,  3,  0,  0,
         1,  0,  0,  0
    );
}

void Bezier::Bruteforce(int steps) {
    for (int i = 0; i <= steps; i++) {
        double t = (double)i / steps;
        AddPoint(Evaluate(t));
    }
}

void Bezier::ForwardDifference(int steps) {
    float h = 1.0f / steps;
    float h2 = h * h;
    float h3 = h2 * h;

    glm::mat4x3 GM = m_geometric * m_base;

    glm::vec3 a =  GM[3];
    glm::vec3 b = GM[2];
    glm::vec3 c = GM[1];
    glm::vec3 dd = GM[0];

    glm::vec3 f   = a;
    glm::vec3 df  = b*h + c*h2 + dd*h3;
    glm::vec3 d2f = 2.0f*c*h2 + 6.0f*dd*h3;
    glm::vec3 d3f = 6.0f*dd*h3;

    for (int i = 0; i <= steps; i++) {
        AddPoint(f);
        f   += df;
        df  += d2f;
        d2f += d3f;
    }
}