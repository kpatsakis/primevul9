main(int argc, char **argv) {
    int i, acount;
    int files = 0;
    int version = 0;
    const char* indent;

    if (argc <= 1) {
	usage(stderr, argv[0]);
	return(1);
    }
    LIBXML_TEST_VERSION
    for (i = 1; i < argc ; i++) {
	if (!strcmp(argv[i], "-"))
	    break;

	if (argv[i][0] != '-')
	    continue;
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    debug++;
	else
#ifdef LIBXML_DEBUG_ENABLED
	if ((!strcmp(argv[i], "-shell")) ||
	         (!strcmp(argv[i], "--shell"))) {
	    shell++;
            noout = 1;
        } else
#endif
#ifdef LIBXML_TREE_ENABLED
	if ((!strcmp(argv[i], "-copy")) || (!strcmp(argv[i], "--copy")))
	    copy++;
	else
#endif /* LIBXML_TREE_ENABLED */
	if ((!strcmp(argv[i], "-recover")) ||
	         (!strcmp(argv[i], "--recover"))) {
	    recovery++;
	    options |= XML_PARSE_RECOVER;
	} else if ((!strcmp(argv[i], "-huge")) ||
	         (!strcmp(argv[i], "--huge"))) {
	    options |= XML_PARSE_HUGE;
	} else if ((!strcmp(argv[i], "-noent")) ||
	         (!strcmp(argv[i], "--noent"))) {
	    noent++;
	    options |= XML_PARSE_NOENT;
	} else if ((!strcmp(argv[i], "-noenc")) ||
	         (!strcmp(argv[i], "--noenc"))) {
	    noenc++;
	    options |= XML_PARSE_IGNORE_ENC;
	} else if ((!strcmp(argv[i], "-nsclean")) ||
	         (!strcmp(argv[i], "--nsclean"))) {
	    options |= XML_PARSE_NSCLEAN;
	} else if ((!strcmp(argv[i], "-nocdata")) ||
	         (!strcmp(argv[i], "--nocdata"))) {
	    options |= XML_PARSE_NOCDATA;
	} else if ((!strcmp(argv[i], "-nodict")) ||
	         (!strcmp(argv[i], "--nodict"))) {
	    options |= XML_PARSE_NODICT;
	} else if ((!strcmp(argv[i], "-version")) ||
	         (!strcmp(argv[i], "--version"))) {
	    showVersion(argv[0]);
	    version = 1;
	} else if ((!strcmp(argv[i], "-noout")) ||
	         (!strcmp(argv[i], "--noout")))
	    noout++;
#ifdef LIBXML_OUTPUT_ENABLED
	else if ((!strcmp(argv[i], "-o")) ||
	         (!strcmp(argv[i], "-output")) ||
	         (!strcmp(argv[i], "--output"))) {
	    i++;
	    output = argv[i];
	}
#endif /* LIBXML_OUTPUT_ENABLED */
	else if ((!strcmp(argv[i], "-htmlout")) ||
	         (!strcmp(argv[i], "--htmlout")))
	    htmlout++;
	else if ((!strcmp(argv[i], "-nowrap")) ||
	         (!strcmp(argv[i], "--nowrap")))
	    nowrap++;
#ifdef LIBXML_HTML_ENABLED
	else if ((!strcmp(argv[i], "-html")) ||
	         (!strcmp(argv[i], "--html"))) {
	    html++;
        }
	else if ((!strcmp(argv[i], "-xmlout")) ||
	         (!strcmp(argv[i], "--xmlout"))) {
	    xmlout++;
	} else if ((!strcmp(argv[i], "-nodefdtd")) ||
	         (!strcmp(argv[i], "--nodefdtd"))) {
            nodefdtd++;
	    options |= HTML_PARSE_NODEFDTD;
        }
#endif /* LIBXML_HTML_ENABLED */
	else if ((!strcmp(argv[i], "-loaddtd")) ||
	         (!strcmp(argv[i], "--loaddtd"))) {
	    loaddtd++;
	    options |= XML_PARSE_DTDLOAD;
	} else if ((!strcmp(argv[i], "-dtdattr")) ||
	         (!strcmp(argv[i], "--dtdattr"))) {
	    loaddtd++;
	    dtdattrs++;
	    options |= XML_PARSE_DTDATTR;
	}
#ifdef LIBXML_VALID_ENABLED
	else if ((!strcmp(argv[i], "-valid")) ||
	         (!strcmp(argv[i], "--valid"))) {
	    valid++;
	    options |= XML_PARSE_DTDVALID;
	} else if ((!strcmp(argv[i], "-postvalid")) ||
	         (!strcmp(argv[i], "--postvalid"))) {
	    postvalid++;
	    loaddtd++;
	    options |= XML_PARSE_DTDLOAD;
	} else if ((!strcmp(argv[i], "-dtdvalid")) ||
	         (!strcmp(argv[i], "--dtdvalid"))) {
	    i++;
	    dtdvalid = argv[i];
	    loaddtd++;
	    options |= XML_PARSE_DTDLOAD;
	} else if ((!strcmp(argv[i], "-dtdvalidfpi")) ||
	         (!strcmp(argv[i], "--dtdvalidfpi"))) {
	    i++;
	    dtdvalidfpi = argv[i];
	    loaddtd++;
	    options |= XML_PARSE_DTDLOAD;
        }
#endif /* LIBXML_VALID_ENABLED */
	else if ((!strcmp(argv[i], "-dropdtd")) ||
	         (!strcmp(argv[i], "--dropdtd")))
	    dropdtd++;
	else if ((!strcmp(argv[i], "-insert")) ||
	         (!strcmp(argv[i], "--insert")))
	    insert++;
	else if ((!strcmp(argv[i], "-quiet")) ||
	         (!strcmp(argv[i], "--quiet")))
	    quiet++;
	else if ((!strcmp(argv[i], "-timing")) ||
	         (!strcmp(argv[i], "--timing")))
	    timing++;
	else if ((!strcmp(argv[i], "-auto")) ||
	         (!strcmp(argv[i], "--auto")))
	    generate++;
	else if ((!strcmp(argv[i], "-repeat")) ||
	         (!strcmp(argv[i], "--repeat"))) {
	    if (repeat)
	        repeat *= 10;
	    else
	        repeat = 100;
	}
#ifdef LIBXML_PUSH_ENABLED
	else if ((!strcmp(argv[i], "-push")) ||
	         (!strcmp(argv[i], "--push")))
	    push++;
	else if ((!strcmp(argv[i], "-pushsmall")) ||
	         (!strcmp(argv[i], "--pushsmall"))) {
	    push++;
            pushsize = 10;
        }
#endif /* LIBXML_PUSH_ENABLED */
#ifdef HAVE_MMAP
	else if ((!strcmp(argv[i], "-memory")) ||
	         (!strcmp(argv[i], "--memory")))
	    memory++;
#endif
	else if ((!strcmp(argv[i], "-testIO")) ||
	         (!strcmp(argv[i], "--testIO")))
	    testIO++;
#ifdef LIBXML_XINCLUDE_ENABLED
	else if ((!strcmp(argv[i], "-xinclude")) ||
	         (!strcmp(argv[i], "--xinclude"))) {
	    xinclude++;
	    options |= XML_PARSE_XINCLUDE;
	}
	else if ((!strcmp(argv[i], "-noxincludenode")) ||
	         (!strcmp(argv[i], "--noxincludenode"))) {
	    xinclude++;
	    options |= XML_PARSE_XINCLUDE;
	    options |= XML_PARSE_NOXINCNODE;
	}
	else if ((!strcmp(argv[i], "-nofixup-base-uris")) ||
	         (!strcmp(argv[i], "--nofixup-base-uris"))) {
	    xinclude++;
	    options |= XML_PARSE_XINCLUDE;
	    options |= XML_PARSE_NOBASEFIX;
	}
#endif
#ifdef LIBXML_OUTPUT_ENABLED
#ifdef LIBXML_ZLIB_ENABLED
	else if ((!strcmp(argv[i], "-compress")) ||
	         (!strcmp(argv[i], "--compress"))) {
	    compress++;
	    xmlSetCompressMode(9);
        }
#endif
#endif /* LIBXML_OUTPUT_ENABLED */
	else if ((!strcmp(argv[i], "-nowarning")) ||
	         (!strcmp(argv[i], "--nowarning"))) {
	    xmlGetWarningsDefaultValue = 0;
	    xmlPedanticParserDefault(0);
	    options |= XML_PARSE_NOWARNING;
        }
	else if ((!strcmp(argv[i], "-pedantic")) ||
	         (!strcmp(argv[i], "--pedantic"))) {
	    xmlGetWarningsDefaultValue = 1;
	    xmlPedanticParserDefault(1);
	    options |= XML_PARSE_PEDANTIC;
        }
#ifdef LIBXML_DEBUG_ENABLED
	else if ((!strcmp(argv[i], "-debugent")) ||
		 (!strcmp(argv[i], "--debugent"))) {
	    debugent++;
	    xmlParserDebugEntities = 1;
	}
#endif
#ifdef LIBXML_C14N_ENABLED
	else if ((!strcmp(argv[i], "-c14n")) ||
		 (!strcmp(argv[i], "--c14n"))) {
	    canonical++;
	    options |= XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_DTDLOAD;
	}
	else if ((!strcmp(argv[i], "-c14n11")) ||
		 (!strcmp(argv[i], "--c14n11"))) {
	    canonical_11++;
	    options |= XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_DTDLOAD;
	}
	else if ((!strcmp(argv[i], "-exc-c14n")) ||
		 (!strcmp(argv[i], "--exc-c14n"))) {
	    exc_canonical++;
	    options |= XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_DTDLOAD;
	}
#endif
#ifdef LIBXML_CATALOG_ENABLED
	else if ((!strcmp(argv[i], "-catalogs")) ||
		 (!strcmp(argv[i], "--catalogs"))) {
	    catalogs++;
	} else if ((!strcmp(argv[i], "-nocatalogs")) ||
		 (!strcmp(argv[i], "--nocatalogs"))) {
	    nocatalogs++;
	}
#endif
	else if ((!strcmp(argv[i], "-encode")) ||
	         (!strcmp(argv[i], "--encode"))) {
	    i++;
	    encoding = argv[i];
	    /*
	     * OK it's for testing purposes
	     */
	    xmlAddEncodingAlias("UTF-8", "DVEnc");
        }
	else if ((!strcmp(argv[i], "-noblanks")) ||
	         (!strcmp(argv[i], "--noblanks"))) {
	    noblanks++;
	    xmlKeepBlanksDefault(0);
	    options |= XML_PARSE_NOBLANKS;
        }
	else if ((!strcmp(argv[i], "-maxmem")) ||
	         (!strcmp(argv[i], "--maxmem"))) {
	     i++;
	     if (sscanf(argv[i], "%d", &maxmem) == 1) {
	         xmlMemSetup(myFreeFunc, myMallocFunc, myReallocFunc,
		             myStrdupFunc);
	     } else {
	         maxmem = 0;
	     }
        }
	else if ((!strcmp(argv[i], "-format")) ||
	         (!strcmp(argv[i], "--format"))) {
	     noblanks++;
#ifdef LIBXML_OUTPUT_ENABLED
	     format = 1;
#endif /* LIBXML_OUTPUT_ENABLED */
	     xmlKeepBlanksDefault(0);
	}
	else if ((!strcmp(argv[i], "-pretty")) ||
	         (!strcmp(argv[i], "--pretty"))) {
	     i++;
#ifdef LIBXML_OUTPUT_ENABLED
       if (argv[i] != NULL) {
	         format = atoi(argv[i]);
	         if (format == 1) {
	             noblanks++;
	             xmlKeepBlanksDefault(0);
	         }
       }
#endif /* LIBXML_OUTPUT_ENABLED */
	}
#ifdef LIBXML_READER_ENABLED
	else if ((!strcmp(argv[i], "-stream")) ||
	         (!strcmp(argv[i], "--stream"))) {
	     stream++;
	}
	else if ((!strcmp(argv[i], "-walker")) ||
	         (!strcmp(argv[i], "--walker"))) {
	     walker++;
             noout++;
#ifdef LIBXML_PATTERN_ENABLED
        } else if ((!strcmp(argv[i], "-pattern")) ||
                   (!strcmp(argv[i], "--pattern"))) {
	    i++;
	    pattern = argv[i];
#endif
	}
#endif /* LIBXML_READER_ENABLED */
#ifdef LIBXML_SAX1_ENABLED
	else if ((!strcmp(argv[i], "-sax1")) ||
	         (!strcmp(argv[i], "--sax1"))) {
	    sax1++;
	    options |= XML_PARSE_SAX1;
	}
#endif /* LIBXML_SAX1_ENABLED */
	else if ((!strcmp(argv[i], "-sax")) ||
	         (!strcmp(argv[i], "--sax"))) {
	    sax++;
	}
	else if ((!strcmp(argv[i], "-chkregister")) ||
	         (!strcmp(argv[i], "--chkregister"))) {
	    chkregister++;
#ifdef LIBXML_SCHEMAS_ENABLED
	} else if ((!strcmp(argv[i], "-relaxng")) ||
	         (!strcmp(argv[i], "--relaxng"))) {
	    i++;
	    relaxng = argv[i];
	    noent++;
	    options |= XML_PARSE_NOENT;
	} else if ((!strcmp(argv[i], "-schema")) ||
	         (!strcmp(argv[i], "--schema"))) {
	    i++;
	    schema = argv[i];
	    noent++;
#endif
#ifdef LIBXML_SCHEMATRON_ENABLED
	} else if ((!strcmp(argv[i], "-schematron")) ||
	         (!strcmp(argv[i], "--schematron"))) {
	    i++;
	    schematron = argv[i];
	    noent++;
#endif
        } else if ((!strcmp(argv[i], "-nonet")) ||
                   (!strcmp(argv[i], "--nonet"))) {
	    options |= XML_PARSE_NONET;
	    xmlSetExternalEntityLoader(xmlNoNetExternalEntityLoader);
        } else if ((!strcmp(argv[i], "-nocompact")) ||
                   (!strcmp(argv[i], "--nocompact"))) {
	    options &= ~XML_PARSE_COMPACT;
	} else if ((!strcmp(argv[i], "-load-trace")) ||
	           (!strcmp(argv[i], "--load-trace"))) {
	    load_trace++;
        } else if ((!strcmp(argv[i], "-path")) ||
                   (!strcmp(argv[i], "--path"))) {
	    i++;
	    parsePath(BAD_CAST argv[i]);
#ifdef LIBXML_XPATH_ENABLED
        } else if ((!strcmp(argv[i], "-xpath")) ||
                   (!strcmp(argv[i], "--xpath"))) {
	    i++;
	    noout++;
	    xpathquery = argv[i];
#endif
	} else if ((!strcmp(argv[i], "-oldxml10")) ||
	           (!strcmp(argv[i], "--oldxml10"))) {
	    oldxml10++;
	    options |= XML_PARSE_OLD10;
	} else {
	    fprintf(stderr, "Unknown option %s\n", argv[i]);
	    usage(stderr, argv[0]);
	    return(1);
	}
    }

#ifdef LIBXML_CATALOG_ENABLED
    if (nocatalogs == 0) {
	if (catalogs) {
	    const char *catal;

	    catal = getenv("SGML_CATALOG_FILES");
	    if (catal != NULL) {
		xmlLoadCatalogs(catal);
	    } else {
		fprintf(stderr, "Variable $SGML_CATALOG_FILES not set\n");
	    }
	}
    }
#endif

#ifdef LIBXML_SAX1_ENABLED
    if (sax1)
        xmlSAXDefaultVersion(1);
    else
        xmlSAXDefaultVersion(2);
#endif /* LIBXML_SAX1_ENABLED */

    if (chkregister) {
	xmlRegisterNodeDefault(registerNode);
	xmlDeregisterNodeDefault(deregisterNode);
    }

    indent = getenv("XMLLINT_INDENT");
    if(indent != NULL) {
	xmlTreeIndentString = indent;
    }


    defaultEntityLoader = xmlGetExternalEntityLoader();
    xmlSetExternalEntityLoader(xmllintExternalEntityLoader);

    xmlLineNumbersDefault(1);
    if (loaddtd != 0)
	xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
    if (dtdattrs)
	xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;
    if (noent != 0) xmlSubstituteEntitiesDefault(1);
#ifdef LIBXML_VALID_ENABLED
    if (valid != 0) xmlDoValidityCheckingDefaultValue = 1;
#endif /* LIBXML_VALID_ENABLED */
    if ((htmlout) && (!nowrap)) {
	xmlGenericError(xmlGenericErrorContext,
         "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n");
	xmlGenericError(xmlGenericErrorContext,
		"\t\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n");
	xmlGenericError(xmlGenericErrorContext,
	 "<html><head><title>%s output</title></head>\n",
		argv[0]);
	xmlGenericError(xmlGenericErrorContext,
	 "<body bgcolor=\"#ffffff\"><h1 align=\"center\">%s output</h1>\n",
		argv[0]);
    }

#ifdef LIBXML_SCHEMATRON_ENABLED
    if ((schematron != NULL) && (sax == 0)
#ifdef LIBXML_READER_ENABLED
        && (stream == 0)
#endif /* LIBXML_READER_ENABLED */
	) {
	xmlSchematronParserCtxtPtr ctxt;

        /* forces loading the DTDs */
        xmlLoadExtDtdDefaultValue |= 1;
	options |= XML_PARSE_DTDLOAD;
	if (timing) {
	    startTimer();
	}
	ctxt = xmlSchematronNewParserCtxt(schematron);
#if 0
	xmlSchematronSetParserErrors(ctxt, xmlGenericError, xmlGenericError,
                NULL);
#endif
	wxschematron = xmlSchematronParse(ctxt);
	if (wxschematron == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "Schematron schema %s failed to compile\n", schematron);
            progresult = XMLLINT_ERR_SCHEMACOMP;
	    schematron = NULL;
	}
	xmlSchematronFreeParserCtxt(ctxt);
	if (timing) {
	    endTimer("Compiling the schemas");
	}
    }
#endif
#ifdef LIBXML_SCHEMAS_ENABLED
    if ((relaxng != NULL) && (sax == 0)
#ifdef LIBXML_READER_ENABLED
        && (stream == 0)
#endif /* LIBXML_READER_ENABLED */
	) {
	xmlRelaxNGParserCtxtPtr ctxt;

        /* forces loading the DTDs */
        xmlLoadExtDtdDefaultValue |= 1;
	options |= XML_PARSE_DTDLOAD;
	if (timing) {
	    startTimer();
	}
	ctxt = xmlRelaxNGNewParserCtxt(relaxng);
	xmlRelaxNGSetParserErrors(ctxt, xmlGenericError, xmlGenericError,
                NULL);
	relaxngschemas = xmlRelaxNGParse(ctxt);
	if (relaxngschemas == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "Relax-NG schema %s failed to compile\n", relaxng);
            progresult = XMLLINT_ERR_SCHEMACOMP;
	    relaxng = NULL;
	}
	xmlRelaxNGFreeParserCtxt(ctxt);
	if (timing) {
	    endTimer("Compiling the schemas");
	}
    } else if ((schema != NULL)
#ifdef LIBXML_READER_ENABLED
		&& (stream == 0)
#endif
	) {
	xmlSchemaParserCtxtPtr ctxt;

	if (timing) {
	    startTimer();
	}
	ctxt = xmlSchemaNewParserCtxt(schema);
	xmlSchemaSetParserErrors(ctxt, xmlGenericError, xmlGenericError, NULL);
	wxschemas = xmlSchemaParse(ctxt);
	if (wxschemas == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "WXS schema %s failed to compile\n", schema);
            progresult = XMLLINT_ERR_SCHEMACOMP;
	    schema = NULL;
	}
	xmlSchemaFreeParserCtxt(ctxt);
	if (timing) {
	    endTimer("Compiling the schemas");
	}
    }
#endif /* LIBXML_SCHEMAS_ENABLED */
#if defined(LIBXML_READER_ENABLED) && defined(LIBXML_PATTERN_ENABLED)
    if ((pattern != NULL) && (walker == 0)) {
        patternc = xmlPatterncompile((const xmlChar *) pattern, NULL, 0, NULL);
	if (patternc == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		    "Pattern %s failed to compile\n", pattern);
            progresult = XMLLINT_ERR_SCHEMAPAT;
	    pattern = NULL;
	}
    }
#endif /* LIBXML_READER_ENABLED && LIBXML_PATTERN_ENABLED */
    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-encode")) ||
	         (!strcmp(argv[i], "--encode"))) {
	    i++;
	    continue;
        } else if ((!strcmp(argv[i], "-o")) ||
                   (!strcmp(argv[i], "-output")) ||
                   (!strcmp(argv[i], "--output"))) {
            i++;
	    continue;
        }
#ifdef LIBXML_VALID_ENABLED
	if ((!strcmp(argv[i], "-dtdvalid")) ||
	         (!strcmp(argv[i], "--dtdvalid"))) {
	    i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-path")) ||
                   (!strcmp(argv[i], "--path"))) {
            i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-dtdvalidfpi")) ||
	         (!strcmp(argv[i], "--dtdvalidfpi"))) {
	    i++;
	    continue;
        }
#endif /* LIBXML_VALID_ENABLED */
	if ((!strcmp(argv[i], "-relaxng")) ||
	         (!strcmp(argv[i], "--relaxng"))) {
	    i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-maxmem")) ||
	         (!strcmp(argv[i], "--maxmem"))) {
	    i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-pretty")) ||
	         (!strcmp(argv[i], "--pretty"))) {
	    i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-schema")) ||
	         (!strcmp(argv[i], "--schema"))) {
	    i++;
	    continue;
        }
	if ((!strcmp(argv[i], "-schematron")) ||
	         (!strcmp(argv[i], "--schematron"))) {
	    i++;
	    continue;
        }
#if defined(LIBXML_READER_ENABLED) && defined(LIBXML_PATTERN_ENABLED)
        if ((!strcmp(argv[i], "-pattern")) ||
	    (!strcmp(argv[i], "--pattern"))) {
	    i++;
	    continue;
	}
#endif
#ifdef LIBXML_XPATH_ENABLED
        if ((!strcmp(argv[i], "-xpath")) ||
	    (!strcmp(argv[i], "--xpath"))) {
	    i++;
	    continue;
	}
#endif
	if ((timing) && (repeat))
	    startTimer();
	/* Remember file names.  "-" means stdin.  <sven@zen.org> */
	if ((argv[i][0] != '-') || (strcmp(argv[i], "-") == 0)) {
	    if (repeat) {
		xmlParserCtxtPtr ctxt = NULL;

		for (acount = 0;acount < repeat;acount++) {
#ifdef LIBXML_READER_ENABLED
		    if (stream != 0) {
			streamFile(argv[i]);
		    } else {
#endif /* LIBXML_READER_ENABLED */
                        if (sax) {
			    testSAX(argv[i]);
			} else {
			    if (ctxt == NULL)
				ctxt = xmlNewParserCtxt();
			    parseAndPrintFile(argv[i], ctxt);
			}
#ifdef LIBXML_READER_ENABLED
		    }
#endif /* LIBXML_READER_ENABLED */
		}
		if (ctxt != NULL)
		    xmlFreeParserCtxt(ctxt);
	    } else {
		nbregister = 0;

#ifdef LIBXML_READER_ENABLED
		if (stream != 0)
		    streamFile(argv[i]);
		else
#endif /* LIBXML_READER_ENABLED */
                if (sax) {
		    testSAX(argv[i]);
		} else {
		    parseAndPrintFile(argv[i], NULL);
		}

                if ((chkregister) && (nbregister != 0)) {
		    fprintf(stderr, "Registration count off: %d\n", nbregister);
		    progresult = XMLLINT_ERR_RDREGIS;
		}
	    }
	    files ++;
	    if ((timing) && (repeat)) {
		endTimer("%d iterations", repeat);
	    }
	}
    }
    if (generate)
	parseAndPrintFile(NULL, NULL);
    if ((htmlout) && (!nowrap)) {
	xmlGenericError(xmlGenericErrorContext, "</body></html>\n");
    }
    if ((files == 0) && (!generate) && (version == 0)) {
	usage(stderr, argv[0]);
    }
#ifdef LIBXML_SCHEMATRON_ENABLED
    if (wxschematron != NULL)
	xmlSchematronFree(wxschematron);
#endif
#ifdef LIBXML_SCHEMAS_ENABLED
    if (relaxngschemas != NULL)
	xmlRelaxNGFree(relaxngschemas);
    if (wxschemas != NULL)
	xmlSchemaFree(wxschemas);
    xmlRelaxNGCleanupTypes();
#endif
#if defined(LIBXML_READER_ENABLED) && defined(LIBXML_PATTERN_ENABLED)
    if (patternc != NULL)
        xmlFreePattern(patternc);
#endif
    xmlCleanupParser();
    xmlMemoryDump();

    return(progresult);
}