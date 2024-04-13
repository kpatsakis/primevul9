bool DL_Dxf::in(const std::string& file, DL_CreationInterface* creationInterface) {
    FILE *fp;
    firstCall = true;
    currentObjectType = DL_UNKNOWN;

    fp = fopen(file.c_str(), "rt");
    if (fp) {
        std::locale oldLocale = std::locale::global(std::locale("C"));	// use dot in numbers
        while (readDxfGroups(fp, creationInterface)) {}
        std::locale::global(oldLocale);
        fclose(fp);
        return true;
    }

    return false;
}