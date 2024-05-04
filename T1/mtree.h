#ifndef MTREE_H
#define MTREE_H

// ==================================
// Funciones y estructuras del M-Tree
// ==================================

typedef struct node Node;
typedef struct point Point;
typedef struct entry Entry;
typedef struct query Query;
typedef struct mtree Mtree;
Node* create_node();
Point *search_points_in_radio(Node *node, Query query, int *disk_acceses);

#endif