/*
 * Cloneable.hpp
 *
 *  Created on: Oct 12, 2009
 *      Author: asantos
 */

#ifndef CLONEABLE_HPP_
#define CLONEABLE_HPP_

#include <boost/shared_ptr.hpp>

template<typename T>
class Cloneable{
public:
	virtual boost::shared_ptr<T> Clone()const = 0;

};


#endif /* CLONEABLE_HPP_ */
