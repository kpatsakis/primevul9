 */
double
xmlXPathStringEvalNumber(const xmlChar *str) {
    const xmlChar *cur = str;
    double ret;
    int ok = 0;
    int isneg = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif
    if (cur == NULL) return(0);
    while (IS_BLANK_CH(*cur)) cur++;
    if ((*cur != '.') && ((*cur < '0') || (*cur > '9')) && (*cur != '-')) {
        return(NAN);
    }
    if (*cur == '-') {
	isneg = 1;
	cur++;
    }

#ifdef __GNUC__
    /*
     * tmp/temp is a workaround against a gcc compiler bug
     * http://veillard.com/gcc.bug
     */
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10;
	tmp = (*cur - '0');
	ok = 1;
	cur++;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	ok = 1;
	cur++;
    }
#endif

    if (*cur == '.') {
	int v, frac = 0, max;
	double fraction = 0;

        cur++;
	if (((*cur < '0') || (*cur > '9')) && (!ok)) {
	    return(NAN);
	}
        while (*cur == '0') {
	    frac = frac + 1;
	    cur++;
        }
        max = frac + MAX_FRAC;
	while (((*cur >= '0') && (*cur <= '9')) && (frac < max)) {
	    v = (*cur - '0');
	    fraction = fraction * 10 + v;
	    frac = frac + 1;
	    cur++;
	}
	fraction /= pow(10.0, frac);
	ret = ret + fraction;
	while ((*cur >= '0') && (*cur <= '9'))
	    cur++;
    }
    if ((*cur == 'e') || (*cur == 'E')) {
      cur++;
      if (*cur == '-') {
	is_exponent_negative = 1;
	cur++;
      } else if (*cur == '+') {
        cur++;
      }
      while ((*cur >= '0') && (*cur <= '9')) {
        if (exponent < 1000000)
	  exponent = exponent * 10 + (*cur - '0');
	cur++;
      }
    }
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur != 0) return(NAN);
    if (isneg) ret = -ret;
    if (is_exponent_negative) exponent = -exponent;
    ret *= pow(10.0, (double)exponent);