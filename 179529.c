guint32_to_str_buf_len(const guint32 u)
{
	/* ((2^32)-1) == 2147483647 */
	if (u >= 1000000000)return 10;
	if (u >= 100000000) return 9;
	if (u >= 10000000)  return 8;
	if (u >= 1000000)   return 7;
	if (u >= 100000)    return 6;
	if (u >= 10000)     return 5;
	if (u >= 1000)      return 4;
	if (u >= 100)       return 3;
	if (u >= 10)        return 2;

	return 1;
}