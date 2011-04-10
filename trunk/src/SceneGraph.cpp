/*
 * SceneGraph.cpp
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */

#include "SceneGraph.h"

SceneGraph::SceneGraph(const std::string& name):SignalSubscriber(*new SignalBroker, "", "SceneGraph"),name(name), root(new SceneGraph::SceneNode("root", *this)){

}

SceneGraph::SceneGraph(SignalBroker& signalbroker):SignalSubscriber(signalbroker, "", "SceneGraph"),root(new SceneGraph::SceneNode("root", *this)), signalbrokerptr(false){

}
SceneGraph::SceneGraph(const SceneGraph& scenegraph):SignalSubscriber(*new SignalBroker(), "", "SceneGraph"), signalbrokerptr(true){

}
SceneGraph::~SceneGraph(){

	VisitNodesWithPushPop(PrintNodeTree());
	if(signalbrokerptr){
		delete &(GetSignalBroker());
	}
}
SceneGraph& SceneGraph::operator=(const SceneGraph& scenegraph){
	return *this;
}
const std::string& SceneGraph::GetName()const{
	return name;
}
SceneGraph::SceneNode& SceneGraph::GetRoot(){
	return *root;
}
const SceneGraph::SceneNode& SceneGraph::GetRoot()const{
	return *root;
}

SceneGraph::SceneNode& SceneGraph::GetNodeByPath(const std::string& path){
	return *(GetRoot().GetDescendantNodePtrByPath(path));
}
const SceneGraph::SceneNode& SceneGraph::GetNodeByPath(const std::string& path)const{
	return *(GetRoot().GetDescendantNodePtrByPath(path));
}


SceneGraph::SceneNodePtr SceneGraph::GetNodePtrByPath(const std::string& path){
	return GetRoot().GetDescendantNodePtrByPath(path);
}
SceneGraph::ConstSceneNodePtr SceneGraph::GetNodePtrByPath(const std::string& path)const{
	return GetRoot().GetDescendantNodePtrByPath(path);
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

