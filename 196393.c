UBool NumberFormatTest::equalValue(const Formattable& a, const Formattable& b) {
    if (a.getType() == b.getType()) {
        return a == b;
    }

    if (a.getType() == Formattable::kLong) {
        if (b.getType() == Formattable::kInt64) {
            return a.getLong() == b.getLong();
        } else if (b.getType() == Formattable::kDouble) {
            return (double) a.getLong() == b.getDouble(); // TODO check use of double instead of long
        }
    } else if (a.getType() == Formattable::kDouble) {
        if (b.getType() == Formattable::kLong) {
            return a.getDouble() == (double) b.getLong();
        } else if (b.getType() == Formattable::kInt64) {
            return a.getDouble() == (double)b.getInt64();
        }
    } else if (a.getType() == Formattable::kInt64) {
        if (b.getType() == Formattable::kLong) {
                return a.getInt64() == (int64_t)b.getLong();
        } else if (b.getType() == Formattable::kDouble) {
            return a.getInt64() == (int64_t)b.getDouble();
        }
    }
    return FALSE;
}