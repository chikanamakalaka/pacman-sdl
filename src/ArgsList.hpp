/*
 * ArgsList.hpp
 *
 *  Created on: May 7, 2009
 *      Author: asantos
 */
#ifndef ARGSLIST_HPP_
#define ARGSLIST_HPP_

class ArgsList{
private:
	std::list<boost::any> values;
public:
	ArgsList(){}
	ArgsList(const ArgsList& args){
		values = args.values;
	}
	~ArgsList(){

	}
	ArgsList& operator= (const ArgsList& args){
		values = args.values;
		return *this;
	}
	template<typename T>
	ArgsList& operator,(T* const value){
		values.push_back(boost::any(value));
		return *this;
	}
	/*template<typename T>
	ArgsList& operator,(T& value){
		values.push_back(boost::any(value));
		return *this;
	}*/
	template<typename T>
	ArgsList& operator,(T value){
		values.push_back(boost::any(value));
		return *this;
	}
	ArgsList& operator,(const char* value){
			values.push_back(boost::any(std::string(value)));
			return *this;
		}

	const std::vector<boost::any> GetValues()const{
		std::vector<boost::any> result;
		std::copy(values.begin(), values.end(), std::back_inserter(result));
		return result;
	}
};
#endif /* ARGSLIST_HPP_ */
