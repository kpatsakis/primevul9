xmlExpHashComputeKey(xmlExpNodeType type, xmlExpNodePtr left,
                     xmlExpNodePtr right) {
    unsigned long value;
    unsigned short ret;

    switch (type) {
        case XML_EXP_SEQ:
	    value = left->key;
	    value += right->key;
	    value *= 3;
	    ret = (unsigned short) value;
	    break;
        case XML_EXP_OR:
	    value = left->key;
	    value += right->key;
	    value *= 7;
	    ret = (unsigned short) value;
	    break;
        case XML_EXP_COUNT:
	    value = left->key;
	    value += right->key;
	    ret = (unsigned short) value;
	    break;
	default:
	    ret = 0;
    }
    return(ret);
}