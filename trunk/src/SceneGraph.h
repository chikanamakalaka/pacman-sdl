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
	typedef SceneNode::ConstSceneNodePtr ConstSceneNodePtr;
	typedef SceneNode::SceneNodes SceneNodes;
	typedef std::map<std::string, SceneNodePtr> SceneNodesByName;
	typedef boost::function<void(SceneNode&)> NodeProcessor;
private:
	std::string name;
	SceneNodePtr root;
	bool signalbrokerptr;
	SignalBroker signalbroker;
	SceneNodesByName scenenodesbyname;
	std::map<std::string, std::list<std::string> > nodeprocessordependencies;

	//Don't allow copying or assignment of scenegraphs
	SceneGraph(const SceneGraph& scenegraph);
	SceneGraph& operator=(const SceneGraph& scenegraph);
public:
	SceneGraph(const std::string& name);
	SceneGraph(SignalBroker& signalbroker);

	virtual ~SceneGraph();
	const std::string& GetName()const;
	SceneNode& GetRoot();
	const SceneNode& GetRoot()const;
	SceneNode& GetNodeByPath(const std::string& path);
	const SceneNode& GetNodeByPath(const std::string& path)const;
	SceneNodePtr GetNodePtrByPath(const std::string& path);
	ConstSceneNodePtr GetNodePtrByPath(const std::string& path)const;

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
	template<typename Fn_Tp>
	void VisitNodesWithPushPop(Fn_Tp visitor){
		VisitNodeAndChildrenWithPushPop(visitor, *root);
	}
protected:

	class PrintNodeTree{
		private:
			int depth;
			std::list<std::string> path;
			std::string last;
		public:
			void operator()(SceneGraph::SceneNode& sceneNode){
				last = sceneNode.GetName();
				std::stringstream ss;
				//for(int i = 0; i<depth; i++){
				//	ss<<"  ";
				//}
				std::list<std::string>::const_iterator itr = path.begin();
				for(; itr != path.end(); itr++){
					ss<<"/"<<*itr;
				}
				ss<<"/"<<last;
				std::cout << ss.str()<<std::endl;
			}
			void Push(){
				depth++;
				path.push_back(last);
			}
			void Pop(){
				depth--;
				path.pop_back();
			}
		};

	template<typename Fn_Tp>
	void VisitNodeAndChildren(Fn_Tp visitor, SceneGraph::SceneNode& sceneNode){
		visitor(sceneNode);
		SceneNode::SceneNodes children = sceneNode.GetChildNodes();
		SceneNode::SceneNodes::iterator itr = children.begin();
		for(;itr!=children.end(); itr++){
			VisitNodeAndChildren(visitor, **itr);
		}
	}

	template<typename Fn_Tp>
	void VisitNodeAndChildrenWithPushPop(Fn_Tp visitor, SceneGraph::SceneNode& sceneNode){
		visitor(sceneNode);
		visitor.Push();
		SceneNode::SceneNodes children = sceneNode.GetChildNodes();
		SceneNode::SceneNodes::iterator itr = children.begin();
		for(;itr!=children.end(); itr++){
			VisitNodeAndChildrenWithPushPop(visitor, **itr);
		}
		visitor.Pop();
	}
};


#endif /* SCENEGRAPH_H_ */
