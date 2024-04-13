announcedata_size()
{
	int size, i;
	PRINTER *printerinfo;

	size = 8;		/* static announce size */
	size += g_num_devices * 0x14;

	for (i = 0; i < g_num_devices; i++)
	{
		if (g_rdpdr_device[i].device_type == DEVICE_TYPE_PRINTER)
		{
			printerinfo = (PRINTER *) g_rdpdr_device[i].pdevice_data;
			printerinfo->bloblen =
				printercache_load_blob(printerinfo->printer, &(printerinfo->blob));

			size += 0x18;
			size += 2 * strlen(printerinfo->driver) + 2;
			size += 2 * strlen(printerinfo->printer) + 2;
			size += printerinfo->bloblen;
		}
	}

	return size;
}