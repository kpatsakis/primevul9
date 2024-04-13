formatErrorMessage(int errorCode, char *inBuffer, jint inBufferLength)
{
	size_t i=0;
	int rc = 0, j=0;
	size_t outLength, lastChar;
	_TCHAR buffer[JVM_DEFAULT_ERROR_BUFFER_SIZE];
	_TCHAR noCRLFbuffer[JVM_DEFAULT_ERROR_BUFFER_SIZE];

	if (inBufferLength <= 1) {
		if (inBufferLength == 1) {
			inBuffer[0] = '\0';
		}
		return 0;
	}

	rc = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, JVM_DEFAULT_ERROR_BUFFER_SIZE, NULL );
	if (rc == 0) {
		inBuffer[0] = '\0';
		return 0;
	}

	j=0;
	for (i = 0; i < _tcslen(buffer)+1 ; i++) {
		if(buffer[i] == _TEXT('\n') ) {
			noCRLFbuffer[j++]=_TEXT(' ');
		} else if(buffer[i] == _TEXT('\r')) {
			continue;
		} else {
			noCRLFbuffer[j++]=buffer[i];
		}
	}

	lastChar = _tcslen(noCRLFbuffer)-1;

	if(_istspace(noCRLFbuffer[lastChar])) {
		noCRLFbuffer[lastChar] = _TEXT('\0');
	}

/* We always return multibyte */

#ifdef UNICODE
	outLength = WideCharToMultiByte(CP_UTF8, 0, noCRLFbuffer, -1, inBuffer, inBufferLength-1, NULL, NULL);
#else
	outLength = strlen(noCRLFbuffer)+1;
	if(outLength > (size_t)inBufferLength) {
		outLength = (size_t)inBufferLength;
	}
	strncpy(inBuffer, noCRLFbuffer, outLength);
	inBuffer[inBufferLength-1]='\0';
#endif

	Assert_SC_true(outLength <= I_32_MAX);
	return (jint)outLength;
}