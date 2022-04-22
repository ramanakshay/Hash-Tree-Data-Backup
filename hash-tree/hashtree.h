//
// Created by Akshay Raman on 22/04/22.
//

#ifndef HASH_TREE_DATA_BACKUP_HASH_TREE_HASHTREE_H_
#define HASH_TREE_DATA_BACKUP_HASH_TREE_HASHTREE_H_

#endif //HASH_TREE_DATA_BACKUP_HASH_TREE_HASHTREE_H_

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

 private:
  string root;

  map<string,vector<string>> tree;

  void build(const string&);

  void build_fromFile(const string&);

 public:

  explicit HashTree(const string&, bool);

  string get_root();

  string get_hash(const string&);

  vector<string> get_children(const string&);

  void save(const string&);

  void print();

  void print_metadata();

};

