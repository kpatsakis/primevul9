static void bnx2x_init_internal(struct bnx2x *bp, u32 load_code)
{
	switch (load_code) {
	case FW_MSG_CODE_DRV_LOAD_COMMON:
	case FW_MSG_CODE_DRV_LOAD_COMMON_CHIP:
		bnx2x_init_internal_common(bp);
		/* no break */

	case FW_MSG_CODE_DRV_LOAD_PORT:
		/* nothing to do */
		/* no break */

	case FW_MSG_CODE_DRV_LOAD_FUNCTION:
		/* internal memory per function is
		   initialized inside bnx2x_pf_init */
		break;

	default:
		BNX2X_ERR("Unknown load_code (0x%x) from MCP\n", load_code);
		break;
	}
}