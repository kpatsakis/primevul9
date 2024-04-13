    std::string stringFormat(const char* format, ...)
    {
        std::string result;

        int     need   = (int) std::strlen(format)*2;          // initial guess
        char*   buffer = NULL;
        int     again  =    4;
        int     rc     =   -1;

        while (rc < 0 && again--) {
            if ( buffer ) delete[] buffer;
            need  *= 2 ;
            buffer = new char[need];
            if ( buffer ) {
                va_list  args;                                 // variable arg list
                va_start(args, format);                        // args start after format
                rc=vsnprintf(buffer,(unsigned int)need, format, args);
                va_end(args);                                  // free the args
            }
        }

        if ( rc > 0 ) result = std::string(buffer) ;
        if ( buffer ) delete[] buffer;                         // free buffer
        return result;
    }