% Quentin Torgerson
% mechatronics midterm motor calculation sheet
% April 11, 2016

clear
clc
clear ALL
close ALL
close ALL HIDDEN

% motor parameters from spec sheet
stall_torque = 0.02; % kg-cm before gearbox
free_run_speed = 30000; % RPM before gearbox
nominal_voltage = 6; % Volts
gear_ratio = 29.86; % 29.86:1 gear ratio
number_of_motors = 2; % number of motors applying force to the ground

% robot parameters
mass = 0.275; % kg
mass_fraction_on_wheels = 1; % for 4 wheeled robot it is 1, for 2 wheel, will depend on center of gravity
coefficient_of_friction = 0.3; % specified in problem statement
wheel_diameter = 3.5; % cm

% track parameters
incline_angle = 15; % degrees
track_length = 2; % meters

% unit conversion
stall_torque = stall_torque * gear_ratio * 9.81 / 100; % convert stall torque to N-m after the motor
wheel_diameter = wheel_diameter / 100; % convert wheel diameter to m
stall_force = stall_torque * number_of_motors / (wheel_diameter / 2); % force applied to the ground through both wheels
free_run_velocity = free_run_speed / gear_ratio / 60 * pi * wheel_diameter; % m/s
incline_angle = incline_angle * pi / 180; % convert to radians

% friction force limit
max_friction_force = mass * 9.81 * coefficient_of_friction * cos(incline_angle); % Newtons - force provided by friction
friction_limited_accel = max_friction_force / mass; % m/s/s - acceleration when limited by friciton

if (max_friction_force < stall_force)
    friction_limited_top_velocity = (max_friction_force - stall_force) / (stall_force / -free_run_velocity); % m/s - motors will accelerate to this velocity while using thier maxium friciton
    friction_limited_acceleration_distance = (friction_limited_top_velocity^2) / (2 * friction_limited_accel); % m - distance spent accelerating at the friciton limit
    friction_limited_acceleration_time = friction_limited_top_velocity / (friction_limited_accel); % s - time spent accelerating at the limit of friction
elseif (max_friction_force >= stall_force)
    friction_limited_top_velocity = 0;
    friction_limited_acceleration_distance = 0;
    friction_limited_acceleration_time = 0;
end
syms t;
syms x(t); % make symbolic equation for distance versus time
assume(t > 0);

Dx = diff(x,t); % define Dx as derivative of x versus time (velocity)
D2x = diff(x,t,2); % define D2x as second derivative of x versus time (acceleration)

% solve differential equation for x(t) based on the limit of the force vs.
% velocity curve
x(t) = dsolve(Dx * stall_force / (-free_run_velocity) + stall_force == mass * D2x, x(friction_limited_acceleration_time) ...
    == friction_limited_acceleration_distance, Dx(friction_limited_acceleration_time) == friction_limited_top_velocity);

% create equation for friciton limited acceleration (porabola)
syms x_const_accel(t)
x_const_accel(t) = 0.5 * friction_limited_accel * t^2; 


% decide if the motor is ever friction limited
if (friction_limited_acceleration_distance < track_length)
    total_time = friction_limited_acceleration_time;
    total_distance = 0;

    % solve for time to travel the track length numerically b/c can't solve
    % algebraically.  This takes time because of complex equation
    while (total_distance < track_length)
        total_time = total_time + 0.01;
        total_distance = x(total_time);
    end
    
    disp('this many meters travelled ');
    disp(double(total_distance)); % shows how close to 2 meters the numerical calculation was
    disp('in this many seconds ');
    disp(total_time); % is the total time it takes to reach the end of the ramp
    
    motor_limited_plot = ezplot(x(t)); % plot motor limited distance vs. time in blue
    hold
    if (friction_limited_acceleration_distance ~= 0)
        friciton_limited_plot = ezplot(x_const_accel(t)); % plot friction limited distance vs. time in red
        disp(' ');
        disp('friciton limited acceleration until t= ');
        disp(friction_limited_acceleration_time);
        disp('then motor limited until the end of the ramp');
    else
        disp('motor limited for entire length of ramp');
    end
elseif (friction_limited_acceleration_distance >= track_length)
    disp('this many meters travelled ');
    disp(track_length);
    disp('in this many seconds ');
    total_time = double(solve(x_const_accel(t) == track_length, t));
    disp(total_time);
    friciton_limited_plot = ezplot(x_const_accel(t)); % plot friction limited distance vs. time in red
    disp('friction limited for entire length of ramp');
end
    
%friction_limited_acceleration_distance

axis([0 (total_time + .2) 0 2.2]);

% note that the plots are continuous and differentiable at
% t=friciton_limited_acceleration_time
% 
% It is at this time that the robot will transition from being limited by
% friction to being limited by motor torque.


















