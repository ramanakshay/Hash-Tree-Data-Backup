# Hash Tree Data Backup

## About

In this project, I have implemented an efficient backup system using hash trees. Because directories in all operating systems are implemented in a tree-like structure, it is possible to store the state of a file system using a hash tree. By using a hash tree, we can easily find which files have been modified by just checking the hashes of the tree in a top-down manner.

The project has three important parts:
1. Construction of Hash Tree
2. Comparing Hash Trees (to find modified files)
3. Backup the changes

## Construction of Hash (Merkle) Tree

A Merkle tree is a generalisation of the hash list and is a hash-based data structure. It is a tree structure in which each leaf node is a hash of a block of data, and each non-leaf node is a hash of its children. Generally, Merkle trees have a branching factor of 2, where each node has a maximum of 2 children. However, an n-ary hash tree is required here for the file system.

*Add merkle tree image

The hash-tree folder consists of the implementation of the hash tree class which build a hash tree. It can be built using two ways:
1. From file path - Build a hash tree of data with the root as the input file path
2. From text file - Build the hash tree data structure from an existing hash tree (stored in a text file as an adjacency list)

The hash tree is built from the file path using a DFS-like approach. I made use of OpenMP tasks to parallelise the building process so that different threads can handle different branches (and nodes) of the tree simultaneously. The SHA256 implementation from the C++ hash-library is used to compute the hash of data.

To build a hash tree from a data/file path. Run the following code:
```
HashTree *ht_data = new HashTree(dataPath, false);
ht_data->print_metadata();
```

The hash tree can be stored/saved as a text file in the form of an adjancency list. Below is the format of the text file:
```
src1|src1_hash|dest1|dest1_hash|dest2|dest2_hash| ... |destn|destn_hash 
src2|src2_hash|dest1|dest1_hash|dest2|dest2_hash| ... |destn|destn_hash 
. 
. 
. 
srcm|srcm_hash|dest1|dest1_hash|dest2|dest2_hash| ... |destn|destn_hash 
```

To build the hash tree from the text file. Run the following code:
```
HashTree *ht_backup = new HashTree(backupPath+"hashtree.txt", true);
ht_backup->print_metadata();
```

## Comparing Hash Trees



