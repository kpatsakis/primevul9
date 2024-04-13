static int isLegalUTF8( const unsigned char *source, int length ) {
    unsigned char a;
    const unsigned char *srcptr = source + length;
    switch ( length ) {
    default:
        return 0;
        /* Everything else falls through when "true"... */
    case 4:
        if ( ( a = ( *--srcptr ) ) < 0x80 || a > 0xBF ) return 0;
    case 3:
        if ( ( a = ( *--srcptr ) ) < 0x80 || a > 0xBF ) return 0;
    case 2:
        if ( ( a = ( *--srcptr ) ) > 0xBF ) return 0;
        switch ( *source ) {
            /* no fall-through in this inner switch */
        case 0xE0:
            if ( a < 0xA0 ) return 0;
            break;
        case 0xF0:
            if ( a < 0x90 ) return 0;
            break;
        case 0xF4:
            if ( a > 0x8F ) return 0;
            break;
        default:
            if ( a < 0x80 ) return 0;
        }
    case 1:
        if ( *source >= 0x80 && *source < 0xC2 ) return 0;
        if ( *source > 0xF4 ) return 0;
    }
    return 1;
}