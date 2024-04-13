static rsRetVal setMaxMsgSize(void __attribute__((unused)) *pVal, long iNewVal)
{
	return glbl.SetMaxLine(iNewVal);
}