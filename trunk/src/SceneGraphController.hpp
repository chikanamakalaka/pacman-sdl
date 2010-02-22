/*
 * SceneGraphController.hpp
 *
 *  Created on: May 4, 2009
 *      Author: asantos
 */

class SceneGraphAlreadyExists : public virtual std::exception{
private:
	std::string msg;
public:

	SceneGraphAlreadyExists(const std::string& msg):msg(msg){

	}
	~SceneGraphAlreadyExists()throw(){}
	char const* what()const throw(){
		return ("SceneGraphAlreadyExists"+msg).c_str();
	}
};
class SceneGraphDoesNotExist : public virtual std::exception{
private:
	std::string msg;
public:
	SceneGraphDoesNotExist(const std::string& msg):msg(msg){

	}
	~SceneGraphDoesNotExist()throw(){}
	char const* what()const throw(){
		return ("SceneGraphDoesNotExist"+msg).c_str();
	}
};

class SceneGraphController{
public:
	typedef void(GetSceneGraphControllerHandler)(SceneGraphController&);
	typedef void(GetSelectedSceneGraphHandler)(SceneGraph&);
	typedef void(CreatedSceneGraphHandler)(const std::string&, SceneGraph&);
	typedef void(SelectSceneGraphHandler)(const std::string&);
	typedef std::map<std::string, SceneGraph*> SceneGraphs_Tp;
private:

	SignalBroker& signalbroker;
	SceneGraphs_Tp scenegraphs;
	SceneGraph* selectedscenegraph;

public:
	SceneGraphController(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		selectedscenegraph(0){
		//bind GetSelectedSceneGraph to /clock/tick
		//any observer that wants the selected scenegraph can access it through
		// /scenegraphcontroller/getselectedscenegraph
		signalbroker.ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&SceneGraphController::GetSelectedSceneGraph, this, _1, _2));
			
		signalbroker.ConnectToSignal
		<ClockView::TickHandler>
		(	"/clock/tick",
			boost::bind(&SceneGraphController::GetSceneGraphController, this, _1, _2));

		signalbroker.ConnectToSignal
		<SelectSceneGraphHandler>
		(	"/scenegraphcontroller/selectscenegraph",
			boost::bind(&SceneGraphController::SelectSceneGraph, this, _1));

	}
	SceneGraph& CreateSceneGraph(const std::string& name){
		SceneGraph* scenegraph = 0;
		if(scenegraphs.find(name)==scenegraphs.end()){
			scenegraph = new SceneGraph();
			scenegraphs.insert(SceneGraphs_Tp::value_type(name, scenegraph));
			if(selectedscenegraph == 0){
				selectedscenegraph = scenegraph;
			}
			//Let subscribers know that a new scenegraph has been created.
			signalbroker.InvokeSignal
			<CreatedSceneGraphHandler>
			(	"/scenegraphcontroller/createdscenegraph",
				name, *scenegraph);

			return *scenegraph;
		}
		else{
			throw SceneGraphAlreadyExists(name.c_str());
		}
	}
	void DeleteSceneGraph(const std::string& name){
		if(scenegraphs.find(name)!=scenegraphs.end()){
			delete scenegraphs.find(name)->second;
			scenegraphs.erase(name);
		}
		else{
			throw  SceneGraphDoesNotExist(name.c_str());
		}
	}
	void SelectSceneGraph(const std::string& name){
		if(scenegraphs.find(name)!=scenegraphs.end()){
			selectedscenegraph = scenegraphs.find(name)->second;
		}else{
			throw SceneGraphDoesNotExist("Trying to select scenegraph that does not exist");
		}
	}
	void GetSelectedSceneGraph(long t, long dt){
		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Getting selected scenegraph.");
		if(selectedscenegraph){
			signalbroker.InvokeSignal<GetSelectedSceneGraphHandler>("/scenegraphcontroller/getselectedscenegraph", *selectedscenegraph);
		}
	}
	SceneGraph& GetSelectedSceneGraph()const{
		return *selectedscenegraph;
	}
	void GetSceneGraphController(long t, long dt){
		signalbroker.InvokeSignal<GetSceneGraphControllerHandler>("/scenegraphcontroller/get", *this);
	}

};
