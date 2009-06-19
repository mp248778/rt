#ifndef SAHSPLITCANDIDATE_HEADER
#define SAHSPLITCANDIDATE_HEADER

#include "triangle.h"
#include "aabb.h"
#include "event.h"

typedef struct {
	unsigned axis;
	float plane;
	unsigned terminate;
	float cost;
} SAHSplitCandidate;

void SAHSplitCandidateFree(SAHSplitCandidate*);
SAHSplitCandidate* igFindSAHSplitCandidate(Triangle*, AABB*, igEvents*);
SAHSplitCandidate* initialSAHSplitCandidate();
SAHSplitCandidate* newSAHSplitCandidate(unsigned, float, AABB*, unsigned, unsigned, unsigned);
SAHSplitCandidate* SAHChooseBetter(SAHSplitCandidate*, SAHSplitCandidate*);
void perfectsplit(AABB*, Triangle*, AABB[2], SAHSplitCandidate*);

#endif
