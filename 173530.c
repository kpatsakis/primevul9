bool Scanner::include(const std::string &filename)
{
    // get name of the current file (before unreading)
    DASSERT(!files.empty());
    const std::string &parent = files.back()->escaped_name;

    // unread buffer tail: we'll return to it later
    // In the buffer nested files go before outer files. In the file stack,
    // however, outer files go before nested files (nested are at the top).
    // We want to break from the unreading cycle early, therefore we go in
    // reverse order of file offsets in buffer and break as soon as the end
    // offset is less than cursor (current position).
    for (size_t i = 0; i < files.size(); ++i) {
        Input *in = files[i];
        if (in->so >= cur) {
            // unread whole fragment
            fseek(in->file, in->so - in->eo, SEEK_CUR);
            in->so = in->eo = ENDPOS;
        }
        else if (in->eo >= cur) {
            // fragment on the boundary, unread partially
            fseek(in->file, cur - in->eo, SEEK_CUR);
            in->eo = cur - 1;
        }
        else {
            // the rest has been consumed already
            break;
        }
    }

    // open new file and place place at the top of stack
    if (!open(filename, &parent)) {
        return false;
    }

    // refill buffer (discard everything up to cursor, clear EOF)
    lim = cur = mar = ctx = tok = ptr = pos = bot + BSIZE;
    eof = NULL;
    return fill(BSIZE);
}