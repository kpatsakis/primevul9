Scanner::~Scanner()
{
    for (size_t i = files.size(); i --> 0; ) {
        delete files[i];
    }
}