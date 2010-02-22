/*
 * SceneGraph.cpp
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */

#include "SceneGraph.h"

SceneGraph::SceneGraph():SignalSubscriber(*new SignalBroker, "", "SceneGraph"),root(new SceneGraph::SceneNode("root", *this)){

}

SceneGraph::SceneGraph(SignalBroker& signalbroker):SignalSubscriber(signalbroker, "", "SceneGraph"),root(new SceneGraph::SceneNode("root", *this)){

}
SceneGraph::SceneGraph(const SceneGraph& scenegraph):SignalSubscriber(*new SignalBroker, "", "SceneGraph"){

}
SceneGraph::~SceneGraph(){
}
SceneGraph& SceneGraph::operator=(const SceneGraph& scenegraph){
	return *this;
}
SceneGraph::SceneNode& SceneGraph::GetRoot(){
	return *root;
}
const SceneGraph::SceneNode& SceneGraph::GetRoot()const{
	return *root;
}

const SceneGraph::SceneNode& SceneGraph::GetNodeByPath(const std::string& path){
	if(scenenodesbyname.find(path)!=scenenodesbyname.end()){
		return *(scenenodesbyname.find(path)->second);
	} else{
		throw "SceneNodeDoesNotExistAtPath";
	}
}


SceneGraph::SceneNodePtr SceneGraph::GetNodePtrByPath(const std::string& path){
	if(scenenodesbyname.find(path)!=scenenodesbyname.end()){
		return scenenodesbyname.find(path)->second;
	} else{
		throw "SceneNodeDoesNotExistAtPath";
	}
}
bool SceneGraph::HasNodeProcessor(const std::string& propertykey){
	return nodeprocessordependencies.find(propertykey)!=nodeprocessordependencies.end();
}
void SceneGraph::RegisterNodeProcessor(const std::string& propertykey, const std::list<std::string>& dependencies, SceneGraph::NodeProcessor processor){
	nodeprocessordependencies[propertykey] = dependencies;
	SignalSubscriber::ConnectToSignal<void(SceneNode&)>("/scenegraph/"+propertykey, processor);
}
void SceneGraph::ProcessNode(SceneGraph::SceneNode& scenenode, const std::string& propertykey){
	signalbroker.InvokeSignal<void(SceneNode&)>("/scenegraph/"+propertykey, scenenode);
}

