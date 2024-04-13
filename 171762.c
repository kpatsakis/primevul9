usage(char *name)
{
    fprintf(stderr,
            _("usage: %s [-x db_args]* [-d dbpathname] [-r dbrealmname]\n"
              "\t\t[-R replaycachename] [-m] [-k masterenctype]\n"
              "\t\t[-M masterkeyname] [-p port] [-P pid_file]\n"
              "\t\t[-n] [-w numworkers] [/]\n\n"
              "where,\n"
              "\t[-x db_args]* - Any number of database specific arguments.\n"
              "\t\t\tLook at each database module documentation for "
              "\t\t\tsupported arguments\n"),
            name);
    exit(1);
}