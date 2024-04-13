is_valid_assembly_flags (guint32 flags) {
	/* Metadata: 22.1.2 */
	flags &= ~(0x8000 | 0x4000); /* ignore reserved bits 0x0030? */
	return ((flags == 1) || (flags == 0));
}