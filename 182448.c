static inline void init_copy_chunk_defaults(struct cifs_tcon *tcon)
{
	tcon->max_chunks = 256;
	tcon->max_bytes_chunk = 1048576;
	tcon->max_bytes_copy = 16777216;
}