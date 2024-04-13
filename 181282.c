static void dup_copy_fields(struct nfsd4_copy *src, struct nfsd4_copy *dst)
{
	dst->cp_src_pos = src->cp_src_pos;
	dst->cp_dst_pos = src->cp_dst_pos;
	dst->cp_count = src->cp_count;
	dst->cp_synchronous = src->cp_synchronous;
	memcpy(&dst->cp_res, &src->cp_res, sizeof(src->cp_res));
	memcpy(&dst->fh, &src->fh, sizeof(src->fh));
	dst->cp_clp = src->cp_clp;
	dst->file_dst = get_file(src->file_dst);
	dst->file_src = get_file(src->file_src);
	memcpy(&dst->cp_stateid, &src->cp_stateid, sizeof(src->cp_stateid));
}