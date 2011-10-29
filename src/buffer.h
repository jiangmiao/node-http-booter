#ifndef HTTP_BOOTER_BUFFER_H
#define HTTP_BOOTER_BUFFER_H

#include "common.h"

HB_NS_START

class Buffer
{
    protected:
	char     *memory_;
	size_t cursor_;
	size_t size_;
	size_t reserve_;

	struct {
		size_t cursor;
		size_t size;
	} savedCursor;
    public:
	bool     auto_clear;
	Buffer(size_t reserve = 1024);
	Buffer(const char *data);
	~Buffer();

	// Data Control
	// ============
	char * &memory();
	char * data();
	void   zero();
	void   clear();

	char * prepare(size_t n);
	char * prepare_acc(size_t n);
	void   consume(size_t n);
	void   commit(size_t n);

	// Size Getter
	// ===========
	size_t size    ( ) const;
	size_t remain  ( ) const;
	size_t used    ( ) const;
	size_t cursor  ( ) const;
	size_t reserve ( ) const;

	// Writer
	// ======
	void write(const char *data);
	void write(const void *data, size_t size);

	// Utils
	size_t find(const char *needle, size_t size, size_t offset);
	void     show();
};

HB_NS_END

#endif
