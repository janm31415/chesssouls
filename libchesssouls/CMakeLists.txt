

if (WIN32)
  set(TBB_INCLUDE_DIR "C:/Program Files/TBB/include")
  set(TBB_LIBRARIES "C:/Program Files/TBB/lib/intel64/vc14/tbb.lib" "C:/Program Files/TBB/lib/intel64/vc14/tbb_debug.lib")
endif (WIN32)


if (UNIX)
  set(TBB_INCLUDE_DIR "/usr/local/include" "/usr/include")
  set(TBB_LIBRARIES "/usr/lib/x86_64-linux-gnu/libtbb.so" "/usr/lib/x86_64-linux-gnu/libtbb.so.2")
endif (UNIX)

set(HDRS
bitboard.h
hash.h
libchesssouls_api.h
movegen.h
notation.h
position.h
types.h
)
	
set(SRCS
bitboard.cpp
hash.cpp
rkiss.h
movegen.cpp
notation.cpp
position.cpp
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
    ${TBB_INCLUDE_DIR}
    )	
    
	
target_link_libraries(libchesssouls
    PRIVATE	
    ${TBB_LIBRARIES}
    )	
