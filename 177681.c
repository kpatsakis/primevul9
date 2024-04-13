    static std::string indent(int depth)
    {
        std::string result;
        while ( depth -- ) result += "  ";
        return result;
    }