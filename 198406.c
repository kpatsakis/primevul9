int sc_append_path(sc_path_t *dest, const sc_path_t *src)
{
	return sc_concatenate_path(dest, dest, src);
}