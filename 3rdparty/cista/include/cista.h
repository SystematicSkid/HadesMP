#pragma once


#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#endif


namespace cista {

template <typename TemplateSizeType>
constexpr TemplateSizeType next_power_of_two(TemplateSizeType n) {
  n--;
  n |= n >> 1U;
  n |= n >> 2U;
  n |= n >> 4U;
  n |= n >> 8U;
  n |= n >> 16U;
  if constexpr (sizeof(TemplateSizeType) > 32U) {
    n |= n >> 32U;
  }
  n++;
  return n;
}

}  // namespace cista

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#include <windows.h>
#endif

#include <cinttypes>
#include <memory>


#include <cstdlib>
#include <cstring>


#include <stdexcept>

namespace cista {

inline void verify(bool const condition, char const* msg) {
  if (!condition) {
    throw std::runtime_error(msg);
  }
}

}  // namespace cista

namespace cista {

struct buffer final {
  buffer() : buf_(nullptr), size_(0) {}

  explicit buffer(std::size_t size) : buf_(malloc(size)), size_(size) {
    verify(buf_ != nullptr, "buffer initialization failed");
  }

  explicit buffer(char const* str) : buffer(std::strlen(str)) {
    std::memcpy(buf_, str, size_);
  }

  buffer(char const* str, std::size_t size) : buffer(size) {
    std::memcpy(buf_, str, size_);
  }

  ~buffer() {
    std::free(buf_);
    buf_ = nullptr;
  }

  buffer(buffer const&) = delete;
  buffer& operator=(buffer const&) = delete;

  buffer(buffer&& o) noexcept : buf_(o.buf_), size_(o.size_) {
    o.buf_ = nullptr;
    o.size_ = 0;
  }

  buffer& operator=(buffer&& o) noexcept {
    buf_ = o.buf_;
    size_ = o.size_;
    o.buf_ = nullptr;
    o.size_ = 0;
    return *this;
  }

  inline std::size_t size() const { return size_; }

  inline unsigned char* data() { return static_cast<unsigned char*>(buf_); }
  inline unsigned char const* data() const {
    return static_cast<unsigned char const*>(buf_);
  }

  inline unsigned char* begin() { return data(); }
  inline unsigned char* end() { return data() + size_; }

  inline unsigned char const* begin() const { return data(); }
  inline unsigned char const* end() const { return data() + size_; }

  unsigned char& operator[](size_t i) { return *(data() + i); }
  unsigned char const& operator[](size_t i) const { return *(data() + i); }

  void* buf_;
  std::size_t size_;
};

}  // namespace cista

#include <cinttypes>
#include <algorithm>

namespace cista {

template <typename Fn>
inline void chunk(unsigned const chunk_size, size_t const total, Fn fn) {
  size_t offset = 0;
  size_t remaining = total;
  while (remaining > 0) {
    auto const curr_chunk_size = static_cast<unsigned>(
        std::min(remaining, static_cast<size_t>(chunk_size)));
    fn(offset, curr_chunk_size);
    offset += curr_chunk_size;
    remaining -= curr_chunk_size;
  }
}

}  // namespace cista

#include <cinttypes>
#include <string_view>

namespace cista {

#if defined(CISTA_XXH3)

// xxh3.h: basic_ios::clear: iostream error

using hash_t = XXH64_hash_t;

constexpr auto const BASE_HASH = 0ULL;

template <typename... Args>
constexpr hash_t hash_combine(hash_t h, Args... val) {
  auto xxh3 = [&](auto const& arg) {
    h = XXH3_64bits_withSeed(&arg, sizeof(arg), h);
  };
  ((xxh3(val)), ...);
  return h;
}

inline hash_t hash(std::string_view s, hash_t h = BASE_HASH) {
  return XXH3_64bits_withSeed(s.data(), s.size(), h);
}

template <size_t N>
constexpr hash_t hash(const char (&str)[N], hash_t const h = BASE_HASH) {
  return XXH3_64bits_withSeed(str, N - 1, h);
}

template <typename T>
constexpr uint64_t hash(T const& buf, hash_t const h = BASE_HASH) {
  return XXH3_64bits_withSeed(&buf[0], buf.size(), h);
}

#elif defined(CISTA_WYHASH)

// wyhash.h: basic_ios::clear: iostream error

using hash_t = uint64_t;

constexpr auto const BASE_HASH = 34432ULL;

template <typename... Args>
constexpr hash_t hash_combine(hash_t h, Args... val) {
  auto wy = [&](auto const& arg) {
    h = wyhash::wyhash(&arg, sizeof(arg), h, wyhash::_wyp);
  };
  ((wy(val)), ...);
  return h;
}

inline hash_t hash(std::string_view s, hash_t h = BASE_HASH) {
  return wyhash::wyhash(s.data(), s.size(), h, wyhash::_wyp);
}

template <size_t N>
constexpr hash_t hash(const char (&str)[N], hash_t const h = BASE_HASH) {
  return wyhash::wyhash(str, N - 1, h, wyhash::_wyp);
}

template <typename T>
constexpr uint64_t hash(T const& buf, hash_t const h = BASE_HASH) {
  return wyhash::wyhash(&buf[0], buf.size(), h, wyhash::_wyp);
}

#elif defined(CISTA_WYHASH_FASTEST)


using hash_t = uint64_t;

constexpr auto const BASE_HASH = 123ULL;

template <typename... Args>
constexpr hash_t hash_combine(hash_t h, Args... val) {
  auto fh = [&](auto const& arg) {
    h = wyhash::FastestHash(&arg, sizeof(arg), h);
  };
  ((fh(val)), ...);
  return h;
}

inline hash_t hash(std::string_view s, hash_t h = BASE_HASH) {
  return wyhash::FastestHash(s.data(), s.size(), h);
}

template <size_t N>
constexpr hash_t hash(const char (&str)[N], hash_t const h = BASE_HASH) {
  return wyhash::FastestHash(str, N - 1, h);
}

template <typename T>
constexpr uint64_t hash(T const& buf, hash_t const h = BASE_HASH) {
  return wyhash::FastestHash(&buf[0], buf.size(), h);
}

#else  // defined(CISTA_FNV1A)

// Algorithm: 64bit FNV-1a
// Source: http://www.isthe.com/chongo/tech/comp/fnv/

using hash_t = std::uint64_t;

constexpr auto const BASE_HASH = 14695981039346656037ULL;

template <typename... Args>
constexpr hash_t hash_combine(hash_t h, Args... val) {
  constexpr hash_t fnv_prime = 1099511628211ULL;
  auto fnv = [&](auto arg) { h = (h ^ static_cast<hash_t>(arg)) * fnv_prime; };
  ((fnv(val)), ...);
  return h;
}

inline hash_t hash(std::string_view s, hash_t h = BASE_HASH) {
  auto const ptr = reinterpret_cast<uint8_t const*>(s.data());
  for (auto i = size_t{0ULL}; i < s.size(); ++i) {
    h = hash_combine(h, ptr[i]);
  }
  return h;
}

template <size_t N>
constexpr hash_t hash(const char (&str)[N], hash_t const h = BASE_HASH) {
  return hash(std::string_view{str, N - 1}, h);
}

template <typename T>
constexpr uint64_t hash(T const& buf, hash_t const h = BASE_HASH) {
  return hash(
      std::string_view{reinterpret_cast<char const*>(&buf[0]), buf.size()}, h);
}

#endif

}  // namespace cista

#include <cinttypes>
#include <limits>

#define PRI_O PRIdPTR

namespace cista {

using offset_t = intptr_t;

constexpr auto const NULLPTR_OFFSET = std::numeric_limits<offset_t>::min();
constexpr auto const DANGLING = std::numeric_limits<offset_t>::min() + 1;

}  // namespace cista

#include <cinttypes>


#include <functional>
#include <type_traits>

namespace cista {

namespace detail {

template <typename T>
struct decay {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
struct decay<std::reference_wrapper<T>> {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

}  // namespace detail

template <typename T>
using decay_t = typename detail::decay<std::remove_reference_t<T>>::type;

}  // namespace cista

namespace cista {

template <typename T>
static inline constexpr size_t serialized_size(void* param = nullptr) {
  (void)param;
  return sizeof(decay_t<T>);
}

}  // namespace cista

#ifdef _MSC_VER
namespace cista {
inline HANDLE open_file(char const* path, char const* mode) {
  bool read = std::strcmp(mode, "r") == 0;
  bool write = std::strcmp(mode, "w+") == 0;

  verify(read || write, "open file mode not supported");

  DWORD access = read ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
  DWORD create_mode = read ? OPEN_EXISTING : CREATE_ALWAYS;

  return CreateFileA(path, access, 0, nullptr, create_mode,
                     FILE_ATTRIBUTE_NORMAL, nullptr);
}

struct file {
  file() = default;

  file(char const* path, char const* mode)
      : f_(open_file(path, mode)), size_{size()} {
    verify(f_ != INVALID_HANDLE_VALUE, "unable to open file");
  }

  ~file() {
    if (f_ != nullptr) {
      CloseHandle(f_);
    }
    f_ = nullptr;
  }

  file(file const&) = delete;
  file& operator=(file const&) = delete;

  file(file&& o) : f_{o.f_}, size_{o.size_} {
    o.f_ = nullptr;
    o.size_ = 0U;
  }

  file& operator=(file&& o) {
    f_ = o.f_;
    size_ = o.size_;
    o.f_ = nullptr;
    o.size_ = 0U;
    return *this;
  }

  size_t size() const {
    if (f_ == nullptr) {
      return 0U;
    } else {
      LARGE_INTEGER filesize;
      verify(GetFileSizeEx(f_, &filesize), "file size error");
      return static_cast<size_t>(filesize.QuadPart);
    }
  }

  buffer content() const {
    constexpr auto block_size = 8192u;
    size_t const file_size = size();

    auto b = buffer(file_size);

    chunk(block_size, size(), [&](size_t const from, unsigned block_size) {
      OVERLAPPED overlapped = {0};
      overlapped.Offset = static_cast<DWORD>(from);
#ifdef _WIN64
      overlapped.OffsetHigh = static_cast<DWORD>(from >> 32u);
#endif
      ReadFile(f_, b.data() + from, static_cast<DWORD>(block_size), nullptr,
               &overlapped);
    });

    return b;
  }

  uint64_t checksum(offset_t const start = 0) const {
    constexpr auto const block_size = 512 * 1024;  // 512kB
    auto c = BASE_HASH;
    char buf[block_size];
    chunk(block_size, size_ - static_cast<size_t>(start),
          [&](auto const from, auto const size) {
            OVERLAPPED overlapped = {0};
            overlapped.Offset = static_cast<DWORD>(start + from);
#ifdef _WIN64
            overlapped.OffsetHigh = static_cast<DWORD>((start + from) >> 32U);
#endif
            DWORD bytes_read = {0};
            verify(ReadFile(f_, buf, static_cast<DWORD>(size), &bytes_read,
                            &overlapped),
                   "checksum read error");
            verify(bytes_read == size, "checksum read error bytes read");
            c = hash(std::string_view{buf, size}, c);
          });
    return c;
  }

  template <typename T>
  void write(std::size_t const pos, T const& val) {
    OVERLAPPED overlapped = {0};
    overlapped.Offset = static_cast<DWORD>(pos);
#ifdef _WIN64
    overlapped.OffsetHigh = pos >> 32u;
#endif
    DWORD bytes_written = {0};
    verify(WriteFile(f_, &val, sizeof(T), &bytes_written, &overlapped),
           "write(pos, val) write error");
    verify(bytes_written == sizeof(T),
           "write(pos, val) write error bytes written");
  }

  offset_t write(void const* ptr, std::size_t const size,
                 std::size_t alignment) {
    auto curr_offset = size_;
    if (alignment != 0 && alignment != 1) {
      auto unaligned_ptr = reinterpret_cast<void*>(size_);
      auto space = std::numeric_limits<std::size_t>::max();
      auto const aligned_ptr =
          std::align(alignment, size, unaligned_ptr, space);
      curr_offset = aligned_ptr ? reinterpret_cast<std::uintptr_t>(aligned_ptr)
                                : curr_offset;
    }

    unsigned char const buf[16] = {0};
    auto const num_padding_bytes = static_cast<DWORD>(curr_offset - size_);
    if (num_padding_bytes != 0U) {
      verify(num_padding_bytes < 16, "invalid padding size");
      OVERLAPPED overlapped = {0};
      overlapped.Offset = static_cast<uint32_t>(size_);
#ifdef _WIN64
      overlapped.OffsetHigh = static_cast<uint32_t>(size_ >> 32u);
#endif
      DWORD bytes_written = {0};
      verify(WriteFile(f_, buf, num_padding_bytes, &bytes_written, &overlapped),
             "write padding error");
      verify(bytes_written == num_padding_bytes,
             "write padding error bytes written");
      size_ = curr_offset;
    }

    constexpr auto block_size = 8192u;
    chunk(block_size, size, [&](size_t const from, unsigned block_size) {
      OVERLAPPED overlapped = {0};
      overlapped.Offset = 0xFFFFFFFF;
      overlapped.OffsetHigh = 0xFFFFFFFF;
      DWORD bytes_written = {0};
      verify(WriteFile(f_, reinterpret_cast<unsigned char const*>(ptr) + from,
                       block_size, &bytes_written, &overlapped),
             "write error");
      verify(bytes_written == block_size, "write error bytes written");
    });

    auto const offset = size_;
    size_ += size;

    return offset;
  }

  HANDLE f_{nullptr};
  size_t size_{0U};
};
}  // namespace cista
#else

#include <cstdio>

#include <sys/stat.h>

namespace cista {

struct file {
  file() = default;

  file(char const* path, char const* mode)
      : f_{std::fopen(path, mode)}, size_{size()} {
    verify(f_ != nullptr, "unable to open file");
  }

  ~file() {
    if (f_ != nullptr) {
      std::fclose(f_);
    }
    f_ = nullptr;
  }

  file(file const&) = delete;
  file& operator=(file const&) = delete;

  file(file&& o) : f_{o.f_}, size_{o.size_} {
    o.f_ = nullptr;
    o.size_ = 0U;
  }

  file& operator=(file&& o) {
    f_ = o.f_;
    size_ = o.size_;
    o.f_ = nullptr;
    o.size_ = 0U;
    return *this;
  }

  int fd() const {
    auto const fd = fileno(f_);
    verify(fd != -1, "invalid fd");
    return fd;
  }

  size_t size() const {
    if (f_ == nullptr) {
      return 0U;
    } else {
      struct stat s;
      verify(fstat(fd(), &s) != -1, "fstat error");
      return static_cast<size_t>(s.st_size);
    }
  }

  buffer content() {
    auto b = buffer(size());
    verify(std::fread(b.data(), 1, b.size(), f_) == b.size(), "read error");
    return b;
  }

  uint64_t checksum(offset_t const start = 0) const {
    constexpr auto const block_size = static_cast<size_t>(512 * 1024);  // 512kB
    verify(size_ >= static_cast<size_t>(start), "invalid checksum offset");
    verify(!std::fseek(f_, static_cast<long>(start), SEEK_SET), "fseek error");
    auto c = BASE_HASH;
    char buf[block_size];
    chunk(block_size, size_ - static_cast<size_t>(start),
          [&](auto const, auto const s) {
            verify(std::fread(buf, 1, s, f_) == s, "invalid read");
            c = hash(std::string_view{buf, s}, c);
          });
    return c;
  }

  template <typename T>
  void write(std::size_t const pos, T const& val) {
    verify(!std::fseek(f_, static_cast<long>(pos), SEEK_SET), "seek error");
    verify(std::fwrite(reinterpret_cast<unsigned char const*>(&val), 1,
                       serialized_size<T>(), f_) == serialized_size<T>(),
           "write error");
  }

  offset_t write(void const* ptr, std::size_t const size,
                 std::size_t alignment) {
    auto curr_offset = size_;
    if (alignment != 0 && alignment != 1) {
      auto unaligned_ptr = reinterpret_cast<void*>(size_);
      auto space = std::numeric_limits<std::size_t>::max();
      auto const aligned_ptr =
          std::align(alignment, size, unaligned_ptr, space);
      curr_offset = aligned_ptr ? reinterpret_cast<std::uintptr_t>(aligned_ptr)
                                : curr_offset;
      verify(!std::fseek(f_, static_cast<long>(curr_offset), SEEK_SET),
             "seek error");
    } else {
      verify(!std::fseek(f_, 0, SEEK_END), "seek error");
    }
    verify(std::fwrite(ptr, 1, size, f_) == size, "write error");
    size_ = curr_offset + size;
    return static_cast<offset_t>(curr_offset);
  }

  FILE* f_{nullptr};
  std::size_t size_ = 0u;
};

}  // namespace cista

#endif

namespace cista {

struct mmap {
  static constexpr auto const OFFSET = 0ULL;
  static constexpr auto const ENTIRE_FILE = std::numeric_limits<size_t>::max();
  enum class protection { READ, WRITE };

  mmap() = default;

  explicit mmap(char const* path, protection const prot = protection::WRITE)
      : f_{path, prot == protection::READ ? "r" : "w+"},
        prot_{prot},
        size_{f_.size()},
        used_size_{f_.size()},
        addr_{size_ == 0U ? nullptr : map()} {}

  ~mmap() {
    if (addr_ != nullptr) {
      sync();
      size_ = used_size_;
      unmap();
      if (size_ != f_.size()) {
        resize_file();
      }
    }
  }

  mmap(mmap const&) = delete;
  mmap& operator=(mmap const&) = delete;

  mmap(mmap&& o)
      : f_{std::move(o.f_)},
        prot_{o.prot_},
        size_{o.size_},
        used_size_{o.used_size_},
        addr_{o.addr_} {
    o.addr_ = nullptr;
  }

  mmap& operator=(mmap&& o) {
    f_ = std::move(o.f_);
    prot_ = o.prot_;
    size_ = o.size_;
    used_size_ = o.used_size_;
    addr_ = o.addr_;
    o.addr_ = nullptr;
    return *this;
  }

  void sync() {
    if (prot_ == protection::WRITE && addr_ != nullptr) {
#ifdef _MSC_VER
      verify(::FlushViewOfFile(addr_, size_) != 0, "flush error");
      verify(::FlushFileBuffers(f_.f_) != 0, "flush error");
#else
      verify(::msync(addr_, size_, MS_SYNC) == 0, "sync error");
#endif
    }
  }

  void resize(size_t const new_size) {
    verify(prot_ == protection::WRITE, "read-only not resizable");
    if (size_ < new_size) {
      resize_map(next_power_of_two(new_size));
    }
    used_size_ = new_size;
  }

  void reserve(size_t const new_size) {
    verify(prot_ == protection::WRITE, "read-only not resizable");
    if (size_ < new_size) {
      resize_map(next_power_of_two(new_size));
    }
  }

  size_t size() const { return used_size_; }

  inline uint8_t* data() { return static_cast<unsigned char*>(addr_); }
  inline uint8_t const* data() const {
    return static_cast<unsigned char const*>(addr_);
  }

  inline uint8_t* begin() { return data(); }
  inline uint8_t* end() { return data() + used_size_; }
  inline uint8_t const* begin() const { return data(); }
  inline uint8_t const* end() const { return data() + used_size_; }

  unsigned char& operator[](size_t i) { return *(data() + i); }
  unsigned char const& operator[](size_t i) const { return *(data() + i); }

private:
  void unmap() {
#ifdef _MSC_VER
    if (addr_ != nullptr) {
      verify(::UnmapViewOfFile(addr_), "unmap error");
      addr_ = nullptr;

      verify(::CloseHandle(file_mapping_), "close file mapping error");
      file_mapping_ = nullptr;
    }
#else
    if (addr_ != nullptr) {
      ::munmap(addr_, size_);
      addr_ = nullptr;
    }
#endif
  }

  void* map() {
#ifdef _MSC_VER
    auto const size_low = static_cast<DWORD>(size_);
#ifdef _WIN64
    auto const size_high = static_cast<DWORD>(size_ >> 32);
#else
    auto const size_high = static_cast<DWORD>(0);
#endif
    const auto fm = ::CreateFileMapping(
        f_.f_, 0, prot_ == protection::READ ? PAGE_READONLY : PAGE_READWRITE,
        size_high, size_low, 0);
    verify(fm != NULL, "file mapping error");
    file_mapping_ = fm;

    auto const addr = ::MapViewOfFile(
        fm, prot_ == protection::READ ? FILE_MAP_READ : FILE_MAP_WRITE, OFFSET,
        OFFSET, size_);
    verify(addr != nullptr, "map error");

    return addr;
#else
    auto const addr = ::mmap(nullptr, size_,
                             prot_ == protection::READ ? PROT_READ : PROT_WRITE,
                             MAP_SHARED, f_.fd(), OFFSET);
    verify(addr != MAP_FAILED, "map error");
    return addr;
#endif
  }

  void resize_file() {
    if (prot_ == protection::READ) {
      return;
    }

#ifdef _MSC_VER
    LARGE_INTEGER Size = {0};
    verify(::GetFileSizeEx(f_.f_, &Size), "resize: get file size error");

    LARGE_INTEGER Distance = {0};
    Distance.QuadPart = size_ - Size.QuadPart;
    verify(::SetFilePointerEx(f_.f_, Distance, nullptr, FILE_END),
           "resize error");
    verify(::SetEndOfFile(f_.f_), "resize set eof error");
#else
    verify(::ftruncate(f_.fd(), static_cast<off_t>(size_)) == 0,
           "resize error");
#endif
  }

  void resize_map(size_t const new_size) {
    if (prot_ == protection::READ) {
      return;
    }

    unmap();
    size_ = new_size;
    resize_file();
    addr_ = map();
  }

  file f_;
  protection prot_;
  size_t size_;
  size_t used_size_;
  void* addr_;
#ifdef _MSC_VER
  HANDLE file_mapping_;
#endif
};

}  // namespace cista

#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <vector>



#include <cinttypes>
#include <iterator>
#include <stdexcept>

namespace cista {

template <typename T, std::size_t Size>
struct array {
  constexpr size_t size() const { return Size; }

  constexpr T const& operator[](size_t index) const { return el_[index]; }
  constexpr T& operator[](size_t index) { return el_[index]; }
  constexpr T& at(size_t index) {
    if (index >= Size) {
      throw std::out_of_range{"array index out of range"};
    }
    return el_[index];
  }
  constexpr T const& at(size_t index) const {
    return const_cast<array*>(this)->at(index);
  }

  constexpr T* begin() { return el_; }
  constexpr T const* begin() const { return el_; }
  constexpr T* end() { return el_ + Size; }
  constexpr T const* end() const { return el_ + Size; }

  constexpr std::reverse_iterator<T const*> rbegin() const {
    return std::reverse_iterator<T*>(el_ + size());  // NOLINT
  }
  constexpr std::reverse_iterator<T const*> rend() const {
    return std::reverse_iterator<T*>(el_);
  }
  constexpr std::reverse_iterator<T*> rbegin() {
    return std::reverse_iterator<T*>(el_ + size());  // NOLINT
  }
  constexpr std::reverse_iterator<T*> rend() {
    return std::reverse_iterator<T*>(el_);
  }

  constexpr friend T const* begin(array const& a) { return a.begin(); }
  constexpr friend T const* end(array const& a) { return a.end(); }

  constexpr friend T* begin(array& a) { return a.begin(); }
  constexpr friend T* end(array& a) { return a.end(); }

  constexpr T const& back() const { return el_[Size - 1]; }
  constexpr T& back() { return el_[Size - 1]; }

  constexpr T& front() { return el_[0]; }
  constexpr T const& front() const { return el_[0]; }

  constexpr T* data() { return el_; }
  constexpr T const* data() const { return el_; }

  T el_[Size];
};

namespace raw {
using cista::array;
}  // namespace raw

namespace offset {
using cista::array;
}  // namespace offset

}  // namespace cista

#include <functional>


#include <cinttypes>
#include <cstring>

#include <functional>
#include <iterator>
#include <stdexcept>
#include <type_traits>


#include <memory>


namespace cista {

template <typename T>
T to_next_multiple(T const n, T const multiple) {
  auto const r = n % multiple;
  return r == 0 ? n : n + multiple - r;
}

}  // namespace cista

#if defined(_MSC_VER)
#define CISTA_ALIGNED_ALLOC(alignment, size) \
  (_aligned_malloc((size), cista::next_power_of_two((alignment))))
#define CISTA_ALIGNED_FREE(ptr) (_aligned_free((ptr)))
#elif defined(_LIBCPP_HAS_C11_FEATURES) || defined(_GLIBCXX_HAVE_ALIGNED_ALLOC)
#include <memory>
#define CISTA_ALIGNED_ALLOC(alignment, size) \
  (std::aligned_alloc(                       \
      cista::next_power_of_two((alignment)), \
      cista::to_next_multiple((size), cista::next_power_of_two((alignment)))))
#define CISTA_ALIGNED_FREE(ptr) std::free((ptr))
#else
#include <cstdlib>
#define CISTA_ALIGNED_ALLOC(alignment, size) (std::malloc((size)))
#define CISTA_ALIGNED_FREE(ptr) (std::free((ptr)))
#endif
#if defined(_MSC_VER)
#include <intrin.h>
#if defined(_M_X64)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(_BitScanForward64)
#endif
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)
#endif

#include <cstddef>

namespace cista {

template <typename T>
constexpr unsigned trailing_zeros(T t) {
  static_assert(sizeof(T) == 8 || sizeof(T) == 4, "not supported");

  if (t == 0) {
    return sizeof(T) == 8 ? 64 : 32;
  }

  if constexpr (sizeof(T) == 8) {  // 64bit
#if defined(_MSC_VER) && defined(_M_X64)
    unsigned long index = 0;
    _BitScanForward64(&index, t);
    return index;
#elif defined(_MSC_VER)
    unsigned long index = 0;
    if (static_cast<uint32_t>(t) == 0) {
      _BitScanForward(&index, t >> 32);
      return index + 32;
    }
    _BitScanForward(&index, static_cast<uint32_t>(t));
    return index;
#else
    return static_cast<unsigned>(__builtin_ctzll(t));
#endif
  } else if constexpr (sizeof(T) == 4) {  // 32bit
#if defined(_MSC_VER)
    unsigned long index = 0;
    _BitScanForward(&index, t);
    return index;
#else
    return static_cast<unsigned>(__builtin_ctz(t));
#endif
  }
}

template <typename T>
constexpr unsigned leading_zeros(T t) {
  static_assert(sizeof(T) == 8 || sizeof(T) == 4, "not supported");

  if (t == 0) {
    return sizeof(T) == 8 ? 64 : 32;
  }

  if constexpr (sizeof(T) == 8) {  // 64bit
#if defined(_MSC_VER) && defined(_M_X64)
    unsigned long index = 0;
    if (_BitScanReverse64(&index, t)) {
      return 63 - index;
    }
    return 64;
#elif defined(_MSC_VER)
    unsigned long index = 0;
    if ((t >> 32) && _BitScanReverse(&index, t >> 32)) {
      return 31 - index;
    }
    if (_BitScanReverse(&index, static_cast<uint32_t>(t))) {
      return 63 - index;
    }
    return 64;
#else
    return static_cast<unsigned>(__builtin_clzll(t));
#endif
  } else if constexpr (sizeof(T) == 4) {  // 32bit
#if defined(_MSC_VER)
    unsigned long index = 0;
    if (_BitScanReverse(&index, t)) {
      return 31 - index;
    }
    return 32;
#else
    return static_cast<unsigned>(__builtin_clz(t));
#endif
  }
}

}  // namespace cista


#include <type_traits>


namespace cista {

template <typename T, typename Enable = void>
struct offset_ptr {
  offset_ptr() = default;
  offset_ptr(std::nullptr_t) : offset_{NULLPTR_OFFSET} {}
  offset_ptr(T const* p) : offset_{ptr_to_offset(p)} {}

  offset_ptr& operator=(T const* p) {
    offset_ = ptr_to_offset(p);
    return *this;
  }
  offset_ptr& operator=(std::nullptr_t) {
    offset_ = NULLPTR_OFFSET;
    return *this;
  }

  offset_ptr(offset_ptr const& o) : offset_{ptr_to_offset(o.get())} {}
  offset_ptr(offset_ptr&& o) : offset_{ptr_to_offset(o.get())} {}
  offset_ptr& operator=(offset_ptr const& o) {
    offset_ = ptr_to_offset(o.get());
    return *this;
  }
  offset_ptr& operator=(offset_ptr&& o) {
    offset_ = ptr_to_offset(o.get());
    return *this;
  }

  ~offset_ptr() = default;

  offset_t ptr_to_offset(T const* p) const {
    return p == nullptr
               ? NULLPTR_OFFSET
               : static_cast<offset_t>(reinterpret_cast<intptr_t>(p) -
                                       reinterpret_cast<intptr_t>(this));
  }

  explicit operator bool() const { return offset_ != NULLPTR_OFFSET; }
  explicit operator void*() const { return get(); }
  explicit operator void const*() const { return get(); }
  operator T*() const { return get(); }
  T& operator*() const { return *get(); }
  T* operator->() const { return get(); }
  T& operator[](size_t const i) const { return *(get() + i); }

  T* get() const {
    auto const ptr =
        offset_ == NULLPTR_OFFSET
            ? nullptr
            : reinterpret_cast<T*>(reinterpret_cast<intptr_t>(this) + offset_);
    return ptr;
  }

  template <typename Int>
  T* operator+(Int i) const {
    return get() + i;
  }

  template <typename Int>
  T* operator-(Int i) const {
    return get() - i;
  }

  offset_ptr& operator++() {
    offset_ = ptr_to_offset(get() + 1);
    return *this;
  }

  offset_ptr& operator--() {
    offset_ = ptr_to_offset(get() - 1);
    return *this;
  }

  offset_ptr operator++(int) const { return offset_ptr{get() + 1}; }
  offset_ptr operator--(int) const { return offset_ptr{get() - 1}; }

  offset_t offset_{NULLPTR_OFFSET};
};

template <typename T>
struct offset_ptr<T, std::enable_if_t<std::is_same_v<void, T>>> {
  offset_ptr() = default;
  offset_ptr(std::nullptr_t) : offset_{NULLPTR_OFFSET} {}
  offset_ptr(T const* p) : offset_{ptr_to_offset(p)} {}

  offset_ptr& operator=(T const* p) {
    offset_ = ptr_to_offset(p);
    return *this;
  }
  offset_ptr& operator=(std::nullptr_t) {
    offset_ = NULLPTR_OFFSET;
    return *this;
  }

  offset_ptr(offset_ptr const& o) : offset_{ptr_to_offset(o.get())} {}
  offset_ptr(offset_ptr&& o) : offset_{ptr_to_offset(o.get())} {}
  offset_ptr& operator=(offset_ptr const& o) {
    offset_ = ptr_to_offset(o.get());
    return *this;
  }
  offset_ptr& operator=(offset_ptr&& o) {
    offset_ = ptr_to_offset(o.get());
    return *this;
  }

  offset_t ptr_to_offset(T const* p) const {
    return p == nullptr
               ? NULLPTR_OFFSET
               : static_cast<offset_t>(reinterpret_cast<intptr_t>(p) -
                                       reinterpret_cast<intptr_t>(this));
  }

  operator bool() const { return offset_ != NULLPTR_OFFSET; }
  explicit operator void*() const { return get(); }
  explicit operator void const*() const { return get(); }
  T* get() const {
    auto const ptr =
        offset_ == NULLPTR_OFFSET
            ? nullptr
            : reinterpret_cast<T*>(reinterpret_cast<intptr_t>(this) + offset_);
    return ptr;
  }

  friend bool operator==(std::nullptr_t, offset_ptr const& o) {
    return o.offset_ == NULLPTR_OFFSET;
  }
  friend bool operator==(offset_ptr const& o, std::nullptr_t) {
    return o.offset_ == NULLPTR_OFFSET;
  }
  friend bool operator!=(std::nullptr_t, offset_ptr const& o) {
    return o.offset_ != NULLPTR_OFFSET;
  }
  friend bool operator!=(offset_ptr const& o, std::nullptr_t) {
    return o.offset_ != NULLPTR_OFFSET;
  }

  offset_t offset_{NULLPTR_OFFSET};
};

template <class T>
struct is_pointer_helper : std::false_type {};

template <class T>
struct is_pointer_helper<T*> : std::true_type {};

template <class T>
struct is_pointer_helper<offset_ptr<T>> : std::true_type {};

template <class T>
constexpr bool is_pointer_v = is_pointer_helper<std::remove_cv_t<T>>::value;

template <class T>
struct remove_pointer_helper {
  typedef T type;
};

template <class T>
struct remove_pointer_helper<T*> {
  typedef T type;
};

template <class T>
struct remove_pointer_helper<offset_ptr<T>> {
  typedef T type;
};

template <class T>
struct remove_pointer : remove_pointer_helper<std::remove_cv_t<T>> {};

template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

}  // namespace cista

namespace cista {

namespace raw {

template <typename T>
using ptr = T*;

}  // namespace raw

namespace offset {

template <typename T>
using ptr = cista::offset_ptr<T>;

}  // namespace offset

template <typename T>
T* ptr_cast(raw::ptr<T> const p) {
  return p;
}

template <typename T>
T* ptr_cast(offset::ptr<T> const p) {
  return p.get();
}

}  // namespace cista


// Based on:
// https://stackoverflow.com/a/32210953 (MSVC)
// https://stackoverflow.com/a/27054190 (GCC/Clang)

#if !defined(CISTA_BIG_ENDIAN) && !defined(CISTA_LITTLE_ENDIAN)

#if defined(__APPLE__)
#include <machine/endian.h>
#elif defined(__GNUC__)
#include <endian.h>
#elif defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#if defined(REG_DWORD) && REG_DWORD == REG_DWORD_BIG_ENDIAN ||               \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN ||                 \
    defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) ||       \
    defined(__MIBSEB__)
#define CISTA_BIG_ENDIAN
#elif defined(REG_DWORD) && REG_DWORD == REG_DWORD_LITTLE_ENDIAN ||       \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN ||           \
    defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) ||                   \
    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || \
    defined(__MIPSEL) || defined(__MIPSEL__)
#define CISTA_LITTLE_ENDIAN
#else
#error "architecture: unknown byte order"
#endif

#endif

#include <type_traits>

namespace cista {

enum class mode {
  NONE = 0U,
  UNCHECKED = 1U << 0U,
  WITH_VERSION = 1U << 1U,
  WITH_INTEGRITY = 1U << 2U,
  SERIALIZE_BIG_ENDIAN = 1U << 3U,
  DEEP_CHECK = 1U << 4U,
  CAST = 1U << 5U,
  _CONST = 1U << 29U,
  _PHASE_II = 1U << 30U
};

constexpr mode operator|(mode const& a, mode const& b) {
  return mode{static_cast<std::underlying_type_t<mode>>(a) |
              static_cast<std::underlying_type_t<mode>>(b)};
}

constexpr mode operator&(mode const& a, mode const& b) {
  return mode{static_cast<std::underlying_type_t<mode>>(a) &
              static_cast<std::underlying_type_t<mode>>(b)};
}

constexpr bool is_mode_enabled(mode const in, mode const flag) {
  return (in & flag) == flag;
}

constexpr bool is_mode_disabled(mode const in, mode const flag) {
  return (in & flag) == mode::NONE;
}

}  // namespace cista

// Based on
// https://github.com/google/flatbuffers/blob/master/include/flatbuffers/base.h

#if defined(_MSC_VER)
#define CISTA_BYTESWAP_16 _byteswap_ushort
#define CISTA_BYTESWAP_32 _byteswap_ulong
#define CISTA_BYTESWAP_64 _byteswap_uint64
#else
#define CISTA_BYTESWAP_16 __builtin_bswap16
#define CISTA_BYTESWAP_32 __builtin_bswap32
#define CISTA_BYTESWAP_64 __builtin_bswap64
#endif

namespace cista {

template <typename T>
constexpr T endian_swap(T t) {
  static_assert(sizeof(T) == 1U || sizeof(T) == 2U || sizeof(T) == 4U ||
                sizeof(T) == 8U);

  if constexpr (sizeof(T) == 1U) {
    return t;
  } else if constexpr (sizeof(T) == 2U) {
    union {
      T t;
      uint16_t i;
    } u{t};
    u.i = CISTA_BYTESWAP_16(u.i);
    return u.t;
  } else if constexpr (sizeof(T) == 4U) {
    union {
      T t;
      uint32_t i;
    } u{t};
    u.i = CISTA_BYTESWAP_32(u.i);
    return u.t;
  } else if constexpr (sizeof(T) == 8U) {
    union {
      T t;
      uint64_t i;
    } u{t};
    u.i = CISTA_BYTESWAP_64(u.i);
    return u.t;
  }
}

template <mode Mode>
constexpr bool endian_conversion_necessary() {
  if constexpr ((Mode & mode::SERIALIZE_BIG_ENDIAN) ==
                mode::SERIALIZE_BIG_ENDIAN) {
#if defined(CISTA_BIG_ENDIAN)
    return false;
#else
    return true;
#endif
  } else {
#if defined(CISTA_LITTLE_ENDIAN)
    return false;
#else
    return true;
#endif
  }
}

template <mode Mode, typename T>
constexpr T convert_endian(T t) {
  if constexpr (endian_conversion_necessary<Mode>()) {
    return endian_swap(t);
  } else {
    return t;
  }
}

}  // namespace cista

#undef CISTA_BYTESWAP_16
#undef CISTA_BYTESWAP_32
#undef CISTA_BYTESWAP_64

namespace cista {

// This class is a generic hash-based container.
// It can be used e.g. as hash set or hash map.
//   - hash map: `T` = `std::pair<Key, Value>`, GetKey = `return entry.first;`
//   - hash set: `T` = `T`, GetKey = `return entry;` (identity)
//
// It is based on the idea of swiss tables:
// https://abseil.io/blog/20180927-swisstables
//
// Original implementation:
// https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal/raw_hash_set.h
//
// Missing features of this implemenation compared to the original:
//   - SSE to speedup the lookup in the ctrl data structure
//   - sanitizer support (Sanitizer[Un]PoisonMemoryRegion)
//   - overloads (conveniance as well to reduce copying) in the interface
//   - allocator support
template <typename T, template <typename> typename Ptr, typename GetKey,
          typename GetValue, typename Hash, typename Eq>
struct hash_storage {
  using entry_t = T;
  using difference_type = ptrdiff_t;
  using size_type = hash_t;
  using key_t =
      decay_t<decltype(std::declval<GetKey>().operator()(std::declval<T>()))>;
  using mapped_type =
      decay_t<decltype(std::declval<GetValue>().operator()(std::declval<T>()))>;
  using group_t = uint64_t;
  using h2_t = uint8_t;
  static constexpr size_type const WIDTH = 8U;

  template <typename Key>
  hash_t compute_hash(Key const& k) {
    if constexpr (std::is_same_v<decay_t<Key>, key_t>) {
      return static_cast<size_type>(Hash{}(k));
    } else {
      return static_cast<size_type>(Hash::template create<Key>()(k));
    }
  }

  enum ctrl_t : int8_t {
    EMPTY = -128,  // 10000000
    DELETED = -2,  // 11111110
    END = -1  // 11111111
  };

  struct find_info {
    size_type offset_, probe_length_;
  };

  struct probe_seq {
    probe_seq(size_type hash, size_type mask)
        : mask_{mask}, offset_{hash & mask_} {}
    size_type offset(size_type const i) const { return (offset_ + i) & mask_; }
    void next() {
      index_ += WIDTH;
      offset_ += index_;
      offset_ &= mask_;
    }
    size_type mask_, offset_, index_{0U};
  };

  struct bit_mask {
    static constexpr auto const SHIFT = 3U;

    explicit bit_mask(group_t mask) : mask_{mask} {}

    bit_mask& operator++() {
      mask_ &= (mask_ - 1);
      return *this;
    }

    size_type operator*() const { return trailing_zeros(); }

    explicit operator bool() const { return mask_ != 0U; }

    bit_mask begin() const { return *this; }
    bit_mask end() const { return bit_mask{0}; }

    size_type trailing_zeros() const {
      return ::cista::trailing_zeros(mask_) >> SHIFT;
    }

    size_type leading_zeros() const {
      constexpr int total_significant_bits = 8 << SHIFT;
      constexpr int extra_bits = sizeof(group_t) * 8 - total_significant_bits;
      return ::cista::leading_zeros(mask_ << extra_bits) >> SHIFT;
    }

    friend bool operator!=(bit_mask const& a, bit_mask const& b) {
      return a.mask_ != b.mask_;
    }

    group_t mask_;
  };

  struct group {
    static constexpr auto MSBS = 0x8080808080808080ULL;
    static constexpr auto LSBS = 0x0101010101010101ULL;
    static constexpr auto GAPS = 0x00FEFEFEFEFEFEFEULL;

    explicit group(ctrl_t const* pos) {
      std::memcpy(&ctrl_, pos, WIDTH);
#if defined(CISTA_BIG_ENDIAN)
      ctrl_ = endian_swap(ctrl_);
#endif
    }
    bit_mask match(h2_t const hash) const {
      auto const x = ctrl_ ^ (LSBS * hash);
      return bit_mask{(x - LSBS) & ~x & MSBS};
    }
    bit_mask match_empty() const {
      return bit_mask{(ctrl_ & (~ctrl_ << 6U)) & MSBS};
    }
    bit_mask match_empty_or_deleted() const {
      return bit_mask{(ctrl_ & (~ctrl_ << 7U)) & MSBS};
    }
    size_t count_leading_empty_or_deleted() const {
      return (trailing_zeros(((~ctrl_ & (ctrl_ >> 7U)) | GAPS) + 1U) + 7U) >>
             3U;
    }
    group_t ctrl_;
  };

  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = hash_storage::entry_t;
    using reference = hash_storage::entry_t&;
    using pointer = hash_storage::entry_t*;
    using difference_type = ptrdiff_t;

    iterator() = default;

    reference operator*() const { return *entry_; }
    pointer operator->() const { return &operator*(); }
    iterator& operator++() {
      ++ctrl_;
      ++entry_;
      skip_empty_or_deleted();
      return *this;
    }
    iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    friend bool operator==(const iterator& a, const iterator& b) {
      return a.ctrl_ == b.ctrl_;
    }
    friend bool operator!=(const iterator& a, const iterator& b) {
      return !(a == b);
    }

    iterator(ctrl_t* ctrl) : ctrl_(ctrl) {}
    iterator(ctrl_t* ctrl, T* entry) : ctrl_(ctrl), entry_(entry) {}

    void skip_empty_or_deleted() {
      while (is_empty_or_deleted(*ctrl_)) {
        auto const shift = group{ctrl_}.count_leading_empty_or_deleted();
        ctrl_ += shift;
        entry_ += shift;
      }
    }

    ctrl_t* ctrl_ = nullptr;
    T* entry_ = nullptr;
  };

  struct const_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = hash_storage::entry_t;
    using reference = hash_storage::entry_t const&;
    using pointer = hash_storage::entry_t const*;
    using difference_type = ptrdiff_t;

    const_iterator() = default;
    const_iterator(iterator i) : inner_(std::move(i)) {}

    reference operator*() const { return *inner_; }
    pointer operator->() const { return inner_.operator->(); }

    const_iterator& operator++() {
      ++inner_;
      return *this;
    }
    const_iterator operator++(int) { return inner_++; }

    friend bool operator==(const const_iterator& a, const const_iterator& b) {
      return a.inner_ == b.inner_;
    }
    friend bool operator!=(const const_iterator& a, const const_iterator& b) {
      return !(a == b);
    }

    const_iterator(ctrl_t const* ctrl, T const* entry)
        : inner_(const_cast<ctrl_t*>(ctrl), const_cast<T*>(entry)) {}

    iterator inner_;
  };

  static inline ctrl_t* empty_group() {
    alignas(16) static constexpr ctrl_t empty_group[] = {
        END,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    return const_cast<ctrl_t*>(empty_group);
  }

  static inline bool is_empty(ctrl_t const c) { return c == EMPTY; }
  static inline bool is_full(ctrl_t const c) { return c >= 0; }
  static inline bool is_deleted(ctrl_t const c) { return c == DELETED; }
  static inline bool is_empty_or_deleted(ctrl_t const c) { return c < END; }

  static inline size_t normalize_capacity(size_type const n) {
    return n == 0U ? 1 : ~size_type{} >> leading_zeros(n);
  }

  static inline size_type h1(size_type const hash) {
    return (hash >> 7) ^ 16777619;
  }

  static inline h2_t h2(size_type const hash) { return hash & 0x7F; }

  static inline size_type capacity_to_growth(size_type const capacity) {
    return (capacity == 7) ? 6 : capacity - (capacity / 8);
  }

  hash_storage() = default;

  hash_storage(std::initializer_list<T> init) {
    insert(init.begin(), init.end());
  }

  hash_storage(hash_storage&& other)
      : entries_{other.entries_},
        ctrl_{other.ctrl_},
        size_{other.size_},
        capacity_{other.capacity_},
        growth_left_{other.growth_left_},
        self_allocated_{other.self_allocated_} {
    other.entries_ = nullptr;
    other.ctrl_ = empty_group();
    other.size_ = 0U;
    other.capacity_ = 0U;
    other.growth_left_ = 0U;
    other.self_allocated_ = false;
  }

  hash_storage(hash_storage const& other) {
    resize(other.size() + 1);
    for (const auto& v : other) {
      auto const hash = compute_hash(GetKey()(v));
      auto target = find_first_non_full(hash);
      set_ctrl(target.offset_, h2(hash));
      new (entries_ + target.offset_) T{v};
    }
    size_ = other.size();
    growth_left_ -= other.size();
  }

  hash_storage& operator=(hash_storage&& other) {
    entries_ = other.entries_;
    ctrl_ = other.ctrl_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    growth_left_ = other.growth_left_;
    self_allocated_ = other.self_allocated_;
    other.entries_ = nullptr;
    other.ctrl_ = empty_group();
    other.size_ = 0U;
    other.capacity_ = 0U;
    other.growth_left_ = 0U;
    other.self_allocated_ = false;
    return *this;
  }

  hash_storage& operator=(hash_storage const& other) {
    resize(other.size());
    for (const auto& v : other) {
      auto const hash = compute_hash(GetKey()(v));
      auto target = find_first_non_full(hash);
      set_ctrl(target.offset, H2(hash));
      new (entries_ + target.offset_) T{v};
    }
    size_ = other.size();
    growth_left_ -= other.size();
    return *this;
  }

  ~hash_storage() { clear(); }

  void set_empty_key(key_t const&) {}
  void set_deleted_key(key_t const&) {}

  // --- operator[]
  template <typename Key>
  mapped_type& bracket_operator_impl(Key&& key) {
    auto const res = find_or_prepare_insert(std::forward<Key>(key));
    if (res.second) {
      new (entries_ + res.first) T{static_cast<key_t>(key), mapped_type{}};
    }
    return GetValue{}(entries_[res.first]);
  }

  template <typename Key>
  mapped_type& operator[](Key&& key) {
    return bracket_operator_impl(std::forward<Key>(key));
  }

  mapped_type& operator[](key_t const& key) {
    return bracket_operator_impl(key);
  }

  // --- at()
  template <typename Key>
  mapped_type& at_impl(Key&& key) {
    if (auto it = find(std::forward<Key>(key)); it != end()) {
      return GetValue{}(*it);
    } else {
      throw std::out_of_range{"hash_storage::at() key not found"};
    }
  }

  mapped_type& at(key_t const& key) { return at_impl(key); }

  mapped_type const& at(key_t const& key) const {
    return const_cast<hash_storage*>(this)->at(key);
  }

  template <typename Key>
  mapped_type& at(Key&& key) {
    return at_impl(std::forward<Key>(key));
  }

  template <typename Key>
  mapped_type const& at(Key&& key) const {
    return const_cast<hash_storage*>(this)->at(std::forward<Key>(key));
  }

  // --- find()
  template <typename Key>
  iterator find_impl(Key&& key) {
    auto const hash = compute_hash(key);
    for (auto seq = probe_seq{h1(hash), capacity_}; true; seq.next()) {
      group g{ctrl_ + seq.offset_};
      for (auto const i : g.match(h2(hash))) {
        if (Eq{}(GetKey()(entries_[seq.offset(i)]), key)) {
          return iterator_at(seq.offset(i));
        }
      }
      if (g.match_empty()) {
        return end();
      }
    }
  }

  template <typename Key>
  const_iterator find(Key&& key) const {
    return const_cast<hash_storage*>(this)->find_impl(std::forward<Key>(key));
  }

  template <typename Key>
  iterator find(Key&& key) {
    return find_impl(std::forward<Key>(key));
  }

  const_iterator find(key_t const& key) const {
    return const_cast<hash_storage*>(this)->find_impl(key);
  }

  iterator find(key_t const& key) { return find_impl(key); }

  template <class InputIt>
  void insert(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      emplace(*first);
    }
  }

  // --- erase()
  template <typename Key>
  size_t erase_impl(Key&& key) {
    auto it = find(std::forward<Key>(key));
    if (it == end()) {
      return 0;
    }
    erase(it);
    return 1;
  }

  size_t erase(key_t const& k) { return erase_impl(k); }

  template <typename Key>
  size_t erase(Key&& key) {
    return erase_impl(std::forward<Key>(key));
  }

  void erase(iterator const it) {
    it.entry_->~T();
    erase_meta_only(it);
  }

  std::pair<iterator, bool> insert(T const& entry) { return emplace(entry); }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    auto entry = T{std::forward<Args>(args)...};
    auto res = find_or_prepare_insert(GetKey()(entry));
    if (res.second) {
      new (entries_ + res.first) T{std::move(entry)};
    }
    return {iterator_at(res.first), res.second};
  }

  iterator begin() {
    auto it = iterator_at(0);
    if (ctrl_ != nullptr) {
      it.skip_empty_or_deleted();
    }
    return it;
  }
  iterator end() { return {ctrl_ + capacity_}; }

  const_iterator begin() const {
    return const_cast<hash_storage*>(this)->begin();
  }
  const_iterator end() const { return const_cast<hash_storage*>(this)->end(); }
  const_iterator cbegin() const { return begin(); }
  const_iterator cend() const { return end(); }

  friend iterator begin(hash_storage& h) { return h.begin(); }
  friend const_iterator begin(hash_storage const& h) { return h.begin(); }
  friend const_iterator cbegin(hash_storage const& h) { return h.begin(); }
  friend iterator end(hash_storage& h) { return h.end(); }
  friend const_iterator end(hash_storage const& h) { return h.end(); }
  friend const_iterator cend(hash_storage const& h) { return h.begin(); }

  bool empty() const { return size() == 0U; }
  size_type size() const { return size_; }
  size_type capacity() const { return capacity_; }
  size_type max_size() const { return std::numeric_limits<size_t>::max(); }

  bool is_free(int index) {
    auto const index_before = (index - WIDTH) & capacity_;

    auto const empty_after = group{ctrl_ + index}.match_empty();
    auto const empty_before = group{ctrl_ + index_before}.match_empty();

    return empty_before && empty_after &&
           (empty_after.trailing_zeros() + empty_before.leading_zeros()) <
               WIDTH;
  }

  inline bool was_never_full(size_t const index) {
    auto const index_before = (index - WIDTH) & capacity_;

    auto const empty_after = group{ctrl_ + index}.match_empty();
    auto const empty_before = group{ctrl_ + index_before}.match_empty();

    return empty_before && empty_after &&
           (empty_after.trailing_zeros() + empty_before.leading_zeros()) <
               WIDTH;
  }

  void erase_meta_only(const_iterator it) {
    --size_;
    auto const index = static_cast<size_t>(it.inner_.ctrl_ - ctrl_);
    auto const wnf = was_never_full(index);
    set_ctrl(index, static_cast<h2_t>(wnf ? EMPTY : DELETED));
    growth_left_ += wnf;
  }

  void clear() {
    if (capacity_ == 0U) {
      return;
    }

    for (auto i = size_t{0U}; i != capacity_; ++i) {
      if (is_full(ctrl_[i])) {
        entries_[i].~T();
      }
    }

    if (self_allocated_) {
      CISTA_ALIGNED_FREE(entries_);
    }
    entries_ = nullptr;
    ctrl_ = empty_group();
    size_ = 0U;
    capacity_ = 0U;
    growth_left_ = 0U;
  }

  template <typename Key>
  std::pair<size_type, bool> find_or_prepare_insert(Key&& key) {
    auto const hash = compute_hash(key);
    for (auto seq = probe_seq{h1(hash), capacity_}; true; seq.next()) {
      group g{ctrl_ + seq.offset_};
      for (auto const i : g.match(h2(hash))) {
        if (Eq{}(GetKey()(entries_[seq.offset(i)]), key)) {
          return {seq.offset(i), false};
        }
      }
      if (g.match_empty()) {
        break;
      }
    }
    return {prepare_insert(hash), true};
  }

  find_info find_first_non_full(size_type const hash) const {
    for (auto seq = probe_seq{h1(hash), capacity_}; true; seq.next()) {
      if (auto const mask = group{ctrl_ + seq.offset_}.match_empty_or_deleted();
          mask) {
        return {seq.offset(*mask), seq.index_};
      }
    }
  }

  size_type prepare_insert(size_type const hash) {
    auto target = find_first_non_full(hash);
    if (growth_left_ == 0U && !is_deleted(ctrl_[target.offset_])) {
      rehash_and_grow_if_necessary();
      target = find_first_non_full(hash);
    }
    ++size_;
    growth_left_ -= (is_empty(ctrl_[target.offset_]) ? 1 : 0);
    set_ctrl(target.offset_, h2(hash));
    return target.offset_;
  }

  void set_ctrl(size_type const i, h2_t const c) {
    ctrl_[i] = static_cast<ctrl_t>(c);
    ctrl_[((i - WIDTH) & capacity_) + 1 + ((WIDTH - 1) & capacity_)] =
        static_cast<ctrl_t>(c);
  }

  void rehash_and_grow_if_necessary() {
    if (capacity_ == 0) {
      resize(1);
    } else {
      resize(capacity_ * 2 + 1);
    }
  }

  void reset_growth_left() {
    growth_left_ = capacity_to_growth(capacity_) - size_;
  }

  void reset_ctrl() {
    std::memset(ctrl_, EMPTY, static_cast<size_t>(capacity_ + WIDTH + 1U));
    ctrl_[capacity_] = END;
  }

  void initialize_entries() {
    self_allocated_ = true;
    auto const size = static_cast<size_type>(capacity_ * sizeof(T) +
                             (capacity_ + 1 + WIDTH) * sizeof(ctrl_t));
    entries_ = reinterpret_cast<T*>(CISTA_ALIGNED_ALLOC(sizeof(T), static_cast<size_t>(size)));
    ctrl_ = reinterpret_cast<ctrl_t*>(
        reinterpret_cast<uint8_t*>(ptr_cast(entries_)) + capacity_ * sizeof(T));
    reset_ctrl();
    reset_growth_left();
  }

  void resize(size_type const new_capacity) {
    auto const old_ctrl = ctrl_;
    auto const old_entries = entries_;
    auto const old_capacity = capacity_;
    auto const old_self_allocated = self_allocated_;

    capacity_ = new_capacity;
    initialize_entries();

    for (auto i = size_type{0U}; i != old_capacity; ++i) {
      if (is_full(old_ctrl[i])) {
        auto const hash = compute_hash(GetKey()(old_entries[i]));
        auto const target = find_first_non_full(hash);
        auto const new_index = target.offset_;
        set_ctrl(new_index, h2(hash));
        new (entries_ + new_index) T{std::move(old_entries[i])};
      }
    }

    if (old_capacity != 0U && old_self_allocated) {
      CISTA_ALIGNED_FREE(old_entries);
    }
  }

  void rehash() { resize(capacity_); }

  iterator iterator_at(size_type const i) { return {ctrl_ + i, entries_ + i}; }
  const_iterator iterator_at(size_type const i) const {
    return {ctrl_ + i, entries_ + i};
  }

  Ptr<T> entries_{nullptr};
  Ptr<ctrl_t> ctrl_{empty_group()};
  size_type size_{0U}, capacity_{0U}, growth_left_{0U};
  bool self_allocated_{false};
};

}  // namespace cista




#include <tuple>


#include <type_traits>


// Credits: Implementation by Anatoliy V. Tomilov (@tomilov),
//          based on gist by Rafal T. Janik (@ChemiaAion)
//
// Resources:
// https://playfulprogramming.blogspot.com/2016/12/serializing-structs-with-c17-structured.html
// https://codereview.stackexchange.com/questions/142804/get-n-th-data-member-of-a-struct
// https://stackoverflow.com/questions/39768517/structured-bindings-width
// https://stackoverflow.com/questions/35463646/arity-of-aggregate-in-logarithmic-time
// https://stackoverflow.com/questions/38393302/returning-variadic-aggregates-struct-and-syntax-for-c17-variadic-template-c

namespace cista {

namespace detail {

struct instance {
  template <typename Type>
  operator Type() const;
};

template <typename Aggregate, typename IndexSequence = std::index_sequence<>,
          typename = void>
struct arity_impl : IndexSequence {};

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

template <typename Aggregate, std::size_t... Indices>
struct arity_impl<Aggregate, std::index_sequence<Indices...>,
                  std::void_t<decltype(Aggregate{
                      (static_cast<void>(Indices), std::declval<instance>())...,
                      std::declval<instance>()})>>
    : arity_impl<Aggregate,
                 std::index_sequence<Indices..., sizeof...(Indices)>> {};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

}  // namespace detail

template <typename T>
constexpr std::size_t arity() {
  return detail::arity_impl<decay_t<T>>().size();
}

}  // namespace cista

namespace cista {

template <typename T>
constexpr inline auto to_tuple_works_v =
    std::is_aggregate_v<T> &&
#if !defined(_MSC_VER) || defined(NDEBUG)
   std::is_standard_layout_v < T>&&
#endif
    !std::is_polymorphic_v<T>;

template <typename T>
inline auto to_tuple(T& t) {
  constexpr auto const a = arity<T>();
  static_assert(a <= 64, "Max. supported members: 64");
  if constexpr (a == 0) {
    return std::tie();
  } else if constexpr (a == 1) {
    auto& [p1] = t;
    return std::tie(p1);
  } else if constexpr (a == 2) {
    auto& [p1, p2] = t;
    return std::tie(p1, p2);
  } else if constexpr (a == 3) {
    auto& [p1, p2, p3] = t;
    return std::tie(p1, p2, p3);
  } else if constexpr (a == 4) {
    auto& [p1, p2, p3, p4] = t;
    return std::tie(p1, p2, p3, p4);
  } else if constexpr (a == 5) {
    auto& [p1, p2, p3, p4, p5] = t;
    return std::tie(p1, p2, p3, p4, p5);
  } else if constexpr (a == 6) {
    auto& [p1, p2, p3, p4, p5, p6] = t;
    return std::tie(p1, p2, p3, p4, p5, p6);
  } else if constexpr (a == 7) {
    auto& [p1, p2, p3, p4, p5, p6, p7] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7);
  } else if constexpr (a == 8) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8);
  } else if constexpr (a == 9) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9);
  } else if constexpr (a == 10) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
  } else if constexpr (a == 11) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
  } else if constexpr (a == 12) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
  } else if constexpr (a == 13) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
  } else if constexpr (a == 14) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
  } else if constexpr (a == 15) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
  } else if constexpr (a == 16) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
  } else if constexpr (a == 17) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
  } else if constexpr (a == 18) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
  } else if constexpr (a == 19) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
  } else if constexpr (a == 20) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
  } else if constexpr (a == 21) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
  } else if constexpr (a == 22) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
  } else if constexpr (a == 23) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
  } else if constexpr (a == 24) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
  } else if constexpr (a == 25) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
  } else if constexpr (a == 26) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
  } else if constexpr (a == 27) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
  } else if constexpr (a == 28) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
  } else if constexpr (a == 29) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
  } else if constexpr (a == 30) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30);
  } else if constexpr (a == 31) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31);
  } else if constexpr (a == 32) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32);
  } else if constexpr (a == 33) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33);
  } else if constexpr (a == 34) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34);
  } else if constexpr (a == 35) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35);
  } else if constexpr (a == 36) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36);
  } else if constexpr (a == 37) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37);
  } else if constexpr (a == 38) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38);
  } else if constexpr (a == 39) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39);
  } else if constexpr (a == 40) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40);
  } else if constexpr (a == 41) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41);
  } else if constexpr (a == 42) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42);
  } else if constexpr (a == 43) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43);
  } else if constexpr (a == 44) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44);
  } else if constexpr (a == 45) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45);
  } else if constexpr (a == 46) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46);
  } else if constexpr (a == 47) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47);
  } else if constexpr (a == 48) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48);
  } else if constexpr (a == 49) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49);
  } else if constexpr (a == 50) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50);
  } else if constexpr (a == 51) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51);
  } else if constexpr (a == 52) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52);
  } else if constexpr (a == 53) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53);
  } else if constexpr (a == 54) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54);
  } else if constexpr (a == 55) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55);
  } else if constexpr (a == 56) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56);
  } else if constexpr (a == 57) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57);
  } else if constexpr (a == 58) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58);
  } else if constexpr (a == 59) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59);
  } else if constexpr (a == 60) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60);
  } else if constexpr (a == 61) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61);
  } else if constexpr (a == 62) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62);
  } else if constexpr (a == 63) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63);
  } else if constexpr (a == 64) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63, p64] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63, p64);
  }
}

template <typename T>
inline auto to_ptr_tuple(T& t) {
  constexpr auto const a = arity<T>();
  static_assert(a <= 64, "Max. supported members: 64");
  if constexpr (a == 0) {
    return std::make_tuple();
  } else if constexpr (a == 1) {
    auto& [p1] = t;
    return std::make_tuple(&p1);
  } else if constexpr (a == 2) {
    auto& [p1, p2] = t;
    return std::make_tuple(&p1, &p2);
  } else if constexpr (a == 3) {
    auto& [p1, p2, p3] = t;
    return std::make_tuple(&p1, &p2, &p3);
  } else if constexpr (a == 4) {
    auto& [p1, p2, p3, p4] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4);
  } else if constexpr (a == 5) {
    auto& [p1, p2, p3, p4, p5] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5);
  } else if constexpr (a == 6) {
    auto& [p1, p2, p3, p4, p5, p6] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6);
  } else if constexpr (a == 7) {
    auto& [p1, p2, p3, p4, p5, p6, p7] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7);
  } else if constexpr (a == 8) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8);
  } else if constexpr (a == 9) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9);
  } else if constexpr (a == 10) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
  } else if constexpr (a == 11) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11);
  } else if constexpr (a == 12) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12);
  } else if constexpr (a == 13) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13);
  } else if constexpr (a == 14) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14);
  } else if constexpr (a == 15) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15);
  } else if constexpr (a == 16) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16);
  } else if constexpr (a == 17) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17);
  } else if constexpr (a == 18) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18);
  } else if constexpr (a == 19) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19);
  } else if constexpr (a == 20) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20);
  } else if constexpr (a == 21) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21);
  } else if constexpr (a == 22) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22);
  } else if constexpr (a == 23) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23);
  } else if constexpr (a == 24) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24);
  } else if constexpr (a == 25) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25);
  } else if constexpr (a == 26) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26);
  } else if constexpr (a == 27) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27);
  } else if constexpr (a == 28) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28);
  } else if constexpr (a == 29) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29);
  } else if constexpr (a == 30) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30);
  } else if constexpr (a == 31) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31);
  } else if constexpr (a == 32) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32);
  } else if constexpr (a == 33) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33);
  } else if constexpr (a == 34) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34);
  } else if constexpr (a == 35) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35);
  } else if constexpr (a == 36) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36);
  } else if constexpr (a == 37) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37);
  } else if constexpr (a == 38) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38);
  } else if constexpr (a == 39) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39);
  } else if constexpr (a == 40) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40);
  } else if constexpr (a == 41) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41);
  } else if constexpr (a == 42) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42);
  } else if constexpr (a == 43) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43);
  } else if constexpr (a == 44) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44);
  } else if constexpr (a == 45) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45);
  } else if constexpr (a == 46) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46);
  } else if constexpr (a == 47) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47);
  } else if constexpr (a == 48) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48);
  } else if constexpr (a == 49) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49);
  } else if constexpr (a == 50) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50);
  } else if constexpr (a == 51) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51);
  } else if constexpr (a == 52) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52);
  } else if constexpr (a == 53) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53);
  } else if constexpr (a == 54) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54);
  } else if constexpr (a == 55) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55);
  } else if constexpr (a == 56) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56);
  } else if constexpr (a == 57) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57);
  } else if constexpr (a == 58) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58);
  } else if constexpr (a == 59) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59);
  } else if constexpr (a == 60) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59, &p60);
  } else if constexpr (a == 61) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59, &p60, &p61);
  } else if constexpr (a == 62) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59, &p60, &p61, &p62);
  } else if constexpr (a == 63) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59, &p60, &p61, &p62, &p63);
  } else if constexpr (a == 64) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29, p30, p31, p32, p33, p34, p35, p36, p37, p38, p39, p40, p41, p42, p43, p44, p45, p46, p47, p48, p49, p50, p51, p52, p53, p54, p55, p56, p57, p58, p59, p60, p61, p62, p63, p64] = t;
    return std::make_tuple(&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12, &p13, &p14, &p15, &p16, &p17, &p18, &p19, &p20, &p21, &p22, &p23, &p24, &p25, &p26, &p27, &p28, &p29, &p30, &p31, &p32, &p33, &p34, &p35, &p36, &p37, &p38, &p39, &p40, &p41, &p42, &p43, &p44, &p45, &p46, &p47, &p48, &p49, &p50, &p51, &p52, &p53, &p54, &p55, &p56, &p57, &p58, &p59, &p60, &p61, &p62, &p63, &p64);
  }
}

}  // namespace cista

#define CISTA_COMPARABLE()                               \
  template <typename T>                                  \
  bool operator==(T&& b) const {                         \
    return cista::to_tuple(*this) == cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator!=(T&& b) const {                         \
    return cista::to_tuple(*this) != cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator<(T&& b) const {                          \
    return cista::to_tuple(*this) < cista::to_tuple(b);  \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator<=(T&& b) const {                         \
    return cista::to_tuple(*this) <= cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator>(T&& b) const {                          \
    return cista::to_tuple(*this) > cista::to_tuple(b);  \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator>=(T&& b) const {                         \
    return cista::to_tuple(*this) >= cista::to_tuple(b); \
  }

namespace cista {

template <typename T1, typename T2>
struct pair {
  CISTA_COMPARABLE()
  using first_type = T1;
  using second_type = T2;
  T1 first;
  T2 second;
};

template <typename T1, typename T2>
pair(T1&&, T2 &&)->pair<decay_t<T1>, decay_t<T2>>;

namespace raw {
using cista::pair;
}  // namespace raw

namespace offset {
using cista::pair;
}  // namespace offset

}  // namespace cista

#include <algorithm>
#include <type_traits>


#include <iterator>
#include <type_traits>


namespace cista {
namespace detail {

using std::begin;
using std::end;

template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(begin(std::declval<T>())),
                                  decltype(end(std::declval<T>()))>>
    : std::true_type {};

template <typename T, typename = void>
struct it_value {
  using type = void;
};

template <typename T>
struct it_value<T, std::enable_if_t<is_iterable<T>::value>> {
  using type = decay_t<decltype(*begin(std::declval<T>()))>;
};

}  // namespace detail

using detail::is_iterable;

template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
using it_value_t = typename detail::it_value<T>::type;

}  // namespace cista

namespace cista {

namespace detail {

template <class F, class Tuple, std::size_t... I>
constexpr bool tuple_equal_impl(F&& is_equal, Tuple&& a, Tuple&& b,
                                std::index_sequence<I...>) {
  return (is_equal(std::get<I>(std::forward<Tuple>(a)),
                   std::get<I>(std::forward<Tuple>(b))) &&
          ...);
}

}  // namespace detail

template <class F, class Tuple>
constexpr decltype(auto) tuple_equal(F&& is_equal, Tuple&& a, Tuple&& b) {
  return detail::tuple_equal_impl(
      std::forward<F>(is_equal), std::forward<Tuple>(a), std::forward<Tuple>(b),
      std::make_index_sequence<
          std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

template <typename A, typename B, typename = void>
struct is_eq_comparable : std::false_type {};

template <typename A, typename B>
struct is_eq_comparable<
    A, B, std::void_t<decltype(std::declval<A>() == std::declval<B>())>>
    : std::true_type {};

template <typename A, typename B>
constexpr bool is_eq_comparable_v = is_eq_comparable<A, B>::value;

template <typename T>
struct equal_to {
  template <typename T1>
  constexpr bool operator()(T const& a, T1 const& b) const {
    using Type = decay_t<T>;
    using Type1 = decay_t<T1>;
    if constexpr (is_iterable_v<Type> && is_iterable_v<Type1>) {
      using std::begin;
      using std::end;
      auto const eq = std::equal(
          begin(a), end(a), begin(b), end(b),
          [](auto&& x, auto&& y) { return equal_to<decltype(x)>{}(x, y); });
      return eq;
    } else if constexpr (to_tuple_works_v<Type> && to_tuple_works_v<Type1>) {
      return tuple_equal(
          [](auto&& x, auto&& y) { return equal_to<decltype(x)>{}(x, y); },
          to_tuple(a), to_tuple(b));
    } else if constexpr (is_eq_comparable_v<Type, Type1>) {
      return a == b;
    } else {
      static_assert(is_iterable_v<Type> || is_eq_comparable_v<Type, Type1> ||
                        to_tuple_works_v<Type>,
                    "Implement custom equality");
    }
  }
};

}  // namespace cista

#include <cstdint>

#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>


#include <cinttypes>
#include <cstring>

#include <ostream>
#include <string>
#include <string_view>


namespace cista {

template <typename Ptr = char const*>
struct generic_string {
  using msize_t = uint32_t;

  static msize_t mstrlen(char const* s) {
    return static_cast<msize_t>(std::strlen(s));
  }

  static constexpr struct owning_t {
  } owning{};
  static constexpr struct non_owning_t {
  } non_owning{};

  generic_string() {
    std::memset(static_cast<void*>(this), 0, sizeof(*this));
    h_.ptr_ = nullptr;
  }
  ~generic_string() { reset(); }

  generic_string(std::string_view s, owning_t) : generic_string() {
    set_owning(s);
  }
  generic_string(std::string_view s, non_owning_t) : generic_string() {
    set_non_owning(s);
  }
  generic_string(std::string const& s, owning_t) : generic_string() {
    set_owning(s);
  }
  generic_string(std::string const& s, non_owning_t) : generic_string() {
    set_non_owning(s);
  }
  generic_string(char const* s, owning_t) : generic_string() {
    set_owning(s, mstrlen(s));
  }
  generic_string(char const* s, non_owning_t) : generic_string() {
    set_non_owning(s);
  }

  char* begin() { return data(); }
  char* end() { return data() + size(); }
  char const* begin() const { return data(); }
  char const* end() const { return data() + size(); }

  bool is_short() const { return s_.is_short_; }

  void reset() {
    if (!h_.is_short_ && h_.ptr_ != nullptr && h_.self_allocated_) {
      std::free(const_cast<char*>(data()));
    }
    std::memset(static_cast<void*>(this), 0, sizeof(*this));
    h_.ptr_ = nullptr;
  }

  void set_owning(std::string const& s) {
    set_owning(s.data(), static_cast<msize_t>(s.size()));
  }

  void set_owning(std::string_view s) {
    set_owning(s.data(), static_cast<msize_t>(s.size()));
  }

  void set_owning(char const* str) { set_owning(str, mstrlen(str)); }

  void set_owning(char const* str, msize_t const len) {
    reset();
    if (str == nullptr || len == 0) {
      return;
    }
    s_.is_short_ = (len <= 15);
    if (s_.is_short_) {
      std::memcpy(s_.s_, str, len);
      for (auto i = len; i < 15; ++i) {
        s_.s_[i] = '\0';
      }
    } else {
      h_.ptr_ = static_cast<char*>(std::malloc(len));
      if (h_.ptr_ == nullptr) {
        throw std::bad_alloc{};
      }
      h_.size_ = len;
      h_.self_allocated_ = true;
      std::memcpy(const_cast<char*>(data()), str, len);
    }
  }

  void set_non_owning(std::string const& v) {
    set_non_owning(v.data(), static_cast<msize_t>(v.size()));
  }

  void set_non_owning(std::string_view v) {
    set_non_owning(v.data(), static_cast<msize_t>(v.size()));
  }

  void set_non_owning(char const* str) { set_non_owning(str, mstrlen(str)); }

  void set_non_owning(char const* str, msize_t const len) {
    reset();
    if (str == nullptr || len == 0) {
      return;
    }

    if (len <= 15) {
      return set_owning(str, len);
    }

    h_.is_short_ = false;
    h_.self_allocated_ = false;
    h_.ptr_ = str;
    h_.size_ = len;
  }

  void move_from(generic_string&& s) {
    std::memcpy(static_cast<void*>(this), &s, sizeof(*this));
    if constexpr (std::is_pointer_v<Ptr>) {
      std::memset(static_cast<void*>(&s), 0, sizeof(*this));
    } else {
      if (!s.is_short()) {
        h_.ptr_ = s.h_.ptr_;
        s.h_.ptr_ = nullptr;
      }
    }
  }

  void copy_from(generic_string const& s) {
    reset();
    if (s.is_short()) {
      std::memcpy(static_cast<void*>(this), &s, sizeof(s));
    } else if (s.h_.self_allocated_) {
      set_owning(s.data(), s.size());
    } else {
      set_non_owning(s.data(), s.size());
    }
  }

  inline bool empty() const { return size() == 0U; }
  std::string_view view() const { return {data(), size()}; }
  std::string str() const { return {data(), size()}; }

  operator std::string_view() const { return view(); }

  char& operator[](size_t i) { return data()[i]; }
  char const& operator[](size_t i) const { return data()[i]; }

  friend std::ostream& operator<<(std::ostream& out, generic_string const& s) {
    return out << s.view();
  }

  friend bool operator==(generic_string const& a, generic_string const& b) {
    return a.view() == b.view();
  }

  friend bool operator!=(generic_string const& a, generic_string const& b) {
    return a.view() != b.view();
  }

  friend bool operator<(generic_string const& a, generic_string const& b) {
    return a.view() < b.view();
  }

  friend bool operator>(generic_string const& a, generic_string const& b) {
    return a.view() > b.view();
  }

  friend bool operator<=(generic_string const& a, generic_string const& b) {
    return a.view() <= b.view();
  }

  friend bool operator>=(generic_string const& a, generic_string const& b) {
    return a.view() >= b.view();
  }

  friend bool operator==(generic_string const& a, std::string_view b) {
    return a.view() == b;
  }

  friend bool operator!=(generic_string const& a, std::string_view b) {
    return a.view() != b;
  }

  friend bool operator<(generic_string const& a, std::string_view b) {
    return a.view() < b;
  }

  friend bool operator>(generic_string const& a, std::string_view b) {
    return a.view() > b;
  }

  friend bool operator<=(generic_string const& a, std::string_view b) {
    return a.view() <= b;
  }

  friend bool operator>=(generic_string const& a, std::string_view b) {
    return a.view() >= b;
  }

  friend bool operator==(std::string_view a, generic_string const& b) {
    return a == b.view();
  }

  friend bool operator!=(std::string_view a, generic_string const& b) {
    return a != b.view();
  }

  friend bool operator<(std::string_view a, generic_string const& b) {
    return a < b.view();
  }

  friend bool operator>(std::string_view a, generic_string const& b) {
    return a > b.view();
  }

  friend bool operator<=(std::string_view a, generic_string const& b) {
    return a <= b.view();
  }

  friend bool operator>=(std::string_view a, generic_string const& b) {
    return a >= b.view();
  }

  friend bool operator==(generic_string const& a, char const* b) {
    return a.view() == std::string_view{b};
  }

  friend bool operator!=(generic_string const& a, char const* b) {
    return a.view() != std::string_view{b};
  }

  friend bool operator<(generic_string const& a, char const* b) {
    return a.view() < std::string_view{b};
  }

  friend bool operator>(generic_string const& a, char const* b) {
    return a.view() > std::string_view{b};
  }

  friend bool operator<=(generic_string const& a, char const* b) {
    return a.view() <= std::string_view{b};
  }

  friend bool operator>=(generic_string const& a, char const* b) {
    return a.view() >= std::string_view{b};
  }

  friend bool operator==(char const* a, generic_string const& b) {
    return std::string_view{a} == b.view();
  }

  friend bool operator!=(char const* a, generic_string const& b) {
    return std::string_view{a} != b.view();
  }

  friend bool operator<(char const* a, generic_string const& b) {
    return std::string_view{a} < b.view();
  }

  friend bool operator>(char const* a, generic_string const& b) {
    return std::string_view{a} > b.view();
  }

  friend bool operator<=(char const* a, generic_string const& b) {
    return std::string_view{a} <= b.view();
  }

  friend bool operator>=(char const* a, generic_string const& b) {
    return std::string_view{a} >= b.view();
  }

  char* data() {
    if constexpr (std::is_pointer_v<Ptr>) {
      return is_short() ? const_cast<char*>(s_.s_) : const_cast<char*>(h_.ptr_);
    } else {
      return is_short() ? const_cast<char*>(s_.s_)
                        : const_cast<char*>(h_.ptr_.get());
    }
  }

  char const* data() const {
    if constexpr (std::is_pointer_v<Ptr>) {
      return is_short() ? s_.s_ : h_.ptr_;
    } else {
      return is_short() ? s_.s_ : h_.ptr_.get();
    }
  }

  msize_t size() const {
    if (is_short()) {
      auto const pos = static_cast<char const*>(std::memchr(s_.s_, '\0', 15));
      if (pos == nullptr) {
        return 15;
      } else {
        return static_cast<msize_t>(pos - s_.s_);
      }
    } else {
      return h_.size_;
    }
  }

  struct heap {
    bool is_short_{false};
    bool self_allocated_{false};
    uint8_t __fill_2__{0};
    uint8_t __fill_3__{0};
    uint32_t size_{0};
    Ptr ptr_{nullptr};
  };

  struct stack {
    bool is_short_{true};
    char s_[15]{0};
  };

  union {
    heap h_;
    stack s_;
  };
};

template <typename Ptr>
struct basic_string : public generic_string<Ptr> {
  using base = generic_string<Ptr>;

  using base::base;
  using base::operator std::string_view;

  friend std::ostream& operator<<(std::ostream& out, basic_string const& s) {
    return out << s.view();
  }

  explicit operator std::string() const { return {base::data(), base::size()}; }

  basic_string(std::string_view s) : base{s, base::owning} {}
  basic_string(std::string const& s) : base{s, base::owning} {}
  basic_string(char const* s) : base{s, base::owning} {}

  basic_string(basic_string const& o) : base{} {
    base::set_owning(o.data(), o.size());
  }
  basic_string(basic_string&& o) : base{} {
    base::set_owning(o.data(), o.size());
  }
  basic_string& operator=(basic_string const& o) {
    base::set_owning(o.data(), o.size());
    return *this;
  }
  basic_string& operator=(basic_string&& o) {
    base::move_from(std::move(o));
    return *this;
  }

  basic_string& operator=(char const* s) {
    base::set_owning(s);
    return *this;
  }
  basic_string& operator=(std::string const& s) {
    base::set_owning(s);
    return *this;
  }
  basic_string& operator=(std::string_view s) {
    base::set_owning(s);
    return *this;
  }
};

template <typename Ptr>
struct basic_string_view : public generic_string<Ptr> {
  using base = generic_string<Ptr>;

  using base::base;
  using base::operator std::string_view;

  friend std::ostream& operator<<(std::ostream& out,
                                  basic_string_view const& s) {
    return out << s.view();
  }

  basic_string_view(std::string_view s) : base{s, base::owning} {}
  basic_string_view(std::string const& s) : base{s, base::owning} {}
  basic_string_view(char const* s) : base{s, base::owning} {}

  basic_string_view(basic_string_view const& o) : base{} {
    base::set_non_owning(o.data(), o.size());
  }
  basic_string_view(basic_string_view&& o) : base{} {
    base::set_non_owning(o.data(), o.size());
  }
  basic_string_view& operator=(basic_string_view const& o) {
    base::set_non_owning(o.data(), o.size());
    return *this;
  }
  basic_string_view& operator=(basic_string_view&& o) {
    base::set_non_owning(o.data(), o.size());
    return *this;
  }

  basic_string_view& operator=(char const* s) {
    base::set_non_owning(s);
    return *this;
  }
  basic_string_view& operator=(std::string_view s) {
    base::set_non_owning(s);
    return *this;
  }
  basic_string_view& operator=(std::string const& s) {
    base::set_non_owning(s);
    return *this;
  }
};

template <typename Ptr>
struct is_string_helper : std::false_type {};

template <typename Ptr>
struct is_string_helper<generic_string<Ptr>> : std::true_type {};

template <typename Ptr>
struct is_string_helper<basic_string<Ptr>> : std::true_type {};

template <typename Ptr>
struct is_string_helper<basic_string_view<Ptr>> : std::true_type {};

template <class T>
constexpr bool is_string_v = is_string_helper<std::remove_cv_t<T>>::value;

namespace raw {
using generic_string = generic_string<ptr<char const>>;
using string = basic_string<ptr<char const>>;
using string_view = basic_string_view<ptr<char const>>;
}  // namespace raw

namespace offset {
using generic_string = generic_string<ptr<char const>>;
using string = basic_string<ptr<char const>>;
using string_view = basic_string_view<ptr<char const>>;
}  // namespace offset

}  // namespace cista

#include <type_traits>
#include <utility>


namespace cista {

template <typename T, typename Fn>
inline void for_each_ptr_field(T& t, Fn&& fn) {
  if constexpr (std::is_pointer_v<T>) {
    if (t != nullptr) {
      for_each_ptr_field(*t, std::forward<Fn>(fn));
    }
  } else if constexpr (std::is_scalar_v<T>) {
    fn(t);
  } else {
    std::apply([&](auto&&... args) { (fn(args), ...); }, to_ptr_tuple(t));
  }
}

template <typename T, typename Fn>
inline void for_each_field(T& t, Fn&& fn) {
  if constexpr (std::is_pointer_v<T>) {
    if (t != nullptr) {
      for_each_field(*t, std::forward<Fn>(fn));
    }
  } else if constexpr (std::is_scalar_v<T>) {
    fn(t);
  } else {
    std::apply([&](auto&&... args) { (fn(args), ...); }, to_tuple(t));
  }
}

template <typename T, typename Fn>
inline void for_each_field(Fn&& fn) {
  T t{};
  for_each_field<T>(t, std::forward<Fn>(fn));
}

}  // namespace cista

namespace cista {

namespace detail {

template <typename T, typename = void>
struct has_hash : std::false_type {};

template <typename T>
struct has_hash<T, std::void_t<decltype(std::declval<T>().hash())>>
    : std::true_type {};

template <typename T, typename = void>
struct has_std_hash : std::false_type {};

template <typename T>
struct has_std_hash<
    T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>>
    : std::true_type {};

}  // namespace detail

template <typename T>
constexpr bool has_hash_v = detail::has_hash<T>::value;

template <typename T>
constexpr bool has_std_hash_v = detail::has_std_hash<T>::value;

template <typename A, typename B>
struct is_hash_equivalent_helper : std::false_type {};

template <typename A, typename B>
constexpr bool is_hash_equivalent_v =
    is_hash_equivalent_helper<std::remove_cv_t<A>, std::remove_cv_t<B>>::value;

template <typename T, typename = void>
struct is_char_array_helper : std::false_type {};

template <std::size_t N>
struct is_char_array_helper<char const[N]> : std::true_type {};

template <std::size_t N>
struct is_char_array_helper<char[N]> : std::true_type {};

template <typename T>
constexpr bool is_char_array_v = is_char_array_helper<T>::value;

template <typename T>
constexpr bool is_string_like_v =
    is_string_v<std::remove_cv_t<T>> || is_char_array_v<T> ||
    std::is_same_v<T, char const*> ||
    std::is_same_v<std::remove_cv_t<T>, std::string> ||
    std::is_same_v<std::remove_cv_t<T>, std::string_view>;

template <typename A, typename B>
constexpr bool is_ptr_same = is_pointer_v<A>&& is_pointer_v<B>;

template <typename T>
struct hashing {
  template <typename A, typename B>
  static constexpr bool is_hash_equivalent() {
    using DecayA = decay_t<A>;
    using DecayB = decay_t<B>;
    return is_hash_equivalent_v<DecayA, DecayB> ||
           std::is_same_v<DecayA, DecayB> ||
           (is_string_like_v<DecayA> && is_string_like_v<DecayB>) ||
           std::is_convertible_v<A, B> || is_ptr_same<DecayA, DecayB>;
  }

  template <typename T1>
  static constexpr hashing<T1> create() {
    static_assert(is_hash_equivalent<T, T1>(), "Incompatible types");
    return hashing<T1>{};
  }

  constexpr hash_t operator()(T const& el, hash_t const seed = BASE_HASH) {
    using Type = decay_t<T>;
    if constexpr (has_hash_v<Type>) {
      return el.hash();
    } else if constexpr (is_pointer_v<Type>) {
      return hash_combine(seed, reinterpret_cast<intptr_t>(ptr_cast(el)));
    } else if constexpr (is_char_array_v<Type>) {
      return hash(std::string_view{el, sizeof(el) - 1}, seed);
    } else if constexpr (is_string_like_v<Type>) {
      using std::begin;
      using std::end;
      return hash(std::string_view{&(*begin(el)), el.size()}, seed);
    } else if constexpr (std::is_scalar_v<Type>) {
      return hash_combine(seed, el);
    } else if constexpr (is_iterable_v<Type>) {
      auto h = seed;
      for (auto const& v : el) {
        h = hashing<decltype(v)>()(v, h);
      }
      return h;
    } else if constexpr (has_std_hash_v<Type>) {
      return std::hash<Type>()(el);
    } else if constexpr (to_tuple_works_v<Type>) {
      auto h = seed;
      for_each_field(el, [&h](auto&& f) { h = hashing<decltype(f)>{}(f, h); });
      return h;
    } else {
      static_assert(has_hash_v<Type> || std::is_scalar_v<Type> ||
                        has_std_hash_v<Type> || is_iterable_v<Type> ||
                        to_tuple_works_v<Type>,
                    "Implement hash");
    }
  }
};  // namespace cista

template <typename T1, typename T2>
struct hashing<std::pair<T1, T2>> {
  constexpr hash_t operator()(std::pair<T1, T2> const& el,
                              hash_t const seed = BASE_HASH) {
    std::size_t h = seed;
    h = hashing<T1>{}(el.first, h);
    h = hashing<T2>{}(el.second, h);
    return h;
  }
};

template <typename... Args>
struct hashing<std::tuple<Args...>> {
  constexpr hash_t operator()(std::tuple<Args...> const& el,
                              hash_t const seed = BASE_HASH) {
    hash_t h = seed;
    std::apply(
        [&h](auto&&... args) {
          ((h = hashing<decltype(args)>{}(args, h)), ...);
        },
        el);
    return h;
  }
};

template <>
struct hashing<char const*> {
  hash_t operator()(char const* el, hash_t const seed = BASE_HASH) {
    return hash(std::string_view{el}, seed);
  }
};

template <typename... Args>
hash_t build_hash(Args... args) {
  hash_t h = BASE_HASH;
  ((h = hashing<decltype(args)>{}(args, h)), ...);
  return h;
}

}  // namespace cista

namespace cista {

struct get_first {
  template <typename T>
  auto&& operator()(T&& t) {
    return t.first;
  }
};

struct get_second {
  template <typename T>
  auto&& operator()(T&& t) {
    return t.second;
  }
};

namespace raw {
template <typename Key, typename Value, typename Hash = hashing<Key>,
          typename Eq = equal_to<Key>>
using hash_map =
    hash_storage<pair<Key, Value>, ptr, get_first, get_second, Hash, Eq>;
}  // namespace raw

namespace offset {
template <typename Key, typename Value, typename Hash = hashing<Key>,
          typename Eq = equal_to<Key>>
using hash_map =
    hash_storage<pair<Key, Value>, ptr, get_first, get_second, Hash, Eq>;
}  // namespace offset

}  // namespace cista

#include <functional>


namespace cista {

struct identity {
  template <typename T>
  decltype(auto) operator()(T&& t) {
    return std::forward<T>(t);
  }
};

namespace raw {
template <typename T, typename Hash = hashing<T>, typename Eq = equal_to<T>>
using hash_set = hash_storage<T, ptr, identity, identity, Hash, Eq>;
}  // namespace raw

namespace offset {
template <typename T, typename Hash = hashing<T>, typename Eq = equal_to<T>>
using hash_set = hash_storage<T, ptr, identity, identity, Hash, Eq>;
}  // namespace offset

}  // namespace cista

#include <cinttypes>
#include <cstddef>

#include <utility>


namespace cista {

template <typename T, typename Ptr = T*>
struct basic_unique_ptr {
  using value_t = T;

  basic_unique_ptr() = default;

  explicit basic_unique_ptr(T* el, bool take_ownership = true)
      : el_{el}, self_allocated_{take_ownership} {}

  basic_unique_ptr(basic_unique_ptr const&) = delete;
  basic_unique_ptr& operator=(basic_unique_ptr const&) = delete;

  basic_unique_ptr(basic_unique_ptr&& o)
      : el_{o.el_}, self_allocated_{o.self_allocated_} {
    o.el_ = nullptr;
    o.self_allocated_ = false;
  }

  basic_unique_ptr& operator=(basic_unique_ptr&& o) {
    el_ = o.el_;
    self_allocated_ = o.self_allocated_;
    o.el_ = nullptr;
    o.self_allocated_ = false;
    return *this;
  }

  basic_unique_ptr(std::nullptr_t) {}
  basic_unique_ptr& operator=(std::nullptr_t) {
    reset();
    return *this;
  }

  ~basic_unique_ptr() { reset(); }

  void reset() {
    if (self_allocated_ && el_ != nullptr) {
      delete el_;
      el_ = nullptr;
      self_allocated_ = false;
    }
  }

  explicit operator bool() const { return el_ != nullptr; }

  friend bool operator==(basic_unique_ptr const& a, std::nullptr_t) {
    return a.el_ == nullptr;
  }
  friend bool operator==(std::nullptr_t, basic_unique_ptr const& a) {
    return a.el_ == nullptr;
  }
  friend bool operator!=(basic_unique_ptr const& a, std::nullptr_t) {
    return a.el_ != nullptr;
  }
  friend bool operator!=(std::nullptr_t, basic_unique_ptr const& a) {
    return a.el_ != nullptr;
  }

  T* get() const { return el_; }
  T* operator->() { return el_; }
  T& operator*() { return *el_; }
  T const& operator*() const { return *el_; }
  T const* operator->() const { return el_; }

  Ptr el_{nullptr};
  bool self_allocated_{false};
  uint8_t __fill_0__{0};
  uint16_t __fill_1__{0};
  uint32_t __fill_2__{0};
};

namespace raw {

template <typename T>
using unique_ptr = basic_unique_ptr<T, ptr<T>>;

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
  return unique_ptr<T>{new T{std::forward<Args>(args)...}, true};
}

}  // namespace raw

namespace offset {

template <typename T>
using unique_ptr = basic_unique_ptr<T, ptr<T>>;

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
  return unique_ptr<T>{new T{std::forward<Args>(args)...}, true};
}

}  // namespace offset

}  // namespace cista

#include <cinttypes>
#include <algorithm>
#include <array>
#include <limits>
#include <type_traits>

// cista/hashing.h": basic_ios::clear: iostream error

namespace cista {

template <std::size_t Size, typename Enable = void>
struct bytes_to_integer_type {};

template <std::size_t Size>
struct bytes_to_integer_type<Size, std::enable_if_t<Size == 1>> {
  using type = std::uint8_t;
};

template <std::size_t Size>
struct bytes_to_integer_type<Size, std::enable_if_t<Size == 2>> {
  using type = std::uint16_t;
};

template <typename... T>
constexpr std::size_t bytes() {
  if (sizeof...(T) > std::numeric_limits<uint8_t>::max()) {
    return 2;
  } else {
    return 1;
  }
}

template <typename... T>
using variant_index_t = typename bytes_to_integer_type<bytes<T...>()>::type;

constexpr auto const TYPE_NOT_FOUND = std::numeric_limits<std::size_t>::max();

template <typename Arg, typename... T>
constexpr std::size_t index_of_type() {
  constexpr std::array<bool, sizeof...(T)> matches = {
      {std::is_same<std::decay_t<Arg>, std::decay_t<T>>::value...}};
  for (std::size_t i = 0; i < sizeof...(T); ++i) {
    if (matches[i]) {
      return i;
    }
  }
  return TYPE_NOT_FOUND;
}

template <int N, typename... T>
struct type_at_index;

template <typename First, typename... Rest>
struct type_at_index<0, First, Rest...> {
  using type = First;
};

template <int N, typename First, typename... Rest>
struct type_at_index<N, First, Rest...> {
  using type = typename type_at_index<N - 1, Rest...>::type;
};

template <std::size_t Index, typename... T>
using type_at_index_t = typename type_at_index<Index, T...>::type;

template <typename... T>
struct variant {
  using index_t = variant_index_t<T...>;
  static constexpr auto NO_VALUE = std::numeric_limits<index_t>::max();

  variant() : idx_{NO_VALUE} {}

  template <typename Arg,
            typename = std::enable_if_t<
                index_of_type<std::decay_t<Arg>, T...>() != TYPE_NOT_FOUND>>
  explicit variant(Arg&& arg)
      : idx_{static_cast<index_t>(index_of_type<Arg, T...>())} {
    new (&storage_) Arg(std::forward<Arg>(arg));
  }

  variant(variant const& o) : idx_{o.idx_} {
    o.apply([this](auto&& el) {
      using Type = std::decay_t<decltype(el)>;
      new (&storage_) Type(std::forward<decltype(el)>(el));
    });
  }
  variant(variant&& o) : idx_{o.idx_} {
    o.apply([this](auto&& el) {
      using Type = std::decay_t<decltype(el)>;
      new (&storage_) Type(std::move(el));
    });
  }

  variant& operator=(variant const& o) {
    return o.apply([&](auto&& el) -> variant& { return *this = el; });
  }
  variant& operator=(variant&& o) {
    return o.apply(
        [&](auto&& el) -> variant& { return *this = std::move(el); });
  }

  template <typename Arg,
            typename = std::enable_if_t<
                index_of_type<std::decay_t<Arg>, T...>() != TYPE_NOT_FOUND>>
  variant& operator=(Arg&& arg) {
    if (index_of_type<Arg, T...>() == idx_) {
      apply([&](auto&& el) {
        if constexpr (std::is_same_v<std::decay_t<decltype(el)>, Arg>) {
          el = std::move(arg);
        }
      });
      return *this;
    } else {
      destruct();
      idx_ = static_cast<index_t>(index_of_type<Arg, T...>());
      new (&storage_) std::decay_t<Arg>{std::forward<Arg>(arg)};
      return *this;
    }
  }

  ~variant() { destruct(); }

  friend bool operator==(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u == v; },
                       a.idx_, a, b)
               : false;
  }

  friend bool operator!=(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u != v; },
                       a.idx_, a, b)
               : true;
  }

  friend bool operator<(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u < v; }, a.idx_,
                       a, b)
               : a.idx_ < b.idx_;
  }

  friend bool operator>(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u > v; }, a.idx_,
                       a, b)
               : a.idx_ > b.idx_;
  }

  friend bool operator<=(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u <= v; },
                       a.idx_, a, b)
               : a.idx_ <= b.idx_;
  }

  friend bool operator>=(variant const& a, variant const& b) {
    return a.idx_ == b.idx_
               ? apply([](auto&& u, auto&& v) -> bool { return u >= v; },
                       a.idx_, a, b)
               : a.idx_ >= b.idx_;
  }

  template <typename Arg, typename... CtorArgs>
  Arg& emplace(CtorArgs&&... ctor_args) {
    static_assert(index_of_type<Arg, T...>() != TYPE_NOT_FOUND);
    destruct();
    idx_ = static_cast<index_t>(index_of_type<Arg, T...>());
    return *(new (&storage_)
                 std::decay_t<Arg>{std::forward<CtorArgs>(ctor_args)...});
  }

  template <size_t I, typename... CtorArgs>
  type_at_index_t<I, T...>& emplace(CtorArgs&&... ctor_args) {
    static_assert(I < sizeof...(T));
    destruct();
    idx_ = I;
    return *(new (&storage_) std::decay_t<type_at_index_t<I, T...>>{
        std::forward<CtorArgs>(ctor_args)...});
  }

  constexpr std::size_t index() const { return idx_; }

  void swap(variant& o) {
    if (idx_ == o.idx_) {
      apply(
          [](auto&& a, auto&& b) {
            using std::swap;
            swap(a, b);
          },
          idx_, *this, o);
    } else {
      variant tmp{std::move(o)};
      o = std::move(*this);
      *this = std::move(tmp);
    }
  }

  void destruct() {
    if (idx_ != NO_VALUE) {
      apply([](auto&& el) {
        using el_type = std::decay_t<decltype(el)>;
        el.~el_type();
      });
    }
  }

  template <typename F>
  auto apply(F&& f) -> decltype(f(std::declval<type_at_index_t<0U, T...>>())) {
    return apply(std::forward<F>(f), idx_, *this);
  }

  template <typename F>
  auto apply(F&& f) const
      -> decltype(f(std::declval<type_at_index_t<0U, T...>>())) {
    return apply(std::forward<F>(f), idx_, *this);
  }

#ifdef _MSC_VER
  static __declspec(noreturn) void noret() {}
#endif

  template <typename F, std::size_t B = 0U, typename... Vs>
  static auto apply(F&& f, index_t const idx, Vs&&... vs)
      -> decltype(f((vs, std::declval<type_at_index_t<0U, T...>>())...)) {
    switch (idx) {
      case B + 0:
        if constexpr (B + 0 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 0, T...>>()...);
        }
        [[fallthrough]];
      case B + 1:
        if constexpr (B + 1 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 1, T...>>()...);
        }
        [[fallthrough]];
      case B + 2:
        if constexpr (B + 2 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 2, T...>>()...);
        }
        [[fallthrough]];
      case B + 3:
        if constexpr (B + 3 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 3, T...>>()...);
        }
        [[fallthrough]];
      case B + 4:
        if constexpr (B + 4 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 4, T...>>()...);
        }
        [[fallthrough]];
      case B + 5:
        if constexpr (B + 5 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 5, T...>>()...);
        }
        [[fallthrough]];
      case B + 6:
        if constexpr (B + 6 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 6, T...>>()...);
        }
        [[fallthrough]];
      case B + 7:
        if constexpr (B + 7 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 7, T...>>()...);
        }
        [[fallthrough]];
      case B + 8:
        if constexpr (B + 8 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 8, T...>>()...);
        }
        [[fallthrough]];
      case B + 9:
        if constexpr (B + 9 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 9, T...>>()...);
        }
        [[fallthrough]];
      case B + 10:
        if constexpr (B + 10 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 10, T...>>()...);
        }
        [[fallthrough]];
      case B + 11:
        if constexpr (B + 11 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 11, T...>>()...);
        }
        [[fallthrough]];
      case B + 12:
        if constexpr (B + 12 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 12, T...>>()...);
        }
        [[fallthrough]];
      case B + 13:
        if constexpr (B + 13 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 13, T...>>()...);
        }
        [[fallthrough]];
      case B + 14:
        if constexpr (B + 14 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 14, T...>>()...);
        }
        [[fallthrough]];
      case B + 15:
        if constexpr (B + 15 < sizeof...(T)) {
          return f(vs.template as<type_at_index_t<B + 15, T...>>()...);
        }
        break;
    }

    if constexpr (B + 15 < sizeof...(T)) {
      return apply<F, B + 16U>(std::forward<F>(f), std::forward<Vs>(vs)...);
    }

#ifndef _MSC_VER
    __builtin_unreachable();
#else
    noret();
#endif
  }

  template <typename AsType>
  AsType& as() {
    return *reinterpret_cast<AsType*>(&storage_);
  }

  template <typename AsType>
  AsType const& as() const {
    return *reinterpret_cast<AsType const*>(&storage_);
  }

  index_t idx_;
  std::aligned_union_t<0, T...> storage_;
};

template <typename T, typename... Ts>
bool holds_alternative(variant<Ts...> const& v) {
  return v.idx_ == index_of_type<std::decay_t<T>, Ts...>();
}

template <std::size_t I, typename... Ts>
constexpr cista::type_at_index_t<I, Ts...> const& get(
    cista::variant<Ts...> const& v) {
  return v.template as<cista::type_at_index_t<I, Ts...>>();
}

template <std::size_t I, typename... Ts>
constexpr cista::type_at_index_t<I, Ts...>& get(cista::variant<Ts...>& v) {
  return v.template as<cista::type_at_index_t<I, Ts...>>();
}

template <class T, class... Ts>
constexpr T const& get(cista::variant<Ts...> const& v) {
  static_assert(cista::index_of_type<T, Ts...>() != cista::TYPE_NOT_FOUND);
  return v.template as<T>();
}
template <class T, class... Ts>
constexpr T& get(cista::variant<Ts...>& v) {
  static_assert(cista::index_of_type<T, Ts...>() != cista::TYPE_NOT_FOUND);
  return v.template as<T>();
}

template <class T, class... Ts>
constexpr std::add_pointer_t<T> get_if(cista::variant<Ts...>& v) {
  static_assert(cista::index_of_type<T, Ts...>() != cista::TYPE_NOT_FOUND);
  return v.idx_ == &cista::index_of_type<T, Ts...> ? v.template as<T>()
                                                   : nullptr;
}

template <std::size_t I, typename... Ts>
constexpr std::add_pointer_t<cista::type_at_index_t<I, Ts...> const> get_if(
    cista::variant<Ts...> const& v) {
  static_assert(I < sizeof...(Ts));
  return v.idx_ == I ? &v.template as<cista::type_at_index_t<I, Ts...>>()
                     : nullptr;
}

template <std::size_t I, typename... Ts>
constexpr std::add_pointer_t<cista::type_at_index_t<I, Ts...>> get_if(
    cista::variant<Ts...>& v) {
  static_assert(I < sizeof...(Ts));
  return v.idx_ == I ? &v.template as<cista::type_at_index_t<I, Ts...>>()
                     : nullptr;
}

template <class T, class... Ts>
constexpr std::add_pointer_t<T const> get_if(cista::variant<Ts...> const& v) {
  static_assert(cista::index_of_type<T, Ts...>() != cista::TYPE_NOT_FOUND);
  return v.idx_ == cista::index_of_type<T, Ts...> ? &v.template as<T>()
                                                  : nullptr;
}

template <class T>
struct variant_size;

template <class... T>
struct variant_size<variant<T...>>
    : std::integral_constant<std::size_t, sizeof...(T)> {};

template <class T>
inline constexpr std::size_t variant_size_v = variant_size<T>::value;

template <typename... T>
struct hashing<variant<T...>> {
  constexpr hash_t operator()(variant<T...> const& el,
                              hash_t const seed = BASE_HASH) {
    return el.apply([&](auto&& val) {
      auto const idx = el.index();
      auto h = seed;
      h = hashing<decltype(idx)>{}(idx, h);
      h = hashing<decltype(val)>{}(val, h);
      return h;
    });
  }
};

namespace raw {
using cista::variant;
}  // namespace raw

namespace offset {
using cista::variant;
}  // namespace offset

}  // namespace cista

namespace std {

using cista::get;

}  // namespace std

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <ostream>
#include <type_traits>
#include <vector>


namespace cista {

template <typename T, typename Ptr = T*, bool IndexPointers = false,
          typename TemplateSizeType = uint32_t>
struct basic_vector {
  using size_type = TemplateSizeType;
  using value_type = T;
  using iterator = T*;
  using const_iterator = T const*;

  basic_vector() = default;
  explicit basic_vector(TemplateSizeType size) { resize(size); }
  basic_vector(std::initializer_list<T> init) { set(init.begin(), init.end()); }

  template <typename It>
  basic_vector(It begin_it, It end_it) {
    set(begin_it, end_it);
  }

  basic_vector(basic_vector&& arr) noexcept
      : el_(arr.el_),
        used_size_(arr.used_size_),
        allocated_size_(arr.allocated_size_),
        self_allocated_(arr.self_allocated_) {
    arr.el_ = nullptr;
    arr.used_size_ = 0;
    arr.self_allocated_ = false;
    arr.allocated_size_ = 0;
  }

  basic_vector(basic_vector const& arr) { set(std::begin(arr), std::end(arr)); }

  basic_vector& operator=(basic_vector&& arr) noexcept {
    deallocate();

    el_ = arr.el_;
    used_size_ = arr.used_size_;
    self_allocated_ = arr.self_allocated_;
    allocated_size_ = arr.allocated_size_;

    arr.el_ = nullptr;
    arr.used_size_ = 0;
    arr.self_allocated_ = false;
    arr.allocated_size_ = 0;

    return *this;
  }

  basic_vector& operator=(basic_vector const& arr) {
    set(std::begin(arr), std::end(arr));
    return *this;
  }

  ~basic_vector() { deallocate(); }

  void deallocate() {
    if (!self_allocated_ || el_ == nullptr) {
      return;
    }

    for (auto& el : *this) {
      el.~T();
    }

    std::free(el_);  // NOLINT
    el_ = nullptr;
    used_size_ = 0;
    allocated_size_ = 0;
    self_allocated_ = 0;
  }

  T const* data() const { return begin(); }
  T* data() { return begin(); }
  T const* begin() const { return el_; }
  T const* end() const { return el_ + used_size_; }  // NOLINT
  T* begin() { return el_; }
  T* end() { return el_ + used_size_; }  // NOLINT

  std::reverse_iterator<T const*> rbegin() const {
    return std::reverse_iterator<T*>(el_ + size());  // NOLINT
  }
  std::reverse_iterator<T const*> rend() const {
    return std::reverse_iterator<T*>(el_);
  }
  std::reverse_iterator<T*> rbegin() {
    return std::reverse_iterator<T*>(el_ + size());  // NOLINT
  }
  std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(el_); }

  friend T const* begin(basic_vector const& a) { return a.begin(); }
  friend T const* end(basic_vector const& a) { return a.end(); }

  friend T* begin(basic_vector& a) { return a.begin(); }
  friend T* end(basic_vector& a) { return a.end(); }

  inline T const& operator[](size_t index) const {
    assert(el_ != nullptr && index < used_size_);
    return el_[index];
  }
  inline T& operator[](size_t index) {
    assert(el_ != nullptr && index < used_size_);
    return el_[index];
  }

  inline T& at(size_t index) {
    if (index >= used_size_) {
      throw std::out_of_range{"vector::at(): invalid index"};
    } else {
      return (*this)[index];
    }
  }

  inline T const& at(size_t index) const {
    return const_cast<basic_vector*>(this)->at(index);
  }

  T const& back() const { return el_[used_size_ - 1]; }
  T& back() { return el_[used_size_ - 1]; }

  T& front() { return el_[0]; }
  T const& front() const { return el_[0]; }

  inline TemplateSizeType size() const { return used_size_; }
  inline bool empty() const { return size() == 0; }

  template <typename It>
  void set(It begin_it, It end_it) {
    auto range_size =
        static_cast<TemplateSizeType>(std::distance(begin_it, end_it));
    assert(range_size <= std::numeric_limits<TemplateSizeType>::max() &&
           "size type overflow");
    reserve(static_cast<TemplateSizeType>(range_size));

    auto copy_source = begin_it;
    auto copy_target = el_;
    for (; copy_source != end_it; ++copy_source, ++copy_target) {
      new (copy_target) T(*copy_source);
    }

    used_size_ = static_cast<TemplateSizeType>(range_size);
  }

  friend std::ostream& operator<<(std::ostream& out, basic_vector const& v) {
    out << "[\n  ";
    auto first = true;
    for (auto const& e : v) {
      if (!first) {
        out << ",\n  ";
      }
      out << e;
      first = false;
    }
    return out << "\n]";
  }

  template <typename Arg>
  void insert(T* it, Arg&& el) {
    auto const pos = it - begin();

    reserve(used_size_ + 1);
    it = begin() + pos;

    for (auto move_it = end() - 1, pred = end(); pred != it; --move_it) {
      *pred = std::move(*move_it);
      pred = move_it;
    }

    new (it) T{std::forward<Arg>(el)};
    ++used_size_;
  }

  void push_back(T const& el) {
    reserve(used_size_ + 1);
    new (el_ + used_size_) T(el);
    ++used_size_;
  }

  template <typename... Args>
  T& emplace_back(Args&&... el) {
    reserve(used_size_ + 1);
    new (el_ + used_size_) T{std::forward<Args>(el)...};
    T* ptr = el_ + used_size_;
    ++used_size_;
    return *ptr;
  }

  void resize(size_type size, T init = T{}) {
    reserve(size);
    for (auto i = used_size_; i < size; ++i) {
      new (el_ + i) T{init};
    }
    used_size_ = size;
  }

  void clear() {
    used_size_ = 0;
    for (auto& el : *this) {
      el.~T();
    }
  }

  void reserve(TemplateSizeType new_size) {
    new_size = std::max(allocated_size_, new_size);

    if (allocated_size_ >= new_size) {
      return;
    }

    auto next_size = next_power_of_two(new_size);
    auto num_bytes = sizeof(T) * next_size;
    auto mem_buf = static_cast<T*>(std::malloc(num_bytes));  // NOLINT
    if (mem_buf == nullptr) {
      throw std::bad_alloc();
    }

    if (size() != 0) {
      try {
        auto move_target = mem_buf;
        for (auto& el : *this) {
          new (move_target++) T(std::move(el));
        }

        for (auto& el : *this) {
          el.~T();
        }
      } catch (...) {
        assert(0);
      }
    }

    auto free_me = el_;
    el_ = mem_buf;
    if (self_allocated_) {
      std::free(free_me);  // NOLINT
    }

    self_allocated_ = true;
    allocated_size_ = next_size;
  }

  T* erase(T* pos) {
    T* last = end() - 1;
    while (pos < last) {
      std::swap(*pos, *(pos + 1));
      pos = pos + 1;
    }
    pos->~T();
    --used_size_;
    return end();
  }

  bool contains(T const* el) const { return el >= begin() && el < end(); }

  friend bool operator==(basic_vector const& a, basic_vector const& b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
  }
  friend bool operator!=(basic_vector const& a, basic_vector const& b) {
    return !(a == b);
  }
  friend bool operator<(basic_vector const& a, basic_vector const& b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
  }
  friend bool operator>(basic_vector const& a, basic_vector const& b) {
    return b < a;
  }
  friend bool operator<=(basic_vector const& a, basic_vector const& b) {
    return !(a > b);
  }
  friend bool operator>=(basic_vector const& a, basic_vector const& b) {
    return !(a < b);
  }

  Ptr el_{nullptr};
  TemplateSizeType used_size_{0};
  TemplateSizeType allocated_size_{0};
  bool self_allocated_{false};
  uint8_t __fill_0__{0};
  uint16_t __fill_1__{0};
  uint32_t __fill_2__{0};
};

#define CISTA_TO_VEC                                                        \
  template <typename It, typename UnaryOperation>                           \
  inline auto to_vec(It s, It e, UnaryOperation&& op)                       \
      ->vector<decay_t<decltype(op(*s))>> {                                 \
    vector<decay_t<decltype(op(*s))>> v;                                    \
    v.reserve(                                                              \
        static_cast<typename decltype(v)::size_type>(std::distance(s, e))); \
    std::transform(s, e, std::back_inserter(v), op);                        \
    return v;                                                               \
  }                                                                         \
                                                                            \
  template <typename Container, typename UnaryOperation>                    \
  inline auto to_vec(Container const& c, UnaryOperation&& op)               \
      ->vector<decltype(op(*std::begin(c)))> {                              \
    vector<decltype(op(*std::begin(c)))> v;                                 \
    v.reserve(static_cast<typename decltype(v)::size_type>(                 \
        std::distance(std::begin(c), std::end(c))));                        \
    std::transform(std::begin(c), std::end(c), std::back_inserter(v), op);  \
    return v;                                                               \
  }                                                                         \
                                                                            \
  template <typename Container>                                             \
  inline auto to_vec(Container const& c)                                    \
      ->vector<decay_t<decltype(*std::begin(c))>> {                         \
    vector<decay_t<decltype(*std::begin(c))>> v;                            \
    v.reserve(static_cast<typename decltype(v)::size_type>(                 \
        std::distance(std::begin(c), std::end(c))));                        \
    std::copy(std::begin(c), std::end(c), std::back_inserter(v));           \
    return v;                                                               \
  }                                                                         \
                                                                            \
  template <typename It, typename UnaryOperation>                           \
  inline auto to_indexed_vec(It s, It e, UnaryOperation&& op)               \
      ->indexed_vector<decay_t<decltype(op(*s))>> {                         \
    indexed_vector<decay_t<decltype(op(*s))>> v;                            \
    v.reserve(                                                              \
        static_cast<typename decltype(v)::size_type>(std::distance(s, e))); \
    std::transform(s, e, std::back_inserter(v), op);                        \
    return v;                                                               \
  }                                                                         \
                                                                            \
  template <typename Container, typename UnaryOperation>                    \
  inline auto to_indexed_vec(Container const& c, UnaryOperation&& op)       \
      ->indexed_vector<decay_t<decltype(op(*std::begin(c)))>> {             \
    indexed_vector<decay_t<decltype(op(*std::begin(c)))>> v;                \
    v.reserve(static_cast<typename decltype(v)::size_type>(                 \
        std::distance(std::begin(c), std::end(c))));                        \
    std::transform(std::begin(c), std::end(c), std::back_inserter(v), op);  \
    return v;                                                               \
  }                                                                         \
                                                                            \
  template <typename Container>                                             \
  inline auto to_indexed_vec(Container const& c)                            \
      ->indexed_vector<decay_t<decltype(*std::begin(c))>> {                 \
    indexed_vector<decay_t<decltype(*std::begin(c))>> v;                    \
    v.reserve(static_cast<typename decltype(v)::size_type>(                 \
        std::distance(std::begin(c), std::end(c))));                        \
    std::copy(std::begin(c), std::end(c), std::back_inserter(v));           \
    return v;                                                               \
  }

namespace raw {

template <typename T>
using vector = basic_vector<T, ptr<T>>;

template <typename T>
using indexed_vector = basic_vector<T, ptr<T>, true>;

CISTA_TO_VEC

}  // namespace raw

namespace offset {

template <typename T>
using vector = basic_vector<T, ptr<T>>;

template <typename T>
using indexed_vector = basic_vector<T, ptr<T>, true>;

CISTA_TO_VEC

}  // namespace offset

#undef CISTA_TO_VEC

}  // namespace cista

#include <cstdio>
#include <cstring>

#include <memory>


// Credits: Manu Sánchez (@Manu343726)
// https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/pretty_function.hpp

#include <string>
#include <string_view>

namespace cista {

#if defined(_MSC_VER)
#define CISTA_SIG __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#define CISTA_SIG __PRETTY_FUNCTION__
#else
#error unsupported compiler
#endif

inline void remove_all(std::string& s, std::string_view substr) {
  auto pos = std::size_t{};
  while ((pos = s.find(substr, pos)) != std::string::npos) {
    s.erase(pos, substr.length());
  }
}

inline void canonicalize_type_name(std::string& s) {
  remove_all(s, "{anonymous}::");  // GCC
  remove_all(s, "(anonymous namespace)::");  // Clang
  remove_all(s, "`anonymous-namespace'::");  // MSVC
  remove_all(s, "struct");  // MSVC "struct my_struct" vs "my_struct"
  remove_all(s, "const");  // MSVC "char const*"" vs "const char*"
  remove_all(s, " ");  // MSVC
}

template <typename T>
constexpr std::string_view type_str() {
#if defined(__clang__)
  constexpr std::string_view prefix =
      "std::string_view cista::type_str() [T = ";
  constexpr std::string_view suffix = "]";
#elif defined(_MSC_VER)
  constexpr std::string_view prefix =
      "class std::basic_string_view<char,struct std::char_traits<char> > "
      "__cdecl cista::type_str<";
  constexpr std::string_view suffix = ">(void)";
#else
  constexpr std::string_view prefix =
      "constexpr std::string_view cista::type_str() [with T = ";
  constexpr std::string_view suffix =
      "; std::string_view = std::basic_string_view<char>]";
#endif

  auto sig = std::string_view{CISTA_SIG};
  sig.remove_prefix(prefix.size());
  sig.remove_suffix(suffix.size());
  return sig;
}

template <typename T>
std::string canonical_type_str() {
  auto const base = type_str<T>();
  std::string s{base.data(), base.length()};
  canonicalize_type_name(s);
  return s;
}

}  // namespace cista

#undef CISTA_SIG

namespace cista {

constexpr auto const MAX_ALIGN = 16;
using byte_buf = std::vector<uint8_t>;

template <typename Buf = byte_buf>
struct buf {
  buf() = default;
  explicit buf(Buf&& buf) : buf_{std::forward<Buf>(buf)} {}

  uint8_t* addr(offset_t const offset) { return (&buf_[0]) + offset; }
  uint8_t* base() { return &buf_[0]; }

  uint64_t checksum(offset_t const start = 0) const {
    return hash(std::string_view{
        reinterpret_cast<char const*>(&buf_[static_cast<size_t>(start)]),
        buf_.size() - static_cast<size_t>(start)});
  }

  template <typename T>
  void write(std::size_t const pos, T const& val) {
    verify(buf_.size() >= pos + serialized_size<T>(), "out of bounds write");
    std::memcpy(&buf_[pos], &val, serialized_size<T>());
  }

  offset_t write(void const* ptr, std::size_t const num_bytes,
                 std::size_t alignment = 0) {
    auto start = static_cast<offset_t>(size());
    if (alignment != 0 && alignment != 1 && buf_.size() != 0) {
      auto unaligned_ptr = static_cast<void*>(addr(start));
      auto space = std::numeric_limits<std::size_t>::max();
      auto const aligned_ptr =
          std::align(alignment, num_bytes, unaligned_ptr, space);
      auto const new_offset = static_cast<offset_t>(
          aligned_ptr ? static_cast<uint8_t*>(aligned_ptr) - base() : 0);
      auto const adjustment = static_cast<std::size_t>(new_offset - start);
      start += adjustment;
    }

    auto const space_left =
        static_cast<int64_t>(buf_.size()) - static_cast<int64_t>(start);
    if (space_left < static_cast<int64_t>(num_bytes)) {
      auto const missing = static_cast<std::size_t>(
          static_cast<int64_t>(num_bytes) - space_left);
      buf_.resize(buf_.size() + missing);
    }
    std::memcpy(addr(start), ptr, num_bytes);

    return start;
  }

  unsigned char& operator[](size_t i) { return buf_[i]; }
  unsigned char const& operator[](size_t i) const { return buf_[i]; }
  size_t size() const { return buf_.size(); }
  void reset() { buf_.resize(0U); }

  Buf buf_;
};

template <typename Buf>
buf(Buf &&)->buf<Buf>;

}  // namespace cista

#include <map>


#include <type_traits>

namespace cista {

template <typename T>
struct indexed : public T {
  using value_type = T;
  using T::T;
  using T::operator=;
};

template <typename Ptr>
struct is_indexed_helper : std::false_type {};

template <typename T>
struct is_indexed_helper<indexed<T>> : std::true_type {};

template <class T>
constexpr inline bool is_indexed_v =
    is_indexed_helper<std::remove_cv_t<T>>::value;

}  // namespace cista

namespace cista {

template <typename T>
hash_t type2str_hash() {
  return hash_combine(hash(canonical_type_str<decay_t<T>>()), sizeof(T));
}

template <typename T>
hash_t type_hash(T const& el, hash_t h, std::map<hash_t, unsigned>& done) {
  using Type = decay_t<T>;

  auto const base_hash = type2str_hash<Type>();
  auto [it, inserted] =
      done.try_emplace(base_hash, static_cast<unsigned>(done.size()));
  if (!inserted) {
    return hash_combine(h, it->second);
  }

  if constexpr (is_pointer_v<Type>) {
    using PointeeType = remove_pointer_t<Type>;
    if constexpr (std::is_same_v<PointeeType, void>) {
      return hash_combine(h, hash("void*"));
    } else {
      return type_hash(remove_pointer_t<Type>{},
                       hash_combine(h, hash("pointer")), done);
    }
  } else if constexpr (std::is_integral_v<Type>) {
    return hash_combine(h, hash("i"), sizeof(Type));
  } else if constexpr (std::is_scalar_v<Type>) {
    return hash_combine(h, type2str_hash<T>());
  } else {
    static_assert(to_tuple_works_v<Type>, "Please implement custom type hash.");
    h = hash_combine(h, hash("struct"));
    for_each_field(el,
                   [&](auto const& member) { h = type_hash(member, h, done); });
    return h;
  }
}

template <typename T, size_t Size>
hash_t type_hash(array<T, Size> const&, hash_t h,
                 std::map<hash_t, unsigned>& done) {
  h = hash_combine(h, hash("array"));
  h = hash_combine(h, Size);
  return type_hash(T{}, h, done);
}

template <typename T, typename Ptr, bool Indexed, typename TemplateSizeType>
hash_t type_hash(basic_vector<T, Ptr, Indexed, TemplateSizeType> const&,
                 hash_t h, std::map<hash_t, unsigned>& done) {
  h = hash_combine(h, hash("vector"));
  return type_hash(T{}, h, done);
}

template <typename T, typename Ptr>
hash_t type_hash(basic_unique_ptr<T, Ptr> const&, hash_t h,
                 std::map<hash_t, unsigned>& done) {
  h = hash_combine(h, hash("unique_ptr"));
  return type_hash(T{}, h, done);
}

template <typename T, template <typename> typename Ptr, typename GetKey,
          typename GetValue, typename Hash, typename Eq>
hash_t type_hash(hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq> const&,
                 hash_t h, std::map<hash_t, unsigned>& done) {
  h = hash_combine(h, hash("hash_storage"));
  return type_hash(T{}, h, done);
}

template <typename... T>
hash_t type_hash(variant<T...> const&, hash_t h,
                 std::map<hash_t, unsigned>& done) {
  h = hash_combine(h, hash("variant"));
  ((h = type_hash(T{}, h, done)), ...);
  return h;
}

template <typename Ptr>
hash_t type_hash(generic_string<Ptr> const&, hash_t h,
                 std::map<hash_t, unsigned>&) {
  return hash_combine(h, hash("string"));
}

template <typename Ptr>
hash_t type_hash(basic_string<Ptr> const&, hash_t h,
                 std::map<hash_t, unsigned>&) {
  return hash_combine(h, hash("string"));
}

template <typename Ptr>
hash_t type_hash(basic_string_view<Ptr> const&, hash_t h,
                 std::map<hash_t, unsigned>&) {
  return hash_combine(h, hash("string"));
}

template <typename T>
hash_t type_hash(indexed<T> const&, hash_t h,
                 std::map<hash_t, unsigned>& done) {
  return type_hash(T{}, h, done);
}

template <typename T>
hash_t type_hash() {
  auto done = std::map<hash_t, unsigned>{};
  return type_hash(T{}, BASE_HASH, done);
}

}  // namespace cista

#define CISTA_UNUSED_PARAM(param) (void)(param);

#ifndef cista_member_offset
#define cista_member_offset(s, m) (static_cast<cista::offset_t>(offsetof(s, m)))
#endif

namespace cista {

// =============================================================================
// SERIALIZE
// -----------------------------------------------------------------------------
struct pending_offset {
  void const* origin_ptr_;
  offset_t pos_;
};

struct vector_range {
  bool contains(void const* begin, void const* ptr) const {
    auto const ptr_int = reinterpret_cast<uintptr_t>(ptr);
    auto const from = reinterpret_cast<uintptr_t>(begin);
    auto const to =
        reinterpret_cast<uintptr_t>(begin) + static_cast<uintptr_t>(size_);
    return ptr_int >= from && ptr_int < to;
  }

  offset_t offset_of(void const* begin, void const* ptr) const {
    return start_ + reinterpret_cast<intptr_t>(ptr) -
           reinterpret_cast<intptr_t>(begin);
  }

  offset_t start_;
  size_t size_;
};

template <typename Target, mode Mode>
struct serialization_context {
  static constexpr auto const MODE = Mode;

  explicit serialization_context(Target& t) : t_{t} {}

  static bool compare(std::pair<void const*, vector_range> const& a,
                      std::pair<void const*, vector_range> const& b) {
    return a.first < b.first;
  }

  offset_t write(void const* ptr, std::size_t const size,
                 std::size_t const alignment = 0) {
    return t_.write(ptr, size, alignment);
  }

  template <typename T>
  void write(offset_t const pos, T const& val) {
    t_.write(static_cast<std::size_t>(pos), val);
  }

  template <typename T>
  bool resolve_pointer(offset_ptr<T> const& ptr, offset_t const pos,
                       bool add_pending = true) {
    return resolve_pointer(ptr.get(), pos, add_pending);
  }

  template <typename Ptr>
  bool resolve_pointer(Ptr ptr, offset_t const pos, bool add_pending = true) {
    if (std::is_same_v<decay_t<remove_pointer_t<Ptr>>, void> && add_pending) {
      write(pos, convert_endian<MODE>(NULLPTR_OFFSET));
      return true;
    } else if (ptr == nullptr) {
      write(pos, convert_endian<MODE>(NULLPTR_OFFSET));
      return true;
    } else if (auto const it = offsets_.find(ptr_cast(ptr));
               it != end(offsets_)) {
      write(pos, convert_endian<MODE>(it->second - pos));
      return true;
    } else if (auto const offset = resolve_vector_range_ptr(ptr);
               offset.has_value()) {
      write(pos, convert_endian<MODE>(*offset - pos));
      return true;
    } else if (add_pending) {
      write(pos, convert_endian<MODE>(NULLPTR_OFFSET));
      pending_.emplace_back(pending_offset{ptr_cast(ptr), pos});
      return true;
    }
    return false;
  }

  template <typename Ptr>
  std::optional<offset_t> resolve_vector_range_ptr(Ptr ptr) {
    if (vector_ranges_.empty()) {
      return std::nullopt;
    } else if (auto const vec_it = vector_ranges_.upper_bound(ptr);
               vec_it == begin(vector_ranges_)) {
      return std::nullopt;
    } else {
      auto const pred = std::prev(vec_it);
      return pred->second.contains(pred->first, ptr)
                 ? std::make_optional(pred->second.offset_of(pred->first, ptr))
                 : std::nullopt;
    }
  }

  uint64_t checksum(offset_t const from) const { return t_.checksum(from); }

  cista::raw::hash_map<void const*, offset_t> offsets_;
  std::map<void const*, vector_range> vector_ranges_;
  std::vector<pending_offset> pending_;
  Target& t_;
};

template <typename Ctx, typename T>
void serialize(Ctx& c, T const* origin, offset_t const pos) {
  using Type = decay_t<T>;
  if constexpr (std::is_union_v<Type>) {
    static_assert(std::is_standard_layout_v<Type> &&
                  std::is_trivially_copyable_v<Type>);
  } else if constexpr (is_pointer_v<Type>) {
    c.resolve_pointer(*origin, pos);
  } else if constexpr (is_indexed_v<Type>) {
    c.offsets_.emplace(origin, pos);
    serialize(c, static_cast<typename Type::value_type const*>(origin), pos);
  } else if constexpr (!std::is_scalar_v<Type>) {
    static_assert(to_tuple_works_v<Type>, "Please implement custom serializer");
    for_each_ptr_field(*origin, [&](auto& member) {
      auto const member_offset =
          static_cast<offset_t>(reinterpret_cast<intptr_t>(member) -
                                reinterpret_cast<intptr_t>(origin));
      serialize(c, member, pos + member_offset);
    });
  } else if constexpr (std::numeric_limits<Type>::is_integer) {
    c.write(pos, convert_endian<Ctx::MODE>(*origin));
  } else {
    (void)origin;
    (void)pos;
  }
}

template <typename Ctx, typename T, typename Ptr, bool Indexed,
          typename TemplateSizeType>
void serialize(Ctx& c,
               basic_vector<T, Ptr, Indexed, TemplateSizeType> const* origin,
               offset_t const pos) {
  using Type = basic_vector<T, Ptr, Indexed, TemplateSizeType>;

  auto const size = serialized_size<T>() * origin->used_size_;
  auto const start = origin->el_ == nullptr
                         ? NULLPTR_OFFSET
                         : c.write(static_cast<T const*>(origin->el_), size,
                                   std::alignment_of_v<T>);

  c.write(pos + cista_member_offset(Type, el_),
          convert_endian<Ctx::MODE>(
              start == NULLPTR_OFFSET
                  ? start
                  : start - cista_member_offset(Type, el_) - pos));
  c.write(pos + cista_member_offset(Type, allocated_size_),
          convert_endian<Ctx::MODE>(origin->used_size_));
  c.write(pos + cista_member_offset(Type, used_size_),
          convert_endian<Ctx::MODE>(origin->used_size_));
  c.write(pos + cista_member_offset(Type, self_allocated_), false);

  if constexpr (Indexed) {
    if (origin->el_ != nullptr) {
      c.vector_ranges_.emplace(origin->el_, vector_range{start, size});
    }
  }

  if (origin->el_ != nullptr) {
    auto i = 0u;
    for (auto it = start; it != start + static_cast<offset_t>(size);
         it += serialized_size<T>()) {
      serialize(c, static_cast<T const*>(origin->el_ + i++), it);
    }
  }
}

template <typename Ctx, typename Ptr>
void serialize(Ctx& c, generic_string<Ptr> const* origin, offset_t const pos) {
  using Type = generic_string<Ptr>;

  if (origin->is_short()) {
    return;
  }

  auto const start = (origin->h_.ptr_ == nullptr)
                         ? NULLPTR_OFFSET
                         : c.write(origin->data(), origin->size());
  c.write(pos + cista_member_offset(Type, h_.ptr_),
          convert_endian<Ctx::MODE>(
              start == NULLPTR_OFFSET
                  ? start
                  : start - cista_member_offset(Type, h_.ptr_) - pos));
  c.write(pos + cista_member_offset(Type, h_.size_),
          convert_endian<Ctx::MODE>(origin->h_.size_));
  c.write(pos + cista_member_offset(Type, h_.self_allocated_), false);
}

template <typename Ctx, typename Ptr>
void serialize(Ctx& c, basic_string<Ptr> const* origin, offset_t const pos) {
  serialize(c, static_cast<generic_string<Ptr> const*>(origin), pos);
}

template <typename Ctx, typename Ptr>
void serialize(Ctx& c, basic_string_view<Ptr> const* origin,
               offset_t const pos) {
  serialize(c, static_cast<generic_string<Ptr> const*>(origin), pos);
}

template <typename Ctx, typename T, typename Ptr>
void serialize(Ctx& c, basic_unique_ptr<T, Ptr> const* origin,
               offset_t const pos) {
  using Type = basic_unique_ptr<T, Ptr>;

  auto const start =
      origin->el_ == nullptr
          ? NULLPTR_OFFSET
          : c.write(origin->el_, serialized_size<T>(), std::alignment_of_v<T>);

  c.write(pos + cista_member_offset(Type, el_),
          convert_endian<Ctx::MODE>(
              start == NULLPTR_OFFSET
                  ? start
                  : start - cista_member_offset(Type, el_) - pos));
  c.write(pos + cista_member_offset(Type, self_allocated_), false);

  if (origin->el_ != nullptr) {
    c.offsets_[origin->el_] = start;
    serialize(c, ptr_cast(origin->el_), start);
  }
}

template <typename Ctx, typename T, template <typename> typename Ptr,
          typename GetKey, typename GetValue, typename Hash, typename Eq>
void serialize(Ctx& c,
               hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq> const* origin,
               offset_t const pos) {
  using Type = hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq>;

  auto const start =
      origin->entries_ == nullptr
          ? NULLPTR_OFFSET
          : c.write(
                origin->entries_,
                static_cast<size_t>(origin->capacity_ * serialized_size<T>() +
                                    (origin->capacity_ + 1 + Type::WIDTH) *
                                        sizeof(typename Type::ctrl_t)),
                std::alignment_of_v<T>);
  auto const ctrl_start =
      start == NULLPTR_OFFSET
          ? c.write(Type::empty_group(), 16 * sizeof(typename Type::ctrl_t),
                    std::alignment_of_v<typename Type::ctrl_t>)
          : start +
                static_cast<offset_t>(origin->capacity_ * serialized_size<T>());

  c.write(pos + cista_member_offset(Type, entries_),
          convert_endian<Ctx::MODE>(
              start == NULLPTR_OFFSET
                  ? start
                  : start - cista_member_offset(Type, entries_) - pos));
  c.write(pos + cista_member_offset(Type, ctrl_),
          convert_endian<Ctx::MODE>(
              ctrl_start == NULLPTR_OFFSET
                  ? ctrl_start
                  : ctrl_start - cista_member_offset(Type, ctrl_) - pos));

  c.write(pos + cista_member_offset(Type, self_allocated_), false);

  c.write(pos + cista_member_offset(Type, size_),
          convert_endian<Ctx::MODE>(origin->size_));
  c.write(pos + cista_member_offset(Type, capacity_),
          convert_endian<Ctx::MODE>(origin->capacity_));
  c.write(pos + cista_member_offset(Type, growth_left_),
          convert_endian<Ctx::MODE>(origin->growth_left_));

  if (origin->entries_ != nullptr) {
    auto i = 0u;
    for (auto it = start;
         it != start + static_cast<offset_t>(origin->capacity_ *
                                             serialized_size<T>());
         it += serialized_size<T>(), ++i) {
      if (Type::is_full(origin->ctrl_[i])) {
        serialize(c, static_cast<T*>(origin->entries_ + i), it);
      }
    }
  }
}

template <typename Ctx, typename T, size_t Size>
void serialize(Ctx& c, array<T, Size> const* origin, offset_t const pos) {
  auto const size =
      static_cast<offset_t>(serialized_size<T>() * origin->size());
  auto i = 0u;
  for (auto it = pos; it != pos + size; it += serialized_size<T>()) {
    serialize(c, origin->el_ + i++, it);
  }
}

template <typename Ctx, typename... T>
void serialize(Ctx& c, variant<T...> const* origin, offset_t const pos) {
  using Type = decay_t<decltype(*origin)>;
  c.write(pos + cista_member_offset(Type, idx_),
          convert_endian<Ctx::MODE>(origin->idx_));
  origin->apply([&](auto&& t) {
    serialize(c, &t, pos + cista_member_offset(Type, storage_));
  });
}

constexpr offset_t integrity_start(mode const m) {
  offset_t start = 0;
  if (is_mode_enabled(m, mode::WITH_VERSION)) {
    start += sizeof(uint64_t);
  }
  return start;
}

constexpr offset_t data_start(mode const m) {
  auto start = integrity_start(m);
  if (is_mode_enabled(m, mode::WITH_INTEGRITY)) {
    start += sizeof(uint64_t);
  }
  return start;
}

template <mode const Mode = mode::NONE, typename Target, typename T>
void serialize(Target& t, T& value) {
  serialization_context<Target, Mode> c{t};

  if constexpr (is_mode_enabled(Mode, mode::WITH_VERSION)) {
    auto const h = convert_endian<Mode>(type_hash<decay_t<T>>());
    c.write(&h, sizeof(h));
  }

  auto integrity_offset = offset_t{0};
  if constexpr (is_mode_enabled(Mode, mode::WITH_INTEGRITY)) {
    auto const h = hash_t{};
    integrity_offset = c.write(&h, sizeof(h));
  }

  serialize(c, &value,
            c.write(&value, serialized_size<T>(),
                    std::alignment_of_v<decay_t<decltype(value)>>));

  for (auto& p : c.pending_) {
    if (!c.resolve_pointer(p.origin_ptr_, p.pos_, false)) {
      printf("warning: dangling pointer at %" PRI_O " (origin=%p)\n", p.pos_,
             p.origin_ptr_);
    }
  }

  if constexpr (is_mode_enabled(Mode, mode::WITH_INTEGRITY)) {
    auto const csum =
        c.checksum(integrity_offset + static_cast<offset_t>(sizeof(hash_t)));
    c.write(integrity_offset, convert_endian<Mode>(csum));
  }
}

template <mode const Mode = mode::NONE, typename T>
byte_buf serialize(T& el) {
  auto b = buf{};
  serialize<Mode>(b, el);
  return std::move(b.buf_);
}

// =============================================================================
// DESERIALIZE
// -----------------------------------------------------------------------------
template <typename Arg, typename... Args>
Arg checked_addition(Arg a1, Args... aN) {
  using Type = decay_t<Arg>;

  auto add_if_ok = [&](Arg x) {
    if (x == 0) {
      return;
    } else if (x < 0) {
      if (a1 < std::numeric_limits<Type>::min() - x) {
        throw std::overflow_error("addition overflow");
      }
    } else if (x > 0) {
      if (a1 > std::numeric_limits<Type>::max() - x) {
        throw std::overflow_error("addition overflow");
      }
    }
    a1 = a1 + x;
  };
  (add_if_ok(aN), ...);
  return a1;
}

template <typename Arg, typename... Args>
Arg checked_multiplication(Arg a1, Args... aN) {
  using Type = decay_t<Arg>;
  auto multiply_if_ok = [&](auto x) {
    if (a1 != 0 && ((std::numeric_limits<Type>::max() / a1) < x)) {
      throw std::overflow_error("addition overflow");
    }
    a1 = a1 * x;
  };
  (multiply_if_ok(aN), ...);
  return a1;
}

template <mode Mode>
struct deserialization_context {
  static constexpr auto const MODE = Mode;

  deserialization_context(uint8_t const* from, uint8_t const* to)
      : from_{reinterpret_cast<intptr_t>(from)},
        to_{reinterpret_cast<intptr_t>(to)} {}

  template <typename T>
  void convert_endian(T& el) const {
    if (endian_conversion_necessary<MODE>()) {
      el = ::cista::convert_endian<MODE>(el);
    }
  }

  template <typename Ptr>
  void deserialize_ptr(Ptr** ptr) const {
    auto const offset =
        reinterpret_cast<offset_t>(::cista::convert_endian<MODE>(*ptr));
    static_assert(is_mode_disabled(MODE, mode::_CONST),
                  "raw pointer deserialize is not const");
    *ptr = offset == NULLPTR_OFFSET
               ? nullptr
               : reinterpret_cast<Ptr*>(
                     checked_addition(reinterpret_cast<offset_t>(ptr), offset));
  }

  template <typename T>
  constexpr static size_t type_size() {
    using Type = decay_t<T>;
    if constexpr (std::is_same_v<Type, void>) {
      return 0;
    } else {
      return sizeof(Type);
    }
  }

  template <typename T>
  void check_ptr(offset_ptr<T> const& el,
                 size_t const size = type_size<T>()) const {
    checked_addition(el.offset_, reinterpret_cast<intptr_t>(&el));
    check_ptr(el.get(), size);
  }

  template <typename T>
  void check_ptr(T* el, size_t const size = type_size<T>()) const {
    if constexpr ((MODE & mode::UNCHECKED) == mode::UNCHECKED) {
      return;
    }

    if (el == nullptr || to_ == 0U) {
      return;
    }

    auto const pos = reinterpret_cast<intptr_t>(el);
    verify(pos >= from_, "underflow");
    verify(checked_addition(pos, static_cast<intptr_t>(size)) <= to_,
           "overflow");
    verify(size < static_cast<size_t>(std::numeric_limits<intptr_t>::max()),
           "size out of bounds");

    if constexpr (!std::is_same_v<T, void>) {
      verify((pos &
              static_cast<intptr_t>(std::alignment_of<decay_t<T>>() - 1)) == 0U,
             "ptr alignment");
    }
  }

  static void check_bool(bool const& b) {
    auto const val = *reinterpret_cast<uint8_t const*>(&b);
    verify(val <= 1U, "valid bool");
  }

  void require(bool condition, char const* msg) const {
    if constexpr ((MODE & mode::UNCHECKED) == mode::UNCHECKED) {
      return;
    }

    if (!condition) {
      throw std::runtime_error(msg);
    }
  }

  intptr_t from_, to_;
};

template <mode Mode>
struct deep_check_context : public deserialization_context<Mode> {
  using parent = deserialization_context<Mode>;

  using parent::parent;

  template <typename T>
  bool add_checked(T const* v) const {
    return checked_.emplace(type_hash<T>(), static_cast<void const*>(v)).second;
  }

  std::set<std::pair<hash_t, void const*>> mutable checked_;
};

template <typename T, mode const Mode = mode::NONE>
void check(uint8_t const* from, uint8_t const* to) {
  verify(to - from > data_start(Mode), "invalid range");

  if constexpr ((Mode & mode::WITH_VERSION) == mode::WITH_VERSION) {
    verify(convert_endian<Mode>(*reinterpret_cast<hash_t const*>(from)) ==
               type_hash<T>(),
           "invalid version");
  }

  if constexpr ((Mode & mode::WITH_INTEGRITY) == mode::WITH_INTEGRITY) {
    verify(convert_endian<Mode>(*reinterpret_cast<uint64_t const*>(
               from + integrity_start(Mode))) ==
               hash(std::string_view{
                   reinterpret_cast<char const*>(from + data_start(Mode)),
                   static_cast<size_t>(to - from - data_start(Mode))}),
           "invalid checksum");
  }
}

// --- GENERIC ---
template <typename Ctx, typename T>
void convert_endian_and_ptr(Ctx const& c, T* el) {
  using Type = decay_t<T>;
  if constexpr (std::is_pointer_v<Type>) {
    c.deserialize_ptr(el);
  } else if constexpr (std::numeric_limits<Type>::is_integer) {
    c.convert_endian(*el);
  } else {
    CISTA_UNUSED_PARAM(c)
    CISTA_UNUSED_PARAM(el)
  }
}

template <typename Ctx, typename T>
void check_state(Ctx const& c, T* el) {
  using Type = decay_t<T>;
  if constexpr (std::is_pointer_v<Type> && !std::is_same_v<Type, void*>) {
    c.check_ptr(*el);
  } else {
    CISTA_UNUSED_PARAM(c)
    CISTA_UNUSED_PARAM(el)
  }
}

template <typename Ctx, typename T, typename Fn>
void recurse(Ctx& c, T* el, Fn&& fn) {
  using Type = decay_t<T>;
  if constexpr (is_indexed_v<Type>) {
    fn(static_cast<typename T::value_type*>(el));
  } else if constexpr (std::is_aggregate_v<Type> && !std::is_union_v<Type>) {
    for_each_ptr_field(*el, [&](auto& f) { fn(f); });
  } else if constexpr (is_mode_enabled(Ctx::MODE, mode::_PHASE_II) &&
                       std::is_pointer_v<Type>) {
    if (*el != nullptr && c.add_checked(el)) {
      fn(*el);
    }
  } else {
    CISTA_UNUSED_PARAM(c)
    CISTA_UNUSED_PARAM(el)
    CISTA_UNUSED_PARAM(fn)
  }
}

template <typename Ctx, typename T>
void deserialize(Ctx const& c, T* el) {
  c.check_ptr(el);
  if constexpr (is_mode_disabled(Ctx::MODE, mode::_PHASE_II)) {
    convert_endian_and_ptr(c, el);
  }
  if constexpr (is_mode_disabled(Ctx::MODE, mode::UNCHECKED)) {
    check_state(c, el);
  }
  recurse(c, el, [&](auto* entry) { deserialize(c, entry); });
}

// --- OFFSET_PTR<T> ---
template <typename Ctx, typename T>
void convert_endian_and_ptr(Ctx const& c, offset_ptr<T>* el) {
  c.convert_endian(el->offset_);
}

template <typename Ctx, typename T>
void check_state(Ctx const& c, offset_ptr<T>* el) {
  c.check_ptr(*el);
}

template <typename Ctx, typename T, typename Fn>
void recurse(Ctx& c, offset_ptr<T>* el, Fn&& fn) {
  if constexpr (is_mode_enabled(Ctx::MODE, mode::_PHASE_II)) {
    if (*el != nullptr && c.add_checked(el)) {
      fn(static_cast<T*>(*el));
    }
  } else {
    CISTA_UNUSED_PARAM(c)
    CISTA_UNUSED_PARAM(el)
    CISTA_UNUSED_PARAM(fn)
  }
}

// --- VECTOR<T> ---
template <typename Ctx, typename T, typename Ptr, bool Indexed,
          typename TemplateSizeType>
void convert_endian_and_ptr(
    Ctx const& c, basic_vector<T, Ptr, Indexed, TemplateSizeType>* el) {
  deserialize(c, &el->el_);
  c.convert_endian(el->allocated_size_);
  c.convert_endian(el->used_size_);
}

template <typename Ctx, typename T, typename Ptr, bool Indexed,
          typename TemplateSizeType>
void check_state(Ctx const& c,
                 basic_vector<T, Ptr, Indexed, TemplateSizeType>* el) {
  c.check_ptr(el->el_,
              checked_multiplication(static_cast<size_t>(el->allocated_size_),
                                     sizeof(T)));
  c.check_bool(el->self_allocated_);
  c.require(!el->self_allocated_, "vec self-allocated");
  c.require(el->allocated_size_ == el->used_size_, "vec size mismatch");
  c.require((el->size() == 0) == (el->el_ == nullptr), "vec size=0 <=> ptr=0");
}

template <typename Ctx, typename T, typename Ptr, bool Indexed,
          typename TemplateSizeType, typename Fn>
void recurse(Ctx&, basic_vector<T, Ptr, Indexed, TemplateSizeType>* el,
             Fn&& fn) {
  for (auto& m : *el) {
    fn(&m);
  }
}

// --- STRING ---
template <typename Ctx, typename Ptr>
void convert_endian_and_ptr(Ctx const& c, generic_string<Ptr>* el) {
  if (*reinterpret_cast<uint8_t const*>(&el->s_.is_short_) == 0U) {
    deserialize(c, &el->h_.ptr_);
    c.convert_endian(el->h_.size_);
  }
}

template <typename Ctx, typename Ptr>
void check_state(Ctx const& c, generic_string<Ptr>* el) {
  c.check_bool(el->s_.is_short_);
  if (!el->is_short()) {
    c.check_ptr(el->h_.ptr_, el->h_.size_);
    c.check_bool(el->h_.self_allocated_);
    c.require(!el->h_.self_allocated_, "string self-allocated");
    c.require((el->h_.size_ == 0) == (el->h_.ptr_ == nullptr),
              "str size=0 <=> ptr=0");
  }
}

template <typename Ctx, typename Ptr, typename Fn>
void recurse(Ctx&, generic_string<Ptr>*, Fn&&) {}

template <typename Ctx, typename Ptr>
void convert_endian_and_ptr(Ctx const& c, basic_string<Ptr>* el) {
  convert_endian_and_ptr(c, static_cast<generic_string<Ptr>*>(el));
}

template <typename Ctx, typename Ptr>
void check_state(Ctx const& c, basic_string<Ptr>* el) {
  check_state(c, static_cast<generic_string<Ptr>*>(el));
}

template <typename Ctx, typename Ptr, typename Fn>
void recurse(Ctx&, basic_string<Ptr>*, Fn&&) {}

template <typename Ctx, typename Ptr>
void convert_endian_and_ptr(Ctx const& c, basic_string_view<Ptr>* el) {
  convert_endian_and_ptr(c, static_cast<generic_string<Ptr>*>(el));
}

template <typename Ctx, typename Ptr>
void check_state(Ctx const& c, basic_string_view<Ptr>* el) {
  check_state(c, static_cast<generic_string<Ptr>*>(el));
}

template <typename Ctx, typename Ptr, typename Fn>
void recurse(Ctx&, basic_string_view<Ptr>*, Fn&&) {}

// --- UNIQUE_PTR<T> ---
template <typename Ctx, typename T, typename Ptr>
void convert_endian_and_ptr(Ctx const& c, basic_unique_ptr<T, Ptr>* el) {
  deserialize(c, &el->el_);
}

template <typename Ctx, typename T, typename Ptr>
void check_state(Ctx const& c, basic_unique_ptr<T, Ptr>* el) {
  c.check_bool(el->self_allocated_);
  c.require(!el->self_allocated_, "unique_ptr self-allocated");
}

template <typename Ctx, typename T, typename Ptr, typename Fn>
void recurse(Ctx&, basic_unique_ptr<T, Ptr>* el, Fn&& fn) {
  if (el->el_ != nullptr) {
    fn(static_cast<T*>(el->el_));
  }
}

// --- HASH_STORAGE<T> ---
template <typename Ctx, typename T, template <typename> typename Ptr,
          typename GetKey, typename GetValue, typename Hash, typename Eq>
void convert_endian_and_ptr(
    Ctx const& c, hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq>* el) {
  deserialize(c, &el->entries_);
  deserialize(c, &el->ctrl_);
  c.convert_endian(el->size_);
  c.convert_endian(el->capacity_);
  c.convert_endian(el->growth_left_);
}

template <typename Ctx, typename T, template <typename> typename Ptr,
          typename GetKey, typename GetValue, typename Hash, typename Eq>
void check_state(Ctx const& c,
                 hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq>* el) {
  using Type = decay_t<remove_pointer_t<decltype(el)>>;
  c.require(el->ctrl_ != nullptr, "hash storage: ctrl must be set");
  c.check_ptr(
      el->entries_,
      checked_addition(
          checked_multiplication(
              el->capacity_, static_cast<typename Type::size_type>(sizeof(T))),
          checked_addition(el->capacity_, 1U, Type::WIDTH)));
  c.check_ptr(el->ctrl_, checked_addition(el->capacity_, 1U, Type::WIDTH));
  c.require(el->entries_ == nullptr ||
                reinterpret_cast<uint8_t const*>(ptr_cast(el->ctrl_)) ==
                    reinterpret_cast<uint8_t const*>(ptr_cast(el->entries_)) +
                        checked_multiplication(
                            static_cast<size_t>(el->capacity_), sizeof(T)),
            "hash storage: entries!=null -> ctrl = entries+capacity");
  c.require(
      (el->entries_ == nullptr) == (el->capacity_ == 0U && el->size_ == 0U),
      "hash storage: entries=null <=> size=capacity=0");

  c.check_bool(el->self_allocated_);
  c.require(!el->self_allocated_, "hash storage: self-allocated");

  c.require(el->ctrl_[el->capacity_] == Type::END,
            "hash storage: end ctrl byte");
  c.require(std::all_of(ptr_cast(el->ctrl_),
                        ptr_cast(el->ctrl_) + el->capacity_ + 1U + Type::WIDTH,
                        [](typename Type::ctrl_t const ctrl) {
                          return Type::is_empty(ctrl) ||
                                 Type::is_deleted(ctrl) ||
                                 Type::is_full(ctrl) ||
                                 ctrl == Type::ctrl_t::END;
                        }),
            "hash storage: ctrl bytes must be empty or deleted or full");

  using st_t = typename Type::size_type;
  auto [empty, full, deleted, growth] = std::accumulate(
      ptr_cast(el->ctrl_), ptr_cast(el->ctrl_) + el->capacity_,
      std::tuple{st_t{0U}, st_t{0U}, st_t{0U}, st_t{0}},
      [&](std::tuple<st_t, st_t, st_t, st_t> const acc,
          typename Type::ctrl_t const& ctrl) {
        auto const [empty, full, deleted, growth_left] = acc;
        return std::tuple{
            Type::is_empty(ctrl) ? empty + 1 : empty,
            Type::is_full(ctrl) ? full + 1 : full,
            Type::is_deleted(ctrl) ? deleted + 1 : deleted,
            (Type::is_empty(ctrl) &&
                     el->was_never_full(static_cast<st_t>(&ctrl - el->ctrl_))
                 ? growth_left + 1
                 : growth_left)};
      });

  c.require(el->size_ == full, "hash storage: size");
  c.require(empty + full + deleted == el->capacity_,
            "hash storage: empty + full + deleted = capacity");
  c.require(std::min(Type::capacity_to_growth(el->capacity_) - el->size_,
                     growth) <= el->growth_left_,
            "hash storage: growth left");
}

template <typename Ctx, typename T, template <typename> typename Ptr,
          typename GetKey, typename GetValue, typename Hash, typename Eq,
          typename Fn>
void recurse(Ctx&, hash_storage<T, Ptr, GetKey, GetValue, Hash, Eq>* el,
             Fn&& fn) {
  for (auto& m : *el) {
    fn(&m);
  }
}

// --- ARRAY<T> ---
template <typename Ctx, typename T, size_t Size, typename Fn>
void recurse(Ctx&, array<T, Size>* el, Fn&& fn) {
  for (auto& m : *el) {
    fn(&m);
  }
}

// --- VARIANT<T...> ---
template <typename Ctx, typename Fn, typename... T>
void convert_endian_and_ptr(Ctx const& c, variant<T...>* el) {
  c.convert_endian(el->idx_);
}

template <typename Ctx, typename Fn, typename... T>
void recurse(Ctx&, variant<T...>* el, Fn&& fn) {
  el->apply([&](auto&& t) { fn(&t); });
}

template <typename Ctx, typename... T>
void check_state(Ctx const& c, variant<T...>* el) {
  c.require(el->index() < sizeof...(T), "variant index");
}

template <typename T, mode const Mode = mode::NONE>
T* deserialize(uint8_t* from, uint8_t* to = nullptr) {
  if constexpr (is_mode_enabled(Mode, mode::CAST)) {
    CISTA_UNUSED_PARAM(to)
    return reinterpret_cast<T*>(from);
  } else {
    check<T, Mode>(from, to);
    auto const el = reinterpret_cast<T*>(from + data_start(Mode));

    deserialization_context<Mode> c{from, to};
    deserialize(c, el);

    if constexpr ((Mode & mode::DEEP_CHECK) == mode::DEEP_CHECK) {
      deep_check_context<Mode | mode::_PHASE_II> c1{from, to};
      deserialize(c1, el);
    }

    return el;
  }
}

template <typename T, mode const Mode = mode::NONE>
T const* deserialize(uint8_t const* from, uint8_t const* to = nullptr) {
  static_assert(!endian_conversion_necessary<Mode>(), "cannot be const");
  return deserialize<T, Mode | mode::_CONST>(const_cast<uint8_t*>(from),
                                             const_cast<uint8_t*>(to));
}

template <typename T, mode const Mode = mode::NONE, typename CharT>
T const* deserialize(CharT const* from, CharT const* to = nullptr) {
  static_assert(sizeof(CharT) == 1U, "byte size entries");
  return deserialize<T, Mode>(reinterpret_cast<uint8_t const*>(from),
                              reinterpret_cast<uint8_t const*>(to));
}

template <typename T, mode const Mode = mode::NONE, typename CharT>
T* deserialize(CharT* from, CharT* to = nullptr) {
  static_assert(sizeof(CharT) == 1U, "byte size entries");
  return deserialize<T, Mode>(reinterpret_cast<uint8_t*>(from),
                              reinterpret_cast<uint8_t*>(to));
}

template <typename T, mode const Mode = mode::NONE>
T const* deserialize(std::string_view c) {
  return deserialize<T const, Mode>(&c[0], &c[0] + c.size());
}

template <typename T, mode const Mode = mode::NONE, typename Container>
auto deserialize(Container& c) {
  return deserialize<T, Mode>(&c[0], &c[0] + c.size());
}

template <typename T, mode const Mode = mode::NONE>
T* unchecked_deserialize(uint8_t* from, uint8_t* to = nullptr) {
  return deserialize<T, Mode | mode::UNCHECKED>(from, to);
}

template <typename T, mode const Mode = mode::NONE, typename Container>
T* unchecked_deserialize(Container& c) {
  return unchecked_deserialize<T, Mode>(&c[0], &c[0] + c.size());
}

namespace raw {
using cista::deserialize;
using cista::unchecked_deserialize;
}  // namespace raw

namespace offset {
using cista::deserialize;
using cista::unchecked_deserialize;
}  // namespace offset

}  // namespace cista

#undef cista_member_offset


#define CISTA_COMPARABLE()                               \
  template <typename T>                                  \
  bool operator==(T&& b) const {                         \
    return cista::to_tuple(*this) == cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator!=(T&& b) const {                         \
    return cista::to_tuple(*this) != cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator<(T&& b) const {                          \
    return cista::to_tuple(*this) < cista::to_tuple(b);  \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator<=(T&& b) const {                         \
    return cista::to_tuple(*this) <= cista::to_tuple(b); \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator>(T&& b) const {                          \
    return cista::to_tuple(*this) > cista::to_tuple(b);  \
  }                                                      \
                                                         \
  template <typename T>                                  \
  bool operator>=(T&& b) const {                         \
    return cista::to_tuple(*this) >= cista::to_tuple(b); \
  }

#include <ostream>

#ifndef CISTA_PRINTABLE_NO_VEC
#include <vector>
#endif


#ifndef CISTA_PRINTABLE_NO_VEC
template <typename T>
inline std::ostream& operator<<(std::ostream& out, std::vector<T> const& v) {
  out << "[\n  ";
  auto first = true;
  for (auto const& e : v) {
    if (!first) {
      out << ",\n  ";
    }
    using Type = cista::decay_t<T>;
    if constexpr (std::is_enum_v<Type>) {
      out << static_cast<std::underlying_type_t<Type>>(e);
    } else {
      out << e;
    }
    first = false;
  }
  return out << "\n]";
}
#endif

#define CISTA_PRINTABLE(class_name)                                         \
  friend std::ostream& operator<<(std::ostream& out, class_name const& o) { \
    bool first = true;                                                      \
    out << "{";                                                             \
    ::cista::for_each_field(o, [&](auto&& f) {                              \
      using Type = ::cista::decay_t<decltype(f)>;                           \
      if (!first) {                                                         \
        out << ", ";                                                        \
      } else {                                                              \
        first = false;                                                      \
      }                                                                     \
      if constexpr (std::is_enum_v<Type>) {                                 \
        out << static_cast<std::underlying_type_t<Type>>(f);                \
      } else {                                                              \
        out << f;                                                           \
      }                                                                     \
    });                                                                     \
    return out << "}";                                                      \
  }
