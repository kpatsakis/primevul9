size_t Scanner::get_input_index() const
{
    // Find index of the current input file: the one corresponding to
    // buffer fragment that contains cursor.
    size_t i = files.size();
    DASSERT(i > 0);
    for (;;) {
        --i;
        Input *in = files[i];
        if (i == 0 || (cur >= in->so && cur <= in->eo)) break;
    }
    return i;
}