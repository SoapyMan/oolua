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

#include "oolua_exception.h"
#include <cstring>

namespace OOLUA
{
	namespace ERROR
	{
		void copy_buffer(char* to, char const* from, size_t& sz)
		{
			to[0]='\0';
			to[511]='\0';
			memcpy(to, from, sz < 512 ? sz : 510);
			sz = sz < 512 ? sz : 511;
		}
		size_t copy_buffer(char* to, char const* from)
		{
			size_t sz = strlen(from);
			copy_buffer(to, from, sz);
			return sz;
		}
	} // namespace ERROR //NOLINT(readability/namespace)
} // namespace OOLUA
