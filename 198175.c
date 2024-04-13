static void testerr(xmlRegExecCtxtPtr exec) {
    const xmlChar *string;
    xmlChar *values[5];
    int nb = 5;
    int nbneg;
    int terminal;
    xmlRegExecErrInfo(exec, &string, &nb, &nbneg, &values[0], &terminal);
}