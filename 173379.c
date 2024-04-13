static u32 bnx2x_get_pretend_reg(struct bnx2x *bp)
{
	u32 base = PXP2_REG_PGL_PRETEND_FUNC_F0;
	u32 stride = PXP2_REG_PGL_PRETEND_FUNC_F1 - base;
	return base + (BP_ABS_FUNC(bp)) * stride;
}