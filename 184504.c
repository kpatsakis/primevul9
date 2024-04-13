static J9StringBuffer* jvmBufferEnsure(J9StringBuffer* buffer, UDATA len) {

	if (buffer == NULL) {
		UDATA newSize = len > MIN_GROWTH ? len : MIN_GROWTH;
		buffer = (J9StringBuffer*) malloc( newSize + 1 + sizeof(UDATA));	/* 1 for null terminator */
		if (buffer != NULL) {
			buffer->remaining = newSize;
			buffer->data[0] = '\0';
		}
		return buffer;
	}

	if (len > buffer->remaining) {
		UDATA newSize = len > MIN_GROWTH ? len : MIN_GROWTH;
		J9StringBuffer* new = (J9StringBuffer*) malloc( strlen((const char*)buffer->data) + newSize + sizeof(UDATA) + 1 );
		if (new) {
			new->remaining = newSize;
			strcpy(new->data, (const char*)buffer->data);
		}
		free(buffer);
		return new;
	}

	return buffer;
}