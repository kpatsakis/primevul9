void DL_Dxf::test() {
    char* buf1;
    char* buf2;
    char* buf3;
    char* buf4;
    char* buf5;
    char* buf6;

    buf1 = new char[10];
    buf2 = new char[10];
    buf3 = new char[10];
    buf4 = new char[10];
    buf5 = new char[10];
    buf6 = new char[10];

    strcpy(buf1, "  10\n");
    strcpy(buf2, "10");
    strcpy(buf3, "10\n");
    strcpy(buf4, "  10 \n");
    strcpy(buf5, "  10 \r");
    strcpy(buf6, "\t10 \n");

    std::cout << "1 buf1: '" << buf1 << "'\n";
    stripWhiteSpace(&buf1);
    std::cout << "2 buf1: '" << buf1 << "'\n";
    //assert(!strcmp(buf1, "10"));

    std::cout << "1 buf2: '" << buf2 << "'\n";
    stripWhiteSpace(&buf2);
    std::cout << "2 buf2: '" << buf2 << "'\n";

    std::cout << "1 buf3: '" << buf3 << "'\n";
    stripWhiteSpace(&buf3);
    std::cout << "2 buf3: '" << buf3 << "'\n";

    std::cout << "1 buf4: '" << buf4 << "'\n";
    stripWhiteSpace(&buf4);
    std::cout << "2 buf4: '" << buf4 << "'\n";

    std::cout << "1 buf5: '" << buf5 << "'\n";
    stripWhiteSpace(&buf5);
    std::cout << "2 buf5: '" << buf5 << "'\n";

    std::cout << "1 buf6: '" << buf6 << "'\n";
    stripWhiteSpace(&buf6);
    std::cout << "2 buf6: '" << buf6 << "'\n";

}