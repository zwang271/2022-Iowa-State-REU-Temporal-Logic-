#include "utils.h"
#include "grammar.h"
#include "nnf_grammar.h"
#include "reg.h"
#include <iostream>

/*
 * Input: Two vectors of computation strings V1 and V2, comma separated
 * Output: Computes pairwise string_intersect between all
 *		  computation strings in V1 and V2
 */
vector<string> set_intersect(vector<string> v1, vector<string> v2, int n, bool simp) {
	vector<string> v = vector<string>();
	if (v1 == v or v2 == v) { // Currently v is the empty vector
		return v;
	}

	//int len_w = int(max(v1[0].length(), v2[0].length()));
	//v1 = pad(v1, n, len_w); v2 = pad(v2, n, len_w);

	// CORRECTION TO VECTOR PADDING LINE
	int len_w = 0;

	// Gets length of longest string in v1
	for (string w : v1) {
		if (w.length() > len_w) {
			len_w = w.length();
		}
	}
	// Gets length of longest string in join(v1, v2)
	for (string w : v2) {
		if (w.length() > len_w) {
			len_w = w.length();
		}
	}

	v1 = pad(v1, n, len_w); v2 = pad(v2, n, len_w);

	// Bit-wise 'and' all entries of v1 and v2
	for (int i = 0; i < v1.size(); ++i) {
		for (int j = 0; j < v2.size(); ++j) {
			string s = string_intersect(v1[i], v2[j], n);
			if (s != "") {
				v.push_back(s);
			}
		}
	}

	v1.clear();
	v1.shrink_to_fit();
	v2.clear();
	v2.shrink_to_fit();

	if (simp) {
		//return simplify(right_expand(v, n), n);
		return simplify(v, n);
	}
	return v;
}


/*
 * Input: Vectors A and B of computation strings
 * Output: Vector A concatenated with B
 */
vector<string> join(vector<string> A, vector<string> B, int n, bool simp) {
	vector<string> AB;
	AB.reserve(A.size() + B.size()); // preallocate memory
	AB.insert(AB.end(), A.begin(), A.end());
	AB.insert(AB.end(), B.begin(), B.end());
	A.clear();
	A.shrink_to_fit();
	B.clear();
	B.shrink_to_fit();

	if (simp) {
		//return simplify(right_expand(AB, n), n);
		return simplify(AB, n);
	}
	return AB;
}


/*
 * Input: Vectors V1 and V2 of computation strings
 *		 n is number of propositional variables
 * Output: Vector of disjoint computation strings
 */
vector<string> set_union(vector<string> v1, vector<string> v2, int n) {
	vector<string> v = join(v1, v2, n);

	v1.clear();
	v1.shrink_to_fit();
	v2.clear();
	v2.shrink_to_fit();
	return right_expand(v, n);
}


/*
 * Prop_cons  ->  'T' | '!'
 * Input: mLTL formula in NNF form (string)
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying "T" or "!"
 */
vector<string> reg_prop_cons(string nnf, int n) {
	if ((nnf == "T") and (n != 0)) {
		return { string(n, 's') };
	}
	else if (nnf == "!" or n == 0) {
		return {};
	} //do nothing
	else {
		string error = nnf + " is not a prop cons";
		throw invalid_argument(error);
	}
	return {};
}


/*
 * Prop_var -> 'p' Num | '~' 'p' Num
 * Input: mLTL formula in NNF form (string)
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying a prop. var.
 */
vector<string> reg_prop_var(string nnf, int n)
{
	int k;

	if (Slice_char(nnf, 0) == "p") {
		k = stoi(nnf.substr(1, nnf.length() - 1));
		string ret = string(k, 's') + "1" + string(n - k - 1, 's');
		return { ret };
	}
	else if (Slice_char(nnf, 0) == "~") {
		k = stoi(nnf.substr(2, nnf.length() - 2));
		string ret = string(k, 's') + "0" + string(n - k - 1, 's');
		return { ret };
	}
	else {
		string error = nnf + " is not a prop var";
		throw invalid_argument(error);
	}
	return {};
}

/*
 * Input: vector reg_alpha of computation strings satisfying MLTL NNF formula alpha
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying F[a,b]alpha
 */
vector<string> reg_F(vector<string> reg_alpha, int a, int b, int n) {
	vector<string> comp = vector<string>();

	string pre = "";
	vector<string> temp_alpha = vector<string>();

	// pre = (s^n ,)^a
	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	// calculate comp = join_{i = a:b} (s^n,)^i alpha
	//				  = join_{i = 0:b-a} (s^n,)^a+i alpha
	//				  = (s^n,)^a join_{i = 0:b-a} (s^n,)^i alpha
	for (int i = 0; i <= b - a; ++i) {
		string w = "";
		for (int j = 0; j < i; ++j) {
			w += string(n, 's') + ",";
		}
		// Now w = (s^n ,)^i

		temp_alpha = list_str_concat_prefix(reg_alpha, w);
		comp = join(comp, temp_alpha, n);
	}
	comp = list_str_concat_prefix(comp, pre);

	// return comp = (s^n,)^a join_{i = 0:b-a} (s^n,)i alpha
	reg_alpha.clear();
	reg_alpha.shrink_to_fit();
	temp_alpha.clear();
	temp_alpha.shrink_to_fit();
	return comp;
}


/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying G[a,b]alpha
 */
vector<string> reg_G(vector<string> reg_alpha, int a, int b, int n)
{
	string pre = "";
	vector<string> temp_alpha = vector<string>();
	// pre = (s^n ,)^a
	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	// Initialize comp = alpha to prevent intersection with empty vector
	vector<string> comp = reg_alpha;

	// calculate comp = set_intersect_{i = a:b} (s^n,)^i alpha
	//				  = set_intersect_{i = 0:b-a} (s^n,)^a+i alpha
	//				  = (s^n,)^a set_intersect_{i = 0:b-a} (s^n,)^i alpha
	//				  = ((s^n,)^a alpha) set_intersect (set_intersect{i = 1:b-a} (s^n,)^i alpha)
	for (int i = 1; i <= b - a; ++i) {
		string w = "";
		for (int j = 0; j < i; ++j) {
			w += string(n, 's') + ",";
		}
		// Now w = (s^n ,)^i

		temp_alpha = list_str_concat_prefix(reg_alpha, w);
		comp = set_intersect(comp, temp_alpha, n);
		/*cout << i << "\t size: " << comp.size() << endl;
		print(comp);*/
	}
	comp = list_str_concat_prefix(comp, pre);

	// return comp = (s^n,)^a join_{i = 0:b-a} (s^n,)i alpha
	reg_alpha.clear();
	reg_alpha.shrink_to_fit();
	temp_alpha.clear();
	temp_alpha.shrink_to_fit();
	return comp;
}

/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      vector reg_beta of comp. strings satisfying mLTL NNF formula beta
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying alphaU[a,b]beta
 */
vector<string> reg_U(vector<string> reg_alpha, vector<string> reg_beta, int a, int b, int n) {
	string pre = "";
	// pre = (s^n,)^a
	for (int i = 0; i < a; ++i) {
		pre += string(n, 's') + ",";
	}

	// initialize comp = G[a,a] beta
	vector<string> comp = list_str_concat_prefix(reg_beta, pre);

	// Calculating comp = alpha U[a,b] beta
	//					= join_{i = a:b} (G[a,i-1] alpha) set_intersect (G[i,i] beta)
	//					= (G[a,a] beta) join join_{i = a:b-1} (G[a,i] alpha) 
	//						set_intersect (G[i+1,i+1] beta)
	for (int i = a; i <= b - 1; ++i) {
		comp = join(comp, set_intersect(
			reg_G(reg_alpha, a, i, n), reg_G(reg_beta, i + 1, i + 1, n), n), n);
	}

	// Return comp = (G[a,a] beta) join join_{i = a:b-1} (G[a,i] alpha) 
	//					set_intersect (G[i+1,i+1] beta)
	reg_alpha.clear();
	reg_alpha.shrink_to_fit();
	reg_beta.clear();
	reg_beta.shrink_to_fit();
	return comp;
}

/*
 * Input: vector reg_alpha of computation strings satisfying mLTL NNF formula alpha
 *      vector reg_beta of comp. strings satisfying mLTL NNF formula beta
 *      a is lower bound of interval
 *      b is upper bound of interval
 *      n is number of propositional variables
 * Output: Vector of computation strings satisfying alphaR[a,b]beta
 */
vector<string> reg_R(vector<string> alpha, vector<string> beta, int a, int b, int n) {
	// initialize comp = G[a,b] beta
	vector<string> comp = reg_G(beta, a, b, n);

	// Calculating comp = alpha U[a,b] beta
	//					= (G[a,b] beta) join join_{i = a:b-1}G[a,i] alpha set_intersect G[i,i] alpha
	for (int i = a; i <= b - 1; ++i) {
		comp = join(comp,
			set_intersect(reg_G(beta, a, i, n), reg_G(alpha, i, i, n), n), n);
	}
	alpha.clear();
	alpha.shrink_to_fit();
	beta.clear();
	beta.shrink_to_fit();
	return comp;
}


/*
 * Nnf ->  ?(~) Prop_var | Prop_cons
 *	                     | Unary_Temp_conn  Interval  Nnf
 *
 *	                     | '(' Assoc_Prop_conn '['  Nnf_Array_entry  ']' ')'
 *                       | '(' Nnf Binary_Prop_conn Nnf ')'
 *                       | '(' Nnf Binary_Temp_conn  Interval Nnf ')'
 * Input: mLTL formula in NNF (string)
 *     n is number of propositional variables
 * Output: Vector of computation strings satisfying the formula
 */
vector<string> reg(string nnf, int n) {
	int len_nnf = int(nnf.length());

	// ?(~) Prop_var 
	if (Prop_var_check(nnf) or
		(Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1)))) {
		return reg_prop_var(nnf, n);
        // if (want_subformilas)
        // v = reg_prop_var(nnf, n)
        // if nnf not in formulas
        //      t = make_tuple(nnf, v)
        //      formulas.push_back(t)
        //return v
	}

	// Prop_cons
	if (Prop_cons_check(nnf)) {
		return reg_prop_cons(nnf, n);
	}

	// Unary_Temp_conn  Interval  Nnf
	if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
		string unary_temp_con = Slice_char(nnf, 0);

		tuple<int, int, int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple);
		int comma_index = get<1>(interval_tuple);
		int end_interval = get<2>(interval_tuple);
		int a = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
		int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
		string alpha = Slice(nnf, end_interval + 1, len_nnf - 1);
		vector<string> reg_alpha = reg(alpha, n); // recursive call
        //push_back_formulas(alpha, v);

		// Empty interval
		if (a > b) {
			// vacously false
			if (unary_temp_con == "F") {
				return {};
			}

			// vacously true
			if (unary_temp_con == "G") {
				string ret_string = string(n, 's');
				return { ret_string };
			}
		}

		// Unary_Temp_conn -> 'F'
		if (unary_temp_con == "F") {
			return reg_F(reg_alpha, a, b, n);
		}

		// Unary_Temp_conn -> 'G'
		if (unary_temp_con == "G") {
            //vector<string> v = reg_G(reg_alpha, a, b, n);
            //push_back_formulas(alpha, v);
            //return v;
			return reg_G(reg_alpha, a, b, n);
		}
	}

	// '(' Assoc_Prop_conn '['  Nnf_Array_entry ']' ')'
	if (Assoc_Prop_conn_check(Slice_char(nnf, 1))) {
		string assoc_prop_conn = Slice_char(nnf, 1);

		// (...((Nnf_1 assoc_prop_conn Nnf_2) assoc_prop_conn Nnf_3) ... assoc_prop_conn Nnf_n)
		// is equiv to (assoc_prop_conn [Nnf_1, ..., Nnf_n])
		int begin_entry = 3;
		string equiv_formula = "";
		// Parsing for Nff between each comma
		for (int end_entry = 3; end_entry <= len_nnf - 1; ++end_entry) {
			if (Nnf_check(Slice(nnf, begin_entry, end_entry))) {
				string alpha = Slice(nnf, begin_entry, end_entry);

				// First entry obtained
				if (begin_entry == 3) {
					// Add Nnf_1 to equiv_formula
					equiv_formula = equiv_formula + alpha;
				}

				// Not first entry
				else {
					// Add Nnf_n to equiv_formula, where n >= 2
					equiv_formula = "(" + equiv_formula + assoc_prop_conn + alpha + ")";
				}

				// Update begin_entry so it has index of the first char of the next entry.
				begin_entry = end_entry + 2;
			}
		}

		return reg(equiv_formula, n);
	}

	// '(' Nnf Binary_Prop_conn Nnf ')' | '(' Nnf Binary_Temp_conn Interval Nnf ')'
	int binary_conn_index = primary_binary_conn(nnf);
	string binary_conn = Slice_char(nnf, binary_conn_index);

	// '(' Nnf Binary_Prop_conn Nnf ')'
	if (Binary_Prop_conn_check(binary_conn)) {
		string alpha = Slice(nnf, 1, binary_conn_index - 1);
		vector<string> reg_alpha = reg(alpha, n);
		string beta = Slice(nnf, binary_conn_index + 1, len_nnf - 2);
		vector<string> reg_beta = reg(beta, n);

		if (binary_conn == "&") {
			return set_intersect(reg_alpha, reg_beta, n);
		}

		if (binary_conn == "v") {
			return join(reg_alpha, reg_beta, n);
		}

		if (binary_conn == "=") {
			// (alpha = beta) is equiv to ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta)))
			// ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta))) is in Nnf-form
			string equiv_nnf_formula = "((" + alpha + "&" + beta + ")v("
				+ Wff_to_Nnf_clean("~" + alpha) + "&"
				+ Wff_to_Nnf_clean("~" + beta) + "))";
			return reg(equiv_nnf_formula, n);
		}

		if (binary_conn == ">") {
			// (alpha > beta) is equiv to (Wff_to_Nnf(~alpha) v beta))
			// (Wff_to_Nnf(~alpha) v beta)) is in Nnf-form
			string equiv_nnf_formula = "(" + Wff_to_Nnf_clean("~" + alpha) + "v" + beta + ")";
			return reg(equiv_nnf_formula, n);
		}
	}

	// '(' Nnf Binary_Temp_conn Interval Nnf ')'
	if (Binary_Temp_conn_check(binary_conn)) {
		tuple<int, int, int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple);
		int comma_index = get<1>(interval_tuple);
		int end_interval = get<2>(interval_tuple);
		int a = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
		int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
		string alpha = Slice(nnf, 1, binary_conn_index - 1);
		string beta = Slice(nnf, end_interval + 1, len_nnf - 2);

		if (binary_conn == "U") {
			return reg_U(reg(alpha, n), reg(beta, n), a, b, n);
		}
		else if (binary_conn == "R") {
			return reg_R(reg(alpha, n), reg(beta, n), a, b, n);
		}
	}

	else {
		string error_string = nnf + " is not in Negation-normal form.\n";
		throw invalid_argument(error_string);
	}
	return {};
}



// Given an Nnf-formula nnf, returns the regex
// for the language of nnf.
// For preformance reasons, reg_clean will return a
// NOT NECESSARILY DISJOINT Union.
// To obtain a disjoint union from reg_clean's output,
// use right_expand.
//
// Cleaner implementation of reg
// in case original is faulty.
vector<string> reg_clean(string nnf, int n) {
	int len_nnf = int(nnf.length());

	// ?(~) Prop_var 
	if (Prop_var_check(nnf) or
		(Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1)))) {

		// Prop_var -> 'p' Num		
		if (Prop_var_check(nnf)) {
			string num = Slice(nnf, 1, len_nnf - 1);
			int k = stoi(num);
			string ret_string = string(k, 's') + "1" + string(n - k - 1, 's');

			//Output
			return { ret_string };
		}

		// '~' Prop_var -> '~' 'p' Num
		if (Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1))) {
			string num = Slice(nnf, 2, len_nnf - 1);
			int k = stoi(num);
			string ret_string = string(k, 's') + '0' + string(n - k - 1, 's');
			return { ret_string };
		}
	}

	// Prop_cons
	if (Prop_cons_check(nnf)) {
		// Prop_cons -> 'T'
		if (nnf == "T") {
			string ret_string = string(n, 's');
			return { ret_string };
		}

		// Prop_cons -> '!'
		if (nnf == "!") {
			return {};
		}
	}

	// Unary_Temp_conn  Interval  Nnf
	if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
		string unary_temp_conn = Slice_char(nnf, 0);

		tuple<int, int, int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple);
		int comma_index = get<1>(interval_tuple);
		int end_interval = get<2>(interval_tuple);
		int lower_bound = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
		int upper_bound = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
		string alpha = Slice(nnf, end_interval + 1, len_nnf - 1);
		vector<string> reg_alpha = reg_clean(alpha, n);

		// Empty interval
		if (lower_bound > upper_bound) {
			// vacously false
			if (unary_temp_conn == "F") {
				return {};
			}

			// vacously true
			if (unary_temp_conn == "G") {
				string ret_string = string(n, 's');
				return { ret_string };
			}

		}


		vector<string> reg_nnf = {};
		string pre = "";

		// [0, lower_bound-1] time steps don't matter.
		for (int i = 0; i <= lower_bound - 1; ++i) {
			pre += string(n, 's') + ",";
		}


		// Determine which operation to use based on initial
		// Unary Temp character

		// If Unary_Temp_conn is 'F', operation is join
		if (unary_temp_conn == "F") {
			// Compute regex for 'F' [0, upper_bound-lower_bound] alpha
			// which is: reg_nnf = join [reg(alpha), 's...s' + reg(alpha), ..., ('s...s')^(upper_bound-lower_bound) + reg(alpha)] 
			for (int i = 0; i <= upper_bound - lower_bound; ++i) {
				// Vector for ('s...s')^(i) + reg(alpha)
				vector<string> current_step_alpha = list_str_concat_prefix(reg_alpha, string(i, 's') + ",");

				// Join current_step_alpha to reg_nnf
				reg_nnf = join(reg_nnf, current_step_alpha, n);
			}
		}

		// If Unary_Temp_conn is 'G', operation is set_intersection
		if (unary_temp_conn == "G") {
			// Compute regex for 'G' [0, upper_bound-lower_bound] alpha
			// which is: reg_nnf = set_intersection [reg(alpha), 's...s' + reg(alpha), ..., ('s...s')^(upper_bound-lower_bound) + reg(alpha)] 
			for (int i = 0; i <= upper_bound - lower_bound; ++i) {
				// Vector for ('s...s')^(i) + reg(alpha)
				vector<string> current_step_alpha = list_str_concat_prefix(reg_alpha, string(i, 's') + ",");

				// Set_intersect current_step_alpha to reg_nnf
				reg_nnf = set_intersect(reg_nnf, current_step_alpha, n);
			}
		}

		// Regex for unary_temp_conn [lower_bound, upper_bound] alpha is pre + reg_nnf
		return list_str_concat_prefix(reg_alpha, pre);
	}

	// '(' Assoc_Prop_conn '['  Nnf_Array_entry ']' ')'
	if (Assoc_Prop_conn_check(Slice_char(nnf, 1))) {
		string assoc_prop_conn = Slice_char(nnf, 1);

		// (...((Nnf_1 assoc_prop_conn Nnf_2) assoc_prop_conn Nnf_3) ... assoc_prop_conn Nnf_n)
		// is equiv to (assoc_prop_conn [Nnf_1, ..., Nnf_n])
		int begin_entry = 3;
		string equiv_formula = "";
		for (int end_entry = 3; end_entry <= len_nnf - 1; ++end_entry) {
			if (Nnf_check(Slice(nnf, begin_entry, end_entry))) {
				string alpha = Slice(nnf, begin_entry, end_entry);

				// First entry obtained
				if (begin_entry == 3) {
					// Add Nnf_1 to equiv_formula
					equiv_formula = equiv_formula + alpha;
				}

				// Not first entry
				else {
					// Add Nnf_n to equiv_formula, where n >= 2
					equiv_formula = "(" + equiv_formula + assoc_prop_conn + alpha + ")";
				}

				// Update begin_entry so it has index of the first char of the next entry.
				begin_entry = end_entry + 2;
			}
		}

		return reg_clean(equiv_formula, n);
	}

	// '(' Nnf Binary_Prop_conn Nnf ')' | '(' Nnf Binary_Temp_conn Interval Nnf ')'
	int binary_conn_index = primary_binary_conn(nnf);
	string binary_conn = Slice_char(nnf, binary_conn_index);

	// '(' Nnf Binary_Prop_conn Nnf ')'
	if (Binary_Prop_conn_check(binary_conn)) {
		string alpha = Slice(nnf, 1, binary_conn_index - 1);
		vector<string> reg_alpha = reg_clean(alpha, n);
		string beta = Slice(nnf, binary_conn_index + 1, len_nnf - 2);
		vector<string> reg_beta = reg_clean(beta, n);


		if (binary_conn == "&") {
			return set_intersect(reg_alpha, reg_beta, n);
		}


		if (binary_conn == "v") {
			return join(reg_alpha, reg_beta, n);
		}


		if (binary_conn == "=") {
			// (alpha = beta) is equiv to ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta)))
			// ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta))) is in Nnf-form
			string equiv_nnf_formula = "((" + alpha + "&" + beta + ")v("
				+ Wff_to_Nnf_clean("~" + alpha) + "&"
				+ Wff_to_Nnf_clean("~" + beta) + "))";
			return reg_clean(equiv_nnf_formula, n);
		}


		if (binary_conn == ">") {
			// (alpha > beta) is equiv to (Wff_to_Nnf(~alpha) v beta))
			// (Wff_to_Nnf(~alpha) v beta)) is in Nnf-form
			string equiv_nnf_formula = "(" + Wff_to_Nnf_clean("~" + alpha) + "v" + beta + ")";
			return reg_clean(equiv_nnf_formula, n);
		}
	}

	// '(' Nnf Binary_Temp_conn Interval Nnf ')'
	if (Binary_Temp_conn_check(binary_conn)) {
		tuple<int, int, int> interval_tuple = primary_interval(nnf);
		int begin_interval = get<0>(interval_tuple);
		int comma_index = get<1>(interval_tuple);
		int end_interval = get<2>(interval_tuple);
		int lower_bound = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
		int upper_bound = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
		string alpha = Slice(nnf, 1, binary_conn_index - 1);
		string beta = Slice(nnf, end_interval + 1, len_nnf - 2);

		if (binary_conn == "U") {
			// '(' alpha 'U' [a,b] beta ')' is equiv
			// to ( (v[F [a,a-1] Wff_to_Nnf(~alpha), ..., F [a,b-1] Wff_to_Nnf(~alpha)]) v F [a,b] beta).
			// ( (v[F [a,a-1] Wff_to_Nnf(~alpha), ..., F [a,b-1] Wff_to_Nnf(~alpha)]) v F [a,b] beta) is
			// in Nnf-form.
			string equiv_nnf_formula = "";
			string nnf_neg_alpha = Wff_to_Nnf_clean("~" + alpha);
			for (int i = lower_bound - 1; i <= upper_bound - 1; ++i) {
				// Add F [a,i] Wff_to_Nnf(~alpha) to equiv_nnf_formula
				equiv_nnf_formula = equiv_nnf_formula + "F["
					+ to_string(lower_bound) + "," + to_string(i) + "]" + nnf_neg_alpha + ",";
			}

			// Remove extra comma
			equiv_nnf_formula = Slice(equiv_nnf_formula, 0, int(equiv_nnf_formula.length() - 2));

			equiv_nnf_formula = "((v[" + equiv_nnf_formula + "])"
				+ "vF[" + to_string(lower_bound) + "," + to_string(upper_bound) + "]" + beta + ")";
			return reg_clean(equiv_nnf_formula, n);
		}


		if (binary_conn == "R") {
			// '(' alpha 'R' [a,b] beta ')' is equiv
			// to ( (&[G [a,a-1] Wff_to_Nnf(~alpha), ..., G [a,b-1] Wff_to_Nnf(~alpha)]) & G [a,b] beta).
			// ( (&[G [a,a-1] Wff_to_Nnf(~alpha), ..., G [a,b-1] Wff_to_Nnf(~alpha)]) & G [a,b] beta) is
			// in Nnf-form.
			string equiv_nnf_formula = "";
			string nnf_neg_alpha = Wff_to_Nnf_clean("~" + alpha);
			for (int i = lower_bound - 1; i <= upper_bound - 1; ++i) {
				// Add G [a,i] Wff_to_Nnf(~alpha) to equiv_nnf_formula
				equiv_nnf_formula = equiv_nnf_formula + "G[" + to_string(lower_bound) + "," + to_string(i) + "]" + nnf_neg_alpha + ",";
			}

			// Remove extra comma
			equiv_nnf_formula = Slice(equiv_nnf_formula, 0, int(equiv_nnf_formula.length() - 2));

			equiv_nnf_formula = "((&[" + equiv_nnf_formula + "])"
				+ "&G[" + to_string(lower_bound) + "," + to_string(upper_bound) + "]" + beta + ")";
			return reg_clean(equiv_nnf_formula, n);
		}
	}

	else {
		string error_string = nnf + " is not in Negation-normal form.\n";
		throw invalid_argument(error_string);
	}
	return vector<string>();
}

vector<tuple<string, vector<string>>> FORMULAS;

vector<tuple<string, vector<string>>> get_formulas() {
    return FORMULAS;
}

bool find_formula(vector<tuple<string, vector<string>>> regex, string str) {
    bool in_vec = false;
    for (int i = 0; i < regex.size(); ++i) {
        if (get<0>(regex[i]) == str) {
            in_vec = true;
        }
    }
    return in_vec;
}


void push_back_formulas(string s, vector<string> v) {
    if (!find_formula(FORMULAS, s)) {
        tuple<string, vector<string>> tuple = make_tuple(s, v);
        FORMULAS.push_back(tuple);
    }
}

vector<string> reg_subformulas(string nnf, int n) {
    int len_nnf = int(nnf.length());

    // ?(~) Prop_var
    if (Prop_var_check(nnf) or
        (Slice_char(nnf, 0) == "~" and Prop_var_check(Slice(nnf, 1, len_nnf - 1)))) {
        //return reg_prop_var(nnf, n);
        push_back_formulas(nnf, reg_prop_var(nnf, n));
        return reg_prop_var(nnf, n);
    }

    // Prop_cons
    if (Prop_cons_check(nnf)) {
        push_back_formulas(nnf, reg_prop_cons(nnf, n));
        return reg_prop_cons(nnf, n);
    }

    // Unary_Temp_conn  Interval  Nnf
    if (Unary_Temp_conn_check(Slice_char(nnf, 0))) {
        string unary_temp_con = Slice_char(nnf, 0);

        tuple<int, int, int> interval_tuple = primary_interval(nnf);
        int begin_interval = get<0>(interval_tuple);
        int comma_index = get<1>(interval_tuple);
        int end_interval = get<2>(interval_tuple);
        int a = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
        int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
        string alpha = Slice(nnf, end_interval + 1, len_nnf - 1);
        vector<string> reg_alpha = reg(alpha, n); // recursive call

        // Empty interval
        if (a > b) {
            // vacously false
            if (unary_temp_con == "F") {
                return {};
            }

            // vacously true
            if (unary_temp_con == "G") {
                string ret_string = string(n, 's');
                return { ret_string };
            }
        }

        // Unary_Temp_conn -> 'F'
        if (unary_temp_con == "F") {
            push_back_formulas(alpha, reg_F(reg_alpha, a, b, n));
            return reg_subformulas(alpha, n);
            //return reg_F(reg_alpha, a, b, n);
        }

        // Unary_Temp_conn -> 'G'
        if (unary_temp_con == "G") {
            push_back_formulas(alpha, reg_G(reg_alpha, a, b, n));
            return reg_subformulas(alpha, n);
            //return reg_G(reg_alpha, a, b, n);
        }
    }

    // '(' Assoc_Prop_conn '['  Nnf_Array_entry ']' ')'
    if (Assoc_Prop_conn_check(Slice_char(nnf, 1))) {
        string assoc_prop_conn = Slice_char(nnf, 1);

        // (...((Nnf_1 assoc_prop_conn Nnf_2) assoc_prop_conn Nnf_3) ... assoc_prop_conn Nnf_n)
        // is equiv to (assoc_prop_conn [Nnf_1, ..., Nnf_n])
        int begin_entry = 3;
        string equiv_formula = "";
        // Parsing for Nff between each comma
        for (int end_entry = 3; end_entry <= len_nnf - 1; ++end_entry) {
            if (Nnf_check(Slice(nnf, begin_entry, end_entry))) {
                string alpha = Slice(nnf, begin_entry, end_entry);

                // First entry obtained
                if (begin_entry == 3) {
                    // Add Nnf_1 to equiv_formula
                    equiv_formula = equiv_formula + alpha;
                }

                // Not first entry
                else {
                    // Add Nnf_n to equiv_formula, where n >= 2
                    equiv_formula = "(" + equiv_formula + assoc_prop_conn + alpha + ")";
                }

                // Update begin_entry so it has index of the first char of the next entry.
                begin_entry = end_entry + 2;
            }
        }
        push_back_formulas(equiv_formula, reg(equiv_formula, n));
        return reg_subformulas(equiv_formula, n);
    }

    // '(' Nnf Binary_Prop_conn Nnf ')' | '(' Nnf Binary_Temp_conn Interval Nnf ')'
    int binary_conn_index = primary_binary_conn(nnf);
    string binary_conn = Slice_char(nnf, binary_conn_index);

    // '(' Nnf Binary_Prop_conn Nnf ')'
    if (Binary_Prop_conn_check(binary_conn)) {
        string alpha = Slice(nnf, 1, binary_conn_index - 1);
        vector<string> reg_alpha = reg(alpha, n);
        string beta = Slice(nnf, binary_conn_index + 1, len_nnf - 2);
        vector<string> reg_beta = reg(beta, n);

        if (binary_conn == "&") {
            push_back_formulas(nnf, set_intersect(reg_alpha, reg_beta, n));
            return set_intersect(reg_alpha, reg_beta, n);
        }

        if (binary_conn == "v") {
            push_back_formulas(nnf, join(reg_alpha, reg_beta, n));
            return join(reg_alpha, reg_beta, n);
        }

        if (binary_conn == "=") {
            // (alpha = beta) is equiv to ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta)))
            // ((alpha & beta) v (Wff_to_Nnf_clean(~alpha) & Wff_to_Nnf_clean(~beta))) is in Nnf-form
            string equiv_nnf_formula = "((" + alpha + "&" + beta + ")v("
                + Wff_to_Nnf_clean("~" + alpha) + "&"
                + Wff_to_Nnf_clean("~" + beta) + "))";
            push_back_formulas(nnf, reg(equiv_nnf_formula, n));
            return reg(equiv_nnf_formula, n);
        }

        if (binary_conn == ">") {
            // (alpha > beta) is equiv to (Wff_to_Nnf(~alpha) v beta))
            // (Wff_to_Nnf(~alpha) v beta)) is in Nnf-form
            string equiv_nnf_formula = "(" + Wff_to_Nnf_clean("~" + alpha) + "v" + beta + ")";
            push_back_formulas(nnf, reg(equiv_nnf_formula, n));
            return reg(equiv_nnf_formula, n);
        }
    }

    // '(' Nnf Binary_Temp_conn Interval Nnf ')'
    if (Binary_Temp_conn_check(binary_conn)) {
        tuple<int, int, int> interval_tuple = primary_interval(nnf);
        int begin_interval = get<0>(interval_tuple);
        int comma_index = get<1>(interval_tuple);
        int end_interval = get<2>(interval_tuple);
        int a = stoi(Slice(nnf, begin_interval + 1, comma_index - 1));
        int b = stoi(Slice(nnf, comma_index + 1, end_interval - 1));
        string alpha = Slice(nnf, 1, binary_conn_index - 1);
        string beta = Slice(nnf, end_interval + 1, len_nnf - 2);

        if (binary_conn == "U") {
            push_back_formulas(nnf, reg_U(reg(alpha, n), reg(beta, n), a, b, n));
            return reg_U(reg(alpha, n), reg(beta, n), a, b, n);
        }
        else if (binary_conn == "R") {
            push_back_formulas(nnf, reg_R(reg(alpha, n), reg(beta, n), a, b, n));
            return reg_R(reg(alpha, n), reg(beta, n), a, b, n);
        }
    }

    else {
        string error_string = nnf + " is not in Negation-normal form.\n";
        throw invalid_argument(error_string);
    }
    return {};
}


