    bool isBlank(std::string& s)
    {
        for ( std::size_t i = 0 ; i < s.length() ; i++ )
            if ( s[i] != ' ' )
                return false ;
        return true ;
    }