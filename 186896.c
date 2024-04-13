magic_load(struct magic_set *ms, const char *magicfile)
{
	if (ms == NULL)
	return -1;
	return file_apprentice(ms, magicfile, FILE_LOAD);
}