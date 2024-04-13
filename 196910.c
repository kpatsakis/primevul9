static void doXPathDump(xmlXPathObjectPtr cur) {
    switch(cur->type) {
        case XPATH_NODESET: {
            int i;
            xmlNodePtr node;
#ifdef LIBXML_OUTPUT_ENABLED
            xmlOutputBufferPtr buf;

            if ((cur->nodesetval == NULL) || (cur->nodesetval->nodeNr <= 0)) {
                fprintf(stderr, "XPath set is empty\n");
                progresult = XMLLINT_ERR_XPATH;
                break;
            }
            buf = xmlOutputBufferCreateFile(stdout, NULL);
            if (buf == NULL) {
                fprintf(stderr, "Out of memory for XPath\n");
                progresult = XMLLINT_ERR_MEM;
                return;
            }
            for (i = 0;i < cur->nodesetval->nodeNr;i++) {
                node = cur->nodesetval->nodeTab[i];
                xmlNodeDumpOutput(buf, NULL, node, 0, 0, NULL);
                xmlOutputBufferWrite(buf, 1, "\n");
            }
            xmlOutputBufferClose(buf);
#else
            printf("xpath returned %d nodes\n", cur->nodesetval->nodeNr);
#endif
	    break;
        }
        case XPATH_BOOLEAN:
	    if (cur->boolval) printf("true\n");
	    else printf("false\n");
	    break;
        case XPATH_NUMBER:
	    switch (xmlXPathIsInf(cur->floatval)) {
	    case 1:
		printf("Infinity\n");
		break;
	    case -1:
		printf("-Infinity\n");
		break;
	    default:
		if (xmlXPathIsNaN(cur->floatval)) {
		    printf("NaN\n");
		} else {
		    printf("%0g\n", cur->floatval);
		}
	    }
	    break;
        case XPATH_STRING:
	    printf("%s\n", (const char *) cur->stringval);
	    break;
        case XPATH_UNDEFINED:
	    fprintf(stderr, "XPath Object is uninitialized\n");
            progresult = XMLLINT_ERR_XPATH;
	    break;
	default:
	    fprintf(stderr, "XPath object of unexpected type\n");
            progresult = XMLLINT_ERR_XPATH;
	    break;
    }
}