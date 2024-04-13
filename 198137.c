xmlExpIsNillable(xmlExpNodePtr exp) {
    if (exp == NULL)
        return(-1);
    return(IS_NILLABLE(exp) != 0);
}