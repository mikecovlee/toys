#pragma once
// std::any by Michael Lee
// Support C++11 Standard
#include <typeinfo>
#include <utility>
#include <memory>
#include <array>
namespace cov {
	template<typename T,long blck_size>
	class allocator final {
		std::allocator<T> mAlloc;
		std::array<T*,blck_size> mPool;
		long mOffset=-1;
		bool mActived=true;
	public:
		void blance()
		{
			if(mOffset!=0.5*blck_size) {
				if(mOffset<0.5*blck_size) {
					for(; mOffset<0.5*blck_size; ++mOffset)
						mPool.at(mOffset+1)=mAlloc.allocate(1);
				}
				else {
					for(; mOffset>0.5*blck_size; --mOffset)
						mAlloc.deallocate(mPool.at(mOffset),1);
				}
			}
		}
		void clean()
		{
			for(; mOffset>=0; --mOffset)
				mAlloc.deallocate(mPool.at(mOffset),1);
		}
		void enable_buffer()
		{
			mActived=true;
			blance();
		}
		void disable_buffer()
		{
			mActived=false;
			clean();
		}
		allocator()
		{
			blance();
		}
		allocator(const allocator&)=delete;
		~allocator()
		{
			clean();
		}
		template<typename...ArgsT>
		T* alloc(ArgsT&&...args)
		{
			T* ptr=nullptr;
			if(mActived&&mOffset>=0) {
				ptr=mPool.at(mOffset);
				--mOffset;
			}
			else
				ptr=mAlloc.allocate(1);
			mAlloc.construct(ptr,std::forward<ArgsT>(args)...);
			return ptr;
		}
		void free(T* ptr)
		{
			mAlloc.destroy(ptr);
			if(mActived&&mOffset<blck_size-1) {
				++mOffset;
				mPool.at(mOffset)=ptr;
			}
			else
				mAlloc.deallocate(ptr,1);
		}
	};
}
namespace std {
	class bad_any_cast final:public std::bad_cast {
	public:
		using bad_cast::bad_cast;
	};
	constexpr std::size_t any_allocator_blck_size=96;
	class any final {
	private:
		class value_base {
		public:
			value_base()=default;
			value_base(const value_base&)=delete;
			value_base(value_base&&) noexcept=delete;
			virtual ~value_base()=default;
			virtual const std::type_info& type() const noexcept=0;
			virtual value_base* copy()=0;
			virtual void kill()=0;
		};
		template<typename T>
		class value:public value_base {
			static cov::allocator<value,any_allocator_blck_size> allocator;
			T data;
		public:
			value()=delete;
			template<typename...ArgsT>
			value(ArgsT&&...args):data(std::forward<ArgsT>(args)...) {}
			virtual ~value()=default;
			virtual const std::type_info& type() const noexcept override
			{
				return typeid(T);
			}
			virtual value_base* copy() override
			{
				return allocator.alloc(data);
			}
			virtual void kill() override
			{
				allocator.free(this);
			}
		};
		value_base* data=nullptr;
		any(value_base* ptr):data(ptr) {}
	public:
		void swap(any& v)
		{
			std::swap(data,v.data);
		}
		void swap(any&& v)
		{
			std::swap(data,v.data);
		}
		any()=default;
		any(const any& v):data(v.data==nullptr?nullptr:v.data->copy()) {}
		any(any&& v) noexcept
		{
			std::swap(data,v.data);
		}
		template<typename T>explicit any(const T& val):data(value<T>::allocator.alloc(val)) {}
		~any()
		{
			if(data!=nullptr)
				data->kill();
		}
		any& operator=(const any& v)
		{
			if(&v!=this) {
				if(data!=nullptr)
					data->kill();
				data=v.data==nullptr?nullptr:v.data->copy();
			}
			return *this;
		}
		template<typename T>any& operator=(const T& val)
		{
			if(data!=nullptr)
				data->kill();
			data=value<T>::allocator.alloc(val);
			return *this;
		}
		template<typename T,typename...ArgsT>
		void emplace(ArgsT&&...args)
		{
			if(data!=nullptr)
				data->kill();
			data=value<T>::allocator.alloc(std::forward<ArgsT>(args)...);
		}
		void reset()
		{
			if(data!=nullptr)
				data->kill();
		}
		bool has_value() const
		{
			return data!=nullptr;
		}
		const std::type_info& type() const noexcept
		{
			return data==nullptr?typeid(void):data->type();
		}
		template<typename T,typename...ArgsT>
		static any make(ArgsT&&...args)
		{
			return value<T>::allocator.alloc(std::forward<ArgsT>(args)...);
		}
		template<typename T>T& cast() const
		{
			if(typeid(T)!=this->type())
				throw bad_any_cast();
			return dynamic_cast<value<T>*>(data)->data;
		}
	};
	void swap(any& a,any& b)
	{
		a.swap(b);
	}
}
