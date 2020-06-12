#pragma once

#ifdef _WIN32
#if defined(libchesssouls_EXPORTS)
#  define LIB_CHESSSOULS_API __declspec(dllexport)
#else
#  define LIB_CHESSSOULS_API __declspec(dllimport)
#endif
#else
#define LIB_CHESSSOULS_API
#endif