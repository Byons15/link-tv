#pragma once
#include <memory>

namespace LinkmeLib {

	template <typename T>
	public ref class NativePtr
	{
	public:

		NativePtr()
		{
			native = new std::shared_ptr<T>;
		}

		NativePtr(const std::shared_ptr<T>& nativeObj)
		{
			native = new std::shared_ptr<T>(nativeObj);
		}

		NativePtr(T* nativeObj)
		{
			native = new std::shared_ptr<T>(nativeObj);
		}

		T* Get()
		{
			return Ptr;
		}

		property  T* Ptr
		{
			T* get()
			{
				return native->get();
			}

			void set(T* other)
			{
				native->reset(other);
			}
		}

		property std::shared_ptr<T>& SharedPtr
		{
			std::shared_ptr<T>& get()
			{
				return *native;
			}

			void set(std::shared_ptr<T>& other)
			{
				*native = other;
			}
		}

		virtual ~NativePtr()
		{
			delete native;
		}

	private:

		std::shared_ptr<T>* native = nullptr;
	};

}