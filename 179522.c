guint64_to_str_buf_len(const guint64 u)
{
	/* ((2^64)-1) == 18446744073709551615 */

	if (u >= G_GUINT64_CONSTANT(10000000000000000000)) return 20;
	if (u >= G_GUINT64_CONSTANT(1000000000000000000))  return 19;
	if (u >= G_GUINT64_CONSTANT(100000000000000000))   return 18;
	if (u >= G_GUINT64_CONSTANT(10000000000000000))    return 17;
	if (u >= G_GUINT64_CONSTANT(1000000000000000))     return 16;
	if (u >= G_GUINT64_CONSTANT(100000000000000))      return 15;
	if (u >= G_GUINT64_CONSTANT(10000000000000))       return 14;
	if (u >= G_GUINT64_CONSTANT(1000000000000))        return 13;
	if (u >= G_GUINT64_CONSTANT(100000000000))         return 12;
	if (u >= G_GUINT64_CONSTANT(10000000000))          return 11;
	if (u >= G_GUINT64_CONSTANT(1000000000))           return 10;
	if (u >= G_GUINT64_CONSTANT(100000000))            return 9;
	if (u >= G_GUINT64_CONSTANT(10000000))             return 8;
	if (u >= G_GUINT64_CONSTANT(1000000))              return 7;
	if (u >= G_GUINT64_CONSTANT(100000))               return 6;
	if (u >= G_GUINT64_CONSTANT(10000))                return 5;
	if (u >= G_GUINT64_CONSTANT(1000))                 return 4;
	if (u >= G_GUINT64_CONSTANT(100))                  return 3;
	if (u >= G_GUINT64_CONSTANT(10))                   return 2;

	return 1;
}