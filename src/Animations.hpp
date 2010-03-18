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
	std::map<float, SplineKey> keys;
public:
	Spline(){}
	virtual ~Spline(){}
	void AddKey(float time, const SplineKey& key){
		keys.insert(std::pair<float, SplineKey>(time, key));
	}
  //http://chi3x10.wordpress.com/2009/10/18/de-boor-algorithm-in-c/
  SplineKey deBoor(float time, bool loop)const{
    std::vector<float> knots;
    std::vector<SplineKey> ctrlPoints;
    return deBoor(time, keys.size(), WhichInterval(time, knots, ti), time, knots, ctrlPoints);
  }
  SplineKey deBoor(int k,int degree, int i, double x, double* knots, Point *ctrlPoints){
    // Please see wikipedia page for detail
    // note that the algorithm here kind of traverses in reverse order
    // comapred to that in the wikipedia page
    if( k == 0)
      return ctrlPoints[i];
    else
    {
      double alpha = (x-knots[i])/(knots[i+degree+1-k]-knots[i]);
      return (deBoor(k-1,degree, i-1, x, knots, ctrlPoints)*(1-alpha )+deBoor(k-1,degree, i, x, knots, ctrlPoints)*alpha );
    }
  }
  int WhichInterval(double x, double *knot, int ti){
    for(int i=1;i<ti-1;i++)
    {
    if(x<knot[i])
    return(i-1);
    else if(x == knot[ti-1])
    return(ti-1);
    }
    return -1;
  }

	SplineKey GetCurrentKey(float time, bool loop)const{
		//if has keys
		if(keys.size() > 0){
			//if only one key, return that key
			if(keys.size() == 1){
				return keys.begin()->second;
			}
			//if before earliest time, return earliest key
			else if(time < keys.begin()->first){
				return keys.begin()->second;
			}
			//if after latest time, return last key
			else if(time > (--keys.end())->first){
				if(loop){
					//if looped animation and elapsed is past the end, loop the elapsed around to the start
					time = std::fmod(time, (--keys.end())->first);
				}else{
					return (--keys.end())->second;
				}
			}
      else{
        //else between two keys?
        std::map<float, SplineKey>::const_iterator itr = keys.begin();
        //perform linear interpolation between two keys
        //find key immediately before and immediately after
        std::map<float, SplineKey>::const_iterator p0;
        std::map<float, SplineKey>::const_iterator p1;
        for(;itr!=keys.end(); itr++){
          if(itr->first > time){
            p0 = itr;
            p1 = (++itr);
            break;
          }
        }
        if(keys.size()==2){
          
        }else{
          //perform quadratic interpolation
          std::map<float, SplineKey>::const_iterator p2 = p1;
          p2++;
          
          if(p2 == keys.end()){
            p2 = p1;
            p1 = p0;
          }
          
        }
        //slerp between orientations
        float scaleQ, scaleR;
        float t = (time-before->first) / (after->first - before->first);
        boost::math::quaternion<float> q = before->second.GetOrientation();
        boost::math::quaternion<float> r = after->second.GetOrientation();
        float cos_theta = boost::math::norm(q);
        float theta = acos(cos_theta);
        float invsin = 1.0 / sin(theta);
        float PIdiv2 = M_PI/2;

        boost::math::quaternion<float> val;

        if((1.0 + cos_theta > 0.001f)){
          if((1.0-cos_theta )> 0.001f){
            scaleQ = sin((1.0 - t)*theta)*invsin;
            scaleR = sin(t*theta)*invsin;
          }else{
            scaleQ = 1.0 - t;
            scaleR = t;
          }
          val = q*scaleQ + r* scaleR;
        }else{
          val = boost::math::quaternion<float>(r.R_component_1(), -r.R_component_1(), r.R_component_1(), -r.R_component_1());
          scaleQ = sin((1.0 - t) * PIdiv2);
          scaleR =sin(t * PIdiv2);
          val*=scaleR;
          q*=scaleQ;
          val+=val;
        }
        val = boost::math::norm(val);
        //interpolate between positions
        return SplineKey(Vector3(), val);
      }
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
