void bcon_print(const bcon *bc) { /* prints internal representation, not JSON */
    char *typespec = 0;
    char *delim = "";
    int end_of_data;
    bcon *bcp;
    putchar('{');
    for (end_of_data = 0, bcp = (bcon*)bc; !end_of_data; bcp++) {
        bcon bci = *bcp;
        char *typespec_next = 0;
        if (typespec) {
            switch (typespec[1]) {
            case '_':
                switch (typespec[2]) {
                case 'f': printf("%s%f", delim, bci.f); break;
                case 's': printf("%s\"%s\"", delim, bci.s); break;
                case 'D': printf("%sPD(0x%lx,..)", delim, (unsigned long)bci.D); break;
                case 'A': printf("%sPA(0x%lx,....)", delim, (unsigned long)bci.A); break;
                case 'o': printf("%s\"%s\"", delim, bci.o); break;
                case 'b': printf("%s%d", delim, bci.b); break;
                case 't': printf("%s%ld", delim, (long)bci.t); break;
                case 'v': printf("%s\"%s\"", delim, bci.v); break;
                case 'x': printf("%s\"%s\"", delim, bci.x); break;
                case 'i': printf("%s%d", delim, bci.i); break;
                case 'l': printf("%s%ld", delim, bci.l); break;
                default: printf("\ntypespec:\"%s\"\n", typespec); assert(NOT_REACHED); break;
                }
                break;
            case 'R':
                switch (typespec[2]) {
                case 'f': printf("%sRf(0x%lx,%f)", delim, (unsigned long)bci.Rf, *bci.Rf); break;
                case 's': printf("%sRs(0x%lx,\"%s\")", delim, (unsigned long)bci.Rs, bci.Rs); break;
                case 'D': printf("%sRD(0x%lx,..)", delim, (unsigned long)bci.RD); break;
                case 'A': printf("%sRA(0x%lx,....)", delim, (unsigned long)bci.RA); break;
                case 'o': printf("%sRo(0x%lx,\"%s\")", delim, (unsigned long)bci.Ro, bci.Ro); break;
                case 'b': printf("%sRb(0x%lx,%d)", delim, (unsigned long)bci.Rb, *bci.Rb); break;
                case 't': printf("%sRt(0x%lx,%ld)", delim, (unsigned long)bci.Rt, (long)*bci.Rt); break;
                case 'x': printf("%sRx(0x%lx,\"%s\")", delim, (unsigned long)bci.Rx, bci.Rx); break;
                case 'i': printf("%sRi(0x%lx,%d)", delim, (unsigned long)bci.Ri, *bci.Ri); break;
                case 'l': printf("%sRl(0x%lx,%ld)", delim, (unsigned long)bci.Rl, *bci.Rl); break;
                default: printf("\ntypespec:\"%s\"\n", typespec); assert(NOT_REACHED); break;
                }
                break;
            case 'P':
                switch (typespec[2]) {
                case 'f': printf("%sPf(0x%lx,0x%lx,%f)", delim, (unsigned long)bci.Pf, (unsigned long)(bci.Pf ? *bci.Pf : 0), bci.Pf && *bci.Pf ? **bci.Pf : 0.0); break;
                case 's': printf("%sPs(0x%lx,0x%lx,\"%s\")", delim, (unsigned long)bci.Ps, (unsigned long)(bci.Ps ? *bci.Ps : 0), bci.Ps && *bci.Ps ? *bci.Ps : ""); break;
                case 'D': printf("%sPD(0x%lx,0x%lx,..)", delim, (unsigned long)bci.PD, (unsigned long)(bci.PD ? *bci.PD : 0)); break;
                case 'A': printf("%sPA(0x%lx,0x%lx,....)", delim, (unsigned long)bci.PA, (unsigned long)(bci.PA ? *bci.PA : 0)); break;
                case 'o': printf("%sPo(0x%lx,0x%lx,\"%s\")", delim, (unsigned long)bci.Po, (unsigned long)(bci.Po ? *bci.Po : 0), bci.Po && *bci.Po ? *bci.Po : ""); break;
                case 'b': printf("%sPb(0x%lx,0x%lx,%d)", delim, (unsigned long)bci.Pb, (unsigned long)(bci.Pb ? *bci.Pb : 0), bci.Pb && *bci.Pb ? **bci.Pb : 0); break;
                case 't': printf("%sPt(0x%lx,0x%lx,%ld)", delim, (unsigned long)bci.Pt, (unsigned long)(bci.Pt ? *bci.Pt : 0), bci.Pt && *bci.Pt ? (long)**bci.Pt : 0); break;
                case 'x': printf("%sPx(0x%lx,0x%lx,\"%s\")", delim, (unsigned long)bci.Px, (unsigned long)(bci.Px ? *bci.Px : 0), bci.Px && *bci.Px ? *bci.Px : ""); break;
                case 'i': printf("%sPi(0x%lx,0x%lx,%d)", delim, (unsigned long)bci.Pi, (unsigned long)(bci.Pi ? *bci.Pi : 0), bci.Pi && *bci.Pi ? **bci.Pi : 0); break;
                case 'l': printf("%sPl(0x%lx,0x%lx,%ld)", delim, (unsigned long)bci.Pl, (unsigned long)(bci.Pl ? *bci.Pl : 0), bci.Pl && *bci.Pl ? **bci.Pl : 0); break;

                default: printf("\ntypespec:\"%s\"\n", typespec); assert(NOT_REACHED); break;
                }
                break;
            default:
                printf("\ntypespec:\"%s\"\n", typespec); assert(NOT_REACHED);
                break;
            }
        }
        else {
            char *s = bci.s;
            switch (s[0]) {
            case '.':
                end_of_data = (s[1] == '\0');
                break;
            case ':':
                typespec_next = bcon_token(s) == Token_Typespec ? s : 0;
                break;
            }
            printf("%s\"%s\"", delim, s);
        }
        typespec = typespec_next;
        delim = ",";
    }
    putchar('}');
}