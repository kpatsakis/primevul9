const char *jsC_opcodestring(enum js_OpCode opcode)
{
	if (opcode < nelem(opname)-1)
		return opname[opcode];
	return "<unknown>";
}