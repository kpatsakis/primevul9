static void mono_init_static_data_info (StaticDataInfo *static_data)
{
	static_data->idx = 0;
	static_data->offset = 0;
	static_data->freelist = NULL;
}