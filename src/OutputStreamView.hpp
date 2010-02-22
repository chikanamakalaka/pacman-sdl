/*
 * ErrorView.hpp
 *
 *  Created on: May 1, 2009
 *      Author: asantos
 */
class OutputStreamView:public SignalSubscriber{
public:
	typedef void(LogHandler)(const std::string&);
private:
	SignalBroker& signalbroker;
	std::ostream& os;
	bool log;
public:
	OutputStreamView(SignalBroker& signalbroker, std::ostream& os, bool log):
		SignalSubscriber(signalbroker, "", "OutputStreamView"),
		signalbroker(signalbroker),
		os(os),
		log(log){
		SignalSubscriber::ConnectToSignal
		<LogHandler>
		(	"/log/output",
			boost::bind(&OutputStreamView::Log, this, _1));
	}
	void Log(const std::string& output){
		if(log){
			os << output << std::endl;
			os.flush();
		}
	}

};
