//Inference in the given Bayesian / Markov network
//by using Bucket Elimination algorithm with Min Degree order.

#include<iostream>
#include<fstream>
#include<map>
#include<set>
#include<string>
#include<string>
#include<cmath>
#include<vector>
#include<algorithm>
using namespace std;

class Variable{
public:
  int d;
  int value;
	bool is_evidence;

public:
	Variable(int d,int value){
		this->d=d;
		this->value=value;
		is_evidence=false;
	}
};

class Factor{
public:
	vector<Variable> variables;
	vector<long double> table;
    bool is_processed;
public:
	Factor(){
	    is_processed = false;
	}
	Factor(vector<Variable> v){
		this->variables=v;
		is_processed = false;
	}
	Factor(vector<Variable> v,vector<long double> t){
		this->variables=v;
		this->table=t;
		is_processed = false;
	}

	void set(vector<long double> t)
	{
		table=t;
	}

	vector<Variable> get_var(){
		return variables;
	}
	void print_var(){
		for(int i=0;i<variables.size();i++){
			cout<<variables[i].value<<"\t";
		}
		cout<<"\n";
	}

    void print_table(){
		int sz=variables.size();
		for(int i=0;i<table.size();i++){
			cout<<(table[i])<<"\t";
			if(sz!=0 && (i+1)%sz==0)	cout<<"\n";
		}
		cout<<"\n";
    }
	vector<long double> get_table(){
		return table;
	}

};

void print_factors(const vector<Factor*> &,int);
Factor* multiply_evidence_factors(vector<Factor*> &factor,int f,long double &, bool);
Factor* sum_factors(Factor* temp,int eliminate);

void set_edge(vector<vector<bool> > &edge, const vector<Variable> &v_temp){
	if(v_temp.size()==1)	edge[v_temp[0].value][v_temp[0].value]=true;
	for(int i=0;i<v_temp.size()-1;i++){
		for(int j=i+1;j<v_temp.size();j++){
			(edge[v_temp[i].value][v_temp[j].value])=true;
			(edge[v_temp[j].value][v_temp[i].value])=true;
			//cout<<v_temp[i]->value<<" "<<v_temp[j]->value<<"\n";
		}
	}
}

int find_min_order(const vector<vector<bool> > &edge,int n, const vector<int> &eliminated_list){
	int count,min=99999,min_index=0;
	vector<int>::const_iterator it = eliminated_list.begin();
	map<int,bool> eliminated_map;
    while( it != eliminated_list.end() ){
        eliminated_map[*it] = true;
        it++;
    }

	for(int i=0;i<n;i++){
		count=0;
/*		it=find(eliminated_list.begin(),eliminated_list.end(),i);
		if(it != eliminated_list.end()){
			continue;
		}*/
		if(eliminated_map[i])    continue;
		for(int j=0;j<n;j++){
			if(edge[i][j])	count++;
		}
		if(min>count){
			min=count;
			min_index=i;
		}
		//cout<<"Variable:"<<i<<" Neighbors:"<<count<<"\n";
	}
	return min_index;
}

void remove_edges(vector<vector<bool> > &edge,int e,int n){
	for(int i=0;i<n;i++){
		edge[i][e]=false;
		edge[e][i]=false;
	}
	return;
}

int find_row_size(const vector<Variable> &var1){
	int size=1;
	for(int i=0;i<var1.size();i++){
		size=size*var1[i].d;
	}
	return size;
}

int find_position(const vector<Variable> &var1,int e){
	int mark=0;
	vector<Variable> temp;
	for(int i=0;i<var1.size();i++){
		if(var1[i].value==e){
			mark=1;
			if(i+1==var1.size()){		//If variable to be eliminated is least significant
				temp.push_back(var1[i]);
			}
			continue;
		}
		if(mark==1)	{
			temp.push_back(var1[i]);
		}
	}
	return find_row_size(temp);
}


//Find how many variables to the right of it and multiply their domain sizes to find out how many times a value of the variable is going to be repeated
int find_value_repititions(const vector<Variable> &var1,int e){
	int mark=0;
	vector<Variable> temp;
	for(int i=0;i<var1.size();i++){
		if(var1[i].value==e){
			mark=1;
			continue;
		}
		if(mark==1)	{
			temp.push_back(var1[i]);
		}
	}
	//cout<<"No of variables that are least significant:"<<temp.size()<<"\n";
	return find_row_size(temp);
}


//Return the index of the variable 'e'
int find_index(const vector<Variable> &var1,int e){
	for(int i=0;i<var1.size();i++){
		if(var1[i].value == e)	return i;
	}
	return -1;
}

int find_value(const vector<Variable> &var1,int e){
	for(int i=0;i<var1.size();i++){
		if(var1[i].value==e){
			return var1[i].d;
		}
	}
	return 0;
}


//Remove the factors containing the variable 'e' from the factors list
void remove_factors(vector<Factor*> &factor,int eliminate,int f){
	vector<Factor*>::iterator it=factor.begin();
	while(it!=factor.end()){
		int sz=((*it)->variables).size();
		bool found = false;
		for(int j=0;j<sz;j++){
				if(((*it)->variables[j]).value==eliminate){
					found = true;
					break;
				}
		}
		if(found)
			it = factor.erase(it);
		else
			it++;
	}
	return;
}



int compare_function(Variable a, Variable b){
	return a.value < b.value;
}


//Find positions of to be multiplied factor varibles in the target factor
vector<int> find_positions_map(const vector<Variable> &FA,const vector<Variable> &target_var){
	vector<int> pos;
	for(int i=0;i<FA.size();i++){
		int val_FA=FA[i].value;
		for(int j=0;j<target_var.size();j++){
			if(val_FA==target_var[j].value){
				pos.push_back(j);
				break;
			}
		}
	}
	return pos;
}



//Find positions of evidence varibles in the target factor
vector<int> find_positions_map(const map<int,int> evid_map,const vector<Variable> &target_var){
	vector<int> pos;
	map<int,int>::const_iterator it=evid_map.begin();
	while(it!=evid_map.end()){
		for(int j=0;j<target_var.size();j++){
			if(it->first==target_var[j].value){
				pos.push_back(j);
				break;
			}
		}
		it++;
	}
	return pos;
}

long double instantiate_evidence(vector<Factor*> &factors, map<int,int> evid_map){
	int sz=factors.size();
	long double z=1;
	for(int j=0;j<sz;j++){
		//Check if the factor contains any of the evidence variables
		vector<int> eliminate_pos = find_positions_map(evid_map,factors[j]->variables);
		vector<int> variable_eliminate;

		for(int i=0;i<eliminate_pos.size();i++){
			variable_eliminate.push_back((factors[j]->variables[eliminate_pos[i]]).value);
		}

		if(eliminate_pos.size()==0)	continue;

		//factors[j]->get_var();
		//factors[j]->get_table();


		//create a new table with only the values that are consistent with the evidence
		vector<long double> table;
		vector<int> target_values(factors[j]->variables.size(),0);
		int trow=0;
		int table_size=find_row_size(factors[j]->variables);
		vector<int> target_repeats(factors[j]->variables.size(),0);
		for(int i=factors[j]->variables.size()-1;i>=0;i--){
			int repeats = find_value_repititions(factors[j]->variables,(factors[j]->variables[i]).value);
			target_repeats[i]=repeats;
			//cout<<target_repeats[i]<<" ";
		}

		while(trow<table_size){
			if(trow){
				for(int i=0;i<factors[j]->variables.size();i++){
					if(trow%target_repeats[i]==0){
						target_values[i]=(target_values[i]+1)%((factors[j]->variables[i]).d);
					}
				}
			}

			bool flag=true;
			for(int k=0;k<eliminate_pos.size();k++){
				int var=factors[j]->variables[eliminate_pos[k]].value;
				if(target_values[eliminate_pos[k]]!=evid_map[var]){
					flag=false;
					break;
				}
			}
			if(flag)
				table.push_back(factors[j]->table[trow]);
			trow++;
		}

		for(int k=0;k<eliminate_pos.size();k++){
			vector<Variable>::iterator it=factors[j]->variables.begin();
			//cout<<"Removing:"<<eliminate_pos[k]<<"\n";
			while(it!=factors[j]->variables.end()){
				if((*it).value==variable_eliminate[k])	break;
				it++;
			}
			if(it != factors[j]->variables.end())
                factors[j]->variables.erase(it);
		}
		if(table.size()==1)	z=z*table[0];
		factors[j]->table = table;
		//cout<<"After instantiation\n";
		//factors[j]->get_var();
		//factors[j]->get_table();

		//cout<<"\n\n";
	}

	vector<Factor*>::iterator it=factors.begin();
	while(it!=factors.end()){
		if((*it)->variables.size()==0){
			it=factors.erase(it);
		}
		else
			it++;
	}

	//cout<<"Value of Z:"<<z<<"\n";
	return z;
}

bool operator<(const Variable a, const Variable b){
    return a.value < b.value;
}

Factor* multiply_factors(vector<Factor*> &factor,int eliminate,int f,map<int,int> evid_map, bool &summed_out, long double &pr_evid){
		vector<Factor*> target_factors;
		vector<long double> target_table;
		vector<Variable> target_var;
		set<Variable> target_set;
		int t=0,target_elements,d;
		map<vector<int>,long double> m;
        summed_out = true;
		//Find the set of factors that contain the variable 'e' and add them to the list of target factors
		for(int i=0;i<f;i++){
            if(factor[i]->is_processed) continue;
			int sz=(factor[i]->variables).size();
			bool found=false;
			for(int j=0;j<sz;j++){
				if((factor[i]->variables[j]).value==eliminate){
					found=true;
					factor[i]->is_processed = true;
					d=(factor[i]->variables[j]).d;
					break;
				}
			}
			if(found){
				target_factors.push_back(factor[i]);
				//factor[i]->print_var();
				//factor[i]->print_table();
				vector<Variable>::iterator vit=(target_factors[t]->variables).begin();
				while(vit!=(target_factors[t]->variables).end()){
					target_set.insert(*vit);
					vit++;
				}
				t++;
			}
		}

		//print_factors(target_factors,target_factors.size());

		//Remove duplicates from target variables list and order them
		set<Variable>::iterator sit=target_set.begin();
		while(sit!=target_set.end()){
			target_var.push_back(*sit);
			sit++;
		}

		if(target_var.size() == 1){
            summed_out = false;
            return multiply_evidence_factors(target_factors,target_factors.size(), pr_evid, true);
		}

		sort(target_var.begin(),target_var.end(),compare_function);
		//cout<<"\n";
		//cout<<"Target Variables size:"<<target_var.size()<<"\n";
		t=0;
		target_elements=target_factors.size();
		int table_size=find_row_size(target_var);
		vector<int> target_repeats(target_var.size(),0);
		for(int i=target_var.size()-1;i>=0;i--){
			target_repeats[i] = find_value_repititions(target_var,target_var[i].value);
			//cout<<target_repeats[i]<<" ";
		}
		//cout<<"\n";

		//vector<int> eliminate_pos = find_positions_map(evid_map,target_var);
		vector< vector<int> > pos;
		vector< vector<int> > factor_repeats;

		for(int i=0;i<target_elements;i++){
			//Hash for each factor to be multiplied
			//Find positions in Factor
			pos.push_back(find_positions_map(target_factors[i]->variables,target_var));
			vector<int> f_repeats(target_var.size(),0);
			for(int k=target_factors[i]->variables.size()-1;k>=0;k--){
				f_repeats[k] = find_value_repititions(target_factors[i]->variables,(target_factors[i]->variables[k]).value);
			}
			factor_repeats.push_back(f_repeats);
		}


	int trow=0;
	vector<int> target_values(target_var.size(),0);
	while(trow<table_size){
			if(trow){
				for(int i=0;i<target_var.size();i++){
					if(trow%target_repeats[i]==0){
						target_values[i]=(target_values[i]+1)%(target_var[i].d);
					}
				}
			}
			trow++;
			long double factor_value=1;
			for(int i=0;i<target_elements;i++){
				int table_pos=0;
				for(int l=0;l<pos[i].size();l++){
					int val=target_values[pos[i][l]];
					table_pos+=val*factor_repeats[i][l];
				}
				factor_value *= target_factors[i]->table[table_pos];
			}
			target_table.push_back(factor_value);
	}
	Factor* temp = new Factor(target_var,target_table);
	//temp->print_table();
	return sum_factors(temp,eliminate);
}




void remove_variable(vector<Variable> &var,int eliminate){
	vector<Variable>::iterator it=var.begin();
	while(it!=var.end()){
		if((*it).value==eliminate){
			var.erase(it);
			break;
		}
		it++;
	}
	return;
}

Factor* sum_factors(Factor* temp,int eliminate){
	int start=0;
	vector<Variable> var1=temp->variables;
	vector<long double> table1=temp->table;
	int row_size1=find_row_size(var1);
	int col_size1=var1.size();
	int limit,d,d1;
	int change_val_interval1=find_value_repititions(var1,eliminate);
	d=find_value(var1,eliminate);
	//cout<<"Summation terms: Interval:"<< change_val_interval1 <<"\n";
	vector<long double> target_table;
	limit=change_val_interval1*d;
	int rowid=0,i=0;

	while(rowid<row_size1){
		d1=d;
		long double sum = 0;
		while(d1>0){
			//cout<<start<<"\t";
            sum += table1[start];
			start+=change_val_interval1;
			d1--;
			rowid++;
		}
		if(rowid%limit==0){
			i=rowid;
			start=rowid;
		}
		else{
			i++;
			start=i;
		}
		//cout<<"\n";
		target_table.push_back(sum);
	}
	remove_variable(var1,eliminate);
	return new Factor(var1,target_table);
}


Factor* multiply_evidence_factors(vector<Factor*> &factor,int f, long double &pr_evid, bool ignore_processed = false){
	vector<long double> table;

    //cout<<ignore_processed<<"\n";

	//factor[0]->print_var();
	//factor[0]->print_table();
	int start = 0;
    while(!ignore_processed && factor[start]->is_processed)  start++;
	int l=(factor[start]->variables[0]).d;
    for(int i=0;i<l;i++){
        table.push_back(factor[start]->table[i]);
    }
	for(int j=start+1;j<f;j++){
        if(!ignore_processed && factor[j]->is_processed) continue;
		//factor[j]->print_var();
		//factor[j]->print_table();
		for(int i=0;i<l;i++){
			table[i] *= (factor[j]->table[i]);
		}
	}
	vector<Variable> var = factor[start]->variables;
	pr_evid = 0;
	for(int i=0; i<l; i++)
        pr_evid += table[i];
	return new Factor(var,table);
}

void print_edge(const vector<vector<bool> > &edge,int n){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++)
			cout<<edge[i][j]<<"\t";
		cout<<"\n";
	}
}

void print_factors(const vector<Factor*> &factors,int f){
	for(int i=0;i<f;i++){
		int sz=factors[i]->variables.size();
		for(int j=0;j<sz;j++){
			cout<<factors[i]->variables[j].value<<"\t";
		}
		cout<<"\n";
	}
}



