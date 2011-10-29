#ifndef HTTP_BOOTER_NODE_HTTP_BOOTER_H
#define HTTP_BOOTER_NODE_HTTP_BOOTER_H

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "http-booter.h"

#define HB_FUNCTION(name) v8::Handle<v8::Value> name(const v8::Arguments &args)
#define HB_SETTER(name) v8::Handle<v8::Value> name(v8::Local<String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
#define HB_GETTER(name) v8::Handle<v8::Value> name(v8::Local<v8::String> property, const v8::AccessorInfo &info)

#define HB_FORWARD_1(c_func, v8_func, ret_type) \
	HB_FUNCTION(v8_func) \
	{ \
		auto hb = Unwrap(args.This()); \
		assert(hb); \
		return ret_type(hb->c_func()); \
	}

#define HB_FORWARD(c_func, v8_func) \
	HB_FUNCTION(v8_func) \
	{ \
		auto hb = Unwrap(args.This()); \
		assert(hb); \
		hb->c_func(); \
		return args.This(); \
	}

#define HB_FORWARD_GETTER(c_name, v8_name, ret_type) \
	v8::Handle<v8::Value> v8_name(v8::Local<v8::String> property, const v8::AccessorInfo &info) \
	{ \
		auto hb = Unwrap(info.This()); \
		assert(hb); \
		return ret_type(hb->c_name); \
	}

#define HB_SET_GETTER(t, name, getter) t->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol(name), getter)

#define HB_PSYMBOL(name, value) v8::Persistent<v8::String> name = v8::Persistent<v8::String>::New(v8::String::New(#value))
#define HB_CONST(t, constant) NODE_DEFINE_CONSTANT(t, constant)

HB_NS_START

class NodeHttpBooter :  public HttpBooter
{
	v8::Persistent<v8::Object> handle;
	NodeHttpBooter(v8::Handle<v8::Object> object)
	: HttpBooter()
	{
		// For we always close the object by manual, 
		// so skip adjust v8 memory.
		// on_alloc=&do_alloc;
		// on_free=&do_free;

		// bind v8 object and c++ object
		object->SetPointerInInternalField(0, this);
		handle = v8::Persistent<v8::Object>::New(object);
		handle.MakeWeak(this, destructor);
	}

	// Behavior when object is going to destroyed by GC
	static void destructor(v8::Persistent<v8::Value> object, void *data)
	{
		puts("[WARNING] NodeHttpBooter wasn't close by manual,"
		     "so the object will not be free until GC performed,"
		     "and GC will takes a lot of time to anazyle weak object!!");
		NodeHttpBooter * hb = reinterpret_cast<NodeHttpBooter*>(data);
		delete hb;
	}

	~NodeHttpBooter()
	{
	}

    public:
	void close()
	{
		handle->SetPointerInInternalField(0, NULL);
		handle.Dispose();
		delete this;
	}

	static NodeHttpBooter * create(v8::Handle<v8::Object> object)
	{
		return new NodeHttpBooter(object);
	}

	// Adjust V8 Memory
	// ================
	static void do_alloc(size_t size)
	{
		v8::V8::AdjustAmountOfExternalAllocatedMemory(+size);
	}

	static void do_free(size_t size)
	{
		v8::V8::AdjustAmountOfExternalAllocatedMemory(-size);
	}

	static HB_FUNCTION(New)
	{
		create(args.This());
		return args.This();
	}

	static NodeHttpBooter * Unwrap(v8::Handle<v8::Object> object)
	{
		return (NodeHttpBooter*)object->GetPointerFromInternalField(0);
	}


	// Setting up class HttpBooter and export to target
	static void Initialize(v8::Handle<v8::Object> target)
	{
		auto t = v8::FunctionTemplate::New(New);
		t->InstanceTemplate()->SetInternalFieldCount(1);

		NODE_SET_PROTOTYPE_METHOD(t , "write"     , Write);
		NODE_SET_PROTOTYPE_METHOD(t , "dump"      , Dump);
		NODE_SET_PROTOTYPE_METHOD(t , "close"     , Close);
		NODE_SET_PROTOTYPE_METHOD(t , "failed"    , Failed);
		NODE_SET_PROTOTYPE_METHOD(t , "succeeded" , Succeeded);
		NODE_SET_PROTOTYPE_METHOD(t , "good"      , Good);
		NODE_SET_PROTOTYPE_METHOD(t , "bad"       , Bad);

		HB_SET_GETTER(t, "status",        GetStatus);
		HB_SET_GETTER(t, "method",        GetMethod);
		HB_SET_GETTER(t, "method_id",     GetMethodId);
		HB_SET_GETTER(t, "error_code",    GetErrorCode);
		HB_SET_GETTER(t, "error_message", GetErrorMessage);
		HB_SET_GETTER(t, "url",           GetUrl);

		NODE_SET_PROTOTYPE_METHOD(t , "format"    , Format);

		HB_SET_GETTER(t , "_get"     , GetGet);
		HB_SET_GETTER(t , "_post"    , GetPost);
		HB_SET_GETTER(t , "_cookie"  , GetCookie);
		HB_SET_GETTER(t , "_headers" , GetHeaders);
		HB_SET_GETTER(t , "_files"   , GetFiles);

		HB_CONST(t, METHOD_GET);
		HB_CONST(t, METHOD_POST);
		HB_CONST(t, METHOD_OTHER);
		HB_CONST(t, READ_START);
		HB_CONST(t, READ_FINISH);

		target->Set(v8::String::NewSymbol("HttpBooter"), t->GetFunction());
	}

	// Methods
	// =======
	static HB_FORWARD(close       , Close);
	static HB_FORWARD(dump        , Dump);
	static HB_FORWARD_1(failed    , Failed    , v8::Boolean::New);
	static HB_FORWARD_1(succeeded , Succeeded , v8::Boolean::New);
	static HB_FORWARD_1(good      , Good      , v8::Boolean::New);
	static HB_FORWARD_1(bad       , Bad       , v8::Boolean::New);

	// Getter
	// ======
	static HB_FORWARD_GETTER(status        , GetStatus       , v8::Int32::New);
	static HB_FORWARD_GETTER(error_code    , GetErrorCode    , v8::Int32::New);
	static HB_FORWARD_GETTER(error_message , GetErrorMessage , v8::String::New);
	static HB_FORWARD_GETTER(method        , GetMethod       , v8_string);
	static HB_FORWARD_GETTER(method_id     , GetMethodId     , v8::Int32::New);

	static HB_FORWARD_GETTER(headers , GetHeaders , source_pairs_to_v8_object);
	static HB_FORWARD_GETTER(get     , GetGet     , source_pairs_to_v8_object);
	static HB_FORWARD_GETTER(post    , GetPost    , source_pairs_to_v8_object);
	static HB_FORWARD_GETTER(cookie  , GetCookie  , source_pairs_to_v8_object);
	static HB_FORWARD_GETTER(files   , GetFiles   , files_to_v8_object);

	static HB_GETTER(GetUrl)
	{
		static HB_PSYMBOL(SYM_SCHEME   , scheme);
		static HB_PSYMBOL(SYM_HOST     , host);
		static HB_PSYMBOL(SYM_PORT     , port);
		static HB_PSYMBOL(SYM_USER     , user);
		static HB_PSYMBOL(SYM_PASS     , pass);
		static HB_PSYMBOL(SYM_PATH     , path);
		static HB_PSYMBOL(SYM_QUERY    , query);
		static HB_PSYMBOL(SYM_FRAGMENT , fragment);

		auto hb = Unwrap(info.This());
		Url url(hb->request_uri);
		auto object = v8::Object::New();
		object->Set(SYM_SCHEME   , v8_string(url.scheme));
		object->Set(SYM_HOST     , v8_string(url.host));
		object->Set(SYM_PORT     , v8_string(url.port));
		object->Set(SYM_USER     , v8_string(url.user));
		object->Set(SYM_PASS     , v8_string(url.pass));
		object->Set(SYM_PATH     , v8_string(url.path));
		object->Set(SYM_QUERY    , v8_string(url.query));
		object->Set(SYM_FRAGMENT , v8_string(url.fragment));
		return object;
	}

	// Convert headers get post cookie files to v8
	static HB_FUNCTION(Format)
	{
		auto self = args.This();
		auto hb = Unwrap(args.This());
		assert(hb);

		// static HB_PSYMBOL(SYM_HEADERS , headers);
		static HB_PSYMBOL(SYM_GET     , get);
		static HB_PSYMBOL(SYM_POST    , post);
		static HB_PSYMBOL(SYM_COOKIE  , cookie);
		static HB_PSYMBOL(SYM_FILES   , files);

		// self->Set(SYM_HEADERS , source_pairs_to_v8_object(hb->headers));
		self->Set(SYM_GET     , source_pairs_to_v8_object(hb->get));
		self->Set(SYM_POST    , source_pairs_to_v8_object(hb->post));
		self->Set(SYM_COOKIE  , source_pairs_to_v8_object(hb->cookie));
		self->Set(SYM_FILES   , files_to_v8_object(hb->files));

		return self;
	}

	static v8::Handle<v8::String> v8_string(const String & string)
	{
		return v8::String::New(string.data, string.length);
	}

	static v8::Handle<v8::Object> v8_buffer(const String & string)
	{
		return node::Buffer::New((char*)string.data, string.length)->handle_;
	}

	static v8::Handle<v8::Value> node_buffer(const Source & source)
	{
		return v8::Null();
	}

	static void smart_save_to_object(v8::Handle<v8::Object> object, const String & key, v8::Handle<v8::Value> v8_value)
	{
		if (key.ends_with("[]"))
		{
			// convert key[] to array
			auto v8_key = v8_string(key.substr(0, -2));
			auto value  = object->Get(v8_key);
			if (value->IsArray())
			{
				auto array = v8::Handle<v8::Array>::Cast(value);
				array->Set(array->Length(), v8_value);
			}
			else
			{
				auto array = v8::Array::New();
				array->Set(0, v8_value);
				object->Set(v8_key, array);
			}
		}
		else
		{
			auto v8_key  = v8_string(key);
			object->Set(v8_key, v8_value);
		}
	}

	static v8::Handle<v8::Object> source_pairs_to_v8_object(const SourcePairs & pairs)
	{
		auto object = v8::Object::New();
		for (auto & pair : pairs)
		{
			String key = pair.first;
			auto   value = v8_string(pair.second);
			smart_save_to_object(object, key, value);
		}
		return object;
	}

	static v8::Handle<v8::Object> files_to_v8_object(const Files & files)
	{
		static HB_PSYMBOL(SYM_FILENAME, filename);
		static HB_PSYMBOL(SYM_DATA,     data);
		auto object = v8::Object::New();
		for (auto & file : files)
		{
			String key = file.name;
			auto v8_file = v8::Object::New();
			v8_file->Set(SYM_FILENAME, v8_string(file.filename));
			v8_file->Set(SYM_DATA,     v8_buffer(file.data));
			smart_save_to_object(object, key, v8_file);
		}
		return object;
	}

	static HB_FUNCTION(Write)
	{
		auto hb = Unwrap(args.This());
		assert(hb);
		if(args[0]->IsString())
		{
			v8::String::Utf8Value data(args[0]);
			hb->write(*data, data.length());
		}
		else
		{
			hb->write(node::Buffer::Data(args[0]->ToObject()), node::Buffer::Length(args[0]->ToObject()));
		}
		return args.This();
	}

};

HB_NS_END

#endif
