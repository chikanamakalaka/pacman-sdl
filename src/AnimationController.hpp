/*
 * AnimationController.hpp
 *
 *  Created on: May 13, 2009
 *      Author: asantos
 */

#ifndef ANIMATIONCONTROLLER_HPP_
#define ANIMATIONCONTROLLER_HPP_


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
		if(scenenode.HasSceneNodeProperty("animations")){
			const std::map<std::string, boost::shared_ptr<IAnimation> >& animations = scenenode.GetSceneNodeProperty<AnimationsProperty>("animations").GetSelectedAnimations();
			std::map<std::string, boost::shared_ptr<IAnimation> >::const_iterator itr = animations.begin();
			for(;itr != animations.end(); itr++){
				itr->second->Animate(scenenode, tf, dtf);
			}
		}

	}

};

#endif /* ANIMATIONCONTROLLER_HPP_ */
