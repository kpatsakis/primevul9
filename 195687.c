int32_t cli_bcapi_version_compare(struct cli_bc_ctx *ctx , const uint8_t* lhs, uint32_t lhs_len, 
				  const uint8_t* rhs, uint32_t rhs_len)
{
    unsigned i = 0, j = 0;
    unsigned long li=0, ri=0;
    do {
	while (i < lhs_len && j < rhs_len && lhs[i] == rhs[j] &&
	       !isdigit(lhs[i]) && !isdigit(rhs[j])) {
	    i++; j++;
	}
	if (i == lhs_len && j == rhs_len)
	    return 0;
	if (i == lhs_len)
	    return -1;
	if (j == rhs_len)
	    return 1;
	if (!isdigit(lhs[i]) || !isdigit(rhs[j]))
	    return lhs[i] < rhs[j] ? -1 : 1;
	while (isdigit(lhs[i]) && i < lhs_len)
	    li = 10*li + (lhs[i++] - '0');
	while (isdigit(rhs[j]) && j < rhs_len)
	    ri = 10*ri + (rhs[j++] - '0');
	if (li < ri)
	    return -1;
	if (li > ri)
	    return 1;
    } while (1);
}