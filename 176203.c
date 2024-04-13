QStringList Utility::LinuxHunspellDictionaryDirs()
{
    QStringList paths;
    // prefer the directory specified by the env var SIGIL_DICTIONARIES above all else.
    if (!hunspell_dicts_override.isEmpty()) {
        // Handle multiple colon-delimited paths
        foreach (QString s, hunspell_dicts_override.split(":")) {
            paths << s.trimmed();
        }
    }
    // else use the env var runtime overridden 'share/sigil/hunspell_dictionaries/' location.
    else if (!sigil_extra_root.isEmpty()) {
        paths.append(sigil_extra_root + "/hunspell_dictionaries/");
    }
    // Bundled dicts were not installed use standard system dictionary location.
    else if (!dicts_are_bundled) {
        paths.append("/usr/share/hunspell");
        // Add additional hunspell dictionary directories. Provided at compile
        // time via the cmake option EXTRA_DICT_DIRS (colon separated list).
        if (!extra_dict_dirs.isEmpty()) {
            foreach (QString s, extra_dict_dirs.split(":")) {
                paths << s.trimmed();
            }
        }
    }
    else {
        // else use the standard build time 'share/sigil/hunspell_dictionaries/'location.
        paths.append(sigil_share_root + "/hunspell_dictionaries/");
    }
    return paths;
}