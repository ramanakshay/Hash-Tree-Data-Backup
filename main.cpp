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

  void build(const string&);

  void build_fromFile(const string& path) {
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

  explicit HashTree(const string& pathName, bool fromFile = false){

	if (fromFile) {
	  build_fromFile(pathName);
	} else {
	  this->root = pathName;
	  build(pathName);
	}
  }

  string get_root() {
	return this->root;
  }

  string get_hash(const string& node) {
	return this->tree[node][0];
  }

  vector<string> get_children(const string& node) {
	return {tree[node].begin() + 1, tree[node].end()};
  }

  void save(const string& destination) {
	ofstream file;
	file.open(destination);
	queue<string> current;
	current.push(this->root);
	while(!current.empty()) {
	  string node = current.front();
	  string s;
	  current.pop();

	  s = node + "|" + tree[node][0] + "|";
	  for(int i = 1; i < tree[node].size();i++) {
		s += tree[node][i] + "|";
		current.push(tree[node][i]);
	  }
	  s += "\n";
	  file << s; // writing a line
	}
	cout << "Hash Tree saved at " << destination << endl;
	file.close();
  }

  void print() {
	queue<string> current;
	current.push(this->root);
	while(!current.empty()) {
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

void HashTree::build(const string& path) {
  SHA256 hash;
  vector<string> info;
  if(is_directory(path)) {
	for (const auto &entry : directory_iterator(path)) {
	  info.push_back(entry.path()); // get all children
	}
	sort(info.begin(), info.end());
	for (const string& child : info) {
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
	  std::size_t size = pbuf->pubseekoff (0,stream.end,std::ifstream::in);
	  pbuf->pubseekpos (0,std::ifstream::in);

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

queue<string> current;

void compare(vector<string> la, vector<string> lb) {
	int i = 0, j = 0;
	while (i < la.size() && j < lb.size()) {
	  if (la[i] < lb[j]) {
		cout << "Deleted " << la[i] << endl;
		i++;
	  } else if (lb[j] < la[i]) {
		cout << "Added " << lb[j] << endl;
		j++;
	  } else {
		current.push(la[i]);
		i++;
		j++;
	  }
	}

	while(i < la.size()) {
	  cout << "Deleted " << la[i] << endl;
	  i++;
	}

	while(j < lb.size()) {
	  cout << "Added " << lb[j] << endl;
	  j++;
	}
}

void BFS(HashTree *hta, HashTree *htb) {
  	cout << "\nChanges: " << endl;
	if (hta -> get_root() != htb -> get_root()) {
	  cout << "Added " << htb -> get_root();
	  cout << "Deleted " << hta -> get_root();
	} else {
	  current.push(hta -> get_root());
	  while(!current.empty()) {
		string node = current.front();
		current.pop();
		if (hta -> get_hash(node) != htb -> get_hash(node)) {
		  if (htb -> get_children(node).empty())
			cout << "Modified " << node << endl;
		  else
			compare(hta-> get_children(node), htb -> get_children(node));
		}
	  }
	}
}

int main() {
  HashTree *ht1 = new HashTree("/Users/akshay_raman/Documents/test/Fall 21-22/STS");
//  ht1 -> print();
  ht1 -> print_metadata();
//  ht1 -> save("/Users/akshay_raman/Documents/test/example.txt");

  cout << endl;

  HashTree *ht2 = new HashTree("/Users/akshay_raman/Documents/test/example.txt",true);
//  ht2 -> print();
  ht2 -> print_metadata();

  BFS(ht2,ht1);
  return 0;
}
