static VALUE json_string_unescape(VALUE result, char *string, char *stringEnd)
{
    char *p = string, *pe = string, *unescape;
    int unescape_len;
    char buf[4];

    while (pe < stringEnd) {
        if (*pe == '\\') {
            unescape = (char *) "?";
            unescape_len = 1;
            if (pe > p) rb_str_buf_cat(result, p, pe - p);
            switch (*++pe) {
                case 'n':
                    unescape = (char *) "\n";
                    break;
                case 'r':
                    unescape = (char *) "\r";
                    break;
                case 't':
                    unescape = (char *) "\t";
                    break;
                case '"':
                    unescape = (char *) "\"";
                    break;
                case '\\':
                    unescape = (char *) "\\";
                    break;
                case 'b':
                    unescape = (char *) "\b";
                    break;
                case 'f':
                    unescape = (char *) "\f";
                    break;
                case 'u':
                    if (pe > stringEnd - 4) {
                      rb_enc_raise(
                        EXC_ENCODING eParserError,
                        "%u: incomplete unicode character escape sequence at '%s'", __LINE__, p
                      );
                    } else {
                        UTF32 ch = unescape_unicode((unsigned char *) ++pe);
                        pe += 3;
                        if (UNI_SUR_HIGH_START == (ch & 0xFC00)) {
                            pe++;
                            if (pe > stringEnd - 6) {
                              rb_enc_raise(
                                EXC_ENCODING eParserError,
                                "%u: incomplete surrogate pair at '%s'", __LINE__, p
                                );
                            }
                            if (pe[0] == '\\' && pe[1] == 'u') {
                                UTF32 sur = unescape_unicode((unsigned char *) pe + 2);
                                ch = (((ch & 0x3F) << 10) | ((((ch >> 6) & 0xF) + 1) << 16)
                                        | (sur & 0x3FF));
                                pe += 5;
                            } else {
                                unescape = (char *) "?";
                                break;
                            }
                        }
                        unescape_len = convert_UTF32_to_UTF8(buf, ch);
                        unescape = buf;
                    }
                    break;
                default:
                    p = pe;
                    continue;
            }
            rb_str_buf_cat(result, unescape, unescape_len);
            p = ++pe;
        } else {
            pe++;
        }
    }
    rb_str_buf_cat(result, p, pe - p);
    return result;
}