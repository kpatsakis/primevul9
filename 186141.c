static void __init of_unittest_printf_one(struct device_node *np, const char *fmt,
					  const char *expected)
{
	unsigned char *buf;
	int buf_size;
	int size, i;

	buf_size = strlen(expected) + 10;
	buf = kmalloc(buf_size, GFP_KERNEL);
	if (!buf)
		return;

	/* Baseline; check conversion with a large size limit */
	memset(buf, 0xff, buf_size);
	size = snprintf(buf, buf_size - 2, fmt, np);

	/* use strcmp() instead of strncmp() here to be absolutely sure strings match */
	unittest((strcmp(buf, expected) == 0) && (buf[size+1] == 0xff),
		"sprintf failed; fmt='%s' expected='%s' rslt='%s'\n",
		fmt, expected, buf);

	/* Make sure length limits work */
	size++;
	for (i = 0; i < 2; i++, size--) {
		/* Clear the buffer, and make sure it works correctly still */
		memset(buf, 0xff, buf_size);
		snprintf(buf, size+1, fmt, np);
		unittest(strncmp(buf, expected, size) == 0 && (buf[size+1] == 0xff),
			"snprintf failed; size=%i fmt='%s' expected='%s' rslt='%s'\n",
			size, fmt, expected, buf);
	}
	kfree(buf);
}