static void streamFile(char *filename) {
    xmlTextReaderPtr reader;
    int ret;
#ifdef HAVE_MMAP
    int fd = -1;
    struct stat info;
    const char *base = NULL;
    xmlParserInputBufferPtr input = NULL;

    if (memory) {
	if (stat(filename, &info) < 0)
	    return;
	if ((fd = open(filename, O_RDONLY)) < 0)
	    return;
	base = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, fd, 0) ;
	if (base == (void *) MAP_FAILED) {
	    close(fd);
	    fprintf(stderr, "mmap failure for file %s\n", filename);
	    progresult = XMLLINT_ERR_RDFILE;
	    return;
	}

	reader = xmlReaderForMemory(base, info.st_size, filename,
	                            NULL, options);
    } else
#endif
	reader = xmlReaderForFile(filename, NULL, options);
#ifdef LIBXML_PATTERN_ENABLED
    if (pattern != NULL) {
        patternc = xmlPatterncompile((const xmlChar *) pattern, NULL, 0, NULL);
	if (patternc == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "Pattern %s failed to compile\n", pattern);
            progresult = XMLLINT_ERR_SCHEMAPAT;
	    pattern = NULL;
	}
    }
    if (patternc != NULL) {
        patstream = xmlPatternGetStreamCtxt(patternc);
	if (patstream != NULL) {
	    ret = xmlStreamPush(patstream, NULL, NULL);
	    if (ret < 0) {
		fprintf(stderr, "xmlStreamPush() failure\n");
		xmlFreeStreamCtxt(patstream);
		patstream = NULL;
            }
	}
    }
#endif


    if (reader != NULL) {
#ifdef LIBXML_VALID_ENABLED
	if (valid)
	    xmlTextReaderSetParserProp(reader, XML_PARSER_VALIDATE, 1);
	else
#endif /* LIBXML_VALID_ENABLED */
	    if (loaddtd)
		xmlTextReaderSetParserProp(reader, XML_PARSER_LOADDTD, 1);
#ifdef LIBXML_SCHEMAS_ENABLED
	if (relaxng != NULL) {
	    if ((timing) && (!repeat)) {
		startTimer();
	    }
	    ret = xmlTextReaderRelaxNGValidate(reader, relaxng);
	    if (ret < 0) {
		xmlGenericError(xmlGenericErrorContext,
			"Relax-NG schema %s failed to compile\n", relaxng);
		progresult = XMLLINT_ERR_SCHEMACOMP;
		relaxng = NULL;
	    }
	    if ((timing) && (!repeat)) {
		endTimer("Compiling the schemas");
	    }
	}
	if (schema != NULL) {
	    if ((timing) && (!repeat)) {
		startTimer();
	    }
	    ret = xmlTextReaderSchemaValidate(reader, schema);
	    if (ret < 0) {
		xmlGenericError(xmlGenericErrorContext,
			"XSD schema %s failed to compile\n", schema);
		progresult = XMLLINT_ERR_SCHEMACOMP;
		schema = NULL;
	    }
	    if ((timing) && (!repeat)) {
		endTimer("Compiling the schemas");
	    }
	}
#endif

	/*
	 * Process all nodes in sequence
	 */
	if ((timing) && (!repeat)) {
	    startTimer();
	}
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
	    if ((debug)
#ifdef LIBXML_PATTERN_ENABLED
	        || (patternc)
#endif
	       )
		processNode(reader);
	    ret = xmlTextReaderRead(reader);
	}
	if ((timing) && (!repeat)) {
#ifdef LIBXML_SCHEMAS_ENABLED
	    if (relaxng != NULL)
		endTimer("Parsing and validating");
	    else
#endif
#ifdef LIBXML_VALID_ENABLED
	    if (valid)
		endTimer("Parsing and validating");
	    else
#endif
	    endTimer("Parsing");
	}

#ifdef LIBXML_VALID_ENABLED
	if (valid) {
	    if (xmlTextReaderIsValid(reader) != 1) {
		xmlGenericError(xmlGenericErrorContext,
			"Document %s does not validate\n", filename);
		progresult = XMLLINT_ERR_VALID;
	    }
	}
#endif /* LIBXML_VALID_ENABLED */
#ifdef LIBXML_SCHEMAS_ENABLED
	if ((relaxng != NULL) || (schema != NULL)) {
	    if (xmlTextReaderIsValid(reader) != 1) {
		fprintf(stderr, "%s fails to validate\n", filename);
		progresult = XMLLINT_ERR_VALID;
	    } else {
	        if (!quiet) {
	            fprintf(stderr, "%s validates\n", filename);
	        }
	    }
	}
#endif
	/*
	 * Done, cleanup and status
	 */
	xmlFreeTextReader(reader);
	if (ret != 0) {
	    fprintf(stderr, "%s : failed to parse\n", filename);
	    progresult = XMLLINT_ERR_UNCLASS;
	}
    } else {
	fprintf(stderr, "Unable to open %s\n", filename);
	progresult = XMLLINT_ERR_UNCLASS;
    }
#ifdef LIBXML_PATTERN_ENABLED
    if (patstream != NULL) {
	xmlFreeStreamCtxt(patstream);
	patstream = NULL;
    }
#endif
#ifdef HAVE_MMAP
    if (memory) {
        xmlFreeParserInputBuffer(input);
	munmap((char *) base, info.st_size);
	close(fd);
    }
#endif
}