#include "treemaker.h"

using namespace std;

/*
TreeMaker::TreeMaker()
{
}


void TreeMaker::reset(float trunkRadius, std::deque<glm::mat4x4> *shapeTransformations, std::deque<glm::mat4x4> *leafTransformations)
{
    m_trunkRadius = trunkRadius;
    current_branch_radius = m_trunkRadius;
    m_shapeTransformations = shapeTransformations;
    m_leafTransformations = leafTransformations;
    L_string = "!";
    generateLString();
}

void TreeMaker::cycleLString(int iterNum, int numIters){

    std::vector<char> new_result;
    int new_i = 0;

    int l = L_string.length();

    for(int i = 0; i < l; i++){
        if((L_string[i] == '[') ||
        (L_string[i] == 'a') ||
        (L_string[i] == 'b') ||
        (L_string[i] == 'c') ||
        (L_string[i] == ']')){
            new_result.push_back(L_string[i]);
        } else if(L_string[i] == '!'){

            if(iterNum == numIters){
                L_string.push_back('[');
                L_string.push_back('x');
                L_string.push_back(']');
                continue;
            }

            new_result[new_i++] = '[';
            int r = rand() % 3;
            new_result[new_i++] = r + 97;
            for(int j = 0; j <= r; j++){
                new_result[new_i++] = '!';
            }
            new_result[new_i++] = ']';

        } else if(L_string[i] == 'x'){
            new_result[new_i++] == 'x';
        } else if(L_string[i] == '\0'){
            break;
        }
    }

    // Copy the iterated string over.
    L_string = "";
    for(int i = 0; i < TEMP_MAX_STR_LENGTH; i++){
        L_string += new_result[i];
    }
}



// Generates a random float between 0.0 and 1.0.  Off the Internet.
float randomFloat(){
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void TreeMaker::makeTree(){
    // Basically a wrapper for the branch function.
    handleBranch(glm::mat4x4(1.0));
}

void TreeMaker::handleBranch(glm::mat4x4 current_total_transformation){
    while(L_string[L_index] != '\0'){

        // **************************************************

        // The section to handle the actual transformation matrix calculations.
        if(L_string[L_index] == '['){

            // This is where we need to recurse.
            phi = phi_rotations.pop_front();
            theta = theta_rotations.pop_front();

            float branch_size_ratio = branch_size_ratios.pop_front;
            current_branch_radius *= branch_size_ratio;

            // The randomly generated length of this branch will be between 5 and 15 times the diameter.
            float length = (randomFloat() * 20.0 + 10.0) * current_branch_radius;

            // Will transform object space to reside at the tip of the new branch.
            glm::mat4x4 coord_translation = glm::translate(0.0, length, 0.0);
            // The cylinder, being centered at the origin, needs a separate translation matrix.
            glm::mat4x4 cyl_translation = glm::translate(0.0, length / 2, 0.0);
            // Will transform object space so that the y-axis is aligned with the new branch. (angle, axis)
            glm::mat4x4 rotation = glm::rotate(phi, sin(theta), 0.0, cos(theta));
            // Scale matrix in order to transform the cylinder itself.
            glm::mat4x4 scale = glm::scale(current_branch_radius, length, current_branch_radius);

            // Used to add the cylinder itself to the scenegraph.
            glm::mat4x4 branch_trans = cyl_translation * rotation * scale;
            // And the tranformation to manipulate object space.
            glm::mat4x4 coord_trans = coord_translation * rotation;

            // Adding the cylinder representing the branch to the sceneview graph.
            m_shapeTransformations->push_front(branch_trans * current_total_transformation);

            // Recurse.  Remember, there's no L_index increment at the beginning of the nested call.
            L_index++;
            handleBranch(coord_trans * current_total_transformation);

            // We need to recover this class-global variable's original value when the sub-recursion finishes.
            current_branch_radius /= branch_size_ratio;

        }

        // **************************************************

        // The section to prepare for branchings.
        else if(L_string[L_index] == 'a'){
            // One branching, only slightly smaller than the parent.
            // This branch may bend up to 45 degrees from the parent, and at any angle.
            float phi = randomFloat() * 45 * DEG_TO_RAD;
            float theta = randomFloat() * 360 * DEG_TO_RAD;

            phi_rotations.push_front(phi);
            theta_rotations.push_front(theta);

            branch_size_ratios.push_front(.9);
        } else if(L_string[L_index] == 'b'){
            // Two branchings.
            // I balance the sizes and directions of the two branches.
            // If one is bigger, the other is smaller.
            // The bigger one will be closer in line to the parent branch.  The smaller will stick out more.
            // Their thetas will differ by 180 degrees.

            // The branches can rotate in any direction.
            float theta1 = randomFloat() * 360 * DEG_TO_RAD;
            float theta2 = theta1 + (180 * DEG_TO_RAD);

            // The ratios can be between .95 and .25, with an average of .65.
            float ratio1 = randomFloat() * .70 + .25;
            float ratio2 = 1.3 - ratio1;

            float phi1 = (1.0 - ratio1) * 45 * DEG_TO_RAD;
            float phi2 = (1.0 - ratio2) * 45 * DEG_TO_RAD;

            phi_rotations.push_front(phi1);
            phi_rotations.push_front(phi2);
            theta_rotations.push_front(theta1);
            theta_rotations.push_front(theta2);
            branch_size_ratios.push_front(ratio1);
            branch_size_ratios.push_front(ratio2);
        } else if(L_string[L_index] == 'b'){
            // Three branchings.
            // The minimum theta angle between two branches is 35 degrees.  Maximum is 160.
            // The minimum phi angle is 25 degrees.  Maximum is 80.  Independent.
            // The ratios range from .6 to .15.  Independent.

            float theta1 = randomFloat() * 360 * DEG_TO_RAD;
            float theta2 = theta1 + (randomFloat() * 125 + 35) * DEG_TO_RAD;
            float theta3 = theta2 + (randomFloat() * 125 + 35) * DEG_TO_RAD;

            float phi1 = (randomFloat() * 55 + 25) * DEG_TO_RAD;
            float phi2 = (randomFloat() * 55 + 25) * DEG_TO_RAD;
            float phi3 = (randomFloat() * 55 + 25) * DEG_TO_RAD;

            float ratio1 = randomFloat() * .45 + .15;
            float ratio2 = randomFloat() * .45 + .15;
            float ratio3 = randomFloat() * .45 + .15;

            phi_rotations.push_front(phi1);
            phi_rotations.push_front(phi2);
            phi_rotations.push_front(phi3);
            theta_rotations.push_front(theta1);
            theta_rotations.push_front(theta2);
            theta_rotations.push_front(theta3);
            branch_size_ratios.push_front(ratio1);
            branch_size_ratios.push_front(ratio2);
            branch_size_ratios.push_front(ratio3);
        }

        // **************************************************

        // The section for generating leaves.
        else if(L_string[L_index] == 'x'){
            // Generate one leaf.
            // Random value for phi and theta.
            float phi = randomFloat() * 30 * DEG_TO_RAD;
            float theta = randomFloat() * 360 * DEG_TO_RAD;

            // We do not translate or scale the leaf in object space.
            // We only need to rotate.
            glm::mat4x4 rotation = glm::rotate(phi, sin(theta), 0.0, cos(theta));

            // Store the transformation matrix.
            m_leafTransformations->push_front(rotation * current_total_transformation);
        }

        // **************************************************

        // Lessen recursion upon closing bracket.
        else if(L_string[L_index] == ']'){
            return;
        }

        // **************************************************

        // This is not a for loop only because L_index should not be re-initialized every recursion.
        // We need to manually increment it here.
        L_index++;

    }
}
*/
