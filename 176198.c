QString Utility::DefinePrefsDir()
{
    // If the SIGIL_PREFS_DIR environment variable override exists; use it.
    // It's up to the user to provide a directory they have permission to write to.
    if (!SIGIL_PREFS_DIR.isEmpty()) {
        return SIGIL_PREFS_DIR;
    } else {
        return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    }
}