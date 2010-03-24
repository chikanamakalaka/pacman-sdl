/*
 * math.hpp
 *
 *  Created on: May 18, 2009
 *      Author: asantos
 */

#ifndef MATH_HPP_
#define MATH_HPP_


#include <boost/math/quaternion.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

class Vector2:public  boost::numeric::ublas::vector<float>{
public:
	Vector2(): boost::numeric::ublas::vector<float>(2){}
	Vector2(float x1, float x2): boost::numeric::ublas::vector<float>(2){
		(*this)(0) = x1;
		(*this)(1) = x2;
	}
	~Vector2(){}
};

class Vector3:public  boost::numeric::ublas::vector<float>{
public:
	Vector3(): boost::numeric::ublas::vector<float>(3){}
  Vector3(const boost::numeric::ublas::vector<float>& v):boost::numeric::ublas::vector<float>(v){}
	~Vector3(){}
};

class Vector4:public  boost::numeric::ublas::vector<float>{
public:
	Vector4(): boost::numeric::ublas::vector<float>(4){}
	~Vector4(){}
};

class Matrix4: public boost::numeric::ublas::bounded_matrix<float,4,4>{
public:
	Matrix4():boost::numeric::ublas::bounded_matrix<float,4,4>(){}
	template<typename T>
	Matrix4(const boost::numeric::ublas::bounded_matrix<T,4,4>& t):boost::numeric::ublas::bounded_matrix<float,4,4>(t){}
	~Matrix4(){}

	Matrix4 Inverse()const{
		return Matrix4(Matrix4::Inverse<float>(*this));
	}
	template<typename T>
	static boost::numeric::ublas::matrix<T> Inverse(const boost::numeric::ublas::matrix<T> t){
		using namespace boost::numeric::ublas;
		typedef permutation_matrix<std::size_t> pmatrix;
		boost::numeric::ublas::matrix<T> inverse(t.size1(), t.size2());

		// create a working copy of the input
		boost::numeric::ublas::matrix<T> A(t);
		// create a permutation matrix for the LU-factorization
		pmatrix pm(A.size1());

		// perform LU-factorization
		int res = lu_factorize(A,pm);
			if( res != 0 ) throw("Matrix4::Invert() - Could not LU-factorize matrix.");

		// create identity matrix of "inverse"
		inverse.assign(boost::numeric::ublas::identity_matrix<T>(A.size1()));

		// backsubstitute to get the inverse
		lu_substitute(A, pm, inverse);

		return inverse;

	}

	template<typename T>
	static boost::numeric::ublas::bounded_matrix<T,4,4> Inverse(const boost::numeric::ublas::bounded_matrix<T,4,4> t){
		using namespace boost::numeric::ublas;
		typedef permutation_matrix<std::size_t> pmatrix;
		boost::numeric::ublas::bounded_matrix<T,4,4> inverse;

		// create a working copy of the input
		boost::numeric::ublas::bounded_matrix<T,4,4> A(t);
		// create a permutation matrix for the LU-factorization
		pmatrix pm(A.size1());

		// perform LU-factorization
		int res = lu_factorize(A,pm);
			if( res != 0 ) throw("Matrix4::Invert() - Could not LU-factorize matrix.");

		// create identity matrix of "inverse"
		inverse.assign(boost::numeric::ublas::identity_matrix<T>(A.size1()));

		// backsubstitute to get the inverse
		lu_substitute(A, pm, inverse);

		return inverse;

	}
};
/*
Matrix4 QuaternionToRotationMatrix(const boost::math::quaternion<float> q){
	Matrix4 r;
	r(0,0) = q.R_component_1()*q.R_component_1() + q.R_component_2()*q.R_component_2() - q.R_component_3()*q.R_component_3() - q.R_component_4()*q.R_component_4();
	r(0,1) = 2*q.R_component_2()*q.R_component_2()*q.R_component_3()*q.R_component_3() - 2*q.R_component_1()*q.R_component_1()*q.R_component_4()*q.R_component_4();
	r(0,2) = 2*q.R_component_1()*q.R_component_1()*q.R_component_3()*q.R_component_3() + 2*q.R_component_2()*q.R_component_2()*q.R_component_4()*q.R_component_4();
	r(0,3) = 0;
	r(1,0) = 2*q.R_component_1()*q.R_component_1()*q.R_component_4()*q.R_component_4() + 2*q.R_component_2()*q.R_component_2()*q.R_component_3()*q.R_component_3();
	r(1,1) = q.R_component_1()*q.R_component_1() - q.R_component_2()*q.R_component_2() + q.R_component_3()*q.R_component_3() - q.R_component_4()*q.R_component_4();
	r(1,2) = 2*q.R_component_3()*q.R_component_3()*q.R_component_4()*q.R_component_4() + 2*q.R_component_1()*q.R_component_1()*q.R_component_2()*q.R_component_2();
	r(1,3) = 0;
	r(2,0) = 2*q.R_component_2()*q.R_component_2()*q.R_component_4()*q.R_component_4() + 2*q.R_component_1()*q.R_component_1()*q.R_component_3()*q.R_component_3();
	r(2,1) = 2*q.R_component_1()*q.R_component_1()*q.R_component_2()*q.R_component_2() + 2*q.R_component_3()*q.R_component_3()*q.R_component_4()*q.R_component_4();
	r(2,2) = q.R_component_1()*q.R_component_1() - q.R_component_2()*q.R_component_2() - q.R_component_3()*q.R_component_3() + q.R_component_4()*q.R_component_4();
	r(2,3) = 0;
	r(3,0) = 0;
	r(3,1) = 0;
	r(3,2) = 0;
	r(3,3) = 1;
	return r;
}

Matrix4 VectorToTranslationMAtrix(const Vector3& v){
	Matrix4 t;
	t(0,0) = 1.0f;
	t(1,1) = 1.0f;
	t(2,2) = 1.0f;
	t(3,3) = 1.0f;

	t(0,3) = v[0];
	t(1,3) = v[1];
	t(2,3) = v[2];

	return t;
}*/

#endif /* MATH_HPP_ */
