#include "utils.h"

#include <math.h>
#include <stdlib.h>

void* vec_alloc(void* vec, size_t* cap, const size_t size, const size_t elemSize)
{
  if (size == 0 || elemSize == 0)
    return vec;

  // Funnily enough, there's actually no condition needed to check whether to
  // resize or not since what we want is to have the minimum possible capacity
  // to hold size which can be achieved by taking the log2 of the current size,
  // flooring it then incrementing it by one.
  *cap = pow(2, floor(log2(size)) + 1);

  void* newVec = realloc(vec, *cap * elemSize);

  // If realloc returns NULL, make sure to free the old memory
  if (newVec == NULL)
    free(vec);

  return newVec;
}
