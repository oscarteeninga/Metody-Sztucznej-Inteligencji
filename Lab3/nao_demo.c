/*
 * Copyright 1996-2020 Cyberbotics Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//---------------------------------------------------------------------------------
//  Description:  Example C controller program for Nao robot.
//                This demonstrates how to access sensors and actuators
//---------------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <webots/accelerometer.h>
#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/gps.h>
#include <webots/gyro.h>
#include <webots/inertial_unit.h>
#include <webots/keyboard.h>
#include <webots/led.h>
#include <webots/motor.h>
#include <webots/robot.h>
#include <webots/touch_sensor.h>
#include <webots/utils/motion.h>

#ifdef _MSC_VER
#define snprintf sprintf_s
#endif

#define PHALANX_MAX 8

static int time_step = -1;

// simulated devices
static WbDeviceTag CameraTop, CameraBottom;  // cameras
static WbDeviceTag us[2];                    // ultra sound sensors
static WbDeviceTag accelerometer, gps, gyro, inertial_unit;
static WbDeviceTag fsr[2];                        // force sensitive resistors
static WbDeviceTag lfoot_lbumper, lfoot_rbumper;  // left foot bumpers
static WbDeviceTag rfoot_lbumper, rfoot_rbumper;  // right foot bumpers
static WbDeviceTag leds[7];                       // controllable led groupsstatic WbDeviceTag lphalanx[PHALANX_MAX];
static WbDeviceTag rphalanx[PHALANX_MAX];         // right hand motors
static WbDeviceTag lphalanx[PHALANX_MAX];         // left hand motors
static WbDeviceTag RShoulderPitch;
static WbDeviceTag LShoulderPitch;

// motion file handles
static WbMotionRef hand_wave, forwards, backwards, side_step_left, side_step_right, turn_left_60, turn_left_180, turn_right_60, forward, shoot;
static WbMotionRef currently_playing = NULL;

static double maxPhalanxMotorPosition[PHALANX_MAX];
static double minPhalanxMotorPosition[PHALANX_MAX];

static void find_and_enable_devices() {
  // camera
  CameraTop = wb_robot_get_device("CameraTop");
  CameraBottom = wb_robot_get_device("CameraBottom");
  wb_camera_enable(CameraTop, 4 * time_step);
  wb_camera_enable(CameraBottom, 4 * time_step);

  // accelerometer
  accelerometer = wb_robot_get_device("accelerometer");
  wb_accelerometer_enable(accelerometer, time_step);

  // gyro
  gyro = wb_robot_get_device("gyro");
  wb_gyro_enable(gyro, time_step);

  // gps
  gps = wb_robot_get_device("gps");
  wb_gps_enable(gps, time_step);

  // inertial unit
  inertial_unit = wb_robot_get_device("inertial unit");
  wb_inertial_unit_enable(inertial_unit, time_step);

  // ultrasound sensors
  us[0] = wb_robot_get_device("Sonar/Left");
  us[1] = wb_robot_get_device("Sonar/Right");
  int i;
  for (i = 0; i < 2; i++)
    wb_distance_sensor_enable(us[i], time_step);

  // foot sensors
  fsr[0] = wb_robot_get_device("LFsr");
  fsr[1] = wb_robot_get_device("RFsr");
  wb_touch_sensor_enable(fsr[0], time_step);
  wb_touch_sensor_enable(fsr[1], time_step);

  // foot bumpers
  lfoot_lbumper = wb_robot_get_device("LFoot/Bumper/Left");
  lfoot_rbumper = wb_robot_get_device("LFoot/Bumper/Right");
  rfoot_lbumper = wb_robot_get_device("RFoot/Bumper/Left");
  rfoot_rbumper = wb_robot_get_device("RFoot/Bumper/Right");
  wb_touch_sensor_enable(lfoot_lbumper, time_step);
  wb_touch_sensor_enable(lfoot_rbumper, time_step);
  wb_touch_sensor_enable(rfoot_lbumper, time_step);
  wb_touch_sensor_enable(rfoot_rbumper, time_step);

  // There are 7 controlable LED groups in Webots
  leds[0] = wb_robot_get_device("ChestBoard/Led");
  leds[1] = wb_robot_get_device("RFoot/Led");
  leds[2] = wb_robot_get_device("LFoot/Led");
  leds[3] = wb_robot_get_device("Face/Led/Right");
  leds[4] = wb_robot_get_device("Face/Led/Left");
  leds[5] = wb_robot_get_device("Ears/Led/Right");
  leds[6] = wb_robot_get_device("Ears/Led/Left");

  // get phalanx motor tags
  // the real Nao has only 2 motors for RHand/LHand
  // but in Webots we must implement RHand/LHand with 2x8 motors
  for (i = 0; i < PHALANX_MAX; i++) {
    char name[32];
    sprintf(name, "LPhalanx%d", i + 1);
    lphalanx[i] = wb_robot_get_device(name);
    sprintf(name, "RPhalanx%d", i + 1);
    rphalanx[i] = wb_robot_get_device(name);

    // assume right and left hands have the same motor position bounds
    maxPhalanxMotorPosition[i] = wb_motor_get_max_position(rphalanx[i]);
    minPhalanxMotorPosition[i] = wb_motor_get_min_position(rphalanx[i]);
  }

  // shoulder pitch motors
  RShoulderPitch = wb_robot_get_device("RShoulderPitch");
  LShoulderPitch = wb_robot_get_device("LShoulderPitch");

  // keyboard
  wb_keyboard_enable(10 * time_step);
}

// load motion files
static void load_motion_files() {
  hand_wave = wbu_motion_new("../../motions/HandWave.motion");
  forwards = wbu_motion_new("../../motions/Forwards50.motion");
  forward = wbu_motion_new("../../motions/Forwards.motion");
  backwards = wbu_motion_new("../../motions/Backwards.motion");
  side_step_left = wbu_motion_new("../../motions/SideStepLeft.motion");
  side_step_right = wbu_motion_new("../../motions/SideStepRight.motion");
  turn_left_60 = wbu_motion_new("../../motions/TurnLeft60.motion");
  turn_right_60 = wbu_motion_new("../../motions/TurnRight60.motion");
  turn_left_180 = wbu_motion_new("../../motions/TurnLeft180.motion");
  shoot = wbu_motion_new("../../motions/Shoot.motion");
}

static void terminate() {
  // add you cleanup code here: write results, close files, free memory, etc.
  // ...
  wb_robot_cleanup();
}

static void simulation_step() {
  if (wb_robot_step(time_step) == -1)
    terminate();
}

static void wait_steps(WbMotionRef motion) {
  for (int i = 0; i < 2*wbu_motion_get_duration(motion)/time_step; i++) {
    simulation_step();
  }
}

static void start_motion(WbMotionRef motion) {
  // interrupt current motion
  if (currently_playing)
    wbu_motion_stop(currently_playing);

  // start new motion
  wbu_motion_play(motion);
  wait_steps(motion);
  currently_playing = motion;
}

static double clamp(double value, double min, double max) {
  if (min > max) {
    assert(0);
    return value;
  }
  return value < min ? min : value > max ? max : value;
}

static float get_ultrasound_sensors() {
  double dist[2];
  int i;
  for (i = 0; i < 2; i++)
    dist[i] = wb_distance_sensor_get_value(us[i]);

  printf("-----sensors: %f, %f-----\n", dist[0], dist[1]);
  return dist[0] > dist[1] ? dist[1] : dist[0];
}


static void set_all_leds_color(int rgb) {
  // these leds take RGB values
  int i;
  for (i = 0; i < 5; i++)
    wb_led_set(leds[i], rgb);

  // ear leds are single color (blue)
  // and take values between 0 - 255
  wb_led_set(leds[5], rgb & 0xff);
  wb_led_set(leds[6], rgb & 0xff);
}

static void set_hands_angle(double angle) {
  // we must activate the 8 phalanx motors
  int j;
  for (j = 0; j < PHALANX_MAX; j++) {
    double clampedAngle = angle;
    if (clampedAngle > maxPhalanxMotorPosition[j])
      clampedAngle = maxPhalanxMotorPosition[j];
    else if (maxPhalanxMotorPosition[j] < minPhalanxMotorPosition[j])
      clampedAngle = minPhalanxMotorPosition[j];

    if (rphalanx[j])
      wb_motor_set_position(rphalanx[j], clampedAngle);
    if (lphalanx[j])
      wb_motor_set_position(lphalanx[j], clampedAngle);
  }
}

static void print_hello() {
  printf("----------nao_oscar----------\n");
  printf("This is my Nao project\n");
}



static void forward_step() {
  start_motion(forward);
  printf("Made a step!\n");
}


static void turn_around() {
  start_motion(turn_right_60);
  start_motion(turn_right_60);
  start_motion(turn_right_60);

  printf("Turned around!\n");
}

static void shoot_animation() {
  start_motion(shoot);
  printf("Shoooooot!!!!\n");
}

static void hand_wave_animation() {
  start_motion(hand_wave);
  printf("Hi, how you doin???\n");
}


int main() {
  // call this before any other call to a Webots function
  wb_robot_init();

  // simulation step in milliseconds
  time_step = wb_robot_get_basic_time_step();

  printf("------timestep: %d------", time_step);
  // initialize stuff
  find_and_enable_devices();
  load_motion_files();

  // print console hello message
  print_hello();

  // sensors get initialization (bugs without it)
  get_ultrasound_sensors();

  // random gen
  srand(time(NULL));

  while (1) {
    if (get_ultrasound_sensors() < 0.27) {
      set_all_leds_color(0xff0000);
      shoot_animation(); 
      set_all_leds_color(0x000000);
      turn_around();
    } else {
      forward_step();
    }
    if (rand() % 100 > 90) {
      hand_wave_animation();
    }
  }
  return 0;
}
