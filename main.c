#include "sqlite/sqlite3.h"
#include <stdio.h>

// 抽出結果が返るコールバック関数
int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i = 0; i < argc; i++)
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  return 0; // success
}

int main(int argc, char* argv[]) {
  sqlite3 *db;
  char *zErrMsg;

  // データベースファイルを新規生成
  int rc = sqlite3_open("sample.db", &db);

  // テーブル生成SQL文
  char *create_sql
    = "create table sample(id integer primary key, worker text not null, age integer, place text)";
  // テーブルを生成
  rc = sqlite3_exec(db, create_sql, 0, 0, &zErrMsg);

  // データ追加
  sqlite3_exec(db, "insert into sample values(0,'IKD',30,'Tokyo')", 0, 0, &zErrMsg);
  sqlite3_exec(db, "insert into sample values(1,'Maru',null,null)", 0, 0, &zErrMsg);
  sqlite3_exec(db, "insert into sample values(2,'Nacky',35,'Hukuoka')", 0, 0, &zErrMsg);

  // "sample"テーブルの全項目を列挙
  rc = sqlite3_exec(db, "select * from sample", callback, 0, &zErrMsg);

  // データベースを閉じる
  sqlite3_close(db);

  return 0;
}
