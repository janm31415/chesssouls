
set(HDRS
bitboard.h
book.h
eval.h
eval_table.h
hash.h
king.h
libchesssouls_api.h
movegen.h
movepick.h
notation.h
pawn.h
perft.h
position.h
search.h
trans.h
types.h
)
	
set(SRCS
bitboard.cpp
book.cpp
eval.cpp
eval_table.cpp
hash.cpp
king.cpp
rkiss.h
movegen.cpp
movepick.cpp
notation.cpp
pawn.cpp
perft.cpp
position.cpp
search.cpp
trans.cpp
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif (WIN32)

# general build definitions
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_library(libchesssouls SHARED ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(libchesssouls
    PRIVATE
    )	
    
	
target_link_libraries(libchesssouls
    PRIVATE	
    )	
