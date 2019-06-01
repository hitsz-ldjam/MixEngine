#include "Mx/Math/MxVector2.h"
#include "Mx/Math/MxVector3.h"
#include "Mx/Math/MxVector4.h"
#include "Mx/Math/MxQuaternion.h"
#include "Mx/Math/MxMatrix4.h"

#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/matrix_decompose.inl>
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

    /*cout << "Default construct:" << Vector3f() << endl;

    Vector3f vec41(1.0f);
    Vector3f vec42(1.0f, 2.0f, 3.0f);

    cout << "vec41 = " << vec41 << endl << "vec42 = " << vec42 << endl;

    cout << "vec41 + vec42 = " << vec41 + vec42 << endl;
    cout << "vec41 - vec42 = " << vec41 - vec42 << endl;
    cout << "vec41 * vec42 = " << vec41 * vec42 << endl;
    cout << "vec41 / vec42 = " << vec41 / vec42 << endl;

    cout << " 1.0f + vec42 = " << 1.0f + vec42 << endl;
    cout << " 1.0f * vec42 = " << 1.0f * vec42 << endl;

    cout << "vec42 + 1.0f = " << vec42 + 1.0f << endl;
    cout << "vec42 - 1.0f = " << vec42 - 1.0f << endl;
    cout << "vec42 * 1.0f = " << vec42 * 1.0f << endl;
    cout << "vec42 / 1.0f = " << vec42 / 1.0f << endl;

    cout << "vec41 += 1.0f = " << (vec41 += 1.0f) << endl;
    cout << "vec41 -= 1.0f = " << (vec41 -= 1.0f) << endl;
    cout << "vec41 *= 2.0f = " << (vec41 *= 2.0f) << endl;
    cout << "vec41 /= 2.0f = " << (vec41 /= 2.0f) << endl;

    cout << "vec41 = Vector4f::One " << (vec41 = Vector3f::One) << endl;

    cout << "vec41 length = " << vec41.length() << endl;
    cout << "vec41 normalize = " << vec41.normalize() << endl;

    cout << "vec41 == vec42 ?" << boolalpha << (vec41 == vec42) << endl;
    cout << "vec41 != vec42 ?" << boolalpha << (vec41 != vec42) << endl;

    cout << "-vec41 = " << -vec41 << endl;

    for (int i = 0; i < 3; ++i) {
        cout << boost::format("vec41[%d] = %d\n") % i % vec41[i];
    }*/
}
