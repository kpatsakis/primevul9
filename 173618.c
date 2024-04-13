_lou_findOpcodeName(TranslationTableOpcode opcode) {
	static char scratchBuf[MAXSTRING];
	/* Used by tools such as lou_debug */
	if (opcode < 0 || opcode >= CTO_None) {
		sprintf(scratchBuf, "%d", opcode);
		return scratchBuf;
	}
	return opcodeNames[opcode];
}