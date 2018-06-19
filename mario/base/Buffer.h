#ifndef MARIO_BASE_BUFFER_H
#define MARIO_BASE_BUFFER_H

#include <algorithm>
#include <string>
#include <vector>

namespace mario {

class Buffer {

public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer()
        : _buffer(kCheapPrepend + kInitialSize)
        , _readerIndex(kCheapPrepend)
        , _writerIndex(kCheapPrepend) {}

    void swap(Buffer& rhs) {
        _buffer.swap(rhs._buffer);
        std::swap(_readerIndex, rhs._readerIndex);
        std::swap(_writerIndex, rhs._writerIndex);
    }

    size_t readableBytes() const {
        return _writerIndex - _readerIndex;
    }

    size_t writableBytes() const {
        return _buffer.size() - _writerIndex;
    }

    size_t prependableBytes() const {
        return _readerIndex;
    }

    char* peek() {
        return begin() + _readerIndex;
    }

    void retrieve(size_t len) {
        _readerIndex += len;
    }

    void retrieveUntil(const char* end) {
        retrieve(end - peek());
    }

    void retrieveAll() {
        _readerIndex = kCheapPrepend;
        _writerIndex = kCheapPrepend;
    }

    std::string retrieveAsString() {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string& str) {
        append(str.data(), str.length());
    }

    void append(const char* data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    char* beginWrite() {
        return begin() + _writerIndex;
    }

    void hasWritten(size_t len) {
        _writerIndex += len;
    }

    void shrink(size_t reserve) {
        std::vector<char> buf(kCheapPrepend + readableBytes() + reserve);
        std::copy(peek(), peek()+readableBytes(), buf.begin()+kCheapPrepend);
        buf.swap(_buffer);
    }

    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() {
        return &*_buffer.begin();
    }

    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            _buffer.resize(_writerIndex + len);
        } else {
            size_t readable = readableBytes();
            std::copy(begin()+_readerIndex, begin()+_writerIndex, begin()+kCheapPrepend);
            _readerIndex = kCheapPrepend;
            _writerIndex = _readerIndex + readable;
        }
    }

private:
    std::vector<char> _buffer;
    size_t _readerIndex;
    size_t _writerIndex;
};

}

#endif
