#pragma once
#include "exprtk.hpp"
#include <vector>
#include <string>


class Evaluator
{
	exprtk::parser<double> m_parser;
	exprtk::expression<double> m_expression;
	exprtk::symbol_table<double> m_symbol_table;

public:
	Evaluator(double& ref, const std::string& evalStr);
	~Evaluator();

	//Recalcuate based on registered expression.
	double Eval();

	//Recalcuate on a new expression.
	static double Eval(const std::string& evalStr, const std::string& variableName, double& var);
};

