/*
Find the most likely state sequence using Hidden Markov Model
Input: Model (observation sequence, initial probability, observation probability, transition probability)
Output: Most likely state sequence
Algorithm: Viterbi decoding

Compiler: g++ 4.6.3

Author: Ramanathan Muthukaruppan
*/

#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<map>
#include<algorithm>
#include<string>
using namespace std;


//Print path by backtracking the path array
vector<int> get_path(const vector< vector<double> > &alpha_at_time, const vector< vector<int> > &path_id){
    int n = alpha_at_time[0].size();
    int t = alpha_at_time.size()-1;
    int final_state=0;
    vector<int> state_sequence;
    double alpha = alpha_at_time[t][0];
    for(int i=1;i<n;i++){
        if(alpha < alpha_at_time[t][i]){
            alpha = alpha_at_time[t][i];
            final_state = i;
        }
    }
    state_sequence.push_back(final_state);
    for(int i=t-1;i>=0;i--){
        final_state = path_id[i][final_state];
        state_sequence.push_back(final_state);
    }
    reverse(state_sequence.begin(),state_sequence.end());
    return state_sequence;
}


//Find most probable state sequence given the observation sequence, initial probability, observation probability, transition probability
vector<int> find_most_probable_path(const vector<string> &observation_sequence,const vector<double> &initial_probability,
const vector< vector<double> > &transition_probability,const vector< vector<double> > &observation_probability, map<string, int> &observation_symbols){

    int length = observation_sequence.size();
    int n = initial_probability.size();
    double alpha,max_alpha;
    vector< vector<int> > path_id;
    vector< vector<double> > alpha_at_time;
    int observation_symbol_id = observation_symbols[observation_sequence[0]];
    vector<double> temp;
    vector<int> pid;
    for(int i=0;i<n;i++){
        alpha = observation_probability[i][observation_symbol_id]*initial_probability[i];
        //cout<<"Alpha: "<<alpha<<"\n";
        temp.push_back(alpha);
    }
    alpha_at_time.push_back(temp);
    for(int t=1;t<length;t++){
        observation_symbol_id = observation_symbols[observation_sequence[t]];
        //cout<<"\n\nTime: "<<t<<"\n";
        temp.clear();
        pid.clear();
        for(int i=0;i<n;i++){
                alpha = 0;
                max_alpha = 0;
                int parent;
                for(int j=0;j<n;j++){

                        alpha = alpha_at_time[t-1][j]*transition_probability[j][i]*observation_probability[i][observation_symbol_id];
                        //cout<<alpha_at_time[t-1][j]<<"  "<<transition_probability[j][i]<<"  "<<observation_probability[i][observation_symbol_id]<<"  "<<alpha<<" "<<"\n";
                        if(max_alpha <= alpha){
                            max_alpha = alpha;
                            parent = j;
                        }
                }
                //cout<<"Alpha: "<<max_alpha<<"\n";
                //cout<<"Parent: "<<parent<<"\n";
                temp.push_back(max_alpha);
                pid.push_back(parent);
        }
        alpha_at_time.push_back(temp);
        path_id.push_back(pid);
    }
    vector<int> state_sequence = get_path(alpha_at_time,path_id);
    return state_sequence;
}


int main(int argc, char** argv){
    int n;
    double temp;
    ifstream model(argv[1]);
    model>>n;   //no of states

    //Reading states and its probabilities
    vector<double> initial_probablity;
    vector< vector<double> > transition_probability;
    for(int i=0;i<n;i++){
        model>>temp;
        initial_probablity.push_back(temp);
    }
    for(int i=0;i<n;i++){
        vector<double> tp;
        for(int j=0;j<n;j++){
            model>>temp;
            tp.push_back(temp);
        }
        transition_probability.push_back(tp);
    }

    //Reading observation and its probabilities
    int m;
    string symbol;
    model>>m;
    map<string, int> observation_symbols;
    vector< vector<double> > observation_probability;
    for(int i=0;i<m;i++){
        model>>symbol;
        observation_symbols[symbol]=i;
    }
    for(int i=0;i<n;i++){
        vector<double> op;
        for(int j=0;j<m;j++){
            model>>temp;
            op.push_back(temp);
        }
        observation_probability.push_back(op);
    }

    string os="";
    ifstream test(argv[2]);
    string buf;
    while(test.good()){
  	getline(test,buf);
		if(buf == "")  break;
		stringstream ss(buf);
		vector<string> observation_sequence;
		while(ss.good()){
		    string temp_s;
		    ss >> temp_s;
		    observation_sequence.push_back(temp_s);
		}
		if(observation_sequence.size()==0)  continue;
		//cout<<observation_sequence.size()<<"\n";
		vector<int> state_sequence = find_most_probable_path(observation_sequence,
		                        initial_probablity,
		                        transition_probability,
		                        observation_probability,
		                        observation_symbols);
        //cout<<"State Sequence: ";
		for(int i=0;i<state_sequence.size();i++){
		    cout<<state_sequence[i]+1<<"\t";
		}
		cout<<"\n";
    }
    return 0;
}
