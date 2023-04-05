#ifndef _STEPPER_MOTOR_H_
#define _STEPPER_MOTOR_H_
#include <functional>
#include "global.h"
class StepperMotor{
	private:
		std::function<uint32_t(uint8_t, uint32_t, uint8_t)> fcSetup;
		std::function<void(uint8_t, uint8_t)> fcAttPin;
		std::function<void(uint8_t, uint32_t)> fcWrite;
		std::function<uint32_t(uint8_t, uint32_t)> fcWriteTone;
		uint8_t pin;
		uint8_t chan;
		uint8_t bit_num;
		long accel;
		long endSpeed  = 0;  // конечная частота
		long currSpeed = 0; // текущая частота
		bool runMode = false;
		int  pinDir;
	// double ledcSetup(uint8_t chan, double freq, uint8_t bit_num)
	// void ledcAttachPin(uint8_t pin, uint8_t chan)
	// void ledcWrite(uint8_t chan, uint32_t duty)
	// double ledcWriteTone(uint8_t chan, double freq)
	// ledcWrite(0, 50);
	// ledcWriteTone(0, frequency);
	public:
	long pulseMl = 0;
	double counter;
	double counterStep;
	explicit StepperMotor(uint32_t(*setupFunc)(uint8_t, uint32_t, uint8_t),
							void(*attachPinFunc)(uint8_t, uint8_t),
							void (*ledcWriteFunc)(uint8_t , uint32_t ),
							uint32_t(*ledcWriteToneFunc)(uint8_t, uint32_t),
							uint8_t pin,
							uint8_t chan,
							uint8_t bit_num,
							long accel,
							int pinDir
							){
		fcSetup = setupFunc;	
		fcAttPin = attachPinFunc;
		fcWrite = ledcWriteFunc;
		fcWriteTone = ledcWriteToneFunc;
		this->pin = pin;
		this->chan = chan;
		this->bit_num = bit_num;	
		this->accel = accel;
		fcSetup(chan, 10, bit_num);
		fcAttPin(pin, chan);
		fcWrite(chan, 0);
		this->pinDir = pinDir;
		this->counter = 0;
	}

	void setFrequency(long freq){
		endSpeed = freq;
		if(endSpeed == 0){
			stop();
		}
		//fcWriteTone(chan, freq);
	}
	void stop(void){
		if(runMode){
			currSpeed = 0;
			endSpeed = 0;
			fcWriteTone(chan, 0);
			runMode = false;
		}
	}
	
	void run(void){
		static unsigned long t_loop = millis();
		static unsigned long t_counter = millis();
		if(millis() - t_loop >= 10UL){
			t_loop = millis();
			if(endSpeed != currSpeed){
				if(endSpeed > 0){
					runMode = true;
				}
			}
			if(runMode){
				if(currSpeed > endSpeed){ // здесь понижаем обороты
					if( ( abs(currSpeed - accel) >= endSpeed) ) {
						currSpeed -= accel;
					}
					else{
						currSpeed = endSpeed;
					}
					fcWriteTone(chan, currSpeed);
				}
				else if(currSpeed < endSpeed){ // здесь увеличиваем обороты
					if( (currSpeed + accel) < endSpeed ){
						currSpeed += accel;
					}
					else{
						currSpeed = endSpeed;
					}
					fcWriteTone(chan, currSpeed);
				}
				else{

				}
			}

			{
				unsigned long samplingTime;
				if(currSpeed > 0){
					samplingTime = millis() - t_counter;
					t_counter = millis();
					if(digitalRead(pinDir) == HIGH){
						counterStep += ((double)samplingTime * 0.001 * (double)currSpeed);
						counter -= ((double)samplingTime * 0.001 * (double)currSpeed) / (double)pulseMl;
					}
					else if(counter > 0){
						counter += ((double)samplingTime * 0.001 * (double)currSpeed) / (double)pulseMl;
					}
				}
				else{
					t_counter = millis();
				}
			}
		}
	}





	// void run(void){
	// 	static unsigned long t = millis();
	// 	static unsigned long speed;
	// 	if(millis() - t >= 10UL){
	// 		t = millis();
	// 		if(runMode){
	// 			if(currSpeed < endSpeed){
	// 				currSpeed += accel;
	// 				if(currSpeed > endSpeed) currSpeed = endSpeed;
	// 				fcWriteTone(chan, currSpeed);
	// 			}
	// 			else if(currSpeed > endSpeed){
	// 				currSpeed -= accel;
	// 				if(currSpeed < endSpeed) currSpeed = endSpeed;
	// 				fcWriteTone(chan, currSpeed);
	// 			}
	// 			// if(startSpeed < endSpeed){
	// 			// 	startSpeed += accel;
	// 			// 	if(startSpeed > endSpeed) startSpeed = endSpeed;
	// 			// 	fcWriteTone(chan, startSpeed);
	// 			// }
	// 			// else{
	// 			// 	startSpeed = endSpeed;
	// 			// 	fcWriteTone(chan, startSpeed);
	// 			// }
	// 		}


	// 	}
	// }

	// void start(	unsigned long accel, unsigned long startSpeed, unsigned long endSpeed){
	// 	this->accel = accel;
	// 	// this->startSpeed = startSpeed;
	// 	this->endSpeed = endSpeed;
	// 	this->currSpeed = startSpeed;
	// 	runMode = true;
	// }

	








};


#endif