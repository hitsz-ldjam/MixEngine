#include "Mx/Math/MxVector2.h"
#include "Mx/Math/MxVector3.h"
#include "Mx/Math/MxVector4.h"
#include "Mx/Math/MxQuaternion.h"
#include "Mx/Math/MxMatrix4.h"

#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "MixEngine.h"

std::ostream& operator<<(std::ostream& _os, const glm::mat4& _m) {
    return _os << boost::format("[\n"
                                "  %f, %f, %f, %f\n"
                                "  %f, %f, %f, %f\n"
                                "  %f, %f, %f, %f\n"
                                "  %f, %f, %f, %f\n"
                                "]")
        % _m[0][0] % _m[1][0] % _m[2][0] % _m[3][0]
        % _m[0][1] % _m[1][1] % _m[2][1] % _m[3][1]
        % _m[0][2] % _m[1][2] % _m[2][2] % _m[3][2]
        % _m[0][3] % _m[1][3] % _m[2][3] % _m[3][3];
}

std::ostream& operator<<(std::ostream& _os, const glm::quat& _q) {
    return _os << boost::format("[%f, %f, %f, %f]") % _q.w %_q.x % _q.y % _q.z;
}

std::ostream& operator<<(std::ostream& _os, const glm::vec3& _v) {
    return _os << boost::format("[%f, %f, %f]") % _v.x % _v.y % _v.z;
}

int main(int argc, char** argv) {
    return Mix::MixEngine::Instance(argc, argv).exec();
    using namespace Mix::Math;
    using namespace std;
}
