#ifndef _VEC_H_
#define _VEC_H_

#include <math.h>

template<class T, unsigned SIZE> class CMatrix;

// Vector class for SIMPLE data types
template <class T, unsigned SIZE>
class CVector {

public:

	// standard constructor
	CVector<T,SIZE> ()  {
		// initialize all elements with zero
		for (int i=0; i<SIZE; i++)
			m_atData[i] = T(0);
	}

	// construction with data array
	CVector<T, SIZE> (const T atData[SIZE]) {
		for (int i=0; i<SIZE; i++)
			m_atData[i] = atData[i];		
	}

	// copy constructor
	CVector<T, SIZE> (const CVector<T, SIZE> &vec) {
		if (this==&vec)
			return; // nothing to do, it's me

		for (int i=0; i<SIZE; i++)
			m_atData[i] = vec.m_atData[i];
	}

	// destructor
	~CVector () {
		// nothing to do here ...
	}

	void setData (const T atData[SIZE]) {
		for (int i=0; i<SIZE; i++)
			m_atData[i] = atData[i];		
	}

	void getData (T atData[SIZE]) {
		for (int i=0; i<SIZE; i++)
			atData[i] = m_atData[i];		
	}

	unsigned getDimension () {
		return SIZE;
	}

	CVector<T, SIZE> &operator = (const CVector<T, SIZE> &vec) {
		if (this==&vec)
			return (*this); // ok, it's me, so no L-value action

		for (int i=0; i<SIZE; i++) // not me, so L-Value action: copy data
			m_atData[i] = vec.m_atData[i];

		return (*this); // also an R-value in e.g
						// vec1    =       vec2      = vec3;
						// L-Value = R-Value/L-Value = R-Value
	}

	CVector<T, SIZE> &operator = (const T atData[SIZE]) {
		for (int i=0; i<SIZE; i++) // not me, so L-Value action: copy data
			m_atData[i] = atData[i];

		return (*this); // also an R-value in e.g.
						// vec1  = vec2  +   (vec2=atData); // parenthesis () needed to evaluate expression vec2=atData
						// L-Val = R-Val +       R-Val
						//   "   =   "   + (L-Val = R-Val)
	}

	// usage of operator:
	// vec(i) = var;	// 0 <= i <= SIZE-1
	// var = vec(i);
	// vec1(i) = vec2(j);
	T &operator () (unsigned i) {
		if (i>=SIZE)
			i = SIZE-1;		// !!! operator clips index ...	
		return m_atData[i]; // ... so we can safely return a reference
	}

	T operator () (unsigned i) const {
		if (i>=SIZE)
			i = SIZE-1;
		return m_atData[i];
	}

	void operator += (const CVector<T, SIZE> &vec) {
		for (int i=0; i<SIZE; i++)
			m_atData[i] += vec.m_atData[i];
	}

	CVector<T, SIZE> operator + (const CVector<T, SIZE> &vec) {
		CVector<T, SIZE> buf (m_atData);
		for (int i=0; i<SIZE; i++)
			buf.m_atData[i] += vec.m_atData[i];
		return buf;
	}

	void operator -= (const CVector<T, SIZE> &vec) {
		for (int i=0; i<SIZE; i++)
			m_atData[i] -= vec.m_atData[i];
	}
	
	CVector<T, SIZE> operator - (const CVector<T, SIZE> &vec) {
		CVector<T, SIZE> buf (m_atData);
		for (int i=0; i<SIZE; i++)
			buf.m_atData[i] -= vec.m_atData[i];
		return buf;
	}
	

	// Kreuzprodukt: nur für CVec3+Hom definiert!
	// Homogene Koordinate wird ignoriert und auf 1 gesetzt.
	CVector<T, 4> crossH (const CVector<T, 4> &vec) {
		T atBuffer[4];
		atBuffer[0]= m_atData[1] * vec.m_atData[2];
		atBuffer[0]-= m_atData[2] * vec.m_atData[1];
		atBuffer[1]= m_atData[0] * vec.m_atData[2];
		atBuffer[1]-= m_atData[2] * vec.m_atData[0];
		atBuffer[2]= m_atData[0] * vec.m_atData[1];
		atBuffer[2]-= m_atData[1] * vec.m_atData[0];
		atBuffer[3]= 1;
		return CVector<T, 4> (atBuffer);
	}
	
	// Kreuzprodukt: nur für CVec3 definiert!
	CVector<T, 3> cross (const CVector<T, 3> &vec) {
		T atBuffer[3];
		atBuffer[0]= m_atData[1] * vec.m_atData[2];
		atBuffer[0]-= m_atData[2] * vec.m_atData[1];
		atBuffer[1]= m_atData[0] * vec.m_atData[2];
		atBuffer[1]-= m_atData[2] * vec.m_atData[0];
		atBuffer[2]= m_atData[0] * vec.m_atData[1];
		atBuffer[2]-= m_atData[1] * vec.m_atData[0];
		return CVector<T, 3> (atBuffer);
	}
	
	// Skalarprodukt zweier beliebiger Vektoren gleicher Dimension.
	double dot (const CVector<T, SIZE> &vec) {
		double ret= 0;
		for (int i=0; i < SIZE; i++)
			ret += m_atData[i] * vec.m_atData[i];
		return ret;
	}

	// Norm: nur für CVec3+Hom definiert!
	// Homogene Koordinate wird zunaechst auf 1 normiert, 
	// anschliessend werden die uebrigen koordinaten als CVec3 auf 1 normiert.
	const CVector<T, 4> normH () {
		T atBuffer[4];

		// hom 1
		T d= 0;
		if (m_atData[3] != 1) {
			d= m_atData[3];
			for (int i=0; i<3; i++)
				atBuffer[i] = m_atData[i] / d;
		}
		atBuffer[3]= 1;

		//laenge 1
		for (int i=0; i<3; i++)
			d += m_atData[i]*m_atData[i];
		d= sqrt(d);
		
		for (int i=0; i<3; i++)
			atBuffer[i] = m_atData[i] / d;

		return CVector<T, 4> (atBuffer);
	}

	// Normiert die Laenge eines beliebigen Vektors auf 1.
	const CVector<T, SIZE> norm () {
		T atBuffer[SIZE];

		//laenge 1
		double d= 0;
		for (int i=0; i < SIZE; i++)
			d += m_atData[i] * m_atData[i];
		d= sqrt(d);
		
		for (int i=0; i<SIZE; i++)
			atBuffer[i] = static_cast<T>(static_cast<double>(m_atData[i]) / d);

		return CVector<T, SIZE> (atBuffer);
	}

	// liefert die laenge dieses vektors
	double length() {
		double d= 0;
		for (int i=0; i < SIZE; i++)
			d += m_atData[i] * m_atData[i];
		return sqrt(d);
	}

	bool isNull() {
		bool ret= true;
		for (int i= 0; i < SIZE && ret; i++)
			if (m_atData[i] != 0) ret= false;
		return ret;
	}

	bool isReal() {
		bool ret= true;
		for (int i= 0; i < SIZE && ret; i++)
			ret= (m_atData[i] == m_atData[i]);
		return ret; 
    }

	void print(char* name= "v") {
		std::cout << name << " = ";
		std::cout << "(" << m_atData[0] << ", " << m_atData[1] << ", " << m_atData[2] << ")" << std::endl;
	}
	
	CVector<T, SIZE> operator - () {
		T atBuffer[SIZE];
		for (int i=0; i<SIZE; i++)
			atBuffer[i] = -m_atData[i];
		return CVector<T, SIZE> (atBuffer);
	}

	T operator * (const CVector<T, SIZE> & vec) {
		T dp = T(0);
		for (int i=0; i<SIZE; i++)
			dp += m_atData[i]*vec.m_atData[i];
		return dp;
	}

	void operator *= (T tScale) {
		for (int i=0; i<SIZE; i++)
			m_atData[i] *= tScale;
	}

	CVector<T, SIZE> operator * (T tScale) {
		T atBuffer[SIZE];
		for (int i=0; i<SIZE; i++)
			atBuffer[i] = m_atData[i]*tScale;
		return CVector<T, SIZE> (atBuffer);
	}

	CVector<T, SIZE> operator * (const CMatrix<T, SIZE> &mat) {
		CVector<T, SIZE> vec;
		for (int j=0; j<SIZE; j++)
			for (int i=0; i<SIZE; i++)
				vec(j) += m_atData[i]*mat(i,j);
		return vec;
	}

private:

	T m_atData[SIZE];

}; // class CVector

// some common vector classes (abbr. names)
typedef CVector<float, 2> CVec2f;
typedef CVector<float, 3> CVec3f;
typedef CVector<float, 4> CVec4f;

typedef CVector<double, 2> CVec2d;
typedef CVector<double, 3> CVec3d;
typedef CVector<double, 4> CVec4d;

typedef CVector<int, 2> CVec2i;
typedef CVector<int, 3> CVec3i;
typedef CVector<int, 4> CVec4i;

typedef CVector<bool, 2> CVec2b;
typedef CVector<bool, 3> CVec3b;
typedef CVector<bool, 4> CVec4b;

#endif
