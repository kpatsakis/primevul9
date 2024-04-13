    std::string indent(int32_t d)
    {
        std::string result ;
        if ( d > 0 )
            while ( d--)
                result += "  ";
        return result;
    }