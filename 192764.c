static int hclge_shaper_para_calc(u32 ir, u8 shaper_level,
				  u8 *ir_b, u8 *ir_u, u8 *ir_s)
{
#define DIVISOR_CLK		(1000 * 8)
#define DIVISOR_IR_B_126	(126 * DIVISOR_CLK)

	const u16 tick_array[HCLGE_SHAPER_LVL_CNT] = {
		6 * 256,        /* Prioriy level */
		6 * 32,         /* Prioriy group level */
		6 * 8,          /* Port level */
		6 * 256         /* Qset level */
	};
	u8 ir_u_calc = 0;
	u8 ir_s_calc = 0;
	u32 ir_calc;
	u32 tick;

	/* Calc tick */
	if (shaper_level >= HCLGE_SHAPER_LVL_CNT ||
	    ir > HCLGE_ETHER_MAX_RATE)
		return -EINVAL;

	tick = tick_array[shaper_level];

	/**
	 * Calc the speed if ir_b = 126, ir_u = 0 and ir_s = 0
	 * the formula is changed to:
	 *		126 * 1 * 8
	 * ir_calc = ---------------- * 1000
	 *		tick * 1
	 */
	ir_calc = (DIVISOR_IR_B_126 + (tick >> 1) - 1) / tick;

	if (ir_calc == ir) {
		*ir_b = 126;
		*ir_u = 0;
		*ir_s = 0;

		return 0;
	} else if (ir_calc > ir) {
		/* Increasing the denominator to select ir_s value */
		while (ir_calc > ir) {
			ir_s_calc++;
			ir_calc = DIVISOR_IR_B_126 / (tick * (1 << ir_s_calc));
		}

		if (ir_calc == ir)
			*ir_b = 126;
		else
			*ir_b = (ir * tick * (1 << ir_s_calc) +
				 (DIVISOR_CLK >> 1)) / DIVISOR_CLK;
	} else {
		/* Increasing the numerator to select ir_u value */
		u32 numerator;

		while (ir_calc < ir) {
			ir_u_calc++;
			numerator = DIVISOR_IR_B_126 * (1 << ir_u_calc);
			ir_calc = (numerator + (tick >> 1)) / tick;
		}

		if (ir_calc == ir) {
			*ir_b = 126;
		} else {
			u32 denominator = (DIVISOR_CLK * (1 << --ir_u_calc));
			*ir_b = (ir * tick + (denominator >> 1)) / denominator;
		}
	}

	*ir_u = ir_u_calc;
	*ir_s = ir_s_calc;

	return 0;
}