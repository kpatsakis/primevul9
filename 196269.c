Formattable::getBogus() const 
{
    return (UnicodeString*)&fBogus; /* cast away const :-( */
}