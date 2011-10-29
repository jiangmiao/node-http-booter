#include <assert.h>
#include "../src/http-booter.h"

using namespace hb;

int main()
{
	{
		HttpBooter http;
		http.write("GET / HTTP/1.1\r\n\r\n");
		assert(http.succeeded());
		assert(http.method_id ==  HttpBooter::METHOD_GET);
		assert(http.http_version == "1.1");
		assert(http.request_uri == "/" );
	}

	{
		HttpBooter http;
		http.write("GET /?a=hello+%E4%BD%A0%E5%A5%BD&b=world HTTP/1.1\r\n\r\n");
		assert(http.succeeded());
		assert(http.method_id ==  HttpBooter::METHOD_GET);
		assert(http.http_version == "1.1");
		assert(http.get.size() == 2);
		assert(http.get[0].first == "a");
		assert(http.get[0].second == "hello 你好");
	}

	{
		char data[] = "POST /?a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C HTTP/1.1\r\n"
		              "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n"
		              "Host: 127.0.0.1:1337\r\n"
		              "Accept: */*\r\n"
		              "Content-Length: 41\r\n"
		              "Content-Type: application/x-www-form-urlencoded\r\n"
		              "\r\n"
		              "a=%E4%BD%A0%E5%A5%BD&b=%E4%B8%96%E7%95%8C";

		HttpBooter http;
		http.write(data);
		assert(http.succeeded());
		assert(http.get.size() == 2);
		assert(http.get[0].first == "a");
		assert(http.get[0].second == "你好");
		assert(http.post.size() == 2);
		assert(http.post[0].first == "a");
		assert(http.post[0].second == "你好");
	}

	{
		char data[] = "POST / HTTP/1.1\r\n"
		              "User-Agent: curl/7.22.0 (i686-pc-linux-gnu) libcurl/7.22.0 OpenSSL/1.0.0e zlib/1.2.5 libssh2/1.3.0\r\n"
		              "Host: 127.0.0.1:1337\r\n"
		              "Accept: */*\r\n"
		              "Content-Length: 445\r\n"
		              "Content-Type: multipart/form-data; boundary=----------------------------3d684f973b22\r\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"foo\"; filename=\"foo\"\r\n"
		              "Content-Type: application/octet-stream\r\n"
		              "\r\n"
		              "foo\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"bar\"; filename=\"bar\"\r\n"
		              "Content-Type: application/octet-stream\r\n"
		              "\r\n"
		              "bar\n"
		              "\r\n"
		              "------------------------------3d684f973b22\r\n"
		              "Content-Disposition: form-data; name=\"baz\"\r\n"
		              "\r\n"
		              "baz\r\n"
		              "------------------------------3d684f973b22--\r\n";

		HttpBooter http;
		http.write(data);
		assert(http.succeeded());
		http.dump();
	}
	return 0;
}
