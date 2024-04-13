xmlExpDumpInt(xmlBufferPtr buf, xmlExpNodePtr expr, int glob) {
    xmlExpNodePtr c;

    if (expr == NULL) return;
    if (glob) xmlBufferWriteChar(buf, "(");
    switch (expr->type) {
        case XML_EXP_EMPTY:
	    xmlBufferWriteChar(buf, "empty");
	    break;
        case XML_EXP_FORBID:
	    xmlBufferWriteChar(buf, "forbidden");
	    break;
        case XML_EXP_ATOM:
	    xmlBufferWriteCHAR(buf, expr->exp_str);
	    break;
        case XML_EXP_SEQ:
	    c = expr->exp_left;
	    if ((c->type == XML_EXP_SEQ) || (c->type == XML_EXP_OR))
	        xmlExpDumpInt(buf, c, 1);
	    else
	        xmlExpDumpInt(buf, c, 0);
	    xmlBufferWriteChar(buf, " , ");
	    c = expr->exp_right;
	    if ((c->type == XML_EXP_SEQ) || (c->type == XML_EXP_OR))
	        xmlExpDumpInt(buf, c, 1);
	    else
	        xmlExpDumpInt(buf, c, 0);
            break;
        case XML_EXP_OR:
	    c = expr->exp_left;
	    if ((c->type == XML_EXP_SEQ) || (c->type == XML_EXP_OR))
	        xmlExpDumpInt(buf, c, 1);
	    else
	        xmlExpDumpInt(buf, c, 0);
	    xmlBufferWriteChar(buf, " | ");
	    c = expr->exp_right;
	    if ((c->type == XML_EXP_SEQ) || (c->type == XML_EXP_OR))
	        xmlExpDumpInt(buf, c, 1);
	    else
	        xmlExpDumpInt(buf, c, 0);
            break;
        case XML_EXP_COUNT: {
	    char rep[40];

	    c = expr->exp_left;
	    if ((c->type == XML_EXP_SEQ) || (c->type == XML_EXP_OR))
	        xmlExpDumpInt(buf, c, 1);
	    else
	        xmlExpDumpInt(buf, c, 0);
	    if ((expr->exp_min == 0) && (expr->exp_max == 1)) {
		rep[0] = '?';
		rep[1] = 0;
	    } else if ((expr->exp_min == 0) && (expr->exp_max == -1)) {
		rep[0] = '*';
		rep[1] = 0;
	    } else if ((expr->exp_min == 1) && (expr->exp_max == -1)) {
		rep[0] = '+';
		rep[1] = 0;
	    } else if (expr->exp_max == expr->exp_min) {
	        snprintf(rep, 39, "{%d}", expr->exp_min);
	    } else if (expr->exp_max < 0) {
	        snprintf(rep, 39, "{%d,inf}", expr->exp_min);
	    } else {
	        snprintf(rep, 39, "{%d,%d}", expr->exp_min, expr->exp_max);
	    }
	    rep[39] = 0;
	    xmlBufferWriteChar(buf, rep);
	    break;
	}
	default:
	    fprintf(stderr, "Error in tree\n");
    }
    if (glob)
        xmlBufferWriteChar(buf, ")");
}