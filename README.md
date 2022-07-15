# WEST mLTL Truth Table Generator

The WEST mLTL Truth Table Generator is a program that parses well-formed mLTL formulas and outputs the structure of the computations of these formulas and their substrings. Our recursive algorithm is based on the regular expressions of mLTL operators and runs with approximately doubly exponential space and time complexity.

## Usage

After downloading and cloning this repository, use the commands
``` 
$ cd 2022-Iowa-State-REU-Temporal-Logic-
$ cd MLTL_reg
$ cd MLTL
$ make
```
Then, the user will be prompted:
```
Please enter a MLTL formula.
```
See the Grammar section below to inform input


## Grammar
Whitespace for formula input is unrestricted.
Non-empty intervals are recommended for meaningful truth table generation.

### Propositional Variables and Constants
One may input propositional constants or propositional variables. <br />

**True:** T <br />
**False:** ! <br />
**First Variable:** p0 <br />
**Second Variable:** p1 <br />
And so on, where each consecutive variable is followed with the appropriate natural number. <br />

**Negation:** ~ <br />
The negation operator should be followed by a propositional variable or well-formed mLTL formula. <br />
Formulas do not necessarily need to be in negation normal form, as our algorithm translates each well-formed formula into negation normal form, and generates the truth table for that translation. <br />

Note: after inputting a well-formed mLTL formula, the user will be prompted:
```
Please enter number of propositional variables.
```
Where the user will enter an appropriate integer. This number must be at least as large as the number of propositional variables defined in the formula. If desired, the user may input a formulas that does not necessarily include p0, p1, etc. In other words, they may be begin with p2, and computations will be generated to reflect that there are no restrictions on the first and second variables (p0 and p1). 

If exclusively propositional constants are inputted, then the user should enter "1" for the number of propositional variables in order to generate a meaningful truth table. 

### Unary Temporal Connectives
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive upper bound of an interval, and let b be inclusive lower bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.  <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />

**Finally:** F[a:b] K <br />
**Globally:** G[a:b] K <br />

### Binary Propositional Connectives
All binary connectives must be enclosed with parentheses. <br />
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (K & L) <br />
**Or:** (K v L)  <br />
**Equivalence:** (K = L)  <br />
**Implies:** (K > L)  <br />

### Associative Propositional Connectives
The entirely of the associative propositional connective formula string must be enclosed in parentheses.  <br />
The list of elements must be preceded by the associative propositional connective. <br />
Let "," separate each element in the list, and let "[" and "]" indicate the beginning and end of the list, respectively. <br />
Let K, L, ..., M be an arbitarily-sized list of well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (&[K, L, ..., M]) <br />
**Or:** (v[K, L, ..., M])  <br />
**Equivalence:** (=[K, L, ..., M])  <br />
**Implies:** (>[K, L, ..., M])  <br />

### Binary Temporal Connectives
All binary connectives must be enclosed with parentheses. <br />
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive upper bound of an interval, and let b be inclusive lower bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**Until:** (K U[a:b] L) <br />
**Release** (K R[a:b] L) <br />








## Contributors
This project is part of the 2022 Iowa State REU with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and Laura Gamboa Guzmán

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset




