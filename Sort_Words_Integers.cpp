//Problem 1: Sorting bag of words and numbers there by maintaining their relative order in the original list
//Compiler used: g++ 4.6.3
//Operating system: ubuntu 12.04
//Coded by Ramanathan Muthukaruppan

#include<iostream>
#include<string>
#include<cmath>
#include<cstdlib>
#include<vector>
#include<sstream>
#include<algorithm>
using namespace std;

int count_spaces(string line){
  int len=line.length();
	int spaces=0,i=0;	
	while(i<len){
		if(line[i]==' ')	spaces++;
		i++;
	}
	return spaces;
}

int main(){
	char line[1000],inp[1000];
	vector<string> string_input;
	vector<int> num_input;
	vector<int> flag;
	int i=0;
	cin.getline(line,1000);
	stringstream s(line);
	int limit=count_spaces(line);

	while(i<=limit){
		s>>inp;
		if(inp[0]=='-' || (inp[0]>=48 && inp[0]<=57)){
			num_input.push_back(atoi(inp));
			flag.push_back(1);
		}
		else{
			string_input.push_back(inp);
			flag.push_back(0);
		}
		i++;
	}

	sort(string_input.begin(),string_input.end());
	sort(num_input.begin(),num_input.end());
	vector<int>::iterator num_iterator=num_input.begin();
	vector<string>::iterator string_iterator=string_input.begin();
	limit=flag.size();

	for(int i=0;i<limit;i++){
		if(flag[i]==1){
			cout<<*num_iterator;
			num_iterator++;
		}
		else if(flag[i]==0){
			cout<<*string_iterator;
			string_iterator++;
		}
		if(i+1!=limit)
		cout<<" ";
	}
	cout<<"\n";

	return 0;
}
