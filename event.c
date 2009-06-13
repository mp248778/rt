#include <stdlib.h>
#include "event.h"



void igSortEvents(igEvents *es) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		const unsigned ecnt = igCountEvents(*es[axis]);
		if(!ecnt) continue;
		igEvent **orderBy = malloc(ecnt * sizeof(igEvent*));
		igEvent *eit = *es[axis];
		unsigned i;
		for(i = 0; eit; i++, eit = eit->next)
				orderBy[i] = eit;
		qsort(orderBy, ecnt, sizeof(igEvent*), igEventCmp);
		for(i = 0; i < ecnt - 1; i++)
			orderBy[i]->next = orderBy[i + 1];
		orderBy[ecnt - 1]->next = NULL;
		*es[axis] = orderBy[0];
		free(orderBy);
	}
}

igEvents* igMergeEvents(igEvents *e1, igEvents *e2) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit1 = *e1[axis], *eit2 = *e2[axis];
		igEvent *curr, *first = malloc(sizeof(igEvent));
		curr = first;
		curr->next = NULL;
		while(eit1 && eit2) {
			while(igEventCmp(&eit1, &eit2) < 0) {
				curr = curr->next = eit1; eit1 = eit1->next;
			}
			curr = curr->next = eit2;
			eit2 = eit2->next;
		}
		if(eit1)
			curr->next = eit1;
		else if(eit2)
			curr->next = eit2;

		*e1[axis] = first->next;
		*e2[axis] = NULL;
		free(first);
	}
	igEventsFree(e2);
	return e1;
}


