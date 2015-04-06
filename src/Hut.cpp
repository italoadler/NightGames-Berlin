//
//  Hut.cpp
//  http_example
//
//  Created by Phoenix Perry on 4/4/15.
//
//

#include "Hut.h"



Hut::Hut(){
    serial_reader = new SerialReader();
    serial_reader->setup();
    for(int i=0; i <NUM_SENSORS; i++){
        int pad_low = 0;
        int pad_high =0;
        int pad = 0;
        padsLow[i] =pad_low;
        padsHigh[i] = pad_high;
        pads[i] = pad;
        calibrate_pad[i] = false;
    }
//    ofAddListener(calibrated, this, &Hut::padCalibrated);
//    ofAddListener(startCalibration, this, &Hut::startCalibrationProcess);
    
}

void Hut::update(){
    serial_reader->update();
    
    ///just for testing change these before shipping code
    pads[0] = serial_reader->pads[0];
    pads[1] =serial_reader->pads[3];
    pads[2] = serial_reader->pads[4];
    pads[3] = serial_reader->pads[5];
    pads[4] = serial_reader->pads[7];
    pads[5] = serial_reader->pads[8];
    pads[6] = serial_reader->pads[10];
    
    
}
void Hut::draw(){
    currentTime = ofGetElapsedTimeMillis();
   
    if(calibrate_state){
        if(calibrate_pad[calibrateNum]==false && keyPress!='0' && welcomeMessage) {
            ofDrawBitmapString("Hello Adelle! :) Welcome to calibration mode for the hut.\nThis isn't super tested code.\nI suggest not mulitasking or pressing any keys that are not asked for.\n\nThere is a variable called duration. You can change that to change the number of seconds \nyou calibrate for to make this easier. \n\nTo start, press 0 to calibrate the 0 pad.", 100,100);
            }
        calibrate();
    }
}

void Hut::calibrate(){
        ofSetColor(255,255,0);
    if(bullshitPress){
        ofSetColor(200, 20, 255);
        ofDrawBitmapString("not a valid key. please follow the guide ok? Hit any pad to resume calibration at that location", 100, 300);
        ofSetColor(200, 200,0);
    }
    //main loop
    
    //if they key pressed recently is equal to the current press we are testing for and this pad is uncalibrated, set it to calibrate.
    if(keyPress==currentPress && calibrate_pad[calibrateNum]==false) {
        calibrate_pad[calibrateNum] = true;
        welcomeMessage = false;
    }
    //if you still have time and the key press is the one we are resting for still and this pad is set to calibrate, let the user we are reading the senseor data
    if(currentTime < endTime && currentPress==keyPress &&calibrate_pad[calibrateNum]){
            ofDrawBitmapString("Get clear of pad "+ ofToString(keyPress) + " I'm now calibrating the untouched state for the next "+ ofToString(duration/1000) +" seconds.", 100, 120);
        
            //save the most recent data
            current_data = pads[calibrateNum];
            // if that data is larger than our saved record, repleace
            // this pad's high data with current data.
            if(current_data > world_record){
                padsHigh[calibrateNum] = current_data;
                world_record = current_data;
               // cout << "setting high"<<endl;
            }
        //if we are out of time, let's now record for the touched state aka the lower data point of this pad.
        //currentPress == keyPress (if we are actually pressing the key we want to test for
        //do calibrate this pad
        }else if (currentTime > endTime && currentPress==keyPress &&calibrate_pad[calibrateNum]) {
            ofDrawBitmapString("pad" + ofToString(currentPress)+ " is set to a high of " + ofToString(padsHigh[calibrateNum]), 10, 140);
            //use the letter A as a switch
            ofDrawBitmapString("press" + ofToString(subState)+ " to calibrate a touched state for pad "+ ofToString(currentPress)+  "touch as much of the sensor during the calibration as possible. ", 10, 150);
        }
    
    cout << currentTime << " " << endTime <<" end time " <<keyPress << " key pressed " << subState << "sub state "<< calibrateNum << " calibrate pad "<< " is eiether on /off " << calibrate_pad[calibrateNum]<<endl;
    // if we have time and the key pressed is  equal to the substate A
    // and we are calibrating, check current data against saved data and save the lower of the two.
    if(currentTime < endTime && keyPress==subState && calibrate_pad[calibrateNum]){
        cout << keyPress << " fired here" << endl;
            ofDrawBitmapString("I'm now calibrating the touched state for the next "+ofToString(duration/1000) +" seconds.", 100, 120);
            current_data = pads[calibrateNum];
        
            //if the data is less that saved data replace it
            if(current_data < world_record){
                padsLow[calibrateNum] = current_data;
                world_record = current_data;
                //cout << "setting low"<<endl;
            //    cout << padsLow[0] << " low num " << pads[0] << " pad 0" << endl;
            }
       //else if we are out of time move on. there's a check to make sure
        //we are not above 6 pads
        }else if (currentTime > endTime &&keyPress == subState &&
                  calibrate_pad[calibrateNum] && calibrateNum < NUM_SENSORS+1)
        {
            ofDrawBitmapString("pad" +ofToString(calibrateNum)+ " is set to a low of " + ofToString(padsLow[calibrateNum]), 100, 140);
            ofSetColor(255, 39, 9);
            ofDrawBitmapString("Let's move on to pad "+ofToString(calibrateNum+1) + " Press " +ofToString(calibrateNum+1) +" to calibrate pad " + ofToString(calibrateNum+1), 100, 160);
            calibrate_pad[calibrateNum+1] = true;
        }
    if(calibrateNum == 6){
        ofSetColor(29, 129, 193);
        ofDrawBitmapString("press 7 to desplay saved data and move on", 100,180);
    }
        if(calibrateNum == 7 ){
            ofSetColor(255,100,100);
            ofDrawBitmapString("Calibration complete!",100,10);
            for(int i=0; i<NUM_SENSORS; i++){
                
                ofDrawBitmapString("pad " + ofToString(i) + " has a low of "+ofToString(padsLow[i]) + " and a high of " +ofToString(padsHigh[i]) + " press " + ofToString(calibrateNum+1) +" to exit this mode and play", 100, 20*i);
            }
         }
    

}

void Hut::keyReleased(ofKeyEventArgs &key){
    keyPress = (char)key.keycode;
  //  cout << keyPress << "is key press ";
    if(calibrate_state)
    {
        if(welcomeMessage && keyPress=='0')
        {
            endTime = currentTime+duration;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            bullshitPress = false;
            calibrateNum = 0;
            calibrate_pad[calibrateNum]=true;
            
        } else if(calibrate_pad[calibrateNum] && keyPress == '1'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '2'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '3'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '4'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        }else if(calibrate_pad[calibrateNum] == true && keyPress == '5'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '6'){
            calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            endTime = currentTime+duration;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '7'){
            //calibrate_pad[calibrateNum]=false;
            calibrateNum++;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            bullshitPress = false;

        } else if(calibrate_pad[calibrateNum] == true && keyPress == '8'){
            //calibrate_pad[calibrateNum]=false;
            calibrate_state = false;

        } else if(calibrate_pad[calibrateNum] && keyPress == 'A'){
            bullshitPress = false;
            endTime = currentTime+duration;
            cout << endTime << "end time"<<endl;
            stringstream ss;
            ss << calibrateNum;
            string press = ss.str();
            char c = ofToChar(press);
            currentPress = c;
            cout << c << "was presseed" <<endl;
            
        } else {
            bullshitPress = true;
        }
    }
}
