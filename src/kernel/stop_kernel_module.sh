# Kill user space driver program first if it happens to be running, because otherwise shutting down the kernel
# module would crash the system if the userland program was still accessing it.
echo Killing existing instances of user space driver program fbcpX
sudo pkill fbcpx
sudo pkill fbcpx-stable

# Now safe to tear down the module
echo Stopping kernel module bcm2835_spi_display.ko
sudo rmmod bcm2835_spi_display.ko

