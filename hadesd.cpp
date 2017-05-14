#include <iostream>
#include <vector>
#include <thread>
#include <signal.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include <sqlite3.h>
#include "constants.h"
#define LOG
#include "log.h"
#include "init.h"

using namespace std;
using namespace cv;
using namespace cv::face;

int inform(enum folks folk);

//int label = 0;

vector<Mat> images;
vector<int> labels;
Ptr<FaceRecognizer> model;

vector<Mat> suspects;

int i_n = 0;
char *zErrMsg = 0;
int sqlite_callback(void *NotUsed, int argc, char**argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		logger("callback for sqlite3, executed the command")(INFO);
		//TODO print executed command
	}
	return 0;
}

int load_gui()
{
	logger("initializing GUI")(INFO);
	cv::namedWindow("Hades", WINDOW_AUTOSIZE);
	//create gui via opencv through highgui
	return 0;

}
void *start_dvr(void *L){
	logger("DVR started")(INFO);
	VideoCapture cap;
	if(!cap.open(0)){
		logger("cannot open CAM")(ERROR);
		return NULL;
	}
	cv::Mat image;
	model = createEigenFaceRecognizer();
	cv::Mat grayMat;
	//
	cap >> image;
	images.push_back(image);
	labels.push_back(i_n++);
	model -> train(images, labels);
	logger("First frame is given as sample for training")(ERROR);
	while(true){ 
		cap >> image;
		cv::imshow("Hades", image);
		cv::waitKey(100);
		//this function is used to process frames
		//verification part
		cv::cvtColor(image, grayMat, cv::COLOR_BGR2GRAY);
		int predicted = -1;
		if( (predicted = model -> predict(grayMat)) == 0){
			//new member
			if(false){ //FIXME enter only if the detected member is a new member
				images.push_back(image);
				labels.push_back(i_n++);
				model->train(images, labels);
			}
			else {
				suspects.push_back(grayMat);
				logger("warn the user")(INFO);
				//TODO alert through app
			}
		}	
		//logger("predicted = %d", predicted)(INFO);
	}
	return NULL;
}

sqlite3 *db;

void exit_operations(){
	printf("\n");
	logger("writing to Database...please wait")(INFO);
	//sqlite3_exec(db, "select * from table1", sqlite_callback, 0 ,&zErrMsg);
	sqlite3_free(zErrMsg);
	sqlite3_close(db);
	//write suspect and persons to db and exit
	exit(0);
}

void exitHandler(int sig){
	exit_operations();
}


int init_db(){
	if(sqlite3_initialize() != SQLITE_OK){
		logger("cannot initialize sqlite3 library")(CRITICAL);
		return -1;
	}
	int rc = sqlite3_open("db/db.sqlite3",&db);
	if(rc){
		logger("can't open database, try giving previleges to the file")(ERROR);
		return -1;
	}else{
		logger("database is opened")(INFO);
		return 0;
	}
}
//returns 0 on success
//loads images, labels, suspects, suspectsl
int load_trained_data(){
	//retreive path form sql and push to stacks 
	return 0;
}

void *find_user(void * L){
	logger("Finding user in the internet")(INFO);
	return NULL;
}

int main(int argc, char *argv[]){
	TAG("Hades")(LOG_INIT | LOG_COLOR);//for logging library
	init_hades();
	signal(SIGINT, exitHandler);
	//load_gui via opencv
	if(init_db()< 0) //TODO clean up return types **db_error
		return -1;
	logger("Database loaded successfully")(INFO);
	load_gui();

	if(load_trained_data() != 0){ //no trained data
		//TODO train data
	}
	//capture from camera
	//process the frames
	pthread_t dvr_thread;
	pthread_create(&dvr_thread, NULL, start_dvr, NULL);
	
	pthread_t find_user_thread;
	pthread_create(&find_user_thread, NULL, find_user, NULL);

	logger("Main thread continues")(INFO);
	//if the stored person is in DB do not alert the home
	//else if check he is breaking in to the house the inform(folks) 
	enum folks folk = USER;
	while(inform(folk) == -1){}

	//async--> if there is a fire in the house. 
	//NOTE alloted for Dinesh Kumar , DHT22 :{}
	pthread_join(dvr_thread, NULL);
	pthread_join(find_user_thread, NULL);
}


// inform the folks in the home that there is a stranger
int inform(enum folks folk){
	if(folk == USER){
		//alert the user
		return 0;
	}
	else if(folk == POLICE){
		//alert the police and user
		return 0;
	}
	else if(folk == ALL){
		//alert user and police
		return 0;
	}
	else return -1; //TODO rather returning failure, try to inform the user using messgage like hike!
}
