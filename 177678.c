    static std::string indent(int d)
    {
    	std::string result ;
    	if ( d > 0 )
    		while ( d--)
    			result += "  ";
    	return result;
    }