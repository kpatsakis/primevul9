update_tls_reference_bitmap (guint32 offset, uintptr_t *bitmap, int max_set)
{
	int i;
	int idx = (offset >> 24) - 1;
	uintptr_t *rb;
	if (!static_reference_bitmaps [idx])
		static_reference_bitmaps [idx] = g_new0 (uintptr_t, 1 + static_data_size [idx] / sizeof(gpointer) / (sizeof(uintptr_t) * 8));
	rb = static_reference_bitmaps [idx];
	offset &= 0xffffff;
	offset /= sizeof (gpointer);
	/* offset is now the bitmap offset */
	for (i = 0; i < max_set; ++i) {
		if (bitmap [i / sizeof (uintptr_t)] & (1L << (i & (sizeof (uintptr_t) * 8 -1))))
			rb [(offset + i) / (sizeof (uintptr_t) * 8)] |= (1L << ((offset + i) & (sizeof (uintptr_t) * 8 -1)));
	}
}