regional_get_mem(struct regional* r)
{
	return r->first_size + (count_chunks(r)-1)*REGIONAL_CHUNK_SIZE 
		+ r->total_large;
}