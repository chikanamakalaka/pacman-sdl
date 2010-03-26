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
public:
	class SplineKey{
	private:
		const boost::math::quaternion<float> orientation;
		const Vector3 position;
	public:
		SplineKey(const boost::math::quaternion<float>& orientation, const Vector3& position):
		orientation(orientation),
		position(position){

		}
		Matrix4 GetMatrix4()const{
			return Matrix4();
		}
		boost::math::quaternion<float> GetOrientation()const{
			return orientation;
		}
		Vector3 GetPosition()const{
			return position;
		}
	};
private:
  int degree;
  std::set<float> knots;
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
  Vector3 Interpolate(float t, bool loop)const{
    //looping?
    if(loop && t>*(--knots.end())){
      t = std::fmod(t, *(--knots.end()));
    }
    //no degree set? set it
    int degree = this->degree;
    if(degree == 0){
      degree = controlpoints.size() - 1;
    }
    
    std::set<float> knots(this->knots);
    //http://www.rhino3d.com/nurbs.htm
    //The knots are a list of degree+N-1 numbers, where N is the number of control points. 
    float terminalknotsneeded = knots.size()-(degree+controlpoints.size()-1);
    if(knots.size()>0){
      //duplicate knots at the beginning
      for(int i =0; i<std::ceil(terminalknotsneeded/2); i++){
        knots.insert(*knots.begin());
      }
      //duplicate knots at the end
      for(int i =0; i<std::floor(terminalknotsneeded/2); i++){
        knots.insert(*(--knots.end()));
      }
    }else{
      //no existing knots, set all knots to 0.0
      for(int i =0; i<terminalknotsneeded; i++){
        knots.insert(0.0f);
      }
    }
    
    return deBoor(t, degree, knots, controlpoints);
  }
  //http://chi3x10.wordpress.com/2009/10/18/de-boor-algorithm-in-c/
  static Vector3 deBoor(float x, int degree, const std::set<float>& knots, const std::vector<Vector3>& controlpoints){
    int i;
	std::set<float>::const_iterator itr = knots.begin();
    for(; itr != knots.end(); itr++){
    	std::set<float>::const_iterator itr1 = itr;
    	itr1++;
		if(itr1 != knots.end()){
			if(x>=*itr && x<(*itr1)){
				i = static_cast<int>(*itr);
			}
		}
	}
    return deBoor(x, degree, knots, controlpoints, i, degree+1);
  }
  //i = closest knot
  static Vector3 deBoor(float x, int degree, const std::set<float>& knots, const std::vector<Vector3>& controlpoints, int i, int order){
    // Please see wikipedia page for detail
    // note that the algorithm here kind of traverses in reverse order
    // comapred to that in the wikipedia page
    if( degree == 0)
      return controlpoints[i];
    else
    {
      float k0, k1;
      std::set<float>::const_iterator itr =  knots.begin();
      for(int l = 0;itr != knots.end(); itr++, l++){
        if(l == i){
          k0 = *itr;
        }
        if(l == i+order-degree){
          k1 = *itr;
        }
      }
      double alpha = (x-k0)/(k1-k0);
      boost::numeric::ublas::vector<float> v = ((1.0 - alpha) * deBoor(x, degree-1, knots, controlpoints, i-1, order)) + (alpha * deBoor(x, degree-1, knots, controlpoints, i, order));
      return Vector3(v);
    }
  }
};

class RotationAnimation:public IAnimation{
private:
	const boost::math::quaternion<float> quaternion;
public:
	RotationAnimation(const boost::math::quaternion<float>& quaternion):quaternion(quaternion){}

	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		PositionProperty& position = scenenode.GetSceneNodeProperty<PositionProperty>("position");

	}
	const std::string GetType()const{
		return "rotationanimation";
	}
};

class SplineAnimation:public IAnimation{
private:
	const Spline spline;
	const Spline orientationspline;
	State state;
	float elapsed;
	bool loop;
public:
	SplineAnimation(const Spline& spline, const Spline& orientationspline, bool loop):spline(spline), orientationspline(orientationspline), state(Paused), elapsed(0.0f), loop(loop){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		if(state == Playing){
			elapsed+=dtf;
			Vector3 position = spline.Interpolate(elapsed, loop);
			Vector3 orientation = orientationspline.Interpolate(elapsed, loop);
			PositionProperty& positionproperty = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			//positionproperty.SetPosition(splinekey.GetMatrix4());
		}
	}
	const std::string GetType()const{
		return "splineanimation";
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
	std::map<float, std::vector<TextureAnimationKey> > textureanimationkeys;
	bool loop;
public:
	TextureAnimation(const std::map<float, std::vector<TextureAnimationKey> >& textureanimationkeys, bool loop):textureanimationkeys(textureanimationkeys), loop(loop){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){

	}
	const std::string GetType()const{
		return "textureanimation";
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
	}
	virtual void Pause(){
	}
	virtual void Stop(){
	}
};



#endif /* ANIMATIONS_HPP_ */
