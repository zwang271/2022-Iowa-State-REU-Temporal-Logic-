#include "utils.h"
#include "grammar.h"

/*
* Input: Two vectors of computation strings V1 and V2, comma separated
* Output: Computes pairwise string_intersect between all
*		  computation strings in V1 and V2
*/
vector<string> set_intersect(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = vector<string>();

	// Bit-wise 'and' all entries of v1 and v2
	for (int i = 0; i < v1.size(); ++i) {
		for (int j = 0; j < v2.size(); ++j) {
			string s = string_intersect(v1[i], v2[j], n);
			if (s != "") {
				v.push_back(s);
			}
		}
	}

	return v;
}


/*
* Input: Vectors V1 and V2 of computation strings
*		 N is number of proposition variables
* Output: Vector of disjoint computation strings
*/
vector<string> set_union(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = join(v1, v2);
	return right_or(v, 0, right_or_aux(v, n), n);
}


/*
* Prop_cons  ->  'T' | 'F'
*/
vector<string> reg_prop_cons(string s, int n)
{
	vector<string> v = vector<string>();
	if (s == "T") {
		v.push_back(string(n, 's'));
	}
	else if (s == "F") {
	}
	return v;
}


/*
* Prop_var -> �p� Num | '~' 'p' Num
*/
vector<string> reg_prop_var(string s, int n)
{
	int k = 0;
	vector<string> v = vector<string>();

	if (s[0] == 'p') {
		k = stoi(s.substr(1, s.length() - 1)) + 1;
		string temp = string(k - 1, 's') + "1" + string(n - k, 's');
		v.push_back(temp);
	}
	else if (s[0] == '~') {
		k = stoi(s.substr(2, s.length() - 2)) + 1;
		v.push_back(string(k - 1, 's') + '0' + string(n - k, 's'));
	}
	return v;
}


/*
* Nnf ->  ?(~) Prop_var | Prop_cons
*	                    | Unary_Temp_conn  Interval  Nnf
*
*	                    | '(' Assoc_Prop_conn �[�  Array_entry_Nnf  �]� ')'
*                       | �(� Nnf Binary_Prop_conn Nnf �)�
*                       | �(� Nnf Binary_Temp_conn  Interval Nnf �)
*/
vector<string> reg(string s, int n) {

	int len_s = s.length();

	// ?(~) Prop_var 
	if (Prop_var_check(s) or
		(Slice_char(s, 0) == "~" and Prop_var_check(Slice(s, 1, len_s - 1)))) {
		return reg_prop_var(s, n);
	}

	// Prop_cons
	if (Prop_cons_check(s)) {
		return reg_prop_cons(s, n);
	}

	//// Unary_Temp_conn  Interval  Nnf
	//if (Unary_Temp_conn_check(Slice_char(s, 0))) {
	//	int begin_interval = 1;
	//	int end_interval = 2;

	//	// Parse for end of interval
	//	while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
	//		end_interval = end_interval + 1;
	//	}

	//	string interval = Slice(s, begin_interval, end_interval);
	//	string alpha = Slice(s, end_interval + 1, len_s - 1);
	//	return Interval_check(interval) and Nnf_check(alpha);
	//}

	//// '(' Assoc_Prop_conn �[�  Array_entry  �]� ')'
	//if (Assoc_Prop_conn_check(Slice_char(s, 1))) {
	//	int begin_array = 2;
	//	int end_array = len_s - 2;

	//	string array_entry = Slice(s, begin_array + 1, end_array - 1);
	//	return Slice_char(s, 0) == "("
	//		and Slice_char(s, 2) == "["
	//		and Array_entry_check(array_entry)
	//		and Slice_char(s, len_s - 2) == "]"
	//		and Slice_char(s, len_s - 1) == ")";
	//}

	//// �(� Wff Binary_Prop_conn Wff �)� | �(� Wff Binary_Temp_conn Interval Wff �)
	//if (Slice_char(s, 0) == "(" and Slice_char(s, len_s - 1) == ")") {

	//	// Number of '(' in s
	//	int left_count = 0;
	//	// Number of ')' in s
	//	int right_count = 0;


	//	//    Parse for binary_conn_index in s

	//	//    When left_count == right_count and s[binary_conn_index] is a binary connective,
	//	//    we are done parsing and have found binary_conn_index.

	//	int binary_conn_index = 1;
	//	for (binary_conn_index = 1; binary_conn_index <= len_s - 1; ++binary_conn_index) {
	//		string c = Slice_char(s, binary_conn_index);

	//		if (c == "(") {
	//			++left_count;
	//		}

	//		if (c == ")") {
	//			++right_count;
	//		}

	//		// Done parsing for binary_conn_index.
	//		if (left_count == right_count and (Binary_Prop_conn_check(c) or Binary_Temp_conn_check(c))) {
	//			break;
	//		}
	//	}

	//	string binary_conn = Slice_char(s, binary_conn_index);

	//	// �(� Wff Binary_Prop_conn Wff �)�
	//	if (Binary_Prop_conn_check(binary_conn)) {
	//		string alpha = Slice(s, 1, binary_conn_index - 1);
	//		string beta = Slice(s, binary_conn_index + 1, len_s - 2);
	//		return Wff_check(alpha) and Wff_check(beta);
	//	}

	//	// �(� Wff Binary_Temp_conn Interval Wff �)
	//	if (Binary_Temp_conn_check(binary_conn)) {
	//		int begin_interval = binary_conn_index + 1;
	//		int end_interval = binary_conn_index + 2;

	//		// Parse for end of interval
	//		while (Slice_char(s, end_interval) != "]" and end_interval <= len_s - 1) {
	//			end_interval = end_interval + 1;
	//		}

	//		string alpha = Slice(s, 1, binary_conn_index - 1);
	//		string interval = Slice(s, begin_interval, end_interval);
	//		string beta = Slice(s, end_interval + 1, len_s - 2);
	//		return Wff_check(alpha) and Interval_check(interval) and Wff_check(beta);
	//	}
	//}

	return vector<string>();
}