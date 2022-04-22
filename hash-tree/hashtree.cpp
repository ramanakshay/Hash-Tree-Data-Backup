//
// Created by Akshay Raman on 22/04/22.
//

#include "hashtree.h"

HashTree::HashTree(const string& pathName, bool fromFile = false){

  if (fromFile) {
	build_fromFile(pathName);
  } else {
	this->root = pathName;
	build(pathName);
  }
}

void HashTree::build_fromFile(const string& path) {
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

string HashTree::get_root() {
  return this->root;
}

string HashTree::get_hash(const string& node) {
  return this->tree[node][0];
}

vector<string> HashTree::get_children(const string& node) {
  return {tree[node].begin() + 1, tree[node].end()};
}

void HashTree::save(const string& destination) {
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

void HashTree::print() {
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

void HashTree::print_metadata() {
  cout << "Path: " << this->root << endl;
  cout << "Root Hash: " << this->tree[root][0] << endl;
  cout << "Number of Files: " << this->tree.size() << endl;
}

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

