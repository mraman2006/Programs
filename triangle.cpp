/*

Coded by Ramanathan Muthukaruppan
By starting at the top of the triangle and moving to adjacent numbers on the row below, the maximum total from top to bottom is 27.

        5
      9  6
    4   6  8
  0   7  1   5

I.e. 5 + 9 + 6 + 7 = 27.

Technique applied: Recursion with memoization
*/


#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<fstream>
#include<map>
#include<math.h>
#include<sstream>
using namespace std;
long a[5051];
long sum[5051];
int MAX_LEVEL;
long long trisum(int pos,int level){
  if(level==MAX_LEVEL) return a[pos];
	if(sum[pos]!=0) return sum[pos];
	sum[pos] = a[pos]+max(trisum(pos+level,level+1), trisum(pos+level+1,level+1));
	return sum[pos];
}
int main(){
	ifstream input("triangle.txt");
	char buf[800];
	if(!input){
		cout<<"File cannot be opened";
		return -1;
	}
	for(int i=0;i<5051;i++) sum[i]=0;
	int cols=2,pos=1,rows=0;
	while(input){
		rows++;
		input.getline(buf,800);
		stringstream in_line(buf);
		for(int i=1;i<cols;i++){
			in_line>>a[pos++];
		}
		cols++;
	}
	MAX_LEVEL = rows-1;
	cout<<trisum(1,1);	
	return 0;
}
