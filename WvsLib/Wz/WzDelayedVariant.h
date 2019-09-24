#pragma once
#include <string>

struct WzDelayedVariant
{
	enum VariantType
	{
		vt_Filtered_Integer,
		vt_Filtered_Long,
		vt_Int16,
		vt_Float32,
		vt_Double64,
		vt_String,
	};

	union NumericalVariant
	{
		long long int liData;
		double dData;
		float fData;
	};

	NumericalVariant uData;
	std::string sData;

	//Value Type ==> For interpretation
	VariantType vType;

	//(Raw)Type in the file ==> For encoding
	int fType;
};

