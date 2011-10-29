#ifndef HTTP_BOOTER_SOURCE_INL_H
#define HTTP_BOOTER_SOURCE_INL_H

#include "common.h"
#include "string.h"

HB_NS_START

struct Source
{
	const char * const *source;
	int          offset;
	int          length;

	Source(const char * const *source, const String &string)
	: source(source)
	, offset(string.data - *source)
	, length(string.length)
	{
	}

	Source(const char * const *source, int offset, int length)
	: source(source)
	, offset(offset)
	, length(length)
	{
		
	}

	Source(char const * const & str)
	: source(&str)
	, offset(0)
	, length(strlen(str))
	{
	}

	Source()
	: source(NULL)
	, offset(0)
	, length(0)
	{
	}

	String to_s() const
	{
		return *this;
	}

	operator String() const
	{
		if (length == 0)
			return String();
		return String(*source + offset, length);
	}

	operator bool() const
	{
		return length > 0;
	}

	bool operator == (const String & right) const
	{
		// this->show(); printf("  ==  "); right.show(); puts("");
		return (String)(*this) == right;
	}

	bool strict_equals(const Source & right)
	{
		return source == right.source && offset == right.offset && length == right.length;
	}


	void show() const
	{
		to_s().show();
	}

	void clear()
	{
		source = NULL;
		offset = 0;
		length = 0;
	}

	Source create(const String & str) const
	{
		return Source(source, str);
	}

	bool split(const String &delim, Source & first, Source & second) const
	{
		Source self = *this;
		String first_str, second_str;
		int result = this->to_s().split(delim, first_str, second_str);
		first  = self.create(first_str);
		second = self.create(second_str);
		return result;
	}
};

HB_NS_END

#endif

