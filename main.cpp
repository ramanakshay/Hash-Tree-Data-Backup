#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <queue>

namespace fs = std::__fs::filesystem;
using namespace std;
using namespace fs;

class HashTree {
  string root;
  map<string,vector<string>> tree;

  void build(string path) {
	vector<string> children;
	if(is_directory(path)) {
	  for (const auto & entry : directory_iterator(path)) {
		children.push_back(entry.path());
	  }
	  sort(children.begin(),children.end());
	  for (string child: children) {
		build(child);
	  }
	}
	this->tree[path] = children;
  }

 public:
  HashTree(string pathName){
	this->root = pathName;
	build(pathName);
  }

  void print() {
	queue<string> current;
	current.push(this->root);
	while(current.size()) {
	  string node = current.front();
	  current.pop();

	  cout << node << " Children: [";
	  for(string s: tree[node]) {
		cout << s << ", ";
		current.push(s);
	  }
	  cout << "] " << tree[node].size() << endl;
	}
  }
};
int main() {
  HashTree* ht = new HashTree("/Users/akshay_raman/Documents/test/Fall 21-22/");
  ht -> print();
  return 0;
}
