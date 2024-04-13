static xmlNodePtr to_xml_datetime_ex(encodeTypePtr type, zval *data, char *format, int style, xmlNodePtr parent TSRMLS_DC)
{
	/* logic hacked from ext/standard/datetime.c */
	struct tm *ta, tmbuf;
	time_t timestamp;
	int max_reallocs = 5;
	size_t buf_len=64, real_len;
	char *buf;
	char tzbuf[8];

	xmlNodePtr xmlParam;

	xmlParam = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, xmlParam);
	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_LONG) {
		timestamp = Z_LVAL_P(data);
		ta = php_localtime_r(&timestamp, &tmbuf);
		/*ta = php_gmtime_r(&timestamp, &tmbuf);*/
		if (!ta) {
			soap_error1(E_ERROR, "Encoding: Invalid timestamp %ld", Z_LVAL_P(data));
		}

		buf = (char *) emalloc(buf_len);
		while ((real_len = strftime(buf, buf_len, format, ta)) == buf_len || real_len == 0) {
			buf_len *= 2;
			buf = (char *) erealloc(buf, buf_len);
			if (!--max_reallocs) break;
		}

		/* Time zone support */
#ifdef HAVE_TM_GMTOFF
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", (ta->tm_gmtoff < 0) ? '-' : '+', abs(ta->tm_gmtoff / 3600), abs( (ta->tm_gmtoff % 3600) / 60 ));
#else
# if defined(__CYGWIN__) || defined(NETWARE)
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", ((ta->tm_isdst ? _timezone - 3600:_timezone)>0)?'-':'+', abs((ta->tm_isdst ? _timezone - 3600 : _timezone) / 3600), abs(((ta->tm_isdst ? _timezone - 3600 : _timezone) % 3600) / 60));
# else
		snprintf(tzbuf, sizeof(tzbuf), "%c%02d:%02d", ((ta->tm_isdst ? timezone - 3600:timezone)>0)?'-':'+', abs((ta->tm_isdst ? timezone - 3600 : timezone) / 3600), abs(((ta->tm_isdst ? timezone - 3600 : timezone) % 3600) / 60));
# endif
#endif
		if (strcmp(tzbuf,"+00:00") == 0) {
		  strcpy(tzbuf,"Z");
		  real_len++;
		} else {
			real_len += 6;
		}
		if (real_len >= buf_len) {
			buf = (char *) erealloc(buf, real_len+1);
		}
		strcat(buf, tzbuf);

		xmlNodeSetContent(xmlParam, BAD_CAST(buf));
		efree(buf);
	} else if (Z_TYPE_P(data) == IS_STRING) {
		xmlNodeSetContentLen(xmlParam, BAD_CAST(Z_STRVAL_P(data)), Z_STRLEN_P(data));
	}

	if (style == SOAP_ENCODED) {
		set_ns_and_type(xmlParam, type);
	}
	return xmlParam;
}