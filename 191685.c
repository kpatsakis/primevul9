static int parse_part_values (void **ret_buffer, size_t *ret_buffer_len,
		value_t **ret_values, int *ret_num_values)
{
	char *buffer = *ret_buffer;
	size_t buffer_len = *ret_buffer_len;

	uint16_t tmp16;
	size_t exp_size;
	int   i;

	uint16_t pkg_length;
	uint16_t pkg_type;
	uint16_t pkg_numval;

	uint8_t *pkg_types;
	value_t *pkg_values;

	if (buffer_len < 15)
	{
		NOTICE ("network plugin: packet is too short: "
				"buffer_len = %zu", buffer_len);
		return (-1);
	}

	memcpy ((void *) &tmp16, buffer, sizeof (tmp16));
	buffer += sizeof (tmp16);
	pkg_type = ntohs (tmp16);

	memcpy ((void *) &tmp16, buffer, sizeof (tmp16));
	buffer += sizeof (tmp16);
	pkg_length = ntohs (tmp16);

	memcpy ((void *) &tmp16, buffer, sizeof (tmp16));
	buffer += sizeof (tmp16);
	pkg_numval = ntohs (tmp16);

	assert (pkg_type == TYPE_VALUES);

	exp_size = 3 * sizeof (uint16_t)
		+ pkg_numval * (sizeof (uint8_t) + sizeof (value_t));
	if (buffer_len < exp_size)
	{
		WARNING ("network plugin: parse_part_values: "
				"Packet too short: "
				"Chunk of size %zu expected, "
				"but buffer has only %zu bytes left.",
				exp_size, buffer_len);
		return (-1);
	}

	if (pkg_length != exp_size)
	{
		WARNING ("network plugin: parse_part_values: "
				"Length and number of values "
				"in the packet don't match.");
		return (-1);
	}

	pkg_types = (uint8_t *) malloc (pkg_numval * sizeof (uint8_t));
	pkg_values = (value_t *) malloc (pkg_numval * sizeof (value_t));
	if ((pkg_types == NULL) || (pkg_values == NULL))
	{
		sfree (pkg_types);
		sfree (pkg_values);
		ERROR ("network plugin: parse_part_values: malloc failed.");
		return (-1);
	}

	memcpy ((void *) pkg_types, (void *) buffer, pkg_numval * sizeof (uint8_t));
	buffer += pkg_numval * sizeof (uint8_t);
	memcpy ((void *) pkg_values, (void *) buffer, pkg_numval * sizeof (value_t));
	buffer += pkg_numval * sizeof (value_t);

	for (i = 0; i < pkg_numval; i++)
	{
		switch (pkg_types[i])
		{
		  case DS_TYPE_COUNTER:
		    pkg_values[i].counter = (counter_t) ntohll (pkg_values[i].counter);
		    break;

		  case DS_TYPE_GAUGE:
		    pkg_values[i].gauge = (gauge_t) ntohd (pkg_values[i].gauge);
		    break;

		  case DS_TYPE_DERIVE:
		    pkg_values[i].derive = (derive_t) ntohll (pkg_values[i].derive);
		    break;

		  case DS_TYPE_ABSOLUTE:
		    pkg_values[i].absolute = (absolute_t) ntohll (pkg_values[i].absolute);
		    break;

		  default:
		    NOTICE ("network plugin: parse_part_values: "
			"Don't know how to handle data source type %"PRIu8,
			pkg_types[i]);
		    sfree (pkg_types);
		    sfree (pkg_values);
		    return (-1);
		} /* switch (pkg_types[i]) */
	}

	*ret_buffer     = buffer;
	*ret_buffer_len = buffer_len - pkg_length;
	*ret_num_values = pkg_numval;
	*ret_values     = pkg_values;

	sfree (pkg_types);

	return (0);
} /* int parse_part_values */