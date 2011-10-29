#ifndef HTTP_BOOTER_HTTP_BOOTER_H
#define HTTP_BOOTER_HTTP_BOOTER_H

#include "string.h"
#include "buffer.h"
#include "http.h"

HB_NS_START

class HttpBooter
{
    public:
	enum Status
	{
		READ_ERROR            = -1,
		READ_START            = 0,
		READ_HEADER           = 1,
		READ_HEADER_FINISH    = 2,
		READ_BODY             = 3,
		READ_BODY_FINISH      = 4,
		READ_MULTIPART_BODY   = 5,
		READ_MULTIPART_HEADER = 6,
		READ_MULTIPART_FINISH = 7,
		READ_FINISH
	};

	enum
	{
		READ_NEXT,
		READ_BREAK
	};

	enum ErrorCode
	{
		ERR_OK,
		// start line must be METHOD URI HTTP_PROTOCOL
		ERR_INVALID_START_LINE,
		// the data afte boundary must be CRLF or --
		ERR_UNEXPECTED_MULTIPART_DATA,
		ERR_DUPLICATE_BOUNDARY,
		ERR_USER_CANCELLED,
		ERR_NUMBERS

	};

	enum Method
	{
		METHOD_GET,
		METHOD_POST,
		METHOD_OTHER
	};

	// Post
	enum ContentType
	{
		CONTENT_TYPE_POSTDATA,
		CONTENT_TYPE_MULTIPART
	};

	Status       status;
	ErrorCode    error_code;
	const char * error_message;
	
	int    pos;
	Buffer buffer;
	Buffer saved_buffer;

	Method method_id;
	Source method;
	Source request_uri;
	Source http_version;

	ContentType content_type;
	int         content_length;
	int         content_remain;
	char *      boundary_data;
	String      boundary;

	SourcePairs headers;
	SourcePairs get;
	SourcePairs post;
	SourcePairs cookie;

	Files files;
	File* file;

	HttpBooter();
	virtual ~HttpBooter();

	void write(const void * data, int length);
	void write(const char * data);

	int process();

	static Buffer delim;

	String token;

	// if status is READ_FINISH
	bool succeeded();

	// if status isn't READ_FINISH
	bool failed();

	// if status is READ_ERROR
	bool bad();

	// if status isn't READ_ERROR
	bool good();

	void dump();
	void dump(const SourcePairs & pairs);

	void close();

	// return -1 cause ERR_USER_CANCELLED.
	int  (*on_process)(void *data);
	void *on_process_data;
	void (*on_alloc)(size_t size);
	void (*on_free)(size_t size);
    private:
	Source source(const String &string);
	Source source_copy(const String &string);
	bool read_token(const String &delim);
	bool read_size(size_t n);

	int read_start();
	int read_header();
	int read_header_finish();
	int read_body();
	int read_body_finish();
	int read_multipart_header();
	int read_multipart_body();
	int read_multipart_finish();
	int read_finish();

	int next(Status status);
	int error(ErrorCode code);

	void consume(size_t n);

	size_t memory;
};


HB_NS_END

#endif
