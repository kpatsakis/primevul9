bool DL_Dxf::getStrippedLine(std::string& s, unsigned int size, FILE *fp, bool stripSpace) {
    if (!feof(fp)) {
        // The whole line in the file.  Includes space for NULL.
        char* wholeLine = new char[size];
        // Only the useful part of the line
        char* line;

        line = fgets(wholeLine, size, fp);

        if (line!=NULL && line[0] != '\0') { // Evaluates to fgets() retval
            // line == wholeLine at this point.
            // Both guaranteed to be NULL terminated.

            // Strip leading whitespace and trailing CR/LF.
            stripWhiteSpace(&line, stripSpace);

            s = line;
            assert(size > s.length());
        }

        delete[] wholeLine; // Done with wholeLine

        return true;
    } else {
        s = "";
        return false;
    }
}