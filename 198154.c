xmlExpRef(xmlExpNodePtr exp) {
    if (exp != NULL)
        exp->ref++;
}