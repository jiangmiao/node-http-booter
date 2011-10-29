#include "http.h"
#include "utils.h"

HB_NS_START

const String 
STR_POST("POST"),
STR_GET("GET"),
STR_BOUNDARY("boundary="),
STR_CONTENT_LENGTH("Content-Length"),
STR_CONTENT_TYPE("Content-Type"),
STR_CONTENT_DISPOSITION("Content-Disposition"),
STR_CRLF("\r\n"),
STR_CRLF2("\r\n\r\n"),
STR_SPACE(" "),
STR_EQUAL("="),
STR_COLON(":"),
STR_SEMICOLON(";"),
STR_AND("&"),
STR_QUESTION("?"),
STR_COOKIE("cookie"),
STR_MULTIPART_FINISH("--");


// Parse
// =====
SourcePairs & parse_str(SourcePairs & dest, Source source, const String & delim_a, const String & delim_b, bool do_unescape)
{
	String fields = source.to_s();
	String field, key, value;
	while(fields)
	{
		fields.split(delim_a,  field, fields);
		field.split(delim_b, key,   value);
		key.trim();
		if (key.length)
		{
			value.trim();
			if (do_unescape)
			{
				dest.push_back({source.create(unescape(key)), source.create(unescape(value))});
			}
			else
			{
				dest.push_back({source.create(key), source.create(value)});
			}
		}
	}
	return dest;
}

SourcePairs & parse_str(SourcePairs & dest, Source source)
{
	return parse_str(dest, source, STR_AND, STR_EQUAL, true);
}

SourcePairs & parse_cookie(SourcePairs & dest, Source source)
{
	return parse_str(dest, source, STR_SEMICOLON, STR_EQUAL, true);
}

SourcePairs & parse_headers(SourcePairs & dest, Source source)
{
	return parse_str(dest, source, STR_CRLF2, STR_COLON, false);
}


// Url
// ===
Url::Url()
{}

Url::Url(Source src)
{
	assign(src);
}

Url & Url::assign(Source url)
{
	/**
	 * scheme :// user : pass @ host : port [/path] ? query # fragment
	 * scheme ://    account  @    domain   [/path] ? query # fragment
	 */
	static const String SCHEME("://"), ACCOUNT("@"), PORT(":"), PATH("/"), USERNAME(":"), QUERY("?"), FRAGMENT("#");
	/*
	String src = url.to_s();


	String field;
	if (!src.split(SCHEME, field, src))
	{
		src = field;
	}
	else
	{
		this->scheme = url.create(field);
	}

	if (src.split(ACCOUNT, field, src))
	{
		String user, pass;
		field.split(USERNAME, user, pass);
		this->user = url.create(user);
		this->pass = url.create(pass);
	}
	else
	{
		src = field;
		this->user.clear();
		this->pass.clear();
	}

	// other # fragment
	if (src.split(FRAGMENT, src, field))
	{
		this->fragment = url.create(field);
	}
	else
	{
		this->fragment.clear();
	}

	// other ? query
	if (src.split(QUERY, src, field))
	{
		this->query = url.create(field);
	}
	else
	{
		this->query.clear();
	}

	// other / path
	if (src.split(PATH, src, field))
	{
		field.data   -= 1;
		field.length += 1;
		this->path = url.create(field);
	}
	else
	{
		this->path.clear();
	}

	// host, port
	src.split(PORT, src, field);
	this->host = url.create(src);
	this->port = url.create(field);

	return *this;
	*/
	/*
	Source src = url;
	Source account, domain;

	if (!src.split(SCHEME, scheme, src))
	{
		src = scheme;
		scheme.clear();
	}

	if(src.split(PATH, domain, src))
	{
		src.split(FRAGMENT , src  , fragment);
		src.split(QUERY    , path  , query);
		if (!path)
		{
			path        = domain;
			path.length = 0;
		}
		path.offset -= 1;
		path.length += 1;
	}
	else
	{
		fragment.clear();
		query.clear();
		path.clear();
	}

	if (domain.split(ACCOUNT, account, domain))
	{
		account.split(USERNAME, user, pass);
	}
	else
	{
		domain = account;
		user.clear();
		pass.clear();
	}

	domain.split(PORT, host , port);
	return *this;
	**/

	// maybe a bit slow
	Source src = url;
	Source account, domainpath, domain;

	if (!src.split(SCHEME, scheme, src))
	{
		src = scheme;
		scheme.clear();
	}

	if (src.split(ACCOUNT, account, src))
	{
		account.split(USERNAME, user, pass);
	}
	else
	{
		src = account;
		account.clear();
	}

	src.split(FRAGMENT , src  , fragment);
	src.split(QUERY    , src  , query);
	if(src.split(PATH  , src  , path))
	{
		path.offset -= 1;
		path.length += 1;
	}
	src.split(PORT, host , port);
	return *this;
}

bool Url::operator == (const Url & r)
{
	return scheme   == r.scheme    && 
	       host     == r.host      && 
	       port     == r.port      && 
	       user     == r.user      && 
	       pass     == r.pass      && 
	       path     == r.path      && 
	       query    == r.query     && 
	       fragment == r.fragment;
}


// File
// ====
File::File()
{}

File::File(Source source)
{}

File & File::assign(Source source)
{
	return *this;
}





HB_NS_END
