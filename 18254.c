static size_t std_opcode_args_count(const RzBinDwarfLineHeader *hdr, ut8 opcode) {
	if (!opcode || opcode > hdr->opcode_base - 1 || !hdr->std_opcode_lengths) {
		return 0;
	}
	return hdr->std_opcode_lengths[opcode - 1];
}