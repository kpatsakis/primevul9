void Scanner::pop_finished_files()
{
    // Pop all files that have been fully processed (file upper bound
    // in buffer points before the first character of current lexeme),
    // except for the first (main) file which must always remain at the
    // bottom of the stack.
    size_t i = files.size();
    DASSERT(i > 0);
    for (;;) {
        --i;
        Input *in = files[i];
        if (i == 0 || in->eo >= tok) break;
        files.pop_back();
        delete in;
    }
}