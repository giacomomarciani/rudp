#include "rudpsgmbuffer.h"

SegmentBuffer *createSegmentBuffer(void) {
	SegmentBuffer *buff = NULL;

	if (!(buff = malloc(sizeof(SegmentBuffer))) ||
		!(buff->mtx = malloc(sizeof(pthread_mutex_t))) ||
		!(buff->insert_cnd = malloc(sizeof(pthread_cond_t))) ||
		!(buff->remove_cnd = malloc(sizeof(pthread_cond_t))) ||
		!(buff->status_cnd = malloc(sizeof(pthread_cond_t))))
		ERREXIT("Cannot allocate memory for timeout segment buffer resources.");

	initializeMutex(buff->mtx);

	initializeConditionVariable(buff->insert_cnd);

	initializeConditionVariable(buff->remove_cnd);

	initializeConditionVariable(buff->status_cnd);

	buff->size = 0;
	
	buff->head = NULL;

	buff->tail = NULL;

	return buff;
}

void freeSegmentBuffer(SegmentBuffer *buff) {

	while (buff->head)
		removeSegmentBuffer(buff, buff->head);

	destroyMutex(buff->mtx);

	destroyConditionVariable(buff->insert_cnd);

	destroyConditionVariable(buff->remove_cnd);

	destroyConditionVariable(buff->status_cnd);

	free(buff);
}

SegmentBufferElement *addSegmentBuffer(SegmentBuffer *buff, const Segment sgm) {
	SegmentBufferElement *new = NULL;

	if (!(new = malloc(sizeof(SegmentBufferElement))))
		ERREXIT("Cannot allocate memory for new segment buffer element.");

	new->segment = sgm;

	if (buff->size == 0) {

		new->prev = NULL;

		new->next = NULL;

		buff->head = new;

		buff->tail = new;

	} else {

		new->prev = buff->tail;

		new->next = NULL;

		buff->tail->next = new;

		buff->tail = new;
	} 

	buff->size++;

	return new;
}

SegmentBufferElement *findSegmentBufferBySequence(SegmentBuffer *buff, const uint32_t seqn) {
	SegmentBufferElement *curr = buff->head;
	SegmentBufferElement *trgt = NULL;

	while (curr) {

		if (curr->segment.hdr.seqn == seqn) {
			
			trgt = curr;

			break;
		}

		curr = curr->next;
	}

	return trgt;
}

SegmentBufferElement *findSegmentBufferByAck(SegmentBuffer *buff, const uint32_t ackn) {
	SegmentBufferElement *curr = buff->head;
	SegmentBufferElement *trgt = NULL;

	while (curr) {

		if (RUDP_ISACKED(curr->segment.hdr.seqn, curr->segment.hdr.plds, ackn)) {
			
			trgt = curr;

			break;
		}

		curr = curr->next;
	}

	return trgt;
}

void removeSegmentBuffer(SegmentBuffer *buff, SegmentBufferElement *elem) {

	if (!elem)
		return;

	if ((elem == buff->head) && (elem == buff->tail)) {

		buff->head = NULL;

		buff->tail = NULL;			

	} else if (elem == buff->head) {

		buff->head = elem->next;

		elem->next->prev = NULL;

	} else if (elem == buff->tail) {

		buff->tail = elem->prev;

		elem->prev->next = NULL;

	} else {

		elem->prev->next = elem->next;

		elem->next->prev = elem->prev;

	}

	free(elem);

	buff->size--;
}

char *segmentBufferToString(SegmentBuffer *buff) {
	SegmentBufferElement *curr = NULL;
	char *strbuff, *strsgm = NULL;	
	uint32_t i;

	if (!(strbuff = malloc(sizeof(char) * (25 + buff->size * (RUDP_SGMSO + 1) + 1))))
		ERREXIT("Cannot allocate memory for string representation of segment buffer.");

	sprintf(strbuff, "size:%ld content:%s", buff->size, (buff->size == 0) ? "" : "\n");

	curr = buff->head;

	i = (uint32_t) strlen(strbuff);

	while (curr) {

		strsgm = segmentToString(curr->segment);

		memcpy(strbuff + i, strsgm, sizeof(char) * strlen(strsgm));		

		i += (uint32_t) strlen(strsgm);

		free(strsgm);

		if (curr->next) {

			strbuff[i] = '\n';

			i++;
		}		

		curr = curr->next;
	}

	strbuff[i] = '\0';
	
	return strbuff;
}