/*references: https://gist.github.com/VictorGarritano/5f894be162d39e9bdd5c*/
/*& https://github.com/chenlei-yang/Red-Black-Tree/blob/master/rbTree.cpp*/
/*a rbt inplemantation in c*/
#ifndef RBT_H
#define RBT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

void left_chileRotate(Treenode**, Treenode*);
void right_childRotate(Treenode **, Treenode*);
void insertFixUp(Treenode**, Treenode*);
int insert_unique(Treenode**, long long, char*);
void insert(Treenode**, long long, char*);
Treenode* find(Treenode*, long long);

#endif