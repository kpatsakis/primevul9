static bool archIsThumbable(RCore *core) {
	RAsm *as = core ? core->assembler : NULL;
	if (as && as->cur && as->bits <= 32 && as->cur->name) {
		return strstr (as->cur->name, "arm");
	}
	return false;
}