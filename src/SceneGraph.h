/*
 * SceneGraph.h
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */


#ifndef SCENEGRAPH_H_
#define SCENEGRAPH_H_

#include <boost/any.hpp>
#include <boost/signals.hpp>
#include "SceneNode.h"
#include "SignalSubscriber.hpp"
#include "SignalBroker.hpp"

class SceneGraph: public SignalSubscriber{
public:
	typedef ::SceneNode SceneNode;
	typedef SceneNode::SceneNodePtr SceneNodePtr;
	typedef SceneNode::SceneNodes SceneNodes;
	typedef std::map<std::string, SceneNodePtr> SceneNodesByName;
	typedef boost::function<void(SceneNode&)> NodeProcessor;
private:
	SceneNodePtr root;
	SignalBroker signalbroker;
	SceneNodesByName scenenodesbyname;
	std::map<std::string, std::list<std::string> > nodeprocessordependencies;

	//Don't allow copying or assignment of scenegraphs
	SceneGraph(const SceneGraph& scenegraph);
	SceneGraph& operator=(const SceneGraph& scenegraph);
public:
	SceneGraph();
	SceneGraph(SignalBroker& signalbroker);

	virtual ~SceneGraph();
	SceneNode& GetRoot();
	const SceneNode& GetRoot()const;
	const SceneNode& GetNodeByPath(const std::string& path);
	SceneNodePtr GetNodePtrByPath(const std::string& path);

	bool HasNodeProcessor(const std::string& propertykey);
	void RegisterNodeProcessor(const std::string& propertykey, const std::list<std::string>& dependencies, NodeProcessor processor);
	const std::list<std::string> GetNodePropertyDependencies(const std::string& propertykey)const{
		if(nodeprocessordependencies.find(propertykey)==nodeprocessordependencies.end()){
			return std::list<std::string>();
		}else{
			return nodeprocessordependencies.find(propertykey)->second;
		}
	}
	void ProcessNode(SceneNode& scenenode, const std::string& propertykey);

	template<typename Fn_Tp>
	void VisitNodes(Fn_Tp visitor){
		VisitNodeAndChildren(visitor, *root);
	}
protected:

	template<typename Fn_Tp>
	void VisitNodeAndChildren(Fn_Tp visitor, SceneGraph::SceneNode& sceneNode){
		visitor(sceneNode);
		SceneNode::SceneNodes children = sceneNode.GetChildNodes();
		SceneNode::SceneNodes::iterator itr = children.begin();
		for(;itr!=children.end(); itr++){
			VisitNodeAndChildren(visitor, **itr);
		}
	}
};


#endif /* SCENEGRAPH_H_ */
