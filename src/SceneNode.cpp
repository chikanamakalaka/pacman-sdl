/*
 * SceneNode.cpp
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */

#include "SceneNode.h"
#include <vector>
#include <string>
#include <sstream>
#include <boost/tokenizer.hpp>

SceneNode::SceneNode(const std::string& name, SceneGraph& scenegraph):name(name), scenegraph(scenegraph){

}
SceneNode::SceneNode(const SceneNode& sceneNode):name(sceneNode.name), scenegraph(sceneNode.scenegraph){
	//clone children
	SceneNodes::const_iterator itr = sceneNode.children.begin();
	for(;itr!=sceneNode.children.end(); itr++){
		children.push_back((*itr)->Clone());
	}
	//clone properties
	SceneNodeProperties::const_iterator propertyitr = sceneNode.properties.begin();
	for(;propertyitr!=sceneNode.properties.end(); propertyitr++){
		properties.insert(SceneNodeProperties::value_type(propertyitr->first, propertyitr->second->Clone()));
	}

}
SceneNode::~SceneNode(){

}
SceneNode& SceneNode::operator=(const SceneNode& sceneNode){
	if(&sceneNode != this){
		children.clear();
		std::copy(sceneNode.children.begin(), sceneNode.children.end(), std::back_inserter(children));
		properties.clear();
		properties.insert(sceneNode.properties.begin(), sceneNode.properties.end());
	}
	return *this;
}
SceneNode::SceneNodePtr SceneNode::Clone()const{
	return SceneNodePtr(new SceneNode(*this));

}
SceneNodeProperty& SceneNode::AddSceneNodeProperty(const std::string& name, SceneNode::SceneNodePropertyPtr scenenodeproperty){
	if(properties.find(name)==properties.end()){
		properties.insert(SceneNodeProperties::value_type(name, scenenodeproperty));
		scenegraph.ProcessNode(*this, name);
		return *scenenodeproperty;
	}else{
		throw SceneNodePropertyAlreadyExists(name.c_str());
	}
}
SceneNodeProperty& SceneNode::GetSceneNodeProperty(const std::string& name)const{
	if(properties.find(name)!=properties.end()){
		return *properties.find(name)->second;
	}else{
		throw SceneNodePropertyDoesNotExist(name.c_str());
	}
}

void SceneNode::DeleteSceneNodeProperty(const std::string& name){
	if(properties.find(name)!=properties.end()){
		properties.erase(properties.find(name));
	}else{
		throw SceneNodePropertyDoesNotExist(name.c_str());
	}
}
bool SceneNode::HasSceneNodeProperty(const std::string& name)const{
	return properties.find(name)!=properties.end();
}
void SceneNode::ProcessNode(const std::string& name){
	scenegraph.ProcessNode(*this, name);
}


SceneNode& SceneNode::CreateChildNode(const std::string& name){
	SceneNodePtr scenenode = SceneNodePtr(new SceneNode(name, scenegraph));
	children.push_back(scenenode);
	return *scenenode;
}
SceneNode::SceneNodePtr SceneNode::CreateChildNodePtr(const std::string& name){
	SceneNodePtr scenenode = SceneNodePtr(new SceneNode(name, scenegraph));
	children.push_back(scenenode);
	return scenenode;
}

void SceneNode::AddChildNode(SceneNodePtr scenenode){
	children.push_back(scenenode);
}
void SceneNode::DeleteChildNode(SceneNodePtr scenenode){
	children.remove(scenenode);
}
void SceneNode::DeleteChildNodeByName(const std::string& name){
	try{
		DeleteChildNode(GetChildNodePtrByName(name));
	}catch(...){

	}
}
SceneNode::SceneNodes& SceneNode::GetChildNodes(){
	return children;
}
const SceneNode::SceneNodes& SceneNode::GetChildNodes()const{
	return children;
}
void SceneNode::CopyChildNodesToTargetParent(SceneNode& target)const{
	SceneNodes::const_iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		target.children.push_back((*itr)->Clone());
	}
}

void SceneNode::MoveChildNodeToTargetParent(SceneNodePtr scenenode, SceneNode& target){
	target.AddChildNode(scenenode);
	DeleteChildNode(scenenode);
}

void SceneNode::DeleteChildNodes(){
	SceneNodes::iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		(*itr)->DeleteChildNodes();
	}
	children.clear();
}


const std::string& SceneNode::GetName()const{
	return name;
}

void SceneNode::SetName(const std::string& name){
	this->name = name;
}

const SceneNode& SceneNode::GetChildNodeByName(const std::string& name)const{
	SceneNodes::const_iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		if((*itr)->GetName() == name){
			return **itr;
		}
	}
	throw SceneNodeDoesNotExist(name.c_str());
}
SceneNode& SceneNode::GetChildNodeByName(const std::string& name){
	SceneNodes::const_iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		if((*itr)->GetName() == name){
			return **itr;
		}
	}
	throw SceneNodeDoesNotExist(name.c_str());
}
SceneNode::ConstSceneNodePtr SceneNode::GetChildNodePtrByName(const std::string& name)const{
	SceneNodes::const_iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		if((*itr)->GetName() == name){
			return *itr;
		}
	}
	throw SceneNodeDoesNotExist(name.c_str());
}
SceneNode::SceneNodePtr SceneNode::GetChildNodePtrByName(const std::string& name){
	int i = children.size();
	SceneNodes::iterator itr = children.begin();
	for(;itr!=children.end(); itr++){
		if((*itr)->GetName() == name){
			return *itr;
		}
	}
	throw SceneNodeDoesNotExist(name.c_str());
}
const SceneNode& SceneNode::GetDescendantNodeByPath(const std::string& path)const{
	typedef boost::tokenizer<boost::char_separator<char> >
	    tokenizer;
	boost::char_separator<char> sep("", "/");
	tokenizer tokens(path, sep);
	std::vector<std::string> pathelements;
	for(tokenizer::iterator tokenitr=tokens.begin(); tokenitr!=tokens.end();++(++tokenitr)){
		pathelements.push_back(*tokenitr);
	}
	const SceneNode& next = GetChildNodeByName(pathelements[1]);
	if(pathelements.size() == 2){
		return next;
	}else{
		std::stringstream ss;

		std::copy(++(++tokens.begin()), tokens.end(), std::ostream_iterator<std::string>(ss));
		return next.GetDescendantNodeByPath(ss.str());
	}
}
