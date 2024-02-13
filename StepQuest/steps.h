
/*
 * Step Algorithm is to be called via timer interrupt every 20ms
 */

// default threshold
float threshold = 1.0;

int num_steps = 0;

// setting time between each accelerometer reading (20ms), shortest valid time between steps, longest valid time between steps
float delay_time_seconds = 0.02;
float low_bound_seconds = 0.24;
float high_bound_seconds = 0.96;

// convert time in seconds to milliseconds and cycles
float delay_time = delay_time_seconds * 1000;
float low_bound_cycles = low_bound_seconds / delay_time_seconds;
float high_bound_cycles = high_bound_seconds / delay_time_seconds;

// enter the drift of the accelerometer (unique to each MPU6050)
float x_drift = 0.40;
float y_drift = 0.45;
float z_drift = 10;

// offsets the gravity and x,y,z drift in order to get an amplitude close to 0 regardless of orientation
float correction = sqrt(pow((x_drift),2)+pow((y_drift),2)+pow((z_drift),2));

// num cycles since last step
int last_step = 0;

// steps to be added if user moves into walking state
int potential_steps = 0;

// steps threshold to leave back into moving state
int moving_threshold = 20;

// 0 = still, 1 = moving
int state = 0;

// used to compare adjacent peak values (only over the threshold)
// the last peak (next value went down comparatively) is counted as a step
// will also count as the threshold itself (resets to threshold upon a step counted)
float last_peak = threshold;

unsigned int stepAlg(sensors_event_t a) {

  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  // calculate the amplitude
  float amp = sqrt(pow((x),2)+pow((y),2)+pow((z),2)) - correction;

  // check if this value goes up or down compared to previous peak value
  if (amp >= last_peak)
  {
    last_peak = amp;
  }
  else if (last_peak != threshold)// potential step found
  {
    if (last_step > low_bound_cycles) // if fails, likely not a real step
    {
      if (last_step < high_bound_cycles) // continuity maintained
      {
        if (state == 0) // in still state
        {
          potential_steps++;
          if (potential_steps >= 10) // move to moving state
          {
            state = 1;
            num_steps += potential_steps;
          }
        }
        else // in moving state
        {
          num_steps++;
        }
        last_step = 0;
      }
      else // move to still state
      {
        state = 0;
        potential_steps = 1;
        last_step = 0;
      }
    }
    last_peak = threshold; // reset to threshold
  }

  last_step++;

  return num_steps;
}
