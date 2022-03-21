This directory contains the Linux driver for the Copley Controls
CAN (Control Area Network) interface card.

The driver supports version 2.6.0 and later of the Linux kernel

To build the driver, simply enter the command 'make' from within the 
directory holding the make file.  This will cause the driver to be 
built as a kernel module.

To install the module, enter the command:
   sudo insmod copleycan.ko

To add the kernel module to the kernel modules directory, enter the command
   make install

The driver will automatically create node files for any CAN channel that it finds.  
Single channel CAN cards will create a single node file, dual channel cards will 
create two node files.  These files will be created in the /dev directory, and will 
be named:

   /dev/copleycan00
   /dev/copleycan01

If more then one card is installed in the system, then additional node
files will be created.

If you find that these node files are created successfully, but you still have
trouble accessing the driver, check the permissions on the node files.  You will
need read/write permission to use the card.  To change the node file permissions
you should execute the following command as root:

   chmod 666 /dev/copleycan*

To ensure that the driver module is loaded each time you restart your system you will
need to add it to the list of modules to load at startup.  This list resides in the 
system file /etc/modules.

To add the copleycan driver to this list, edit that file as root and add a line to the
end with the name of the driver module (copleycan).

The Linux udev system can be used to automatically set the owner and permissions of
the node files on creation.  To use this, create a file named 10-cancard.rules in the
/etc/udev/rules.d directory containing the following line:

SUBSYSTEM=="cancard", OWNER="someuser", GROUP="somegroup", MODE="0666"

Change the someuser and somegroup strings to valid user and group names in the system.
Please see the Linux udev documentation for more information on writing udev rules.

