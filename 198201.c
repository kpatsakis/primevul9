xmlExpMaxToken(xmlExpNodePtr expr) {
    if (expr == NULL)
        return(-1);
    return(expr->c_max);
}