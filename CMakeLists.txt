# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# リリースビルド用オプション
SET(CMAKE_C_FLAGS_RELEASE "-Wall -O4 -lsqlite3")
# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g")
# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Release)

# プロジェクト名
PROJECT(HIT_TEST C)
# バージョン番号
SET(serial "1.0.0")

# 実行ファイルの作成
add_executable (htest main.c)

IF (UNIX)
  ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
  SET(DISTCLEANED
    cmake.depends
    cmake.check_depends
    CMakeCache.txt
    cmake.check_cache
    *.cmake
    Makefile
    core core.*
    gmon.out
    *~
    )

  ADD_CUSTOM_COMMAND(
    DEPENDS clean
    COMMENT "distribution clean"
    COMMAND rm
    ARGS    -Rf CMakeTmp ${DISTCLEANED}
    TARGET  distclean
    )
ENDIF(UNIX)
