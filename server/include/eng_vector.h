#ifndef __ENG_VECTOR_H__
#define __ENG_VECTOR_H__

#ifdef _USE_STD_CPP_LIB_

#include <vector>

#define EngVector std::vector

#else

#include <utils/Vector.h>

#define EngVector android::Vector

#endif

#endif