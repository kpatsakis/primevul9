
static int
xmlXPathEqualValuesCommon(xmlXPathParserContextPtr ctxt,
  xmlXPathObjectPtr arg1, xmlXPathObjectPtr arg2) {
    int ret = 0;
    /*
     *At this point we are assured neither arg1 nor arg2
     *is a nodeset, so we can just pick the appropriate routine.
     */
    switch (arg1->type) {
        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	    xmlGenericError(xmlGenericErrorContext,
		    "Equal: undefined\n");
#endif
	    break;
        case XPATH_BOOLEAN:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: %d boolean %d \n",
			    arg1->boolval, arg2->boolval);
#endif
		    ret = (arg1->boolval == arg2->boolval);
		    break;
		case XPATH_NUMBER:
		    ret = (arg1->boolval ==
			   xmlXPathCastNumberToBoolean(arg2->floatval));
		    break;
		case XPATH_STRING:
		    if ((arg2->stringval == NULL) ||
			(arg2->stringval[0] == 0)) ret = 0;
		    else
			ret = 1;
		    ret = (arg1->boolval == ret);
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_NUMBER:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    ret = (arg2->boolval==
			   xmlXPathCastNumberToBoolean(arg1->floatval));
		    break;
		case XPATH_STRING:
		    valuePush(ctxt, arg2);
		    xmlXPathNumberFunction(ctxt, 1);
		    arg2 = valuePop(ctxt);
                    /* Falls through. */
		case XPATH_NUMBER:
		    /* Hand check NaN and Infinity equalities */
		    if (xmlXPathIsNaN(arg1->floatval) ||
			    xmlXPathIsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == 1) {
		        if (xmlXPathIsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == -1) {
			if (xmlXPathIsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == 1) {
			if (xmlXPathIsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == -1) {
			if (xmlXPathIsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_STRING:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    if ((arg1->stringval == NULL) ||
			(arg1->stringval[0] == 0)) ret = 0;
		    else
			ret = 1;
		    ret = (arg2->boolval == ret);
		    break;
		case XPATH_STRING:
		    ret = xmlStrEqual(arg1->stringval, arg2->stringval);
		    break;
		case XPATH_NUMBER:
		    valuePush(ctxt, arg1);
		    xmlXPathNumberFunction(ctxt, 1);
		    arg1 = valuePop(ctxt);
		    /* Hand check NaN and Infinity equalities */
		    if (xmlXPathIsNaN(arg1->floatval) ||
			    xmlXPathIsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == 1) {
			if (xmlXPathIsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg1->floatval) == -1) {
			if (xmlXPathIsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == 1) {
			if (xmlXPathIsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (xmlXPathIsInf(arg2->floatval) == -1) {
			if (xmlXPathIsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_USERS:
	case XPATH_POINT:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    TODO
	    break;
	case XPATH_NODESET:
	case XPATH_XSLT_TREE:
	    break;
    }
    xmlXPathReleaseObject(ctxt->context, arg1);
    xmlXPathReleaseObject(ctxt->context, arg2);