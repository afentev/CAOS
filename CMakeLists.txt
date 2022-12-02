cmake_minimum_required(VERSION 3.23)
project(CAOS LANGUAGES C ASM)

enable_language(ASM)

set(CMAKE_C_STANDARD 11)

#set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fsanitize=thread -g")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fpermissive -g -std=c11 -fmax-errors=10 -Wall -Wextra -Weffc++ -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wlarger-than=8192 -Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -fPIE -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -lm -pie")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -Wextra -pedantic -Wstrict-aliasing=1 -Wgnu-folding-constant")
#set_property(SOURCE "CAOS 2.0/10. asm-x86/hello-x86.S" PROPERTY LANGUAGE C)
set_source_files_properties("CAOS 2.0/10. asm-x86/arr.S" PROPERTIES COMPILE_FLAGS "-g3 -Fdwarf")

set(CMAKE_C_FLAGS "-O0 -g3")
#set(CMAKE_C_FLAGS "-m64 -masm=intel -g3")

#add_executable(CAOS "CAOS 2.0/5. processes-threads/posix-exec-cgi.c")
add_executable(CAOS "CAOS 2.0/10. asm-x86/arr.S" "CAOS 2.0/10. asm-x86/arr_get.c")
