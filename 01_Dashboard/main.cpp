#include<iostream>
#include<string>
#include<stack>
using namespace std;
  

bool isValidPair(char a, char b) {
    return (a == '(' && b == ')') || (a == '[' && b == ']');
}

int solve(const std::string& s) {
    std::stack<char> st;
    for (char c : s) {
        if (!st.empty() && isValidPair(st.top(), c)) {
            st.pop();
        }
        else {
            st.push(c);
        }
    }
    return static_cast<int>(st.size());
}
int main() {
	string s;
	getline(cin, s);
	solve(s);
	return 0;
}