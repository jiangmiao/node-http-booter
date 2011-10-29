#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "buffer.h"

HB_NS_START

template <class T> 
inline const T& min ( const T& a, const T& b ) 
{
	return (a<b)?a:b;
}


size_t find(const char *haystack_start, size_t haystack_size, const char *needle, size_t needle_size)
{
	const char *haystack     = haystack_start;
	const char *haystack_end = haystack + haystack_size;
	const char *needle_end   = needle + needle_size;
	const char *first1       = NULL;
	const char *first2       = NULL;

	while (haystack != haystack_end) {
		first1 = haystack;
		first2 = needle;
		while (*first1 == *first2) {
			//printf("equal %c %c %p %p\n", *first1, *first2, haystack, haystack_start);
			++first1;
			++first2;
			if (first1 == haystack_end || first2 == needle_end) {
				return haystack - haystack_start;
			}
		}
		//printf("not equal %c %c %p %p\n", *first1, *first2, haystack, haystack_start);
		++haystack;
	}
	return haystack_size;
}

Buffer::Buffer(size_t reserve)
: memory_(0)
, cursor_(0)
, size_(0)
, auto_clear(true)
{
	if (reserve == 0)
		reserve = 1;
	reserve_ = reserve;
	memory_ = (char*)malloc(reserve_);
}

Buffer::Buffer(const char *data)
: memory_(0)
, cursor_(0)
, size_(0)
{
	reserve_ = 1;
	memory_ = (char*)malloc(reserve_);
	write(data);
}

Buffer::~Buffer()
{
	free(memory_);
}

// Data Controller
// ===============
char * &Buffer::memory()
{
	return memory_;
}

char * Buffer::data()
{
	return memory_ + cursor_;
}

void Buffer::zero()
{
	prepare(1);
	*(memory_+used()) = 0;
}

void Buffer::clear()
{
	cursor_ = 0;
	size_   = 0;
}

char * Buffer::prepare(size_t n)
{
	if (n == 0) 
		n = 1;
	size_t reserve = reserve_;
	while (used() + n > reserve) {
		reserve *= 3;
	}

	if (reserve > reserve_) {
		memory_ = (char*)::realloc(memory_, reserve);
		reserve_ = reserve;
	}
	return memory_ + used();
}

char * Buffer::prepare_acc(size_t n)
{
	if (n == 0) 
		n = 1;
	size_t reserve = reserve_;
	if (used() + n > reserve) {
		reserve = used() + n;
	}

	if (reserve > reserve_) {
		memory_ = (char*)::realloc(memory_, reserve);
		reserve_ = reserve;
	}
	return memory_ + used();
}

void Buffer::commit(size_t n)
{
	size_ += n;
}

void Buffer::consume(size_t n)
{
	if (size_ < n)
		n = size_;
	cursor_ += n;
	size_   -= n;
	if (size_ == 0 && auto_clear) {
		clear();
	}
}


// Size Getter
// ==========
size_t Buffer::size() const
{
	return size_;
}

size_t Buffer::cursor() const
{
	return cursor_;
}
size_t Buffer::remain() const
{
	return reserve_ - used();
}

size_t Buffer::used() const
{
	return cursor_ + size_;
}

size_t Buffer::reserve() const
{
	return reserve_;
}


// Writer
// ======
void Buffer::write(const void *data, size_t n) // }}}
{
	prepare(n);
	memcpy(memory_ + used(), data, n);
	commit(n);
}

void Buffer::write(const char *data)
{
	write(data, strlen(data));
}

// Utils
// =====
size_t Buffer::find(const char *data, size_t size, size_t offset)
{
	if (size == 0) {
		return offset;
	}

	if (offset >= size_) {
		return size_;
	}

	size_t pos =  hb::find(this->data() + offset, size_ - offset, data, size);
	return pos + offset;
}

void Buffer::show()
{
	fwrite(data(), 1, size(), stdout);
	fflush(stdout);
}


HB_NS_END
