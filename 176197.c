QString Utility::ChangeCase(const QString &text, const Utility::Casing &casing)
{
    if (text.isEmpty()) {
        return text;
    }

    switch (casing) {
        case Utility::Casing_Lowercase: {
            return text.toLower();
        }

        case Utility::Casing_Uppercase: {
            return text.toUpper();
        }

        case Utility::Casing_Titlecase: {
            // This is a super crude algorithm, could be replaced by something more clever.
            QString new_text = text.toLower();
            // Skip past any leading spaces
            int i = 0;

            while (i < text.length() && new_text.at(i).isSpace()) {
                i++;
            }

            while (i < text.length()) {
                if (i == 0 || new_text.at(i - 1).isSpace()) {
                    new_text.replace(i, 1, new_text.at(i).toUpper());
                }

                i++;
            }

            return new_text;
        }

        case Utility::Casing_Capitalize: {
            // This is a super crude algorithm, could be replaced by something more clever.
            QString new_text = text.toLower();
            // Skip past any leading spaces
            int i = 0;

            while (i < text.length() && new_text.at(i).isSpace()) {
                i++;
            }

            if (i < text.length()) {
                new_text.replace(i, 1, new_text.at(i).toUpper());
            }

            return new_text;
        }

        default:
            return text;
    }
}