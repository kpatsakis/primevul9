_rdpdr_check_fds(fd_set * rfds, fd_set * wfds, RD_BOOL timed_out)
{
	RD_NTSTATUS status;
	uint32 result = 0;
	DEVICE_FNS *fns;
	struct async_iorequest *iorq;
	struct async_iorequest *prev;
	uint32 req_size = 0;
	uint32 buffer_len;
	struct stream out;
	uint8 *buffer = NULL;


	if (timed_out)
	{
		/* check serial iv_timeout */

		iorq = g_iorequest;
		prev = NULL;
		while (iorq != NULL)
		{
			if (iorq->fd == g_min_timeout_fd)
			{
				if ((iorq->partial_len > 0) &&
				    (g_rdpdr_device[iorq->device].device_type ==
				     DEVICE_TYPE_SERIAL))
				{

					/* iv_timeout between 2 chars, send partial_len */
					/*printf("RDPDR: IVT total %u bytes read of %u\n", iorq->partial_len, iorq->length); */
					rdpdr_send_completion(iorq->device,
							      iorq->id, RD_STATUS_SUCCESS,
							      iorq->partial_len,
							      iorq->buffer, iorq->partial_len);
					iorq = rdpdr_remove_iorequest(prev, iorq);
					return;
				}
				else
				{
					break;
				}

			}
			else
			{
				break;
			}


			prev = iorq;
			if (iorq)
				iorq = iorq->next;

		}

		rdpdr_abort_io(g_min_timeout_fd, 0, RD_STATUS_TIMEOUT);
		return;
	}

	iorq = g_iorequest;
	prev = NULL;
	while (iorq != NULL)
	{
		if (iorq->fd != 0)
		{
			switch (iorq->major)
			{
				case IRP_MJ_READ:
					if (FD_ISSET(iorq->fd, rfds))
					{
						/* Read the data */
						fns = iorq->fns;

						req_size =
							(iorq->length - iorq->partial_len) >
							8192 ? 8192 : (iorq->length -
								       iorq->partial_len);
						/* never read larger chunks than 8k - chances are that it will block */
						status = fns->read(iorq->fd,
								   iorq->buffer + iorq->partial_len,
								   req_size, iorq->offset, &result);

						if ((long) result > 0)
						{
							iorq->partial_len += result;
							iorq->offset += result;
						}
#if WITH_DEBUG_RDP5
						DEBUG(("RDPDR: %d bytes of data read\n", result));
#endif
						/* only delete link if all data has been transfered */
						/* or if result was 0 and status success - EOF      */
						if ((iorq->partial_len == iorq->length) ||
						    (result == 0))
						{
#if WITH_DEBUG_RDP5
							DEBUG(("RDPDR: AIO total %u bytes read of %u\n", iorq->partial_len, iorq->length));
#endif
							rdpdr_send_completion(iorq->device,
									      iorq->id, status,
									      iorq->partial_len,
									      iorq->buffer,
									      iorq->partial_len);
							iorq = rdpdr_remove_iorequest(prev, iorq);
						}
					}
					break;
				case IRP_MJ_WRITE:
					if (FD_ISSET(iorq->fd, wfds))
					{
						/* Write data. */
						fns = iorq->fns;

						req_size =
							(iorq->length - iorq->partial_len) >
							8192 ? 8192 : (iorq->length -
								       iorq->partial_len);

						/* never write larger chunks than 8k - chances are that it will block */
						status = fns->write(iorq->fd,
								    iorq->buffer +
								    iorq->partial_len, req_size,
								    iorq->offset, &result);

						if ((long) result > 0)
						{
							iorq->partial_len += result;
							iorq->offset += result;
						}

#if WITH_DEBUG_RDP5
						DEBUG(("RDPDR: %d bytes of data written\n",
						       result));
#endif
						/* only delete link if all data has been transfered */
						/* or we couldn't write */
						if ((iorq->partial_len == iorq->length)
						    || (result == 0))
						{
#if WITH_DEBUG_RDP5
							DEBUG(("RDPDR: AIO total %u bytes written of %u\n", iorq->partial_len, iorq->length));
#endif
							rdpdr_send_completion(iorq->device,
									      iorq->id, status,
									      iorq->partial_len,
									      (uint8 *) "", 1);

							iorq = rdpdr_remove_iorequest(prev, iorq);
						}
					}
					break;
				case IRP_MJ_DEVICE_CONTROL:
					if (serial_get_event(iorq->fd, &result))
					{
						buffer = (uint8 *) xrealloc((void *) buffer, 0x14);
						out.data = out.p = buffer;
						out.size = sizeof(buffer);
						out_uint32_le(&out, result);
						result = buffer_len = out.p - out.data;
						status = RD_STATUS_SUCCESS;
						rdpdr_send_completion(iorq->device, iorq->id,
								      status, result, buffer,
								      buffer_len);
						xfree(buffer);
						iorq = rdpdr_remove_iorequest(prev, iorq);
					}

					break;
			}

		}
		prev = iorq;
		if (iorq)
			iorq = iorq->next;
	}

	/* Check notify */
	iorq = g_iorequest;
	prev = NULL;
	while (iorq != NULL)
	{
		if (iorq->fd != 0)
		{
			switch (iorq->major)
			{

				case IRP_MJ_DIRECTORY_CONTROL:
					if (g_rdpdr_device[iorq->device].device_type ==
					    DEVICE_TYPE_DISK)
					{

						if (g_notify_stamp)
						{
							g_notify_stamp = False;
							status = disk_check_notify(iorq->fd);
							if (status != RD_STATUS_PENDING)
							{
								rdpdr_send_completion(iorq->device,
										      iorq->id,
										      status, 0,
										      NULL, 0);
								iorq = rdpdr_remove_iorequest(prev,
											      iorq);
							}
						}
					}
					break;



			}
		}

		prev = iorq;
		if (iorq)
			iorq = iorq->next;
	}

}