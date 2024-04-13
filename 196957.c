xmllintExternalEntityLoader(const char *URL, const char *ID,
			     xmlParserCtxtPtr ctxt) {
    xmlParserInputPtr ret;
    warningSAXFunc warning = NULL;
    errorSAXFunc err = NULL;

    int i;
    const char *lastsegment = URL;
    const char *iter = URL;

    if ((nbpaths > 0) && (iter != NULL)) {
	while (*iter != 0) {
	    if (*iter == '/')
		lastsegment = iter + 1;
	    iter++;
	}
    }

    if ((ctxt != NULL) && (ctxt->sax != NULL)) {
	warning = ctxt->sax->warning;
	err = ctxt->sax->error;
	ctxt->sax->warning = NULL;
	ctxt->sax->error = NULL;
    }

    if (defaultEntityLoader != NULL) {
	ret = defaultEntityLoader(URL, ID, ctxt);
	if (ret != NULL) {
	    if (warning != NULL)
		ctxt->sax->warning = warning;
	    if (err != NULL)
		ctxt->sax->error = err;
	    if (load_trace) {
		fprintf \
			(stderr,
			 "Loaded URL=\"%s\" ID=\"%s\"\n",
			 URL ? URL : "(null)",
			 ID ? ID : "(null)");
	    }
	    return(ret);
	}
    }
    for (i = 0;i < nbpaths;i++) {
	xmlChar *newURL;

	newURL = xmlStrdup((const xmlChar *) paths[i]);
	newURL = xmlStrcat(newURL, (const xmlChar *) "/");
	newURL = xmlStrcat(newURL, (const xmlChar *) lastsegment);
	if (newURL != NULL) {
	    ret = defaultEntityLoader((const char *)newURL, ID, ctxt);
	    if (ret != NULL) {
		if (warning != NULL)
		    ctxt->sax->warning = warning;
		if (err != NULL)
		    ctxt->sax->error = err;
		if (load_trace) {
		    fprintf \
			(stderr,
			 "Loaded URL=\"%s\" ID=\"%s\"\n",
			 newURL,
			 ID ? ID : "(null)");
		}
		xmlFree(newURL);
		return(ret);
	    }
	    xmlFree(newURL);
	}
    }
    if (err != NULL)
        ctxt->sax->error = err;
    if (warning != NULL) {
	ctxt->sax->warning = warning;
	if (URL != NULL)
	    warning(ctxt, "failed to load external entity \"%s\"\n", URL);
	else if (ID != NULL)
	    warning(ctxt, "failed to load external entity \"%s\"\n", ID);
    }
    return(NULL);
}