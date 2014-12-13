#ifndef TREEMAKER_H
#define TREEMAKER_H

#include <deque>
#include <string>
#include "Common.h"

class TreeMaker{

public:
    TreeMaker();
    ~TreeMaker();

    void reset(float trunkRadius, std::deque<glm::mat4x4> *shapeTransformations, std::deque<glm::mat4x4> *leafTransformations);

    void makeTree();

protected:

    void cycleLString(int iterNum);
    void handleBranch(glm::mat4x4 current_total_transformation);

    float m_trunkRadius;

    std::deque<glm::mat4x4> *m_shapeTransformations;
    std::deque<glm::mat4x4> *m_leafTransformations;

    std::string L_string;
    int L_index;

    float m_x, m_y;

    int numIters;

    // Only ever use push/pop_front on these, turns them into a stack.
    std::deque<float> phi_rotations;
    std::deque<float> theta_rotations;
    std::deque<float> branch_size_ratios;

    float current_branch_radius;

};

#endif // TREEMAKER_H
