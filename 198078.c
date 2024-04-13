xmlRegPrintQuantType(FILE *output, xmlRegQuantType type) {
    switch (type) {
        case XML_REGEXP_QUANT_EPSILON:
	    fprintf(output, "epsilon "); break;
        case XML_REGEXP_QUANT_ONCE:
	    fprintf(output, "once "); break;
        case XML_REGEXP_QUANT_OPT:
	    fprintf(output, "? "); break;
        case XML_REGEXP_QUANT_MULT:
	    fprintf(output, "* "); break;
        case XML_REGEXP_QUANT_PLUS:
	    fprintf(output, "+ "); break;
	case XML_REGEXP_QUANT_RANGE:
	    fprintf(output, "range "); break;
	case XML_REGEXP_QUANT_ONCEONLY:
	    fprintf(output, "onceonly "); break;
	case XML_REGEXP_QUANT_ALL:
	    fprintf(output, "all "); break;
    }
}