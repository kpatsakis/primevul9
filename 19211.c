static bool isValidSymbol(RBinSymbol *symbol) {
	if (symbol && symbol->type) {
		const char *type = symbol->type;
		return (symbol->paddr != UT64_MAX) && (!strcmp (type, R_BIN_TYPE_FUNC_STR) || !strcmp (type, R_BIN_TYPE_HIOS_STR) || !strcmp (type, R_BIN_TYPE_LOOS_STR) || !strcmp (type, R_BIN_TYPE_METH_STR) || !strcmp (type , R_BIN_TYPE_STATIC_STR));
	}
	return false;
}