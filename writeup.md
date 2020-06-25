# **PID Control Project** 

---
### Reflection

#### 1. Describe the effect each of the P, I, D components had in your implementation.

P is the proportional value. If it is too small the vehicle cannot drive a smooth path in curves because it cannot keep the center line well. If it is too large the vehicle starts to oscillate.
I is the integral value. Its value keeps growing when the car is constanly on a particular side of center line, for example there can be a bias towards to left or right. I found it useful correcting sharp curves.
D is the differential value. It prevents overshooting by keeping a force against the proportional effect. The car started wobbling if it wasnt tuned well.
#### 2. Describe how the final hyperparameters were chosen.
I have implemented twiddle using a state machine. It is in the main for simplicity. The error was calculated after every full lap. The error is the sum of square of total error. Total error is -Kp * p_error -Kd * d_error - Ki * i_error after each relevant websocket message.
I found that error sum values cannot be compared to each other unless the car goes through exactly the same path in every measurements. So the sum of errors is always measured after a full lap. It was very time consuming. Also sometimes the car left the road so roughly tuned twiddle starting paramters should be given manually first.
The speed was limited to 16mph. I tried with larger values but 16mph seemed to be a safe value to left the car going around for hours without leaving the road.

__First measurement__
70 laps
<pre>
Starting paramteres:
P = 0.5 I =0.006 D =3
DP= 0.1 DI=0.001 DD=0.5
</pre>
[First measurements of 70 laps](https://github.com/balinth0rvath/CarND-PID-Control-Project/blob/master/measurements_1.txt)

Best paramters:
P=0.4 I=0.0061981 D=2.5

__Second measurement__
<pre>
Starting paramteres:
P = 0.5 I =0.006 D =3
DP= 0.1 DI=0.001 DD=0.5
</pre>

Best parameters:
P=0.44 I=0.00531 D=2.43297



