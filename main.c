#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "csv.h"
#include "sqlite/sqlite3.h"

/**************************************
 * LIBXML2 HEADERS
 **************************************/
//#include <libxml/DOCBparser.h>
//#include <libxml/HTMLparser.h>
//#include <libxml/HTMLtree.h>
//#include <libxml/SAX.h>
//#include <libxml/SAX2.h>
//#include <libxml/c14n.h>
//#include <libxml/catalog.h>
//#include <libxml/chvalid.h>
//#include <libxml/debugXML.h>
//#include <libxml/dict.h>
//#include <libxml/encoding.h>
//#include <libxml/entities.h>
//#include <libxml/globals.h>
//#include <libxml/hash.h>
//#include <libxml/list.h>
//#include <libxml/nanoftp.h>
//#include <libxml/nanohttp.h>
#include <libxml/parser.h>
//#include <libxml/parserInternals.h>
//#include <libxml/pattern.h>
//#include <libxml/relaxng.h>
//#include <libxml/schemasInternals.h>
//#include <libxml/schematron.h>
//#include <libxml/threads.h>
#include <libxml/tree.h>
//#include <libxml/uri.h>
#include <libxml/valid.h>
//#include <libxml/xinclude.h>
//#include <libxml/xlink.h>
//#include <libxml/xmlIO.h>
//#include <libxml/xmlautomata.h>
//#include <libxml/xmlerror.h>
//#include <libxml/xmlexports.h>
//#include <libxml/xmlmemory.h>
//#include <libxml/xmlmodule.h>
//#include <libxml/xmlreader.h>
//#include <libxml/xmlregexp.h>
//#include <libxml/xmlsave.h>
//#include <libxml/xmlschemas.h>
//#include <libxml/xmlschemastypes.h>
//#include <libxml/xmlstring.h>
//#include <libxml/xmlunicode.h>
//#include <libxml/xmlversion.h>
//#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
//#include <libxml/xpointer.h>


typedef struct htest {
  char name[100];
  int position;
} htest_t;


//
int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i = 0; i < argc; i++)
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  return 0; // success
}

static const char *result = "<list><people>a</people><people>b</people></list>";
static const char *cur = NULL;
static int rlen;

/**
 * sqlMatch:
 * @URI: an URI to test
 *
 * Check for an sql: query
 *
 * Returns 1 if yes and 0 if another Input module should be used
 */
static int
sqlMatch(const char * URI) {
  if ((URI != NULL) && (!strncmp(URI, "sql:", 4)))
    return(1);
  return(0);
}

/**
 * sqlOpen:
 * @URI: an URI to test
 *
 * Return a pointer to the sql: query handler, in this example simply
 * the current pointer...
 *
 * Returns an Input context or NULL in case or error
 */
static void *
sqlOpen(const char * URI) {
  if ((URI == NULL) || (strncmp(URI, "sql:", 4)))
    return(NULL);
  cur = result;
  rlen = strlen(result);
  return((void *) cur);
}

/**
 * sqlClose:
 * @context: the read context
 *
 * Close the sql: query handler
 *
 * Returns 0 or -1 in case of error
 */
static int
sqlClose(void * context) {
  if (context == NULL) return(-1);
  cur = NULL;
  rlen = 0;
  return(0);
}

/**
 * sqlRead:
 * @context: the read context
 * @buffer: where to store data
 * @len: number of bytes to read
 *
 * Implement an sql: query read.
 *
 * Returns the number of bytes read or -1 in case of error
 */
static int
sqlRead(void * context, char * buffer, int len) {
  const char *ptr = (const char *) context;

  if ((context == NULL) || (buffer == NULL) || (len < 0))
    return(-1);

  if (len > rlen) len = rlen;
  memcpy(buffer, ptr, len);
  rlen -= len;
  return(len);
}

const char *include = "<?xml version='1.0'?>\n\
                       <document xmlns:xi=\"http://www.w3.org/2003/XInclude\">\n\
                       <p>List of people:</p>\n\
                       <xi:include href=\"sql:select_name_from_people\"/>\n\
                       </document>\n";


#define CSV_END 0
#define CSV_COL 1
#define CSV_ROW 2
#define CSV_ERR 3

#define DO_TEST(name, options) test_parser("test" #name, options, test ## name ## _data, \
    sizeof(test ## name ## _data) - 1, test ## name ## _results, \
    CSV_COMMA, CSV_QUOTE, NULL, NULL)

#define DO_TEST_CUSTOM(name, options, d, q, s, t) test_parser("custom" #name, options, custom ## name ## _data, \
    sizeof(custom ## name ## _data) - 1, custom ## name ## _results, d, q, s, t)

struct event {
  int event_type; 
  int retval;
  size_t size;  /* Number of bytes in this event or return value from csv_fini */
  char *data;   /* Data for this event */
};

struct event *event_ptr;
int event_idx;
size_t row, col;

  void
fail_parser (char * test_name, char * message)
{
  fprintf(stderr, "Parser test %s failed on event %d: %s\n", test_name, event_idx, message);
  exit(EXIT_FAILURE);
}

  void
fail_writer (char * test_name, char * message)
{
  fprintf(stderr, "Writer test %s failed: %s\n", test_name, message);
  exit(EXIT_FAILURE);
}

  void
cb1 (void *data, size_t len, void *t)
{
//  char * test_name = t;
//
//  /* Make sure we were expecting a column */
//  if (event_ptr->event_type != CSV_COL)
//    fail_parser(test_name, "didn't expect a column");
//
//  /* Check the actual size against the expected size */
//  if (event_ptr->size != len)
//    fail_parser(test_name, "actual data length doesn't match expected data length");
//
//  /* Check the actual data against the expected data */
//  if ((event_ptr->data == NULL || data == NULL)) {
//    if (event_ptr->data != data)
//      fail_parser(test_name, "actual data doesn't match expected data");
//  }
//  else if (memcmp(event_ptr->data, data, len) != 0)
//    fail_parser(test_name, "actual data doesn't match expected data");
//
//  event_idx++;
//  event_ptr++;
  col++;
}

  void
cb2 (int c, void *t)
{
//  char * test_name = t;
//
//  /* Make sure we were expecting an the end of a row */
//  if (event_ptr->event_type != CSV_ROW)
//    fail_parser(test_name, "didn't expect end of row");
//
//  /* Check that the row ended with the character we expected */
//  if (event_ptr->retval != c)
//    fail_parser(test_name, "row ended with unexpected character");
//
//  event_idx++;
//  event_ptr++;
  col = 1;
  row++;
}

  void
test_parser2 (char *test_name, unsigned char options, void *input, size_t len,
    char delimiter, char quote, int (*space_func)(unsigned char), int (*term_func)(unsigned char))
{
  fprintf(stdout, "test_parser2\n");


  int result = 0;
  size_t retval;
  struct csv_parser p;
  size_t size;

  for (size = 1; size <= len; size++) {
    size_t bytes_processed = 0;
    csv_init(&p, options);
    csv_set_delim(&p, delimiter);
    csv_set_quote(&p, quote);
    csv_set_space_func(&p, space_func);
    csv_set_term_func(&p, term_func);

    row = col = 1;

    do {
      size_t bytes = size < len - bytes_processed ? size : len - bytes_processed;
      retval = csv_parse(&p, input + bytes_processed, bytes, cb1, cb2, test_name);
      if (retval != bytes) {
        csv_free(&p);
        fprintf(stdout, "a\n");
        return;
      }

      bytes_processed += bytes;
    } while (bytes_processed < len);

    result = csv_fini(&p, cb1, cb2, test_name);

    if (result != 0) {
      csv_free(&p);
      fprintf(stdout, "b\n");
      return;
    }

    csv_free(&p);
  }
  fprintf(stdout, "c\n");
}

  void
test_parser (char *test_name, unsigned char options, void *input, size_t len, struct event expected[],
    char delimiter, char quote, int (*space_func)(unsigned char), int (*term_func)(unsigned char))
{
  int result = 0;
  size_t retval;
  struct csv_parser p;
  size_t size;

  for (size = 1; size <= len; size++) {
    size_t bytes_processed = 0;
    csv_init(&p, options);
    csv_set_delim(&p, delimiter);
    csv_set_quote(&p, quote);
    csv_set_space_func(&p, space_func);
    csv_set_term_func(&p, term_func);

    row = col = 1;
    event_ptr = &expected[0];
    event_idx = 1;

    do {
      size_t bytes = size < len - bytes_processed ? size : len - bytes_processed;
      retval = csv_parse(&p, input + bytes_processed, bytes, cb1, cb2, test_name);
      if (retval != bytes) {
        if (event_ptr->event_type != CSV_ERR) {
          fail_parser(test_name, "unexpected parse error occured");
        } else {
          csv_free(&p);
          return;
        }
      }

      bytes_processed += bytes;
    } while (bytes_processed < len);

    result = csv_fini(&p, cb1, cb2, test_name);

    if (result != 0) {
      if (event_ptr->event_type != CSV_ERR) {
        fail_parser(test_name, "unexpected parse error occured");
      } else {
        csv_free(&p);
        return;
      }
    }

    csv_free(&p);

    if (event_ptr->event_type != CSV_END)
      fail_parser(test_name, "unexpected end of input");
  }
}

  void
test_writer (char * test_name, char *input, size_t input_len, char *expected, size_t expected_len)
{
  size_t actual_len;
  char *temp = malloc(input_len*2 + 2);
  if (!temp) {
    fprintf(stderr, "Failed to allocate memory in test_writer!\n");
    exit(EXIT_FAILURE);
  }

  actual_len = csv_write(temp, input_len*2 + 2, input, input_len);
  if (actual_len != expected_len)
    fail_writer(test_name, "actual length doesn't match expected length");
  if (memcmp(temp, expected, actual_len) != 0)
    fail_writer(test_name, "actual data doesn't match expected data");
}

  void
test_writer2 (char * test_name, char *input, size_t input_len, char *expected, size_t expected_len, char quote)
{
  size_t actual_len;
  char *temp = malloc(input_len*2 + 2);
  if (!temp) {
    fprintf(stderr, "Failed to allocate memory in test_writer!\n");
    exit(EXIT_FAILURE);
  }

  actual_len = csv_write2(temp, input_len*2 + 2, input, input_len, quote);
  if (actual_len != expected_len)
    fail_writer(test_name, "actual length doesn't match expected length");
  if (memcmp(temp, expected, actual_len) != 0)
    fail_writer(test_name, "actual data doesn't match expected data");
}



void trim(char *str)
{
  int i;

  for (i = strlen(str) - 1; 0 <= i; i--) {
    if ((str[i] == '\r') ||
        (str[i] == '\n') ||
        (str[i] == ' ')) {
      str[i] = '\0';
    }
    else {
      break;
    }
  }
}

void print_attr(xmlAttrPtr attr, FILE *output)
{
  xmlAttrPtr cur_attr = NULL;
  for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
    fprintf(output, "attr %s=%s\n", attr->name, attr->children->content);
  }
}


void print_nodes(xmlNodeSetPtr nodes, FILE *output)
{
  xmlNodePtr cur;
  int        size;
  int        i;
  char      *content;
  char       buf[128];

  size = (nodes) ? nodes->nodeNr : 0;
  fprintf(output, "Result (%d nodes):\n", size);
  for(i = 0; i < size; ++i) {
    if(nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
      xmlNsPtr ns;

      ns = (xmlNsPtr)nodes->nodeTab[i];
      cur = (xmlNodePtr)ns->next;
      if(cur->ns) {
        fprintf(output,
            "= namespace \"%s\"=\"%s\" for node %s:%s",
            ns->prefix,
            ns->href, cur->ns->href, cur->name);
      }
      else {
        fprintf(output,
            "= namespace \"%s\"=\"%s\" for node %s",
            ns->prefix, ns->href, cur->name);
      }
    }
    else if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];
      if(cur->ns) {
        fprintf(output,
            "= element node \"%s:%s\"",
            cur->ns->href, cur->name);
      }
      else {
        fprintf(output, "= element node \"%s\"",
            cur->name);
      }
    }
    else {
      cur = nodes->nodeTab[i];
      fprintf(output, "= node \"%s\": type %d: ", cur->name, cur->type);
    }

    content = (char *)cur->children->content;
    if (NULL != content) {
      strcpy(buf, content);
      trim(buf);
      fprintf(output, " content %s\n", buf);
    }
    else {
      fprintf(output, " content (null)\n");
    }
    print_attr(cur->properties, output);
  }
}




void printErrorMessageSqlite(int errorNo)
{
  switch(errorNo) {
    case SQLITE_OK:         fprintf(stderr, "* SQLITE ERROR: 成功\n"); break;
    case SQLITE_ERROR:      fprintf(stderr, "* SQLITE ERROR: SQLエラー、もしくはデータベースが見つかりません\n"); break;
    case SQLITE_INTERNAL:   fprintf(stderr, "* SQLITE ERROR: SQLiteの内部ロジックエラー\n"); break;
    case SQLITE_PERM:       fprintf(stderr, "* SQLITE ERROR: アクセス権がありません\n"); break;
    case SQLITE_ABORT:      fprintf(stderr, "* SQLITE ERROR: コールバック関数がアボートを要求した\n"); break;
    case SQLITE_BUSY:       fprintf(stderr, "* SQLITE ERROR: データベースがロックされています\n"); break;
    case SQLITE_LOCKED:     fprintf(stderr, "* SQLITE ERROR: テーブルがロックされています\n"); break;
    case SQLITE_NOMEM:      fprintf(stderr, "* SQLITE ERROR: malloc()に失敗しました\n"); break;
    case SQLITE_READONLY:   fprintf(stderr, "* SQLITE ERROR: 読み取り専用のデータベースに書き込もうとしました\n"); break;
    case SQLITE_INTERRUPT:  fprintf(stderr, "* SQLITE ERROR: オペレーションがsqlite_interrupt()によって終了された\n"); break;
    case SQLITE_IOERR:      fprintf(stderr, "* SQLITE ERROR: ディスクI/Oエラー\n"); break;
    case SQLITE_CORRUPT:    fprintf(stderr, "* SQLITE ERROR: データベースディスクイメージがmalformed\n"); break;
    case SQLITE_NOTFOUND:   fprintf(stderr, "* SQLITE ERROR: (Internal Only) テーブルかレコードが見つかりません\n"); break;
    case SQLITE_FULL:       fprintf(stderr, "* SQLITE ERROR: データベースにはこれ以上追加できません\n"); break;
    case SQLITE_CANTOPEN:   fprintf(stderr, "* SQLITE ERROR: データベースファイルが開けませんでした\n"); break;
    case SQLITE_PROTOCOL:   fprintf(stderr, "* SQLITE ERROR: データベースロックプロトコルエラー\n"); break;
    case SQLITE_EMPTY:      fprintf(stderr, "* SQLITE ERROR: (Internal Only) テーブルが空でした\n"); break;
    case SQLITE_SCHEMA:     fprintf(stderr, "* SQLITE ERROR: データベースのスキーマが変更されました\n"); break;
    case SQLITE_TOOBIG:     fprintf(stderr, "* SQLITE ERROR: Too much data for one row of a table\n"); break;
    case SQLITE_CONSTRAINT: fprintf(stderr, "* SQLITE ERROR: contraint violationのため強制終了しました\n"); break;
    case SQLITE_MISMATCH:   fprintf(stderr, "* SQLITE ERROR: データ型が間違ってます\n"); break;
    case SQLITE_MISUSE:     fprintf(stderr, "* SQLITE ERROR: ライブラリの使い方が間違ってます\n"); break;
    case SQLITE_NOLFS:      fprintf(stderr, "* SQLITE ERROR: Uses OS features not supported on host\n"); break;
    case SQLITE_AUTH:       fprintf(stderr, "* SQLITE ERROR: Authorization denied\n"); break;
    case SQLITE_ROW:        fprintf(stderr, "* SQLITE ERROR: sqlite_step() has another row ready\n"); break;
    case SQLITE_DONE:       fprintf(stderr, "* SQLITE ERROR: sqlite_step() has finished executing\n"); break;
  }
}


void printErrorMessage(int errorNo)
{

  fprintf(stderr, "\n");
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*\n");

  switch (errorNo) {
    case 1:
      fprintf(stderr, "* DTDエラーが発生しています。XML/DTDの見直しを行ってください。\n");
      break;

    case 2:
      fprintf(stderr, "* Error: unable to create new XPath context.\n");
      break;

    case 3:
      fprintf(stderr, "* Error: unable to evaluate xpath expression.\n");
      break;

    case 4:
      fprintf(stderr, "* Error: CSVファイルを開くことができません\n");
      fprintf(stderr, "*        CSVファイル名はDATA_A.csvとしてください\n");
      break;

    default:
      fprintf(stderr, "* 不明なエラーが発生しています。\n");
  }

  // only sqlite
  if (errorNo > 10000) {
    printErrorMessageSqlite(errorNo - 10000);
  }

  fprintf(stderr, "* エラーコード：%05d\n", errorNo);
  fprintf(stderr, "*\n");
  fprintf(stderr, "**************************************************************\n");
}

/*
 *
 * MAIN
 *
 * ToDo:XML/DTDファイルを引数で渡す
 */
int main (void) {

  /*
   *
   * XMLを読み込み & チェック
   *
   */
  int valid;

  xmlValidCtxtPtr cvp;
  cvp = xmlNewValidCtxt();

  xmlDocPtr doc;
  doc = xmlReadFile("signal.xml", NULL, XML_PARSE_NOERROR|XML_PARSE_NOWARNING);

  xmlDtdPtr dtd;
  dtd = xmlParseDTD(NULL,(xmlChar *)"signal.dtd" );

  // 成功時1,失敗時1以外
  // エラーコメントを抑制できない
  valid = xmlValidateDtd(cvp, doc, dtd); 

  if (valid != 1) {
    printErrorMessage(1);
    return EXIT_FAILURE;
  }

  //
  // ごりごりバージョン
  // もう使わないと思うけど、メモがてら残す
  // 
  //// ROOT
  //xmlNodePtr rootNode;
  //rootNode = xmlDocGetRootElement(doc);
  //printf("node name1:%s\n", rootNode->name);
  //printf("node name1:%d\n", rootNode->type);
  //
  //// Signals
  //xmlNodePtr SignalsNode;
  //SignalsNode = rootNode->children->next->next->next;
  //printf("node name2:%s\n", SignalsNode->name);
  //printf("node name2:%d\n", SignalsNode->type);
  //
  //// Signal
  //xmlNodePtr curNode = SignalsNode->children->next;
  //while(curNode != NULL) {
  //  xmlNodePtr SignalNode;
  //  SignalNode = SignalsNode->children->next;
  //  printf("node name3:%s\n", SignalNode->name);
  //  printf("node name3:%d\n", SignalNode->type);
  //
  //  curNode = curNode->next;
  //}


  xmlXPathContextPtr xpath_context;
  xmlXPathObjectPtr  xpath_obj;
  xmlChar *xpath_expr = (xmlChar *)"/TestProperties/Signals/Signal";
  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(doc);
  if(xpath_context == NULL) {
    //fprintf(stderr,"Error: unable to create new XPath context\n");
    printErrorMessage(2);
    xmlFreeDoc(doc);
    return EXIT_FAILURE;
  }

  /* Evaluate xpath expression */
  xpath_obj = xmlXPathEvalExpression(xpath_expr, xpath_context);
  if(xpath_obj == NULL) {
    //fprintf(stderr,
    //    "Error: unable to evaluate xpath expression \"%s\"\n",
    //    xpath_expr);
    printErrorMessage(3);
    xmlXPathFreeContext(xpath_context);
    xmlFreeDoc(doc);
    return EXIT_FAILURE;
  }

  /* Print results */
  xmlNodeSetPtr nodes = xpath_obj->nodesetval;
#ifdef DEBUG
  print_nodes(nodes, stdout);
#endif

  /*
   *
   * Signalノード抜き出し
   *
   */
  xmlNodePtr cur;
  int        size;
  int        i;

  size = (nodes) ? nodes->nodeNr : 0;
  htest_t ht[size];

#ifdef DEBUG
  fprintf(stdout, "Result (%d nodes):\n", size);
#endif
  for(i = 0; i < size; ++i) {
    if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];

      if(!cur->ns)
      {
#ifdef DEBUG
        fprintf(stdout, "= element node \"%s\"", cur->name);
#endif
      }

    }
    xmlAttrPtr attr = cur->properties;
    xmlAttrPtr cur_attr = NULL;
    for (cur_attr = attr; cur_attr; cur_attr = cur_attr->next) {
#ifdef DEBUG
      fprintf(stdout, " attr %s=%s\n", cur_attr->name, cur_attr->children->content);
#endif
      if (!strcmp((char *)cur_attr->name, "position")) {
        ht[i].position = atoi((char *)cur_attr->children->content);
      } else if (!strcmp((char *)cur_attr->name, "sqlName")) {
        sprintf(ht[i].name, "%s", cur_attr->children->content);
      }
    }

#ifdef DEBUG
    fprintf(stdout, "name=%s ,attr=%d\n", ht[i].name, ht[i].position);
#endif
  }

  /* Cleanup */
  xmlXPathFreeObject(xpath_obj);
  xmlXPathFreeContext(xpath_context);

  xmlFreeDtd(dtd);
  xmlFreeDoc(doc);

  /*
   *
   * SQLiteのテーブル作成
   *
   */
  sqlite3 *db;
  char *zErrMsg;
  char dbName[100];

  sprintf(dbName, "signal_%ld.sqlite", time(NULL));

  // データベースファイルを新規生成
  int rc = sqlite3_open(dbName, &db);

  // テーブル生成SQLテンプレート
  char *template_sql
    = "create table %s(id integer primary key autoincrement, status integer not null, time integer not null)";

  char create_table[200];
  for (int i = 0; i < size; i++) {
    // テーブル生成SQL文
    sprintf(create_table, template_sql, ht[i].name);
    // テーブルを生成
    rc = sqlite3_exec(db, create_table, 0, 0, &zErrMsg);
    if (rc > 0) {
      printErrorMessage(rc + 10000);
      return EXIT_FAILURE;
    }
  }

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




  /*
   *
   * CSVファイル読み込み、dbへの登録
   *
   */
  FILE *fp;
  char line[512];

  //ファイルの存在チェック
  if ((fp = fopen("DATA_A.csv", "r")) == NULL) {
    printErrorMessage(4);
    return EXIT_FAILURE;
  }     

  /* ファイルの終わりに達するまで */
  while (fgets(line, sizeof(line), fp) != NULL) {

    int result = 0;
    size_t retval;
    struct csv_parser p;
    size_t tmp_size;

    int line_len = strlen(line) - 1;

    //char test00_data[] = " 1,2 ,  3         ,4,5\x0d\x0a";
    //struct event test00_results[] =
    //{ {CSV_COL, 0, 1, "1"},
    //  {CSV_COL, 0, 1, "2"},
    //  {CSV_COL, 0, 1, "3"},
    //  {CSV_COL, 0, 1, "4"},
    //  {CSV_COL, 0, 1, "5"},
    //  {CSV_ROW, '\x0d', 1, NULL}, {CSV_END, 0, 0, NULL} };
    //test_parser("test", 0, test00_data, sizeof(test00_data) - 1, test00_results, CSV_COMMA, CSV_QUOTE, NULL, NULL);
    //test_parser2("test", 0, test00_data, sizeof(test00_data) - 1, CSV_COMMA, CSV_QUOTE, NULL, NULL);

    for (tmp_size = 1; tmp_size <= line_len; tmp_size++) {
      size_t bytes_processed = 0;
      csv_init(&p, CSV_STRICT);
      csv_set_delim(&p, CSV_COMMA);
      csv_set_quote(&p, CSV_QUOTE);
      csv_set_space_func(&p, NULL);
      csv_set_term_func(&p, NULL);

      row = col = 1;

      //  DO_TEST(01, 0);
      //#define DO_TEST(name, options) test_parser("test" #name, options, test ## name ## _data, \
      //    sizeof(test ## name ## _data) - 1, test ## name ## _results, \
      //    CSV_COMMA, CSV_QUOTE, NULL, NULL)
      //
      //test_parser (char *test_name, unsigned char options, void *input, size_t len, struct event expected[],
      //    char delimiter, char quote, int (*space_func)(unsigned char), int (*term_func)(unsigned char))
      //
      //char test01_data[] = " 1,2 ,  3         ,4,5\x0d\x0a";

      do {
        size_t bytes = tmp_size < line_len - bytes_processed ? tmp_size : line_len - bytes_processed;
        retval = csv_parse(&p, line + bytes_processed, bytes, cb1, cb2, "");
        if (retval != bytes) {
          fprintf(stdout, "break\n");
          //終わり
          break;
        }
        bytes_processed += bytes;
      } while (bytes_processed < line_len);

      result = csv_fini(&p, cb1, cb2, "");

      if (result != 0) {
        //エラー
      }

    }
    // TODO: データを入れ込む
    fprintf(stdout, "data:%s\n", p.entry_buf);

    csv_free(&p);
  }

  // CSVファイルおわり
  fclose(fp);









  ///* Parser Tests */

  ///* Parser tests presented here consist of two parts:
  //   1. the raw data to be parsed as an array of char
  //   2. the behavior expected from attempting to parse the data

  //   The latter is encapsulated into an array of struct event, each event
  //   described the type of event that was expected at that point and details
  //   of the event.  There are four event types:
  //   1. CSV_END - signifies the successful end of parsing, no events should
  //   follow in this test
  //   2. CSV_COL - signifies that a column or CSV element is expected to be
  //   submitted to the appropriate callback function
  //   3. CSV_ROW - specifies the end of a row has been reached
  //   4. CSV_ERR - signifies that an error is expected at this point, no
  //   further events should follow in this test

  //   In addition to the event type, an event is also characterized by a size
  //   and a data member in the event structure.  The meaning of these fields
  //   depends on the event type.

  //   The size member for CSV_COL is the size of the expected data column,
  //   for CSV_ROW is it the size of the terminating character which should
  //   always be 1.  The size for CSV_END should always be zero, for CSV_ERR
  //   the size is not used.

  //   The data member represents the actual data after parsing for a CSV_COL,
  //   the terminating character for CSV_ROW (the first character of the provided
  //   char array is the one used), and is not used for CSV_END or CSV_ERR, it
  //   should be NULL in these cases.
  //   */


  //char test01_data[] = " 1,2 ,  3         ,4,5\x0d\x0a";
  //char test02_data[] = ",,,,,\x0a";
  //char test03_data[] = "\",\",\",\",\"\"";
  //char test04_data[] = "\"I call our world Flatland,\x0a"
  //  "not because we call it so,\x0a"
  //  "but to make its nature clearer\x0a"
  //  "to you, my happy readers,\x0a"
  //  "who are privileged to live in Space.\"";
  //char test05_data[] = "\"\"\"a,b\"\"\",,\" \"\"\"\" \",\"\"\"\"\" \",\" \"\"\"\"\",\"\"\"\"\"\"";
  //char test06_data[] = "\" a, b ,c \", a b  c,";
  //char test07_data[] = "\" \"\" \" \" \"\" \"";
  //char test07b_data[] = "\" \"\" \" \" \"\" \"";
  //char test08_data[] = "\" abc\"                                             "
  //  "                                                     "
  //  "                                                     "
  //  "                                                     "
  //  "                                                     "
  //  "                                                     "
  //  "                                                     "
  //  "                                                     "
  //  "                                          \", \"123\"";
  //char test09_data[] = "";
  //char test10_data[] = "a\x0a";
  //char test11_data[] = "1,2 ,3,4\x0a";
  //char test12_data[] = "\x0a\x0a\x0a\x0a";
  //char test12b_data[] = "\x0a\x0a\x0a\x0a";
  //char test13_data[] = "\"abc\"";
  //char test14_data[] = "1, 2, 3,\x0a\x0d\x0a  \"4\", \x0d,";
  //char test15_data[] = "1, 2, 3,\x0a\x0d\x0a  \"4\", \x0d\"\"";
  //char test16_data[] = "\"1\",\"2\",\" 3 ";
  //char test16b_data[] = "\"1\",\"2\",\" 3 ";
  //char test17_data[] = " a\0b\0c ";
  //char test18_data[] = "12345678901234567890123456789012";
  //char test19_data[] = "  , \"\" ,";

  ///* Custom tests */
  //char custom01_data[] = "'''a;b''';;' '''' ';''''' ';' ''''';''''''";

  ///* |1|2|3|4|5| */
  //struct event test01_results[] =
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_COL, 0, 1, "3"},
  //  {CSV_COL, 0, 1, "4"},
  //  {CSV_COL, 0, 1, "5"},
  //  {CSV_ROW, '\x0d', 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* ||||||| */
  //struct event test02_results[] =
  //{ {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, '\x0a', 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |,|,|| */
  //struct event test03_results[] = 
  //{ {CSV_COL, 0, 1, ","},
  //  {CSV_COL, 0, 1, ","},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  //struct event test04_results[] = 
  //{ {CSV_COL, 0, 147, "I call our world Flatland,\x0a"
  //    "not because we call it so,\x0a"
  //      "but to make its nature clearer\x0a"
  //      "to you, my happy readers,\x0a"
  //      "who are privileged to live in Space."},
  //{CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |"a,b"|| "" |"" | ""|""| */
  //struct event test05_results[] = 
  //{ {CSV_COL, 0, 5, "\"a,b\""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 4, " \"\" "},
  //  {CSV_COL, 0, 3, "\"\" "},
  //  {CSV_COL, 0, 3, " \"\""},
  //  {CSV_COL, 0, 2, "\"\""},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* | a, b ,c |a b  c|| */
  //struct event test06_results[] = 
  //{ {CSV_COL, 0, 9, " a, b ,c "},
  //  {CSV_COL, 0, 6, "a b  c"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* | " " " " | */
  //struct event test07_results[] = 
  //{ {CSV_COL, 0, 9, " \" \" \" \" "},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* Will cause error with CSV_STRICT set */
  //struct event test07b_results[] = 
  //{ {CSV_ERR, 0, 0, NULL} };

  //struct event test08_results[] = 
  //{ {CSV_COL, 0, 463,   " abc\"                                               "
  //    "                                                     "
  //      "                                                     "
  //      "                                                     "
  //      "                                                     "
  //      "                                                     "
  //      "                                                     "
  //      "                                                     "
  //      "                                        "},
  //{CSV_COL, 0, 3, "123"},
  //{CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* empty */
  //struct event test09_results[] = 
  //{ {CSV_END, 0, 0, NULL} };

  ///* |a| */
  //struct event test10_results[] = 
  //{ {CSV_COL, 0, 1, "a"},
  //  {CSV_ROW, '\x0a', 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |1|2|3|4| */
  //struct event test11_results[] = 
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_COL, 0, 1, "3"}, 
  //  {CSV_COL, 0, 1, "4"},
  //  {CSV_ROW, '\x0a', 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* empty */
  //struct event test12_results[] = 
  //{ {CSV_END, 0, 0, NULL} };

  ///* Test CSV_REPALL_NL */
  //struct event test12b_results[] = 
  //{ {CSV_ROW, '\x0a', 1, NULL},
  //  {CSV_ROW, '\x0a', 1, NULL},
  //  {CSV_ROW, '\x0a', 1, NULL},
  //  {CSV_ROW, '\x0a', 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |abc| */
  //struct event test13_results[] =
  //{ {CSV_COL, 0, 3, "abc"},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |1|2|3||
  //   |4||
  //   |||       */
  //struct event test14_results[] =
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_COL, 0, 1, "3"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, '\x0a', 1, NULL},
  //  {CSV_COL, 0, 1, "4"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, '\x0d', 1, NULL},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, -1, 0, NULL}, {CSV_END, 0, 0, NULL} }; 

  ///* |1|2|3||
  //   |4||
  //   ||       */
  //struct event test15_results[] =
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_COL, 0, 1, "3"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, '\x0a', 1, NULL},
  //  {CSV_COL, 0, 1, "4"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, '\x0d', 1, NULL},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_ROW, -1, 0, NULL}, {CSV_END, 0, 0, NULL} };


  ///* |1|2| 3 | */
  //struct event test16_results[] = 
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_COL, 0, 3, " 3 "},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* Will cause an error with CSV_STRICT_FINI set */
  //struct event test16b_results[] = 
  //{ {CSV_COL, 0, 1, "1"},
  //  {CSV_COL, 0, 1, "2"},
  //  {CSV_ERR, 0, 0, NULL} };

  ///* |a\0b\0c| */
  //struct event test17_results[] = 
  //{ {CSV_COL, 0, 5, "a\0b\0c"},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* Test CSV_EMPTY_IS_NULL */
  //struct event test19_results[] = 
  //{ {CSV_COL, 0, 0, NULL},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 0, NULL},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  ///* |'a;b'|| '' |'' | ''|''| */
  //struct event custom01_results[] = 
  //{ {CSV_COL, 0, 5, "'a;b'"},
  //  {CSV_COL, 0, 0, ""},
  //  {CSV_COL, 0, 4, " '' "},
  //  {CSV_COL, 0, 3, "'' "},
  //  {CSV_COL, 0, 3, " ''"},
  //  {CSV_COL, 0, 2, "''"},
  //  {CSV_ROW, -1, 1, NULL}, {CSV_END, 0, 0, NULL} };

  //DO_TEST(01, 0);
  //  DO_TEST(01, CSV_STRICT);
  //  DO_TEST(01, CSV_STRICT | CSV_EMPTY_IS_NULL);
  //  DO_TEST(02, 0);
  //  DO_TEST(02, CSV_STRICT);
  //  DO_TEST(03, 0);
  //  DO_TEST(03, CSV_STRICT);
  //  DO_TEST(04, 0);
  //  DO_TEST(04, CSV_STRICT);
  //  DO_TEST(05, 0);
  //  DO_TEST(05, CSV_STRICT);
  //  DO_TEST(05, CSV_STRICT | CSV_STRICT_FINI);
  //  DO_TEST(06, 0);
  //  DO_TEST(06, CSV_STRICT);
  //  DO_TEST(07, 0);
  //  DO_TEST(07b, CSV_STRICT);
  //  DO_TEST(08, 0);
  //  DO_TEST(09, 0);
  //  DO_TEST(09, CSV_EMPTY_IS_NULL);
  //  DO_TEST(10, 0);
  //  DO_TEST(11, 0);
  //  DO_TEST(11, CSV_EMPTY_IS_NULL);
  //  DO_TEST(12, 0);
  //  DO_TEST(12, CSV_EMPTY_IS_NULL);
  //  DO_TEST(12b, CSV_REPALL_NL);
  //  DO_TEST(12b, CSV_REPALL_NL | CSV_EMPTY_IS_NULL);
  //  DO_TEST(13, 0);
  //  DO_TEST(14, 0);
  //  DO_TEST(14, CSV_STRICT);
  //  DO_TEST(15, 0);
  //  DO_TEST(15, CSV_STRICT);
  //  DO_TEST(16, 0);
  //  DO_TEST(16, CSV_STRICT);
  //  DO_TEST(16b, CSV_STRICT | CSV_STRICT_FINI);
  //  DO_TEST(16, 0);
  //  DO_TEST(16, CSV_STRICT);
  //  DO_TEST(17, 0);
  //  DO_TEST(17, CSV_STRICT);
  //  DO_TEST(17, CSV_STRICT | CSV_EMPTY_IS_NULL);
  //  DO_TEST(19, CSV_EMPTY_IS_NULL);
  //
  //  DO_TEST_CUSTOM(01, 0, ';', '\'', NULL, NULL);
  //
  //  /* Writer Tests */
  //
  //  /* The writer tests are simpler, the test_writer function is used to
  //     test the csv_write function and takes five arguments: the name of
  //     the test, the data to convert, the length of that data, the expected
  //     resulting CSV data, and the expected length of the result.
  //     */
  //
  //  test_writer("1", "abc", 3, "\"abc\"", 5);
  //  test_writer("2", "\"\"\"\"\"\"\"\"", 8, "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", 18);
  //
  //  test_writer2("1", "abc", 3, "'abc'", 5, '\'');
  //  test_writer2("2", "''''''''", 8, "''''''''''''''''''", 18, '\'');

  puts("All tests passed");
  return 0;
}
