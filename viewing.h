#ifndef AFX_VIEWING_H_
#define AFX_VIEWING_H_
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
using namespace std;

class viewing
{
	public:
		viewing(const char* view_data);
		~viewing();
		void loadview(string view_data);
		float *eye;
		float *vat;
		float *vup;
		float theta;
		float dnear_d;
		float dfar_d;
		float *viewport;
};
#endif