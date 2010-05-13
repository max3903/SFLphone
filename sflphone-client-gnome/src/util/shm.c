#include "shm.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

sflphone_shm_t* sflphone_shm_new(char* location)
{
  sflphone_shm_t* shm = (sflphone_shm_t*) malloc(sizeof(sflphone_shm_t));
  if (shm == NULL) {
    ERROR("Failed to create new sflphone shm type:  (%s)", strerror(errno));
    return NULL;
  }

  shm->path = strdup(location);
  shm->fd = 0;
  shm->addr = NULL;
  shm->size = 0;

  return shm;
}

int sflphone_shm_free(sflphone_shm_t* shm)
{
  free(shm->path);
  free(shm);

  return 0;
}

/**
 * @param fd The file descriptor.
 * @return The size in bytes of the file with file descriptor fd.
 */
static off_t sflphone_get_size_video_shm(int fd)
{
        assert(fd);

        struct stat buffer;
        if (fstat(fd, &buffer) < 0) {
            ERROR("Cannot get size: (%s)", strerror(errno));
        }

        return buffer.st_size;
}

/**
 * Attach a given shm into the process' address space.
 * @param shm The shared memory structure.
 */
static int attach(sflphone_shm_t* shm)
{
  if ((shm->addr = mmap(0, shm->size, PROT_READ, MAP_SHARED, shm->fd, 0)) == MAP_FAILED)
    {
      ERROR ("cannot mmap shm segment (%s)", strerror (errno));
      close (shm->fd);
      return EINVAL;
    }
}

/**
 * Unmap the shm.
 * @param shm The shared memory structure.
 */
static int release(sflphone_shm_t* shm)
{
  if (munmap(shm->addr, shm->size) < 0) {
      ERROR ("cannot release shm segment: %s", strerror(errno));
    }

    shm->addr = NULL;
    shm->size = 0;

    return 0;
}

int sflphone_shm_open (sflphone_shm_t* shm)
{
  // Open
  int shm_fd;
  if ((shm_fd = shm_open (shm->path, O_RDWR, 0666)) < 0)
    {
      int rc = errno;
      if (errno != ENOENT)
        {
          ERROR ("cannot open existing shm segment (%s)", strerror (errno));
        }
      close (shm_fd);
      return rc;
    }

  // Attach
  shm->fd = shm_fd;
  shm->size = get_size(shm->fd);
  if (attach(shm) < 0) {
    return -1;
  }

  return 0;
}

int sflphone_shm_close(sflphone_shm_t* shm)
{
  if (close(shm->fd) < 0) {
    ERROR ("cannot close shm fd : (%s)", strerror(errno));
  }

  shm->fd = 0;

  return release(shm);
}

void * sflphone_shm_get_addr(sflphone_shm_t* shm)
{
  return shm->addr;
}
