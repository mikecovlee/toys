/*
* Covariant Script Early Prototype, firstly finished in 2015
* also include early prototype of Mozart Utilities
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2015 - 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <stdexcept>
#include <typeinfo>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>
#include <list>
#include <map>
#define auto_assert(exp) \
	assert(exp);         \
	if (!(exp))          \
		throw #exp;
namespace cov {
	class baseHolder;
	template <typename T>
	class holder;
	class genericType;
	typedef genericType any;
	template <typename T>
	const std::string &toString(const T &)
	{
		static std::string str(__func__);
		str += " is not defined.";
		return str;
	}
} // namespace cov
// 资源持有者基类
class cov::baseHolder {
public:
	baseHolder() = default;
	virtual ~baseHolder() = default;
	virtual const std::type_info &type() const = 0;
	virtual baseHolder *duplicate() = 0;
	virtual bool compare(const baseHolder *) const = 0;
	virtual const std::string &toString() const = 0;
};
// 资源持有者派生类
template <typename T>
class cov::holder : public baseHolder {
protected:
	T mDat;

public:
	holder() = default;
	holder(const T &dat) : mDat(dat)
	{
	}
	virtual ~holder() = default;
	virtual const std::type_info &type() const override
	{
		return typeid(T);
	}
	virtual baseHolder *duplicate() override
	{
		return new holder(mDat);
	}
	virtual bool compare(const baseHolder *obj) const override
	{
		if (obj->type() == this->type()) {
			const holder<T> *ptr = dynamic_cast<const holder<T> *>(obj);
			if (ptr == nullptr)
				return false;
			return mDat == ptr->data();
		}
		else
			return false;
	}
	virtual const std::string &toString() const override
	{
		return cov::toString(mDat);
	}
	T &data()
	{
		return mDat;
	}
	const T &data() const
	{
		return this->data();
	}
	void data(const T &dat)
	{
		mDat = dat;
	}
};
// 动态类型 Generic Type
class cov::genericType {
protected:
	baseHolder *mDat;

public:
	genericType() : mDat(nullptr)
	{
	}
	template <typename T>
	genericType(const T &dat) : mDat(new holder<T>(dat))
	{
	}
	genericType(const genericType &v) : mDat(v.mDat->duplicate())
	{
	}
	~genericType()
	{
		delete mDat;
	}
	bool empty() const
	{
		return mDat != nullptr;
	}
	const std::type_info &type() const
	{
		if (this->mDat != nullptr)
			return this->mDat->type();
		else
			return typeid(void);
	}
	const std::string &toString() const
	{
		auto_assert(this->mDat != nullptr);
		return this->mDat->toString();
	}
	genericType &operator=(const genericType &var)
	{
		delete mDat;
		mDat = var.mDat->duplicate();
		return *this;
	}
	bool operator==(const genericType &var) const
	{
		return this->mDat->compare(var.mDat);
	}
	bool operator!=(const genericType &var) const
	{
		return !this->mDat->compare(var.mDat);
	}
	template <typename T>
	T &val()
	{
		auto_assert(typeid(T) == this->type() && this->mDat != nullptr);
		return dynamic_cast<holder<T> *>(this->mDat)->data();
	}
	template <typename T>
	const T &val() const
	{
		auto_assert(typeid(T) == this->type() && this->mDat != nullptr);
		return dynamic_cast<holder<T> *>(this->mDat)->data();
	}
	template <typename T>
	void assign(const T &dat)
	{
		if (typeid(T) != this->type() || this->mDat == nullptr) {
			delete mDat;
			mDat = new holder<T>(dat);
		}
		return dynamic_cast<holder<T> *>(mDat)->data(dat);
	}
	template <typename T>
	genericType &operator=(const T &dat)
	{
		assign(dat);
		return *this;
	}
};

namespace Parser {
	class Variable {
	public:
		typedef cov::any Var;
		typedef double Number;
		typedef bool Boolean;
		typedef std::string String;
		typedef cov::any (*Function)(std::vector<cov::any> &);

	private:
		static const String mMainSpace;
		static std::list<String> mSpace;
		static std::map<String, Var> mData;

	public:
		static void addSpace(const String &);
		static const String &nowSpace();
		static void delSpace();
		static String inferName(const String &);
		static bool haveVar(const String &);
		static void inferVar(const String &, const String &);
		static void delVar(const String &);
		static Var callFunc(const String &, std::vector<Var> &);
		template <typename T>
		static bool checkType(const String &);
		template <typename T>
		static void addVar(const String &, T);
		template <typename T>
		static T getVar(const String &);
		static void addVar(const String &, const Var &);
		static Var getVar(const String &);
	};

	class Lambda {
	private:
		std::vector<std::string> mArgs;
		std::string mExpression;

	public:
		Lambda() = default;
		Lambda(std::vector<std::string> &arglist, const std::string &expr) : mArgs(arglist), mExpression(expr) {}
		Lambda(const Lambda &) = default;
		virtual ~Lambda() = default;
		bool operator==(const Lambda &exp) const
		{
			return this->mExpression == exp.mExpression;
		}
		Variable::Number call(std::vector<cov::any> &);
	};

	class Expression {
	private:
		static const long Precision = 100000;
		static void cutPrecision(int, Variable::Number &);

	public:
		static Variable::Number compute(const Variable::String &);
	};

	const Variable::String Variable::mMainSpace("::");

	std::list<Variable::String> Variable::mSpace;

	std::map<Variable::String, Variable::Var> Variable::mData;

	void Variable::addSpace(const String &name)
	{
		mSpace.push_back(name + "::");
	}

	const Variable::String &Variable::nowSpace()
	{
		if (mSpace.empty())
			return mMainSpace;
		else
			return mSpace.back();
	}

	void Variable::delSpace()
	{
		if (!mSpace.empty())
			mSpace.pop_back();
	}

	Variable::String Variable::inferName(const String &name)
	{
		if (haveVar(nowSpace() + name))
			return nowSpace() + name;
		else
			return name;
	}

	bool Variable::haveVar(const String &name)
	{
		return mData.find(name) != mData.end();
	}

	void Variable::delVar(const String &name)
	{
		if (haveVar(nowSpace() + name))
			mData.erase(mData.find(nowSpace() + name));
	}

	template <typename T>
	bool Variable::checkType(const String &name)
	{
		if (!haveVar(inferName(name)))
			return false;
		return mData[inferName(name)].type() == typeid(T);
	}

	template <>
	void Variable::addVar<Variable::Number>(const String &name, Number var)
	{
		mData[nowSpace() + name] = var;
	}

	template <>
	void Variable::addVar<Variable::Boolean>(const String &name, Boolean var)
	{
		mData[nowSpace() + name] = var;
	}

	template <>
	void Variable::addVar<Variable::String>(const String &name, String str)
	{
		mData[nowSpace() + name] = str;
	}

	template <>
	void Variable::addVar<Variable::Function>(const String &name, Function func)
	{
		mData[nowSpace() + name] = func;
	}

	template <>
	void Variable::addVar<Lambda>(const String &name, Lambda func)
	{
		mData[nowSpace() + name] = func;
	}

	template <>
	Variable::Number Variable::getVar<Variable::Number>(const String &name)
	{
		if (!checkType<Number>(name))
			throw std::logic_error("Variable \"" + name + "\" Type is not Number.");
		return mData[inferName(name)].val<Number>();
	}

	template <>
	Variable::Boolean Variable::getVar<Variable::Boolean>(const String &name)
	{
		if (!checkType<Boolean>(name))
			throw std::logic_error("Variable \"" + name + "\" Type is not Boolean.");
		return mData[inferName(name)].val<Boolean>();
	}

	template <>
	Variable::String Variable::getVar<Variable::String>(const String &name)
	{
		if (!checkType<String>(name))
			throw std::logic_error("Variable \"" + name + "\" Type is not String.");
		return mData[inferName(name)].val<String>();
	}

	template <>
	Variable::Function Variable::getVar<Variable::Function>(const String &name)
	{
		if (!checkType<Function>(name))
			throw std::logic_error("Variable \"" + name + "\" Type is not Function.");
		return mData[inferName(name)].val<Function>();
	}

	template <>
	Lambda Variable::getVar<Lambda>(const String &name)
	{
		if (!checkType<Lambda>(name))
			throw std::logic_error("Variable \"" + name + "\" Type is not Lambda.");
		return mData[inferName(name)].val<Lambda>();
	}

	void Variable::addVar(const String &name, const Var &val)
	{
		mData[nowSpace() + name] = val;
	}

	Variable::Var Variable::getVar(const String &name)
	{
		if (!haveVar(inferName(name)))
			throw std::logic_error("Invalid Variable.");
		return mData[inferName(name)];
	}

	Variable::Var Variable::callFunc(const String &name, std::vector<Var> &arglist)
	{
		return getVar<Function>(name)(arglist);
	}

	void Variable::inferVar(const String &name, const String &val)
	{
		if (name.find('(') != std::string::npos && name.find(')') != std::string::npos) {
			std::string::size_type begin(name.find('(')), end(name.find(')'));
			std::string args = name.substr(begin + 1, end - begin - 1);
			std::vector<std::string> arglist;
			std::string tmp;
			for (int i = 0; i < args.size(); ++i) {
				if (args[i] != ',')
					tmp += args[i];
				else {
					arglist.push_back(tmp);
					tmp.clear();
				}
			}
			arglist.push_back(tmp);
			mData[nowSpace() + name.substr(0, begin)] = Lambda(arglist, val);
			return;
		}
		if (val == "true" || val == "True" || val == "TRUE") {
			mData[nowSpace() + name] = true;
			return;
		}
		if (val == "false" || val == "False" || val == "FALSE") {
			mData[nowSpace() + name] = false;
			return;
		}
		try {
			mData[nowSpace() + name] = Expression::compute(val);
		}
		catch (...) {
			mData[nowSpace() + name] = (String)val;
		}
	}

	void Expression::cutPrecision(int precision, Variable::Number &num)
	{
		num *= precision;
		num = (long)num;
		num /= precision;
	}

	Variable::Number Expression::compute(const Variable::String &exp)
	{
		bool reverse = false;
		std::list<Variable::Number> nums;
		std::list<char> operators;
		std::string tmp;
		for (int i = 0; i < exp.size();) {
			if (std::isspace(exp[i])) {
				++i;
				continue;
			}
			if (exp[i] == '(') {
				int level(1), pos(++i);
				for (; pos < exp.size() && level > 0; ++pos) {
					if (exp[pos] == '(')
						++level;
					if (exp[pos] == ')')
						--level;
				}
				if (level > 0)
					throw std::logic_error("The lack of corresponding brackets.");
				nums.push_back(compute(exp.substr(i, pos - i - 1)));
				i = pos;
				continue;
			}
			if (std::ispunct(exp[i]) && exp[i] != ':') {
				if (nums.empty()) {
					switch (exp[i]) {
					case '+':
						reverse = false;
						break;
					case '-':
						reverse = true;
						break;
					default:
						throw std::logic_error("Operator does not recognize.");
					}
					++i;
					continue;
				}
				operators.push_back(exp[i]);
				++i;
				continue;
			}
			if (std::isdigit(exp[i]) || exp[i] == '.') {
				tmp.clear();
				for (; i < exp.size() && (isdigit(exp[i]) || exp[i] == '.'); ++i)
					tmp += exp[i];
				nums.push_back(atof(tmp.c_str()));
				continue;
			}
			if (std::isalpha(exp[i]) || exp[i] == ':') {
				tmp.clear();
				for (; i < exp.size() && (std::isalnum(exp[i]) || exp[i] == '_' || exp[i] == ':'); ++i)
					tmp += exp[i];
				if (Variable::checkType<Variable::Function>(tmp) || Variable::checkType<Lambda>(tmp)) {
					int level(1), pos(++i);
					for (; pos < exp.size() && level > 0; ++pos) {
						if (exp[pos] == '(')
							++level;
						if (exp[pos] == ')')
							--level;
					}
					fflush(stdout);
					if (level > 0)
						throw std::logic_error("The lack of corresponding brackets.");
					std::string arglist = exp.substr(i, pos - i - 1);
					std::string temp;
					std::vector<cov::any> args;
					for (int i = 0; i < arglist.size(); ++i) {
						if (arglist[i] == '(')
							++level;
						if (arglist[i] == ')')
							--level;
						if (level > 0 || arglist[i] != ',') {
							temp += arglist[i];
						}
						else {
							if (Variable::haveVar(Variable::inferName(temp)))
								args.push_back(Variable::getVar(temp));
							else
								args.push_back(compute(temp));
							temp.clear();
						}
					}
					if (Variable::haveVar(Variable::inferName(temp)))
						args.push_back(Variable::getVar(temp));
					else
						args.push_back(compute(temp));
					if (Variable::checkType<Lambda>(tmp)) {
						Lambda &&expr = Variable::getVar<Lambda>(tmp);
						Variable::addSpace(tmp);
						nums.push_back(expr.call(args));
						Variable::delSpace();
					}
					else
						nums.push_back(Variable::callFunc(tmp, args).val<Variable::Number>());
					i = pos;
					continue;
				}
				nums.push_back(Variable::getVar<Variable::Number>(tmp));
				continue;
			}
			throw std::logic_error("Operator does not recognize.");
		}
		if (nums.empty())
			return -1;
		Variable::Number left = nums.front();
		Variable::Number right = 0;
		char signal = 0;
		nums.pop_front();
		for (auto &current : nums) {
			switch (operators.front()) {
			case '+': {
				if (right != 0) {
					switch (signal) {
					case '+':
						left += right;
						break;
					case '-':
						left -= right;
						break;
					}
				}
				right = current;
				signal = '+';
				break;
			}
			case '-': {
				if (right != 0) {
					switch (signal) {
					case '+':
						left += right;
						break;
					case '-':
						left -= right;
						break;
					}
				}
				right = current;
				signal = '-';
				break;
			}
			case '*': {
				if (right != 0)
					right *= current;
				else
					left *= current;
				break;
			}
			case '/': {
				if (right != 0)
					right /= current;
				else
					left /= current;
				break;
			}
			default:
				throw std::logic_error("Operator does not recognize.");
			}
			operators.pop_front();
		}
		Variable::Number result = 0;
		switch (signal) {
		case '+':
			result = left + right;
			break;
		case '-':
			result = left - right;
			break;
		default:
			result = left;
			break;
		}
		cutPrecision(Precision, result);
		if (reverse)
			result = -result;
		return result;
	}
	Variable::Number Lambda::call(std::vector<cov::any> &arglist)
	{
		if (arglist.size() != mArgs.size())
			throw std::logic_error("The number of parameter error.");
		for (int i = 0; i < mArgs.size(); ++i)
			Variable::addVar<Variable::Number>(mArgs[i], arglist[i].val<Variable::Number>());
		Variable::Number result = Expression::compute(mExpression);
		for (auto &it : mArgs)
			Variable::delVar(it);
		return result;
	}
} // namespace Parser

cov::any abs(std::vector<cov::any> &args)
{
	return std::abs(args.front().val<Parser::Variable::Number>());
}

cov::any ln(std::vector<cov::any> &args)
{
	return std::log(args.front().val<Parser::Variable::Number>());
}

cov::any log(std::vector<cov::any> &args)
{
	if (args.size() == 1)
		return std::log10(args.front().val<Parser::Variable::Number>());
	else
		return std::log(args.back().val<Parser::Variable::Number>()) / std::log(args.front().val<Parser::Variable::Number>());
}

cov::any sin(std::vector<cov::any> &args)
{
	return std::sin(args.front().val<Parser::Variable::Number>());
}

cov::any cos(std::vector<cov::any> &args)
{
	return std::cos(args.front().val<Parser::Variable::Number>());
}

cov::any tan(std::vector<cov::any> &args)
{
	return std::tan(args.front().val<Parser::Variable::Number>());
}

cov::any asin(std::vector<cov::any> &args)
{
	return std::asin(args.front().val<Parser::Variable::Number>());
}

cov::any acos(std::vector<cov::any> &args)
{
	return std::acos(args.front().val<Parser::Variable::Number>());
}

cov::any atan(std::vector<cov::any> &args)
{
	return std::atan(args.front().val<Parser::Variable::Number>());
}

cov::any sqrt(std::vector<cov::any> &args)
{
	return std::sqrt(args.front().val<Parser::Variable::Number>());
}

cov::any root(std::vector<cov::any> &args)
{
	return std::pow(args.front().val<Parser::Variable::Number>(), 1.0 / args.back().val<Parser::Variable::Number>());
}

cov::any pow(std::vector<cov::any> &args)
{
	return std::pow(args.front().val<Parser::Variable::Number>(), args.back().val<Parser::Variable::Number>());
}

#define regist_func(func) Parser::Variable::addVar<Parser::Variable::Function>(#func, func);

#include <iostream>

cov::any table(std::vector<cov::any> &args)
{
	if (args.size() < 4)
		throw std::logic_error("The number of parameter error.");
	Parser::Lambda expr = args[0].val<Parser::Lambda>();
	Parser::Variable::Number start = args[1].val<Parser::Variable::Number>();
	Parser::Variable::Number end = args[2].val<Parser::Variable::Number>();
	Parser::Variable::Number step = args[3].val<Parser::Variable::Number>();
	std::vector<cov::any> arglist;
	for (Parser::Variable::Number i = start; i <= end; i += step) {
		arglist.push_back(i);
		std::cout << "x=" << i << " | f(x)=" << expr.call(arglist) << std::endl;
		arglist.pop_back();
	}
	return 0.0;
}

cov::any type(std::vector<cov::any> &args)
{
	for (auto &it : args)
		std::cout << it.type().name() << std::endl;
	return 0.0;
}

cov::any print(std::vector<cov::any> &args)
{
	for (auto &it : args)
		std::cout << it.val<Parser::Variable::String>();
	std::cout << std::endl;
	return 0.0;
}

cov::any tell(std::vector<cov::any> &args)
{
	if (args.size() != 3)
		throw std::logic_error("The number of parameter error.");
	std::vector<cov::any> arglist;
	arglist.push_back(args[2]);
	if (args[0].val<Parser::Variable::Boolean>())
		return args[1].val<Parser::Lambda>().call(arglist);
	return 0.0;
}

int main()
{
	regist_func(abs);
	regist_func(ln);
	regist_func(log);
	regist_func(sin);
	regist_func(cos);
	regist_func(tan);
	regist_func(asin);
	regist_func(acos);
	regist_func(atan);
	regist_func(sqrt);
	regist_func(root);
	regist_func(pow);
	regist_func(table);
	regist_func(type);
	regist_func(print);
	regist_func(tell);
	Parser::Variable::addVar<Parser::Variable::Number>("ans", 0);
	Parser::Variable::addVar<Parser::Variable::Number>("pi", 3.1415926535);
	Parser::Variable::addVar<Parser::Variable::Number>("e", 2.7182818284);
	double answer = 0;
	std::string line;
	while (true) {
		std::getline(std::cin, line);
		if (line.find('=') != std::string::npos)
			Parser::Variable::inferVar(line.substr(0, line.find('=')), line.substr(line.find('=') + 1));
		else {
			answer = Parser::Expression::compute(line);
			Parser::Variable::addVar<Parser::Variable::Number>("ans", answer);
			std::cout << "Answer:" << answer << std::endl;
		}
	}
	return 0;
}