#include <cxxabi.h>
#include<atomic>
#include<iostream>
#include<typeinfo>
#include<mutex>
#ifndef SHAREDPTR_HPP
#define SHAREDPTR_HPP
/*
	in namespace cs540
*/
namespace cs540
{
	class Reference
	{
	public:
		std::atomic<int> referenceCounter;
		Reference(){
				this->referenceCounter =0 ;
			}
			virtual ~Reference() {
			}
	};
	
	// hold the actual data of type T.
	template <typename T>
	class ReferenceObject : public Reference{
		public:
			T* data;
			
			ReferenceObject() {
				this->data = NULL;
				this->referenceCounter = 0;
			}
			ReferenceObject(T* temp) {
				this->data = temp;
				this->referenceCounter++;
			}
			~ReferenceObject() {
				delete this->data;
				this->data = NULL;
				this->referenceCounter = 0;
			}
	};

	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: The smart pointer points to an object of type T. T may refer to a const-qualified type.
	*/
	template <typename T>
	class SharedPtr
	{
		public:
		Reference *refObject;
		T* data;
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: Constructs a smart pointer that points to null. 
	*/
	SharedPtr()
	{
		this->data=NULL;
		this->refObject = NULL;
	}
	~SharedPtr()
	{
		if(this->refObject!=NULL)
		{
			if(this->refObject->referenceCounter == 1)
			{
				this->refObject->referenceCounter=0;
				delete this->refObject;
				this->refObject = NULL;
				this->data=NULL;
			}
			else
				this->refObject->referenceCounter--;
		}
	
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: Constructs a smart pointer that points to the given object. The reference count is initialized to one.
	*/
	template <typename U> 
	explicit SharedPtr(U *temp)
	{
		this->refObject = new ReferenceObject<U>(temp);
		this->data = static_cast<T*>(temp);
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: If p is not null, then reference count of the managed object is incremented. 
	*/
	SharedPtr(const SharedPtr &p)
	{
		this->data = p.data;
		this->refObject= p.refObject;
		if(this->refObject!=NULL)
			this->refObject->referenceCounter++;
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: If U * is not implicitly convertible to T *, 
					use of the second constructor will result in a compile-time error 
					when the compiler attempts to instantiate the member template.

	*/
	template <typename U> 
	SharedPtr(const SharedPtr<U> &p)
	{
		this->data = static_cast<T*>(p.data);
		this->refObject= (ReferenceObject<T>*)p.refObject;
		if(this->refObject!=NULL)
			this->refObject->referenceCounter++;
	}
	
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Returns a pointer to the owned object. 
							Note that this will be a pointer-to-const
							 if T is a const-qualified type.  
	*/
	T *get() const
	{
		return (this->data);
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Move the managed object from the given smart pointer.
	*/
	SharedPtr(SharedPtr &&p)
	{
		this->data = p.data;
		this->refObject= p.refObject;
		p.data = NULL;
		p.refObject = NULL;
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Returns a pointer to the owned object. 
							Note that this will be a pointer-to-const
							 if T is a const-qualified type.  
	*/
	template <typename U> SharedPtr(SharedPtr<U> &&p)
	{
		this->data = static_cast<T*>(p.data);
		this->refObject= (ReferenceObject<T>*)p.refObject;
		p.data = NULL;
		p.refObject = NULL;		
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:  Copy assignment. Must handle self assignment. 
					Decrement reference count of current object, 
					if any, and increment reference count of the given object. 
	*/
		
	SharedPtr &operator=(const SharedPtr &p)
	{
		if(this != &p)
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter == 1)
				{
					this->refObject->referenceCounter=0;
					delete this->refObject;
					this->refObject = NULL;
					this->data=NULL;
				}
			}
			if(this->refObject != (ReferenceObject<T>*)p.refObject)
			{
				if(this->refObject!=NULL)
					this->refObject->referenceCounter++;
			}
			this->data = p.data;
			this->refObject = p.refObject;
			//if(this->refObject==NULL)
				//this->refObject->referenceCounter=0;
		}
		return *this;
	}
	template <typename U>
	SharedPtr<T> &operator=(const SharedPtr<U> &p)
	{
		if(this != (SharedPtr<T>*)&p)
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter == 1)
				{
					this->refObject->referenceCounter=0;
					delete this->refObject;
					this->refObject = NULL;
				}
			}
			if(this->refObject != (ReferenceObject<T>*)p.refObject)
			{
				if(this->refObject!=NULL)
					this->refObject->referenceCounter++;
			}
			this->data = static_cast<T*>(p.data);
			this->refObject= (ReferenceObject<T>*)p.refObject;
			//if(this->refObject==NULL)
				//this->refObject->referenceCounter=0;
		}
		return *this;
	}
	/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Move assignment. After this operation, p must be null. 
					The reference count associated with the object 
					(if p is not null before the operation) 
					will remain the same after this operation. 
					This must compile and run correctly 
					if U * is implicitly convertible to T *, 
					otherwise, it must be a syntax error.
	*/
	SharedPtr &operator=(SharedPtr &&p)
	{
		if(p.refObject!=NULL)
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter==1)
				{
					this->refObject->referenceCounter = 0;
					delete this->refObject;
					this->refObject=NULL;
					this->data=NULL;
				}
			}	
			this->data = p.data;
			this->refObject = p.refObject;
			p.data = NULL;
			p.refObject = NULL;	
			
		}
		else
		{
			this->data = NULL;
			this->refObject = NULL;
			this->refObject->referenceCounter = 0;
		}
		return *this;
		
	}
	template <typename U> SharedPtr &operator=(SharedPtr<U> &&p)
	{
		if(p.refObject!=NULL)
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter==1)
				{
					this->refObject->referenceCounter = 0;
					delete this->refObject;
					this->refObject=NULL;
					this->data;
				}
			}	
			this->data = static_cast<T*>(p.data);
			this->refObject = (ReferenceObject<T>*)p.refObject;
			p.data = NULL;
			p.refObject = NULL;	
		}
		else
		{
			this->data = NULL;
			this->refObject = NULL;
			this->refObject->referenceCounter = 0;
		}
		return *this;		
	}
			/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: The smart pointer is set to point to the null pointer. 
					The reference count for the currently pointed to object,
					if any, is decremented. 
	*/
		void reset()
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter == 1)
				{
					this->refObject->referenceCounter=0;
					delete this->refObject;
					this->data = NULL;
					this->refObject= NULL;
				}
				else 
				{
					this->data=NULL;
					this->refObject = NULL;
				}
			}
			
		}
		/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description: Replace owned resource with another pointer. 
					If the owned resource has no other references,
					it is deleted.
					If p has been associated with some other smart pointer,
					the behavior is undefined.
	*/
		template <typename U>
		void reset(U *p)
		{
			if(this->refObject!=NULL)
			{
				if(this->refObject->referenceCounter == 1)
				{
					this->refObject->referenceCounter=0;
					delete this->refObject;
					this->data = NULL;
					this->refObject= NULL;
				}
				else
				{
					this->data=NULL;
					this->refObject = NULL;
				}
			}
			this->refObject = new ReferenceObject<U>(p); 
			this->data = static_cast<T*>(p);
		}
		/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:A reference to the pointed-to object is returned. 
								Note that this will be a const-reference 
								if T is a const-qualified type. 
	*/
		T &operator*() const
		{
			return *(this->data);
		}
		/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:The pointer is returned. 
								Note that this will be a pointer-to-const
								 if T is a const-qualified type. 
	*/
	
		T *operator->() const
		{
			return (this->data);
		}

		/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Returns true if the SharedPtr is not null. 
	*/
		explicit operator bool() const
		{
			if(this->data==NULL)
				return false;
			else
				return true;
		}
	
			};
/*
		Author: Aditya Manjrekar
		Date: 04/21/2015
		Description:Returns true if the two smart pointers point to the same object 
						or if they are both null. 
					Note that implicit conversions may be applied. 
	*/
		template <typename T1, typename T2>
bool operator==(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2)
{
	if((p1.data == p2.data) || (p1.data==NULL && p2.data == NULL))
		return true;
	else
		return false;
}
template <typename T1, typename T2>
bool operator!=(const SharedPtr<T1>&p1, const SharedPtr<T2> &p2)
{
	return !(p1==p2);
}
	template <typename T1>
        bool operator==(const SharedPtr<T1> &p, std::nullptr_t rhs) {
                return !p;
        }

        template <typename T1>
        bool operator==(std::nullptr_t lhs, const SharedPtr<T1> &p) {
                return !p;
        }
		
		
        template <typename T1>
        bool operator!=(const SharedPtr<T1> &p, std::nullptr_t rhs){
                return (bool)p;
        }

        template <typename T1>
        bool operator!=(std::nullptr_t lhs, const SharedPtr<T1> &p) {
		return (bool)p;
        }
template <typename T, typename U>
SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp)
{
	SharedPtr<T> temp(sp);
	temp.data = static_cast<T*>(sp.data);
	return temp;
}
template <typename T, typename U>
SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp)
{
	SharedPtr<T> temp(sp);
	temp.data = dynamic_cast<T*>(sp.data);
	return temp;
}


}
#endif