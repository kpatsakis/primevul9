static inline RzBinDwarfLocList *create_loc_list(ut64 offset) {
	RzBinDwarfLocList *list = RZ_NEW0(RzBinDwarfLocList);
	if (list) {
		list->list = rz_list_new();
		list->offset = offset;
	}
	return list;
}