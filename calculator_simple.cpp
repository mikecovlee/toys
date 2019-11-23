#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stack>
#include <deque>
#include <map>
enum class token_types
{
    null,
    number,
    signal
};
class token_base
{
public:
    token_base() = default;
    token_base(const token_base &) = default;
    virtual ~token_base() = default;
    virtual token_types get_type() const noexcept = 0;
};
// TODO
using number_type = double;
inline bool is_number(char ch)
{
    return std::isdigit(ch) || ch == '.';
}
inline number_type str2num(const std::string &str)
{
    // TODO
    return std::stod(str);
}
class token_number final : public token_base
{
public:
    number_type data;
    token_number(const std::string &str) : data(str2num(str)) {}
    token_types get_type() const noexcept override
    {
        return token_types::number;
    }
};
enum class signal_types
{
    add_,
    sub_,
    mul_,
    div_
};
const std::map<std::string, signal_types> signal_map{
    {"+", signal_types::add_},
    {"-", signal_types::sub_},
    {"*", signal_types::mul_},
    {"/", signal_types::div_}};
const std::vector<char> signals{
    '+', '-', '*', '/'};
inline bool is_signal(char ch)
{
    for (auto &it : signals)
        if (ch == it)
            return true;
    return false;
}
inline signal_types str2sig(const std::string &str)
{
    if (signal_map.count(str) > 0)
        return signal_map.at(str);
    else
    {
        std::cout << str << std::endl;
        throw std::runtime_error("Unrecognized signal!");
    }
}
class token_signal final : public token_base
{
public:
    signal_types data;
    token_signal(const std::string &str) : data(str2sig(str)) {}
    token_types get_type() const noexcept override
    {
        return token_types::signal;
    }
};
class calculator final
{
    std::deque<token_base *> m_tokens;

public:
    calculator() = delete;
    calculator(const std::string &);
    ~calculator();
    number_type operator()();
};
calculator::calculator(const std::string &str)
{
    token_types status = token_types::null;
    std::string buff;
    for (std::size_t i = 0; i < str.size();)
    {
        switch (status)
        {
        case token_types::null:
            if (is_number(str[i]))
            {
                status = token_types::number;
                continue;
            }
            else if (is_signal(str[i]))
            {
                status = token_types::signal;
                continue;
            }
            else if (!std::isspace(str[i]))
                throw std::runtime_error("Unrecognized grammar!");
            ++i;
            break;
        case token_types::number:
            if (!is_number(str[i]))
            {
                m_tokens.push_back(new token_number(buff));
                status = token_types::null;
                buff = "";
            }
            else
                buff += str[i++];
            break;
        case token_types::signal:
            if (!is_signal(str[i]))
            {
                m_tokens.push_back(new token_signal(buff));
                status = token_types::null;
                buff = "";
            }
            else
                buff += str[i++];
            break;
        }
    }
    if (!buff.empty())
    {
        switch (status)
        {
        case token_types::number:
            m_tokens.push_back(new token_number(buff));
            break;
        case token_types::signal:
            m_tokens.push_back(new token_signal(buff));
            break;
        }
    }
    if (!m_tokens.empty())
    {
        if (m_tokens.front()->get_type() == token_types::signal && static_cast<token_signal *>(m_tokens.front())->data == signal_types::sub_)
            m_tokens.push_front(new token_number("0"));
        status = token_types::number;
        for (auto &it : m_tokens)
        {
            if (it->get_type() == status)
            {
                switch (it->get_type())
                {
                case token_types::number:
                    status = token_types::signal;
                    break;
                case token_types::signal:
                    status = token_types::number;
                    break;
                }
            }
            else
                throw std::runtime_error("Wrong grammar(1)!");
        }
        if (status != token_types::signal)
            throw std::runtime_error("Wrong grammar(2)!");
    }
}
calculator::~calculator()
{
    for (auto &it : m_tokens)
        delete it;
}
number_type calculator::operator()()
{
    if (!m_tokens.empty())
    {
        std::stack<number_type> num_stack;
        std::stack<signal_types> sig_stack;
        for (auto it = m_tokens.rbegin(); it != m_tokens.rend(); ++it)
        {
            switch ((*it)->get_type())
            {
            case token_types::number:
                num_stack.push(static_cast<token_number *>(*it)->data);
                break;
            case token_types::signal:
                sig_stack.push(static_cast<token_signal *>(*it)->data);
                break;
            }
        }
        while (!sig_stack.empty())
        {
            number_type left = num_stack.top();
            num_stack.pop();
            number_type right = num_stack.top();
            num_stack.pop();
            switch (sig_stack.top())
            {
            case signal_types::add_:
                num_stack.push(left + right);
                break;
            case signal_types::sub_:
                num_stack.push(left - right);
                break;
            case signal_types::mul_:
                num_stack.push(left * right);
                break;
            case signal_types::div_:
                num_stack.push(left / right);
                break;
            }
            sig_stack.pop();
        }
        return num_stack.top();
    }
    else
        throw std::runtime_error("Empty expression!");
}
int main()
{
    while (true)
    {
        std::string str;
        std::getline(std::cin, str);
        calculator cal(str);
        std::cout << cal() << std::endl;
    }
    return 0;
}