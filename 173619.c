compileError(FileInfo *nested, char *format, ...) {
#ifndef __SYMBIAN32__
	char buffer[MAXSTRING];
	va_list arguments;
	va_start(arguments, format);
	vsnprintf(buffer, sizeof(buffer), format, arguments);
	va_end(arguments);
	if (nested)
		_lou_logMessage(LOG_ERROR, "%s:%d: error: %s", nested->fileName,
				nested->lineNumber, buffer);
	else
		_lou_logMessage(LOG_ERROR, "error: %s", buffer);
	errorCount++;
#endif
}