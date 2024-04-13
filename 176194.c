QString Utility::getSpellingSafeText(const QString &raw_text)
{
    // There is currently a problem with Hunspell if we attempt to pass
    // words with smart apostrophes from the CodeView encoding.
    // Hunspell dictionaries typically store their main wordlist using
    // the dumb apostrophe variants only to save space and speed checking
    QString text(raw_text);
    return text.replace(QChar(0x2019),QChar(0x27));
}