xmlExpCheckCard(xmlExpNodePtr exp, xmlExpNodePtr sub) {
    int ret = 1;

    if (sub->c_max == -1) {
        if (exp->c_max != -1)
	    ret = 0;
    } else if ((exp->c_max >= 0) && (exp->c_max < sub->c_max)) {
        ret = 0;
    }
#if 0
    if ((IS_NILLABLE(sub)) && (!IS_NILLABLE(exp)))
        ret = 0;
#endif
    return(ret);
}