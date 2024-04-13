static void bnx2x_set_modes_bitmap(struct bnx2x *bp)
{
	u32 flags = 0;

	if (CHIP_REV_IS_FPGA(bp))
		SET_FLAGS(flags, MODE_FPGA);
	else if (CHIP_REV_IS_EMUL(bp))
		SET_FLAGS(flags, MODE_EMUL);
	else
		SET_FLAGS(flags, MODE_ASIC);

	if (CHIP_MODE_IS_4_PORT(bp))
		SET_FLAGS(flags, MODE_PORT4);
	else
		SET_FLAGS(flags, MODE_PORT2);

	if (CHIP_IS_E2(bp))
		SET_FLAGS(flags, MODE_E2);
	else if (CHIP_IS_E3(bp)) {
		SET_FLAGS(flags, MODE_E3);
		if (CHIP_REV(bp) == CHIP_REV_Ax)
			SET_FLAGS(flags, MODE_E3_A0);
		else /*if (CHIP_REV(bp) == CHIP_REV_Bx)*/
			SET_FLAGS(flags, MODE_E3_B0 | MODE_COS3);
	}

	if (IS_MF(bp)) {
		SET_FLAGS(flags, MODE_MF);
		switch (bp->mf_mode) {
		case MULTI_FUNCTION_SD:
			SET_FLAGS(flags, MODE_MF_SD);
			break;
		case MULTI_FUNCTION_SI:
			SET_FLAGS(flags, MODE_MF_SI);
			break;
		case MULTI_FUNCTION_AFEX:
			SET_FLAGS(flags, MODE_MF_AFEX);
			break;
		}
	} else
		SET_FLAGS(flags, MODE_SF);

#if defined(__LITTLE_ENDIAN)
	SET_FLAGS(flags, MODE_LITTLE_ENDIAN);
#else /*(__BIG_ENDIAN)*/
	SET_FLAGS(flags, MODE_BIG_ENDIAN);
#endif
	INIT_MODE_FLAGS(bp) = flags;
}