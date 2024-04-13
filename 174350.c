rdpsnd_process_packet(uint8 opcode, STREAM s)
{
	uint16 vol_left, vol_right;
	static uint16 tick, format;
	static uint8 packet_index;

	switch (opcode)
	{
		case RDPSND_WRITE:
			in_uint16_le(s, tick);
			in_uint16_le(s, format);
			in_uint8(s, packet_index);
			in_uint8s(s, 3);
			DEBUG_SOUND(("RDPSND: RDPSND_WRITE(tick: %u, format: %u, index: %u, data: %u bytes)\n", (unsigned) tick, (unsigned) format, (unsigned) packet_index, (unsigned) s->size - 8));

			if (format >= MAX_FORMATS)
			{
				error("RDPSND: Invalid format index\n");
				break;
			}

			if (!device_open || (format != current_format))
			{
				/*
				 * If we haven't selected a device by now, then either
				 * we've failed to find a working device, or the server
				 * is sending bogus RDPSND_WRITE.
				 */
				if (!current_driver)
				{
					rdpsnd_send_completion(tick, packet_index);
					break;
				}
				if (!device_open && !current_driver->wave_out_open())
				{
					rdpsnd_send_completion(tick, packet_index);
					break;
				}
				if (!current_driver->wave_out_set_format(&formats[format]))
				{
					rdpsnd_send_completion(tick, packet_index);
					current_driver->wave_out_close();
					device_open = False;
					break;
				}
				device_open = True;
				current_format = format;
			}

			rdpsnd_queue_write(rdpsnd_dsp_process
					   (s->p, s->end - s->p, current_driver,
					    &formats[current_format]), tick, packet_index);
			return;
			break;
		case RDPSND_CLOSE:
			DEBUG_SOUND(("RDPSND: RDPSND_CLOSE()\n"));
			if (device_open)
				current_driver->wave_out_close();
			device_open = False;
			break;
		case RDPSND_NEGOTIATE:
			rdpsnd_process_negotiate(s);
			break;
		case RDPSND_PING:
			rdpsnd_process_ping(s);
			break;
		case RDPSND_SET_VOLUME:
			in_uint16_le(s, vol_left);
			in_uint16_le(s, vol_right);
			DEBUG_SOUND(("RDPSND: RDPSND_VOLUME(left: 0x%04x (%u %%), right: 0x%04x (%u %%))\n", (unsigned) vol_left, (unsigned) vol_left / 655, (unsigned) vol_right, (unsigned) vol_right / 655));
			if (device_open)
				current_driver->wave_out_volume(vol_left, vol_right);
			break;
		case RDPSND_REC_NEGOTIATE:
			rdpsnd_process_rec_negotiate(s);
			break;
		case RDPSND_REC_START:
			in_uint16_le(s, format);
			DEBUG_SOUND(("RDPSND: RDPSND_REC_START(format: %u)\n", (unsigned) format));

			if (format >= MAX_FORMATS)
			{
				error("RDPSND: Invalid format index\n");
				break;
			}

			if (rec_device_open)
			{
				error("RDPSND: Multiple RDPSND_REC_START\n");
				break;
			}

			if (!current_driver->wave_in_open())
				break;

			if (!current_driver->wave_in_set_format(&rec_formats[format]))
			{
				error("RDPSND: Device not accepting format\n");
				current_driver->wave_in_close();
				break;
			}
			rec_current_format = format;
			rec_device_open = True;
			break;
		case RDPSND_REC_STOP:
			DEBUG_SOUND(("RDPSND: RDPSND_REC_STOP()\n"));
			rdpsnd_flush_record();
			if (rec_device_open)
				current_driver->wave_in_close();
			rec_device_open = False;
			break;
		case RDPSND_REC_SET_VOLUME:
			in_uint16_le(s, vol_left);
			in_uint16_le(s, vol_right);
			DEBUG_SOUND(("RDPSND: RDPSND_REC_VOLUME(left: 0x%04x (%u %%), right: 0x%04x (%u %%))\n", (unsigned) vol_left, (unsigned) vol_left / 655, (unsigned) vol_right, (unsigned) vol_right / 655));
			if (rec_device_open)
				current_driver->wave_in_volume(vol_left, vol_right);
			break;
		default:
			unimpl("RDPSND packet type %x\n", opcode);
			break;
	}
}