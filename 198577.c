int sc_append_file_id(sc_path_t *dest, unsigned int fid)
{
	u8 id[2] = { fid >> 8, fid & 0xff };

	return sc_append_path_id(dest, id, 2);
}