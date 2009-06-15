#ifndef INGOKDTREEINTERNAL_HEADER
#define INGOKDTREEINTERNAL_HEADER

#include "triangle.h"
#include "IngoKdTree.h"

typedef enum {
	IGBOTH,
	IGLEFT_ONLY,
	IGRIGHT_ONLY
} igTriangleData;

igTriangleData* igPrepareTriangleData(Triangle*, unsigned);
void igTriangleDataFree(igTriangleData*);
igKdTree* igKdTreeRoot();
igKdTree* igCreateTree(Triangle *, igTriangleData *, AABB *, igEvents *);
igKdTree* igCreateLeaf(Triangle*, igEvents*);
void igClassifyTriangles(Triangle*, igTriangleData *, igEvents *, SAHSplitCandidate *);
void igSpliceEvents(Triangle*, igTriangleData*, igEvents**, igEvents**, igEvents**, AABB*);
void igGenerateNewEvents(Triangle*, igTriangleData*, igEvents**, igEvents**, igEvents**, AABB*, SAHSplitCandidate*);
igKdTree* igCreateInternalNode(SAHSplitCandidate*, igKdTree*, igKdTree*);

#endif
