static const char *unittest_path(int nr, enum overlay_type ovtype)
{
	const char *base;
	static char buf[256];

	switch (ovtype) {
	case PDEV_OVERLAY:
		base = "/testcase-data/overlay-node/test-bus";
		break;
	case I2C_OVERLAY:
		base = "/testcase-data/overlay-node/test-bus/i2c-test-bus";
		break;
	default:
		buf[0] = '\0';
		return buf;
	}
	snprintf(buf, sizeof(buf) - 1, "%s/test-unittest%d", base, nr);
	buf[sizeof(buf) - 1] = '\0';
	return buf;
}