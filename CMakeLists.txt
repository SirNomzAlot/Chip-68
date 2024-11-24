# HOW TO BUILD USING CMAKE as a standalone project:
# mkdir build
# cd build
# cmake .. -DCMAKE_TOOLCHAIN_FILE=~/Desktop/Retro68-build/toolchain/m68k-apple-macos/cmake/retro68.toolchain.cmake
# make

SET(CMAKE_CXX_FLAGS "-pedantic -Ofast")

add_application(Chip68
    src/chip68.c
    src/chip68.r
    src/c/core.c
    src/c/interpreter.c
    src/c/opcode.c
    src/c/screen.c
    src/c/textutils.c
  
    CREATOR "EKMN"
   )

    # make the result as small as possible
    # by removing unused code (gc-sections)
    # and by removing macsbug function names on 68K
    # (don't do this when debugging...)
set_target_properties(Chip68 PROPERTIES COMPILE_OPTIONS -ffunction-sections)
if(CMAKE_SYSTEM_NAME MATCHES Retro68)
    set_target_properties(Chip68 PROPERTIES LINK_FLAGS "-Wl,-gc-sections -Wl,--mac-strip-macsbug")
else()
    set_target_properties(Chip68 PROPERTIES LINK_FLAGS "-Wl,-gc-sections")
endif()
