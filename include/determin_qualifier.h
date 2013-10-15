/*
The MIT License

Copyright (c) 2009 - 2013 Liam Devine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef DETERMIN_QUALIFIER_H_
#	define DETERMIN_QUALIFIER_H_


//TODO: This file has been used in the library for quite some time now and
//it should be considered part of it. Change the namespace to OOLUA::INTERNAL

/**
	\file determin_qualifier.h
	\brief Template struct which report if the type has qualifiers and also
	removes some of the possible qualifiers
*/
namespace LVD
{
	struct is_true{};
	struct is_false{};

	template<typename T, typename T1>
	struct is_same
	{
		enum {value = 0};
	};

	template<typename T>
	struct is_same<T, T>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const
	{
		enum {value = 0};
	};

	template<typename T>struct is_const<T const>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T const&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T const*>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T * const>
	{
		enum {value = 0};
	};

	template<typename T>struct is_const<T const * const>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T const * const&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T * const&>
	{
		enum {value = 0};
	};

	template<typename T>struct is_const<T const *&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const<T * const *&>
	{
		enum {value = 0};
	};

	template<typename T>struct is_const<T const * const *&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_void
	{
  		enum {value = 0};
	};

	template<>struct is_void<void>
	{
		enum {value = 1};
	};

	template<typename T>struct is_ref
	{
		enum {value = 0};
	};

	template<typename T>struct is_ref<T&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_ref_to_const
	{
		enum {value = 0};
	};

	template<typename T>struct is_ref_to_const<T const&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_ref_to_pointer
	{
		enum {value = 0};
	};

	template<typename T>struct is_ref_to_pointer<T*&>
	{
		enum {value = 1};
	};

	template<typename T>struct is_ptr
	{
		enum {value = 0};
	};

	template<typename T>struct is_ptr<T*>
	{
		enum {value = 1};
	};

	template<typename T>struct is_ptr_to_const
	{
		enum {value = 0};
	};

	template<typename T>struct is_ptr_to_const<T const*>
	{
		enum {value = 1};
	};
	template<typename T>struct is_const_ptr_to
	{
		enum {value = 0};
	};

	template<typename T>struct is_const_ptr_to<T *const>
	{
		enum {value = 1};
	};

	template<typename T>struct is_const_ptr_to_const
	{
		enum {value = 0};
	};

	template<typename T>struct is_const_ptr_to_const<T const * const>
	{
		enum {value = 1};
	};

	template<typename T>struct has_qualifiers
	{
		enum { value = (
			is_ref<T>::value
			+ is_ref_to_const<T>::value
			+ is_ref_to_pointer<T>::value
			+ is_ptr<T>::value
			+ is_ptr_to_const<T>::value
			+ is_const_ptr_to<T>::value
			+ is_const_ptr_to_const<T>::value
			+ is_const<T>::value )
		? 1 : 0 };
	};

	template<typename T>struct by_reference
	{
		enum { value = (
			is_ref<T>::value
			+ is_ref_to_const<T>::value
			+ is_ref_to_pointer<T>::value
			+ is_ptr<T>::value
			+ is_ptr_to_const<T>::value
			+ is_const_ptr_to<T>::value
			+ is_const_ptr_to_const<T>::value )
		? 1 : 0 };
	};

	template<typename T>struct has_qualifiers1
	{
		enum { value = (
			if_or<is_ref<T>::value
			, if_or<is_ref_to_const<T>::value
			, if_or<is_ref_to_pointer<T>::value
			, if_or<is_ptr<T>::value
			, if_or<is_ptr_to_const<T>::value
			, if_or<is_const_ptr_to<T>::value
			, if_or<is_const_ptr_to_const<T>::value
			, is_const<T>::value
			>::value >::value >::value >::value>::value >::value >::value )
		? 1 : 0 };
	};

	template<typename T>struct constant_remover
	{
		typedef T  type;
	};

	template<typename T>struct constant_remover<T const>
	{
		typedef T  type;
	};

	template<typename T>struct constant_remover<T const&>
	{
		typedef T&  type;
	};

	template<typename T>struct constant_remover<T const*>
	{
		typedef T*  type;
	};

	template<typename T>struct constant_remover<T const * const>
	{
		typedef T* const  type;
	};

	template<typename T>struct constant_remover<T* const>
	{
		typedef T* const  type;
	};

	template<typename T>struct remove_const
	{
		typedef typename constant_remover<T>::type type;
	};

/////////////////////////////////////////////////////////////////////////////////////


	template<typename T>struct all_constant_remover
	{
		typedef T  type;
	};

	template<typename T>struct all_constant_remover<T const>
	{
		typedef T  type;
	};

	template<typename T>struct all_constant_remover<T const&>
	{
		typedef T&  type;
	};

	template<typename T>struct all_constant_remover<T const*>
	{
		typedef T*  type;
	};

	template<typename T>struct all_constant_remover<T const * const>
	{
		typedef T*  type;
	};

	template<typename T>struct all_constant_remover<T* const>
	{
		typedef T*  type;
	};

	template<typename T>struct remove_all_const
	{
		typedef typename all_constant_remover<T>::type type;
	};

	template<typename T>struct remove_ref
	{
		typedef T type;
	};

	template<typename T>struct remove_ref<T&>
	{
		typedef T type;
	};

	template<typename T>struct ptr_remover
	{
		typedef T type;
	};

	template<typename T>struct ptr_remover<T*>
	{
		typedef T type;
	};

	template<typename T>struct remove_ptr
	{
		typedef typename ptr_remover<T>::type type;
	};

	namespace
	{
		template<typename Input, template <typename> class Do,
					template <typename> class Test, int result>
		struct do_while;


		template<typename Input, template <typename> class Do,
					template <typename> class Test, int result>
		struct do_while
		{
			typedef typename do_while<typename Do<Input>::type
									, Do
									, Test
									, Test<typename Do<Input>::type>::value >::type type;
		};

		template<typename Input, template <typename>class Do,
					template <typename> class Test >
		struct do_while<Input, Do, Test, 0>
		{
			typedef Input type;
		};
	} // namespace

	template<typename T>struct remove_all_ptrs
	{
		typedef typename do_while<T //input
									, remove_ptr //do
									, is_ptr //test
									, is_ptr<T>::value //result
								>::type type;
	};

} // namespace LVD // NOLINT

#endif //DETERMIN_QUALIFIER_H_
