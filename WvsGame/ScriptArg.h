#pragma once
#include <string>

struct ScriptArg
{

	static const int MAX_CHAR_BUFFER_SIZE = 32;
	enum
	{
		e_INT,
		e_DOUBLE,
		e_STR,
		e_BOOL
	};

	union Data
	{
		int n;
		double d;
		bool b;
		char c[MAX_CHAR_BUFFER_SIZE];
	};

	int nType;
	Data data;
	template<typename T> ScriptArg(const T& data)
	{
		Init(data);
	}

	template<typename D>
	void Init(const D& d);

	template<>
	void Init<int>(const int& d)
	{
		nType = e_INT;
		data.n = d;
	}

	template<>
	void Init<double>(const double& d)
	{
		nType = e_DOUBLE;
		data.d = d;
	}

	void Init(const char d[])
	{
		Init<std::string>(d);
	}

	template<>
	void Init<std::string>(const std::string& d)
	{
		int nCount = (int)d.size();
		if (nCount >= MAX_CHAR_BUFFER_SIZE)
			nCount = MAX_CHAR_BUFFER_SIZE - 1;
		nType = e_STR;
		memcpy(data.c, d.c_str(), nCount);
		data.c[nCount] = '\0';
	}

	void Init(bool d)
	{
		nType = e_BOOL;
		data.b = d;
	}
};