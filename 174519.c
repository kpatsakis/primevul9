s_free(STREAM s)
{
	free(s->data);
	free(s);
}