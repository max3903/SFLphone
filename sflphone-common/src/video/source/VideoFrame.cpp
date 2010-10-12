#include "VideoFrame.h"

#include <string.h>
#include <stdlib.h>

namespace sfl
{
VideoFrame::VideoFrame (const uint8_t* frame, const size_t size,
                        unsigned int depth, unsigned int height, unsigned int width)
{

    this->frame = (uint8_t*) malloc (size);
    memcpy (this->frame, frame, size);

    this->size = size;
    this->height = height;
    this->width = width;
    this->depth = depth;
}

VideoFrame::~VideoFrame()
{
    free (frame);
}
}
