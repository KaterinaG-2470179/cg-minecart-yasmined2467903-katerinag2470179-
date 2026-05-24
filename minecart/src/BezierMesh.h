#pragma once

#include "Mesh.h"
#include "Bezier.h"

class BezierMesh : public Mesh {
    public:
        BezierMesh(Bezier& bezier);

        void update();
        void draw(const ShaderProgram& shader) override;

    private:
        Bezier& m_bezier;
        size_t m_count = 0;
};