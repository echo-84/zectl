/*
 * Copyright (c) 2018, John Ramsden.
 * https://github.com/johnramsden/zectl/blob/master/LICENSE.md
 */

// Make sure libspl mnttab.h isn't imported, creates getmnttent conflict
#define _SYS_MNTTAB_H

#include <mntent.h>

#include "system_linux.h"
#include "util.h"

system_linux_error
system_linux_get_dataset(char *mountpoint, libze_handle_t *lzeh) {

    struct mntent *ent = NULL;
    system_linux_error ret = SYSTEM_ERR_SUCCESS;

    const char *mnt_location_file = "/proc/mounts";
    FILE *mnt_file = setmntent(mnt_location_file, "r");

    if (!mnt_file) {
        return SYSTEM_ERR_MNT_FILE;
    }

    do { /* Loop until root found or EOF */
        ent = getmntent(mnt_file);
    } while (ent != NULL && (strcmp(ent->mnt_dir, mountpoint) != 0));


    if (!ent) {
        ret = SYSTEM_ERR_NOT_FOUND;
        goto fin;
    }

    /* Found root, checking if zfs */
    if (strcmp(ent->mnt_type, "zfs") != 0) {
        ret = SYSTEM_ERR_WRONG_FSTYPE;
        goto fin;
    }

    if (copy_string(lzeh->rootfs, ent->mnt_fsname, ZE_MAXPATHLEN) != 0) {
        ret = SYSTEM_ERR_UNKNOWN;
        goto fin;
    }



fin:
    endmntent(mnt_file);
    return ret;
}