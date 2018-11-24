#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <utility>
namespace cov {
    enum class object_status
    {
        normal, deposit, recycle
    };
    class object
    {
        mutable object_status this_obj_status=object_status::normal;
    public:
        object()=default;
        object(const object&)=default;
        virtual ~object()=default;
        virtual std::type_index get_type() const=0;
        virtual object* clone()=0;
        object_status get_status() const
        {
            return this_obj_status;
        }
        void set_status(object_status s) const
        {
            this_obj_status=s;
        }
    };
    struct visitor final
    {
        object* obj=nullptr;
        ~visitor()
        {
            delete obj;
        }
    };
    struct proxy final
    {
        object* obj=nullptr;
        std::size_t count=0;
        ~proxy()
        {
            switch(obj->get_status())
            {
                case object_status::normal:
                    delete obj;
                    break;
                case object_status::deposit:
                    obj->set_status(object_status::normal);
                    break;
            }
        }
    };
    template<typename T>
    struct obj_instance:public object
    {
        T data;
        template<typename...ArgsT>obj_instance(ArgsT&&...args):data(std::forward<ArgsT>(args)...) {}
        virtual std::type_index get_type() const
        {
            return typeid(T);
        }
        virtual object* clone() override
        {
            return new obj_instance<T>(data);
        }
    };
    class deposit_gc_any final
    {
        friend class standalone_gc_any;
        visitor* ptr=nullptr;
    public:
        constexpr deposit_gc_any()=default;
        deposit_gc_any(const deposit_gc_any& v):ptr(v.ptr) {}
        template<typename T>deposit_gc_any(const T& v):ptr(new visitor{new obj_instance<T>(v)}) {}
        deposit_gc_any& operator=(const deposit_gc_any& v)
        {
            if(&v!=this)
                ptr=v.ptr;
            return *this;
        }
        template<typename T>deposit_gc_any& operator=(const T& v)
        {
            if(ptr!=nullptr)
            {
                if(typeid(ptr->obj->get_type())!=typeid(T))
                {
                    delete ptr->obj;
                    ptr->obj=new obj_instance<T>(v);
                }else
                    static_cast<obj_instance<T>*>(ptr->obj)->data=v;
            }
            return *this;
        }
        template<typename T>
        T& get_value() const
        {
            if(typeid(ptr->obj->get_type())==typeid(T))
                return static_cast<obj_instance<T>*>(ptr->obj)->data;
            else
                throw std::logic_error("Bad cast.");
        }
        template<typename T>
        const T& get_const_value() const
        {
            if(typeid(ptr->obj->get_type())==typeid(T))
                return static_cast<obj_instance<T>*>(ptr->obj)->data;
            else
                throw std::logic_error("Bad cast.");
        }
    };
    class standalone_gc_any final
    {
        proxy* ptr=nullptr;
        void recycle()
        {
            if(ptr!=nullptr&&--ptr->count==0)
                delete ptr;
        }
    public:
        constexpr standalone_gc_any()=default;
        standalone_gc_any(const standalone_gc_any& v):ptr(v.ptr) {
            if(ptr!=nullptr)
                ++ptr->count;
        }
        standalone_gc_any(const deposit_gc_any& v):ptr(v.ptr!=nullptr?new proxy{v.ptr->obj, 1}:nullptr)
        {
            if(ptr!=nullptr)
                ptr->obj->set_status(object_status::deposit);
        }
        template<typename T>standalone_gc_any(const T& v):ptr(new proxy{new obj_instance<T>(v)}) {}
        standalone_gc_any& operator=(const standalone_gc_any& v)
        {
            if(&v!=this)
                ptr=v.ptr;
            return *this;
        }
        standalone_gc_any& operator=(const deposit_gc_any& v)
        {
            recycle();
            if(v.ptr!=nullptr)
            {
                ptr=new proxy{v.ptr->obj, 1};
                ptr->obj->set_status(object_status::deposit);
            }else
                ptr=nullptr;
            return *this;
        }
        template<typename T>standalone_gc_any& operator=(const T& v)
        {
            if(ptr!=nullptr)
            {
                if(typeid(ptr->obj->get_type())!=typeid(T))
                {
                    delete ptr->obj;
                    ptr->obj=new obj_instance<T>(v);
                }else
                    static_cast<obj_instance<T>*>(ptr->obj)->data=v;
            }
            return *this;
        }
        template<typename T>
        T& get_value() const
        {
            if(typeid(ptr->obj->get_type())==typeid(T))
                return static_cast<obj_instance<T>*>(ptr->obj)->data;
            else
                throw std::logic_error("Bad cast.");
        }
        template<typename T>
        const T& get_const_value() const
        {
            if(typeid(ptr->obj->get_type())==typeid(T))
                return static_cast<obj_instance<T>*>(ptr->obj)->data;
            else
                throw std::logic_error("Bad cast.");
        }
    };
}