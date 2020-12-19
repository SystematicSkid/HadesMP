#pragma once

namespace engine::misc
{
	struct Color
	{
		// Fields
		unsigned int v = -1;
		char r;
		char g;
		char b;
		char a;

		// Constructors

		// Default white
		Color()
		{
			r = 255;
			g = 255;
			b = 255;
			a = 255;
		}

		// 3 args
		Color(char r, char g, char b) : r(r), g(g), b(b) { a = 255; }
		// 4 args
		Color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) { }
	};
}