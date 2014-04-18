# CMakeのバージョン
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# リリースビルド用オプション
SET(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -lxml2")
# デバッグビルド用オプション
SET(CMAKE_C_FLAGS_DEBUG "-g")
# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Release)

# プロジェクト名
PROJECT(HIT_TEST C)
# バージョン番号
SET(serial "1.0.0")

# ヘッダファイルの構成
#configure_file (
#  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
#  )

# インクルードヘッダ
include_directories(./sqlite/)
#include_directories(./libxml2/include/)

# for libxml2
#SET(libxml2_devel ${PROJECT_SOURCE_DIR}/libxml2/include/libxml)
#include_directories(${libxml2_devel}/xmlversion.h.in ${libxml2_devel}/xmlversion.h)

# リンクヘッダ
#link_directories(./sqlite/)

# 
find_package(libxml2 REQUIRED)
#find_package(libcsv REQUIRED)

# 実行ファイルの作成
add_executable (htest main.c sqlite/sqlite3.c libcsv/libcsv.c)
