// HW2_TODO: Please implement the abstract class Warper
// 1. The Warper class should abstract the **mathematical mapping** involved in
// the warping problem, **independent of image**.
// 2. The Warper class should have a virtual function warp(...) to be called in
// our image warping application.
//    - You should design the inputs and outputs of warp(...) according to the
//    mathematical abstraction discussed in class.
//    - Generally, the warping map should map one input point to another place.
// 3. Subclasses of Warper, IDWWarper and RBFWarper, should implement the
// warp(...) function to perform the actual warping.
#pragma once

#include <utility>
#include <vector>

#include "type_define.h"

namespace USTC_CG
{
class Warper
{
   public:
    virtual ~Warper() = default;

    // HW2_TODO: A virtual function warp(...)
    virtual std::pair<int, int> warp(const int& x, const int& y) = 0;
    
    virtual bool get_transform() = 0;
    // HW2_TODO: other functions or variables if you need
   protected:
    std::vector<V2D> m_P;
    std::vector<V2D> m_Q;
    int n_points_ = 0;
    bool is_valid_ = true;
};
}  // namespace USTC_CG