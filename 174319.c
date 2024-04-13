rdpdr_process_irp(STREAM s)
{
	uint32 result = 0,
		length = 0,
		desired_access = 0,
		request,
		file,
		info_level,
		buffer_len,
		id,
		major,
		minor,
		device,
		offset,
		bytes_in,
		bytes_out,
		error_mode,
		share_mode, disposition, total_timeout, interval_timeout, flags_and_attributes = 0;

	char *filename;
	uint32 filename_len;

	uint8 *buffer, *pst_buf;
	struct stream out;
	DEVICE_FNS *fns;
	RD_BOOL rw_blocking = True;
	RD_NTSTATUS status = RD_STATUS_INVALID_DEVICE_REQUEST;

	in_uint32_le(s, device);
	in_uint32_le(s, file);
	in_uint32_le(s, id);
	in_uint32_le(s, major);
	in_uint32_le(s, minor);

	filename = NULL;

	buffer_len = 0;
	buffer = (uint8 *) xmalloc(1024);
	buffer[0] = 0;

	if (device >= RDPDR_MAX_DEVICES)
	{
		error("invalid irp device 0x%lx file 0x%lx id 0x%lx major 0x%lx minor 0x%lx\n",
		      device, file, id, major, minor);
		xfree(buffer);
		return;
	}

	switch (g_rdpdr_device[device].device_type)
	{
		case DEVICE_TYPE_SERIAL:

			fns = &serial_fns;
			rw_blocking = False;
			break;

		case DEVICE_TYPE_PARALLEL:

			fns = &parallel_fns;
			rw_blocking = False;
			break;

		case DEVICE_TYPE_PRINTER:

			fns = &printer_fns;
			break;

		case DEVICE_TYPE_DISK:

			fns = &disk_fns;
			rw_blocking = False;
			break;

		case DEVICE_TYPE_SCARD:
#ifdef WITH_SCARD
			fns = &scard_fns;
			rw_blocking = False;
			break;
#endif
		default:

			error("IRP for bad device %ld\n", device);
			xfree(buffer);
			return;
	}

	switch (major)
	{
		case IRP_MJ_CREATE:

			in_uint32_be(s, desired_access);
			in_uint8s(s, 0x08);	/* unknown */
			in_uint32_le(s, error_mode);
			in_uint32_le(s, share_mode);
			in_uint32_le(s, disposition);
			in_uint32_le(s, flags_and_attributes);
			in_uint32_le(s, length);

			if (length && (length / 2) < 256)
			{
				rdp_in_unistr(s, length, &filename, &filename_len);
				if (filename)
					convert_to_unix_filename(filename);
			}

			if (!fns->create)
			{
				status = RD_STATUS_NOT_SUPPORTED;
				free(filename);
				break;
			}

			status = fns->create(device, desired_access, share_mode, disposition,
					     flags_and_attributes, filename, &result);

			free(filename);
			buffer_len = 1;
			break;

		case IRP_MJ_CLOSE:
			if (!fns->close)
			{
				status = RD_STATUS_NOT_SUPPORTED;
				break;
			}

			status = fns->close(file);
			break;

		case IRP_MJ_READ:

			if (!fns->read)
			{
				status = RD_STATUS_NOT_SUPPORTED;
				break;
			}

			in_uint32_le(s, length);
			in_uint32_le(s, offset);
#if WITH_DEBUG_RDP5
			DEBUG(("RDPDR IRP Read (length: %d, offset: %d)\n", length, offset));
#endif
			if (!rdpdr_handle_ok(device, file))
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			if (rw_blocking)	/* Complete read immediately */
			{
				buffer = (uint8 *) xrealloc((void *) buffer, length);
				if (!buffer)
				{
					status = RD_STATUS_CANCELLED;
					break;
				}
				status = fns->read(file, buffer, length, offset, &result);
				buffer_len = result;
				break;
			}

			/* Add request to table */
			pst_buf = (uint8 *) xmalloc(length);
			if (!pst_buf)
			{
				status = RD_STATUS_CANCELLED;
				break;
			}
			serial_get_timeout(file, length, &total_timeout, &interval_timeout);
			if (add_async_iorequest
			    (device, file, id, major, length, fns, total_timeout, interval_timeout,
			     pst_buf, offset))
			{
				status = RD_STATUS_PENDING;
				break;
			}

			status = RD_STATUS_CANCELLED;
			break;
		case IRP_MJ_WRITE:

			buffer_len = 1;

			if (!fns->write)
			{
				status = RD_STATUS_NOT_SUPPORTED;
				break;
			}

			in_uint32_le(s, length);
			in_uint32_le(s, offset);
			in_uint8s(s, 0x18);
#if WITH_DEBUG_RDP5
			DEBUG(("RDPDR IRP Write (length: %d)\n", result));
#endif
			if (!rdpdr_handle_ok(device, file))
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			if (rw_blocking)	/* Complete immediately */
			{
				status = fns->write(file, s->p, length, offset, &result);
				break;
			}

			/* Add to table */
			pst_buf = (uint8 *) xmalloc(length);
			if (!pst_buf)
			{
				status = RD_STATUS_CANCELLED;
				break;
			}

			in_uint8a(s, pst_buf, length);

			if (add_async_iorequest
			    (device, file, id, major, length, fns, 0, 0, pst_buf, offset))
			{
				status = RD_STATUS_PENDING;
				break;
			}

			status = RD_STATUS_CANCELLED;
			break;

		case IRP_MJ_QUERY_INFORMATION:

			if (g_rdpdr_device[device].device_type != DEVICE_TYPE_DISK)
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}
			in_uint32_le(s, info_level);

			out.data = out.p = buffer;
			out.size = sizeof(buffer);
			status = disk_query_information(file, info_level, &out);
			result = buffer_len = out.p - out.data;

			break;

		case IRP_MJ_SET_INFORMATION:

			if (g_rdpdr_device[device].device_type != DEVICE_TYPE_DISK)
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			in_uint32_le(s, info_level);

			out.data = out.p = buffer;
			out.size = sizeof(buffer);
			status = disk_set_information(file, info_level, s, &out);
			result = buffer_len = out.p - out.data;
			break;

		case IRP_MJ_QUERY_VOLUME_INFORMATION:

			if (g_rdpdr_device[device].device_type != DEVICE_TYPE_DISK)
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			in_uint32_le(s, info_level);

			out.data = out.p = buffer;
			out.size = sizeof(buffer);
			status = disk_query_volume_information(file, info_level, &out);
			result = buffer_len = out.p - out.data;
			break;

		case IRP_MJ_DIRECTORY_CONTROL:

			if (g_rdpdr_device[device].device_type != DEVICE_TYPE_DISK)
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			switch (minor)
			{
				case IRP_MN_QUERY_DIRECTORY:

					in_uint32_le(s, info_level);
					in_uint8s(s, 1);
					in_uint32_le(s, length);
					in_uint8s(s, 0x17);
					if (length && length < 2 * 255)
					{
						rdp_in_unistr(s, length, &filename, &filename_len);
						if (filename)
							convert_to_unix_filename(filename);
					}

					out.data = out.p = buffer;
					out.size = sizeof(buffer);
					status = disk_query_directory(file, info_level, filename,
								      &out);
					result = buffer_len = out.p - out.data;
					if (!buffer_len)
						buffer_len++;

					free(filename);
					break;

				case IRP_MN_NOTIFY_CHANGE_DIRECTORY:

					/* JIF
					   unimpl("IRP major=0x%x minor=0x%x: IRP_MN_NOTIFY_CHANGE_DIRECTORY\n", major, minor);  */

					in_uint32_le(s, info_level);	/* notify mask */

					status = disk_create_notify(file, info_level);
					result = 0;

					if (status == RD_STATUS_PENDING)
						add_async_iorequest(device, file, id, major, length,
								    fns, 0, 0, NULL, 0);
					break;

				default:

					status = RD_STATUS_INVALID_PARAMETER;
					/* JIF */
					unimpl("IRP major=0x%x minor=0x%x\n", major, minor);
			}
			break;

		case IRP_MJ_DEVICE_CONTROL:

			if (!fns->device_control)
			{
				status = RD_STATUS_NOT_SUPPORTED;
				break;
			}

			in_uint32_le(s, bytes_out);
			in_uint32_le(s, bytes_in);
			in_uint32_le(s, request);
			in_uint8s(s, 0x14);

			buffer = (uint8 *) xrealloc((void *) buffer, bytes_out + 0x14);
			if (!buffer)
			{
				status = RD_STATUS_CANCELLED;
				break;
			}

			out.data = out.p = buffer;
			out.size = sizeof(buffer);

#ifdef WITH_SCARD
			scardSetInfo(g_epoch, device, id, bytes_out + 0x14);
#endif
			status = fns->device_control(file, request, s, &out);
			result = buffer_len = out.p - out.data;

			/* Serial SERIAL_WAIT_ON_MASK */
			if (status == RD_STATUS_PENDING)
			{
				if (add_async_iorequest
				    (device, file, id, major, length, fns, 0, 0, NULL, 0))
				{
					status = RD_STATUS_PENDING;
					break;
				}
			}
#ifdef WITH_SCARD
			else if (status == (RD_STATUS_PENDING | 0xC0000000))
				status = RD_STATUS_PENDING;
#endif
			break;


		case IRP_MJ_LOCK_CONTROL:

			if (g_rdpdr_device[device].device_type != DEVICE_TYPE_DISK)
			{
				status = RD_STATUS_INVALID_HANDLE;
				break;
			}

			in_uint32_le(s, info_level);

			out.data = out.p = buffer;
			out.size = sizeof(buffer);
			/* FIXME: Perhaps consider actually *do*
			   something here :-) */
			status = RD_STATUS_SUCCESS;
			result = buffer_len = out.p - out.data;
			break;

		default:
			unimpl("IRP major=0x%x minor=0x%x\n", major, minor);
			break;
	}

	if (status != RD_STATUS_PENDING)
	{
		rdpdr_send_completion(device, id, status, result, buffer, buffer_len);
	}
	if (buffer)
		xfree(buffer);
	buffer = NULL;
}