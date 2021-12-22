#pragma once
#include "libchesssouls_api.h"

LIB_CHESSSOULS_API int WinAtChess_test();
LIB_CHESSSOULS_API int Kaufman_test();
LIB_CHESSSOULS_API int BratkoKopec_test();
LIB_CHESSSOULS_API int CCR_test();
LIB_CHESSSOULS_API int silent_but_deadly_test();


//reads extended position description files and tries
//to solve them
LIB_CHESSSOULS_API int test_epd(const char* filename);