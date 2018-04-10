set(LLVM_LINK_COMPONENTS support)

add_clang_executable(match-global-variable
  MatchGlobalVariable.cpp
  )
target_link_libraries(match-global-variable PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )

