/*
 * Animations.hpp
 *
 *  Created on: Mar 12, 2010
 *      Author: aaronsantos
 */

#ifndef ANIMATIONS_HPP_
#define ANIMATIONS_HPP_

#include <cmath>


class Spline{
private:
	int degree;
	std::multiset<float> knots;
	std::vector<Vector3> controlpoints;
public:
	Spline():degree(0){}
	Spline(int degree):degree(degree){}
	virtual ~Spline(){}
	void AddKnot(float knot){
		knots.insert(knot);
	}
	void AddControlPoint(const Vector3& controlpoint){
		controlpoints.push_back(controlpoint);
	}
	Vector3 InterpolatePosition(float t, bool loop)const{
		//looping?
		if(loop && t>*(--knots.end())){
		  t = std::fmod(t, *(--knots.end()));
		}
		//no degree set? set it
		int degree = knots.size()-controlpoints.size()-1;
		//if(degree < 2){
		//  degree = controlpoints.size()-1;
		//}
		int order = degree + 1;

		/*
		std::multiset<float> knots(this->knots);
		//http://www.rhino3d.com/nurbs.htm
		//The knots are a list of degree+N+1 numbers, where N is the number of control points.
		float terminalknotsneeded = static_cast<float>((degree+controlpoints.size()+1) - knots.size())/2.0f;
		if(knots.size()>0){
		  //duplicate knots at the beginning
		  for(int i =0; i<std::ceil(terminalknotsneeded); i++){
			knots.insert(*knots.begin());
		  }
		  //duplicate knots at the end
		  for(int i =0; i<std::floor(terminalknotsneeded); i++){
			knots.insert(*(--knots.end()));
		  }
		  //clamped b-spline terminal knots must have multiplicity degree+1
		  {
			  int proximalknotmultiplicitydifference = degree;
			  std::multiset<float>::const_iterator itr = knots.begin();
			  int proximalknotvalue = *itr;
			  for(; itr!=knots.end(); itr++){
				  if(*itr==proximalknotvalue){
					  proximalknotmultiplicitydifference--;
				  }else{
					  break;
				  }
			  }
			  for(;proximalknotmultiplicitydifference>0; proximalknotmultiplicitydifference--){
				  knots.insert(*knots.begin());
			  }
		  }
		  {
			  int terminalknotmultiplicitydifference = degree;
			  std::multiset<float>::const_reverse_iterator itr = knots.rbegin();
			  int terminalknotvalue = *itr;
			  for(; itr!=knots.rend(); itr++){
				  if(*itr==terminalknotvalue){
					  terminalknotmultiplicitydifference--;
				  }else{
					  break;
				  }
			  }
			  for(;terminalknotmultiplicitydifference>0; terminalknotmultiplicitydifference--){
				  knots.insert(*knots.rbegin());
			  }
		  }

		  std::cout << "inserted knots: ";
		  std::copy(knots.begin(), knots.end(), std::ostream_iterator<float>(std::cout, " "));
		  std::cout << std::endl;
		}else{
		  //no existing knots, set all knots to 0.0
		  for(int i =0; i<terminalknotsneeded; i++){
			knots.insert(0.0f);
		  }
		}*/
		return deBoor(t, order, knots, controlpoints);
	}
	//http://chi3x10.wordpress.com/2009/10/18/de-boor-algorithm-in-c/
	static Vector3 deBoor(float x, int order, const std::multiset<float>& knots, const std::vector<Vector3>& controlpoints){
		Vector3 result;
		result(0) = result(1) = result(2) = 0.0f;
		std::vector<Vector3>::const_iterator controlitr = controlpoints.begin();
		std::vector<float> t;
		std::copy(knots.begin(),knots.end(),std::back_inserter(t));

		for(int i = 0; controlitr != controlpoints.end(); controlitr++, i++){
			float basis = deBoorBasis(i, order, t, x);
			result += *controlitr * basis;

		}
		return result;
	}
	static float deBoorBasis(int i, int k, const std::vector<float>& t, float u){

		if(k == 1){
			if(t[i]<= u && u<= t[i+1]){
				return 1.0f;
			} else{
				return 0.0f;
			}
		} else{
			if(u>9.5f){
				int s = t.size();
				s++;
			}
			float n0 = (u-t[i]);
			float d0 = (t[i+k-1]-t[i]);
			float n1 = (t[i+k]-u);
			float d1 = (t[i+k]-t[i+1]);
			float a0 = d0>0.0f ? n0/d0*deBoorBasis(i,   k-1, t, u) : 0.0f;
			float a1 = d1>0.0f ? n1/d1*deBoorBasis(i+1, k-1, t, u) : 0.0f;
			return a0 + a1;
		}
	}
	//i = closest knot
	static Vector3 deBoor(float x, unsigned int degree, const std::multiset<float>& knots, const std::vector<Vector3>& controlpoints, unsigned int i, unsigned int order){
		// Please see wikipedia page for detail
		// note that the algorithm here kind of traverses in reverse order
		// comapred to that in the wikipedia page
		if( degree == 0)
		  return controlpoints[i<0?0:i>=controlpoints.size()?controlpoints.size()-1:i];
		else
		{
		  float k0, k1;
		  std::multiset<float>::const_iterator itr =  knots.begin();
		  for(unsigned int l = 0;itr != knots.end(); itr++, l++){
			if(l == i){
			  k0 = *itr;
			}
			if(l == i+order-degree){
			  k1 = *itr;
			}
		  }
		  double alpha = (x-k0)/(k1-k0);
		  if(k0==k1){
			  alpha = 0.0f;
		  }
		  Vector3 v0(deBoor(x, degree-1, knots, controlpoints, i-1, order));
		  Vector3 v1(deBoor(x, degree-1, knots, controlpoints, i, order));

		  v0*=(1.0f-alpha);
		  v1*=alpha;
		  boost::numeric::ublas::vector<float> v(v0+v1);
		  return Vector3(v);
		}
	}
};

class RotationAnimation:public IAnimation{
private:
	std::string name;
	bool loop;
	State state;
	float elapsed;
	std::map<float, boost::math::quaternion<float> > orientationkeys;
public:
	RotationAnimation(const std::string& name, bool loop):name(name), loop(loop), state(Paused), elapsed(0.0f){}
	void AddOrientationKey(float time, const boost::math::quaternion<float> orientation){
		orientationkeys.insert(std::map<float, boost::math::quaternion<float> >::value_type(time, orientation));
	}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		if(state == Playing){
			//looping?
			if(loop && tf>(--orientationkeys.end())->first){
			  tf = std::fmod(tf, (--orientationkeys.end())->first);
			}
			PositionProperty& position = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			Matrix4& m = position.GetPosition();
			boost::math::quaternion<float> q = Interpolate(tf);
			//http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#From_a_quaternion_to_an_orthogonal_matrix
			float a = q.R_component_1();
			float b = q.R_component_2();
			float c = q.R_component_3();
			float d = q.R_component_4();
			m(0,0) = a * a + b * b - c * c - d * d;
			m(0,1) = 2 * b * c - 2 * a * d;
			m(0,2) = 2 * b * d + 2 * a * c;
			m(1,0) = 2 * b * c + 2 * a * d;
			m(1,1) = a * a - b * b + c * c - d * d;
			m(1,2) = 2 * c * d - 2 * a * b;
			m(0,1) = 2 * b * d - 2 * a * c;
			m(0,2) = 2 * c * d + 2 * a * b;
			m(1,1) = a * a - b * b - c * c + d * d;
		}
	}
	boost::math::quaternion<float> Interpolate(float t){
		if(orientationkeys.size() == 0){
			return boost::math::quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else if(orientationkeys.size() == 1){
			return orientationkeys.begin()->second;
		}
		else{
			std::map<float, boost::math::quaternion<float> >::const_iterator itr = orientationkeys.begin();
			for(; itr != orientationkeys.end() && itr->first < t; itr++);
			std::map<float, boost::math::quaternion<float> >::const_iterator itr2 = itr;
			itr2++;
			float a =  (t-itr->first)/(itr2->first - itr->first);
			boost::math::quaternion<float> q0 = itr->second;
			boost::math::quaternion<float> q1 = itr2->second;
			return q0 * boost::math::pow((boost::math::pow(q1, -1) * q1), a);
		}
	}
	const std::string GetType()const{
		return "rotationanimation";
	}
	const std::string& GetName()const{
		return name;
	}
	State GetState()const{
		return state;
	}
	virtual void Play(){
		state = Playing;
	}
	virtual void Pause(){
		state = Paused;
	}
	virtual void Stop(){
		state = Stopped;
		elapsed = 0.0f;
	}
};

class SplineAnimation:public IAnimation{
private:
	const Spline spline;
	std::string name;
	State state;
	float elapsed;
	bool loop;
public:
	SplineAnimation(const Spline& spline, const std::string& name, bool loop):spline(spline), name(name), state(Paused), elapsed(0.0f), loop(loop){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		if(state == Playing){
			elapsed+=dtf;
			Vector3 position = spline.InterpolatePosition(elapsed, loop);
			PositionProperty& positionproperty = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			Matrix4& m = positionproperty.GetPosition();
			m(0,3) = position(0);
			m(1,3) = position(1);
			m(2,3) = position(2);
		}
	}
	const std::string GetType()const{
		return "splineanimation";
	}
	const std::string& GetName()const{
		return name;
	}
	State GetState()const{
		return state;
	}
	virtual void Play(){
		state = Playing;
	}
	virtual void Pause(){
		state = Paused;
	}
	virtual void Stop(){
		state = Stopped;
		elapsed = 0.0f;
	}
};

class TextureAnimationKey{
private:
	 Vector2 coords;
public:
	TextureAnimationKey(const Vector2& coords):coords(coords){}
	const Vector2& GetTextureCoordinates()const{
		return coords;
	}
};

class TextureAnimation:public IAnimation{
private:
	State state;
	std::map<float, std::vector<TextureAnimationKey> > textureanimationkeys;
	std::string name;
	bool loop;
public:
	TextureAnimation(const std::map<float, std::vector<TextureAnimationKey> >& textureanimationkeys, const std::string& name,  bool loop):
		state(Paused), textureanimationkeys(textureanimationkeys), name(name), loop(loop){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){

	}
	const std::string GetType()const{
		return "textureanimation";
	}
	const std::string& GetName()const{
		return name;
	}
	State GetState()const{
		return state;
	}
	bool Loops()const{
		return loop;
	}
	std::map<float, std::vector<TextureAnimationKey> > GetTextureAnimationKeys()const{
		return textureanimationkeys;
	}
	void AddKey(float time, const std::vector<TextureAnimationKey>& key){
		textureanimationkeys.insert(std::pair<float, std::vector<TextureAnimationKey> >(time, key));
	}
	virtual void Play(){
		state=Playing;
	}
	virtual void Pause(){
		state=Paused;
	}
	virtual void Stop(){
		state=Stopped;
	}
};



#endif /* ANIMATIONS_HPP_ */
