static int init_comp_unit(RzBinDwarfCompUnit *cu) {
	if (!cu) {
		return -EINVAL;
	}
	cu->dies = calloc(sizeof(RzBinDwarfDie), COMP_UNIT_CAPACITY);
	if (!cu->dies) {
		return -ENOMEM;
	}
	cu->capacity = COMP_UNIT_CAPACITY;
	cu->count = 0;
	return 0;
}