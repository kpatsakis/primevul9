rdp_out_bmpcache2_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BMPCACHE2);
	out_uint16_le(s, RDP_CAPLEN_BMPCACHE2);

	out_uint16_le(s, g_bitmap_cache_persist_enable ? 2 : 0);	/* version */

	out_uint16_be(s, 3);	/* number of caches in this set */

	/* max cell size for cache 0 is 16x16, 1 = 32x32, 2 = 64x64, etc */
	out_uint32_le(s, BMPCACHE2_C0_CELLS);
	out_uint32_le(s, BMPCACHE2_C1_CELLS);
	if (pstcache_init(2))
	{
		out_uint32_le(s, BMPCACHE2_NUM_PSTCELLS | BMPCACHE2_FLAG_PERSIST);
	}
	else
	{
		out_uint32_le(s, BMPCACHE2_C2_CELLS);
	}
	out_uint8s(s, 20);	/* other bitmap caches not used */
}