streamsave(STREAM s, char *fn)
{
	FILE *f = fopen(fn, "wb");
	fwrite(s->data, s_length(s), 1, f);
	fclose(f);
}