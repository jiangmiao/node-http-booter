#ifndef HTTP_BOOTER_HTTP_H
#define HTTP_BOOTER_HTTP_H

#include <vector>
#include "source.h"

HB_NS_START

class SourcePair
{
    public:
	Source first;
	Source second;

	SourcePair(const Source & first, const Source & second)
	: first(first)
	, second(second)
	{
	}
};


typedef std::vector<SourcePair> SourcePairs;


extern SourcePairs & parse_str(SourcePairs & dest, Source source, const String & delim_a, const String & delim_b, bool do_unescape);
extern SourcePairs & parse_str(SourcePairs & dest, Source source);
extern SourcePairs & parse_cookie(SourcePairs & dest, Source source);

extern SourcePairs & parse_headers(SourcePairs & dest, Source source);


class Url
{
    public:
	Source scheme;
	Source host;
	Source port;
	Source user;
	Source pass;
	Source path;
	Source query;
	Source fragment;

	Url();
	Url(Source source);
	Url & assign(Source url);
	bool operator == (const Url & right);
};

class File
{
    public:
	Source type;
	Source filename;
	Source name;
	Source data;

	File();
	File & assign(Source source);
	File(Source source);
};

typedef std::vector<File> Files;


HB_NS_END


#endif
