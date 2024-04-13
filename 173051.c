static void bnx2x_set_endianity(struct bnx2x *bp)
{
#ifdef __BIG_ENDIAN
	bnx2x_config_endianity(bp, 1);
#else
	bnx2x_config_endianity(bp, 0);
#endif
}