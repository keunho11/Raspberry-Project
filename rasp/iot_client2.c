/* author : KSH */
/* 서울기술 교육센터 IoT */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 13


static char* host = "localhost";
static char* user = "root";
static char* pass = "kcci";
static char* dbname = "openWeather";
/*********************/







void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE]="[Default]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
	
	//buffer value
	char ser_buff[1000] = { 0 };
	
	//parsing value
	char* pToken;
	char* pArray[10] = { 0 };

/******************socket****************/
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;

	//start
	printf("Raspberry Startup\n");
	fflush(stdout);

	//argument check
	if(argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n",argv[0]);
		exit(1);
	}

	sprintf(name, "%s",argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	sprintf(msg,"[%s:PASSWD]",name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);

	pthread_join(snd_thread, &thread_return);
	//	pthread_join(rcv_thread, &thread_return);

	close(sock);
	return 0;
}

void * send_msg(void * arg)
{
	int *sock = (int *)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE+2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n",stdout);
	while(1) {
		memset(msg,0,sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if(FD_ISSET(STDIN_FILENO, &newset))
		{
			fgets(msg, BUF_SIZE, stdin);
			if(!strncmp(msg,"quit\n",5)) {
				*sock = -1;
				return NULL;
			}
			else if(msg[0] != '[')
			{
				strcat(name_msg,"[ALLMSG]");
				strcat(name_msg,msg);
			}
			else
				strcpy(name_msg,msg);
			if(write(*sock, name_msg, strlen(name_msg))<=0)
			{
				*sock = -1;
				return NULL;
			}
		}
		if(ret == 0) 
		{
			if(*sock == -1) 
				return NULL;
		}
	}
}

void * recv_msg(void * arg)
{
	MYSQL* conn;
	conn = mysql_init(NULL);

	int sql_index, flag = 0;
	char in_sql[400] = { 0 };
	char in_sql2[400] = { 0 };
	int sql_len = 0;


	int  res1, res2, res3, res4 = 0;
	//temp, humi value
	double temp, tempSeoul, tempDaejeon, tempBusan, tempJeju = 0;
	int humi, humiSeoul, humiDaejeon, humiBusan, humiJeju = 0;

	if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
	{
		fprintf(stderr, "ERROR: %s[%d]\n",
			mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful!\n\n");


	/*********************/

	int * sock = (int *)arg;	
	int i;
	char *pToken;
	char *pArray[ARR_CNT]={0};

	char name_msg[NAME_SIZE + BUF_SIZE +1];
	int str_len;
	while(1) {
		memset(name_msg,0x0,sizeof(name_msg));
		str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE );
		if(str_len <= 0) 
		{
			*sock = -1;
			return NULL;
		}
		name_msg[str_len] = 0;
	//	fputs(name_msg, stdout);
		printf("%s\n",name_msg);
		

		if(str_len < 50) {
		pToken = strtok(name_msg, "]");
		int i = 0;
		while (pToken != NULL) {
			pArray[i] = pToken;
			if (++i > 12)
				break;
			pToken = strtok(NULL, "@");
		}
		printf("%s test\n", pArray[0]);
		temp = atof(pArray[1]);
		
		humi = atoi(pArray[2]);
		tempSeoul = atof(pArray[3]);
		humiSeoul = atoi(pArray[4]);
		tempDaejeon = atof(pArray[5]);
		humiDaejeon = atoi(pArray[6]);
		tempBusan = atof(pArray[7]);
		humiBusan = atoi(pArray[8]);
		tempJeju = atof(pArray[9]);
		humiJeju = atoi(pArray[10]);
		flag = 1;
		}
		//	printf("%s test\n", pArray[0]);
		printf("temp = %.1lf, humi = %d\n", temp, humi);
		printf("tempS = %.1lf, humiS = %d\n", tempSeoul, humiSeoul);
		printf("tempD = %.1lf, humiD = %d\n", tempDaejeon, humiDaejeon);
		printf("tempB = %.1lf, humiB = %d\n", tempBusan, humiBusan);
		printf("tempJ = %.1lf, humiJ = %d\n", tempJeju, humiJeju);

		/*************query*****************/

		if (temp > 0 && humi > 0)
		{
			if (flag == 1)
			{

				sprintf(in_sql, "insert into SeoulData (id,date,time,SeoulTemp, SeoulHumi, IndoorTemp, IndoorHumi) values (null, curdate(), curtime(), %.1lf, %d, %.llf, %d)", tempSeoul, humiSeoul, temp, humi);
				res1 = mysql_query(conn, in_sql);
				if (res1)
				{
					fprintf(stderr, "error: %s[%d]\n",
						mysql_error(conn), mysql_errno(conn));
					exit(1);
				}
				sql_len = strlen(in_sql);
				memset(in_sql, '\0', 400);
				//					printf("%s test\n", in_sql);

				sprintf(in_sql2, "insert into LocalData (id,date,time,DaejeonTemp, DaejeonHumi, BusanTemp, BusanHumi, JejuTemp, JejuHumi) values (null,curdate(), curtime(), %.1lf, %d, %.1lf, %d, %.1lf, %d)", tempDaejeon, humiDaejeon, tempBusan, humiBusan, tempJeju, humiJeju);
				res2 = mysql_query(conn, in_sql2);
				if (res2)
				{
					fprintf(stderr, "error: %s[%d]\n",
						mysql_error(conn), mysql_errno(conn));
					exit(1);
				}
				sql_len = strlen(in_sql2);
				memset(in_sql2, '\0', 400);

				//					printf("%s test2\n", in_sql2);

			}
			temp = 0;
			humi = 0;
		}
		flag = 0;



		/*   	pToken = strtok(name_msg,"[:]");
			i = 0;
			while(pToken != NULL)
			{
			pArray[i] =  pToken;
			if(i++ >= ARR_CNT)
			break;
			pToken = strtok(NULL,"[:]");
			}

		//		printf("id:%s, msg:%s,%s,%s,%s\n",pArray[0],pArray[1],pArray[2],pArray[3],pArray[4]);
		printf("id:%s, msg:%s\n",pArray[0],pArray[1]);
		*/



	}
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
