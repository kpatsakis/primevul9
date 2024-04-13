toggle_dac_capability(int writable, int enable)
{
	unsigned int capability = writable ? CAP_DAC_OVERRIDE : CAP_DAC_READ_SEARCH;

	if (capng_update(enable ? CAPNG_ADD : CAPNG_DROP, CAPNG_EFFECTIVE, capability)) {
		fprintf(stderr, "Unable to update capability set.\n");
		return EX_SYSERR;
	}
	if (capng_apply(CAPNG_SELECT_CAPS)) {
		fprintf(stderr, "Unable to apply new capability set.\n");
		return EX_SYSERR;
	}
	return 0;
}