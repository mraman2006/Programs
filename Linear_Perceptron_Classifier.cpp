//Linear Perceptron Classifier

#include<iostream>
#include<set>
#include<map>
#include<cstdlib>
#include<cmath>
#include<vector>
#include<algorithm>
#include<sstream>
#include<string>
#include<fstream>
using namespace std;

#define ERROR -1
#define Col 50

double weights[Col];
int COL;


void print_instances(vector< vector<short> > instances,int row){
  for(int i=0;i<row;i++){
		for(int j=0;j<=COL;j++)
			cout<<instances[i][j]<<"\t";
		cout<<"\n";
	}
	return;
}


//Store the instances in an array.
vector< vector<short> > read_test_data(char* filename){
	char buf[100];
	vector< vector<short> > test_instances;
	ifstream input(filename);
	int row=0,t;
	input.getline(buf,100);
//Read the file and store it in instances array
	while(input){
		input.getline(buf,100);
		stringstream s(buf);
		vector<short> temp;
		temp.push_back(0);
		int val;
		for(int i=1;i<=COL;i++){
			s>>val;
			temp.push_back(val);			
		}
		test_instances.push_back(temp);
		row++;
	}
	row--;
	return test_instances;
}

//Count the no. of instances.
int lines_test_data(char *filename){
	ifstream input(filename);	
	int row=0;
	char buf[100];
	while(input){
		input.getline(buf,100);
		row++;
	}
	row-=2;
	return row;
}

//Count the # of spaces in the attributes list to find the no of attributes in training set
int count_spaces(char buf[]){
	int attr=0,i=0,spaces=0;
	while(buf[i]!='\0'){
		if(buf[i]==' ' || buf[i]=='\t')	spaces++;
		i++;
	}
	attr=spaces+1;
	return attr;
}

void print_weights(){
	for(int i=0;i<COL;i++)	cout<<weights[i]<<"\t";
	cout<<"\n";
}

int get_activator_class(vector<short> instances,long double &output){
		long double sum_weights=0;
		int activator_class=-1;
		for(int j=0;j<COL;j++){
			sum_weights+=instances[j]*weights[j];
		}
		output=1/(1+exp(-sum_weights));
		if(output>=0.5)	activator_class=1;
		else	activator_class=0;
		return activator_class;
}


void update_weights(vector<short> instance,int COL,int activator_class,int target_class,long double output,double learning_rate){
	long double delta_weight=1,gradient_descent;
	gradient_descent = output*(1-output);	
	for(int i=0;i<COL;i++){
		delta_weight=learning_rate*gradient_descent*instance[i]*(target_class-output);				
		weights[i]+=delta_weight;
	}	
}


int find_accuracy(vector< vector<short> > instances,int COL,int ROW){
	int activator_class,target_class,count=0;
	long double output=0;
	for(int i=0;i<ROW;i++){
		target_class=instances[i][COL];		
		activator_class=get_activator_class(instances[i],output);	
		if(target_class==activator_class){
			count++;
		}
	}
	return count;
}


void linear_perceptron(vector< vector<short> > instances,int COL,int ROW,int iterations,double learning_rate){

	for(int i=0;i<COL;i++)		weights[i]=0;
	long double output=0;
	int row;
	int activator_class,target_class;
	for(int i=0;i<iterations;i++){
		row=i%ROW;
		activator_class=get_activator_class(instances[row],output);	
		target_class=instances[row][COL];			
		update_weights(instances[row],COL,activator_class,target_class,output,learning_rate);			
		//print_weights();		
	}

}

int main(int argc,char **argv){	
	ifstream input(argv[1]);
	char buf[100];
	vector< vector<short> > instances;
	int row=0;
	string aname;
	input.getline(buf,100);
	COL=count_spaces(buf)+1;	//no of attributes + 1
	//cout<<COL<<"\n";
	int iterations=atoi(argv[4]);
	iterations+=1;
	double learning_rate=atof(argv[3]);
	//Read the file and store it in instances array
	while(input){
		input.getline(buf,100);
		stringstream s(buf);
		vector<short> temp;
		temp.push_back(0);
		int val;
		for(int i=1;i<=COL;i++){
			s>>val;
			temp.push_back(val);			
		}
		instances.push_back(temp);
		row++;
	}
	row--;
	
	//print_instances(instances,row);
	linear_perceptron(instances,COL,row,iterations,learning_rate);
	//print_weights();
	
	int count;
	count=find_accuracy(instances,COL,row);
	cout<<"Accuracy of Training data:"<<(float)count*100/row<<"\%\n";
	
	//Accuracy of test data
	vector< vector<short> > test_instances=read_test_data(argv[2]);
	row=lines_test_data(argv[2]);

	//print_instances(test_instances,row);
	count=find_accuracy(test_instances,COL,row);	
	cout<<"Accuracy of Test data:"<<(float)count*100/row<<"\%\n";
	
	return 0;
}

