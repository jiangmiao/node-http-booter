#include <stdio.h>
#include <assert.h>
#include "../src/string.h"
#include "../src/source.h"
#include "../src/utils.h"
#include "../src/http.h"
#include "../src/buffer.h"

using namespace hb;

void test_string()
{
	String a("hello world");
	String b = a;
	assert( a == b );
	assert( a.length == 11);
	assert( a == "hello world" );
	assert( a.find("") == 0);
	assert( a.find("ll") == 2 );
	assert( a.find("ww") == -1 );
	assert( a.find("h")  == 0 );
	assert( a.find("hello world") == 0);
	assert( String("").find("") == 0);

	String first, second, third;
	a.split(" ", first, second);
	assert( first == "hello" );
	assert( second == "world" );
	second.split("r", second, third);
	assert( second == "wo");
	assert( third == "ld" );

	a.split("ll", first, second);
	assert( first == "he");
	assert( second == "o world");
	assert(!a.split("aoeu", first, second));
	assert( a == first );
	assert( second.empty() );
	assert( !second );

	a = "world";
	a.split("world", first, second);
	assert( first == "" );
	assert( second == "" );

	a = "world";
	assert( a.substr(0, 1) == "w" );
	assert( a.substr(-1, 1) == "d" );
	assert( a.substr(-2, 2) == "ld" );
	assert( a.substr(1, 100) == "orld");
	assert( a.substr(0, -2) == "wor" );

	a = "hello";
	assert( a.cmp("hel") != 0);
	assert( a.cmp("hello") == 0);
	assert( a.cmp("HELLO") != 0);
	assert( a.casecmp("HELLO") == 0);

	a = "hello";
	assert( a.length == 5);

	char content[] = "Content-Length";
	a = content;
	assert( a.downcase() == "content-length");
	assert( a.upcase()   == "CONTENT-LENGTH");

	a = "  hello  ";
	assert( a.ltrim() == "hello  ");
	assert( a.rtrim() == "hello"  );

	a = "  hello ";
	assert( a.trim() == "hello" );

	a = "hello world[]";
	assert( a.starts_with("hell"));
	assert( a.ends_with("[]"));
	assert( !a.starts_with("wo"));
	assert( !a.ends_with("he"));
	assert( !a.starts_with("this is very long string"));
	assert( !a.ends_with("this is very long string"));
}

void test_source()
{
	char * buffer = strdup("Hello");
	String buffer_str = buffer;
	Source buffer_src(&buffer, buffer_str);
	free(buffer);
	buffer = strdup("Hello");
	String hello = "Hello";
	assert( buffer_src.to_s() == "Hello" );
	assert( buffer_src == "Hello" );
	assert( buffer_src == hello );

	Source buffer2(&buffer, hello);
	assert( buffer2 == buffer_src);

	Source b3("hello");
	assert( b3 == "hello");

	const char * world = "world";
	String a = "world";
	Source b4(&world, a);
	assert(b4 == a);

	String ns = b4.create(a);
	assert(ns == b4);

	Source b5;
	assert( b5 == "");

	Source h = "hello world";
	Source first, second;
	h.split(" ", first, second);
	assert(first == "hello");
	assert(second == "world");
}

void test_utils()
{
	String a;
	char str[] = "%E4%BD%A0%E5%A5%BDworld";
	a = str;
	assert( unescape(a) == "你好world");

	a = strcpy(str, "%A");
	assert( unescape(a) == "?");

	a = strcpy(str, "%");
	assert( unescape(a) == "?");

	a = strcpy(str, "%E4%BD%A0");
	assert( unescape(a) == "你" );
}


void test_buffer()
{
	{
		Buffer buffer;
		assert( buffer.size() == 0 );
		buffer.write("hello world");
		assert( buffer.size() == 11 );
		buffer.consume(6);
		assert( buffer.size() == 5 );
	}

	{
		Buffer buffer(1);
		assert( buffer.size() == 0);
		assert( buffer.reserve() == 1);
		buffer.prepare(64);
		assert(buffer.reserve() == 1*3*3*3*3);
		buffer.commit(64);
		assert(buffer.size() == 64);
		buffer.prepare(64);
		assert(buffer.size() == 64);
		assert(buffer.reserve() == 1*3*3*3*3*3);
		buffer.clear();
		assert(buffer.size() == 0);
		assert(buffer.reserve() == 1*3*3*3*3*3);
	}

	{
		Buffer buffer("hello world");
		buffer.prepare_acc(100);
		assert(buffer.reserve() == 111);
	}

	{
		Buffer buffer("hello world");
		assert(buffer.find("ld", 2, 0) == 9);
		assert(buffer.find("he", 2, 0) == 0);
		assert(buffer.find("ff", 2, 0) == 11);
		assert(buffer.find("ldld", 4, 0) == 9);
	}
}

void test_http_url()
{
	Url url("http://www.jiangmiao.org");
	Url e;
	e.scheme = "http";
	e.host   = "www.jiangmiao.org";
	assert(url == e);

	url.assign("http://username:password@hostname/path?arg=value#anchor");
	assert( url.scheme   == "http" );
	assert( url.user     == "username" );
	assert( url.pass     == "password" );
	assert( url.host     == "hostname" );
	assert( url.port     == "" );
	assert( url.path     == "/path" );
	assert( url.query    == "arg=value" );
	assert( url.fragment == "anchor" );

	url.assign("http://username:password@hostname/#anchor");
	assert( url.scheme   == "http" );
	assert( url.user     == "username" );
	assert( url.pass     == "password" );
	assert( url.host     == "hostname" );
	assert( url.port     == "" );
	assert( url.path     == "/" );
	assert( url.query    == "" );
	assert( url.fragment == "anchor" );

	url.assign("/");
	assert( url.path == "/" );

	url.assign("127.0.0.1");
	assert( url.host == "127.0.0.1" );

	url.assign("a@127.0.0.1:8080");
	assert( url.user == "a" );
	assert( url.host == "127.0.0.1" );
	assert( url.port == "8080" );
}

void test_parse_str()
{
	{
	char text[] = "a=%E4%BD%A0%E5%A5%BD+world%20&b=%E4%B8%96%E7%95%8C";
	Source source = text;
	SourcePairs dest;
	parse_str(dest, source);
	assert(dest.size()    == 2);
	assert(dest[0].first  == "a");
	assert(dest[0].second == "你好 world ");
	assert(dest[1].first  == "b");
	assert(dest[1].second == "世界");
}

{
	char text[] = "a=%E4%BD%A0%E5%A5%BD; b=%E4%B8%96%E7%95%8C";
	Source source = text;
	SourcePairs dest;
	parse_cookie(dest, source);
	assert(dest.size()    == 2);
	assert(dest[0].first  == "a");
	assert(dest[0].second == "你好");
	assert(dest[1].first  == "b");
	assert(dest[1].second == "世界");
}
}

int main()
{
	test_string();
	test_source();
	test_utils();
	test_http_url();
	test_parse_str();
	test_buffer();

	puts("done.");
	return 0;
}

