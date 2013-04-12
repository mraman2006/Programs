//Used by sort function
bool compare_neighbors(pair<int,int> p1, pair<int,int> p2){
    return p1.second > p2.second;
}

//Get the variables list in the graph ordered by min degree
void get_min_order(vector<vector<bool> > &edge, vector<int> &var_order, const vector<int> &eliminated_list){
    int n = edge.size();
    vector< pair<int,int> > neighbors;
    map<int,bool> exclude;

    for(int i=0; i<eliminated_list.size(); i++){
        exclude[ eliminated_list[i] ] = true;
    }

    for(int i=0; i<n; i++){
        if(exclude[i])  continue;
        pair<int,int> p;
        int sum = 0;
        for(int j=0; j<n; j++){
            if(edge[i][j])  sum++;
        }
        remove_edges(edge,i,n);
        neighbors.push_back(make_pair(i,sum));
    }
    sort(neighbors.begin(),neighbors.end(),compare_neighbors);
    for(int i=0; i<neighbors.size(); i++){
        var_order.push_back(neighbors[i].first);
    }
    return;
}

//Find the clusters according to the min degree order
void find_clusters(vector< set<int> > &bucket, const vector<Factor*> &factor, const vector<int> &var_order){
    vector<bool> visited_factors(factor.size(),false);
    for(int k=0; k<var_order.size(); k++){
        int eliminate = var_order[k];
        set<int> temp_var_list;
        for(int i=0;i<factor.size();i++){
            if(visited_factors[i])  continue;
  		int sz = (factor[i]->variables).size();
			bool found = false;
			for(int j=0;j<sz;j++){
				if((factor[i]->variables[j]).value==eliminate){
					found = true;
					break;
				}
			}
			if(found){
                visited_factors[i] = true;
                for(int j=0;j<sz;j++){
                    temp_var_list.insert((factor[i]->variables[j]).value);
                }
            }
		}
        bucket.push_back(temp_var_list);
    }
    return;
}


//Check if width of each cluster is equal to the wCutSet
bool check_width(const vector<set<int> > &bucket,int w){
    for(int i=0; i<bucket.size(); i++){
        if(bucket[i].size() > w)    return true;
    }
    return false;
}

//Find the most frequently occuring variable among all the clusters
int find_most_occuring_variable(const vector<set<int> > &bucket){
    //cout<<"Find most occurence\n";
    map<int,int> m;
    for(int i=0; i<bucket.size(); i++){
        set<int> s = bucket[i];
        set<int>::iterator it = s.begin();
        while(it != s.end()){
            m[*it]++;
            it++;
        }
    }
    map<int,int>::iterator it = m.begin();
    int frequent_variable, max_occurence = -1;
    while(it != m.end()){
        if(max_occurence < (*it).second){
            max_occurence = (*it).second;
            frequent_variable = (*it).first;
        }
        it++;
    }
    //cout<<frequent_variable<<"\n";
    return frequent_variable;
}


//Remove the most frequently occuring variable from all the clusters
void remove_most_occuring_variable(vector< set<int> > &bucket, int e){
    for(int i=0; i<bucket.size(); i++){
        bucket[i].erase(e);
    }
    return;
}


//Compute the variables to be present in the wCutSet
vector<int> wCutSet(const vector<Factor*> &factors,const vector<int> &var_order,int w){
    vector< set<int> > bucket;
    find_clusters(bucket,factors,var_order);
    vector<int> evidence_variable_id;
    while(check_width(bucket,w)){   // Returns true if any of the bucket sizes is greater than w;
        int e = find_most_occuring_variable(bucket);
        evidence_variable_id.push_back(e);
        remove_most_occuring_variable(bucket,e);
    }
    return evidence_variable_id;
}


//Assign values to the variables in wCutSet using the sampling of Q distribution
map<int,int> sample_evidence(const vector<int> &cutset_evidence,vector<Variable> &v, const vector< vector<double> > &Q_distribution, int random_seed){
    map<int,int> m;
    vector<int> domain_values;
    vector<Variable>::iterator it = v.begin();
    for(int i=0; i<cutset_evidence.size(); i++){
        while(cutset_evidence[i] != (*it).value){
            it++;
        }
        domain_values.push_back((*it).d);
    }
    int r,domain,j;
    double q_value,sum;
    for(int i=0; i<cutset_evidence.size(); i++){
        r = rand();
        q_value = ((double)r) / RAND_MAX;
        domain = Q_distribution[i].size();
        sum = 0;
        for(j=0; j<domain; j++){
            sum += Q_distribution[i][j];
            if(q_value < sum)   break;
        }
        m[cutset_evidence[i]] = j;
    }
    return m;
}


/*Update Q using the adaptive distribution:
Q(X = x)
for all samples generated upto t:
    if (x is present in the sample Xt)
        Add weight(Xt)
Normalize the summation using the combined weight of all samples
*/

void updateQ(vector< vector<double> > &Q_distribution, vector< map<int,int> > &samples_upto_t,const vector<int> &cutset_evidence, const vector<long double> &w_t){
    int variables_size = Q_distribution.size();
    for(int i=0; i<variables_size; i++){
        int domain = Q_distribution[i].size();
        map<int,double> m;
        int samples = samples_upto_t.size();
        int var = cutset_evidence[i];   //Variable to be updated in Q_distribution
        double sum = 0;
        for(int k=0; k<samples; k++){
            int value = samples_upto_t[k][var]; //Value assigned to a variable at that particular sample
            m[value] += w_t[k];
            sum += w_t[k];
        }
        if(sum==0)  return;
        for(int j=0; j<domain; j++){
            Q_distribution[i][j] = m[j] / sum;
        }
    }
    return;
}

//Compute the value of Q(X=x) = Q(x1) * Q(x2) * .... * Q(xn)
double compute_Q(vector< vector<double> > &Q_distribution, map<int,int> evid_map){
    double q = 1;
    int i = 0;
    map<int,int>::iterator it = evid_map.begin();
    while(it != evid_map.end()){
        q = q * Q_distribution[i][(*it).second];
        it++;
        i++;
    }
    return q;
}

//Initialize Q vector with uniform distribution
void initialize_Q(vector< vector<double> > &Q_distribution,const vector<int> &cutset_evidence, const vector<Variable> &v){
    vector<Variable>::const_iterator it = v.begin();
    int sz = cutset_evidence.size();
    for(int i=0; i<sz; i++){
        while((*it).value != cutset_evidence[i])    it++;
        int domain = (*it).d;
        vector<double> temp;
        for(int i=0; i<domain; i++){
            temp.push_back(1.0/domain);
        }
        Q_distribution.push_back(temp);
    }
    return;
}

