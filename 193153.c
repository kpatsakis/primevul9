    bool operator () (Int64 *a, Int64 *b)
    {
        return ((*a > *b) || ((*a == *b) && (a > b)));
    }