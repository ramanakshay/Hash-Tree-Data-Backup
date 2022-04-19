#include <iostream>
#include <map>
#include <vector>
#include <filesystem>
#include <queue>
#include <fstream>
#include <sstream>
#include "sha256.h"

namespace fs = std::__fs::filesystem;
using namespace std;
using namespace fs;

class HashTree {
  string root;
  map<string,vector<string>> tree;

  void build(string path) {
	SHA256 hash;
	vector<string> info;
	if(is_directory(path)) {
	  for (const auto &entry : directory_iterator(path)) {
		info.push_back(entry.path()); // get all children
	  }
	  sort(info.begin(), info.end());
	  for (string child : info) {
		build(child); // recursive call to child
		string child_hash = this->tree[child][0]; // get hash of child
		hash.add(&child_hash, child_hash.length());
	  }
	} else {
	  ifstream stream;
	  stream.open(path, ifstream::binary);
	  if(!stream) {
		perror("Unable to Open File");
	  } else {
		// get pointer to associated buffer object
		std::filebuf* pbuf = stream.rdbuf();

		// get file size using buffer's members
		std::size_t size = pbuf->pubseekoff (0,stream.end,stream.in);
		pbuf->pubseekpos (0,stream.in);

		// allocate memory to contain file data
		char* buffer=new char[size];

		// get file data
		pbuf->sgetn (buffer,size);
		hash.add(buffer, size);
	  }
	}
	info.insert(info.begin(), hash.getHash());
	this->tree[path] = info;
  }

  void build_fromFile(string path) {
	bool first = true;
	ifstream file(path);
	if(!file) {
	  perror("Unable to Open File");
	}
	string line;
	while(getline(file,line)) {
	  string node;
	  vector<string> info;
	  stringstream tokenizer(line);
	  getline(tokenizer, node, '|');
	  string temp;
	  if (first) {
		this->root = node;
		first = false;
	  }
	  while(getline(tokenizer, temp, '|')) {
		info.push_back(temp);
	  }
	  this->tree[node] = info;
	}
  }

 public:
  void save(string destination) {
	ofstream file;
	file.open(destination);
	queue<string> current;
	current.push(this->root);
	while(current.size()) {
	  string node = current.front();
	  string s = "";
	  current.pop();

	  s = s + node + "|" + tree[node][0] + "|";
	  for(int i = 1; i < tree[node].size();i++) {
		s = s + tree[node][i] + "|";
		current.push(tree[node][i]);
	  }
	  s += "\n";
	  file << s; // writing a line
	}
	cout << "Hash Tree saved at " << destination << endl;
	file.close();
  }

  HashTree(string pathName, bool fromFile = false){

	if (fromFile) {
	  build_fromFile(pathName);
	} else {
	  this->root = pathName;
	  build(pathName);
	}
  }

  void print() {
	queue<string> current;
	current.push(this->root);
	while(current.size()) {
	  string node = current.front();
	  current.pop();

	  cout << node << " " << "Hash: " << tree[node][0] << " Children: [";
	  for(int i = 1; i < tree[node].size();i++) {
		cout << tree[node][i] << ", ";
		current.push(tree[node][i]);
	  }
	  cout << "] " << tree[node].size() - 1 << endl;
	}
  }

  void print_metadata() {
	cout << "Path: " << this->root << endl;
	cout << "Root Hash: " << this->tree[root][0] << endl;
	cout << "Number of Files: " << this->tree.size() << endl;
  }
};


int main() {
  HashTree* ht = new HashTree("/Users/akshay_raman/Documents/test/Fall 21-22/STS");
//  ht -> print();
  ht -> print_metadata();
  ht -> save("/Users/akshay_raman/Documents/test/example4.txt");

  cout << endl;

  HashTree* ht2 = new HashTree("/Users/akshay_raman/Documents/test/example4.txt",true);
  ht2 -> print();
  ht2 -> print_metadata();
  return 0;
}
