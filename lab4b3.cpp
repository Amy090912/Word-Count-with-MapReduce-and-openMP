#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<thread>
#include <utility>
#include<algorithm>
#include <chrono>
#include <time.h>
#include <omp.h>
#include <mutex>

using namespace std;
using namespace chrono;

mutex m;
vector<string> input_reader(string filename)
{
	vector<string> a;
	ifstream file(filename);
	string s;
	while (file >> s)
	{
		a.push_back(s);
	}
	return a;
}

pair<string, int> map(string input)
{
	pair<string, int> p;
	p = make_pair(input, 1);
	return p;
}

pair<string, int> reduce(vector<pair<string, int>> vec)
{
	for (int i = 1; i < vec.size(); i++)
	{
		vec[0].second++;
	}
	return vec[0];
}


void output(vector<pair<string, int>> vec)
{
	vector<int> p_val;
	for (int i = 0; i < vec.size(); i++)
	{
		p_val.push_back(vec[i].second);
	}

	vector<pair<string, int>> b_vec3;  //in count order
	int biggest;
	while (p_val.size() > 0)
	{
		biggest = std::distance(p_val.begin(), max_element(p_val.begin(), p_val.end())); //biggest element index
		b_vec3.push_back(vec[biggest]);
		p_val.erase(p_val.begin() + biggest);
		vec.erase(vec.begin() + biggest);
	}

	for (int i = 0; i < b_vec3.size(); i++)
	{
		//for (int j = 0; j < b_vec3[i].second; j++)
			//cout << b_vec3[i].first << " ";
		//cout << endl;
		cout << b_vec3[i].first << "  " << b_vec3[i].second << endl;
	}
}


int main()
{
	auto start = system_clock::now();

#pragma omp parallel for
	for (int i = 0; i < 10; i++)
	{
		cout << i << endl;
	}

	vector<string> a = input_reader("text2.txt");
	
	string* a_arr;
	a_arr = new string[a.size()];
	for (int i = 0; i < a.size(); i++)
	{
		a_arr[i] = a[i];
	}
	/*
	for (int i = 0; i < a.size(); i++)
	{
		cout << a_arr[i] << endl;
	}*/

	string input;
	vector<pair<string, int>> b_vec; //store the pairs from map

	//////////////////////////////map/////////////////////////////
	pair<string, int> * pointer;
	pointer = new pair<string, int>[a.size()];

#pragma omp parallel for 
	for (int i = 0; i < a.size(); i++)
	{
		//m.lock();
		input = a_arr[i];
		//m.unlock();
		//m.lock();
		//input = a[i];
		//m.unlock();
		//string input = a_arr[i];
		//m.lock();
		//pair<string, int> b = map(a[i]);
		pair<string, int> b = map(input);
		
		//cout << "b is: " << b.first << "  " << b.second << endl;
		//m.unlock();
		//m.lock();
		pointer[i] = b;
		//m.unlock();
		
		//pointer[i] = map(a[i]);
		//m.lock();
		//cout << "pointer is : " << pointer[i].first << " " << pointer[i].second << endl;
		//m.unlock();
	}

	for (int i = 0; i < a.size(); i++)
	{
		b_vec.push_back(pointer[i]);
	}

	delete[] a_arr;
	delete[] pointer;
	
	cout << endl << endl << "b_vec: " << endl;
	for (int i = 0; i < b_vec.size(); i++)
	{

		cout << b_vec[i].first << "  " << b_vec[i].second << endl;
	}
	
	vector< vector<pair<string, int>>> table; //vector for row
	vector<pair<string, int>> row; //vector for one group
	for (int i = 0; i < b_vec.size(); i++)
	{
		row.push_back(b_vec[i]);
		for (int j = i + 1; j < b_vec.size(); j++)
		{
			if (b_vec[j] == b_vec[i])
			{
				row.push_back(b_vec[j]);
				b_vec.erase(b_vec.begin() + j);
				j--;
			}
		}
		table.push_back(row);
		row.clear();
	}

	vector<pair<string, int>> b_vec2; //store the pairs from reduce

	/////////////////////////reduce///////////////////////////////////[
	pair<string, int> * pointer2;
	pointer2 = new pair<string, int>[table.size()];
#pragma omp parallel for 
	for (int i = 0; i < table.size(); i++)
	{
		//pair<string, int> b2 = reduce(table[i]);
		//pointer2[i] = b2;

		pointer2[i] = reduce(table[i]);
	}

	for (int i = 0; i < table.size(); i++)
	{
		b_vec2.push_back(pointer2[i]);
	}
	delete[] pointer2;

	/*
	cout << endl << endl << "b_vec2: " << endl;
	for (int i = 0; i < b_vec2.size(); i++)
	{

		cout << b_vec[i].first << "  " << b_vec[i].second << endl;
	}*/

	output(b_vec2);

	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	cout << endl << "time cost: " << double(duration.count()) << " microseconds" << endl;
	system("pause");
}