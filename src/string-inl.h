#ifndef HTTP_BOOTER_STRING_INL_H
#define HTTP_BOOTER_STRING_INL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

HB_NS_START

// String never alloc or free memory
// Maybe split the the string to ConstString and MutableString is better.
class String
{
    public:
	const char * data;
	size_t       length;

	String(const char *data)
	: data(data)
	, length(strlen(data))
	{
	}

	String(const char *data, size_t length)
	: data(data)
	, length(length)
	{
	}

	String()
	: data(NULL)
	, length(0)
	{
	}

	void show() const
	{
		if (length)
			fwrite(data, 1, length, stdout);
		else
			printf("%s", "(null)");
	}

	bool operator == (const String & right) const
	{
		if (length == 0 && right.length == 0)
		{
			return true;
		}
		return length == right.length && memcmp(data, right.data, length) == 0;
	}

	int find(const String & delim) const
	{
		if (length < delim.length)
		{
			return -1;
		}

		const char * src_st = data;
		const char * src_ed = data + length - delim.length; 
		const char * delim_st = delim.data;
		int delim_length = delim.length;
		do
		{
			if (memcmp((void*)src_st, (void*)delim_st, delim_length) == 0)
				return src_st - data;
		} while (src_st++ != src_ed);
		return -1;
	}

	// split String to first and second by delim
	// delim and second can the same as String
	// returns
	//   true  split succeeded
	//   false split failed, first will be the same as this
	bool split(const String &delim, String & first, String & second) const
	{
		int pos =find(delim);
		if (pos == -1)
		{
			first = *this;
			second.clear();
			return false;
		}
		// Clone self make the input could be self
		const char * data   = this->data;
		int          length = this->length;

		first.data    = data;
		first.length  = pos;
		second.data   = data   + pos + delim.length;
		second.length = length - pos - delim.length;
		return true;
	}

	bool split3(const String &delim, String & first, String & second, String & third) const
	{
		return split(delim, first, second) && second.split(delim, second, third);
	}

	String substr(int pos, int length) const
	{
		if (pos < 0)
		{
			pos += this->length;
		}

		if (pos < 0)
		{
			return "";
		}

		if (length < 0)
		{
			length += this->length;
		}

		if (length < 0)
		{
			return "";
		}

		if ((size_t)pos >= this->length)
		{
			return "";
		}

		if ((size_t)pos + (size_t)length > this->length)
		{
			length = this->length - pos;
		}

		return String(data + pos, length);
	}

	int casecmp(const String &dest) const
	{
		if (length == 0 && dest.length == 0)
		{
			return 0;
		}
		if (length < dest.length)
		{
			return -1;
		}
		if (length > dest.length)
		{
			return 1;
		}
		return strncasecmp(data, dest.data, length);
	}

	int cmp(const String &dest) const
	{
		if (length == 0 && dest.length == 0)
		{
			return 0;
		}
		if (length < dest.length)
		{
			return -1;
		}
		if (length > dest.length)
		{
			return 1;
		}
		return strncmp(data, dest.data, length);
	}

	bool equals(const String & dest) const
	{
		return cmp(dest) == 0;
	}

	bool iequals(const String & dest) const
	{
		return casecmp(dest) == 0;
	}

	void clear()
	{
		length = 0;
		data   = NULL;
	}

	bool empty() const
	{
		return length == 0;
	}

	operator bool () const
	{
		return !empty();
	}

	String & ltrim()
	{
		char * st = (char*)data;
		char * ed = st + length;
		while (st != ed)
		{
			// isspace(*str++)
			if (*st++ == ' ')
			{
				++data;
				--length;
			}
			else
			{
				break;
			}
		}
		return *this;
	}

	String & rtrim()
	{
		char * st = (char*)data - 1;
		char * ed = st + length;
		while (ed != st)
		{
			if (*ed-- == ' ')
			{
				--length;
			}
			else
			{
				break;
			}
		}
		return *this;
	}

	String & trim()
	{
		ltrim();
		rtrim();
		return *this;
	}

	// will modify the data
	String & downcase()
	{
		char * st = (char*)data;
		char * ed = st + length;
		while (st != ed)
		{
			char c = *st;
			if ( 'A' <= c && c <= 'Z')
			{
				*st = c - 'A' + 'a';
			}
			st++;
		}
		return *this;
	}

	// will modify the data
	String & upcase()
	{
		char * st = (char*)data;
		char * ed = st + length;
		while (st != ed)
		{
			char c = *st;
			if ( 'a' <= c && c <='z' )
			{
				*st = c - 'a' + 'A';
			}
			st++;
		}
		return *this;
	}

	bool ends_with(const String & str) const
	{
		if (length < str.length)
		{
			return false;
		}
		return memcmp(data + length - str.length, str.data, str.length) == 0;
	}

	bool starts_with(const String & str) const
	{
		if (length < str.length)
		{
			return false;
		}
		return memcmp(data, str.data, str.length) == 0;
	}

};


HB_NS_END

#endif
