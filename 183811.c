yang_read_string(struct ly_ctx *ctx, const char *input, char *output, int size, int offset, int indent)
{
    int i = 0, out_index = offset, space = 0;

    while (i < size) {
        switch (input[i]) {
        case '\n':
            out_index -= space;
            output[out_index] = '\n';
            space = 0;
            i = read_indent(input, indent, size, i + 1, &out_index, output);
            break;
        case ' ':
        case '\t':
            output[out_index] = input[i];
            ++space;
            break;
        case '\\':
            if (input[i + 1] == 'n') {
                out_index -= space;
                output[out_index] = '\n';
                space = 0;
                i = read_indent(input, indent, size, i + 2, &out_index, output);
            } else if (input[i + 1] == 't') {
                output[out_index] = '\t';
                ++i;
                ++space;
            } else if (input[i + 1] == '\\') {
                output[out_index] = '\\';
                ++i;
            } else if ((i + 1) != size && input[i + 1] == '"') {
                output[out_index] = '"';
                ++i;
            } else {
                /* backslash must not be followed by any other character */
                LOGVAL(ctx, LYE_XML_INCHAR, LY_VLOG_NONE, NULL, input + i);
                return NULL;
            }
            break;
        default:
            output[out_index] = input[i];
            space = 0;
            break;
        }
        ++i;
        ++out_index;
    }
    output[out_index] = '\0';
    if (size != out_index) {
        output = realloc(output, out_index + 1);
        LY_CHECK_ERR_RETURN(!output, LOGMEM(ctx), NULL);
    }
    return output;
}