#pragma once

#include "ParamCubeCurve.h"

class Bezier : public ParamCubeCurve {
    public:
        enum IterationMethod {
            BRUTEFORCE,
            FORWARDDIFFERENCE
        };

        Bezier();
        Bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int steps, IterationMethod method = IterationMethod::BRUTEFORCE);

        void iterate();

    private:
        IterationMethod m_method;

        void Bruteforce(int steps);
        void ForwardDifference(int steps);
};