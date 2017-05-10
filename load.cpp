#include <stdio.h>
#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>

//for apostraphe replacment in strings
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
  if(from.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
      }
    }


    static int callback(void *NotUsed, int argc, char **argv, char **azColName){
     int i;
     for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
  }



  void creatTable(const char* createCommand, std::string tableName, int cols, sqlite3 *db, const char* filePath){

  //fields
    char *zErrMsg = 0;

  //-----drop table if exists----------
    std::string drop = "DROP TABLE IF EXISTS ";
    drop+= tableName;
    drop+= ";";
    const char* dropCommand = drop.c_str();

    int rc0 = sqlite3_exec(db, dropCommand, callback, 0, &zErrMsg);


    if( rc0 != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }

   else{
    fprintf(stdout, "Table drop worked\n");
  }


  //-------create table----------
  int rc = sqlite3_exec(db, createCommand, callback, 0, &zErrMsg);


  if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
   sqlite3_free(zErrMsg);
 }

 else{
  fprintf(stdout, "Table created successfully\n");
}


//now lets fill this db
std::ifstream textfile(filePath);

if (textfile.fail())
{
  std::cout << "open failed";

}

std::string line;
while (getline(textfile, line))
{


    //get into format to insert
  std::vector<std::string> list;
  std::string delimiter = "^";
  std::string token;
  int pos = 0;
  
  while ((pos = line.find(delimiter)) != std::string::npos) {

    token = line.substr(0, pos);
    line.erase(0, pos + delimiter.length());


    //remove punctuation replace single ' with ''
    replaceAll(token, "'", "''");
    token.erase(std::remove(token.begin(), token.end(), '~'), token.end());
    token.erase(std::remove(token.begin(), token.end(), '\n'), token.end());
    
    if(token.empty()){
      list.push_back("null");
    }else{

      list.push_back(token);
    }
  }
  
  //remove punctuation replace single ' with ''
  replaceAll(line, "'", "''");
  line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
  line.erase(std::remove(line.begin(), line.end(), '~'), line.end());

  if(line.size() < 2){
   list.push_back("null");
 } else{


  list.push_back(line);
}


//create insert statement
sqlite3_stmt *stmtinsert;
std::string insertstmt = "INSERT INTO ";
insertstmt+= tableName;
insertstmt+= " VALUES ('";
std::string comma= "','";

for(int i = 0; i < cols-1; i++){
 insertstmt+=(list[i]);
 insertstmt+=(comma);
}

insertstmt+=(list[cols-1]);
insertstmt+= "');";
const char *insert = insertstmt.c_str();

sqlite3_exec(db, "BEGIN", 0, 0, 0);
int rc2 = sqlite3_exec(db, insert, callback, 0, &zErrMsg);
sqlite3_exec(db, "COMMIT", 0, 0, 0);
if( rc2 != SQLITE_OK ){
  fprintf(stderr, "SQL error: %s\n", zErrMsg);
  sqlite3_free(zErrMsg);
}
}
}


int main(int argc, char* argv[])
{
  sqlite3 *db;
  int conn;
  int rc;
  char *zErrMsg = 0;


  //open DATA_SRC
  conn = sqlite3_open("nutrients.db", &db); //sqlite3 api

  if( conn ){
    fprintf(stdout, "Unable to open the database: %s\n", sqlite3_errmsg(db)); //sqlite3 api
    return(0);
  }else{
    fprintf(stdout, "database opened successfully\n");
  }

//------Create Tables----------------//

  //Food_DES.txt
  const char* create0 = 
  "CREATE TABLE FOOD_DES ("  \
  "NDB_No         CHAR(5) PRIMARY KEY     NOT NULL," \
  "FdGrp_Cd        CHAR(4)    NOT NULL," \
  "Long_Desc          CHAR(200)     NOT NULL," \
  "Shrt_Desc           CHAR(60)      NOT NULL," \
  "ComName        CHAR(100)," \
  "ManufacName       CHAR(65)," \
  "Survey    CHAR(1)," \
  "Ref_desc     CHAR(135)," \
  "Refuse       INTEGER," \
  "SciName     CHAR(65)," \
  "N_Factor     REAL," \
  "Pro_Factor     REAL," \
  "Fat_Factor     REAL," \
  "CHO_Factor     REAL);";

  std::string tablename0 = "FOOD_DES";
  const char* filePath0 = "FOOD_DES.txt";
  creatTable(create0, tablename0, 14, db, filePath0);


  //NUTR_DEF.txt
  const char* create1 = 
  "CREATE TABLE NUTR_DEF ("  \
  "Nutr_No         CHAR(3) PRIMARY KEY     NOT NULL," \
  "Units        CHAR(7)    NOT NULL," \
  "Tagname          CHAR(20)," \
  "NutrDesc           CHAR(60)      NOT NULL," \
  "Num_Dec        CHAR(1)  NOT NULL," \
  "SR_Order       INTEGER  NOT NULL);" ;

  std::string tablename1 = "NUTR_DEF";
  const char* filePath1 = "NUTR_DEF.txt";
  creatTable(create1, tablename1, 6, db, filePath1);


  //NUT_DATA.txt
  const char* create2 =
  "CREATE TABLE NUT_DATA ("  \
  "NDB_No         CHAR(5) NOT NULL," \
  "Nutr_No        CHAR(3) NOT NULL," \
  "Nutr_Val       REAL     NOT NULL," \
  "Num_Data_Pts   REAL     NOT NULL," \
  "Std_Error        REAL," \
  "Src_Cd       CHAR(2) NOT NULL," \
  "Deriv_Cd    CHAR(4)," \
  "Ref_NDB_No     CHAR(5)," \
  "Add_Nutr_Mark       CHAR(1)," \
  "Num_Studies     INTEGER," \
  "Min     REAL," \
  "Max     REAL," \
  "DF     INTEGER," \
  "Low_EB     REAL,"\
  "Up_EB     REAL,"\
  "Stat_cmt     CHAR(10),"\
  "AddMod_Date     CHAR(10),"\
  "CC     A(1),"\
  "FOREIGN KEY(NDB_No) REFERENCES FOOD_DES(NDB_No),"\
  "FOREIGN KEY(Nutr_No) REFERENCES NUTR_DEF(Nutr_No));";
  
  std::string tablename2 = "NUT_DATA";
  const char* filePath2 = "NUT_DATA.txt";
  creatTable(create2, tablename2, 18, db, filePath2);

  sqlite3_close(db);
  return 0;
}

