#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
struct course_info final
{
    std::string number;
    std::string name;
    std::size_t period = 0;
    std::size_t request_semester = 0;
    std::unordered_set<std::string> dependencies;
};
class do_sort final {
    std::list<course_info> graph;
    std::vector<std::string> split(const std::string& str)
    {
        std::vector<std::string> dat;
        bool status = false;
        std::string tmp;
        for(auto& ch:str)
        {
            if (std::isspace(ch))
            {
                if (status)
                {
                    dat.emplace_back(std::move(tmp));
                    tmp.clear();
                    status = false;
                }
            } else
                status = true;
            if (status)
                tmp.push_back(ch);
        }
        if (!tmp.empty())
            dat.emplace_back(std::move(tmp));
        return std::move(dat);
    }
    void remove_depency(const std::string& str)
    {
        for(auto& c:graph)
        {
            if(c.dependencies.count(str) > 0)
                c.dependencies.erase(str);
        }
    }
public:
    do_sort(const std::string& path)
    {
        std::ifstream ifs(path);
        if (!ifs)
            throw std::runtime_error("Bad file path.");
        std::string line;
        std::getline(ifs, line);
        auto count_dat = split(line);
        std::vector<int> semester_dat;
        for(auto& str:count_dat)
            semester_dat.emplace_back(atoi(str.c_str()));
        while (std::getline(ifs, line))
        {
            auto dat = split(line);
            course_info info;
            info.number = dat[0];
            info.name = dat[1];
            info.period = std::atoi(dat[2].c_str());
            info.request_semester = std::atoi(dat[3].c_str());
            for(int i = 4; i < dat.size(); ++i)
                info.dependencies.emplace(dat[i]);
            graph.emplace_back(info);
        }
        for (std::size_t i = 0; i < semester_dat.size(); ++i)
        {
            std::cout<< "第" << i+1 << "学期: ";
            int count = 0;
            for (auto it = graph.begin(); it != graph.end() && count<semester_dat[i];)
            {
                if (it->dependencies.empty() && it->request_semester == i+1)
                {
                    std::cout << it->name << "  ";
                    remove_depency(it->number);
                    it = graph.erase(it);
                    ++count;
                } else
                    ++it;
            }
            for (auto it = graph.begin(); it != graph.end() && count<semester_dat[i];)
            {
                if (it->dependencies.empty())
                {
                    std::cout << it->name << "  ";
                    remove_depency(it->number);
                    it = graph.erase(it);
                    ++count;
                } else
                    ++it;
            }
            std::cout << std::endl;
        }
    }
};
int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;
    do_sort xxx(argv[1]);
    return 0;
}