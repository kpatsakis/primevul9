mono_alloc_static_data_slot (StaticDataInfo *static_data, guint32 size, guint32 align)
{
	guint32 offset;

	if (!static_data->idx && !static_data->offset) {
		/* 
		 * we use the first chunk of the first allocation also as
		 * an array for the rest of the data 
		 */
		static_data->offset = sizeof (gpointer) * NUM_STATIC_DATA_IDX;
	}
	static_data->offset += align - 1;
	static_data->offset &= ~(align - 1);
	if (static_data->offset + size >= static_data_size [static_data->idx]) {
		static_data->idx ++;
		g_assert (size <= static_data_size [static_data->idx]);
		g_assert (static_data->idx < NUM_STATIC_DATA_IDX);
		static_data->offset = 0;
	}
	offset = static_data->offset | ((static_data->idx + 1) << 24);
	static_data->offset += size;
	return offset;
}