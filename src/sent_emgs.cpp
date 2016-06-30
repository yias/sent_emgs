

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define NBDIM 16    // number of dimensions
#define TW 0.15     // length of the time window in milliseconds
#define SR 1500     // samplerate of the EMG signals
#define DELAY 0.05  // delay between the time windows in milliseconds

std::vector<double> line2double(std::string ll){

    std::vector<double> signal(NBDIM, 0);

    int tmp = 0;
    int counter = 0;

    for (int i = 0; i < (int)ll.size(); i++){
        if (ll[i] == ' '){
            std::istringstream conv(ll.substr(tmp,i-1));
            conv>>signal[counter];
            //std::cout << signal[counter] << "\n";
            counter++;
            tmp = i+1;
        }
    }

    //std::cout << ll << "\n";

    return signal;

}


std::string win2str(std::vector< std::vector<double> > mm, int s_start, int s_end){
    std::string stmp;

    std::ostringstream convert;

    if(s_end>(int)mm.size()){
        s_end=(int)mm.size();
    }

    for (int i = s_start; i < s_end; i++){
        for (int j = 0; j < NBDIM; j++){
            convert << mm[i][j] << " ";
        }
        convert << "\n";
    }

    stmp = convert.str();
    //std::cout << stmp;
    return stmp;


}

std::vector< std::string > emgtw(int nb){

    std::vector< std::string > tmp;

    std::string fname("src/sent_emgs/emg_data/trial");

    std::string line;

    std::ostringstream convert;   // stream used for the conversion

    convert << nb;				// insert the textual representation of 'Number' in the characters in the stream

    fname = fname + convert.str() + ".txt";
    std::cout << "name of the file: " << fname <<"\n";
    std::ifstream myfile(fname.c_str());

    std::vector< std::vector <double> > signal;


    if (myfile.is_open()){

        int counter = 0;

        while (getline(myfile, line)){
            std::cout<< counter <<"\n";
            signal.push_back(line2double(line));
            counter++;
        }

        myfile.close();

        std::cout<<"okokokok\n";
    }
    else{
        std::cout << "Unable to open file " << fname << "\n";
    }

    int w_l = TW*SR;
    int d_l = DELAY*SR;

    for (int i = 0; i < (int)signal.size(); i = i + d_l){
        tmp.push_back(win2str(signal,i,i+w_l));
    }

    return tmp;

}



int main(int argc, char** argv){


    ros::init(argc,argv,"sent_emgs");

    ros::NodeHandle n;

    ros::Publisher chatter_pub = n.advertise<std_msgs::String>("daq_pub", 1000);

    ros::Rate loop_rate(1);

    std::vector< std::string > emgtws=emgtw(2);


    int count = 0;

    while (ros::ok()){

        std_msgs::String msg;

//        std::stringstream ss;
//        ss<<emgtws[count];

        //msg.data=ss.str();
        if (count<18){
            msg.data=emgtws[count];
            chatter_pub.publish(msg);
        }


        ros::spinOnce();
        loop_rate.sleep();

        ++count;
    }

}
