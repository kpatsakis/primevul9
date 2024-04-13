static inline u8 gfs2_testbit(const struct gfs2_rbm *rbm)
{
	struct gfs2_bitmap *bi = rbm_bi(rbm);
	const u8 *buffer = bi->bi_bh->b_data + bi->bi_offset;
	const u8 *byte;
	unsigned int bit;

	byte = buffer + (rbm->offset / GFS2_NBBY);
	bit = (rbm->offset % GFS2_NBBY) * GFS2_BIT_SIZE;

	return (*byte >> bit) & GFS2_BIT_MASK;
}