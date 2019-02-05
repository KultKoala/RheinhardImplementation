struct RGBA_float{
	float r,g,b,a;

	RGBA_float():r(0),g(0),b(0),a(0){};
	RGBA_float(float red, float green, float blue, float alpha):r(red), g(green), b(blue), a(alpha){};
};

struct RGBA_pixel{
	unsigned char r,g,b,a;

	RGBA_pixel():r(0),g(0),b(0),a(0){};
	RGBA_pixel(char red, char green, char blue, char alpha):r(red), g(green), b(blue), a(alpha){};
	RGBA_pixel(RGBA_float rgba_float):r(rgba_float.r), g(rgba_float.g), b(rgba_float.b), a(rgba_float.a){};

	RGBA_pixel operator*(float x){
		RGBA_pixel res;
		res.r = r*x;
		res.g = g*x;
		res.b = b*x;
		res.a = a*x;

		return res;

	}

	RGBA_pixel operator+(RGBA_pixel other){
		RGBA_pixel pix;
		pix.r = r+other.r;
		pix.g = g+other.g;
		pix.b = b+other.b;
		pix.a = a+other.a;

		return pix;
	}
};
