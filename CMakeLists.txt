# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# リリースビルド用オプション
SET(CMAKE_C_FLAGS_RELEASE "-Wall -O3")
# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g")
# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Release)

# プロジェクト名
PROJECT(HIT_TEST C)
# バージョン番号
SET(serial "1.0.0")

#
# 必須パッケージの確認
#
find_package(LIBXML2 REQUIRED)

#
# インクルードヘッダの確認
#
set (INCLUDE_DIRS "")
set (DEFINES )

if (LIBXML2_FOUND)
  set (INCLUDE_DIRS ${INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})
else (LIBXML2_FOUND)
  message ( FATAL_ERROR "Could not find Libxml2" )
endif (LIBXML2_FOUND)

set (LIVSQLITE_DIRS "sqlite")
set (INCLUDE_DIRS ${INCLUDE_DIRS} ${LIVSQLITE_DIRS})

set (LIVCSV_DIRS "libcsv")
set (INCLUDE_DIRS ${INCLUDE_DIRS} ${LIVCSV_DIRS})
include_directories(${INCLUDE_DIRS})

#
# コンパイルソースの定義
#
set ( main
  main.c
  sqlite/sqlite3.c
  libcsv/libcsv.c
  )

#
# ライブラリの確認
#
set (LINK_LIBRARIES )

if (LIBXML2_FOUND)
  set (LINK_LIBRARIES ${LINK_LIBRARIES} ${LIBXML2_LIBRARIES})
endif (LIBXML2_FOUND)
#set (LINK_LIBRARIES ${LINK_LIBRARIES} ${LIVCSV_DIRS})

# 実行ファイルの作成
add_executable (htest ${main})
target_link_libraries(htest ${LINK_LIBRARIES})

