#include <iostream>
#include <vector>
#include <filesystem>
#include <queue>
#include <omp.h>
#include "sha256.h"
#include "hashtree.h"

#define NUM_THREADS 8

namespace fs = std::__fs::filesystem;
using namespace std;
using namespace fs;

const string backupPath = "/Users/akshay_raman/Documents/backup/";
const string basePath = "/Users/akshay_raman/Documents/";
const string dataPath = "/Users/akshay_raman/Documents/test/Fall 21-22/STS";

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

queue<string> current; // stores current nodes in BFS
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
  cout << "(Parallel) Data Backup Using Hash Trees" << endl;

  cout << endl;

  cout << "Current Data Hash Tree:" << endl;

  HashTree *ht_data;

  omp_set_num_threads(NUM_THREADS);

  double start = omp_get_wtime();

#pragma omp parallel default(none) firstprivate(dataPath) shared(ht_data)
  {
#pragma omp single
	{
	  ht_data = new HashTree(dataPath, false); // current state
	}
  }

  double end = omp_get_wtime();

//  ht_data.print();
  ht_data->print_metadata();

  cout << endl;

  cout << "Time Taken: " << end - start << "s" << endl;

  cout << endl;

  if (!fs::exists(backupPath + "hashtree.txt")) {
	cout << "No backup found. Creating backup..." << endl;
	queries.push_back(query("add",dataPath));
  } else {
	cout << "Backup Hash Tree:" << endl;
	HashTree *ht_backup = new HashTree(backupPath+"hashtree.txt", true);
//	ht_data.print();
	ht_data->print_metadata();
	compareTrees(ht_backup,ht_data); // find changes
  }

  cout << endl;

  cout << "Executed Queries:" << endl;
#pragma omp parallel default(none) shared(queries)
#pragma omp for schedule(dynamic,1)
  for (query q : queries) { //update backup
	q.print();
	q.execute();
  }

  cout << endl;

  ht_data->save(backupPath + "hashtree.txt");

  return 0;
}
