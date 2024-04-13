static void rfx_update_context_properties(RFX_CONTEXT* context)
{
	UINT16 properties;
	/* properties in tilesets: note that this has different format from the one in TS_RFX_CONTEXT */
	properties = 1;                          /* lt */
	properties |= (context->flags << 1);     /* flags */
	properties |= (COL_CONV_ICT << 4);       /* cct */
	properties |= (CLW_XFORM_DWT_53_A << 6); /* xft */
	properties |= ((context->mode == RLGR1 ? CLW_ENTROPY_RLGR1 : CLW_ENTROPY_RLGR3) << 10); /* et */
	properties |= (SCALAR_QUANTIZATION << 14);                                              /* qt */
	context->properties = properties;
}