#ifndef _DB_SQLITE_H_
#define _DB_SQLITE_H_
extern "C"
{
#include "sqlite3.h"
}
typedef enum {
	DB_NULL,
	DB_INT,        /**< represents an 32 bit integer number      */
	DB_BIGINT,     /**< represents an 64 bit integer number      */
	DB_DOUBLE,     /**< represents a floating point number       */
	DB_STRING,     /**< represents a zero terminated const char* */
	DB_DATETIME,   /**< represents date and time                 */
} db_type_t;

typedef struct {
	db_type_t type; /**< Type of the value                              */
	union {
		int           int_val;    /**< integer value              */
		long long     ll_val;     /**< long long value            */
		double        double_val; /**< double value               */
		const char*   time_val;   /**< time value str     */
		const char*   string_val; /**< zero terminated string     */
	} val;
} db_val_t;
typedef int (*ObserverCallback)(char *sqlstr,db_val_t *params,int paramnum,char*dbnum,int updatelocal);
typedef int (*PictureCallback)(char *picpath);

int sqllite_init(char *dburl,int encrypted);

void sqllite_close();

#define PSS_DB_CPY_STR(outstr,stmt,index) do{\
	if(sqlite3_column_type(stmt , index) == SQLITE_NULL) \
		strcpy(outstr,""); \
	else \
		strcpy(outstr,(const char *)sqlite3_column_text(stmt, index)); \
	}while(0)

#define PSS_DB_CPY_STR_2(outstr,stmt,index) do{\
	if(sqlite3_column_type(stmt , index) == SQLITE_NULL) \
		strcpy(outstr,"-1"); \
	else \
		strcpy(outstr,(const char *)sqlite3_column_text(stmt, index)); \
	}while(0)	
	
#define PSS_DB_CPY_INT(out,stmt,index) do{\
	if(sqlite3_column_type(stmt , index) == SQLITE_NULL) \
		out = -1; \
	else \
		out = sqlite3_column_int(stmt, index); \
	}while(0)

#define PSS_DB_CPY_INT2(out,stmt,index) do{\
	if(sqlite3_column_type(stmt , index) == SQLITE_NULL) \
		out = 0; \
	else \
		out = sqlite3_column_int(stmt, index); \
	}while(0)
	
#define PSS_DB_CPY_DOUBLE(out,stmt,index) do{\
	if(sqlite3_column_type(stmt , index) == SQLITE_NULL) \
		out = 0; \
	else \
		out = sqlite3_column_double(stmt, index); \
	}while(0)


int sqllite_query(char *sqlcmd,db_val_t *params,int paramnum,sqlite3_stmt **stmtout,char*dbnum=NULL);
int sqllite_insert(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum=NULL,int observer=1,int *newid = NULL);
int sqllite_delete(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum=NULL,int observer=1);
int sqllite_update(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum=NULL,int observer=1);
int sqllite_beginTransaction(char*dbnum=NULL);
int sqllite_rollbackTransaction(char*dbnum=NULL);
int sqllite_commitTransaction(char*dbnum=NULL);
int sqllite_exec(char *sql,char*dbnum=NULL);
int sqlite_reg_insertcallback(ObserverCallback insertcallback0);
int sqlite_reg_insertobserver(char *tablename);
int sqlite_reg_deletecallback(ObserverCallback insertcallback0);
int sqlite_reg_deleteobserver(char *tablename);
int sqlite_reg_updatecallback(ObserverCallback insertcallback0);
int sqlite_reg_updateobserver(char *tablename);
int sqlite_unreg_allcallback();
int sqlite_unreg_allobserver();
#endif
