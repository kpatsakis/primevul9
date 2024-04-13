bool Utility::IsMixedCase(const QString &string)
{
    if (string.isEmpty() || string.length() == 1) {
        return false;
    }

    bool first_char_lower = string[ 0 ].isLower();

    for (int i = 1; i < string.length(); ++i) {
        if (string[ i ].isLower() != first_char_lower) {
            return true;
        }
    }

    return false;
}