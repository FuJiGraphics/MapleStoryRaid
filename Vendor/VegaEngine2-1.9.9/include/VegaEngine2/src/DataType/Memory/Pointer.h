#pragma once
#include <memory>

namespace fz {

	template <typename T>
	using Shared = std::shared_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Shared<T> CreateShared(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Unique = std::unique_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	class Weak
	{
	public:
		Weak() = default;
		Weak(const std::nullptr_t);
		Weak(const Weak<T>& _weak);
		Weak(const Shared<T>& _ptr);

		bool IsEmpty() const;

		Weak<T>& operator=(const Weak<T>& _weak);
		Weak<T>& operator=(const Shared<T>& _ptr);
		Weak<T>& operator=(std::nullptr_t null);
		T& operator*();
		Shared<T> operator->();

		bool operator==(const Shared<T>& _ptr) const;
		bool operator==(std::nullptr_t null) const;
		operator bool() const;

	private:
		std::weak_ptr<T> m_Ptr;
	};

	template<typename T>
	inline Weak<T>::Weak(const std::nullptr_t null)
		: m_Ptr()
	{
		// Empty
	}

	template<typename T>
	inline Weak<T>::Weak(const Weak<T>& _weak)
		: m_Ptr(_weak.m_Ptr)
	{
		// Empty
	}

	template<typename T>
	inline Weak<T>::Weak(const Shared<T>& _ptr) 
		: m_Ptr(_ptr)
	{
		// Empty
	}

	template<typename T>
	inline bool Weak<T>::IsEmpty() const
	{
		return m_Ptr.expired();
	}

	template<typename T>
	inline Weak<T>& Weak<T>::operator=(const Weak<T>& _weak) 
	{
		if (this != &_weak)
		{
			m_Ptr = _weak.m_Ptr;
		}
		return *this;
	}

	template<typename T>
	inline Weak<T>& Weak<T>::operator=(const Shared<T>& _ptr)
	{
		m_Ptr = _ptr;
		return *this;
	}

	template<typename T>
	inline Weak<T>& Weak<T>::operator=(std::nullptr_t null)
	{
		m_Ptr.reset();
		return *this;
	}

	template<typename T>
	inline T& Weak<T>::operator*()
	{
		auto ptr = m_Ptr.lock();
		if (!ptr) {
			throw std::runtime_error("Weak 객체가 가리키는 shared_ptr이 만료되어 접근할 수 없습니다");
		}
		return *ptr;
	}

	template<typename T>
	inline Shared<T> Weak<T>::operator->()
	{
		auto ptr = m_Ptr.lock();
		return ptr;
	}

	template<typename T>
	inline bool Weak<T>::operator==(const Shared<T>& _ptr) const
	{
		auto ptr = m_Ptr.lock();
		return ptr == _ptr;
	}

	template<typename T>
	inline bool Weak<T>::operator==(std::nullptr_t null) const
	{
		return this->IsEmpty();
	}

	template<typename T>
	inline Weak<T>::operator bool() const
	{
		return !this->IsEmpty();
	}

} // namesapce fz
