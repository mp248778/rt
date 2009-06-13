#ifndef EVENT_HEADER
#define EVENT_HEADER

#include "triangle.h"

typedef enum {
	IGEND,
	IGBEGIN,
	IGPLANAR
} Type;

typedef struct igEvent {
	unsigned axis;
	float plane;
	Type type;
	struct igEvent *next;
	unsigned trid;
} igEvent;

typedef igEvent* igEvents[3];


igEvents* igCreateEvents(Triangle *, unsigned);
void igEventsFree(igEvents *);
void igSortEvents(igEvents*);
igEvents* igMergeEvents(igEvents*, igEvents*);
unsigned igCountTriangles(igEvents*);
unsigned igCountEvents(igEvent*);
int igEventCmp(const void *, const void*);



#endif
