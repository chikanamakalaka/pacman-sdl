/*
 * SceneNodeProperty.hpp
 *
 *  Created on: May 4, 2009
 *      Author: asantos
 */

#include "Cloneable.hpp"

class SceneNodeProperty:public Cloneable<SceneNodeProperty>{
public:
	virtual ~SceneNodeProperty(){

	}
	//virtual boost::shared_ptr<SceneNodeProperty> Clone()const = 0;
};
class SceneNodePropertyAlreadyExists : public virtual std::exception{
public:
	SceneNodePropertyAlreadyExists(const char* msg){

	}
	char const* what()const throw(){
		return "SceneNodePropertyAlreadyExists";
	}
};
class SceneNodePropertyDoesNotExist : public virtual std::exception{
private:
		const char* msg;
public:
	SceneNodePropertyDoesNotExist(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};





