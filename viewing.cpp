#include "viewing.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

viewing::viewing(const char* view_data)
{
	eye = new float[3];				//eye x y z 
	vat = new float[3];				//vat x y z
	vup = new float[3];				//vup x y z
	viewport = new float[4];		//viewport x y w h

	loadview((string)view_data);
}

viewing::~viewing()
{
}

void viewing::loadview(string view_data)
{
	fstream view_input;
	string cmp;
	view_input.open(view_data.c_str(), ios::in);
	if (!view_input)
	{
		cout << "Open Fail view" << endl;
		return;
	}
	cout << view_data << endl;

	while (view_input >> cmp)
	{
		if (cmp == "eye")
		{
			for (int i = 0; i < 3; i++)
			{
				view_input >> eye[i];
			}
		}
		else if (cmp == "vat")
		{
			for (int i = 0; i < 3; i++)
			{
				view_input >> vat[i];
			}
		}
		else if (cmp == "vup")
		{
			for (int i = 0; i < 3; i++)
			{
				view_input >> vup[i];
			}
		}
		else if (cmp == "fovy")
		{
			view_input >> theta;
		}
		else if (cmp == "dnear")
		{
			view_input >> dnear_d;
		}
		else if (cmp == "dfar")
		{
			view_input >> dfar_d;
		}
		else if (cmp == "viewport")
		{
			for (int i = 0; i < 4; i++)
			{
				view_input >> viewport[i];
			}
		}
	}
}