#include <iostream>
#include "Bezier.h"

Bezier::Bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int steps, Bezier::IterationMethod method) : ParamCubeCurve(p0, p1, p2, p3, steps), m_method(method)
{
    m_segments.push_back({ p0, p1, p2, p3 });
}

void Bezier::iterate() {
    ClearPoints();
    if (m_method == IterationMethod::BRUTEFORCE)
        Bruteforce(m_steps * m_segments.size());
    else
        ForwardDifference(m_steps * m_segments.size());
}

Bezier::Bezier() : ParamCubeCurve(glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3(), 0), m_method(IterationMethod::BRUTEFORCE) {
    m_base = glm::mat4(
        -1,  3, -3,  1,
         3, -6,  3,  0,
        -3,  3,  0,  0,
         1,  0,  0,  0
    );
    m_segments.push_back({ glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3() });
}

void Bezier::Bruteforce(int steps) {
    for (int x = 0; x < m_segments.size(); x++) {
        Segment& seg = m_segments[x];

        //do this check for safety to check if we are at our endpoint/beginpoint, because we dont want to put out point to be duped in our point list
        int end = steps - 1;
        if (x == m_segments.size() - 1) { end = steps; }

        for (int i = 0; i <= end; i++) {
            float t = (float)i / steps;
            float x = 1.0f - t;
            glm::vec3 point = x * x * x * seg.p0 +
                3 * x * x * t * seg.p1 +
                3 * x * t * t * seg.p2 +
                t * t * t * seg.p3;
            AddPoint(point);
        }
    }
}

void Bezier::ForwardDifference(int steps) {
    float h = 1.0f / steps;
    float h2 = h * h;
    float h3 = h2 * h;

    //need to constantly build it for each segment that we have
    for (int i = 0; i < m_segments.size(); i++) {
        Segment& seg = m_segments[i];

        //build geometric matrix for curr segment
        glm::mat4x3 geo = glm::mat4x3(
            seg.p0.x, seg.p0.y, seg.p0.z,
            seg.p1.x, seg.p1.y, seg.p1.z,
            seg.p2.x, seg.p2.y, seg.p2.z,
            seg.p3.x, seg.p3.y, seg.p3.z
        );

        glm::mat4x3 GM = geo * m_base;

        glm::vec3 a = GM[3];
        glm::vec3 b = GM[2];
        glm::vec3 c = GM[1];
        glm::vec3 dd = GM[0];

        glm::vec3 f = a;
        glm::vec3 df = b * h + c * h2 + dd * h3;
        glm::vec3 d2f = 2.0f * c * h2 + 6.0f * dd * h3;
        glm::vec3 d3f = 6.0f * dd * h3;

        //do this check for safety to check if we are at our endpoint/beginpoint, because we dont want to put out point to be duped in our point list
        int end = steps - 1;
        if (i == m_segments.size() - 1) { end = steps; }

        for (int x = 0; x <= end; x++) {
            AddPoint(f);
            f += df;
            df += d2f;
            d2f += d3f;
        }
    }
}

//build a look-up table where we save a corresponding t to our arclen d
void Bezier::buildArcLenTable() 
{
    glm::vec3 prev_pt = GetPointAt(0.0f);
    m_arcLens.push_back(0.0f);

    //for every point in our curve we calculate our t by calculating the arcleng between current point and previous point, and then adding that with the previous total to our arcLens
    for (int i = 1; i < m_points.size(); i++) 
    {
        float t = float (i) / float ( (m_points.size() - 1) ); //t needs to be between 0 and 1 (0 = begin, 1 = end) so we do m_points.size - 1 incase that m_points.size is bigger than i so!
        glm::vec3 curr_pt = GetPointAt(t);

        float pt_dist = glm::length(curr_pt - prev_pt);     //calculate the distance between ccurrent and previous point, 
        m_arcLens.push_back(m_arcLens.back() + pt_dist);    //then add said distance with our previous calculated distance so we constantly have the total distance from begin till that endpoint

        prev_pt = curr_pt;
    }
    m_totalLen = m_arcLens.back();      //when for loop completely finished, we calculated an approach of our arc length of the curve
}

//give t from a given arc len d 
float Bezier::tFromArcLen(float d) 
{
    d = glm::clamp(d, 0.0f, m_totalLen);    //gives a value that can only vall between a given min and max, say we give a negative d, then we get 0.0f as d, or a d bigger than totalLen gives us totalLen

    //we search for the indexes where our given d is located in m_arcLens
    int begin = 0;
    int end = m_arcLens.size() - 1;

    while (begin < end - 1) {                   //we constantly keep iterating until we have found the 2 closest indices in which our d is 
        int mid = (begin + end) / 2;            //take the middle point between the 2

        if (m_arcLens[mid] < d) begin = mid;    //if the arclen located on index mid is lower than the arclen we are looking for, we put mid as our new begin point and continue iterating
        else end = mid;                         //if not, then we put our mid as the end index so that on our next iteration the loop will stop
    }

    float t0 = float(begin) / float( (m_arcLens.size() - 1) );  //we have the begin and end indices in which our d falls, so now we need to get the corresponding t (between 0 and 1)
    float t1 = float(end) / float( (m_arcLens.size() - 1) );

    float seg_dist = m_arcLens[end] - m_arcLens[begin]; //get the distance for the segment between our 2 indices

    // lastly we check how far we are into our segment, if frac is 0.80 then we are 80% through our segment between t0 and t1
    float frac;
    if (seg_dist > 0.0f) { frac = (d - m_arcLens[begin]) / seg_dist; }
    else { frac = 0.0f; }

    return t0 + frac * (t1 - t0);   //we use said frac to calculate our t with linear interpolation (formula: result = start + fraction * (end - start) )
}

float Bezier::GetTotalLen() 
{ 
    return m_totalLen; 
}

//return the point on bezier curve at a given t
glm::vec3 Bezier::GetPointAt(float t) 
{
    //we need to first sccale our t to the amount of segments our loop has
    float t_saled = t * m_segments.size();

    //then we calculate in which segment we're working by casting our float to int (0.5 -> 0) and save said segment so we can get p0,p1,p2,p3 from it
    int i = glm::min((int)t_saled, (int)m_segments.size() - 1);
    Segment& curr_seg = m_segments[i];

    //then we get the local t withing a segment and use that for caluclating our point
    float local_t = t_saled - i;

    //our bezier curve is built up of 4 points, meaning that we work with a cubic bezier curve
    //we use the corresponding formula for said curve to calculate the point at a given t
    //formula: https://www.mathwords.com/b/bezier_curve.htm 
    float x = 1.0f - local_t; //we use this instead of repeatedly writing (1 - t)

    return x * x * x * curr_seg.p0 +                     // part with P0
            3 * x * x * local_t * curr_seg.p1 +          // part with P1
            3 * x * local_t * local_t * curr_seg.p2 +    // part with P2
           local_t * local_t * local_t * curr_seg.p3;    // part with P3
}

void Bezier::addSegment(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    glm::vec3 p0 = m_segments.back().p3;        //if we add a new segment to our bezier loop, we need to make sure that our the endpoint of our previous segment is the begin point of our new one
    m_segments.push_back({ p0, p1, p2, p3 });
}