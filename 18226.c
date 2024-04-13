static int expand_abbrev_decl(RzBinDwarfAbbrevDecl *ad) {
	RzBinDwarfAttrDef *tmp;

	if (!ad || !ad->capacity || ad->capacity != ad->count) {
		return -EINVAL;
	}

	tmp = (RzBinDwarfAttrDef *)realloc(ad->defs,
		ad->capacity * 2 * sizeof(RzBinDwarfAttrDef));

	if (!tmp) {
		return -ENOMEM;
	}

	// Set the area in the buffer past the length to 0
	memset((ut8 *)tmp + ad->capacity * sizeof(RzBinDwarfAttrDef),
		0, ad->capacity * sizeof(RzBinDwarfAttrDef));
	ad->defs = tmp;
	ad->capacity *= 2;

	return 0;
}