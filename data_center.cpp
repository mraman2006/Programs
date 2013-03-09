//Problem 2: Data Center replica problem. To restore missing data sets in data centers by copying them from other data centers.
//Compiler used: gcc 4.6.3
//Operating system: ubuntu 12.04
//Coded by Ramanathan Muthukaruppan

#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<algorithm>
using namespace std;

int find(int val,vector<int> v){  
	int high=v.size()-1;
	int low=0,mid;
	while(low<=high){
		mid=(low+high)/2;
		if(val<v[mid])	high=mid-1;
		else if(val>v[mid])	low=mid+1;
		else return mid;
	}
	return -1;
}

int main(){
	int data_centers,data_sets,temp;
	map<int, vector<int> > map_data_center_set;
	map<int,int> data_set_id;
	cin>>data_centers;

	for(int i=0;i<data_centers;i++){
		cin>>data_sets;
		vector<int> v;
		for(int j=0;j<data_sets;j++){
			cin>>temp;
			v.push_back(temp);
			data_set_id[temp]=i;
		}
		sort(v.begin(),v.end());
		map_data_center_set[i]=v;
	}

	int unique_data_set_id = data_set_id.size();
	for(int i=0;i<data_centers;i++){

		if(map_data_center_set[i].size()==unique_data_set_id)	continue;	//if data center i has all data sets, then skip it
	
		map<int,int>::iterator it=data_set_id.begin();
		while(it!=data_set_id.end()){
			temp=find(it->first,map_data_center_set[i]);
			if(temp==-1){	//Data set id does not exist in Data center i
				cout<<it->first<<" "<<it->second+1<<" "<<i+1<<"\n";
			}
			++it;
		}	
	}

	cout<<"done\n";
	return 0;
}
