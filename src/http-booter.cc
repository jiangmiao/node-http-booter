#include "http.h"
#include "http-booter.h"
#include "utils.h"

HB_NS_START

const String 
STR_POST("POST"),
STR_GET("GET"),
STR_BOUNDARY("boundary="),
STR_CONTENT_LENGTH("content-length"),
STR_CONTENT_TYPE("content-type"),
STR_CONTENT_DISPOSITION("content-disposition"),
STR_NAME("name"),
STR_FILENAME("filename"),
STR_CRLF("\r\n"),
STR_CRLF2("\r\n\r\n"),
STR_SPACE(" "),
STR_EQUAL("="),
STR_COLON(":"),
STR_SEMICOLON(";"),
STR_AND("&"),
STR_QUESTION("?"),
STR_COOKIE("cookie"),
STR_FRAGMENT("#"),
STR_MULTIPART_FINISH("--");


const char *error_messages[] = {
	"ok",
	"invalid start line",
	"unexpected multipart data",
	"duplicated boundary",
	"user cancelled"
};

HttpBooter::HttpBooter()
: status(READ_START)
, error_code(ERR_OK)
, error_message(error_messages[0])
, pos(0)
, buffer(256)
, content_type(CONTENT_TYPE_POSTDATA)
, content_length(0)
, content_remain(0)
, boundary_data(NULL)
, file(NULL)
, on_process(NULL)
, on_process_data(NULL)
, on_alloc(NULL)
, on_free(NULL)
, memory(0)
{
	get.reserve(10);
	buffer.auto_clear       = false;
	saved_buffer.auto_clear = false;
}

HttpBooter::~HttpBooter()
{
	if (boundary_data)
		free(boundary_data);
	if (memory && on_free)
		on_free(memory);
}

void HttpBooter::write(const void * data, int length)
{
	if (content_length > 0 && status >= READ_HEADER_FINISH )
	{
		if (length > content_remain)
		{
			length = content_remain;
		}
	}

	if (length == 0)
	{
		return;
	}

	buffer.write(data, length);
	while(process() != READ_BREAK) 
		continue; 
}

void HttpBooter::write(const char * data)
{
	HttpBooter::write(data, strlen(data));
}

int HttpBooter::process()
{
	if(on_process)
	{
		if(on_process(on_process_data) == -1)
		{
			return error(ERR_USER_CANCELLED);
		}
	}

	int n = 0;
	switch (status)
	{
	    case READ_START            : n = read_start()            ; break ; 
	    case READ_HEADER           : n = read_header()           ; break ; 
	    case READ_HEADER_FINISH    : n = read_header_finish()    ; break ; 
	    case READ_MULTIPART_BODY   : n = read_multipart_body()   ; break ; 
	    case READ_MULTIPART_HEADER : n = read_multipart_header() ; break ; 
	    case READ_MULTIPART_FINISH : n = read_multipart_finish() ; break ; 
	    case READ_BODY             : n = read_body()             ; break ; 
	    case READ_BODY_FINISH      : n = read_body_finish()      ; break ; 
	    case READ_FINISH:
	    case READ_ERROR:
		return READ_BREAK;
	}
	return n;
}

bool HttpBooter::read_token(const String & delim)
{
	pos = buffer.find(delim.data, delim.length, pos);
	if (pos + (size_t)delim.length <= buffer.size())
	{
		token =  {buffer.data(), pos + delim.length};
		consume(token.length);
		return true;
	}
	else
	{
		return false;
	}
}

bool HttpBooter::read_size(size_t n)
{
	if (buffer.size() < n)
	{
		return false;
	}
	else
	{
		token = {buffer.data(), n};
		consume(token.length);
		return true;
	}
}

void HttpBooter::consume(size_t n)
{
	if (content_remain && status >= READ_HEADER_FINISH)
	{
		content_remain -= n;
		if (content_remain < 0)
			content_remain = 0;

	}
	buffer.consume(n);
	pos = 0;
}

int HttpBooter::error(ErrorCode code)
{
	error_code = code;
	if (0 <= code && code < ERR_NUMBERS )
		error_message = error_messages[code];
	status = READ_ERROR;
	return READ_BREAK;
}

int HttpBooter::next(Status status)
{
	this->status = status;
	return READ_NEXT;
}


int HttpBooter::read_start()
{
	if(read_token(STR_CRLF))
	{
		String line = token.substr(0, -STR_CRLF.length);
		String method, request_uri, http_version, ignore;
		if ( line.split3(" ", method, request_uri, http_version) &&
		     http_version.split("/", ignore, http_version)       )
		{
			if (method.casecmp(STR_POST) == 0)
				this->method_id = METHOD_POST;
			else if (method.casecmp(STR_GET) == 0)
				this->method_id = METHOD_GET;
			else
				this->method_id = METHOD_OTHER;

			this->request_uri  = source_copy(request_uri);
			this->http_version = source(http_version);
			this->method       = source(method);
			String querystring;
			// Simple split by ?
			if(request_uri.split(STR_QUESTION, ignore, querystring))
			{
				querystring.split(STR_FRAGMENT, querystring, ignore);
				parse_str(get, source(querystring));
			}
			// url.assign(this->request_uri);
			// parse_str(get, url.query);
			return next(READ_HEADER);
		} 
		else 
		{
			return error(ERR_INVALID_START_LINE);
		}
	} else
	{
		return READ_BREAK;
	}
}

int HttpBooter::read_body()
{
	if (read_size(content_length))
	{
		String fields = token;
		parse_str(post, source(fields));
		return next(READ_BODY_FINISH);
	} 
	else
	{
		return READ_BREAK;
	}
}

int HttpBooter::read_body_finish()
{
	return next(READ_FINISH);
}

int HttpBooter::read_header()
{
	if (read_token(STR_CRLF))
	{
		String key, value;
		if (token == STR_CRLF)
		{
			return next(READ_HEADER_FINISH);
		}
		if (token.split(": ", key, value))
		{
			value = value.substr(0, - STR_CRLF.length).ltrim();
			key.downcase();
			headers.push_back({source_copy(key), source_copy(value)});
			if (method_id == METHOD_POST) 
			{
				if (key.equals(STR_CONTENT_LENGTH)) 
				{
					content_length = atoi(value.data);
				} 
				else if (key.equals(STR_CONTENT_TYPE))
				{
					if (boundary_data)
					{
						return error(ERR_DUPLICATE_BOUNDARY);
					}
					String ignore, boundary;
					if (value.split(STR_BOUNDARY, ignore, boundary))
					{
						int boundary_size = boundary.length + 2 + 1;
						boundary_data = (char*)malloc(boundary_size);
						boundary_data[boundary.length+2] = 0;
						strncpy(boundary_data, "--", 2);
						strncpy(boundary_data+2, boundary.data, boundary.length);
						this->boundary = boundary_data;
						content_type = CONTENT_TYPE_MULTIPART;
					}
				}
			}

			if (key.equals(STR_COOKIE))
			{
				parse_cookie(cookie, source(value));
			}
			return next(READ_HEADER);
		} else
		{
			// ignore the header without key
			return READ_BREAK;
		}
	} else 
	{
		return READ_BREAK;
	}
}

int HttpBooter::read_header_finish()
{
	// raw_header = Source(&buffer.memory(), 0, buffer.cursor());
	content_remain = content_length;
	if (content_length)
	{
		buffer.prepare_acc(content_length);
	}
	if (boundary_data)
	{
		return next(READ_MULTIPART_BODY);
	}
	else if (content_length)
	{
		return next(READ_BODY);
	}
	if (on_alloc)
	{
		memory = buffer.reserve() + 256;
		on_alloc(memory);
	}
	return next(READ_FINISH);
}

int HttpBooter::read_multipart_body()
{
	if (buffer.size() < boundary.length + 2)
	{
		return READ_BREAK;
	}
	if (read_token(boundary))
	{
		// body
		if (file)
		{
			String body = token.substr(0, -boundary.length-2);
			file->data = source(body);
			if (!file->filename)
			{
				if (file->name)
				{
					post.push_back({file->name, file->data});
				}
				files.pop_back();
				file = NULL;
			}
		}
		read_size(2);
		if (token == STR_CRLF)
		{
			// New multipart started
			files.push_back(File());
			file = &*files.rbegin();
			return next(READ_MULTIPART_HEADER);
		} 
		else if (token == STR_MULTIPART_FINISH)
		{
			// decrease content_remain as many as possible
			read_size(buffer.size());
			consume(buffer.size());
			return next(READ_MULTIPART_FINISH);
		}
		else
		{
			return error(ERR_UNEXPECTED_MULTIPART_DATA);
		}
	}
	return READ_BREAK;
}

int HttpBooter::read_multipart_header()
{
	if (read_token(STR_CRLF))
	{
		if(token == STR_CRLF)
		{
			return next(READ_MULTIPART_BODY);
		}
		else
		{
			String header = token.substr(0, -STR_CRLF.length);
			String key, value;
			header.split(STR_COLON, key, value);
			key.downcase();
			if (key.equals(STR_CONTENT_DISPOSITION))
			{
				String field;
				while(value)
				{
					value.split(STR_SEMICOLON, field, value);
					value.ltrim();
					String field_key, field_value;
					field.split(STR_EQUAL, field_key, field_value);
					// remove " "
					if (field_value.length >= 2)
					{
						field_value = field_value.substr(1, field_value.length-2);
					}
					field_key.downcase();
					if (field_key == STR_NAME)
					{
						file->name = source(field_value);
					}
					else if (field_key == STR_FILENAME)
					{
						file->filename = source(field_value);
					}
				}
			}
			else if (key == STR_CONTENT_TYPE)
			{
				file->type = source(value);

				//multipart.header.push_back({source(key), source(value)});
			}

			return next(READ_MULTIPART_HEADER);
		}
	}
	return READ_BREAK;
}

int HttpBooter::read_multipart_finish()
{
	if (content_length && content_remain)
	{
		if (read_size(content_remain))
		{
			// keep reading until content_remain become 0
			consume(content_remain);
			return next(READ_FINISH);
		}
		else
		{
			return READ_BREAK;
		}
	}
	return next(READ_FINISH);
}

int HttpBooter::read_finish()
{
	return READ_BREAK;
}


bool HttpBooter::failed()
{
	return status != READ_FINISH;
}

bool HttpBooter::succeeded()
{
	return status == READ_FINISH;
}

bool HttpBooter::good()
{
	return status != READ_ERROR;
}

bool HttpBooter::bad()
{
	return status == READ_ERROR;
}

Source HttpBooter::source(const String &string)
{
	return Source(&buffer.memory(), string);
}

Source HttpBooter::source_copy(const String &string)
{
	saved_buffer.write(string.data, string.length);
	Source s(&saved_buffer.memory(), saved_buffer.cursor(), string.length);
	saved_buffer.consume(string.length);
	return s;
}

void HttpBooter::dump()
{
	puts("---- HEADERS ----");
	dump(headers);
	puts("---- GET ----");
	dump(get);
	puts("---- POST ----");
	dump(post);
	puts("---- COOKIE ---- ");
	dump(cookie);
	puts("---- MULTIPART ----");
	for(auto & file : files)
	{
		puts("-- FILE --");
		printf("type: ");
		file.type.show();
		puts("");
		printf("filename: ");
		file.filename.show();
		puts("");
		printf("name: ");
		file.name.show();
		puts("");
		printf("data ");
		file.data.show();
		puts("");
	}
}

void HttpBooter::dump(const SourcePairs & pairs)
{
	for (auto & pair : pairs)
	{
		pair.first.show();
		printf(": ");
		pair.second.show();
		puts("");
	}
}


void HttpBooter::close()
{
	if (memory)
		on_free(memory);
}

HB_NS_END
