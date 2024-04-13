mark_tls_slots (void *addr, MonoGCMarkFunc mark_func)
{
	int i;
	gpointer *static_data = addr;
	for (i = 0; i < NUM_STATIC_DATA_IDX; ++i) {
		int j, numwords;
		void **ptr;
		if (!static_data [i])
			continue;
		numwords = 1 + static_data_size [i] / sizeof (gpointer) / (sizeof(uintptr_t) * 8);
		ptr = static_data [i];
		for (j = 0; j < numwords; ++j, ptr += sizeof (uintptr_t) * 8) {
			uintptr_t bmap = static_reference_bitmaps [i][j];
			void ** p = ptr;
			while (bmap) {
				if ((bmap & 1) && *p) {
					mark_func (p);
				}
				p++;
				bmap >>= 1;
			}
		}
	}
}