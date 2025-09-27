#ifdef NOB_MOD
#	ifdef NO_LINKERS
NOBDEF void _nob__go_rebuild_urself(int argc, char** argv, Nob_Cmd *cflags, const char* source_path, ...);
#		define CUSTOM_GO_REBUILD_URSELF(argc, argv, CFLAGS, SRC_PATH, ...) _nob__go_rebuild_urself(argc, argv, CFLAGS, SRC_PATH, NULL)
#	else
NOBDEF void _nob__go_rebuild_urself(int argc, char** argv, Nob_Cmd *cflags, Nob_Cmd *linkers, const char* source_path, ...);
#		define CUSTOM_GO_REBUILD_URSELF(argc, argv, CFLAGS, LINKERS, SRC_PATH, ...) _nob__go_rebuild_urself(argc, argv, CFLAGS, LINKERS, SRC_PATH, NULL)
#	endif
#	define NOB_SRC_GO_REBUILD_URSELF(argc, argv, SRC_PATH) nob__go_rebuild_urself(argc, argv, SRC_PATH, NULL)

#	ifdef NOB_IMPLEMENTATION

// The implementation idea is stolen from https://github.com/zhiayang/nabs
#		ifndef NO_LINKERS
NOBDEF void _nob__go_rebuild_urself(int argc, char** argv, Nob_Cmd *cflags, Nob_Cmd *linkers, const char* source_path, ...){
	const char *binary_path = nob_shift(argv, argc);
#			ifdef _WIN32
    // On Windows executables almost always invoked without extension, so
    // it's ./nob, not ./nob.exe. For renaming the extension is a must.
    if (!nob_sv_end_with(nob_sv_from_cstr(binary_path), ".exe")) {
        binary_path = nob_temp_sprintf("%s.exe", binary_path);
    }
#			endif

    Nob_File_Paths source_paths = {0};
    nob_da_append(&source_paths, source_path);
    va_list args;
    va_start(args, source_path);
    for (;;) {
        const char *path = va_arg(args, const char*);
        if (path == NULL) break;
        nob_da_append(&source_paths, path);
    }
    va_end(args);

    int rebuild_is_needed = nob_needs_rebuild(binary_path, source_paths.items, source_paths.count);
    if (rebuild_is_needed < 0) exit(1); // error
    if (!rebuild_is_needed) {           // no rebuild is needed
        NOB_FREE(source_paths.items);
        return;
    }

    Nob_Cmd cmd = {0};

    const char *old_binary_path = nob_temp_sprintf("%s.old", binary_path);

    if (!nob_rename(binary_path, old_binary_path)) exit(1);
	nob_cc(&cmd);
#			ifdef WFLAGS_PRESET
	nob_cc_flags(&cmd);
#			endif
	nob_cmd_extend(&cmd, cflags);
	nob_cc_output(&cmd, binary_path);
	nob_cc_inputs(&cmd, source_path);
	nob_cmd_extend(&cmd, linkers);
    //nob_cmd_append(&cmd, NOB_REBUILD_URSELF(binary_path, source_path));
    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_rename(old_binary_path, binary_path);
        exit(1);
    }
#			ifdef NOB_EXPERIMENTAL_DELETE_OLD
    // TODO: this is an experimental behavior behind a compilation flag.
    // Once it is confirmed that it does not cause much problems on both POSIX and Windows
    // we may turn it on by default.
    nob_delete_file(old_binary_path);
#			endif // NOB_EXPERIMENTAL_DELETE_OLD

    nob_cmd_append(&cmd, binary_path);
    nob_da_append_many(&cmd, argv, argc);
    if (!nob_cmd_run_sync_and_reset(&cmd)) exit(1);
    exit(0);
}

#		else
NOBDEF void _nob__go_rebuild_urself(int argc, char** argv, Nob_Cmd *cflags, const char* source_path, ...){
	const char *binary_path = nob_shift(argv, argc);
#			ifdef _WIN32
    // On Windows executables almost always invoked without extension, so
    // it's ./nob, not ./nob.exe. For renaming the extension is a must.
    if (!nob_sv_end_with(nob_sv_from_cstr(binary_path), ".exe")) {
        binary_path = nob_temp_sprintf("%s.exe", binary_path);
    }
#			endif

    Nob_File_Paths source_paths = {0};
    nob_da_append(&source_paths, source_path);
    va_list args;
    va_start(args, source_path);
    for (;;) {
        const char *path = va_arg(args, const char*);
        if (path == NULL) break;
        nob_da_append(&source_paths, path);
    }
    va_end(args);

    int rebuild_is_needed = nob_needs_rebuild(binary_path, source_paths.items, source_paths.count);
    if (rebuild_is_needed < 0) exit(1); // error
    if (!rebuild_is_needed) {           // no rebuild is needed
        NOB_FREE(source_paths.items);
        return;
    }

    Nob_Cmd cmd = {0};

    const char *old_binary_path = nob_temp_sprintf("%s.old", binary_path);

    if (!nob_rename(binary_path, old_binary_path)) exit(1);
	nob_cc(&cmd);
#			ifdef WFLAGS_PRESET
	nob_cc_flags(&cmd);
#			endif
	nob_cmd_extend(&cmd, cflags);
	nob_cc_output(&cmd, binary_path);
	nob_cc_inputs(&cmd, source_path);
    //nob_cmd_append(&cmd, NOB_REBUILD_URSELF(binary_path, source_path));
    if (!nob_cmd_run_sync_and_reset(&cmd)) {
        nob_rename(old_binary_path, binary_path);
        exit(1);
    }
#			ifdef NOB_EXPERIMENTAL_DELETE_OLD
    // TODO: this is an experimental behavior behind a compilation flag.
    // Once it is confirmed that it does not cause much problems on both POSIX and Windows
    // we may turn it on by default.
    nob_delete_file(old_binary_path);
#			endif // NOB_EXPERIMENTAL_DELETE_OLD

    nob_cmd_append(&cmd, binary_path);
    nob_da_append_many(&cmd, argv, argc);
    if (!nob_cmd_run_sync_and_reset(&cmd)) exit(1);
    exit(0);
}
#		endif
#	endif
#endif