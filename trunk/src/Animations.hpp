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
	Spline(){}
  Spline(int degree):degree(degree){}
	virtual ~Spline(){}
	void AddKnot(float knot){
		knots.insert(knot);
	}
  void AddControlPoint(const Vector3& controlpoint){
    controlpoints.push_back(controlpoint);
  }
  Vector3 Interpolate(float t, bool loop)const{
    std::set<float> knots(this->knots);
    float terminalknots = controlpoints.size()-knots.size()+degree;
    if(knots.size()>0){
      //duplicate knots at the beginning
      for(int i =0; i<std::ceil(terminalknots/2); i++){
        knots.insert(*knots.begin());
      }
      //duplicate knots at the end
      for(int i =0; i<std::floor(terminalknots/2); i++){
        knots.insert(*(--knots.end()));
      }
    }else{
      //no existing knots, set all knots to 0.0
      for(int i =0; i<terminalknots; i++){
        knots.insert(0.0f);
      }
    }
    //looping?
    if(loop && t>*(--knots.end())){
      t = std::fmod(t, *(--knots.end()));
    }
    return deBoor(t, knots, controlpoints);
  }
  //http://chi3x10.wordpress.com/2009/10/18/de-boor-algorithm-in-c/
  Vector3 deBoor(float x, const std::set<float>& knots, const std::vector<Vector3>& controlpoints)const{
    int i,k;
    std::set<float>::const_iterator itr = knots.begin():
    for(; itr != knots.end(); itr++){
      if(itr+1 != knots.end()){
        if(x>=*itr ** x<*(itr+1)){
          i = std::static_cast<int>(*itr);
      }
    }
    k = knots.size() - controlpoints.size();
    return deBoor(time, knots, controlpoints, i, k-1, k);
  }
  Vector3 deBoor(float x, const std::set<float>& knots, const std::vector<Vector3>& controlpoints, i, j, k){
    // Please see wikipedia page for detail
    // note that the algorithm here kind of traverses in reverse order
    // comapred to that in the wikipedia page
    if( j == 0)
      return ctrlPoints[i];
    else
    {
      float k0, k1;
      std::set<float>::const_iterator itr =  knots.begin();
      for(int l = 0;itr != knots.end(); itr++, l++){
        if(l == i){
          k0 = *itr;
        }
        if(l == i+k-j){
          k1 = *itr;
        }
      }
      double alpha = (x-k0)/(k1-k0);
      return (1 - alpha) * deBoor(x, knots, controlpoints, i-1, j-1, k)+ alpha * deBoor(x, knots, controlpoints, i, j-1, k);
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
	State state;
	float elapsed;
	bool loop;
public:
	SplineAnimation(const Spline& spline, bool loop):spline(spline), state(Paused), elapsed(0.0f), loop(loop){}
	virtual void Animate(SceneNode& scenenode, float tf, float dtf){
		if(state == Playing){
			elapsed+=dtf;
			Spline::SplineKey splinekey = spline.GetCurrentKey(elapsed, loop);
			PositionProperty& positionproperty = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			positionproperty.SetPosition(splinekey.GetMatrix4());
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
