#pragma once
#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP



#include "general/configuration.hpp"





class TestConfig : public Configuration
{
public:
	typedef std::shared_ptr<TestConfig> Ptr;
public:
	TestConfig();

	template<typename T>
	Ptr add(const std::string& key, const T& value)
	{
		testOptions[key] = value;

		return boost::static_pointer_cast<TestConfig>(this->shared_from_this());
	}

	static Ptr Create();

	std::shared_ptr<TestConfig> clone() const;

protected:
	const boost::any& getValueByKey(const std::string& key) const;

private:
	std::map<string, boost::any> testOptions;

};




#endif