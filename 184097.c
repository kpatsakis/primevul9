getFIOPName(unsigned iFIOP)
{
	char *pRet;
	switch(iFIOP) {
		case FIOP_CONTAINS:
			pRet = "contains";
			break;
		case FIOP_ISEQUAL:
			pRet = "isequal";
			break;
		case FIOP_STARTSWITH:
			pRet = "startswith";
			break;
		case FIOP_REGEX:
			pRet = "regex";
			break;
		default:
			pRet = "NOP";
			break;
	}
	return pRet;
}