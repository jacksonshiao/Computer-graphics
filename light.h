#ifndef AFX_LIGHT_H_
#define AFX_LIGHT_H_
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
using namespace std;

class light {
	public:
		light(const char* light_data, const int n);
		~light();
		void loadlight(string light_data);
		int nlight;
		float **pos;
		float **a;
		float **d;
		float **s;
		float *amb;
	};

#endif