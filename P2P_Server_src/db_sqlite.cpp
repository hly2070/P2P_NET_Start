#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>               /* localtime, time */
#include <math.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>  
#include <netdb.h>
#include <sys/socket.h>   
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iconv.h>  
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include<unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "db_sqlite.h"
//#include <io.h>
#define MAX_DB_OBSERVER_NUM 50

  
static char *insertobserver[MAX_DB_OBSERVER_NUM]={0};
static char *deleteobserver[MAX_DB_OBSERVER_NUM]={0};
static char *updateobserver[MAX_DB_OBSERVER_NUM]={0};
pthread_mutex_t addseqlock;

static ObserverCallback insertcallback = NULL;
static ObserverCallback deletecallback = NULL;
static ObserverCallback updatecallback = NULL;
static int dbwillclose = 0,isinsert = 0,isupdate=0,isdelete =0,isquery=0;
pthread_mutex_t db2insertlock;

static sqlite3 *db=NULL,*db2 = NULL;  
int sqllite_init(char *dburl,int encrypted)
{
	 int rc,res = 0;  
	 FILE *fd = fopen(dburl,"rb");
	 if(!fd)
	{  
		FILE *fp=fopen(dburl,"wb");
		fclose(fp);
		res = 1;
	    
	}  
	 else
	 {
		fclose(fd);
	 }
	  
	rc = sqlite3_open(dburl, &db);  

	if( rc != SQLITE_OK)  
	{    
	    fprintf(stderr, "%s open failed: %s\n", dburl,sqlite3_errmsg(db));    
	    //sqlite3_close_v2(db);   
		sqlite3_close(db); 
	    db = NULL;
	    res = -1;
	}  
	//if(encrypted)
	//	sqlite3_key(db, "IE59261Uio$", strlen("IE59261Uio$")); 
	pthread_mutex_init(&addseqlock,NULL);
	dbwillclose = 0;
	return res;
}

void sqllite_close()
{
	dbwillclose = 1;
	
	
	pthread_mutex_lock(&addseqlock);
	//int res = sqlite3_close_v2(db);
	int res = sqlite3_close(db);
	db = NULL;
	pthread_mutex_unlock(&addseqlock);
	
	pthread_mutex_destroy(&addseqlock);
}

int sqllite_query(char *sqlcmd,db_val_t *params,int paramnum,sqlite3_stmt **stmtout,char*dbnum)
{
	int rc= 0;
	int i;  
	int res;

	sqlite3_stmt* stmt = NULL;  
     
	isquery = 1;

	rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, 0);  

	if (rc != SQLITE_OK)  
	{  
	    printf("-----\nsqllite_query failure rc:%d:errmsg%s,sqlcmd:%s \n", rc,sqlite3_errmsg(db),sqlcmd);    
	    isquery = 0;
	    return -1;    
	}  
	for(i = 0; i < paramnum; i++)
	{
		switch(params[i].type)
		{
		   case DB_NULL:  
		    sqlite3_bind_null(stmt, i+1);
		    break;  
		  case DB_INT:  
		      sqlite3_bind_int(stmt, i+1,params[i].val.int_val);
		    break;  
		  case DB_BIGINT:  
		     sqlite3_bind_int64(stmt, i+1,params[i].val.ll_val);     
		    break;  
		  case DB_DOUBLE:
		    sqlite3_bind_double(stmt, i+1,params[i].val.double_val);  
		    break;  
		  case DB_STRING:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.string_val,-1,SQLITE_STATIC);  
		    break;  
		  case DB_DATETIME:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.time_val,-1,SQLITE_STATIC);  
		    break;  
		  default:  
		  	break;
		}
	}
	res = sqlite3_step(stmt);
	if(res != SQLITE_ROW)
	{
		//printf("%d",res);
		sqlite3_finalize(stmt);  
		 isquery = 0;
		if(res == SQLITE_DONE)
			return -2;
		else
			return -1;
	}
	if(stmtout)
		*stmtout = stmt;
	else
		sqlite3_finalize(stmt);  
	 isquery = 0;
	return 0;
}

int sqlite_reg_insertcallback(ObserverCallback insertcallback0)
{
	if(insertcallback  != NULL)
		return -1;
	insertcallback = insertcallback0;
	return 0;
}

int sqlite_reg_deletecallback(ObserverCallback deletecallback0)
{
	if(deletecallback  != NULL)
		return -1;
	deletecallback = deletecallback0;
	return 0;
}

int sqlite_reg_updatecallback(ObserverCallback updatecallback0)
{
	if(updatecallback  != NULL)
		return -1;
	updatecallback = updatecallback0;
	return 0;
}

int sqlite_reg_insertobserver(char *tablename)
{
	int i;
	for(i = 0; i < MAX_DB_OBSERVER_NUM; i++)
	{
		if(insertobserver[i] == NULL)
		{
			insertobserver[i] = (char *)malloc(strlen(tablename)+1);
			strcpy(insertobserver[i],tablename);
			break;
		}
	}
	if(i == MAX_DB_OBSERVER_NUM)
		return -1;
	return 0;
}

int sqlite_reg_deleteobserver(char *tablename)
{
	int i;
	for(i = 0; i < MAX_DB_OBSERVER_NUM; i++)
	{
		if(deleteobserver[i] == NULL)
		{
			deleteobserver[i] = (char*)malloc(strlen(tablename)+1);
			strcpy(deleteobserver[i],tablename);
			break;
		}
	}
	if(i == MAX_DB_OBSERVER_NUM)
		return -1;
	return 0;
}

int sqlite_reg_updateobserver(char *tablename)
{
	int i;
	for(i = 0; i < MAX_DB_OBSERVER_NUM; i++)
	{
		if(updateobserver[i] == NULL)
		{
			updateobserver[i] = (char*)malloc(strlen(tablename)+1);
			strcpy(updateobserver[i],tablename);
			break;
		}
	}
	if(i == MAX_DB_OBSERVER_NUM)
		return -1;
	return 0;
}

int sqlite_unreg_allcallback()
{
	insertcallback = NULL;
	deletecallback = NULL;
	updatecallback = NULL;
	return 0;
}
int sqlite_unreg_allobserver()
{
	int i;
	for(i = 0; i < MAX_DB_OBSERVER_NUM; i++)
	{
		if(insertobserver[i] != NULL)
		{
			free(insertobserver[i]);
			insertobserver[i] = NULL;
		}
		if(deleteobserver[i] != NULL)
		{
			free(deleteobserver[i]);
			deleteobserver[i] = NULL;
		}
		if(updateobserver[i] != NULL)
		{
			free(updateobserver[i]);
			updateobserver[i] = NULL;
		}
	}
	return 0;
}

int sqllite_beginTransaction(char*dbnum)
{
	char *szErrMsg = 0;

	return sqlite3_exec(db, "begin;", 0,0, &szErrMsg);
}

int sqllite_rollbackTransaction(char*dbnum)
{
	char *szErrMsg = 0;
	return sqlite3_exec(db, "rollback;", 0,0, &szErrMsg);
}

int sqllite_commitTransaction(char*dbnum)
{
	char *szErrMsg = 0;
	return sqlite3_exec(db, "commit;", 0,0, &szErrMsg);
}

int sqllite_exec(char *sql,char*dbnum)
{
	char *szErrMsg = 0;
	return sqlite3_exec(db, sql, 0,0, &szErrMsg);
}



int sqllite_insert(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum,int observer,int *newid)
{
#define SERVER_NUM_CYC 50
	int rc= 0;
	int i,haslock = 0,paramnumlocal = 0,res = 0,updatelocal = 0;  
	sqlite3_stmt* stmt = NULL;  
	db_val_t *dbvalue = NULL,*paramlocal =NULL;
	char sqlstr[1000]={0};
	int id = 0;
	
	isinsert = 1;
	
	if((observer && insertcallback != NULL) || newid)
	{
		haslock =1;
	}	

	if(haslock)
		pthread_mutex_lock(&addseqlock);
	if(haslock)
	{
		char tempobser[50],tempobser2[50],tempobser3[50];
		for(i = 0; i < MAX_DB_OBSERVER_NUM; i++)
		{
			if(insertobserver[i] == NULL)
				continue;
			strcpy(tempobser,insertobserver[i]);
			strcat(tempobser,"(");
			strcpy(tempobser2,insertobserver[i]);
			strcat(tempobser2," (");
			strcpy(tempobser3,insertobserver[i]);
			strcat(tempobser3,"  (");
			if(strstr(sqlcmd,tempobser) || strstr(sqlcmd,tempobser2) || strstr(sqlcmd,tempobser2))
			{	
				char tempstr[100]="select seq from sqlite_sequence where name='";
				char *line,*lineend;
				sqlite3_stmt* stmt2 = NULL;  
				strcat(tempstr,insertobserver[i]);
				strcat(tempstr,"'");
				int restemp = sqllite_query(tempstr,NULL,0, &stmt2,dbnum);
				if(restemp == 0)
				{
					id = sqlite3_column_int(stmt2,0);
					sqlite3_finalize(stmt2);  
					
					if((id+1) % SERVER_NUM_CYC == 0)
						id += 2;
					else
						id += 1;
					
					
				}
				else if(restemp == -2)
				{
					id = 1;
				}
				else
				{
					break;
				}
				
				line = strstr(sqlcmd,"(");
				if(!line)
					break;
				line++;
				strncpy(sqlstr,sqlcmd,line-sqlcmd);
				strcat(sqlstr,"id,");

				lineend = strstr(line,"(");
				if(!lineend)
					break;
				lineend++;
				memcpy(sqlstr+strlen(sqlstr),line,lineend-line);
				strcat(sqlstr,"?,");
				strcat(sqlstr,lineend);
				dbvalue = (db_val_t*)calloc(paramnum+1,sizeof(db_val_t));
				dbvalue->type = DB_INT;
				dbvalue->val.int_val = id; 
				memcpy(&dbvalue[1],params,sizeof(db_val_t)*paramnum);
				break;
			}
			
		}
	}

	if(dbvalue == NULL)
	{
		paramlocal = params;
		paramnumlocal = paramnum;
		rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, 0);  
		if (rc != SQLITE_OK)  
		{  
			if(db == NULL)
			{
				updatelocal = 2;
				goto END;
			}
			if(haslock)
			{
				pthread_mutex_unlock(&addseqlock);
			}
			printf("-----\nsqllite_insert failure rc:%d:errmsg%s,sqlcmd:%s \n", rc,sqlite3_errmsg(db),sqlcmd);    
			isinsert = 0;
			return -1;    
		}  
	}
	else
	{
		paramlocal = dbvalue;
		paramnumlocal = paramnum+1;
		rc = sqlite3_prepare_v2(db, sqlstr, -1, &stmt, 0);  
		if (rc != SQLITE_OK)  
		{  
			if(db == NULL)
			{
				updatelocal = 2;
				free(dbvalue);
				dbvalue = NULL;
				id = 0;
				goto END;
			}
			if(haslock)
			{
				if(dbvalue)
				{
					free(dbvalue);
				}
				pthread_mutex_unlock(&addseqlock);
			}
			printf("-----\nsqllite_insert failure rc:%d:errmsg%s,sqlcmd:%s \n", rc,sqlite3_errmsg(db),sqlcmd);    
			
			isinsert = 0;
			return -1;    
		}  
	}
	for(i = 0; i < paramnumlocal; i++)
	{
		switch(paramlocal[i].type)
		{
		   case DB_NULL:  
		    sqlite3_bind_null(stmt, i+1);
		    break;  
		  case DB_INT:  
		      sqlite3_bind_int(stmt, i+1,paramlocal[i].val.int_val);
		    break;  
		  case DB_BIGINT:  
		     sqlite3_bind_int64(stmt, i+1,paramlocal[i].val.ll_val);     
		    break;  
		  case DB_DOUBLE:
		    sqlite3_bind_double(stmt, i+1,paramlocal[i].val.double_val);  
		    break;  
		  case DB_STRING:   
		   sqlite3_bind_text(stmt, i+1,paramlocal[i].val.string_val,-1,SQLITE_STATIC);  
		    break;  
		  case DB_DATETIME:   
		   sqlite3_bind_text(stmt, i+1,paramlocal[i].val.time_val,-1,SQLITE_STATIC);  
		    break;  
		  default:  
		     break;
		    
		}
	}
	
	res = sqlite3_step(stmt);
	if(res != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);  
		if(res == SQLITE_BUSY)
		{
			updatelocal = 2;
			if(dbvalue)
			{
				free(dbvalue);
				dbvalue = NULL;
				id = 0;
			}
			goto END;
		}
		if(haslock)
		{
			if(dbvalue)
			{
				free(dbvalue);
			}
			pthread_mutex_unlock(&addseqlock);
		}
		printf("-----\nsqllite_insert failure:res:%d,%s \n",res, sqlite3_errmsg(db));  
		
		isinsert = 0;
		return -1;
	}
	sqlite3_finalize(stmt); 
END:
	if(observer && insertcallback != NULL)
	{
		if(dbvalue)
		{
			insertcallback(sqlstr,dbvalue,paramnum+1,dbnum,updatelocal);
		}
		else
		{
			insertcallback(sqlcmd,params,paramnum,dbnum,updatelocal);
		}
	}
	if(haslock)
	{
		pthread_mutex_unlock(&addseqlock);
		if(dbvalue)
			free(dbvalue);
	}
	if(newid) *newid = id;
	isinsert = 0;
	return 0;	
}

int sqllite_delete(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum,int observer)
{
	int rc= 0;
	int i ,res = 0;  
	sqlite3_stmt* stmt = NULL;  
	
	isdelete = 1;

	rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, 0);  
	if (rc != SQLITE_OK)  
	{  
	    if(db == NULL)
	    {
	    		res = SQLITE_BUSY;
			goto END;
	    }
	    printf("-----\nsqllite_delete failure rc:%d:errmsg%s,sqlcmd:%s \n", rc,sqlite3_errmsg(db),sqlcmd);    
	    isdelete = 0;
	    return -1;    
	}  
	for(i = 0; i < paramnum; i++)
	{
		switch(params[i].type)
		{
		   case DB_NULL:  
		    sqlite3_bind_null(stmt, i+1);
		    break;  
		  case DB_INT:  
		      sqlite3_bind_int(stmt, i+1,params[i].val.int_val);
		    break;  
		  case DB_BIGINT:  
		     sqlite3_bind_int64(stmt, i+1,params[i].val.ll_val);     
		    break;  
		  case DB_DOUBLE:
		    sqlite3_bind_double(stmt, i+1,params[i].val.double_val);  
		    break;  
		  case DB_STRING:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.string_val,-1,SQLITE_STATIC);  
		    break;  
		  case DB_DATETIME:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.time_val,-1,SQLITE_STATIC);  
		    break;  
		  default:  
		    break;
		    
		}
	}
	res = sqlite3_step(stmt);
	if(res != SQLITE_DONE && res != SQLITE_BUSY)
	{
		sqlite3_finalize(stmt);  
		printf("-----\nsqllite_delete failure:%s \n", sqlite3_errmsg(db));    
		isdelete = 0;
		
		return -1;
	}
	sqlite3_finalize(stmt);  
END:
	if(observer && deletecallback != NULL)
	{
		deletecallback(sqlcmd,params,paramnum,dbnum,res == SQLITE_BUSY?1:0);
	}
	isdelete = 0;
	return 0;	
}


int sqllite_update(char *sqlcmd,db_val_t *params,int paramnum,char*dbnum,int observer)
{
	int rc= 0 ,res = 0;
	int i;  
	sqlite3_stmt* stmt = NULL;  
	
	isupdate = 1;

	rc = sqlite3_prepare_v2(db, sqlcmd, -1, &stmt, 0);  
	if (rc != SQLITE_OK)  
	{  
	    if(db == NULL)
	    {
	    		res = SQLITE_BUSY;
			goto END;
	    }
	    printf("-----\nsqllite_update failure rc:%d:errmsg%s,sqlcmd:%s \n", rc,sqlite3_errmsg(db),sqlcmd);
	    isupdate = 0;
	    return -1;    
	}  
	for(i = 0; i < paramnum; i++)
	{
		switch(params[i].type)
		{
		   case DB_NULL:  
		    sqlite3_bind_null(stmt, i+1);
		    break;  
		  case DB_INT:  
		      sqlite3_bind_int(stmt, i+1,params[i].val.int_val);
		    break;  
		  case DB_BIGINT:  
		     sqlite3_bind_int64(stmt, i+1,params[i].val.ll_val);     
		    break;  
		  case DB_DOUBLE:
		    sqlite3_bind_double(stmt, i+1,params[i].val.double_val);  
		    break;  
		  case DB_STRING:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.string_val,-1,SQLITE_STATIC);  
		    break;  
		  case DB_DATETIME:   
		   sqlite3_bind_text(stmt, i+1,params[i].val.time_val,-1,SQLITE_STATIC);  
		    break;  
		  default:  
		    break;
		    
		}
	}
	res = sqlite3_step(stmt);
	if(res != SQLITE_DONE && res != SQLITE_BUSY)
	{
		sqlite3_finalize(stmt);  
		printf("-----\nsqllite_update failure:%s \n", sqlite3_errmsg(db));     
		isupdate = 0;
		
		return -1;
	}
	sqlite3_finalize(stmt);  
END:
	if(observer && updatecallback != NULL)
	{
		
		updatecallback(sqlcmd,params,paramnum,dbnum,res == SQLITE_BUSY?1:0);
	}
	isupdate = 0;
	return 0;	
}

