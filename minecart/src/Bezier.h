#pragma once

#include "ParamCubeCurve.h"

//wordt gebruikt om een loop vna curves te maken
struct Segment {
    glm::vec3 p0, p1, p2, p3;
};

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

    void addSegment(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

private:
    IterationMethod m_method;

    void Bruteforce(int steps);
    void ForwardDifference(int steps);

    std::vector<float> m_arcLens;
    float m_totalLen;

    //loop of bezier curves
    std::vector<Segment> m_segments;
};