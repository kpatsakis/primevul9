append_text_move(pdf_text_state_t *pts, double dw)
{
    int count = pts->buffer.count_moves;
    int pos = pts->buffer.count_chars;
    double rounded;

    if (count > 0 && pts->buffer.moves[count - 1].index == pos) {
        /* Merge adjacent moves. */
        dw += pts->buffer.moves[--count].amount;
    }
    /* Round dw if it's very close to an integer. */
    rounded = floor(dw + 0.5);
    if (fabs(dw - rounded) < 0.001)
        dw = rounded;
    if (dw < -MAX_USER_COORD) {
        /* Acrobat reader 4.0c, 5.0 can't handle big offsets.
           Adobe Reader 6 can. */
        return -1;
    }
    if (dw != 0) {
        if (count == MAX_TEXT_BUFFER_MOVES)
            return -1;
        pts->buffer.moves[count].index = pos;
        pts->buffer.moves[count].amount = dw;
        ++count;
    }
    pts->buffer.count_moves = count;
    return 0;
}