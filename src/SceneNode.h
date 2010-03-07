/*
 * SceneNode.h
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */

#ifndef SCENENODE_H_
#define SCENENODE_H_


#include <list>
#include <string>
#include <map>
#include <boost/function.hpp>
#include <boost/signals.hpp>
#include "Cloneable.hpp"
#include "SignalBroker.hpp"
#include "SceneNodeProperty.hpp"

class SceneNodeDoesNotExist : public virtual std::exception{
private:
		const char* msg;
public:
	SceneNodeDoesNotExist(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};

class SceneGraph;

class SceneNode:public Cloneable<SceneNode>{
public:
	typedef boost::shared_ptr<SceneNode> SceneNodePtr;
	typedef boost::shared_ptr<const SceneNode> ConstSceneNodePtr;
	typedef std::list<SceneNodePtr> SceneNodes;

	typedef boost::shared_ptr<SceneNodeProperty> SceneNodePropertyPtr;
	typedef std::map<std::string, SceneNodePropertyPtr> SceneNodeProperties;
	typedef std::map<std::list<std::string>, boost::function<void(SceneNodePtr)> > SceneNodeProcessorDependencies;
private:
	std::string name;
	SceneGraph& scenegraph;
	SceneNodes children;
	SceneNodeProperties properties;
	SceneNodeProcessorDependencies processordependencies;

public:
	SceneNode(const std::string& name, SceneGraph& scenegraph);
	SceneNode(const SceneNode& sceneNode);
	virtual ~SceneNode();

	SceneNode& operator=(const SceneNode& sceneNode);

	SceneNodePtr Clone()const;

	const std::string& GetName()const;
	void SetName(const std::string& name);

	//Property CRUD
	SceneNodeProperty& AddSceneNodeProperty(const std::string& name, SceneNodePropertyPtr scenenodeproperty);
	template<typename T>
	T& AddSceneNodeProperty(const std::string& name, boost::shared_ptr<T> scenenodeproperty);

	SceneNodeProperty& GetSceneNodeProperty(const std::string& name)const;
	template<typename T>
	T& GetSceneNodeProperty(const std::string& name);
	template<typename T>
	const T& GetSceneNodeProperty(const std::string& name)const;

	void DeleteSceneNodeProperty(const std::string& name);
	bool HasSceneNodeProperty(const std::string& name)const;



	//Child / descendant CRUD
	SceneNode& CreateChildNode(const std::string& name);
	SceneNodePtr CreateChildNodePtr(const std::string& name);
	void AddChildNode(SceneNodePtr scenenode);
	void DeleteChildNode(SceneNodePtr scenenode);

	SceneNodes& GetChildNodes();
	const SceneNodes& GetChildNodes()const;

	void CopyChildNodesToTargetParent(SceneNode& target)const;
	void MoveChildNodeToTargetParent(SceneNodePtr scenenode, SceneNode& target);

	void DeleteChildNodes();
	void DeleteChildNodeByName(const std::string& name);

	const SceneNode& GetChildNodeByName(const std::string& name)const;
	SceneNode& GetChildNodeByName(const std::string& name);
	ConstSceneNodePtr GetChildNodePtrByName(const std::string& name)const;
	SceneNodePtr GetChildNodePtrByName(const std::string& name);
	const SceneNode& GetDescendantNodeByPath(const std::string& path)const;

	//Node processing
	void ProcessNode(const std::string& name);

};



#include "SceneGraph.h"

template<typename T>
T& SceneNode::AddSceneNodeProperty(const std::string& name, boost::shared_ptr<T> scenenodeproperty){
	if(properties.find(name)==properties.end()){
		properties.insert(SceneNodeProperties::value_type(name, scenenodeproperty));
		ProcessNode(name);
		return *scenenodeproperty;
	}else{
		throw SceneNodePropertyAlreadyExists(name.c_str());
	}
}

template<typename T>
T& SceneNode::GetSceneNodeProperty(const std::string& name){
	if(properties.find(name)!=properties.end()){
		boost::shared_ptr<T> property = boost::dynamic_pointer_cast<T>(properties.find(name)->second);
		if(property == 0){
			throw SceneNodePropertyDoesNotExist(name.c_str());
		}else{
			return *property;
		}
	}else{
		throw SceneNodePropertyDoesNotExist(name.c_str());
	}
}
template<typename T>
const T& SceneNode::GetSceneNodeProperty(const std::string& name)const{
	if(properties.find(name)!=properties.end()){
		boost::shared_ptr<T> property = boost::dynamic_pointer_cast<T>(properties.find(name)->second);
		if(property == 0){
			throw SceneNodePropertyDoesNotExist(name.c_str());
		}else{
			return *property;
		}
	}else{
		throw SceneNodePropertyDoesNotExist(name.c_str());
	}
}

#endif /* SCENENODE_H_ */
