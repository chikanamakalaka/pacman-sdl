/*
 * ArgsListObserver.hpp
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */

#ifndef ARGSLISTOBSERVER_HPP_
#define ARGSLISTOBSERVER_HPP_

#include <boost/function.hpp>
#include <boost/signals.hpp>

class ArgsListObserver{
public:
	//typedef ArgsList_Tp ArgsList;
	//typedef boost::function<void(const ArgsList_Tp&)> Handler;
	//typedef boost::signal<void(const ArgsList_Tp&)> Signal;
	typedef SignalBroker<void(const ArgsList&)> SignalBroker;
	typedef SignalBroker::Handler Handler;
	typedef SignalBroker::Signal Signal;
};

#endif /* ARGSLISTOBSERVER_HPP_ */
