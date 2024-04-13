static int expand_cu(RzBinDwarfCompUnit *cu) {
	RzBinDwarfDie *tmp;

	if (!cu || cu->capacity == 0 || cu->capacity != cu->count) {
		return -EINVAL;
	}

	tmp = (RzBinDwarfDie *)realloc(cu->dies,
		cu->capacity * 2 * sizeof(RzBinDwarfDie));
	if (!tmp) {
		return -ENOMEM;
	}

	memset((ut8 *)tmp + cu->capacity * sizeof(RzBinDwarfDie),
		0, cu->capacity * sizeof(RzBinDwarfDie));
	cu->dies = tmp;
	cu->capacity *= 2;

	return 0;
}