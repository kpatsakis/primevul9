static char* jvmBufferData(J9StringBuffer* buffer) {
	return buffer ? buffer->data : NULL;
}