#pragma once

struct geometry;

#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

constexpr float M_PI = 3.14159265358979323846f;

template<class T>
T clamp(T val, T min, T max) {
	return std::max(min, std::min(max, val));
}


static ID3D11Device* g_device = NULL;
static ID3D11DeviceContext* g_context = NULL;
static IDXGIFactory* g_factory = NULL;
static ID3D11Buffer* g_vb = NULL;
static ID3D11Buffer* g_ib = NULL;
static ID3D11VertexShader* g_vshader = NULL;
static ID3D11InputLayout* g_inputlayout = NULL;
static ID3D11Buffer* g_vcbuff = NULL;
static ID3D11PixelShader* g_pshader = NULL;
static ID3D11SamplerState* g_fontsampler = NULL;
static ID3D11RasterizerState* g_rasterizer = NULL;
static ID3D11BlendState* g_blendstate = NULL;
static ID3D11DepthStencilState* g_depthstate = NULL;
static ID3D11ShaderResourceView* g_default_texture = NULL;
static size_t g_vb_size = 5000, g_ib_size = 10000;

static ID3D11ShaderResourceView* make_d3d11_texture(const void* rgba, int w, int h)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = rgba;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	ID3D11ShaderResourceView* view;
	g_device->CreateShaderResourceView(pTexture, &srvDesc, &view);
	pTexture->Release();
	return view;
}

struct VERTEX_CONSTANT_BUFFER
{
	float   mvp[4][4];
};



inline uint8_t mul_b_by_f(uint8_t b, float f)
{
	return static_cast<uint8_t>((static_cast<float>(b) * f));
}

inline uint8_t div_b_by_f(uint8_t b, float f)
{
	return static_cast<uint8_t>((static_cast<float>(b) / f));
}

inline float rsqrt(float number)
{
#ifdef _WIN64
	__m128  xx = _mm_load_ss(&number);
	__m128  xr = _mm_rsqrt_ss(xx);

	_mm_store_ss(&number, xr);
	return number;
#else
	float rroot;
	_asm
	{
		rsqrtss	xmm0, number
		movss	rroot, xmm0
	}
	return rroot;
#endif
}

using index = uint16_t;

struct color
{
	color() = default;
	color(uint8_t shade, uint8_t a) :
		r(shade), g(shade), b(shade), a(a) {}
	color(uint8_t r, uint8_t g, uint8_t b) :
		r(r), g(g), b(b), a(255) {}
	color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
		r(r), g(g), b(b), a(a) {}

	[[nodiscard]] uint32_t d3d9() const
	{
		return a << 24 | r << 16 | g << 8 | b;
	}

	[[nodiscard]] uint32_t rgba() const
	{
		return *reinterpret_cast<const uint32_t*>(this);
	}

	operator uint32_t() const {
		return rgba();
	}

	color& operator=(uint32_t v)
	{
		*reinterpret_cast<uint32_t*>(this) = v;
		return *this;
	}

	[[nodiscard]] color lerp(const color& c2, float f) const {
		return color(
			r + static_cast<int16_t>((static_cast<float>(c2.r - r) * f)),
			g + static_cast<int16_t>((static_cast<float>(c2.g - g) * f)),
			b + static_cast<int16_t>((static_cast<float>(c2.b - b) * f)),
			a + static_cast<int16_t>((static_cast<float>(c2.a - a) * f)));
	}

	[[nodiscard]] color new_a(uint8_t na) const {
		return { r, g, b, na };
	}

	uint8_t r = 255, g = 255, b = 255, a = 255;
};

struct position
{
	position() = default;
	position(float x, float y) :
		x(x), y(y) {}

	[[nodiscard]] float len() const {
		return x * x + y * y;
	}

	[[nodiscard]] float inv_len(float failure = 1.f) const {
		const auto l = len();

		if (l > .0f)
			return rsqrt(l);

		return failure;
	}

	[[nodiscard]] position max(const position& v) const {
		return { std::max(x, v.x), std::max(y, v.y) };
	}

	[[nodiscard]] position min(const position& v) const {
		return { std::min(x, v.x), std::min(y, v.y) };
	}

	[[nodiscard]] float distance_to(const position& p2) const {
		const auto a = x - p2.x;
		const auto b = y - p2.y;
		return sqrtf(a * a + b * b);
	}

	[[nodiscard]] position operator+(float v) const {
		return position(x + v, y + v);
	}

	[[nodiscard]] position operator+(const position& v) const {
		return position(x + v.x, y + v.y);
	}

	[[nodiscard]] position operator-(float v) const {
		return position(x - v, y - v);
	}

	[[nodiscard]] position operator-(const position& v) const {
		return position(x - v.x, y - v.y);
	}

	[[nodiscard]] position operator*(float v) const {
		return position(x * v, y * v);
	}

	[[nodiscard]] position operator*(const position& v) const {
		return position(x * v.x, y * v.y);
	}

	[[nodiscard]] position operator/(float v) const {
		return position(x / v, y / v);
	}

	[[nodiscard]] position operator/(const position& v) const {
		return position(x / v.x, y / v.y);
	}

	position& clamp(const position& min, const position& max) {
		x = ::clamp(x, min.x, max.x);
		y = ::clamp(y, min.y, max.y);
		return *this;
	}

	position& operator+=(float v) {
		x += v; y += v; return *this;
	}

	position& operator+=(const position& v) {
		x += v.x; y += v.y; return *this;
	}

	position& operator-=(float v) {
		x -= v; y -= v; return *this;
	}

	position& operator-=(const position& v) {
		x -= v.x; y -= v.y; return *this;
	}

	position& operator*=(float v) {
		x *= v; y *= v; return *this;
	}

	position& operator*=(const position& v) {
		x += v.x; y += v.y; return *this;
	}

	position& operator/=(float v) {
		x /= v; y /= v; return *this;
	}

	position& operator/=(const position& v) {
		x /= v.x; y /= v.y; return *this;
	}

	position& move(float ax, float ay) {
		x += ax; y += ay; return *this;
	}

	bool operator==(const position& v) const {
		return x == v.x && y == v.y;
	}

	bool operator!=(const position& v) const {
		return x != v.x || y != v.y;
	}

	float x = 0, y = 0;
};

position g_screen_size;
static std::vector<geometry> g_geometry;
static position g_circle_24[24];

struct rectangle
{
	rectangle() = default;
	rectangle(const position& p, const position& s) :
		x(p.x), y(p.y), w(s.x), h(s.y) {}

	rectangle(float x, float y, float w, float h) :
		x(x), y(y), w(w), h(h) {}

	[[nodiscard]] position center() const {
		return { x + w / 2, y + h / 2 };
	}
	[[nodiscard]] position lt() const {
		return { x, y };
	}
	[[nodiscard]] position rt() const {
		return { x + w, y };
	}
	[[nodiscard]] position lb() const {
		return { x, y + h };
	}
	[[nodiscard]] position rb() const {
		return { x + w, y + h };
	}
	[[nodiscard]] rectangle operator+(const rectangle& v) const {
		return rectangle(x + v.x, y + v.y, w + v.w, h + v.h);
	}
	[[nodiscard]] rectangle operator-(const rectangle& v) const {
		return rectangle(x - v.x, y - v.y, w - v.w, h - v.h);
	}
	[[nodiscard]] rectangle intersect(const rectangle& v) const {
		auto new_x = std::max(x, v.x);
		auto new_y = std::max(y, v.y);
		auto new_w = std::max(.0f, std::min(x + w, v.x + v.w) - new_x);
		auto new_h = std::max(.0f, std::min(y + h, v.y + v.h) - new_y);
		return { new_x, new_y, new_w, new_h };
	}

	float x = 0, y = 0, w = 0, h = 0;
};

extern position g_screen_size;


static inline void arc_fast(position* path, uint8_t& i, position center, float radius, int min, int max)
{
	for (int a = min; a <= max; a++)
	{
		const auto& c = g_circle_24[a % 24];
		path[i++] = { center.x + c.x * radius, center.y + c.y * radius };
	}
}

static inline void draw_rounded_rect(position* path, const rectangle& rect, float rounding)
{
	rounding = std::min(rounding, fabsf(rect.w) * .5f - 1.0f);
	rounding = std::min(rounding, fabsf(rect.h) * .5f - 1.0f);

	uint8_t i = 0;

	auto a = position(rect.x, rect.y) + .5f;
	auto b = position(rect.x + rect.w, rect.y + rect.h) - .5f;

	arc_fast(path, i, { a + rounding }, rounding, 12, 18);
	arc_fast(path, i, { b.x - rounding, a.y + rounding }, rounding, 18, 24);
	arc_fast(path, i, { b - rounding }, rounding, 0, 6);
	arc_fast(path, i, { a.x + rounding, b.y - rounding }, rounding, 6, 12);
}

struct vertex
{
	vertex() = default;
	vertex(float x, float y, const color& col) :
		x(x), y(y), col(col) {}

	vertex(float x, float y, const color& col, float u, float v) :
		x(x), y(y), u(u), v(v), col(col) {}

	float x = .0f, y = .0f;
	float u = .0f, v = .0f;
	color col;
};

// ReSharper disable All
template<typename T>
class ImVector
{
	size_t		size_;
	size_t		capacity_;
	T* data_;

	typedef T                   value_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

public:

	ImVector() { size_ = capacity_ = 0; data_ = NULL; }
	~ImVector() { if (data_) free(data_); }
	ImVector(const ImVector<T>& src) { size_ = capacity_ = 0; data_ = NULL; operator=(src); }
	ImVector<T>& operator=(const ImVector<T>& src) { clear(); resize(src.size_); memcpy(data_, src.data_, size_ * sizeof(value_type)); return *this; }

	bool                 empty() const { return size_ == 0; }
	size_t               size() const { return size_; }
	size_t               capacity() const { return capacity_; }
	const value_type* data() const { return data_; }
	value_type* data() { return data_; }
	value_type& operator[](size_t i) { assert(i < size_); return data_[i]; }
	const value_type& operator[](size_t i) const { assert(i < size_); return data_[i]; }

	void                 clear() { if (data_) { size_ = capacity_ = 0; free(data_); data_ = NULL; } }
	iterator             begin() { return data_; }
	const_iterator       begin() const { return data_; }
	iterator             end() { return data_ + size_; }
	const_iterator       end() const { return data_ + size_; }
	value_type& front() { assert(size_ > 0); return data_[0]; }
	const value_type& front() const { assert(size_ > 0); return data_[0]; }
	value_type& back() { assert(size_ > 0); return data_[size_ - 1]; }
	const value_type& back() const { assert(size_ > 0); return data_[size_ - 1]; }
	void                 swap(ImVector<value_type>& rhs) { size_t rhs_size = rhs.size_; rhs.size_ = size_; size_ = rhs_size; size_t rhs_cap = rhs.capacity_; rhs.capacity_ = capacity_; capacity_ = rhs_cap; value_type* rhs_data = rhs.data_; rhs.data_ = data_; data_ = rhs_data; }

	size_t          _grow_capacity(size_t sz) const { size_t new_capacity = capacity_ ? (capacity_ + capacity_ / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
	void         resize(size_t new_size) { if (new_size > capacity_) reserve(_grow_capacity(new_size)); size_ = new_size; }
	void         resize(size_t new_size, const value_type& v) { if (new_size > capacity_) reserve(_grow_capacity(new_size)); if (new_size > size_) for (size_t n = size_; n < new_size; n++) memcpy(&data_[n], &v, sizeof(v)); size_ = new_size; }
	void         reserve(size_t new_capacity)
	{
		if (new_capacity <= capacity_)
			return;
		value_type* new_data = (value_type*)malloc((size_t)new_capacity * sizeof(value_type));
		if (data_)
		{
			memcpy(new_data, data_, (size_t)size_ * sizeof(value_type));
			free(data_);
		}
		data_ = new_data;
		capacity_ = new_capacity;
	}

	// NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data_ itself! e.g. v.push_back(v[10]) is forbidden.
	void         push_back(const value_type& v) { if (size_ == capacity_) reserve(_grow_capacity(size_ + 1)); memcpy(&data_[size_], &v, sizeof(v)); size_++; }
	void         pop_back() { assert(size_ > 0); size_--; }
	void         push_front(const value_type& v) { if (size_ == 0) push_back(v); else insert(data_, v); }
	iterator     erase(const_iterator it) { assert(it >= data_ && it < data_ + size_); const ptrdiff_t off = it - data_; memmove(data_ + off, data_ + off + 1, ((size_t)size_ - (size_t)off - 1) * sizeof(value_type)); size_--; return data_ + off; }
	iterator     erase(const_iterator it, const_iterator it_last) { assert(it >= data_ && it < data_ + size_ && it_last > it && it_last <= data_ + size_); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - data_; memmove(data_ + off, data_ + off + count, ((size_t)size_ - (size_t)off - count) * sizeof(value_type)); size_ -= (size_t)count; return data_ + off; }
	iterator     erase_unsorted(const_iterator it) { assert(it >= data_ && it < data_ + size_);  const ptrdiff_t off = it - data_; if (it < data_ + size_ - 1) memcpy(data_ + off, data_ + size_ - 1, sizeof(value_type)); size_--; return data_ + off; }
	iterator     insert(const_iterator it, const value_type& v) { assert(it >= data_ && it <= data_ + size_); const ptrdiff_t off = it - data_; if (size_ == capacity_) reserve(_grow_capacity(size_ + 1)); if (off < (size_t)size_) memmove(data_ + off + 1, data_ + off, ((size_t)size_ - (size_t)off) * sizeof(value_type)); memcpy(&data_[off], &v, sizeof(v)); size_++; return data_ + off; }
	bool         contains(const value_type& v) const { const T* dataa = data_;  const T* data_end = dataa + size_; while (dataa < data_end) if (*dataa++ == v) return true; return false; }
	size_t       index_from_pointer(const_iterator it) const { assert(it >= data_ && it <= data_ + size_); const ptrdiff_t off = it - data_; return (size_t)off; }
	template <typename ...Args> decltype(auto) emplace_back(Args&&... v) { if (size_ == capacity_)reserve(_grow_capacity(size_ + 1)); ::new(end()) T(std::forward<Args>(v)...); size_++; return back(); }
};
// ReSharper restore All

struct drawcmd
{
	drawcmd() :
		indices(0), texture(NULL) { }

	drawcmd(const rectangle& clip_rect, void* texture) :
		indices(0), clip_rect(clip_rect), texture(texture) {}

	size_t indices;
	rectangle clip_rect;
	void* texture;
};

inline position get_screen_size() {
	extern HWND g_hwnd;
	RECT rect;
	GetClientRect(g_hwnd, &rect);
	return { static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top) };
}

struct geometry
{
	explicit geometry(rectangle* clip_rect = NULL, void* texture = NULL) : idx_write_ptr(NULL), vtx_write_ptr(NULL) {
		if (clip_rect)
			clip_rects.push_back(*clip_rect);
		else
			clip_rects.push_back(rectangle(0, 0, g_screen_size.x, g_screen_size.y));

		textures.push_back(texture);
		push_draw_cmd();

	}

	void make_circle_path(position* path, const position& center, float radius, int segments)
	{
		const float a_max = M_PI * 2.0f * static_cast<float>(segments - 1) / static_cast<float>(segments);
		radius -= 0.5f;
		for (int i = 0; i <= segments; i++)
		{
			const float a = (static_cast<float>(i) / static_cast<float>(segments)) * a_max;
			path[i] = position(center.x + cosf(a) * radius, center.y + sinf(a) * radius);
		}
	}

	void rect(rectangle rect, const color& clr, float thickness)
	{
		position rect_path[] = { {rect.x, rect.y}, {rect.x + rect.w, rect.y}, {rect.x + rect.w, rect.y + rect.h}, {rect.x, rect.y + rect.h} };
		polyline(rect_path, 4, clr, thickness, true);
	}

	void filled_rect(const rectangle& rect, const color& clr)
	{
		auto idx = reserve(4, 6);

		vtx_write_ptr[0].x = rect.x;			vtx_write_ptr[0].y = rect.y;			vtx_write_ptr[0].u = .0f;	vtx_write_ptr[0].v = .0f;	vtx_write_ptr[0].col = clr;
		vtx_write_ptr[1].x = rect.x + rect.w;	vtx_write_ptr[1].y = rect.y;			vtx_write_ptr[1].u = .0f;	vtx_write_ptr[1].v = .0f;	vtx_write_ptr[1].col = clr;
		vtx_write_ptr[2].x = rect.x;			vtx_write_ptr[2].y = rect.y + rect.h;	vtx_write_ptr[2].u = .0f;	vtx_write_ptr[2].v = .0f;	vtx_write_ptr[2].col = clr;
		vtx_write_ptr[3].x = rect.x + rect.w;	vtx_write_ptr[3].y = rect.y + rect.h;	vtx_write_ptr[3].u = .0f;	vtx_write_ptr[3].v = .0f;	vtx_write_ptr[3].col = clr;

		idx_write_ptr[0] = static_cast<index>(idx);
		idx_write_ptr[1] = static_cast<index>(idx + 1);
		idx_write_ptr[2] = static_cast<index>(idx + 2);
		idx_write_ptr[3] = static_cast<index>(idx + 3);
		idx_write_ptr[4] = static_cast<index>(idx + 2);
		idx_write_ptr[5] = static_cast<index>(idx + 1);
	}

	void special_rect(const rectangle& rect, const rectangle& uv, const color& clr, float rot_deg)
	{
		auto idx = reserve(5, 12);

		auto center = rect.center();
		auto uv_center = uv.center();
		auto hw = rect.w / 2.f;
		auto hh = rect.h / 2.f;

		rot_deg += 45.f;

		auto rad = rot_deg * M_PI / 180.f;

		auto s = sinf(rad);
		auto c = cosf(rad);

		vtx_write_ptr[0].x = center.x;			vtx_write_ptr[0].y = center.y;			vtx_write_ptr[0].u = uv_center.x;	vtx_write_ptr[0].v = uv_center.y;	vtx_write_ptr[0].col = clr;
		vtx_write_ptr[1].x = center.x - hw * c;	vtx_write_ptr[1].y = center.y - hh * s;	vtx_write_ptr[1].u = uv.x;			vtx_write_ptr[1].v = uv.y;			vtx_write_ptr[1].col = clr; // lt
		vtx_write_ptr[2].x = center.x + hw * s;	vtx_write_ptr[2].y = center.y - hh * c;	vtx_write_ptr[2].u = uv.x + uv.w;	vtx_write_ptr[2].v = uv.y;			vtx_write_ptr[2].col = clr; // rt
		vtx_write_ptr[3].x = center.x - hw * s;	vtx_write_ptr[3].y = center.y + hh * c;	vtx_write_ptr[3].u = uv.x;			vtx_write_ptr[2].v = uv.y + uv.h;	vtx_write_ptr[3].col = clr; // lb
		vtx_write_ptr[4].x = center.x + hw * c;	vtx_write_ptr[4].y = center.y + hh * s;	vtx_write_ptr[4].u = uv.x + uv.w;	vtx_write_ptr[3].v = uv.y + uv.h;	vtx_write_ptr[4].col = clr; // rb

		idx_write_ptr[0] = static_cast<index>(idx); // top
		idx_write_ptr[1] = static_cast<index>(idx + 1);
		idx_write_ptr[2] = static_cast<index>(idx + 2);

		idx_write_ptr[3] = static_cast<index>(idx); // left
		idx_write_ptr[4] = static_cast<index>(idx + 1);
		idx_write_ptr[5] = static_cast<index>(idx + 3);

		idx_write_ptr[6] = static_cast<index>(idx); // right
		idx_write_ptr[7] = static_cast<index>(idx + 2);
		idx_write_ptr[8] = static_cast<index>(idx + 4);

		idx_write_ptr[9] = static_cast<index>(idx); // bot
		idx_write_ptr[10] = static_cast<index>(idx + 3);
		idx_write_ptr[11] = static_cast<index>(idx + 4);
	}

	void gradient(const rectangle& rect, const color& clr, const color& clr2, bool horizontal)
	{
		auto idx = reserve(4, 6);

		vtx_write_ptr[0].x = rect.x;			vtx_write_ptr[0].y = rect.y;			vtx_write_ptr[0].u = .0f;	vtx_write_ptr[0].v = .0f;	vtx_write_ptr[0].col = clr;
		vtx_write_ptr[1].x = rect.x + rect.w;	vtx_write_ptr[1].y = rect.y;			vtx_write_ptr[1].u = .0f;	vtx_write_ptr[1].v = .0f;	vtx_write_ptr[1].col = horizontal ? clr2 : clr;
		vtx_write_ptr[2].x = rect.x;			vtx_write_ptr[2].y = rect.y + rect.h;	vtx_write_ptr[2].u = .0f;	vtx_write_ptr[2].v = .0f;	vtx_write_ptr[2].col = horizontal ? clr : clr2;
		vtx_write_ptr[3].x = rect.x + rect.w;	vtx_write_ptr[3].y = rect.y + rect.h;	vtx_write_ptr[3].u = .0f;	vtx_write_ptr[3].v = .0f;	vtx_write_ptr[3].col = clr2;

		idx_write_ptr[0] = static_cast<index>(idx);
		idx_write_ptr[1] = static_cast<index>(idx + 1);
		idx_write_ptr[2] = static_cast<index>(idx + 2);
		idx_write_ptr[3] = static_cast<index>(idx + 3);
		idx_write_ptr[4] = static_cast<index>(idx + 2);
		idx_write_ptr[5] = static_cast<index>(idx + 1);
	}

	void rounded_rect(rectangle rect, const color& clr, float rounding, float thickness = 1.f)
	{
		//rect.x += thickness / 2.f; rect.y += thickness / 2.f; rect.h -= thickness / 2.f; rect.w -= thickness / 2.f;
		position path[28];
		draw_rounded_rect(path, rect, rounding);
		polyline(path, 28, clr, thickness, true);
	}

	void filled_rounded_rect(const rectangle& rect, const color& clr, float rounding)
	{
		position path[28];
		draw_rounded_rect(path, rect, rounding);
		convex(path, 28, clr);
	}
	void line(const position& p1, const position& p2, const color& clr, float thickness = 1.f) {
		position pts[2] = { p1 + .5f, p2 + .5f };
		polyline(pts, 2, clr, thickness);
	}


	void polyline(const position* points, int points_count, const color& clr, float thickness = 1, bool closed = false)
	{
		if (points_count < 2)
			return;

		int count = points_count;
		if (!closed)
			count = points_count - 1;

		const auto thick_line = thickness > 1.0f;

		auto clr_trans = clr;
		clr_trans.a = 0;

		const int idx_count = thick_line ? count * 18 : count * 12;
		const int vtx_count = thick_line ? points_count * 4 : points_count * 3;

		auto idx = reserve(vtx_count, idx_count);

		auto temp_normals = static_cast<position*>(alloca(points_count * (thick_line ? 5 : 3) * sizeof(position)));
		auto temp_points = temp_normals + points_count;

		for (int i1 = 0; i1 < count; i1++)
		{
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			position diff = points[i2] - points[i1];
			diff *= diff.inv_len();
			temp_normals[i1].x = diff.y;
			temp_normals[i1].y = -diff.x;
		}
		if (!closed)
			temp_normals[points_count - 1] = temp_normals[points_count - 2];

		if (!thick_line)
		{
			if (!closed)
			{
				temp_points[0] = points[0] + temp_normals[0];
				temp_points[1] = points[0] - temp_normals[0];
				temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1];
				temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1];
			}

			auto idx1 = idx;
			for (int i1 = 0; i1 < count; i1++)
			{
				const auto i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				const auto idx2 = (i1 + 1) == points_count ? idx : idx1 + 3;

				auto dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				auto dmr2 = dm.x * dm.x + dm.y * dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				temp_points[i2 * 2 + 0] = points[i2] + dm;
				temp_points[i2 * 2 + 1] = points[i2] - dm;

				idx_write_ptr[0] = static_cast<index>(idx2 + 0); idx_write_ptr[1] = static_cast<index>(idx1 + 0); idx_write_ptr[2] = static_cast<index>(idx1 + 2);
				idx_write_ptr[3] = static_cast<index>(idx1 + 2); idx_write_ptr[4] = static_cast<index>(idx2 + 2); idx_write_ptr[5] = static_cast<index>(idx2 + 0);
				idx_write_ptr[6] = static_cast<index>(idx2 + 1); idx_write_ptr[7] = static_cast<index>(idx1 + 1); idx_write_ptr[8] = static_cast<index>(idx1 + 0);
				idx_write_ptr[9] = static_cast<index>(idx1 + 0); idx_write_ptr[10] = static_cast<index>(idx2 + 0); idx_write_ptr[11] = static_cast<index>(idx2 + 1);
				idx_write_ptr += 12;

				idx1 = idx2;
			}

			for (int i = 0; i < points_count; i++)
			{
				vtx_write_ptr[0].x = points[i].x;				vtx_write_ptr[0].y = points[i].y;				vtx_write_ptr[0].col = clr;			vtx_write_ptr[0].u = vtx_write_ptr[0].v = .0f;
				vtx_write_ptr[1].x = temp_points[i * 2 + 0].x;  vtx_write_ptr[1].y = temp_points[i * 2 + 0].y;	vtx_write_ptr[1].col = clr_trans;	vtx_write_ptr[1].u = vtx_write_ptr[1].v = .0f;
				vtx_write_ptr[2].x = temp_points[i * 2 + 1].x;  vtx_write_ptr[2].y = temp_points[i * 2 + 1].y;	vtx_write_ptr[2].col = clr_trans;	vtx_write_ptr[2].u = vtx_write_ptr[2].v = .0f;
				vtx_write_ptr += 3;
			}
		}
		else
		{
			const float half_inner_thickness = (thickness - 1) * 0.5f;
			if (!closed)
			{
				temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + 1);
				temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
				temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
				temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + 1);
				temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + 1);
				temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + 1);
			}

			auto idx1 = idx;
			for (int i1 = 0; i1 < count; i1++)
			{
				const auto i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				const auto idx2 = (i1 + 1) == points_count ? idx : idx1 + 4;

				position dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				float dmr2 = dm.x * dm.x + dm.y * dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				position dm_out = dm * (half_inner_thickness + 1);
				position dm_in = dm * half_inner_thickness;
				temp_points[i2 * 4 + 0] = points[i2] + dm_out;
				temp_points[i2 * 4 + 1] = points[i2] + dm_in;
				temp_points[i2 * 4 + 2] = points[i2] - dm_in;
				temp_points[i2 * 4 + 3] = points[i2] - dm_out;

				idx_write_ptr[0] = static_cast<index>(idx2 + 1); idx_write_ptr[1] = static_cast<index>(idx1 + 1); idx_write_ptr[2] = static_cast<index>(idx1 + 2);
				idx_write_ptr[3] = static_cast<index>(idx1 + 2); idx_write_ptr[4] = static_cast<index>(idx2 + 2); idx_write_ptr[5] = static_cast<index>(idx2 + 1);
				idx_write_ptr[6] = static_cast<index>(idx2 + 1); idx_write_ptr[7] = static_cast<index>(idx1 + 1); idx_write_ptr[8] = static_cast<index>(idx1 + 0);
				idx_write_ptr[9] = static_cast<index>(idx1 + 0); idx_write_ptr[10] = static_cast<index>(idx2 + 0); idx_write_ptr[11] = static_cast<index>(idx2 + 1);
				idx_write_ptr[12] = static_cast<index>(idx2 + 2); idx_write_ptr[13] = static_cast<index>(idx1 + 2); idx_write_ptr[14] = static_cast<index>(idx1 + 3);
				idx_write_ptr[15] = static_cast<index>(idx1 + 3); idx_write_ptr[16] = static_cast<index>(idx2 + 3); idx_write_ptr[17] = static_cast<index>(idx2 + 2);
				idx_write_ptr += 18;

				idx1 = idx2;
			}

			for (int i = 0; i < points_count; i++)
			{
				vtx_write_ptr[0].x = temp_points[i * 4 + 0].x; vtx_write_ptr[0].y = temp_points[i * 4 + 0].y; vtx_write_ptr[0].col = clr_trans;	vtx_write_ptr[0].u = vtx_write_ptr[0].v = .0f;
				vtx_write_ptr[1].x = temp_points[i * 4 + 1].x; vtx_write_ptr[1].y = temp_points[i * 4 + 1].y; vtx_write_ptr[1].col = clr;		vtx_write_ptr[1].u = vtx_write_ptr[1].v = .0f;
				vtx_write_ptr[2].x = temp_points[i * 4 + 2].x; vtx_write_ptr[2].y = temp_points[i * 4 + 2].y; vtx_write_ptr[2].col = clr;		vtx_write_ptr[2].u = vtx_write_ptr[2].v = .0f;
				vtx_write_ptr[3].x = temp_points[i * 4 + 3].x; vtx_write_ptr[3].y = temp_points[i * 4 + 3].y; vtx_write_ptr[3].col = clr_trans;	vtx_write_ptr[3].u = vtx_write_ptr[3].v = .0f;
				vtx_write_ptr += 4;
			}
		}
		/*
		if (points_count < 2)
			return;

		int count = points_count;
		if (!closed)
			count = points_count - 1;

		auto idx = reserve(count * 4, count * 6);

		for (int i = 0; i < count; i++)
		{
			const int i2 = (i + 1) == points_count ? 0 : i+1;
			const position& p1 = points[i];
			const position& p2 = points[i2];
			position diff = { p2.x - p1.x, p2.y - p1.y };
			auto d = diff.x * diff.x + diff.y * diff.y;
			float inv_len = 1.f;
			if (d > 0.f)
				inv_len = 1.f / sqrtf(d);
			diff.x *= inv_len; diff.y *= inv_len;

			const float dx = diff.x * (thickness * .5f);
			const float dy = diff.y * (thickness * .5f);
			vtx_write_ptr[0].x = p1.x + dy; vtx_write_ptr[0].y = p1.y - dx; vtx_write_ptr[0].u = vtx_write_ptr[0].v = .0f; vtx_write_ptr[0].col = clr;
			vtx_write_ptr[1].x = p2.x + dy; vtx_write_ptr[1].y = p2.y - dx; vtx_write_ptr[1].u = vtx_write_ptr[1].v = .0f; vtx_write_ptr[1].col = clr;
			vtx_write_ptr[2].x = p2.x - dy; vtx_write_ptr[2].y = p2.y + dx; vtx_write_ptr[2].u = vtx_write_ptr[2].v = .0f; vtx_write_ptr[2].col = clr;
			vtx_write_ptr[3].x = p1.x - dy; vtx_write_ptr[3].y = p1.y + dx; vtx_write_ptr[3].u = vtx_write_ptr[3].v = .0f; vtx_write_ptr[3].col = clr;
			vtx_write_ptr += 4;

			idx_write_ptr[0] = static_cast<index>(idx); idx_write_ptr[1] = static_cast<index>(idx + 1); idx_write_ptr[2] = static_cast<index>(idx + 2);
			idx_write_ptr[3] = static_cast<index>(idx); idx_write_ptr[4] = static_cast<index>(idx + 2); idx_write_ptr[5] = static_cast<index>(idx + 3);
			idx_write_ptr += 6;
		}
		*/
	}

	void convex(const position* points, int points_count, const color& clr)
	{
		auto clr_trans = clr;
		clr_trans.a = 0;

		auto vtx_count = (points_count * 2);
		auto idx_count = (points_count - 2) * 3 + points_count * 6;

		auto idx = reserve(vtx_count, idx_count);

		auto vtx_inner_idx = idx;
		auto vtx_outer_idx = idx + 1;
		for (int i = 2; i < points_count; i++)
		{
			idx_write_ptr[0] = static_cast<index>(vtx_inner_idx);
			idx_write_ptr[1] = static_cast<index>(vtx_inner_idx + ((i - 1) << 1));
			idx_write_ptr[2] = static_cast<index>(vtx_inner_idx + (i << 1));
			idx_write_ptr += 3;
		}

		auto temp_normals = static_cast<position*>(alloca(points_count * sizeof(position)));
		for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
		{
			const auto& p0 = points[i0];
			const auto& p1 = points[i1];
			auto diff = p1 - p0;
			diff *= diff.inv_len();
			temp_normals[i0].x = diff.y;
			temp_normals[i0].y = -diff.x;
		}

		for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
		{
			const position& n0 = temp_normals[i0];
			const position& n1 = temp_normals[i1];
			position dm = (n0 + n1) * 0.5f;
			float dmr2 = dm.x * dm.x + dm.y * dm.y;
			if (dmr2 > 0.000001f)
			{
				float scale = 1.0f / dmr2;
				if (scale > 100.0f) scale = 100.0f;
				dm *= scale;
			}
			dm *= .5f;

			vtx_write_ptr[0].x = (points[i1] - dm).x; vtx_write_ptr[0].y = (points[i1] - dm).y; vtx_write_ptr[0].col = clr;			vtx_write_ptr[0].u = vtx_write_ptr[0].v = .0f;
			vtx_write_ptr[1].x = (points[i1] + dm).x; vtx_write_ptr[1].y = (points[i1] + dm).y; vtx_write_ptr[1].col = clr_trans;	vtx_write_ptr[0].u = vtx_write_ptr[0].v = .0f;
			vtx_write_ptr += 2;

			idx_write_ptr[0] = static_cast<index>(vtx_inner_idx + (i1 << 1));
			idx_write_ptr[1] = static_cast<index>(vtx_inner_idx + (i0 << 1));
			idx_write_ptr[2] = static_cast<index>(vtx_outer_idx + (i0 << 1));
			idx_write_ptr[3] = static_cast<index>(vtx_outer_idx + (i0 << 1));
			idx_write_ptr[4] = static_cast<index>(vtx_outer_idx + (i1 << 1));
			idx_write_ptr[5] = static_cast<index>(vtx_inner_idx + (i1 << 1));
			idx_write_ptr += 6;
		}
	}

	void circle(position center, float radius, const color& clr, float thickness = 1.f, int segments = 24)
	{
		auto path = static_cast<position*>(alloca((segments + 1) * sizeof(position)));
		make_circle_path(path, center, radius, segments);
		polyline(path, segments + 1, clr, thickness, true);
	}

	void filled_circle(const position& center, float radius, const color& clr, int segments = 24)
	{
		auto path = static_cast<position*>(alloca((segments + 1) * sizeof(position)));
		make_circle_path(path, center, radius, segments);
		convex(path, segments + 1, clr);
	}


	void texture(const rectangle& rect, const rectangle& uv, const color& clr)
	{
		auto idx = reserve(4, 6);

		vtx_write_ptr[0].x = rect.x;			vtx_write_ptr[0].y = rect.y;			vtx_write_ptr[0].u = uv.x;			vtx_write_ptr[0].v = uv.y;			vtx_write_ptr[0].col = clr;
		vtx_write_ptr[1].x = rect.x + rect.w;	vtx_write_ptr[1].y = rect.y;			vtx_write_ptr[1].u = uv.x + uv.w;	vtx_write_ptr[1].v = uv.y;			vtx_write_ptr[1].col = clr;
		vtx_write_ptr[2].x = rect.x;			vtx_write_ptr[2].y = rect.y + rect.h;	vtx_write_ptr[2].u = uv.x;			vtx_write_ptr[2].v = uv.y + uv.h;	vtx_write_ptr[2].col = clr;
		vtx_write_ptr[3].x = rect.x + rect.w;	vtx_write_ptr[3].y = rect.y + rect.h;	vtx_write_ptr[3].u = uv.x + uv.w;	vtx_write_ptr[3].v = uv.y + uv.h;	vtx_write_ptr[3].col = clr;

		idx_write_ptr[0] = static_cast<index>(idx);
		idx_write_ptr[1] = static_cast<index>(idx + 1);
		idx_write_ptr[2] = static_cast<index>(idx + 2);
		idx_write_ptr[3] = static_cast<index>(idx + 3);
		idx_write_ptr[4] = static_cast<index>(idx + 2);
		idx_write_ptr[5] = static_cast<index>(idx + 1);
	}

	void distribute_uv_over(const rectangle& rect, const position& uv1, const position& uv2, vertex* start = NULL, vertex* end = NULL)
	{
		if (!end)
			end = &vertices.back();

		const auto uv_size = uv2 - uv1;
		const auto scale = position(
			rect.w > 0 ? (uv_size.x / rect.w) : 0.0f,
			rect.h > 0 ? (uv_size.y / rect.h) : 0.0f);

		const auto min = uv1.min(uv2);
		const auto max = uv1.max(uv2);

		for (auto v = start; v <= end; ++v)
		{
			const auto p = (position(v->x - rect.x, v->y - rect.y) * scale).clamp(min, max);
			v->u = uv1.x + p.x;
			v->v = uv1.y + p.y;
		}
	}
	void push_draw_cmd() {
		cmds.emplace_back(clip_rects.back(), textures.back());
	}

	void push_intersected_cliprect(const rectangle& rect, bool push_cmd) {

		if (!clip_rects.empty())
			clip_rects.emplace_back(clip_rects.back().intersect(rect));
		else
			clip_rects.emplace_back(rect);

		if (push_cmd)
			push_draw_cmd();
	}

	drawcmd& last_cmd() {
		return cmds.back();
	}

	void cleanup() {
		cmds.clear();
		indices.clear();
		vertices.clear();
		textures.clear();
		clip_rects.clear();
		idx_write_ptr = NULL;
		vtx_write_ptr = NULL;
	}

	void append_valid_parameters() {
		clip_rects.push_back(rectangle(0, 0, g_screen_size.x, g_screen_size.y));
		textures.push_back(NULL);
		push_draw_cmd();
	}

	size_t reserve(size_t v, size_t i) {
		auto vc = vertices.size();
		auto ic = indices.size();
		vertices.resize(vc + v);
		indices.resize(ic + i);
		vtx_write_ptr = vertices.data() + vc;
		idx_write_ptr = indices.data() + ic;
		last_cmd().indices += i;
		return vc;
	}

	void render()
	{
		g_geometry.emplace_back(*this);
	}

	ImVector<vertex> vertices;
	ImVector<index> indices;
	ImVector<drawcmd> cmds;
	ImVector<rectangle> clip_rects;
	ImVector<void*> textures;

	index* idx_write_ptr;
	vertex* vtx_write_ptr;
};

#define TEX11 reinterpret_cast<ID3D11ShaderResourceView*>(this->t)

struct texture
{
	texture(const void* rgba, int w, int h) { create(rgba, w, h); }
	~texture() { release(); }

	void create(const void* rgba, int w_, int h_)
	{
		t = make_d3d11_texture(rgba, w_, h_);
		w = w_;
		h = h_;
	}

	void resize(int w_, int h_, const color& space)
	{
		if (!t)
			return;

		ID3D11Resource* t1 = NULL;
		TEX11->GetResource(&t1);

		if (!t1)
			return;

		std::vector<uint32_t> rgba;
		rgba.resize((size_t)w_ * h_);
		for (auto& v : rgba)
			v = space;

		auto old = TEX11;
		t = make_d3d11_texture(rgba.data(), w_, h_);

		ID3D11Resource* t2 = NULL;
		TEX11->GetResource(&t2);

		if (!t2)
			return;

		D3D11_BOX box;
		box.back = 1;
		box.front = 0;
		box.left = 0;
		box.top = 0;
		box.right = w;
		box.bottom = h;

		g_context->CopySubresourceRegion(t2, 0, 0, 0, 0, t1, 0, &box);

		w = w_;
		h = h_;

		t1->Release();
		t2->Release();
		old->Release();
		return;

	}

	void update(const void* rgba, int x, int y, int w, int h)
	{
		ID3D11Resource* t = NULL;
		TEX11->GetResource(&t);

		if (!t)
			return;

		D3D11_BOX box;
		box.back = 1;
		box.front = 0;
		box.left = x;
		box.top = y;
		box.right = x + w;
		box.bottom = y + h;
		g_context->UpdateSubresource(t, 0, &box, rgba, w * 4, w * h * 4);

		t->Release();
		return;
	}

	void release()
	{
		if (t)
		{
			TEX11->Release();
			t = NULL;
		}
	}


	void draw(geometry& g, const rectangle& rect, const rectangle& uv = { 0, 0, 1, 1 }, const color& clr = { 255, 255 }) const {
		g.textures.push_back(t);
		g.push_draw_cmd();
		g.texture(rect, uv, clr);
		g.textures.pop_back();
		g.push_draw_cmd();
	}

	int w = 0, h = 0;
	void* t = NULL;
};

#undef TEX11


enum : std::uint8_t
{
	FONT_DEFAULT = 0 << 0,
	FONT_BOLD = 1 << 0,
	FONT_ITALIC = 1 << 1
};

enum : std::uint8_t
{
	TEXT_LEFT = 0 << 0,
	TEXT_RIGHT = 1 << 1,
	TEXT_CENTERED_X = 1 << 2,
	TEXT_CENTERED_Y = 1 << 3,
	TEXT_CENTERED = 1 << 2 | 1 << 3,
	TEXT_OUTLINE = 1 << 4,
	TEXT_COLORTAGS = 1 << 5
};

class font
{
public:
	~font()
	{
		if (tex_)
			static_cast<ID3D11ShaderResourceView*>(tex_)->Release();
	}

	bool create(const std::string& family, long height, std::uint8_t flags)
	{
		HGDIOBJ gdi_font = nullptr;
		HGDIOBJ prev_gdi_font = nullptr;

		auto dc = CreateCompatibleDC(nullptr);
		SetMapMode(dc, MM_TEXT);

		auto create_gdi_font = [&]()
		{
			const auto character_height = -MulDiv(height, static_cast<int>(static_cast<float>(GetDeviceCaps(dc, LOGPIXELSY)) * text_scale_), 72);

			const DWORD bold = (flags & FONT_BOLD) ? FW_BOLD : FW_NORMAL;
			const DWORD italic = (flags & FONT_ITALIC) ? TRUE : FALSE;

			gdi_font = CreateFontA(character_height, 0, 0, 0, bold, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, family.c_str());
		};

		text_scale_ = 1.0f;

		create_gdi_font();

		if (!gdi_font) {
			return false;
		}

		prev_gdi_font = SelectObject(dc, gdi_font);

		tex_width_ = tex_height_ = 64;

		HRESULT hr = S_OK;
		while (ERROR_MORE_DATA == (hr = paint_alphabet(dc, true)))
		{
			tex_width_ *= 2;
			tex_height_ *= 2;
		}

		if (FAILED(hr))
			return false;

		if (tex_width_ > static_cast<long>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION))
		{
			text_scale_ = static_cast<float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) / static_cast<float>(tex_width_);
			tex_width_ = tex_height_ = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;

			auto first_iteration = true;

			do
			{
				if (!first_iteration)
					text_scale_ *= 0.9f;

				DeleteObject(SelectObject(dc, prev_gdi_font));

				create_gdi_font();

				if (!gdi_font) {
					return false;
				}

				prev_gdi_font = SelectObject(dc, gdi_font);

				first_iteration = false;
			} while (ERROR_MORE_DATA == (hr = paint_alphabet(dc, true)));
		}

		DWORD* bitmap_bits;

		BITMAPINFO bitmap_ctx{};
		bitmap_ctx.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_ctx.bmiHeader.biWidth = tex_width_;
		bitmap_ctx.bmiHeader.biHeight = -tex_height_;
		bitmap_ctx.bmiHeader.biPlanes = 1;
		bitmap_ctx.bmiHeader.biCompression = BI_RGB;
		bitmap_ctx.bmiHeader.biBitCount = 32;

		auto bmp = CreateDIBSection(dc, &bitmap_ctx, DIB_RGB_COLORS, reinterpret_cast<void**>(&bitmap_bits), nullptr, 0);

		if (!bmp)
			return false;

		auto prev_bmp = SelectObject(dc, bmp);

		SetTextColor(dc, RGB(255, 255, 255));
		SetBkColor(dc, 0x00000000);
		SetTextAlign(dc, TA_TOP);

		if (FAILED(paint_alphabet(dc, false)))
			return false;

		std::vector<uint32_t> rgba;
		rgba.resize((size_t)tex_width_ * tex_height_);

		auto dst = reinterpret_cast<std::uint32_t*>(rgba.data());

		for (long y = 0; y < tex_height_; y++)
		{
			for (long x = 0; x < tex_width_; x++)
			{
				auto alpha = bitmap_bits[tex_width_ * y + x] & 0xFF;
				if (alpha > 0)
				{
					*dst++ = 0xffffff | alpha << 24;
				}
				else
				{
					*dst++ = 0;
				}
			}
		}

		tex_ = make_d3d11_texture(rgba.data(), tex_width_, tex_height_);

		SelectObject(dc, prev_bmp);
		SelectObject(dc, prev_gdi_font);
		DeleteObject(bmp);
		DeleteObject(gdi_font);
		DeleteDC(dc);

		return true;
	}



	position get_text_extent(std::string_view text, bool cf)
	{
		auto row_width = 0.f;
		const auto row_height = (tex_coords_[0][3] - tex_coords_[0][1]) * static_cast<float>(tex_height_);
		auto width = 0.f;
		auto height = row_height;

		for (auto it = text.begin(); it != text.end(); ++it)
		{
			const auto& c = *it;

			if (c == '\n')
			{
				row_width = 0.f;
				height += row_height;
			}

			if (cf && c == '{')
			{
				std::size_t index = std::distance(text.begin(), it);
				bool alpha = false;
				if (text.length() > index + 11 && text[index + 1] == '#' && ((alpha = text[index + 10] == '}')) || text[index + 8] == '}')
				{
					alpha ? it += 10 : it += 8;
					continue;
				}
			}

			if (c < ' ')
				continue;

			auto tx1 = tex_coords_[c - 32][0];
			auto tx2 = tex_coords_[c - 32][2];

			row_width += (tx2 - tx1) * static_cast<float>(tex_width_) - 2.f * static_cast<float>(spacing_);

			if (row_width > width)
				width = row_width;
		}

		return { width, height };
	}

	void draw_text(geometry* g, position p, std::string_view text, color col, std::uint8_t flags, color outline)
	{
		if (!g)
		{
			g_geometry.emplace_back();
			g = &g_geometry.back();
			g->textures.push_back(tex_);
			g->push_draw_cmd();
		}
		else
		{
			g->textures.push_back(tex_);
			g->push_draw_cmd();
		}


		if (flags & (TEXT_RIGHT | TEXT_CENTERED))
		{
			const auto size = get_text_extent(text, flags & TEXT_COLORTAGS);

			if (flags & TEXT_RIGHT)
				p.x -= size.x;
			else if (flags & TEXT_CENTERED_X)
				p.x -= 0.5f * size.x;

			if (flags & TEXT_CENTERED_Y)
				p.y -= 0.5f * size.y;
		}

		p.x -= static_cast<float>(spacing_);

		p.x = roundf(p.x); p.y = roundf(p.y);

		const auto start_x = p.x;

		size_t rl = 0;

		for (auto it = text.begin(); it != text.end(); ++it)
		{
			const auto& c = *it;
			if (c > ' ' && c < '\x7F')
			{
				if (c == '{' && flags & TEXT_COLORTAGS)
				{
					const std::size_t index = std::distance(text.begin(), it);
					auto alpha = false;
					if (text.length() > index + 11 && text[index + 1] == '#' && ((alpha = text[index + 10] == '}')) || text[index + 8] == '}')
					{
						alpha ? it += 10 : it += 8;
						continue;
					}
				}
				++rl;
			}
		}

		const auto outlined = flags & TEXT_OUTLINE;

		auto idx = outlined ? g->reserve(rl * 20, rl * 30) : g->reserve(rl * 4, rl * 6);

		for (auto it = text.begin(); it != text.end(); ++it)
		{
			const auto& c = *it;

			if (flags & TEXT_COLORTAGS && c == '{') // format: {#aarrggbb} or {##rrggbb}, {#aarrggbb} will inherit alpha from color argument.
			{
				const std::size_t index = std::distance(text.begin(), it);
				bool alpha = false;
				if (text.length() > index + 11 && text[index + 1] == '#' && ((alpha = text[index + 10] == '}')) || text[index + 8] == '}')
				{
					std::string_view color_str = text.substr(index + 2, alpha ? 8 : 6);
					if (std::find_if(color_str.begin(), color_str.end(), [](char c)->bool {return !std::isxdigit(c); }) != color_str.end())
						continue;

					col = std::stoul(alpha ? static_cast<std::string>(color_str) : "ff" + static_cast<std::string>(color_str), nullptr, 16);
					alpha ? it += 10 : it += 8;
					continue;
				}
			}

			if (c == '\n')
			{
				p.x = start_x;
				p.y += (tex_coords_[0][3] - tex_coords_[0][1]) * static_cast<float>(tex_height_);
			}

			if (c < ' ')
				continue;

			float tx1 = tex_coords_[c - 32][0];
			float ty1 = tex_coords_[c - 32][1];
			float tx2 = tex_coords_[c - 32][2];
			float ty2 = tex_coords_[c - 32][3];

			float w = (tx2 - tx1) * static_cast<float>(tex_width_) / text_scale_;
			float h = (ty2 - ty1) * static_cast<float>(tex_height_) / text_scale_;

			if (c != ' ')
			{
				auto quick_texture = [&](float x, float y, const color& clr)
				{
					auto vtx_write_ptr = g->vtx_write_ptr;
					auto idx_write_ptr = g->idx_write_ptr;

					vtx_write_ptr[0].x = x;			vtx_write_ptr[0].y = y;			vtx_write_ptr[0].u = tx1;	vtx_write_ptr[0].v = ty1;	vtx_write_ptr[0].col = clr;
					vtx_write_ptr[1].x = x + w;		vtx_write_ptr[1].y = y;			vtx_write_ptr[1].u = tx2;	vtx_write_ptr[1].v = ty1;	vtx_write_ptr[1].col = clr;
					vtx_write_ptr[2].x = x;			vtx_write_ptr[2].y = y + h;		vtx_write_ptr[2].u = tx1;	vtx_write_ptr[2].v = ty2;	vtx_write_ptr[2].col = clr;
					vtx_write_ptr[3].x = x + w;		vtx_write_ptr[3].y = y + h;		vtx_write_ptr[3].u = tx2;	vtx_write_ptr[3].v = ty2;	vtx_write_ptr[3].col = clr;

					idx_write_ptr[0] = static_cast<index>(idx);
					idx_write_ptr[1] = static_cast<index>(idx + 1);
					idx_write_ptr[2] = static_cast<index>(idx + 2);
					idx_write_ptr[3] = static_cast<index>(idx + 3);
					idx_write_ptr[4] = static_cast<index>(idx + 2);
					idx_write_ptr[5] = static_cast<index>(idx + 1);
					g->vtx_write_ptr += 4;
					g->idx_write_ptr += 6;
					idx += 4;
				};

				if (outlined)
				{
					quick_texture(p.x + 1, p.y, outline);
					quick_texture(p.x - 1, p.y, outline);
					quick_texture(p.x, p.y + 1, outline);
					quick_texture(p.x, p.y - 1, outline);
				}

				quick_texture(p.x, p.y, col);
			}

			p.x += w - (static_cast<float>(spacing_) * 2.f);
		}
		g->textures.pop_back();
		g->push_draw_cmd();
	}

private:
	void* tex_ = NULL;
	long					tex_width_ = 0;
	long					tex_height_ = 0;
	float					text_scale_ = 0;
	float					tex_coords_[128 - 32][4] = { {0} };
	long					spacing_ = 0;

	HRESULT paint_alphabet(HDC ctx, bool measure_only)
	{
		SIZE size;
		char chr[2] = "x";

		if (0 == GetTextExtentPoint32A(ctx, chr, 1, &size))
			return E_FAIL;

		spacing_ = static_cast<long>(ceil(static_cast<double>(size.cy) * 0.3));

		long x = spacing_;
		long y = 0;

		for (char c = 32; c < 127; c++)
		{
			chr[0] = c;
			if (0 == GetTextExtentPoint32A(ctx, chr, 1, &size))
				return E_FAIL;

			if (x + size.cx + spacing_ > tex_width_)
			{
				x = spacing_;
				y += size.cy + 1;
			}

			if (y + size.cy > tex_height_)
				return ERROR_MORE_DATA;

			if (!measure_only)
			{
				if (0 == ExtTextOutA(ctx, x + 0, y + 0, ETO_OPAQUE, nullptr, chr, 1, nullptr))
					return E_FAIL;

				tex_coords_[c - 32][0] = (static_cast<float>(x + 0 - spacing_)) / static_cast<float>(tex_width_);
				tex_coords_[c - 32][1] = (static_cast<float>(y + 0 + 0)) / static_cast<float>(tex_height_);
				tex_coords_[c - 32][2] = (static_cast<float>(x + size.cx + spacing_)) / static_cast<float>(tex_width_);
				tex_coords_[c - 32][3] = (static_cast<float>(y + size.cy + 0)) / static_cast<float>(tex_height_);
			}

			x += size.cx + (2 * spacing_);
		}

		return S_OK;
	}


};

static void renderer_release_resources()
{
	if (!g_device)
		return;

	if (g_fontsampler) { g_fontsampler->Release(); g_fontsampler = NULL; }
	if (g_ib) { g_ib->Release(); g_ib = NULL; }
	if (g_vb) { g_vb->Release(); g_vb = NULL; }

	if (g_blendstate) { g_blendstate->Release(); g_blendstate = NULL; }
	if (g_depthstate) { g_depthstate->Release(); g_depthstate = NULL; }
	if (g_rasterizer) { g_rasterizer->Release(); g_rasterizer = NULL; }
	if (g_pshader) { g_pshader->Release(); g_pshader = NULL; }
	if (g_vshader) { g_vshader->Release(); g_vshader = NULL; }
	if (g_vcbuff) { g_vcbuff->Release(); g_vcbuff = NULL; }
	if (g_inputlayout) { g_inputlayout->Release(); g_inputlayout = NULL; }
	if (g_default_texture) { g_default_texture->Release(); g_default_texture = NULL; }
}

static void renderer_recreate()
{
	if (!g_device)
		return;

	if (g_fontsampler)
		renderer_release_resources();

	// By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
	// If you would like to use this DX11 sample code but remove this dependency you can:
	//  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
	//  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
	// See https://github.com/ocornut/imgui/pull/638 for sources and details.

	// Create the vertex shader
	{
		static const char* vertexShader =
			"cbuffer vertexBuffer : register(b0) \
            {\
              float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
              float2 pos : POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
              PS_INPUT output;\
              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
              output.col = input.col;\
              output.uv  = input.uv;\
              return output;\
            }";

		ID3DBlob* vertexShaderBlob;
		if (FAILED(D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &vertexShaderBlob, NULL)))
			return; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
		if (g_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &g_vshader) != S_OK)
		{
			vertexShaderBlob->Release();
			return;
		}

		// Create the input layout
		D3D11_INPUT_ELEMENT_DESC local_layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(vertex, x), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(vertex, u),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(vertex, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (g_device->CreateInputLayout(local_layout, 3, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_inputlayout) != S_OK)
		{
			vertexShaderBlob->Release();
			return;
		}
		vertexShaderBlob->Release();

		// Create the constant buffer
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			g_device->CreateBuffer(&desc, NULL, &g_vcbuff);
		}
	}

	// Create the pixel shader
	{
		static const char* pixelShader =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";

		ID3DBlob* pixelShaderBlob;
		if (FAILED(D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pixelShaderBlob, NULL)))
			return; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
		if (g_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &g_pshader) != S_OK)
		{
			pixelShaderBlob->Release();
			return;
		}
		pixelShaderBlob->Release();
	}

	// Create the blending setup
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		g_device->CreateBlendState(&desc, &g_blendstate);
	}

	// Create the rasterizer state
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.ScissorEnable = true;
		desc.DepthClipEnable = true;
		g_device->CreateRasterizerState(&desc, &g_rasterizer);
	}

	// Create depth-stencil State
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = false;
		desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace = desc.FrontFace;
		g_device->CreateDepthStencilState(&desc, &g_depthstate);
	}

	{
		uint32_t rgba = 0xFFFFFFFF;
		g_default_texture = make_d3d11_texture(&rgba, 1, 1);
	}

	// Create texture sampler
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0.f;
		desc.MaxLOD = 0.f;
		g_device->CreateSamplerState(&desc, &g_fontsampler);
	}
}

void renderer_setup(void* device, void* device_context)
{
	auto _device = reinterpret_cast<ID3D11Device*>(device);

	IDXGIDevice* pDXGIDevice = NULL;
	IDXGIAdapter* pDXGIAdapter = NULL;
	IDXGIFactory* pFactory = NULL;

	if (_device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)) == S_OK)
		if (pDXGIDevice->GetParent(IID_PPV_ARGS(&pDXGIAdapter)) == S_OK)
			if (pDXGIAdapter->GetParent(IID_PPV_ARGS(&pFactory)) == S_OK)
			{
				g_device = _device;
				g_context = reinterpret_cast<ID3D11DeviceContext*>(device_context);
				g_factory = pFactory;
			}
	if (pDXGIDevice) pDXGIDevice->Release();
	if (pDXGIAdapter) pDXGIAdapter->Release();
	g_device->AddRef();
	g_context->AddRef();

	for (int i = 0; i < 24; i++)
	{
		const float a = (static_cast<float>(i) * 2 * M_PI) / 24.f;
		g_circle_24[i] = position(cosf(a), sinf(a));
	}

	renderer_recreate();
}
void renderer_draw()
{
	if (!g_fontsampler)
		renderer_recreate();

	size_t total_indices = 0;
	size_t total_vertices = 0;

	for (auto& l : g_geometry)
	{
		total_indices += l.indices.size();
		total_vertices += l.vertices.size();
	}

	ID3D11DeviceContext* ctx = g_context;

	// Create and grow vertex/index buffers if needed
	if (!g_vb || g_vb_size < total_vertices)
	{
		if (g_vb) { g_vb->Release(); g_vb = NULL; }
		g_vb_size = total_vertices + 5000;
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = (UINT)g_vb_size * sizeof(vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		if (g_device->CreateBuffer(&desc, NULL, &g_vb) < 0)
			return;
	}
	if (!g_ib || g_ib_size < total_indices)
	{
		if (g_ib) { g_ib->Release(); g_ib = NULL; }
		g_ib_size = total_indices + 10000;
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = (UINT)g_ib_size * sizeof(index);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (g_device->CreateBuffer(&desc, NULL, &g_ib) < 0)
			return;
	}

	// Upload vertex/index data into a single contiguous GPU buffer
	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (ctx->Map(g_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	if (ctx->Map(g_ib, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
		return;
	vertex* vtx_dst = (vertex*)vtx_resource.pData;
	index* idx_dst = (index*)idx_resource.pData;

	for (const auto& set : g_geometry)
	{
		std::memcpy(vtx_dst, set.vertices.data(), set.vertices.size() * sizeof(vertex));
		std::memcpy(idx_dst, set.indices.data(), set.indices.size() * sizeof(index));
		vtx_dst += set.vertices.size();
		idx_dst += set.indices.size();
	}

	ctx->Unmap(g_vb, 0);
	ctx->Unmap(g_ib, 0);

	// Setup orthographic projection matrix into our constant buffer
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (ctx->Map(g_vcbuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
			return;
		VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
		float L = 0;
		float R = g_screen_size.x;
		float T = 0;
		float B = g_screen_size.y;
		float mvp[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		};
		memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
		ctx->Unmap(g_vcbuff, 0);
	}

	// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
	struct BACKUP_DX11_STATE
	{
		UINT                        ScissorRectsCount, ViewportsCount;
		D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		ID3D11RasterizerState* RS;
		ID3D11BlendState* BlendState;
		FLOAT                       BlendFactor[4];
		UINT                        SampleMask;
		UINT                        StencilRef;
		ID3D11DepthStencilState* DepthStencilState;
		ID3D11ShaderResourceView* PSShaderResource;
		ID3D11SamplerState* PSSampler;
		ID3D11PixelShader* PS;
		ID3D11VertexShader* VS;
		ID3D11GeometryShader* GS;
		UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
		ID3D11ClassInstance* PSInstances[256], * VSInstances[256], * GSInstances[256];   // 256 is max according to PSSetShader documentation
		D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
		ID3D11Buffer* IndexBuffer, * VertexBuffer, * VSConstantBuffer;
		UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
		DXGI_FORMAT                 IndexBufferFormat;
		ID3D11InputLayout* InputLayout;
	};
	BACKUP_DX11_STATE old;
	old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
	ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
	ctx->RSGetState(&old.RS);
	ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
	ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
	ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
	ctx->PSGetSamplers(0, 1, &old.PSSampler);
	old.PSInstancesCount = old.VSInstancesCount = old.GSInstancesCount = 256;
	ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
	ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
	ctx->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
	ctx->GSGetShader(&old.GS, old.GSInstances, &old.GSInstancesCount);

	ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
	ctx->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
	ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
	ctx->IAGetInputLayout(&old.InputLayout);

	// Setup desired DX state
	D3D11_VIEWPORT vp;
	memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	vp.Width = g_screen_size.x;
	vp.Height = g_screen_size.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0;
	ctx->RSSetViewports(1, &vp);

	// Setup shader and vertex buffers
	unsigned int stride = sizeof(vertex);
	unsigned int offset = 0;
	ctx->IASetInputLayout(g_inputlayout);
	ctx->IASetVertexBuffers(0, 1, &g_vb, &stride, &offset);
	ctx->IASetIndexBuffer(g_ib, sizeof(index) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->VSSetShader(g_vshader, NULL, 0);
	ctx->VSSetConstantBuffers(0, 1, &g_vcbuff);
	ctx->PSSetShader(g_pshader, NULL, 0);
	ctx->PSSetSamplers(0, 1, &g_fontsampler);
	ctx->GSSetShader(NULL, NULL, 0);
	ctx->HSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
	ctx->DSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
	ctx->CSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..

	// Setup blend state
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	ctx->OMSetBlendState(g_blendstate, blend_factor, 0xffffffff);
	ctx->OMSetDepthStencilState(g_depthstate, 0);
	ctx->RSSetState(g_rasterizer);

	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	size_t idx_pos = 0;
	size_t vtx_pos = 0;

	for (const auto& set : g_geometry)
	{
		if (!set.cmds.size())
			continue;

		for (const auto& cmd : set.cmds)
		{
			if (cmd.indices == 0)
				continue;

			D3D11_RECT rect = {
				(LONG)cmd.clip_rect.x, (LONG)cmd.clip_rect.y, (LONG)(cmd.clip_rect.x + cmd.clip_rect.w), (LONG)(cmd.clip_rect.y + cmd.clip_rect.h)
			};

			ctx->RSSetScissorRects(1, &rect);
			ctx->PSSetShaderResources(0, 1, cmd.texture ? reinterpret_cast<ID3D11ShaderResourceView* const*>(&cmd.texture) : &g_default_texture);
			ctx->DrawIndexed(static_cast<UINT>(cmd.indices), static_cast<UINT>(idx_pos), static_cast<UINT>(vtx_pos));
			idx_pos += cmd.indices;
		}

		vtx_pos += set.vertices.size();
	}

	// Restore modified DX state
	ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
	ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
	ctx->RSSetState(old.RS); if (old.RS) old.RS->Release();
	ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
	ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
	ctx->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
	ctx->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
	ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
	for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
	ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
	ctx->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
	ctx->GSSetShader(old.GS, old.GSInstances, old.GSInstancesCount); if (old.GS) old.GS->Release();
	for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
	ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
	ctx->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	ctx->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();

	g_geometry.clear();
}

void renderer_start_drawing()
{
	auto ss = get_screen_size();
	g_screen_size = { ss.x, ss.y };
}
void renderer_on_device_lost()
{
	renderer_release_resources();
}

void renderer_on_device_reset()
{
	renderer_recreate();
}
