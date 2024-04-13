int DL_Dxf::getLibVersion(const std::string& str) {
    int d[4];
    int idx = 0;
    //char v[4][5];
    std::string v[4];
    int ret = 0;

    for (unsigned int i=0; i<str.length() && idx<3; ++i) {
        if (str[i]=='.') {
            d[idx] = i;
            idx++;
        }
    }

    if (idx>=2) {
        d[3] = str.length();

        v[0] = str.substr(0, d[0]);
        v[1] = str.substr(d[0]+1, d[1]-d[0]-1);
        v[2] = str.substr(d[1]+1, d[2]-d[1]-1);
        if (idx>=3) {
            v[3] = str.substr(d[2]+1, d[3]-d[2]-1);
        }
        else {
            v[3] = "0";
        }

        ret = (atoi(v[0].c_str())<<(3*8)) +
              (atoi(v[1].c_str())<<(2*8)) +
              (atoi(v[2].c_str())<<(1*8)) +
              (atoi(v[3].c_str())<<(0*8));

        return ret;
    } else {
        std::cerr << "DL_Dxf::getLibVersion: invalid version number: " << str << "\n";
        return 0;
    }
}