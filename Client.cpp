#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <vector>
#include <map>
#include <unistd.h>

using namespace std;


     
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int dealer = 0;
    int player = 0;
    bool hit = true;
    bool game_over = false;
    bool holding = false;
    vector<string> card_vec;
    map<string,int> card_map;  
    map<string,int> val_map;

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
    
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    srand(time(NULL));
    int randIndex = rand()%card_map.size();
            
    bzero(buffer,256);
    
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
     
    dealer += val_map[buffer];
    cout<<"Dealer's card is: "<<buffer<<"\nDealer's total is: "<<dealer<<"\n"<<endl;
    
    std::map<string,int>::iterator it;
       
    it = card_map.find(card_vec.at(randIndex));

    char *temp; 
       
    temp = (char *)it->first.c_str();

    card_vec.erase(card_vec.begin()+randIndex);

    card_map.erase (it);
    player += val_map[temp];
    cout<<"Your card is: "<<temp<<"\nYour total is: "<<player<<"\n"<<endl;
    n = write(sockfd,temp,sizeof(temp));

    temp = NULL;
    if (n < 0) error("ERROR writing to socket");

       
    
    while(hit && !game_over && !holding)
    {    
        bzero(buffer,256);

        n = read(sockfd,buffer,255);
        if (n < 0) 
            error("ERROR reading from socket");
        if(atoi(buffer) == 0)
        {
            holding = true;
            cout<<"Dealer is holding\n"<<endl;
        }
        else
        {       
            dealer += val_map[buffer];
            cout<<"Dealer's card is: "<<buffer<<"\nDealer's total is: "<<dealer<<"\n"<<endl;
        }
        
        if(dealer > 21 || player > 21)
        {
            game_over = true;
            hit = false;
        }
        else
        {
            cout<<"Choose 1 to hit or 2 to hold: ";
            bzero(buffer,256);
            cin.getline(buffer,256); 
            if(atoi(buffer) == 2)
            {
                hit = false;
                cout<<"You are holding\n"<<endl;
                n = write(sockfd,"0",1);
                if (n < 0) 
                    error("ERROR writing to socket");
            }
            else
            {
                srand(time(NULL));
                randIndex = rand()%card_map.size();
                it = card_map.find(card_vec.at(randIndex));

                char *temp; 

                temp = (char *)it->first.c_str();

                card_vec.erase(card_vec.begin()+randIndex);

                card_map.erase (it);
                player += val_map[temp];
                cout<<"Your card is: "<<temp<<"\nYour total is: "<<player<<"\n"<<endl;
                n = write(sockfd,temp,sizeof(temp));
                if (n < 0) 
                    error("ERROR writing to socket");
            }
        }
    }
    
    while(!holding && !hit)
    {
        
       
       bzero(buffer,256);

       n = read(sockfd,buffer,255); 
       if (n < 0) 
            error("ERROR reading from socket");
       if(atoi(buffer) == 0)
       { 
            holding = true;
            game_over = true;
       }
       else
       {
            dealer += val_map[buffer];
            cout<<"Dealer's card is: "<<buffer<<"\nDealer's total is: "<<dealer<<"\n"<<endl;            
            
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
        player += atoi(temp);
        cout<<"Your card is: "<<temp<<"\nYour total is: "<<player<<"\n"<<endl;
        n = write(sockfd,temp,sizeof(temp));
        if (n < 0) 
            error("ERROR writing to socket");
       
       bzero(buffer,256);

       n = read(sockfd,buffer,255);
       if (n < 0) 
            error("ERROR reading from socket");
       
       if(atoi(buffer) == 0)
       {
           cout<< "Dealer is holding"<<endl;
       }
       
        cout<<"Dealer's total is: "<<dealer<<"\n"<<endl;
        cout<<"Choose 1 to hit or 2 to hold: ";
        bzero(buffer,256);
        
        cin.getline(buffer,256); 
        if(atoi(buffer) == 2)
        {
            hit = false;
            game_over = true;
        }
    }
    
    if(game_over)
    {
        if(dealer > 21)
        {
            cout<< "You win!\nCeaser's Palace Loses" << endl;
        }
        else if(player > 21)
        {
            cout<<"Ceaser's Palace Wins!\nYou lose!"<<endl;
        }
        else if(player > dealer)
        {
            cout<< "You win!\nCeaser's Palace Loses" << endl;
        }
        else
        {
            cout<<"Ceaser's Palace Wins!\nYou lose!"<<endl;
        }
    }
    
    
    usleep(1);
    close(sockfd);
    return 0;
}