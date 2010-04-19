/*
 * SignalSubscriber.hpp
 *
 *  Created on: Aug 21, 2009
 *      Author: asantos
 */

#ifndef SIGNALSUBSCRIBER_HPP_
#define SIGNALSUBSCRIBER_HPP_

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include "SignalBroker.hpp"

class OutputStreamView;

class SignalSubscriber{
private:
	SignalBroker& signalbroker;

	typedef std::multimap<std::string, boost::signals::connection> Connections;
	Connections connections;
protected:
	const std::string signalnamespace;
	const std::string childname;

public:
	SignalSubscriber(SignalBroker& signalbroker, const std::string signalnamespace = std::string(), const std::string childname = std::string()):
		signalbroker(signalbroker),
		signalnamespace(signalnamespace),
		childname(childname){

		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
		//		"Connecting "+childname+"/\""+signalnamespace+"\" to /signalbroker/block");

		//use signal broker to connect to signal to bypass signal subscriber logic.
		//if signal subscriber is used, then unblock will be blocked and subscribers can never be unblocked.
		signalbroker.ConnectToSignal
		<SignalBroker::BlockHandler>
			("/signalbroker/block", boost::bind(&SignalSubscriber::Block, this, _1));

		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
		//		"Connecting "+childname+"/\""+signalnamespace+"\" to /signalbroker/unblock");

		signalbroker.ConnectToSignal
		<SignalBroker::BlockHandler>
			("/signalbroker/unblock", boost::bind(&SignalSubscriber::Unblock, this, _1));

	}
	virtual ~SignalSubscriber(){}

	template<typename Sig_Tp>
	boost::signals::connection ConnectToSignal(const std::string& name, boost::function<Sig_Tp> slot){
		return AddConnection(name, signalbroker.ConnectToSignal(name, slot));
	}
	boost::signals::connection AddConnection(const std::string& name, boost::signals::connection connection){
		connections.insert(Connections::value_type(name, connection));

		//block if signalbroker says so, but do not block global signals
		if(signalbroker.IsBlocked(signalnamespace) && signalnamespace != ""){
			connection.block();
		}
		return connection;
	}
protected:
	 void Block(const std::string& signalnamespace){
		//matching namespace? don't block global (non-namespaced) signals
		if(signalnamespace == this->signalnamespace && this->signalnamespace != ""){
			//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			//	"SignalSubsriber<" + childname + ">::Block("+this->signalnamespace+")");

			Connections::iterator itr = connections.begin();
			for(; itr!= connections.end(); itr++){
				itr->second.block();
			}
		}
	}
	void Unblock(const std::string& signalnamespace){
		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
		//	"SignalSubsriber<" + childname + ">::Unblock(" + signalnamespace + "):this->signalnamespace==\"" + this->signalnamespace + "\"");


		//matching namespace? don't unblock global (non-namespaced) signals
		if(signalnamespace == this->signalnamespace && this->signalnamespace != ""){
			//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output",
			//	"SignalSubsriber<" + childname + ">::Unblock(" + this->signalnamespace + ")");

			Connections::iterator itr = connections.begin();
			for(; itr!= connections.end(); itr++){
				itr->second.unblock();
			}
		}
	}
	SignalBroker& GetSignalBroker(){
		return signalbroker;
	}
};

#endif /* SIGNALSUBSCRIBER_HPP_ */
