/*
 * AnimationController.hpp
 *
 *  Created on: May 13, 2009
 *      Author: asantos
 */
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
	SplineKey GetCurrentKey(float time){
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
				return (--keys.end())->second;
			}else{
				//else between two keys
				std::map<float, SplineKey>::const_iterator itr = keys.begin();
				//find key immediately before and immediately after
				std::map<float, SplineKey>::const_iterator before;
				std::map<float, SplineKey>::const_iterator after;
				for(;itr!=keys.end(); itr++){
					if(itr->first > time){
						after = itr;
						before = (++itr);
						break;
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

			}
		}
		//if between first and last
		//get key before
		//get key after
		//interpolate between before and after

		//create and return interpolated key


	}

};

class RotationAnimation:public IAnimation{
private:
	const boost::math::quaternion<float> quaternion;
public:
	RotationAnimation(const boost::math::quaternion<float>& quaternion):quaternion(quaternion){}

	virtual void Animate(PositionProperty& positionproperty, float tf, float dtf){

	}

};

class SplineAnimation:public IAnimation{
private:
	const Spline spline;
public:
	SplineAnimation(const Spline& spline):spline(spline){}
	virtual void Animate(PositionProperty& positionproperty, float tf, float dtf){
		//Matrix4& position = positionproperty.GetPosition();


	}
};


class AnimationController:public SignalSubscriber{
public:
	typedef std::map<std::string, SceneGraph*> SceneGraphs_Tp;
private:

	SignalBroker& signalbroker;
	SceneGraphs_Tp scenegraphs;
	boost::weak_ptr<SceneGraph> selectedscenegraph;

	float tf;
	float dtf;

public:
	AnimationController(SignalBroker& signalbroker):
		SignalSubscriber(signalbroker, "", "AnimationController"),
		signalbroker(signalbroker){

		SignalSubscriber::ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/scenegraphcontroller/getselectedscenegraph",
			boost::bind(&AnimationController::GetSelectedSceneGraph, this, _1));

		SignalSubscriber::ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&AnimationController::AnimateSceneGraph, this, _1, _2));


	}
	void GetSelectedSceneGraph(boost::shared_ptr<SceneGraph> selectedscenegraph){
		//get the currently selected scenegraph
		this->selectedscenegraph = selectedscenegraph;
	}

	void AnimateSceneGraph(long t, long dt){
		if(boost::shared_ptr<SceneGraph> scenegraph = selectedscenegraph.lock()){
			tf = t/1000.0f;
			dtf = dt/1000.0f;

			scenegraph->VisitNodes(boost::bind(&AnimationController::AnimateSceneNode, this, _1, tf, dtf));
		}
	}

	void AnimateSceneNode(SceneGraph::SceneNode& scenenode, float tf, float dtf){
		if(scenenode.HasSceneNodeProperty("position") && scenenode.HasSceneNodeProperty("animation")){
			PositionProperty& position = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			IAnimation& animation = scenenode.GetSceneNodeProperty<AnimationProperty>("animation").GetAnimation();

			animation.Animate(position, tf, dtf);
		}
	}

};
