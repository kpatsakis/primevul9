file_signextend(struct magic_set *ms, struct magic *m, uint64_t v)
{
	if (!(m->flag & UNSIGNED)) {
		switch(m->type) {
		/*
		 * Do not remove the casts below.  They are
		 * vital.  When later compared with the data,
		 * the sign extension must have happened.
		 */
		case FILE_BYTE:
			v = (char) v;
			break;
		case FILE_SHORT:
		case FILE_BESHORT:
		case FILE_LESHORT:
			v = (short) v;
			break;
		case FILE_DATE:
		case FILE_BEDATE:
		case FILE_LEDATE:
		case FILE_MEDATE:
		case FILE_LDATE:
		case FILE_BELDATE:
		case FILE_LELDATE:
		case FILE_MELDATE:
		case FILE_LONG:
		case FILE_BELONG:
		case FILE_LELONG:
		case FILE_MELONG:
		case FILE_FLOAT:
		case FILE_BEFLOAT:
		case FILE_LEFLOAT:
			v = (int32_t) v;
			break;
		case FILE_QUAD:
		case FILE_BEQUAD:
		case FILE_LEQUAD:
		case FILE_QDATE:
		case FILE_QLDATE:
		case FILE_QWDATE:
		case FILE_BEQDATE:
		case FILE_BEQLDATE:
		case FILE_BEQWDATE:
		case FILE_LEQDATE:
		case FILE_LEQLDATE:
		case FILE_LEQWDATE:
		case FILE_DOUBLE:
		case FILE_BEDOUBLE:
		case FILE_LEDOUBLE:
			v = (int64_t) v;
			break;
		case FILE_STRING:
		case FILE_PSTRING:
		case FILE_BESTRING16:
		case FILE_LESTRING16:
		case FILE_REGEX:
		case FILE_SEARCH:
		case FILE_DEFAULT:
		case FILE_INDIRECT:
		case FILE_NAME:
		case FILE_USE:
			break;
		default:
			if (ms->flags & MAGIC_CHECK)
			    file_magwarn(ms, "cannot happen: m->type=%d\n",
				    m->type);
			return ~0U;
		}
	}
	return v;
}