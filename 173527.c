bool Scanner::open(const std::string &filename, const std::string *parent)
{
    Input *in = new Input(msg.filenames.size());
    files.push_back(in);
    if (!in->open(filename, parent, globopts->incpaths)) {
        return false;
    }
    msg.filenames.push_back(in->escaped_name);
    return true;
}