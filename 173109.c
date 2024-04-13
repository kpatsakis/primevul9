static u32 bnx2x_update_mng_version_utility(u8 *version, bool bnx2x_format)
{
	u8 vals[4];
	int i = 0;

	if (bnx2x_format) {
		i = sscanf(version, "1.%c%hhd.%hhd.%hhd",
			   &vals[0], &vals[1], &vals[2], &vals[3]);
		if (i > 0)
			vals[0] -= '0';
	} else {
		i = sscanf(version, "%hhd.%hhd.%hhd.%hhd",
			   &vals[0], &vals[1], &vals[2], &vals[3]);
	}

	while (i < 4)
		vals[i++] = 0;

	return (vals[0] << 24) | (vals[1] << 16) | (vals[2] << 8) | vals[3];
}