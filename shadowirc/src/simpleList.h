typedef struct simpleList {
	int eltCount;
	int (*compareFn)(const void *, const void *);
	
	void* *elts;
} simpleList, *simpleListPtr;


simpleListPtr NewSimpleList(int (*compareFn)(const void *, const void *));
void FreeSimpleList(simpleListPtr sl);

void SLAddItem(simpleListPtr sl, void* item);

void SLSort(simpleListPtr sl);

//-

int SLPstrCaseCompare(const void* a, const void* b);
