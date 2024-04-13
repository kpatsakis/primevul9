DL_WriterA* DL_Dxf::out(const char* file, DL_Codes::version version) {
    char* f = new char[strlen(file)+1];
    strcpy(f, file);
    this->version = version;

    DL_WriterA* dw = new DL_WriterA(f, version);
    if (dw->openFailed()) {
        delete dw;
        delete[] f;
        return NULL;
    } else {
        delete[] f;
        return dw;
    }
}