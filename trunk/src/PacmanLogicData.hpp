/*
 * PacmanLogicData.hpp
 *
 *  Created on: Oct 24, 2010
 *      Author: aaronsantos
 */

class PacmanLogicDataProperty: public SceneNodeProperty{
public:
	enum Actor{Pacman, Inky, Binky, Pinky, Clyde};
private:
	boost::numeric::ublas::matrix<bool> collisionmap;
	boost::numeric::ublas::matrix<std::pair<Actor, SceneNode::SceneNodePtr> > actorsmap;
	boost::numeric::ublas::matrix<SceneNode::SceneNodePtr> dotsmap;
	boost::numeric::ublas::matrix<SceneNode::SceneNodePtr> pillsmap;
public:
	PacmanLogicDataProperty(){}

	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<PacmanLogicDataProperty>(new PacmanLogicDataProperty(*this));
	}
	virtual boost::shared_ptr<PacmanLogicDataProperty> ConcreteClone()const{
		return boost::shared_ptr<PacmanLogicDataProperty>(new PacmanLogicDataProperty(*this));
	}

	boost::numeric::ublas::matrix<bool>& GetCollisionMap(){
		return collisionmap;
	}
	const boost::numeric::ublas::matrix<bool>& GetCollisionMap()const{
		return collisionmap;
	}

	boost::numeric::ublas::matrix<std::pair<Actor, SceneNode::SceneNodePtr> >& GetActorsMap(){
		return actorsmap;
	}
	const boost::numeric::ublas::matrix<std::pair<Actor, SceneNode::SceneNodePtr> >& GetActorsMap()const{
		return actorsmap;
	}

	boost::numeric::ublas::matrix<SceneNode::SceneNodePtr>& GetDotsMap(){
		return dotsmap;
	}
	const boost::numeric::ublas::matrix<SceneNode::SceneNodePtr>& GetDotsMap()const{
		return dotsmap;
	}

	boost::numeric::ublas::matrix<SceneNode::SceneNodePtr>& GetPillsMap(){
		return pillsmap;
	}
	const boost::numeric::ublas::matrix<SceneNode::SceneNodePtr>& GetPillsMap()const{
		return pillsmap;
	}
};
