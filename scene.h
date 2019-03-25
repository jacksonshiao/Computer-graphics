#ifndef AFX_SCENE_H_
#define AFX_SCENE_H_
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
using namespace std;

class scene
{
	public:
		scene(const char* scene_data, const int n);
		~scene();
		void loadscene(string scene_data);
		int nobj;
		char **obj_name;
		float **s;
		float *angle;
		float **r;
		float **t;
		char** effect;
		char** texture_name;
		int texture_num;
		int* texture_use;
};
#endif