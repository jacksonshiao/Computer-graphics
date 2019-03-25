#include "light.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

light::light(const char* light_data, const int n){
	nlight = n;
	pos = new float*[nlight];
	a = new float*[nlight];
	d = new float*[nlight];
	s = new float*[nlight];
	amb = new float[3];			//r g b 

	for (int i = 0; i < nlight; i++){
		pos[i] = new float[3];	//x y z
		a[i] = new float[3];	//ax ay az
		d[i] = new float[3];	//dx dy dz
		s[i] = new float[3];	//sx sy sz
	}

	loadlight((string)light_data);
}

light::~light(){
	for (int i = 0; i < nlight; i++){
		delete[] pos[i];
		delete[] a[i];
		delete[] d[i];
		delete[] s[i];
	}
	delete[] pos;
	delete[] a;
	delete[] d;
	delete[] s;
	delete[] amb;
}

void light::loadlight(string light_data){
	fstream light_input;
	string cmp;
	light_input.open(light_data.c_str(), ios::in);
	if (!light_input) {
		cout << "Open Fail light" << endl;
		return;
	}
	cout << light_data << endl;
	
	int n = 0;
	while (light_input >> cmp){
		if (cmp == "light"){
			for (int i = 0; i < 3; i++){
				light_input >> pos[n][i];
			}
			for (int i = 0; i < 3; i++){
				light_input >> a[n][i];
			}
			for (int i = 0; i < 3; i++){
				light_input >> d[n][i];
			}
			for (int i = 0; i < 3; i++){
				light_input >> s[n][i];
			}
			n++;
		}
		if (cmp == "ambient"){
			for (int i = 0; i < 3; i++){
				light_input >> amb[i];
			}
		}	
	}
}











