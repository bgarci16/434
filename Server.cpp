/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <iostream>
#include <time.h>
#include<vector>
#include <unistd.h>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    int dealer = 0;
    int player = 0;
    bool hit = true;
    bool game_over = false;
    bool holding = false;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // srand(time(NULL));

     vector<string> card_vec;
     map<string,int> card_map;
     map<string,int> val_map;
     //std::map<string,int>::iterator it;   
   
     card_vec.push_back("2");
     card_vec.push_back("3");
     card_vec.push_back("4");   
     card_vec.push_back("5");
     card_vec.push_back("6");
     card_vec.push_back("7");
     card_vec.push_back("8");
     card_vec.push_back("9");
     card_vec.push_back("10");
     card_vec.push_back("J");
     card_vec.push_back("K");
     card_vec.push_back("Q");
     card_vec.push_back("A");


     card_map["2"] = 2;
     card_map["3"] = 3;
     card_map["4"] = 4;
     card_map["5"] = 5;
     card_map["6"] = 6;
     card_map["7"] = 7;
     card_map["8"] = 8;
     card_map["9"] = 9;
     card_map["10"] =10;
     card_map["J"] = 10;
     card_map["K"] = 10;
     card_map["Q"] = 10;
     card_map["A"] = 1; 
     
     val_map["2"] = 2;
    val_map["3"] = 3;
    val_map["4"] = 4;
    val_map["5"] = 5;
    val_map["6"] = 6;
    val_map["7"] = 7;
    val_map["8"] = 8;
    val_map["9"] = 9;
    val_map["10"] =10;
    val_map["J"] = 10;
    val_map["K"] = 10;
    val_map["Q"] = 10;
    val_map["A"] = 1;

    // it = card_map.find("2"); 

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     srand(time(NULL));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);
     
     int randIndex = rand()%card_map.size();    
     
     std::map<string,int>::iterator it;
       
     it = card_map.find(card_vec.at(randIndex));

     char *temp; 
            
     temp = (char *)it->first.c_str();
     
     card_vec.erase(card_vec.begin()+randIndex);

     card_map.erase (it);
     dealer += val_map[temp];
     cout<<"Your card is: "<<temp<<"\nYour total is: "<<dealer<<"\n"<<endl;
     n = write(newsockfd,temp,sizeof(temp));
   
     temp = NULL;
     if (n < 0) error("ERROR writing to socket");
     
     while(hit && !game_over && !holding) 
     {
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if(atoi(buffer) == 0)
        {
            holding = true;
            cout<<"Player is holding\n"<<endl;
        }
        else
        {
            srand(time(NULL));

            int randIndex = rand()%card_map.size();
            player += val_map[buffer];
            cout<<"Player's card is: "<<buffer<<"\nPlayer's total is: "<<player<<"\n"<<endl;     

            
        }
        
        std::map<string,int>::iterator it;

            it = card_map.find(card_vec.at(randIndex));

            char *temp; 

            temp = (char *)it->first.c_str();

            card_vec.erase(card_vec.begin()+randIndex);

            card_map.erase (it);
            dealer += val_map[temp];
            cout<<"Your card is: "<<temp<<"\nYour total is: "<<dealer<<"\n"<<endl;
            n = write(newsockfd,temp,sizeof(temp));



            temp = NULL;
            if (n < 0) error("ERROR writing to socket");
        
        if(dealer > 17 && dealer <= 21)
        {
            hit = false;
            cout<<"You are holding\n"<<endl;
            n = write(newsockfd,"0",1);
            if (n < 0) 
                error("ERROR writing to socket");
        }
        else if(dealer > 21 || player > 21)
        {
            hit = false;
            game_over = true;
        }

    }

    while(!holding && !hit)
    {
       bzero(buffer,256);

       n = read(newsockfd,buffer,255);
       if (n < 0) 
            error("ERROR reading from socket");
       if(atoi(buffer) == 0)
       {
           holding = true;
           game_over = true;
       }
       else
       {
            player += val_map[buffer];
            cout<<"Player's card is: "<<buffer<<"\nPlayer's total is: "<<player<<"\n"<<endl;
       }
            
    }
    
    while(hit  && holding)
    {
        
        srand(time(NULL));
        randIndex = rand()%card_map.size();
        it = card_map.find(card_vec.at(randIndex));

        char *temp; 

        temp = (char *)it->first.c_str();

        card_vec.erase(card_vec.begin()+randIndex);

        card_map.erase (it);
        dealer += atoi(temp);
        cout<<"Your card is: "<<temp<<"\nYour total is: "<<dealer<<"\n"<<endl;
        n = write(newsockfd,temp,sizeof(temp));
        if (n < 0) 
            error("ERROR writing to socket");
       
       bzero(buffer,256);

       n = read(newsockfd,buffer,255);
       if (n < 0) 
            error("ERROR reading from socket");
       
       if(atoi(buffer) == 0)
       {
           cout<< "Player is holding"<<endl;
       }
       
        cout<<"Player's total is: "<<player<<"\n"<<endl;
        
        if(dealer > 17 && dealer <= 21)
        {
            hit = false;
        }
        else if(dealer > 21 || player > 21)
        {
            game_over = true;
            hit = false;
        }
        
    }
    
    if(game_over)
    {
        if(dealer > 21)
        {
            cout<< "Player wins!\nCeaser's Palace Loses" << endl;
        }
        else if(player > 21)
        {
            cout<<"Ceaser's Palace Wins!\nPlayer loses!"<<endl;
        }
        else if(player > dealer)
        {
            cout<< "Player wins!\nCeaser's Palace Loses" << endl;
        }
        else
        {
            cout<<"Ceaser's Palace Wins!\nPlayer loses!"<<endl;
        }
    }
    usleep(1);
    close(sockfd);
    close(newsockfd);
     
     
     return 0; 
}