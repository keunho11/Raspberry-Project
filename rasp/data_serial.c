#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

static char * host = "localhost";
static char * user = "root";
static char * pass = "kcci";
static char * dbname = "openWeather";



char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;  


int main()
{
	MYSQL *conn;
	conn = mysql_init(NULL);

	int sql_index, flag = 0;
	char in_sql[400] = {0};
	char in_sql2[400] = {0};
	int sql_len =0;

	if(!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
	{
		fprintf(stderr, "ERROR: %s[%d]\n",
		mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful!\n\n");

        //buffer value
	char ser_buff[1000] = {0};
	int index = 0, str_len, res1, res2, res3, res4 =0;
	//temp, humi value
	double temp, tempSeoul, tempDaejeon, tempBusan, tempJeju =0;
	int humi,  humiSeoul,  humiDaejeon, humiBusan, humiJeju =0;	
	//parsing value
	char *pToken;
	char *pArray[10] = {0};


	//start
	printf("Raspberry Startup\n");
	fflush(stdout);
	if((fd = serialOpen(device, baud)) < 0){
		fprintf(stderr, "Unable %s\n",strerror(errno));
		exit(1);
	}
	if(wiringPiSetup() == -1)
		return 1;
	while(1)
	{
		if(serialDataAvail(fd)) {

			flag = 1;
			ser_buff[index++] = serialGetchar(fd);

			if(ser_buff[index-1] == 'L')
			{
				ser_buff[index-1] = '\0';
				str_len = strlen(ser_buff);
				pToken = strtok(ser_buff, "@");
				int i = 0;
				while(pToken != NULL) {
					pArray[i] = pToken;
					if(++i>11)
						break;
					pToken = strtok(NULL, "@");
				}
				temp = atof(pArray[0]);
				humi = atoi(pArray[1]);
				tempSeoul = atof(pArray[2]);
				humiSeoul = atoi(pArray[3]);
				tempDaejeon = atof(pArray[4]);
				humiDaejeon = atoi(pArray[5]);
				tempBusan = atof(pArray[6]);
				humiBusan = atoi(pArray[7]);
				tempJeju = atof(pArray[8]);
				humiJeju = atoi(pArray[9]);

			//	printf("%s test\n", pArray[0]);
				printf("temp = %.1lf, humi = %d\n", temp, humi);
				printf("tempS = %.1lf, humiS = %d\n", tempSeoul, humiSeoul);
				printf("tempD = %.1lf, humiD = %d\n", tempDaejeon, humiDaejeon);
				printf("tempB = %.1lf, humiB = %d\n", tempBusan, humiBusan);
				printf("tempJ = %.1lf, humiJ = %d\n", tempJeju, humiJeju);

				//buffer clear
				for(int i = 0; i <= str_len; i++)
				{
					ser_buff[i]=0;
				}
				index = 0;
			}
			
           }
			
		/*************query*****************/	
			
			if(temp > 0 && humi > 0)
			{
				if(flag == 1)
				{
					
					sprintf(in_sql,"insert into SeoulData (id,date,time,SeoulTemp, SeoulHumi, IndoorTemp, IndoorHumi) values (null, curdate(), curtime(), %.1lf, %d, %.llf, %d)",tempSeoul, humiSeoul, temp, humi);
					res1 = mysql_query(conn,in_sql);
					if(res1)
					{
						fprintf(stderr, "error: %s[%d]\n",
						mysql_error(conn), mysql_errno(conn));
						exit(1);
					}
					sql_len = strlen(in_sql);
					memset(in_sql,'\0',400);
//					printf("%s test\n", in_sql);
					
					sprintf(in_sql2,"insert into LocalData (id,date,time,DaejeonTemp, DaejeonHumi, BusanTemp, BusanHumi, JejuTemp, JejuHumi) values (null,curdate(), curtime(), %.1lf, %d, %.1lf, %d, %.1lf, %d)",tempDaejeon, humiDaejeon, tempBusan, humiBusan, tempJeju, humiJeju);
					res2 = mysql_query(conn,in_sql2);
					if(res2)
					{
						fprintf(stderr, "error: %s[%d]\n",
						mysql_error(conn), mysql_errno(conn));
						exit(1);
					}
					sql_len = strlen(in_sql2);
					memset(in_sql2,'\0',400);

//					printf("%s test2\n", in_sql2);

				}
				temp=0;
				humi=0;
			}
			flag = 0;
			
		}
		fflush(stdout);
	

	return 0;
}

