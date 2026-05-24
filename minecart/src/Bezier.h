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

    void buildArcLenTable();
    float tFromArcLen(float d);

    float GetTotalLen();
    glm::vec3 GetPointAt(float t);

private:
    IterationMethod m_method;

    void Bruteforce(int steps);
    void ForwardDifference(int steps);

    //everything underneath is neded for animation calculating 
    glm::vec3 m_p0;
    glm::vec3 m_p1;
    glm::vec3 m_p2;
    glm::vec3 m_p3;

    std::vector<float> m_arcLens;
    float m_totalLen;
};