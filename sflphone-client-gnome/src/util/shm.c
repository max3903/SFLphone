#include "shm.h"
#include "sflphone_const.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define WAIT_SHM_TIME_INCREMENT 1000000

sflphone_shm_t*
sflphone_shm_new ()
{
    return sflphone_shm_new_with_path ("");
}

sflphone_shm_t*
sflphone_shm_new_with_path (char* location)
{
    sflphone_shm_t* shm = (sflphone_shm_t*) malloc (sizeof (sflphone_shm_t));

    if (shm == NULL) {
        ERROR ("SharedMemory: Failed to create new sflphone shm type:  (%s)", strerror (errno));
        return NULL;
    }

    shm->path = strdup (location);
    shm->fd = 0;
    shm->addr = NULL;
    shm->size = 0;

    return shm;
}

int
sflphone_shm_set_path (sflphone_shm_t* shm, char* path)
{
    free (shm->path);
    shm->path = strdup (path);
}

int
sflphone_shm_free (sflphone_shm_t* shm)
{
    free (shm->path);
    free (shm);

    return 0;
}


int
sflphone_shm_ensure_non_zero (sflphone_shm_t* shm, useconds_t max_wait)
{
    // Open
    int shm_fd;

    if ( (shm_fd = shm_open (shm->path, O_RDWR, 0666)) < 0) {
        int rc = errno;

        if (errno != ENOENT) {
            ERROR ("SharedMemory: Cannot open shm segment (%s)", strerror (errno));
        }

        close (shm_fd);
        return -1;
    }

    // Make sure that the shm is non-zero.
    useconds_t time = 0;

    struct stat buffer;

    if (fstat (shm_fd, &buffer) < 0) {
        ERROR ("SharedMemory: Cannot get size: (%s)", strerror (errno));
    }

    while ( (buffer.st_size == 0) && (time + WAIT_SHM_TIME_INCREMENT) < max_wait) {
        usleep (WAIT_SHM_TIME_INCREMENT);
        time += WAIT_SHM_TIME_INCREMENT;

        if (fstat (shm_fd, &buffer) < 0) {
            ERROR ("SharedMemory: Cannot get size: (%s)", strerror (errno));
            return -1;
        }
    }

    // Close
    if (close (shm_fd) < 0) {
        ERROR ("SharedMemory: cannot close shm fd : (%s)", strerror (errno));
        return -1;
    }

    return 0;
}

/**
 * @param An existing shared memory segment structure.
 * @return The size in bytes of the file with file descriptor fd.
 */
static off_t
sflphone_shm_get_file_size (sflphone_shm_t* shm)
{
    struct stat buffer;

    if (fstat (shm->fd, &buffer) < 0) {
        ERROR ("SharedMemory: Cannot get size: (%s)", strerror (errno));
    }

    DEBUG ("shm (%s) is %d bytes long.", shm->path, buffer.st_size);
    return buffer.st_size;
}

off_t
sflphone_shm_get_size (sflphone_shm_t* shm)
{
    return shm->size;
}

/**
 * Attach a given shm into the process' address space.
 * @param shm The shared memory structure.
 */
static int
attach (sflphone_shm_t* shm)
{
    DEBUG ("SharedMemory: Attaching in read only mode to segment %s for %d bytes", shm->path, shm->size);

    if ( (shm->addr = mmap (NULL, shm->size, PROT_READ, MAP_SHARED, shm->fd,
                            (off_t) 0)) == MAP_FAILED) {
        ERROR ("cannot mmap shm segment (%s)", strerror (errno));
        close (shm->fd);
        return EINVAL;
    }
}

/**
 * Unmap the shm.
 * @param shm The shared memory structure.
 */
static int
release (sflphone_shm_t* shm)
{
    if (munmap (shm->addr, shm->size) < 0) {
        ERROR ("SharedMemory: cannot release shm segment: %s", strerror (errno));
    }

    shm->addr = NULL;
    shm->size = 0;

    return 0;
}

int
sflphone_shm_open (sflphone_shm_t* shm)
{
    DEBUG ("SharedMemory: Opening shared memory segment (%s)", shm->path);

    // Open
    int shm_fd;

    if ( (shm_fd = shm_open (shm->path, O_RDWR, 0666)) < 0) {
        int rc = errno;

        if (errno != ENOENT) {
            ERROR ("SharedMemory: cannot open existing shm segment (%s)", strerror (errno));
        }

        close (shm_fd);
        return rc;
    }

    // Attach
    shm->fd = shm_fd;
    shm->size = sflphone_shm_get_file_size (shm);
    DEBUG ("SharedMemory: In sflphone_shm_open, file size is %d", shm->size);

    if (attach (shm) < 0) {
        ERROR ("SharedMemory: attach() failed in sflphone_shm_open");
        return -1;
    }

    return 0;
}

int
sflphone_shm_close (sflphone_shm_t* shm)
{
    if (close (shm->fd) < 0) {
        ERROR ("SharedMemory: cannot close shm fd : (%s)", strerror (errno));
    }

    shm->fd = 0;

    return release (shm);
}

void *
sflphone_shm_get_addr (sflphone_shm_t* shm)
{
    return shm->addr;
}
