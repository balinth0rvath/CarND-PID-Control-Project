# **PID Control Project** 

---
### Reflection

#### 1. Describe the effect each of the P, I, D components had in your implementation.

P is the proportional value. If it is too small the vehicle cannot drive smooth path in curves because it cannot keep the center line well. If it is too large the vehicle starts to oscillate.
I is the integral value. Its value keeps growing when the car is constanly on a particular side of center line, for example there can be a bias towards to left or right. I found it useful correcting sharp curves.
D is the differential value. It prevents overshooting keeping a force against the proportional effect. The car started wobbling if it wasnt tuned well
#### 2. Describe how the final hyperparameters were chosen.
I have implemented twiddle using a state machine. It is in the main for simplicity. The error was calculated after every full lap. The error is the sum of square of total error. Total error is -Kp * p_error -Kd * d_error - Ki * i_error after each relevant websocket message.
I found that error sums cannot be compared unless the car goes through exactly the same path in every measurements. So the sum of errors is always measured after a full lap. Also sometimes the car left the road so roughly tuned twiddle starting paramters should be given manually first.



### Simulatio

#### 1. The vehicle must successfully drive a lap around the track.


