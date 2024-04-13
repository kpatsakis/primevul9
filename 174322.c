rdpdr_handle_ok(int device, int handle)
{
	switch (g_rdpdr_device[device].device_type)
	{
		case DEVICE_TYPE_PARALLEL:
		case DEVICE_TYPE_SERIAL:
		case DEVICE_TYPE_PRINTER:
		case DEVICE_TYPE_SCARD:
			if (g_rdpdr_device[device].handle != handle)
				return False;
			break;
		case DEVICE_TYPE_DISK:
			if (g_fileinfo[handle].device_id != device)
				return False;
			break;
	}
	return True;
}