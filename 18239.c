static void free_attr_value(RzBinDwarfAttrValue *val) {
	// TODO adjust to new forms, now we're leaking
	if (!val) {
		return;
	}
	switch (val->attr_form) {
	case DW_FORM_strp:
	case DW_FORM_string:
		RZ_FREE(val->string.content);
		break;
	case DW_FORM_exprloc:
	case DW_FORM_block:
	case DW_FORM_block1:
	case DW_FORM_block2:
	case DW_FORM_block4:
		RZ_FREE(val->block.data);
		break;
	default:
		break;
	};
}