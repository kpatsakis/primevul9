static int network_write (const data_set_t *ds, const value_list_t *vl,
		user_data_t __attribute__((unused)) *user_data)
{
	int status;

	if (!check_send_okay (vl))
	{
#if COLLECT_DEBUG
	  char name[6*DATA_MAX_NAME_LEN];
	  FORMAT_VL (name, sizeof (name), vl);
	  name[sizeof (name) - 1] = 0;
	  DEBUG ("network plugin: network_write: "
	      "NOT sending %s.", name);
#endif
	  /* Counter is not protected by another lock and may be reached by
	   * multiple threads */
	  pthread_mutex_lock (&stats_lock);
	  stats_values_not_sent++;
	  pthread_mutex_unlock (&stats_lock);
	  return (0);
	}

	uc_meta_data_add_unsigned_int (vl,
	    "network:time_sent", (uint64_t) vl->time);

	pthread_mutex_lock (&send_buffer_lock);

	status = add_to_buffer (send_buffer_ptr,
			network_config_packet_size - (send_buffer_fill + BUFF_SIG_SIZE),
			&send_buffer_vl,
			ds, vl);
	if (status >= 0)
	{
		/* status == bytes added to the buffer */
		send_buffer_fill += status;
		send_buffer_ptr  += status;

		stats_values_sent++;
	}
	else
	{
		flush_buffer ();

		status = add_to_buffer (send_buffer_ptr,
				network_config_packet_size - (send_buffer_fill + BUFF_SIG_SIZE),
				&send_buffer_vl,
				ds, vl);

		if (status >= 0)
		{
			send_buffer_fill += status;
			send_buffer_ptr  += status;

			stats_values_sent++;
		}
	}

	if (status < 0)
	{
		ERROR ("network plugin: Unable to append to the "
				"buffer for some weird reason");
	}
	else if ((network_config_packet_size - send_buffer_fill) < 15)
	{
		flush_buffer ();
	}

	pthread_mutex_unlock (&send_buffer_lock);

	return ((status < 0) ? -1 : 0);
} /* int network_write */