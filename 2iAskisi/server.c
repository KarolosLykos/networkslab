//dilwsi anagkaiwn biblio8ikwn
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <signal.h>

using namespace std;

void error(const char *msg) //sunartisi sfalmatwn
{
    perror(msg);
    exit(1);
}
void shotgun(int signo); //sunartisi pou elegxei to sima ctrl +c twn clients
int pid;
int main(int argc, char *argv[])
{
    int flagname=0,flagip=0,flagserv=0,flagprot=0,len=0; //dilwsi metablitwn pou xrisimopoiw
    string iptemp,tempmes;  //dilwsi metablitwn pou xrisimopoiw
    struct service       //dilwsi struct metablitwn pou xrisimopoiw
    {
        struct servent *serv;
        string hostname,tempserv,tempprot;
        struct in_addr hostip;
        long justh;
        struct hostent *hp,*ph;
    }test;

    int sockfd, newsockfd, portno,i,j;
    socklen_t clilen;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) //elgexos gia ta inputs
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //arxikopoihsh tou API socket
    if (sockfd < 0)//elegxos dimiourgias
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    if (argv[1][0] != '-' || argv[1][1] != 'p')//elgexos gia ta inputs (ta orismata -p kai port)
    {
        fprintf(stderr,"Error Wrong arguments .Must be -p xxxx \n");
        exit(1);
    }
    portno = atoi(argv[2]); //cast apo char se int to port number
    if (portno< 9600 || portno>9609) //elgexos port sumfwna me ton ari8mo tis omadas
    {
        fprintf(stderr,"Error Wrong Ports, Must be from 9600 to 9609 !!!\n");
        exit(1);
    }
    signal( SIGCHLD, shotgun); // xrisi tou signal handler
    serv_addr.sin_family = AF_INET; //xrisi AF_INET (internet) protokollou
    serv_addr.sin_addr.s_addr = INADDR_ANY; // i karta diktuou 8a akouei apo ola ta interfaces
    serv_addr.sin_port = htons(portno); //xrisi tis port pou dinei o user
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //elegxos kai ektelesi tou bind
        error("ERROR on binding");
    listen(sockfd,100); //o server akouei mexri 100 pelates



    for (;;) //atermwn loop pou dexetai pelates
    {

        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pid=fork(); //gia ka8e client dimiourgite kainourgia process
        if (pid==0)/*An i diergasia einai to paidi, tote to programma proxoraei sto parakatw block tis if*/
        {

            close(sockfd);

            bzero(buffer,255); //ka8arismos tou buffer pou xrisimopoieitai gia tin epikoinwnia client/server
            n = write(newsockfd,"Welcome to the Dns server\n",27);
            if (n < 0) error("ERROR writing to socket111");
            n = write(newsockfd,"Enter HOSTNAME[enter if blank]:",31);
            if (n < 0) error("ERROR writing to socket111");
            n = read(newsockfd,buffer,255);
            if (n < 0) error("ERROR reading from socket");

            if (buffer[1] !='\n')//elegxos an o client den edwse orisma
            {
                for (i=0;i<=255;i++)//diatrexei oles tis 8eseis tou buffer mexri na dei trash
                {
                    if (buffer[i+2]=='\0')//elegxos tou buffer(apotrepei ta trash apo to na diabastoun ws clientdata)
                    {
                        break;
                    }
                    test.hostname+=buffer[i]; //apo8ikeuetai apo ton buffer to orisma tou client
                }
                flagname=1;//flag to opoio xrisimopoieitai gia ton elegxo tis xrisis upiresias tou server
            }
            bzero(buffer,255);//ka8arismos tou buffer pou xrisimopoieitai gia tin epikoinwnia client/server
            n = write(newsockfd,"Enter HOSTIPADDRESS[enter if blank]:",37);
            n = read(newsockfd,buffer,255);//o server dexetai to input tou client
            if (n < 0) error("ERROR reading from socket");

            if (buffer[1]!='\n')//elegxos an o client den edwse orisma
            {
                for (i=0;i<=255;i++)//diatrexei oles tis 8eseis tou buffer mexri na dei trash
                {
                    if (buffer[i+2]=='\0')//elegxos tou buffer(apotrepei ta trash apo to na diabastoun ws clientdata)
                    {
                        break;
                    }
                    iptemp+=buffer[i]; //apo8ikeuetai apo ton buffer to orisma tou client
                    if (!(buffer[i]>='0' && buffer[i]<='9')&&buffer[i] !='.')//suntaktikos elegxos tis ip
                    {
                        n = write(newsockfd,"IP syntax error",15);
                        exit(1);
                    }
                }
                flagip=1;//flag to opoio xrisimopoieitai gia ton elegxo tis xrisis upiresias tou server
                test.justh =inet_addr(iptemp.c_str()); //cast metablitis ip wste na tin dexetai i suntartisi gethostbyaddr
            }
            bzero(buffer,255);//ka8arismos tou buffer pou xrisimopoieitai gia tin epikoinwnia client/server
            n = write(newsockfd,"Enter Service[enter if blank]:",31);
            n = read(newsockfd,buffer,255);//o server dexetai to input tou client
            if (n < 0) error("ERROR reading from socket");
            if (buffer[1]!='\n')//elegxos an o client den edwse orisma
            {
                for (i=0;i<=255;i++)//diatrexei oles tis 8eseis tou buffer mexri na dei trash
                {
                    if (buffer[i+2]=='\0')//elegxos tou buffer(apotrepei ta trash apo to na diabastoun ws clientdata)
                    {
                        break;
                    }
                    test.tempserv+=buffer[i];//apo8ikeuetai apo ton buffer to orisma tou client
                }
                flagserv=1;//flag to opoio xrisimopoieitai gia ton elegxo tis xrisis upiresias tou server
            }
            bzero(buffer,255);//ka8arismos tou buffer pou xrisimopoieitai gia tin epikoinwnia client/server
            n = write(newsockfd,"Enter Protocol[enter if blank]:",32);
            n = read(newsockfd,buffer,255);//o server dexetai to input tou client
            if (n < 0) error("ERROR reading from socket");
            if (buffer[1]!='\n')//elegxos an o client den edwse orisma
            {
                for (i=0;i<=255;i++)//diatrexei oles tis 8eseis tou buffer mexri na dei trash
                {
                    if (buffer[i+2]=='\0')//elegxos tou buffer(apotrepei ta trash apo to na diabastoun ws clientdata)
                    {
                        break;
                    }
                    test.tempprot+=buffer[i];//apo8ikeuetai apo ton buffer to orisma tou client
                }
                flagprot=1;//flag to opoio xrisimopoieitai gia ton elegxo tis xrisis upiresias tou server
            }

            if (flagname==1)//elegxei an i upiresia ziti8ike apo ton client
            {
                test.hp = gethostbyname(test.hostname.c_str());//ektelesi tis gethostbyname kai apo8ikeusi tis
                if (!test.hp)//elegxos gia to antistoixei o host se kapoia ip
                {
                    n = write(newsockfd,"Unknown Host\n",14);
                }
                else
                {
                    n = write(newsockfd,"name: ",6);//apostolei dedomenwn ston client
                    n = write(newsockfd,test.hp->h_name,strlen(test.hp->h_name));//apostolei dedomenwn ston client
                    n = write(newsockfd,"\nalias: ",9);//apostolei dedomenwn ston client
                    while (*test.hp->h_addr_list)//elegxos an uparxoun panw apo 1 ips
                    {
                        bcopy(*test.hp->h_addr_list++, (char *) &test.hostip, sizeof(test.hostip));
                        n = write(newsockfd,"\naddress: ",10);//apostolei dedomenwn ston client
                        n = write(newsockfd,inet_ntoa(test.hostip),strlen(inet_ntoa(test.hostip)));//apostolei dedomenwn ston client
                    }
                }
            }

            if (flagip==1)//elegxei an i upiresia ziti8ike apo ton client
            {

                test.ph = gethostbyaddr((char *)&test.justh ,sizeof(test.justh), AF_INET );//ektelesi gethostbyname kai apo8ikeusi apotelesmatwn
                if (!test.ph)//elegxos an uparxei auti i ip
                {
                    n = write(newsockfd,"\nUnknown IP address\n",22);
                }
                else
                {
                    n = write(newsockfd,"Hostname: ",10);//apostolei dedomenwn ston client
                    n = write(newsockfd,test.ph->h_name,strlen(test.ph->h_name));//apostolei dedomenwn ston client
                    n = write(newsockfd,"\nAddress: ",11);//apostolei dedomenwn ston client
                    n = write(newsockfd,iptemp.c_str(), strlen(iptemp.c_str()));//apostolei dedomenwn ston client
                    exit(0);
                }
            }
            int tmp=0;
            char str[5];

            if (flagserv == 1 && flagprot == 1)//elegxei an i upiresia ziti8ike apo ton client
            {
                test.serv = getservbyname(test.tempserv.c_str(), test.tempprot.c_str() );//ektelesi getservbyname kai apo8. apot.
                if (test.serv == NULL)//elegxos an ta services i protocolla uparxoun
                {
                    n = write(newsockfd,"Unknown Service or Protocol\n",29);
                }
                else
                {
                    n = write(newsockfd,"\nService: ",11);//apostolei dedomenwn ston client
                    n = write(newsockfd,test.serv->s_name,strlen(test.serv->s_name));//apostolei dedomenwn ston client
                    n = write(newsockfd,"\nPort: ",8);//apostolei dedomenwn ston client
                    tmp = ntohs(test.serv->s_port); //cast se int to s_port
                    sprintf(str, "%d", tmp);//cast apo int se char ga na to dextei to socket
                    n = write(newsockfd, str, strlen(str));//apostolei dedomenwn ston client
                    n = write(newsockfd,"\nProtocol: ",12);//apostolei dedomenwn ston client
                    n = write(newsockfd,test.serv->s_proto,strlen(test.serv->s_proto));//apostolei dedomenwn ston client
                }

            }
            write(newsockfd,"\n***Client will now terminate***\n", 35);
            close(newsockfd);//kleisimo eswterikou socket
            exit(0);
        }

    }

}
void shotgun(int signo)/*Synarthsh termatismou twn zombie-processes*/
{
    pid_t pid;
    signal( SIGCHLD, shotgun );

    while (( pid = waitpid( -1, NULL , WNOHANG )) > 0 )
        printf( "---> [PID: %d] Child process terminated.\n", pid );

}
