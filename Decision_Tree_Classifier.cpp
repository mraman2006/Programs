//Coded by Ramanathan Muthukaruppan
//Implementation of Decision Tree algorithm - Machine Learning
//Binary classification, Attributes take integer value

#include<iostream>
#include<set>
#include<map>
#include<cmath>
#include<vector>
#include<algorithm>
#include<sstream>
#include<string>
#include<fstream>
using namespace std;

#define ERROR -1
#define Col 50

map<int,set<int> > attr_values_map;
short attr_values[Col];
int COL;


// The node which holds the pointers
class node{
public:
  int attr,node_class;
	node* v[50];
	
	node(int attr){
		this->attr=attr;
		this->node_class=-1;
	}
	
};

//Calculate the entropy of an attribute given the no. of positive and negative instances.
float entropy(int pos,int neg){
	if(pos==0 || neg==0)	return 1;
	int tot=pos+neg;
	float pos_ratio = float(pos)/tot;
	float neg_ratio = float(neg)/tot;
	float entropy_pos=(-pos_ratio*log2(pos_ratio));
	float entropy_neg=(-neg_ratio*log2(neg_ratio));
	return entropy_pos+entropy_neg;
}

//Count the no. of positive and negative instances.
float count_classes(short instance_class[][Col],int row){
	int c[2]={0,0};
	for(int i=0;i<row;i++){
		c[instance_class[i][6]]++;
	}
	return entropy(c[0],c[1]);
}

//Calculate the information gain and return the attribute that has maximum IG
int information_gain(short instances[][Col],int row,float class_entropy,int attributes_in_tree[]){
	float ig, max=-999999999;
	int max_index=-1;
	for(int attr=0;attr<COL-1;attr++){
		if(attributes_in_tree[attr]==1)	continue;
		int total_values=attr_values[attr];
		float c[total_values][2];
		for(int i=0;i<total_values;i++)	c[i][0]=c[i][1]=0;
		int i=0;
		while(i<row){
			if(instances[i][COL-1]==0)
				c[instances[i][attr]][0]++;
			else
				c[instances[i][attr]][1]++;
			i++;
		}
		ig = class_entropy;
		for(int i=0;i<total_values;i++){
		  ig = ig - ((c[i][0]+c[i][1])/row*entropy(c[i][0],c[i][1]));
		}
		if(ig>max){
			max=ig;
			max_index=attr;
		}
	}
	return max_index;
}

//Return true if the value for that attribute contains instances of a single class, else returns false.
bool isleaf(short instances[][Col],int row,int attr,int val){
	int c[2]={-1,-1};
	set<int>::iterator it=attr_values_map[attr].begin();
	for(int i=0;i<val;i++){
		++it;
	}
	int temp=*it;
	for(int i=0;i<row;i++){
		if(instances[i][attr]==temp)
			c[instances[i][COL-1]]++;
	}
	if(c[0]==-1 || c[1]==-1) return true;
	return false;
}

//Find the class the instance belongs to.
node* find_class(short instances[][Col],int row,int attr,int val,int mfc){
	int c=-1;
	set<int>::iterator it=attr_values_map[attr].begin();
	for(int i=0;i<val;i++){
		++it;
	}
	int temp=*it;
	for(int i=0;i<row;i++){
		if(instances[i][attr]==temp){
			c=instances[i][COL-1];
			break;
		}
	}
	if(c==-1){
		int c1[2]={-1,-1};
		for(int i=0;i<row;i++){
			c1[instances[i][6]]++;
		}
		if(c1[0]>c1[1]) c=0;
		else if(c1[1]>c1[0]) c=1;
		else c=mfc;
	}
	node* newnode=(node*) new node(attr);
	newnode->node_class=c;
	for(int k=0;k<attr_values[attr];k++)
		newnode->v[k]=NULL;
	return newnode;
}

//If the instances in the leaf node contain a mix of classes, then return the class that occurs more frequently among the instances.
//If there is no such class, then return the most frequently occuring class in the entire training set.
int check_tree(short instances[][Col],int attr,int val,int row,int attributes_in_tree[],int mfc){
	int count=0;
	for(int i=0;i<COL-1;i++){
		if(attributes_in_tree[i]==1)	count++;
	}
	if(count!=COL-1)	return ERROR;
	int c[2]={-1,-1};
	set<int>::iterator it=attr_values_map[attr].begin();
	for(int i=0;i<val;i++){
		++it;
	}
	int temp=*it;

	for(int i=0;i<row;i++){
		if(instances[i][attr]==temp)
			c[instances[i][COL-1]]++;
	}
	if(c[0]>c[1]) return 0;
	if(c[1]>c[0]) return 1;
	return mfc;
}

//Find the most frequently occuring class in the training set. Binary class with two values.
int mostfrequentclass(short instances[][Col],int row){
	int c[2]={-1,-1};
	for(int i=0;i<row;i++){
		c[instances[i][6]]++;
	}
	if(c[0]>c[1]) return 0;
	else return 1;
}

void print_instances(short instances[][Col],int row){
	for(int i=0;i<row;i++){
		for(int j=0;j<COL;j++)
			cout<<instances[i][j]<<"\t";
		cout<<"\n";
	}
	return;
}

//Create the decision tree based on the Information gain and entropy values.
node* learntree(short instances[][Col],int row,int attributes_in_tree[],int mfc,int arg_val,int arg_attr,int tabspace){	

 	float class_entropy = count_classes(instances,row);
 //	cout<<"Entropy:"<<class_entropy<<"\n";
	int attr=information_gain(instances,row,class_entropy,attributes_in_tree);
	node *newnode = (node*)new node(attr);	
	attributes_in_tree[attr]=1;
	
	int flag;
	int VALUES=attr_values[attr];
	for(int val=0;val<VALUES;val++){
		
		for(int t=0;t<tabspace;t++){
			cout<<"| ";
		}
		cout<<"attr"<<attr+1<<" = "<<val<<" : ";
		
		if(isleaf(instances,row,attr,val)){
			newnode->v[val]=find_class(instances,row,attr,val,mfc);	
			cout<<newnode->v[val]->node_class<<"\n";
		}
		else if((flag=check_tree(instances,attr,val,row,attributes_in_tree,mfc))!=ERROR){
			node* tempnode=(node*)new node(attr);
			tempnode->node_class=flag;
			for(int k=0;k<VALUES;k++)
				tempnode->v[k]=NULL;
			newnode->v[val]=tempnode;				
			cout<<newnode->v[val]->node_class<<"\n";	
		}
		else{
			short new_instances[2000][Col]={0};			
			int new_rows=0;
			cout<<"\n";
			set<int>::iterator it=attr_values_map[attr].begin();
			for(int j=0;j<val;j++){
				++it;
			}
			int temp=*it;
			for(int i=0;i<row;i++){				
				if(instances[i][attr]==temp){
					for(int j=0;j<COL;j++){
						new_instances[new_rows][j]=instances[i][j];											
					}
					new_rows++;
				}				
			}
//			print_instances(new_instances,new_rows);
			newnode->v[val]=learntree(new_instances,new_rows,attributes_in_tree,mfc,val,attr,tabspace+1);
		}
		
	}
	attributes_in_tree[attr]=0;
	return newnode;
}

//Store the unique values that an attribute can take.
void find_unique_values(short instances[][Col],int row){
	for(int j=0;j<COL-1;j++){
		set<int> s;
		for(int i=0;i<row;i++){
			s.insert(instances[i][j]);
		}
		attr_values_map[j]=s;
	}
}

//Find the class given the new unseen data.
int find_tree_class(node root_node,short* instance_attr){
		if(root_node.node_class!=-1)
			return root_node.node_class;
		int val=instance_attr[root_node.attr];
		int total_values=attr_values[root_node.attr];
//		cout<<val<<" "<<root_node.attr<<"\n";
		set<int>::iterator it=attr_values_map[root_node.attr].begin();
		int j=0;
		for(;it!=attr_values_map[root_node.attr].end();){
			if(*it==val) break;
			++it;
			++j;
		}		
		return find_tree_class(*root_node.v[j],instance_attr);
}

//Store the instances in an array.
short** read_test_data(char* filename){
	char buf[100];
	short **test_instances;
	ifstream input(filename);
	test_instances=new short*[2000];
	for(int i=0;i<2000;i++){
		test_instances[i]=new short[Col];	
	}
	int row=0;
	input.getline(buf,100);
	//Read the file and store it in instances array
	while(input){
		for(int i=0;i<COL;i++){
			input>>test_instances[row][i];
		}
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

//Free the memory dynamically allocated using delete to prevent memory leak.
void free_memory(node *root_node){
	if(root_node->node_class!=-1)	return;
	int total_values=attr_values[root_node->attr];
	for(int i=0;i<total_values;i++){
			free_memory(root_node->v[i]);
			delete(root_node->v[i]);
	}
	return;
}

//Count the # of spaces in the attributes list to find the no of attributes in training set
int count_spaces(char buf[]){
	int attr=0,i=0,spaces=0;
	while(buf[i]!='\0'){
		//cout<<buf[i];
		if(buf[i]==' ' || buf[i]=='\t')	spaces++;
		i++;
	}
//	cout<<spaces<<"\n";
	attr=(spaces+1)/2;
	return attr;
}



int main(int argc,char **argv){	
	ifstream input(argv[1]);
	char buf[100];
	short instances[2000][Col]={0};
	int row=0;
	string aname;
	input.getline(buf,100);
	COL=count_spaces(buf)+1;
	stringstream s1(buf);
	for(int i=0;i<COL-1;i++){
		s1>>aname>>attr_values[i];
	}
	attr_values[COL-1]=0;

//Read the file and store it in instances array
	while(input){
		input.getline(buf,100);
		stringstream s(buf);
		for(int i=0;i<COL;i++){
			s>>instances[row][i];
		}
		row++;
	}
	row--;
	find_unique_values(instances,row);
	int attributes_in_tree[Col]={0};
	int mfc=mostfrequentclass(instances,row);	
	node* root_node;
	root_node=learntree(instances,row,attributes_in_tree,mfc,0,0,0);
	
	//Accuracy of training data
	int correct=0;
	for(int i=0;i<row;i++){		
		int tree_cl=find_tree_class(*root_node,instances[i]);
		if(tree_cl==instances[i][COL-1])	correct++;	
	}
	cout<<"Accuracy of training data ("<<row<<" instances) = "<<(float)correct*100/row<<"%\n";

	//Accuracy of test data
	short **test_instances=read_test_data(argv[2]);
	row=lines_test_data(argv[2]);
	correct=0;
	for(int i=0;i<row;i++){		
		int tree_cl=find_tree_class(*root_node,test_instances[i]);
		if(tree_cl==test_instances[i][COL-1])	correct++;	
	}
	cout<<"Accuracy of test data ("<<row<<" instances) = "<<(float)correct*100/row<<"%\n";

	//Free memory
	free_memory(root_node);
	delete(root_node);
	for(int i=0;i<row;i++){	
		delete(test_instances[i]);
	}	
	delete(test_instances);
	return 0;
}

