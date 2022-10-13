#include<bits/stdc++.h>
using namespace std;
int main(){
    ifstream myfile ("error.html");
    auto my_string=ostringstream{};
    string ans;
    my_string << myfile.rdbuf();
    ans=my_string.str();
    cout<<ans<<endl;
}