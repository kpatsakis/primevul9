bool DL_Dxf::getStrippedLine(std::string &s, unsigned int size,
                            std::istream& stream, bool stripSpace) {

    if (!stream.eof()) {
        // Only the useful part of the line
        char* line = new char[size+1];
        char* oriLine = line;
        stream.getline(line, size);
        stripWhiteSpace(&line, stripSpace);
        s = line;
        assert(size > s.length());
        delete[] oriLine;
        return true;
    } else {
        s[0] = '\0';
        return false;
    }
}