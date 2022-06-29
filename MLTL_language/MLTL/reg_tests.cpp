#include <stdio.h>
#include "utils.h"
#include "unit_test_framework.h"
#include "grammar.h"
#include <iostream>
#include "reg.h"

using namespace std;

TEST(assoc_and_0) {
    int n = 2;
    string s = "(&[p0])";
    vector<string> v_reg = reg(s, n);
    vector<string> v_actual = { "1s" };
    ASSERT_EQUAL(v_reg, v_actual)
}

TEST(assoc_and_1) {
    int n = 2;
    string s = "(&[p0,p1])";
    vector<string> v_reg = reg(s, n);
    vector<string> v_actual = { "11" };
    ASSERT_EQUAL(v_reg, v_actual)
}

TEST(test_finally_basic) {
    string s = "F[2,4]p1";
    vector<string> v_expected = {"ss,ss,s1", "ss,ss,ss,s1", "ss,ss,ss,ss,s1"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_a_equals_b) {
    string s = "F[2,2]p0";
    vector<string> v_expected = {"s,s,1"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_finally_negation) {
    string s = "F[2,2]~p0";
    vector<string> v_expected = {"s,s,0"};
    vector<string> v_actual = reg(s, 1);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_two_props) {
    string s = "F[2,2]p0";
    vector<string> v_expected = {"ss,ss,1s"};
    vector<string> v_actual = reg(s, 2);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_zero_props) {
    string s = "F[0,2]T";
    vector<string> v_expected = {};
    vector<string> v_actual = reg(s, 0);
    
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_1) {
    string s = "F[0,2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    
    vector<string> v_expected = {"ss,ss,11", "ss,ss,ss,11", "ss,ss,ss,ss,11"};
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_and_2) {
    string s = "F[0,2](&[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"sss,sss,11s", "sss,sss,sss,11s", "sss,sss,sss,sss,11s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_1) {
    string s = "F[2,4](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"ss,ss,s1", "ss,ss,ss,s1", "ss,ss,ss,ss,s1",
                                 "ss,ss,1s", "ss,ss,ss,1s", "ss,ss,ss,ss,1s"};
    
    vector<string> v_actual = reg(s, 2);
    //print(reg(s, 2));
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_finally_with_or_2) {
    string s = "F[0,2](v[p0,p1])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"sss,sss,11s", "sss,sss,sss,11s", "sss,sss,sss,sss,11s"};
    vector<string> v_actual = reg(s, 3);
    //print(reg(s, 3));
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_global_basic_1) {
    string s = "G[0,1]p0";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"1,1"};
    vector<string> v_actual = reg(s, 1);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_basic_2) {
    string s = "G[0,1]p0";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"1s,1s"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_with_or) {
    string s = "G[0,1](v[p1,p2])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"s1s,ss1",  "ss1,ss1", "s1s,s1s", "ss1, s1s"};
    vector<string> v_actual = reg(s, 3);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_global_with_and) {
    string s = "G[0,1](&[p1,p2])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"s11,s11"};
    vector<string> v_actual = reg(s, 3);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_basic) {
    string s = "(p0U[0,1]p1)";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_expected = {"s1", "1s,s1"};
    vector<string> v_actual = reg(s, 2);
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_many_props) {
    string s = "(p0U[0,1]p1)";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = reg(s, 5);
    vector<string> v_expected = {"s1sss", "1ssss,s1sss"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_swapped_props) {
    string s = "(p1U[0,1]p0)";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = reg(s, 2);
    vector<string> v_expected = {"1s", "s1,1s"};
    ASSERT_EQUAL(v_expected, v_actual);
}

//TEST(test_until_nested_diff_complen) {
//    string s = "(p0U[0,1]p1)U[0,2])p2)";
//    ASSERT_TRUE(Wff_check(s));
//    vector<string> v_actual = reg(s, 3);
//    vector<string> v_expected = {"1s", "s1,1s"};
//ASSERT_EQUAL(v_expected, v_actual);
//}


//BUG IN UNTIL
TEST(test_p_until_p_until_q) {
    string s = "(p0U[0,1](p0U[0,1]p1))";
    //should return comps for (p0U[0,2]p1)
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"s1,ss,ss", "1s,s1,ss", "1s,1s,s1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

//BUG IN UNTIL
TEST(test_and_with_until_diff_complen) {
    string s = "(&[(p0U[0,1]p1),(p0U[0,2]p1)])";
    //should return comps for (p0U[0,1]p1)
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = reg(s, 2);
    vector<string> v_expected = {"s1", "1s,s1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_until_nested_same_complen) {
    string s1 = "(p0U[0,3](p0U[0,3]p1))";
    string s2 = "(p0U[0,6]p1)";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    vector<string> v1 = simplify(reg(s1, 2), 2);
    vector<string> v2 = simplify(reg(s2, 2), 2);
    ASSERT_EQUAL(v1, v2);
}

TEST(test_until_nested_a_not_zero) {
    string s1 = "(p0U[1,3](p0U[0,3]p1))";
    string s2 = "(p0U[1,6]p1)";
    ASSERT_TRUE(Wff_check(s1));
    ASSERT_TRUE(Wff_check(s2));
    vector<string> v1 = simplify(reg(s1, 2), 2);
    vector<string> v2 = simplify(reg(s2, 2), 2);
    ASSERT_EQUAL(v1, v2);
}

TEST(test_implies_basic) {
    string s = "(p0>p1)";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"0s", "11"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_1) {
    string s = "G[0,1](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0,1", "1,0"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_2) {
    string s = "G[0,2](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0,1,0", "1,0,1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_3) {
    string s = "G[0,0](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 1), 1);
    vector<string> v_expected = {"0", "1"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_4) {
    string s = "G[0,0](&[(p0>G[1,1]~p0),(~p0>G[1,1]p0)])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"0s", "1s"};
    ASSERT_EQUAL(v_expected, v_actual);
}

TEST(test_oscillation_5) {
    string s = "G[0,1](&[(p1>G[1,1]~p1),(~p1>G[1,1]p1)])";
    ASSERT_TRUE(Wff_check(s));
    vector<string> v_actual = simplify(reg(s, 2), 2);
    vector<string> v_expected = {"s0,s1", "s1,s0"};
    ASSERT_EQUAL(v_expected, v_actual);
}


TEST(test_equivalent) {
    
}

TEST(test_finally_and_release) {
    
}

TEST(test_finally_and_until) {
    
}

TEST(test_global_and_release) {
    
}

TEST(test_global_and_until) {
    
}

TEST_MAIN()


