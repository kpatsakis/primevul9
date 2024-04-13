int32_t cli_bcapi_atoi(struct cli_bc_ctx *ctx, const uint8_t* str, int32_t len)
{
    int32_t number = 0;
    const uint8_t *end = str + len;
    while (isspace(*str) && str < end) str++;
    if (str == end)
	return -1;/* all spaces */
    if (*str == '+') str++;
    if (str == end)
	return -1;/* all spaces and +*/
    if (*str == '-')
	return -1;/* only positive numbers */
    if (!isdigit(*str))
	return -1;
    while (isdigit(*str) && str < end) {
	number = number*10 + (*str - '0');
    }
    return number;
}