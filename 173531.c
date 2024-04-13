bool Scanner::read(size_t want)
{
    DASSERT(!files.empty());
    for (size_t i = files.size(); i --> 0; ) {
        Input *in = files[i];
        const size_t have = fread(lim, 1, want, in->file);
        in->so = lim;
        lim += have;
        in->eo = lim;
        want -= have;

        // buffer filled
        if (want == 0) return true;
    }
    return false;
}