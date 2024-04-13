jvmBufferCat(J9StringBuffer* buffer, const char* string)
{
	UDATA len = strlen(string);

	buffer = jvmBufferEnsure(buffer, len);
	if (buffer) {
		strcat(buffer->data, string);
		buffer->remaining -= len;
	}

	return buffer;
}