#include <unordered_map>
#include <type_traits>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <utility>
class people_base {
public:
	people_base()=default;
	people_base(const people_base&)=default;
	virtual ~people_base()=default;
	virtual people_base* copy()=0;
	virtual const std::type_info& get_type() const noexcept final
	{
		return typeid(*this);
	}
};
class database final {
	std::unordered_map<std::size_t,people_base*> mDb;
public:
	database()=default;
	database(const database& db):mDb(db.mDb)
	{
		for(auto& pair:mDb)
			pair.second=pair.second->copy();
	}
	~database()
	{
		for(auto& pair:mDb)
			delete pair.second;
	}
	template<typename T,typename...ArgsT>
	void add_profile(std::size_t id,ArgsT&&...args)
	{
		static_assert(std::is_base_of<people_base,T>::value,"Object type must base of people_base.");
		if(mDb.count(id)>0)
			throw std::logic_error("People id is exist.");
		else
			mDb.emplace(id,new T(std::forward<ArgsT>(args)...));
	}
	template<typename T>
	T& get_profile(std::size_t id)
	{
		if(mDb.count(id)==0)
			throw std::logic_error("Profile is not exist.");
		people_base* ptr=mDb.at(id);
		if(ptr->get_type()!=typeid(T))
			throw std::logic_error("Type is not match.");
		return *dynamic_cast<T*>(ptr);
	}
	const std::type_info& get_type(std::size_t id) const
	{
		if(mDb.count(id)==0)
			throw std::logic_error("Profile is not exist.");
		return mDb.at(id)->get_type();
	}
	bool exist(std::size_t id) const
	{
		return mDb.count(id)>0;
	}
};
enum class gender {
	male,female,other
};
class people:public people_base {
protected:
	gender mGender=other;
	std::string mName;
public:
	people()=delete;
	people(gender g,const std::string& n):mGender(g),mName(n) {}
	people(const people&)=default;
	virtual ~people()=default;
	virtual people_base* copy() override
	{
		return new people(*this);
	}
};
