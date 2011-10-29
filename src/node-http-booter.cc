#include <node.h>
#include <node_buffer.h>

#include "node-http-booter.h"

using namespace hb;


extern "C"
void init(v8::Handle<v8::Object> target)
{
	NodeHttpBooter::Initialize(target);
}

