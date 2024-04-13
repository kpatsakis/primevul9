static int init_abbrev_decl(RzBinDwarfAbbrevDecl *ad) {
	if (!ad) {
		return -EINVAL;
	}
	ad->defs = calloc(sizeof(RzBinDwarfAttrDef), ABBREV_DECL_CAP);

	if (!ad->defs) {
		return -ENOMEM;
	}

	ad->capacity = ABBREV_DECL_CAP;
	ad->count = 0;

	return 0;
}