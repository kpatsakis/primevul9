static char hexbyte( char hex ) {
    if (hex >= '0' && hex <= '9')
        return (hex - '0');
    else if (hex >= 'A' && hex <= 'F')
        return (hex - 'A' + 10);
    else if (hex >= 'a' && hex <= 'f')
        return (hex - 'a' + 10);
    else
        return 0x0;
}