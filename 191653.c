static int write_part_values (char **ret_buffer, int *ret_buffer_len,
		const data_set_t *ds, const value_list_t *vl)
{
	char *packet_ptr;
	int packet_len;
	int num_values;

	part_header_t pkg_ph;
	uint16_t      pkg_num_values;
	uint8_t      *pkg_values_types;
	value_t      *pkg_values;

	int offset;
	int i;

	num_values = vl->values_len;
	packet_len = sizeof (part_header_t) + sizeof (uint16_t)
		+ (num_values * sizeof (uint8_t))
		+ (num_values * sizeof (value_t));

	if (*ret_buffer_len < packet_len)
		return (-1);

	pkg_values_types = (uint8_t *) malloc (num_values * sizeof (uint8_t));
	if (pkg_values_types == NULL)
	{
		ERROR ("network plugin: write_part_values: malloc failed.");
		return (-1);
	}

	pkg_values = (value_t *) malloc (num_values * sizeof (value_t));
	if (pkg_values == NULL)
	{
		free (pkg_values_types);
		ERROR ("network plugin: write_part_values: malloc failed.");
		return (-1);
	}

	pkg_ph.type = htons (TYPE_VALUES);
	pkg_ph.length = htons (packet_len);

	pkg_num_values = htons ((uint16_t) vl->values_len);

	for (i = 0; i < num_values; i++)
	{
		pkg_values_types[i] = (uint8_t) ds->ds[i].type;
		switch (ds->ds[i].type)
		{
			case DS_TYPE_COUNTER:
				pkg_values[i].counter = htonll (vl->values[i].counter);
				break;

			case DS_TYPE_GAUGE:
				pkg_values[i].gauge = htond (vl->values[i].gauge);
				break;

			case DS_TYPE_DERIVE:
				pkg_values[i].derive = htonll (vl->values[i].derive);
				break;

			case DS_TYPE_ABSOLUTE:
				pkg_values[i].absolute = htonll (vl->values[i].absolute);
				break;

			default:
				free (pkg_values_types);
				free (pkg_values);
				ERROR ("network plugin: write_part_values: "
						"Unknown data source type: %i",
						ds->ds[i].type);
				return (-1);
		} /* switch (ds->ds[i].type) */
	} /* for (num_values) */

	/*
	 * Use `memcpy' to write everything to the buffer, because the pointer
	 * may be unaligned and some architectures, such as SPARC, can't handle
	 * that.
	 */
	packet_ptr = *ret_buffer;
	offset = 0;
	memcpy (packet_ptr + offset, &pkg_ph, sizeof (pkg_ph));
	offset += sizeof (pkg_ph);
	memcpy (packet_ptr + offset, &pkg_num_values, sizeof (pkg_num_values));
	offset += sizeof (pkg_num_values);
	memcpy (packet_ptr + offset, pkg_values_types, num_values * sizeof (uint8_t));
	offset += num_values * sizeof (uint8_t);
	memcpy (packet_ptr + offset, pkg_values, num_values * sizeof (value_t));
	offset += num_values * sizeof (value_t);

	assert (offset == packet_len);

	*ret_buffer = packet_ptr + packet_len;
	*ret_buffer_len -= packet_len;

	free (pkg_values_types);
	free (pkg_values);

	return (0);
} /* int write_part_values */