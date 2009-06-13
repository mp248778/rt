#ifndef SAHSPLITCANDIDATE_HEADER
#define SAHSPLITCANDIDATE_HEADER

typedef struct {
	unsigned axis;
	float plane;
	int terminate;
} SAHSplitCandidate;

void SAHSplitCandidateFree(SAHSplitCandidate*);
SAHSplitCandidate* igFindSAHSplitCandidate(Triangle*, AABB*, igEvents*);
SAHSplitCandidate* initialSAHSplitCandidate();
SAHSplitCandidate* newSAHSplitCandidate(unsigned, float, unsigned, unsigned);
SAHSplitCandidate* SAHChooseBetter(SAHSplitCandidate*, SAHSplitCandidate*);


#endif
