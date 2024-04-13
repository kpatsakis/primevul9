drop_capabilities(int parent)
{
	capng_select_t set = CAPNG_SELECT_CAPS;

	capng_setpid(getpid());
	capng_clear(CAPNG_SELECT_BOTH);
	if (parent) {
		if (capng_updatev(CAPNG_ADD, CAPNG_PERMITTED, CAP_DAC_READ_SEARCH, CAP_DAC_OVERRIDE, -1)) {
			fprintf(stderr, "Unable to update capability set.\n");
			return EX_SYSERR;
		}
		if (capng_update(CAPNG_ADD, CAPNG_PERMITTED|CAPNG_EFFECTIVE, CAP_SYS_ADMIN)) {
			fprintf(stderr, "Unable to update capability set.\n");
			return EX_SYSERR;
		}
	} else {
		if (capng_update(CAPNG_ADD, CAPNG_PERMITTED, CAP_DAC_READ_SEARCH)) {
			fprintf(stderr, "Unable to update capability set.\n");
			return EX_SYSERR;
		}
	}
	if (capng_have_capability(CAPNG_EFFECTIVE, CAP_SETPCAP)) {
		set = CAPNG_SELECT_BOTH;
	}
	if (capng_apply(set)) {
		fprintf(stderr, "Unable to apply new capability set.\n");
		return EX_SYSERR;
	}
	return 0;
}