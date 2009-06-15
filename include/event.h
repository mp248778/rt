#ifndef EVENT_HEADER
#define EVENT_HEADER

#include "triangle.h"
#include "aabb.h"

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

igEvents* igInitEvents();
void igCreateEvent(AABB*, igEvents*, unsigned);
igEvents* igCreateEvents(Triangle *, unsigned);
igEvents* igMergeEvents(igEvents*, igEvents*);
int igEventCmp(const void *, const void*);
unsigned igCountEvents(igEvent*);
unsigned igCountTriangles(igEvents*);
void igEventsFree(igEvents *);
void igSortEvents(igEvents*);

#endif
