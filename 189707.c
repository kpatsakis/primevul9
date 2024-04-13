static const char *phase_name(int phase) {
    switch(phase) {
        case 1 :
            return "REQUEST_HEADERS";
            break;
        case 2 :
            return "REQUEST_BODY";
            break;
        case 3 :
            return "RESPONSE_HEADERS";
            break;
        case 4 :
            return "RESPONSE_BODY";
            break;
        case 5 :
            return "LOGGING";
            break;
    }
    
    return "INVALID";
}