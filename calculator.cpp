/*
* Simple Calculator translate from Covariant Script Calculator
* F**k double stack, I love parser!!!
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <variant>
#include <string>
#include <deque>
bool is_op(char ch)
{
	return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

class calculator {
	std::deque<char> op_stack;
	std::deque<std::variant<char, double>> num_stack;

	void lex(const std::string &expr)
	{
		int status = 0, last_status = 2, bracket_lev = 0;
		bool expected_neg = true;
		std::string buff = "";
		for (std::size_t i = 0; i < expr.size();) {
			char ch = expr[i];
			switch (status) {
			case 0:
				if (std::isspace(ch))
					continue;
				if (std::isdigit(ch) || ch == '.') {
					status = 1;
					continue;
				}
				if (is_op(ch)) {
					status = 2;
					continue;
				}
				if (ch == '(') {
					status = 3;
					expected_neg = true;
					continue;
				}
				if (ch == ')') {
					status = 4;
					continue;
				}
				throw std::runtime_error("Wrong Format Expression");
				break;
			case 1:
				if (last_status == 1)
					throw std::runtime_error("Wrong Format Expression");
				if (!std::isdigit(ch) && ch != '.') {
					num_stack.emplace_back(last_status < 0 ? -std::stod(buff) : std::stod(buff));
					last_status = status;
					status = 0;
					buff.clear();
				}
				else {
					buff += ch;
					++i;
				}
				break;
			case 2:
				if (expected_neg && ch == '-') {
					if (last_status < 0)
						throw std::runtime_error("Hanging operator.");
					last_status = -1;
					status = 0;
					++i;
					continue;
				}
				if (ch == '-' && last_status <= -1)
					throw std::runtime_error("Hanging operator.");
				if (last_status == 2 || last_status < 0) {
					if (ch == '-') {
						last_status = -1;
						status = 0;
						++i;
						continue;
					}
					else
						throw std::runtime_error("Wrong Format Expression");
				}
				op_stack.emplace_back(ch);
				last_status = status;
				status = 0;
				++i;
				break;
			case 3:
				++bracket_lev;
				num_stack.emplace_back(ch);
				last_status = 2;
				status = 0;
				++i;
				break;
			case 4:
				--bracket_lev;
				num_stack.emplace_back(ch);
				last_status = 1;
				status = 0;
				++i;
				break;
			}
			expected_neg = false;
		}
		if (status == 1) {
			num_stack.emplace_back(last_status < 0 ? -std::stod(buff) : std::stod(buff));
			last_status = status;
		}
		if (last_status == 2 || last_status < 0)
			throw std::runtime_error("Hanging operator.");
		if (bracket_lev != 0)
			throw std::runtime_error("Bracket does not closed.");
	}

	std::variant<char, double> parse(int rec)
	{
		while (!num_stack.empty()) {
			auto lhs = std::move(num_stack.front());
			num_stack.pop_front();
			if (std::holds_alternative<char>(lhs) && std::get<char>(lhs) == '(')
				lhs = parse(0);
			if (!num_stack.empty() && std::holds_alternative<char>(num_stack.front()) && std::get<char>(num_stack.front()) == ')') {
				num_stack.pop_front();
				return lhs;
			}
			if (op_stack.empty())
				return lhs;
			char op = op_stack.front();
			if (op == '+' || op == '-') {
				if (rec == 1)
					return lhs;
				op_stack.pop_front();
				auto rhs = parse(1);
				if (op == '+')
					num_stack.emplace_front(std::get<double>(lhs) + std::get<double>(rhs));
				else
					num_stack.emplace_front(std::get<double>(lhs) - std::get<double>(rhs));
			}
			else {
				op_stack.pop_front();
				auto rhs = std::move(num_stack.front());
				num_stack.pop_front();
				if (std::holds_alternative<char>(rhs) && std::get<char>(rhs) == '(')
					rhs = parse(0);
				if (op == '*')
					num_stack.emplace_front(std::get<double>(lhs) * std::get<double>(rhs));
				else
					num_stack.emplace_front(std::get<double>(lhs) / std::get<double>(rhs));
			}
		}
		return static_cast<double>(0);
	}

public:
	double eval(const std::string &expr)
	{
		lex(expr);
		return std::get<double>(parse(0));
	}
};

#include <iostream>

int main()
{
	for (;;) {
		try {
			std::string line;
			std::getline(std::cin, line);
			std::cout << calculator().eval(line) << std::endl;
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	return 0;
}