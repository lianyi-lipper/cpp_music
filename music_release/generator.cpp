#include <bits/stdc++.h>
using namespace std;
#include <regex>
bool isNumeric(string const &str){
    return regex_match(str,regex("[(-|+)|][0-9]+"));
}
signed main(){
	string res,src,cur;
	printf("File:");cin>>cur;
	src=cur+".txt";res=cur+".cpp";
	string s;
	ifstream music_h("music_simple.h");
	ofstream result(res);
	getline(music_h,s);
	while (getline(music_h,s)){
		result<<s<<endl;
	}
	result<<"signed main(){"<<endl;
	
	ifstream music(src);int dctn;
	music>>dctn;
	result<<"	MusicPlayer mp;mp.setDelay("<<dctn<<");"<<endl;
	while (getline(music,s)){
		string s1="",s2="";
		while (s=="" && getline(music,s));
		if (isNumeric(s)){
			result<<"	mp.setDelay("<<stoi(s)<<");"<<endl;
			continue;
		}
		s1=s;
		if (getline(music,s) && s!="") s2=s;
		result<<"	mp.play(\""<<s1<<"\",\""<<s2<<"\");"<<endl;
	}
	result<<"	return 0;"<<endl;
	result<<"}"<<endl;
}