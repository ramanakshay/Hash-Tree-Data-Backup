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

string backupPath = "/Users/akshay_raman/Documents/backup/";
string basePath = "/Users/akshay_raman/Documents/";
string dataPath = "/Users/akshay_raman/Documents/test/Fall 21-22/STS";


class query {
  string pathName;
  string operation;

 public:
  query(string operation, string pathName) {
	this->operation = operation;
	this->pathName = pathName;
  }

  bool execute() {
	string source = pathName;
	string destination = backupPath + source.substr(basePath.length(), source.length() - basePath.length());
	fs::path fileLoc = destination;
	fileLoc.remove_filename();

	try {
	  if (operation == "add") {
		fs::create_directories(fileLoc);
		fs::copy(source,destination, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	  } else if (operation == "delete") {
		  fs::remove_all(destination);
	  } else if (operation == "modify") {
		  fs::remove_all(destination);

		  fs::create_directories(fileLoc);
		  fs::copy(source,destination, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	  } else {
		cout << "Invalid query" << endl;
		return false;
	  }
	  return true;
	} catch (exception& e){
	  cout << "Unable to execute query";
	  return false;
	}
  }

  void print() {
	cout << operation << " " << pathName << endl;
  }
};

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
vector<query> queries;

void compareHelper(vector<string> lold, vector<string> lnew) {
	int i = 0, j = 0;
	while (i < lold.size() && j < lnew.size()) {
	  if (lold[i] < lnew[j]) {
		cout << "Deleted " << lold[i] << endl;
		queries.push_back(query("delete",lold[i]));
		i++;
	  } else if (lnew[j] < lold[i]) {
		cout << "Added " << lnew[j] << endl;
		queries.push_back(query("add",lnew[j]));
		j++;
	  } else {
		current.push(lold[i]);
		i++;
		j++;
	  }
	}

	while(i < lold.size()) {
	  cout << "Deleted " << lold[i] << endl;
	  queries.push_back(query("delete",lold[i]));
	  i++;
	}

	while(j < lnew.size()) {
	  cout << "Added " << lnew[j] << endl;
	  queries.push_back(query("add",lnew[j]));
	  j++;
	}
}

void compareTrees(HashTree *htold, HashTree *htnew) {
  	cout << "\nChanges: " << endl;
	current.push(htold -> get_root());
	while(!current.empty()) {
	  string node = current.front();
	  current.pop();
	  if (htold -> get_hash(node) != htnew -> get_hash(node)) {
		if (htnew -> get_children(node).empty()) {
		  cout << "Modified " << node << endl;
		  queries.push_back(query("modify",node));
		} else
			compareHelper(htold-> get_children(node), htnew -> get_children(node));
		}
	  }
}

int main() {
  HashTree *ht_data = new HashTree(dataPath); // current state
  ht_data -> print();
  ht_data -> print_metadata();
  cout << endl;

  if (!fs::exists(backupPath + "hashtree.txt")) {
	queries.push_back(query("add",dataPath));
  } else {
	HashTree *ht_backup = new HashTree(backupPath+"hashtree.txt", true);
	compareTrees(ht_backup,ht_data); // find changes
  }

  for (query q : queries) { //update backup
	q.print();
	q.execute();
  }

  ht_data -> save(backupPath + "hashtree.txt");
  return 0;
}
