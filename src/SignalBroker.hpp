/*
 * SignalBroker.hpp
 *
 *  Created on: Apr 29, 2009
 *      Author: asantos
 */

#ifndef SIGNALBROKER_HPP_
#define SIGNALBROKER_HPP_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <boost/signals.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include "ArgsList.hpp"
class SignalSubsrciber;

template<typename Sig_Tp>
class Signal;

class UnableToCastSignal : public virtual std::exception{
private:
		const char* msg;
public:
		UnableToCastSignal(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};

class SignalBase{
public:
	virtual ~SignalBase(){}
	template<typename Sig_Tp>
	const Signal<Sig_Tp>& GetSignal()const{
		Signal<Sig_Tp>* signal = dynamic_cast<Signal<Sig_Tp>*>(this);
		if(signal){
			return *signal;
		}else{
			throw UnableToCastSignal("");
		}
	}
	template<typename Sig_Tp>
	Signal<Sig_Tp>& GetSignal(){
		Signal<Sig_Tp>* signal = dynamic_cast<Signal<Sig_Tp>*>(this);
		if(signal){
			return *signal;
		}else{
			throw UnableToCastSignal("");
		}
	}
};

template<typename Sig_Tp>
class Signal: public SignalBase, public boost::signal<Sig_Tp>{
public:
	virtual ~Signal(){}
};

class SignalAlreadyExists : public virtual std::exception{
public:
	SignalAlreadyExists(const char* msg){

	}
	char const* what()const throw(){
		return "SignalAlreadyExists";
	}
};
class SignalDoesNotExist : public virtual std::exception{
private:
		const char* msg;
public:
	SignalDoesNotExist(const char* msg):msg(msg){

	}
	char const* what()const throw(){
		return msg;
	}
};

class SignalBroker{
	friend class SignalSubscriber;
public:
	typedef void(BlockHandler)(const std::string&);
	typedef void(UnblockHandler)(const std::string&);
	typedef void(GenericHandler)(const ArgsList& args);
private:
	typedef std::map<const std::string, SignalBase*> Signals;
	typedef std::set<std::string> BlockedNamespaces;

	Signals signals;
	BlockedNamespaces blockednamespaces;
	bool verbose;

public:
	const std::string blockstr;
	const std::string unblockstr;

public:
	SignalBroker(bool verbose = false):verbose(verbose), blockstr("/signalbroker/block"), unblockstr("/signalbroker/unblock"){
		CreateSignal<BlockHandler>(blockstr);
		CreateSignal<UnblockHandler>(unblockstr);
	}
	virtual ~SignalBroker(){
		if(verbose){
			std::cout << "Deconstructing signalbroker." << std::endl;
			std::cout << "Here's a list of all the signals:" << std::endl;
		}
		for(Signals::iterator itr = signals.begin(); itr!=signals.end(); itr++){
			if(verbose){
				std::cout << itr->first << std::endl;
			}
			delete itr->second;
		}
	}
	template<typename Sig_Tp>
	Signal<Sig_Tp>& EnsureSignal(const std::string& name){
		if(HasSignal(name)){
			return signals.find(name)->second->GetSignal<Sig_Tp>();
		}
		else{
			Signal<Sig_Tp>* signal = new Signal<Sig_Tp>();
			signals.insert(Signals::value_type(name, static_cast<SignalBase*>(signal)));

			Signal<GenericHandler>* genericsignal = new Signal<GenericHandler>();
			SignalBase* genericsignalbase = static_cast<SignalBase*>(genericsignal);
			signals.insert(Signals::value_type(name+"-generic", genericsignalbase));
			return *signal;
		}
	}
	template<typename Sig_Tp>
	boost::signals::connection ConnectToSignal(const std::string& name, boost::function<Sig_Tp> slot){
		Signal<Sig_Tp>& sig = EnsureSignal<Sig_Tp>(name);
		return sig.connect(slot);
	}

	void Block(const std::string& signalnamespace){
		blockednamespaces.insert(signalnamespace);
		InvokeSignal<void(const std::string&)>(blockstr, signalnamespace);
	}
	void Unblock(const std::string& signalnamespace){
		blockednamespaces.erase(signalnamespace);
		InvokeSignal<void(const std::string&)>(unblockstr, signalnamespace);
	}
	bool IsBlocked(const std::string& signalnamespace){
		return blockednamespaces.find(signalnamespace)!=blockednamespaces.end();
	}

private:
	bool HasSignal(const std::string& name)const{
		return signals.find(name)!=signals.end();
	}
	template<typename Sig_Tp>
	Signal<Sig_Tp>& CreateSignal(const std::string& name){
		Signal<Sig_Tp>* signal = 0;
		if(signals.find(name)==signals.end()){
			signal = new Signal<Sig_Tp>();
			SignalBase* signalbase = static_cast<SignalBase*>(signal);
			signals.insert(Signals::value_type(name, signalbase));

			Signal<GenericHandler>* genericsignal = new Signal<GenericHandler>();
			SignalBase* genericsignalbase = static_cast<SignalBase*>(genericsignal);
			signals.insert(Signals::value_type(name+"-generic", genericsignalbase));
			return *signal;
		}
		else{
			throw SignalAlreadyExists(name.c_str());
		}
	}
	void DeleteSignal(const std::string& name){
		if(signals.find(name)==signals.end()){
			delete signals.find(name)->second;
			signals.erase(name);
		}
		else{
			throw SignalDoesNotExist(name.c_str());
		}
	}
	template<typename Sig_Tp>
	Signal<Sig_Tp>& GetSignal(const std::string& name){
		if(signals.find(name)!=signals.end()){
			return signals.find(name)->second->GetSignal<Sig_Tp>();
		}
		else{
			throw SignalDoesNotExist(name.c_str());
		}
	}
	template<typename Sig_Tp>
	const Signal<Sig_Tp>& GetSignal(const std::string& name)const{
		if(signals.find(name)!=signals.end()){
			return signals.find(name)->second->GetSignal<Sig_Tp>();
		}
		else{
			throw SignalDoesNotExist(name.c_str());
		}
	}

public:

	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name){
		if(HasSignal(name)){
			//std::cout<<"Signalbroker invokoing signal: "<<name<<std::endl;
			//std::cout.flush();
			GetSignal<Fn_Tp>(name)();
			GetSignal<GenericHandler>(name+"-generic")(ArgsList());
		}
	}
	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name, typename boost::function<Fn_Tp>::arg1_type arg1){
		if(HasSignal(name)){
			//std::cout<<"Signalbroker invokoing signal: "<<name<<std::endl;
			//std::cout.flush();
			GetSignal<Fn_Tp>(name)(arg1);
			GetSignal<GenericHandler>(name+"-generic")((ArgsList().operator,<typename boost::function<Fn_Tp>::arg1_type>(arg1)));

		}
	}
	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name, typename boost::function<Fn_Tp>::arg1_type arg1, typename boost::function<Fn_Tp>::arg2_type arg2){
		if(HasSignal(name)){
			//std::cout<<"Signalbroker invokoing signal: "<<name<<std::endl;
			//std::cout.flush();
			GetSignal<Fn_Tp>(name)(arg1, arg2);
			GetSignal<GenericHandler>(name+"-generic")((ArgsList(), arg1, arg2));
		}
	}
	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name, typename boost::function<Fn_Tp>::arg1_type arg1, typename boost::function<Fn_Tp>::arg2_type arg2, typename boost::function<Fn_Tp>::arg3_type arg3){
		if(HasSignal(name)){
			//std::cout<<"Signalbroker invokoing signal: "<<name<<std::endl;
			//std::cout.flush();
			GetSignal<Fn_Tp>(name)(arg1, arg2, arg3);
			GetSignal<GenericHandler>(name+"-generic")((ArgsList(), arg1, arg2, arg3));
		}
	}
	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name, typename boost::function<Fn_Tp>::arg1_type arg1, typename boost::function<Fn_Tp>::arg2_type arg2, typename boost::function<Fn_Tp>::arg3_type arg3, typename boost::function<Fn_Tp>::arg4_type arg4){
		if(HasSignal(name)){
			GetSignal<Fn_Tp>(name)(arg1, arg2, arg3, arg4);
			GetSignal<GenericHandler>(name+"-generic")((ArgsList(), arg1, arg2, arg3, arg4));
		}
	}
	template<typename Fn_Tp>
	void InvokeSignal(const std::string& name, typename boost::function<Fn_Tp>::arg1_type arg1, typename boost::function<Fn_Tp>::arg2_type arg2, typename boost::function<Fn_Tp>::arg3_type arg3, typename boost::function<Fn_Tp>::arg4_type arg4, typename boost::function<Fn_Tp>::arg5_type arg5){
		if(HasSignal(name)){
			GetSignal<Fn_Tp>(name)(arg1, arg2, arg3, arg4, arg5);
			GetSignal<GenericHandler>(name+"-generic")((ArgsList(), arg1, arg2, arg3, arg4, arg5));
		}
	}
};

#endif /* SIGNALBROKER_HPP_ */

