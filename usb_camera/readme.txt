1 . compile linux driver for the camera
make
2. Load the kernel module by running the following command in the terminal:

sudo insmod timer_interrupt.ko
3. Verify that the kernel module loaded correctly by checking the system log:

dmesg | tail

This command will show the last few lines of the system log, which should include a message indicating that the timer interrupt handler was registered.

4. download and  install opencv library https://opencv.org/
5. compile test_cam.cpp with below command in terminal
g++ -o camera_display camera_display.cpp `pkg-config --cflags --libs opencv`

5. run test_cam file in terminal and you should see your face or something
./test_cam