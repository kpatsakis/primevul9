get_device_index(RD_NTHANDLE handle)
{
	int i;
	for (i = 0; i < RDPDR_MAX_DEVICES; i++)
	{
		if (g_rdpdr_device[i].handle == handle)
			return i;
	}
	return -1;
}