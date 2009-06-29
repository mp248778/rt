#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "writer.h"

typedef struct BFSNode {
	igKdTree *left;
	igKdTree *right;
	struct BFSNode *next;
} BFSNode;

void push_back(BFSNode **, igKdTree *);
void igWriteNode(igKdTree *, uint32_t, uint32_t *, uint32_t , FILE *);
void igWriteNodes(igKdTree *, uint32_t, FILE *);
void igWriteTriangles(igKdTree *, FILE *);
void igWriteNodeTriangles(igKdTree *, FILE *);
uint32_t countNodes(igKdTree*);

void igWriteTree(igKdTree *tree, const char *filename) {
	FILE *file = fopen(filename, "w");
	uint32_t nodescnt = countNodes(tree);
	igWriteNodes(tree, nodescnt, file);
	igWriteTriangles(tree, file);
}

void igWriteNodes(igKdTree *tree, uint32_t nodescnt, FILE *file) {
	uint32_t tcnt = 0;
	uint32_t queueSize = 1;
	BFSNode *front = malloc(sizeof(BFSNode));
	BFSNode *back = front;
	front->next = NULL;
	front->left = tree->left;
	front->right = tree->right;
	igWriteNode(tree, nodescnt, &tcnt, 1, file);
	while(queueSize) {
		queueSize--;
		igWriteNode(front->left, nodescnt, &tcnt, 2 * queueSize + 2, file);
		igWriteNode(front->right, nodescnt, &tcnt, 2 * queueSize + 3, file);
		if(front->left->axis != 3) {
			push_back(&back, front->left);
			queueSize++;
		}
		if(front->right->axis != 3) {
			push_back(&back, front->right);
			queueSize++;
		}
		BFSNode *tmp = front;
		front = front->next;
		free(tmp);
	}
}

void igWriteTriangles(igKdTree *tree, FILE *file) {
	BFSNode *front = malloc(sizeof(BFSNode));
	BFSNode *back = front;
	front->next = NULL;
	front->left = tree->left;
	front->right = tree->right;
	if(tree->axis == 3) {
		igWriteNodeTriangles(tree, file);
		free(front);
	} else {
		while(front) {
			if(front->left->axis == 3)
				igWriteNodeTriangles(front->left, file);
			else 
				push_back(&back, front->left);

			if(front->right->axis == 3)
				igWriteNodeTriangles(front->right, file);
			else 
				push_back(&back, front->right);
			BFSNode *tmp = front;
			front = front->next;
			free(tmp);
		}
	}
}

void igWriteNodeTriangles(igKdTree *node, FILE *file) {
	fwrite(node->trids, sizeof(uint32_t), node->tcnt, file);
}

void push_back(BFSNode **back, igKdTree *node) {
	(*back) = (*back)->next = malloc(sizeof(BFSNode));
	(*back)->left = node->left;
	(*back)->right = node->right;
	(*back)->next = NULL;
}

void igWriteNode(igKdTree *node, uint32_t nodescnt, uint32_t *tcnt, uint32_t childrenOffset, FILE *file) {
	if(node->axis < 3) {
		uint32_t filepos = ftell(file);
		uint32_t childrenAddr = filepos + childrenOffset * 8 + node->axis;
		fwrite(&node->plane, sizeof(float), 1, file);
		fwrite(&childrenAddr, sizeof(uint32_t), 1, file);
	} else {
		fwrite(&node->tcnt, sizeof(uint32_t), 1, file);
		uint32_t trisaddr = nodescnt * 8 + *tcnt * 4;
		fwrite(&trisaddr, sizeof(uint32_t), 1, file);
		*tcnt += node->tcnt;
	}
}

uint32_t countNodes(igKdTree* tree) {
	if(tree->axis == 3) return 1;
	return 1 + countNodes(tree->left) + countNodes(tree->right);
}


