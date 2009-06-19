#include <stdlib.h>
#include "event.h"
#include "utils.h"

igEvent* igInitEvent(igEvent *next, unsigned axis, float plane, Type t, unsigned trid) {
	igEvent* e = malloc(sizeof(igEvent));
	e->next = next;
	e->axis = axis;
	e->plane = plane;
	e->type = t;
	e->trid = trid;
	return e;
}

void igCreateEvent(AABB *aabb, igEvents *es, unsigned trid) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		float start = (*aabb)[axis];
		float end = (*aabb)[axis + 3];
		if(start == end) {
			(*es)[axis]->next = igInitEvent((*es)[axis]->next, axis, start, IGPLANAR, trid);
		} else {
			igEvent *e1 = igInitEvent((*es)[axis]->next, axis, start, IGBEGIN, trid);
			igEvent *e2 = igInitEvent(e1, axis, end, IGEND, trid);
			(*es)[axis]->next = e2;
		}
	}
}

igEvents* igCreateEvents(Triangle *t, unsigned tcnt) {
	unsigned axis;
	igEvents *es = malloc(sizeof(igEvents));
	for(axis = 0; axis < 3; axis++) {
		(*es)[axis] = malloc(sizeof(igEvent));
		(*es)[axis]->next = NULL;
	}
	unsigned i;
	for(i = 0; i < tcnt; i++) {
		AABB aabb;
		for(axis = 0; axis < 3; axis++) {
			float start = minv(3, t[i][axis], t[i][axis+3], t[i][axis+6]);
			float end = maxv(3, t[i][axis], t[i][axis+3], t[i][axis+6]);
			aabb[axis] = start;
			aabb[axis + 3] = end;
		}
		igCreateEvent(&aabb, es, i);
	}
	for(axis = 0; axis < 3; axis++) {
		igEvent *tmp;
		tmp = (*es)[axis];
		(*es)[axis] = (*es)[axis]->next;
		free(tmp);
	}
	igSortEvents(es);
	return es;
}

void igSortEvents(igEvents *es) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		const unsigned ecnt = igCountEvents((*es)[axis]);
		if(!ecnt) continue;
		igEvent **orderBy = malloc(ecnt * sizeof(igEvent*));
		igEvent *eit = (*es)[axis];
		unsigned i;
		for(i = 0; eit; i++, eit = eit->next)
				orderBy[i] = eit;
		qsort(orderBy, ecnt, sizeof(igEvent*), igEventCmp);
		for(i = 0; i < ecnt - 1; i++)
			orderBy[i]->next = orderBy[i + 1];
		orderBy[ecnt - 1]->next = NULL;
		(*es)[axis] = orderBy[0];
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

unsigned igCountTriangles(igEvents *es) {
	unsigned cnt;
	igEvent *eit;
	for(cnt = 0, eit = (*es)[0]; eit; eit = eit->next)
			if(eit->type == IGBEGIN || eit->type == IGPLANAR) cnt++;
	return cnt;
}

unsigned igCountEvents(igEvent *e) {
	unsigned cnt;
	for(cnt = 0; e; e = e->next, cnt++);
	return cnt;
}

int igEventCmp(const void *ve1, const void *ve2) {
	const igEvent *e1 = *((const igEvent**)ve1);
	const igEvent *e2 = *((const igEvent**)ve2);
	if(e1->plane < e2->plane) return -1;
	if(e1->plane > e2->plane) return 1;
	if(e1->type == IGEND && e1->type != e2->type) return 1;
	if(e1->type == IGPLANAR && e1->type != e2->type) return 1;
	return 0;
}

void igEventsFree(igEvents *es) {
	unsigned axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis];
		while(eit) {
			igEvent *prev = eit;
			eit = eit->next;
			free(prev);
		}
	}
//	printf("%p\n", es);
	free(es);
}

igEvents* igInitEvents() {
	igEvents *es = malloc(sizeof(igEvents));
	(*es)[0] = (*es)[1] = (*es)[2] = NULL;
	return es;
}


