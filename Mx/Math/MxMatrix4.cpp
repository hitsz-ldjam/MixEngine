#include "MxMatrix4.h"
#include <glm/gtx/euler_angles.hpp>

namespace Mix {
	namespace Math {
		Matrix4::Matrix4(const float _diagonal) :
			cols{
				{_diagonal, 0.0f, 0.0f, 0.0f},
				{0.0f, _diagonal, 0.0f, 0.0f},
				{0.0f, 0.0f, _diagonal, 0.0f},
				{0.0f, 0.0f, 0.0f, _diagonal}
		} {
		}

		Matrix4::Matrix4(const float _source[16]) :
			cols{
				{_source[0], _source[1], _source[2], _source[3]},
				{_source[4], _source[5], _source[6], _source[7]},
				{_source[8], _source[9], _source[10], _source[11]},
				{_source[12], _source[13], _source[14], _source[15]}
		} {
		}

		Matrix4::Matrix4(const float _a00, const float _a01, const float _a02, const float _a03, const float _a10,
						 const float _a11, const float _a12, const float _a13, const float _a20, const float _a21, const float _a22,
						 const float _a23, const float _a30, const float _a31, const float _a32, const float _a33) :
			linear{ _a00, _a01, _a02, _a03, _a10,
			 _a11, _a12, _a13, _a20, _a21, _a22,
			 _a23, _a30, _a31, _a32, _a33 } {
		}

		Matrix4 Matrix4::add(const Matrix4& _other) const {
			Matrix4 result = *this;

			for (uint32_t i = 0; i < 4; ++i) {
				result.cols[i] += _other.cols[i];
			}
			return result;
		}

		Matrix4 Matrix4::minus(const Matrix4& _other) const {
			Matrix4 result = *this;

			for (uint32_t i = 0; i < 4; ++i) {
				result.cols[i] -= _other.cols[i];
			}
			return result;
		}

		Matrix4 Matrix4::multiply(const Matrix4& _other) const {
			Matrix4 result;

			result[0] = cols[0] * _other[0][0] + cols[1] * _other[0][1] + cols[2] * _other[0][2] + cols[3] * _other[0][3];
			result[1] = cols[0] * _other[1][0] + cols[1] * _other[1][1] + cols[2] * _other[1][2] + cols[3] * _other[1][3];
			result[2] = cols[0] * _other[2][0] + cols[1] * _other[2][1] + cols[2] * _other[2][2] + cols[3] * _other[2][3];
			result[3] = cols[0] * _other[3][0] + cols[1] * _other[3][1] + cols[2] * _other[3][2] + cols[3] * _other[3][3];

			return result;
		}

		Vector4f Matrix4::multiply(const Vector4f& _vector) const {
			return cols[0] * _vector.x + cols[1] * _vector.y + cols[2] * _vector.z + cols[3] * _vector.w;
		}

		Vector3f Matrix4::multiplyPoint(const Vector3f& _point) const {
			return Vector3f(cols[0].x*_point.x + cols[1].x*_point.y + cols[2].x*_point.z + cols[3].x,
							cols[0].y*_point.x + cols[1].y*_point.y + cols[2].y*_point.z + cols[3].y,
							cols[0].z*_point.x + cols[1].z*_point.y + cols[2].z*_point.z + cols[3].z);
		}

		Vector3f Matrix4::multiplyVector(const Vector3f& _vector) const {
			auto col0 = Vector3f(cols[0]);
			auto col1 = Vector3f(cols[1]);
			auto col2 = Vector3f(cols[2]);

			return Vector3f(col0.x*_vector.x + col1.x*_vector.y + col2.x*_vector.z,
							col0.y*_vector.x + col1.y*_vector.y + col2.y*_vector.z,
							col0.z*_vector.x + col1.z*_vector.y + col2.z*_vector.z);
		}

		Vector3f Matrix4::multiplyDirection(const Vector3f& _direction) const {
			auto col0 = Vector3f(cols[0]).normalize();
			auto col1 = Vector3f(cols[1]).normalize();
			auto col2 = Vector3f(cols[2]).normalize();

			return Vector3f(col0.x*_direction.x + col1.x*_direction.y + col2.x*_direction.z,
							col0.y*_direction.x + col1.y*_direction.y + col2.y*_direction.z,
							col0.z*_direction.x + col1.z*_direction.y + col2.z*_direction.z);
		}

		Matrix4 Matrix4::divide(const Matrix4& _other) const {
			Matrix4 result = *this;

			for (uint32_t i = 0; i < 4; ++i) {
				result.cols[i] /= _other.cols[i];
			}
			return result;
		}

		//Matrix4 Matrix4::translate(const Vector4f& _vector) const {
		//    Matrix4 result = *this;
		//    result[3] = result[0] * _vector[0] + result[1] * _vector[1] + result[2] * _vector[2] + result[3];
		//    return result;
		//}

		//Matrix4 Matrix4::translate(const Vector3f& _vector) const {
		//    Matrix4 result = *this;
		//    result[3] = result[0] * _vector[0] + result[1] * _vector[1] + result[2] * _vector[2] + result[3];
		//    return result;
		//}

		//Matrix4 Matrix4::rotate(const Quaternion& _qua) const {
		//    return _qua.toMatrix() * *this;
		//}

		//Matrix4 Matrix4::rotate(const Vector3f& _axis, const float _angle) const {
		//    return Quaternion(_angle, _axis).toMatrix() * *this;
		//}

		//Matrix4 Matrix4::rotate(const float _xAngle, const float _yAngle, const float _zAngle) const {
		//    // implement this later
		//    Matrix4 result;
		//    result.mat = glm::eulerAngleXYZ(_xAngle, _yAngle, _zAngle);
		//    return result * *this;
		//}

		//Matrix4 Matrix4::scale(const Vector4f& _scale) const {
		//    Matrix4 result = *this;
		//    result[0] = result[0] * _scale[0];
		//    result[1] = result[1] * _scale[1];
		//    result[2] = result[2] * _scale[2];
		//    return result;
		//}

		//Matrix4 Matrix4::scale(const Vector3f& _scale) const {
		//    Matrix4 result = *this;
		//    result[0] = result[0] * _scale[0];
		//    result[1] = result[1] * _scale[1];
		//    result[2] = result[2] * _scale[2];
		//    return result;
		//}

		Matrix4 Matrix4::inverse() const {
			// implement this later
			Matrix4 result;
			result.mat = glm::inverse(this->mat);
			return result;
		}

		Matrix4 Matrix4::transpose() const {
			Matrix4 result;
			result[0] = Vector4f(cols[0][0], cols[1][0], cols[2][0], cols[3][0]);
			result[1] = Vector4f(cols[0][1], cols[1][1], cols[2][1], cols[3][1]);
			result[2] = Vector4f(cols[0][2], cols[1][2], cols[2][2], cols[3][2]);
			result[3] = Vector4f(cols[0][3], cols[1][3], cols[2][3], cols[3][3]);
			return result;
		}

		bool Matrix4::isIdentity() const {
			return cols[0] == Vector4f(1.0f, 0.0f, 0.0f, 0.0f) &&
				cols[1] == Vector4f(0.0f, 1.0f, 0.0f, 0.0f) &&
				cols[2] == Vector4f(0.0f, 0.0f, 1.0f, 0.0f) &&
				cols[3] == Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		}

		Vector4f Matrix4::getColumn(const uint32_t _index) const {
			if (!(_index < 4)) {
				throw Exception("Matrix4::getColumn index out of bounds!");
			}

			return cols[_index];
		}

		Vector4f Matrix4::getRow(const uint32_t _index) const {
			if (!(_index < 4)) {
				throw Exception("Matrix4::getRow index out of bounds!");
			}

			return Vector4f(cols[0][_index], cols[1][_index], cols[2][_index], cols[3][_index]);
		}

		void Matrix4::setColumn(const uint32_t _index, const Vector4f& _col) {
			if (!(_index < 4)) {
				throw Exception("Matrix4::setColumn index out of bounds!");
			}

			cols[_index] = _col;
		}

		void Matrix4::setRow(const uint32_t _index, const Vector4f& _row) {
			if (!(_index < 4)) {
				throw Exception("Matrix4::setRow index out of bounds!");
			}

			cols[0][_index] = _row[0];
			cols[1][_index] = _row[1];
			cols[2][_index] = _row[2];
			cols[3][_index] = _row[3];
		}

		Vector3f Matrix4::getTranslation() const {
			return Vector3f(cols[3]);
		}

		Quaternion Matrix4::getRotation() const {
			Matrix4 temp(*this);
			temp[0] /= temp[0].length();
			temp[1] /= temp[1].length();
			temp[2] /= temp[2].length();
			return Quaternion(temp);
		}

		Vector3f Matrix4::getScale() const {
			return Vector3f(cols[0].length(), cols[1].length(), cols[2].length());
		}

		bool Matrix4::validTRS() const {
			// check if the last row is [0, 0, 0, 1]
			if (cols[0].w != 0.0f || cols[1].w != 0.0f || cols[2].w != 0.0f || cols[3].w != 1.0f)
				return false;

			//the columns of its upper-left 3x3 subblock are non-zero and pairwise orthogonal.
			Vector3f col0 = Vector3f(cols[0]);
			Vector3f col1 = Vector3f(cols[1]);
			Vector3f col2 = Vector3f(cols[2]);

			if (col0.x == 0.0f && col0.y == 0.0f && col0.z == 0.0f)
				return false;
			if (col1.x == 0.0f && col1.y == 0.0f && col1.z == 0.0f)
				return false;
			if (col2.x == 0.0f && col2.y == 0.0f && col2.z == 0.0f)
				return false;

			return EpsilonEqual(col0.dot(col1), 0.0f) &&
				EpsilonEqual(col1.dot(col2), 0.0f) &&
				EpsilonEqual(col2.dot(col0), 0.0f);
		}

		void Matrix4::setTRS(const Vector3f& _pos, const Quaternion& _qua, const Vector3f& _scale) {
			// Set rotation
			*this = _qua.toMatrix();

			// Set translate
			(*this)[3].x = _pos.x;
			(*this)[3].y = _pos.y;
			(*this)[3].z = _pos.z;

			// Set scale
			(*this)[0] = (*this)[0] * _scale[0];
			(*this)[1] = (*this)[1] * _scale[1];
			(*this)[2] = (*this)[2] * _scale[2];
		}

		Matrix4 Matrix4::Frustum(const float _left, const float _right, const float _bottom, const float _top,
								 const float _zNear, const float _zFar) {
			return Matrix4(glm::frustumLH_ZO(_left, _right, _bottom, _top, _zNear, _zFar));
		}

		Matrix4 Matrix4::Perspective(const float _fov, const float _aspect, const float _zNear, const float _zFar) {
			auto result = Matrix4(glm::perspectiveLH_ZO(_fov, _aspect, _zNear, _zFar));
			return result;
		}

		Matrix4 Matrix4::LookAt(const Vector3f& _from, const Vector3f& _to, const Vector3f& _up) {
			return TRS(_from, Quaternion::LookRotation(_to - _from, _up), Vector3f::One);
		}

		Matrix4 Matrix4::ViewMatrix(const Vector3f& _eye, const Vector3f& _center, const Vector3f& _up) {
			return Matrix4(glm::lookAtLH(_eye.vec, _center.vec, _up.vec));
		}

		Matrix4 Matrix4::Ortho(const float _left, const float _right, const float _bottom, const float _top,
							   const float _zNear, const float _zFar) {
			return Matrix4(glm::orthoLH_ZO(_left, _right, _bottom, _top, _zNear, _zFar));
		}

		Matrix4 Matrix4::Translate(const Vector3f& _vector) {
			Matrix4 result;
			result[3] = Vector4f(_vector, 1.0f);
			return result;
		}

		Matrix4 Matrix4::Rotate(const Quaternion& _qua) {
			return _qua.toMatrix();
		}

		Matrix4 Matrix4::Scale(const Vector3f& _scale) {
			Matrix4 result;
			result[0] *= _scale.x;
			result[1] *= _scale.y;
			result[2] *= _scale.z;
			return result;
		}

		Matrix4 Matrix4::TRS(const Vector3f& _pos, const Quaternion& _qua, const Vector3f& _scale) {
			Matrix4 result;
			result.setTRS(_pos, _qua, _scale);
			return result;
		}

		std::string Matrix4::toString() const {
			return (boost::format("[\n"
					"  %f, %f, %f, %f\n"
					"  %f, %f, %f, %f\n"
					"  %f, %f, %f, %f\n"
					"  %f, %f, %f, %f\n"
					"]")
					% cols[0][0] % cols[1][0] % cols[2][0] % cols[3][0]
					% cols[0][1] % cols[1][1] % cols[2][1] % cols[3][1]
					% cols[0][2] % cols[1][2] % cols[2][2] % cols[3][2]
					% cols[0][3] % cols[1][3] % cols[2][3] % cols[3][3]).str();
		}

		bool Matrix4::operator==(const Matrix4& _other) const {
			return cols[0] == _other[0] && cols[1] == _other[1] && cols[2] == _other[2] && cols[3] == _other[3];
		}

		Matrix4 Matrix4::operator-() const {
			const Matrix4 result = Zero;
			return result - *this;
		}

		Matrix4 Matrix4::operator+(const Matrix4& _right) const {
			return add(_right);
		}

		Matrix4 Matrix4::operator-(const Matrix4& _right) const {
			return minus(_right);
		}

		Matrix4 Matrix4::operator*(const Matrix4& _right) const {
			return multiply(_right);
		}

		Matrix4 Matrix4::operator/(const Matrix4& _right) const {
			return divide(_right);
		}

		Matrix4 Matrix4::operator*(const float _right) const {
			Matrix4 result = *this;
			result.cols[0] *= _right;
			result.cols[1] *= _right;
			result.cols[2] *= _right;
			result.cols[3] *= _right;
			return result;
		}

		Matrix4 Matrix4::operator/(const float _right) const {
			Matrix4 result = *this;
			result.cols[0] /= _right;
			result.cols[1] /= _right;
			result.cols[2] /= _right;
			result.cols[3] /= _right;
			return result;
		}

		Vector4f Matrix4::operator*(const Vector4f& _right) const {
			return multiply(_right);
		}

		const Vector4f& Matrix4::operator[](const uint32_t _index) const {
			if (!(_index < 4)) {
				throw Exception("Matrix4 index out of bounds!");
			}

			return cols[_index];
		}

		Matrix4& Matrix4::operator+=(const Matrix4& _right) {
			return *this = add(_right);
		}

		Matrix4& Matrix4::operator-=(const Matrix4& _right) {
			return *this = minus(_right);
		}

		Matrix4& Matrix4::operator*=(const Matrix4& _right) {
			return *this = multiply(_right);
		}

		Matrix4& Matrix4::operator/=(const Matrix4& _right) {
			return *this = divide(_right);
		}

		Matrix4& Matrix4::operator*=(const float& _right) {
			cols[0] *= _right;
			cols[1] *= _right;
			cols[2] *= _right;
			cols[3] *= _right;

			return *this;
		}

		Matrix4& Matrix4::operator/=(const float& _right) {
			cols[0] /= _right;
			cols[1] /= _right;
			cols[2] /= _right;
			cols[3] /= _right;

			return *this;
		}

		Matrix4 operator*(const float _left, const Matrix4& _right) {
			return _right * _left;
		}

		const Matrix4 Matrix4::Identity = Matrix4(1.0f);
		const Matrix4 Matrix4::Zero = Matrix4(0.0f);
	}
}
