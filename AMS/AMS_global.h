#ifndef AMS_GLOBAL_H
#define AMS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AMS_LIBRARY)
#  define AMSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define AMSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AMS_GLOBAL_H
