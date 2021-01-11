find . | grep -E "\.cpp$|\.h$" | grep -v CMakeFiles | xargs clang-format -i -verbose
