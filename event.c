#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "event.h"
#include "utils.h"

igEvent* igInitDummyEvent();
igEvent* igInitEvent(uint32_t, float, Type t, uint32_t);


igEvent* igInitEvent(uint32_t axis, float plane, Type t, uint32_t trid) {
	igEvent* e = malloc(sizeof(igEvent));
	e->next = (igEvent*)0xDEADBEEF;
	e->axis = axis;
	e->plane = plane;
	e->type = t;
	e->trid = trid;
	return e;
}

void igCreateEvent(AABB *aabb, igEvents *es, uint32_t trid) {
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		float start = (*aabb)[axis];
		float end = (*aabb)[axis + 3];
		if(start == end) {
			igEvent *tmp = igInitEvent(axis, start, IGPLANAR, trid);
			igInsertEvent(es, tmp, axis);
		} else {
			igEvent *e1 = igInitEvent(axis, start, IGBEGIN, trid);
			igEvent *e2 = igInitEvent(axis, end, IGEND, trid);
			igInsertEvent(es, e1, axis);
			igInsertEvent(es, e2, axis);
		}
	}
}

igEvents* igCreateEvents(Triangle *t, uint32_t tcnt) {
	uint32_t axis;
	igEvents *es = igInitEvents();
	uint32_t i;
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
	igSortEvents(es);
	return es;
}

void igSortEvents(igEvents *es) {
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		const uint32_t ecnt = igCountEvents((*es)[axis]);
		if(!ecnt) continue;
		igEvent **orderBy = malloc(ecnt * sizeof(igEvent*));
		igEvent *eit = (*es)[axis]->next;
		uint32_t i;
		for(i = 0; eit; i++, eit = eit->next)
				orderBy[i] = eit;

		qsort(orderBy, ecnt, sizeof(igEvent*), igEventCmp);

		for(i = 0; i < ecnt - 1; i++)
			orderBy[i]->next = orderBy[i + 1];
		orderBy[ecnt - 1]->next = NULL;
		(*es)[axis]->next = orderBy[0];
		free(orderBy);
	}
}

igEvents* igMergeEvents(igEvents *e1, igEvents *e2) {
	uint32_t axis;
	igEvents *es = igInitEvents();
	igEvents curr;
	memcpy(&curr, *es, sizeof(igEvents));
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit1 = ((*e1)[axis])->next, *eit2 = ((*e2)[axis])->next;
		while(eit1 && eit2) {
			if(igEventCmp(&eit1, &eit2) <= 0) {
				igEvent *tmp = eit1->next;
				curr[axis] = curr[axis]->next = eit1;
				eit1 = tmp;
			} else {
				igEvent *tmp = eit2->next;
				curr[axis] = curr[axis]->next = eit2;
				eit2 = tmp;
			}
		}
		curr[axis]->next = eit1 ? (eit1) : (eit2);

		((*e1)[axis])->next = ((*e2)[axis])->next = NULL;
	}
	igEventsFree(e1);
	igEventsFree(e2);
	return es;
}

uint32_t igCountTriangles(igEvents *es) {
	uint32_t cnt;
	igEvent *eit;
	for(cnt = 0, eit = (*es)[0]->next; eit; eit = eit->next)
			if(eit->type == IGBEGIN || eit->type == IGPLANAR) cnt++;
	return cnt;
}

uint32_t igCountEvents(igEvent *e) {
	uint32_t cnt;
	for(cnt = 0, e = e->next; e; e = e->next, cnt++);
	return cnt;
}

int igEventCmp(const void *ve1, const void *ve2) {
	const igEvent *e1 = *((const igEvent**)ve1);
	const igEvent *e2 = *((const igEvent**)ve2);
	if(e1->axis == 4 || e2->axis == 4) {
		fprintf(stderr, "blad! porownanie niezainicjowanego eventa");
		abort();
	}
	if(e1->plane < e2->plane) return -1;
	if(e1->plane > e2->plane) return 1;
	if(e1->type == IGEND) {
		if(e2->type == IGEND)
			return 0;
		return -1;
	} else if(e1->type == IGPLANAR) {
		if(e2->type == IGPLANAR) 
			return 0;
		else if(e2->type == IGEND) {
			return 1;
		}
		return -1;
	} else if(e1->type == IGBEGIN) {
		if(e2->type == IGBEGIN)
			return 0;
		return 1;
	}
	fprintf(stderr, "blad! zly protokol pornownania\n");
	abort();
	return 0;
}

void igEventsFree(igEvents *es) {
	uint32_t axis;
	for(axis = 0; axis < 3; axis++) {
		igEvent *eit = (*es)[axis];
		while(eit) {
			igEvent *prev = eit;
			eit = eit->next;
			free(prev);
		}
	}
	free(es);
}

igEvents* igInitEvents() {
	igEvents *es = malloc(sizeof(igEvents));
	uint32_t axis;
	for(axis = 0; axis < 3; axis++)
		(*es)[axis] = igInitDummyEvent();
	return es;
}

igEvent* igInitDummyEvent() {
	igEvent *e = malloc(sizeof(igEvent));
	e->next = NULL;
	e->axis = 4;
	return e;
}

void igInsertEvent(igEvents *es, igEvent *e, uint32_t axis) {
	e->next = (*es)[axis]->next;
	(*es)[axis]->next = e;
}

