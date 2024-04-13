rdpsnddbg_line_handler(const char *line, void *data)
{
#ifdef WITH_DEBUG_SOUND
	fprintf(stderr, "SNDDBG: %s\n", line);
#endif
	return True;
}