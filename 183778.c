read_indent(const char *input, int indent, int size, int in_index, int *out_index, char *output)
{
    int k = 0, j;

    while (in_index < size) {
        if (input[in_index] == ' ') {
            k++;
        } else if (input[in_index] == '\t') {
            /* RFC 6020 6.1.3 tab character is treated as 8 space characters */
            k += 8;
        } else  if (input[in_index] == '\\' && input[in_index + 1] == 't') {
            /* RFC 6020 6.1.3 tab character is treated as 8 space characters */
            k += 8;
            ++in_index;
        } else {
            break;
        }
        ++in_index;
        if (k >= indent) {
            for (j = k - indent; j > 0; --j) {
                output[*out_index] = ' ';
                if (j > 1) {
                    ++(*out_index);
                }
            }
            break;
        }
    }
    return in_index - 1;
}