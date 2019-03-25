#include "scene.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

scene::scene(const char* scene_data, const int n)
{
	nobj = n;
	texture_num = 0;					//number of texture
	s = new float*[nobj];
	r = new float*[nobj];
	t = new float*[nobj];
	effect = new char*[nobj];
	texture_name = new char*[nobj];		
	obj_name = new char*[nobj];		
	angle = new float[nobj];		//angle
	texture_use = new int[nobj];		//used texture is from XX to XX

	for (int i = 0; i < nobj; i++)
	{
		s[i] = new float[3];		//Sx Sy Sz
		r[i] = new float[3];		//Rx Ry Rz
		t[i] = new float[3];		//Tx Ty Tz
		obj_name[i] = new char[50];	//name of object
		effect[i] = new char[50];		//which texture method
		texture_name[i] = new char[50];	//name of texture
	}

	loadscene((string)scene_data);
}

scene::~scene()
{
}

void scene::loadscene(string scene_data)
{
	fstream input;
	string cmp;
	input.open(scene_data.c_str(), ios::in);
	if (!input)
	{
		cout << "Opened Failed scene" << endl;
		return;
	}
	cout << scene_data << endl;

	string str;
	int used = texture_num;
	int n = 0;
	while (input >> cmp)
	{
		if (cmp == "no-texture")
		{
			str = "no";
		}
		else if (cmp == "single-texture")
		{
			str = "single";
			used = texture_num;
			input >> texture_name[texture_num];
			texture_num++;
		}
		else if (cmp == "multi-texture")
		{
			str = "multi";
			used = texture_num;
			for (int i = 0; i < 2; i++)
			{
				input >> texture_name[texture_num];
				texture_num++;
			}
		}
		else if (cmp == "cube-map")
		{
			str = "cube";
			used = texture_num;
			for (int i = 0; i < 6; i++)
			{
				input >> texture_name[texture_num];
				texture_num++;
			}
		}
		else if (cmp == "model")
		{
			input >> obj_name[n];
			strcpy(effect[n], str.c_str());
			cout << obj_name[n] << "   " << effect[n] << "    " << n << "   " << used << endl;
			texture_use[n] = used;

			for (int i = 0; i < 3; i++)
			{
				input >> s[n][i];
			}

			input >> angle[n];
			
			for (int i = 0; i < 3; i++)
			{
				input >> r[n][i];
			}

			for (int i = 0; i < 3; i++)
			{
				input >> t[n][i];
			}
			n++;
		}
	}
}