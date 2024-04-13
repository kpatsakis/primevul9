static int clrspctojp2(jas_clrspc_t clrspc)
{
	switch (clrspc) {
	case JAS_CLRSPC_SRGB:
		return JP2_COLR_SRGB;
	case JAS_CLRSPC_SYCBCR:
		return JP2_COLR_SYCC;
	case JAS_CLRSPC_SGRAY:
		return JP2_COLR_SGRAY;
	default:
		abort();
		break;
	}
}