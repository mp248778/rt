#include <stdio.h>
#include "printer.h"

void printSAHSplitCandidate(SAHSplitCandidate *sahc) {
	printf("sahc: axis=%i plane=%f terminate=%i cost=%f\n", sahc->axis, sahc->plane, sahc->terminate, sahc->cost); 
}
