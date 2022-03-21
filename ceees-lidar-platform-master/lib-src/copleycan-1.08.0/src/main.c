/* Main entry point for the CAN card driver */

#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/usb.h>

#include "fileinfo.h"
#include "internal.h"
#include "bdm.h"
#include "cardintf.h"
#include "command.h"
#include "main.h"
#include "recv.h"
#include "send.h"
#include "usb.h"
#include "os/debug.h"

// Max number of minor devices to allocate for this driver
#define MAX_MINOR  (MAX_CARDS*MAX_PORTS)

/* local data */
static int major;
static SPINLOCK_TYPE driverSpin;
static CardInfo *cards[ MAX_MINOR ];

/* local functions */
static int     probe_pci(struct pci_dev *dev, const struct pci_device_id *id);
static void    remove_pci(struct pci_dev *dev);
static int     probe_usb( struct usb_interface *intf, const struct usb_device_id *id );
static void    disconnect_usb( struct usb_interface *intf );
static void    FreeCardInfo( CardInfo *pCard );
static int     open( struct inode *inode, struct file *file );
static int     close( struct inode *inode, struct file *file );
static ssize_t read( struct file *file, char *buffer, size_t length, loff_t *offset );
static ssize_t write( struct file *file, const char *buffer, size_t length, loff_t *offset );
static long    ioctl( struct file *file, uint cmd, ulong arg );
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21))
static irqreturn_t intHandler( int irq, void *ptr );
#else
static irqreturn_t intHandler( int irq, void *ptr, struct pt_regs *regs );
#endif

// access_ok dropped the type parameter in version 5.0
#if( LINUX_VERSION_CODE > KERNEL_VERSION(5,0,0))
static inline int check_access_ok( int write, void *addr, size_t sz ){ return access_ok( addr, sz ); }
#else
static inline int check_access_ok( int write, void *addr, size_t sz ){ return access_ok( write ? VERIFY_WRITE : VERIFY_READ, addr, sz ); }
#endif

static int FindFreeMinor( CardInfo *pCard );
static void ReleaseMinor( CardInfo *pCard );

static int RegisterModule( void );
static int UnRegisterModule( void );
static int RegisterDevice( CardInfo *pCard, int minor );
static int UnRegisterDevice( CardInfo *pCard );
static PortInfo *FindPortInfo( struct inode *inode );

/* global data */
struct file_operations fops =
{
   .owner          = THIS_MODULE,
   .read           = read,
   .write          = write,
   .unlocked_ioctl = ioctl,
   .open           = open,
   .release        = close,
};
//EXPORT_SYMBOL(fops);

static const struct pci_device_id id_tbl[] =
{
   { 0x10EE, 0xD154, 0x10EE, 0xD154 },  // PCI
   { 0x1CC5, 0x0100, 0x1CC5, 0x0100 },  // PCIe-02
   { 0x1CC5, 0x0101, 0x1CC5, 0x0101 },  // PCIe-01
   { 0x1CC5, 0x0102, 0x1CC5, 0x0102 },  // PC104-2

   { 0x1CC5, 0x0200, 0x1CC5, 0x0200 },  // obsolete PCIe-02
   { 0x1CC5, 0x0201, 0x1CC5, 0x0201 },  // obsolete PCIe-01
   { 0 },
};

static const struct usb_device_id usb_tbl[] = 
{
   { USB_DEVICE( 0x2B74, 0x0103 ) },
   { USB_DEVICE( 0xffff, 0x0103 ) },
   { 0 },
};

struct pci_driver pciDrvr =
{
   .name      =    DEVICE_NAME,
   .id_table  =    id_tbl,
   .probe     =    probe_pci,
   .remove    =    remove_pci,
};

struct usb_driver usbDrvr = 
{
   .name       =   DEVICE_NAME,
   .id_table   =   usb_tbl,
   .probe      =   probe_usb,
   .disconnect =   disconnect_usb,
};

int msgLevel = DEBUG_DBG;

/**************************************************************
 * Initialize the module.
 *************************************************************/
int init_module( void )
{
   int i, ret;

   info( "Init module called\n" );

   // This array is used to keep track of the minor device numbers
   // For a card with N ports, there will be N consecutive pointers
   // to the card structure in the array.
   for( i=0; i<MAX_MINOR; i++ )
      cards[i] = 0;

   // Init a mutex used to synchronize access to all devices managed by this driver
   SpinLockInit( &driverSpin );

   // Do any additional one time init for this module
   ret = RegisterModule();
   if( ret < 0 )
   {
      errmsg( "Error %d registering module\n", ret );
      return ret;
   }

   ret = pci_register_driver( &pciDrvr );
   if( ret < 0 ) return ret;

   ret = usb_register( &usbDrvr );
   if( ret )
   {
      errmsg( "Error %d registering usb driver\n", ret );
      return ret;
   }

   return 0;
}

/**************************************************************
 *
 *************************************************************/
void cleanup_module( void )
{
   info( "Cleanup module called\n" );
   pci_unregister_driver( &pciDrvr );
   usb_deregister( &usbDrvr );
   UnRegisterModule();
   return;
}

/**************************************************************
 *
 *************************************************************/
static int probe_pci( struct pci_dev *dev, const struct pci_device_id *id )
{
   int err;
   int minor = -1;
   CardInfo *pCard;
   ulong start, len;
   char *hwPtr;

   info( "Probe called\n" );

   if( pci_request_regions( dev, DEVICE_NAME ) )
   {
      errmsg( "Unable to reserve PCI regions required for card.  Already in use?\n" );
      return -EBUSY;
   }

   if( pci_enable_device( dev ) )
   {
      errmsg( "Unable to enable PCI device\n" );
      err = -EBUSY;
      goto release;
   }

   pCard = (CardInfo *)kzalloc( sizeof(CardInfo), GFP_KERNEL );
   if( !pCard )
   {
      errmsg( "Unable to allocate kernel memory for card data\n" );
      err = -ENOMEM;
      goto disable;
   }

   // Find how many ports this card has
   switch( id->device )
   {
      case 0xD154: 
         pCard->useSafeReads = 1;
      case 0x0100:
      case 0x0200:
      case 0x0102:
         pCard->bigEndianData = 1;
         pCard->portCt = 2; 
         break;
      default:
         pCard->bigEndianData = 1;
         pCard->portCt = 1;
         break;
   }

   // Find the minor device number
   minor = FindFreeMinor( pCard );
   if( minor < 0 )
   {
      errmsg( "Unable to allocate a minor device number.  Driver only compiled to support %d cards\n", MAX_CARDS );
      err = -ENOMEM;
      goto dealloc;
   }

   pci_set_drvdata( dev, pCard );
   pCard->os.dev = dev;

   start = pci_resource_start( dev, 0 );
   len = pci_resource_len( dev, 0 );

   hwPtr = (char*)ioremap( start, len );
   if( !hwPtr )
   {
      errmsg( "Unable to remap PCI address 0x%08lx\n", start );
      err = -ENOMEM;
      goto dealloc;
   }

   CardInit( pCard, hwPtr, len );

   err = RegisterDevice( pCard, minor );
   if( err )
   {
      errmsg( "Unable to register device\n" );
      err = -EBUSY;
      goto unmap;
   }

#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21))
   err = request_irq( dev->irq, intHandler, IRQF_SHARED, "copleycan", pCard );
#else
   err = request_irq( dev->irq, intHandler, SA_SHIRQ, "copleycan", pCard );
#endif
   if( err ) goto freeirq;

   CardEnable( pCard );
   return 0;

freeirq:
   free_irq( dev->irq, pCard );

unmap:
   iounmap( hwPtr );

dealloc:
   ReleaseMinor( pCard );
   kfree( pCard );

disable:
   pci_disable_device( dev );

release:
   pci_release_regions( dev );
   return err;
}

// Same as above, but for USB based cards
static int probe_usb( struct usb_interface *intf, const struct usb_device_id *id )
{
   int err = 0;
   int minor = -1;
   CardInfo *pCard;

   info( "Probe USB called\n" );

   pCard = (CardInfo *)kzalloc( sizeof(CardInfo), GFP_KERNEL );
   if( !pCard )
   {
      errmsg( "Unable to allocate kernel memory for card data\n" );
      err = -ENOMEM;
      goto disable;
   }

   // Find how many ports this card has
   switch( id->idProduct )
   {
      case 0x0104:
         pCard->portCt = 2; 
         break;

      case 0x0103:
      default:
         pCard->portCt = 1;
         break;
   }

   // Find the minor device number
   minor = FindFreeMinor( pCard );
   if( minor < 0 )
   {
      errmsg( "Unable to allocate a minor device number.  Driver only compiled to support %d cards\n", MAX_CARDS );
      err = -ENOMEM;
      goto dealloc;
   }

   struct usb_device *dev = interface_to_usbdev( intf );
   err = usb_set_interface( dev, 0, 1 );
   if( err )
      warn( "usb_set_interface failed with error %d\n", err );

   usb_set_intfdata( intf, pCard );
   pCard->os.usbintf = intf;

   CardInit( pCard, 0, 0 );

   err = RegisterDevice( pCard, minor );
   if( err )
   {
      errmsg( "Unable to register device\n" );
      err = -EBUSY;
      goto dealloc;
   }

   err = USB_CardEnable( pCard );
   if( err )
   {
      errmsg( "Unable to enable card\n" );
      goto unreg;
   }

   return 0;

unreg:
   UnRegisterDevice( pCard );

dealloc:
   ReleaseMinor( pCard );
   kfree( pCard );

disable:
   return err;
}

/**************************************************************
 *
 *************************************************************/
static void remove_pci(struct pci_dev *dev)
{
   CardInfo *pCard = pci_get_drvdata( dev );

   CardShutDown( pCard );

   ReleaseMinor( pCard );

   free_irq( dev->irq, pCard );

   pci_disable_device( pCard->os.dev );

   pci_release_regions( pCard->os.dev );

   UnRegisterDevice( pCard );

   iounmap( pCard->hwPtr );

   pCard->cardRemoved = 1;
   FreeCardInfo( pCard );
}

static void disconnect_usb( struct usb_interface *intf )
{
   CardInfo *pCard = usb_get_intfdata( intf );
   info( "disconnect_usb called\n" );

   ReleaseMinor( pCard );

   UnRegisterDevice( pCard );

   pCard->cardRemoved = 1;
   FreeCardInfo( pCard );
}

// Free the card info memory if it's no longer needed.
static void FreeCardInfo( CardInfo *pCard )
{
   // Make sure the card has been removed / disconnected
   if( !pCard->cardRemoved )
      return;

   // Make sure there are no open files
   for( int i=0; i<pCard->portCt; i++ )
   {
      if( CountFiles( &pCard->port[i] ) )
         return;
   }

   // OK, safe to free the card structure
   kfree( pCard );
}

/**************************************************************
 *
 *************************************************************/
static int open( struct inode *inode, struct file *file )
{
   FileInfo *pFile;
   PortInfo *pPort;

//   info( "open called\n" );

   // Find the port associated with the device being opened.
   pPort = FindPortInfo( inode );
   if( !pPort )
   {
      errmsg( "Unable to locate port associated with file node!\n" );
      return -ENOMEM;
   }

   // Create a file object and assign it to this port
   pFile = CreateFile( pPort );
   if( !pFile )
   {
      errmsg( "Unable to allocate kernel memory for file data\n" );
      return -ENOMEM;
   }

   file->private_data = pFile;

   return 0;
}

/**************************************************************
 * 
 *************************************************************/
static int close( struct inode *inode, struct file *file )
{
   FileInfo *pFile = (FileInfo *)file->private_data;

   // If this file had the CAN port open, then reduce the
   // open count for the port.  The port will actually be
   // closed when this hits zero.
   if( pFile->open )
   {
      uint32_t rCt, cmd[EICAN_CMD_LEN];
      cmd[0] = (EICAN_CMD_CLOSEPORT<<16);
      rCt = 0;
      SendCmd( pFile, cmd, 1, &rCt );
   }

   CardInfo *pCard = pFile->pPort->pCard;
   FreeFile( pFile );

   // Free the card info structure if it's no longer needed
   FreeCardInfo( pCard );

   return 0;
}

/**************************************************************
 *
 *************************************************************/
static ssize_t read( struct file *file, char *buffer, size_t length, loff_t *offset )
{
   return -EINVAL;
}

/**************************************************************
 *
 *************************************************************/
static ssize_t write( struct file *file, const char *buffer, size_t length, loff_t *offset )
{
   return -EINVAL;
}

/**************************************************************
 *
 *************************************************************/
static long ioctl( struct file *file, uint cmd, ulong arg )
{
   FileInfo *pFile = (FileInfo *)file->private_data;
   int size, err = 0;

   union
   {
      CANCARD_MSG can;
      EICAN_BDM bdm;
      uint32_t cmd[64];
   } ioctlData;

   // Do a quick sanity check on the command ID
   if( (_IOC_TYPE(cmd) != EICAN_IOCTL_TYPE) )
   {
      errmsg( "Bad ioctl command code 0x%08x\n", cmd );
      return -ENOTTY;
   }

   // Verify the argument address and copy data on write
   size = _IOC_SIZE(cmd);
   if( size )
   {
      int ok;
      if( _IOC_DIR(cmd) & _IOC_READ )
	 ok = check_access_ok( 1, (void*)arg, size );
      else
	 ok = check_access_ok( 0, (void*)arg, size );

      if( ok && (_IOC_DIR(cmd) & _IOC_WRITE) )
	 ok = !__copy_from_user( &ioctlData, (void*)arg, size );

      if( !ok )
      {
	 errmsg( "Bad address passed to ioctl 0x%08x: 0x%08lx\n", cmd, arg );
	 return -EFAULT;
      }
   }

   // Now, handle the command
   switch( cmd )
   {
      case EICAN_IOCTL_SENDCAN:
      case EICAN_IOCTL_SENDFD:
	 err = SendCAN( pFile->pPort, &ioctlData.can );
	 break;

      case EICAN_IOCTL_RECVCAN:
      case EICAN_IOCTL_RECVFD:
	 err = RecvCAN( pFile, &ioctlData.can );
	 break;

      case EICAN_IOCTL_CMD:
      {
	 // For commands I will need to find the number of words
	 // of data passed and validate it.  Only the first word
	 // has been validated at this point.
	 int inCt, outCt, ok;

	 inCt = 1 + (ioctlData.cmd[0] & 0x3F);
	 outCt = 64;
	 size = 4*inCt;
	 ok = check_access_ok( 1, (void*)arg, size );

	 if( ok ) ok = !__copy_from_user( &ioctlData, (void*)arg, size );
	 if( !ok )
	 {
	    errmsg( "Bad address passed to ioctl 0x%08x: 0x%08lx\n", cmd, arg );
	    return -EFAULT;
	 }

	 err = SendCmd( pFile, ioctlData.cmd, inCt, &outCt );
	 size = 4+4*outCt;
	 break;
      }

      case EICAN_IOCTL_BDM:
	 err = HandleBDM( pFile->pPort->pCard, &ioctlData.bdm );
	 break;

      case EICAN_IOCTL_PCITEST:
	 err = TestCard( pFile->pPort->pCard );
	 break;

      default:
	 warn( "Unknown IOCTL command code: 0x%08x\n", cmd );
         warn( "Known IOCTL codes:\n" );
         warn( "  SENDCAN: 0x%08lx\n", (long)EICAN_IOCTL_SENDCAN );
         warn( "  RECVCAN: 0x%08lx\n", (long)EICAN_IOCTL_RECVCAN );
         warn( "      CMD: 0x%08lx\n", (long)EICAN_IOCTL_CMD     );
         warn( "  PCITEST: 0x%08lx\n", (long)EICAN_IOCTL_PCITEST );
         break;
   }

   // Just return if an error was returned
   if( err )
      return err;

   // If this was a read, copy the returned data from my stack to
   // the passed pointer.
   if( size && (_IOC_DIR(cmd) & _IOC_READ) )
   {
      if( __copy_to_user( (void*)arg, &ioctlData, size ) )
      {
	 errmsg( "Unable to copy returned data to passed address 0x%08x: 0x%08lx\n", cmd, arg );
	 return -EFAULT;
      }
   }

   return 0;
}

/**************************************************************
 *
 *************************************************************/
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21))
static irqreturn_t intHandler( int irq, void *ptr )
#else
static irqreturn_t intHandler( int irq, void *ptr, struct pt_regs *regs )
#endif
{
   CardInfo *pCard = (CardInfo*)ptr;

   if( !EICAN_ISR_Top( pCard ) )
      return IRQ_NONE;

   EICAN_ISR_Bottom( pCard );
   return IRQ_HANDLED;
}

/**
 * Find a free device minor number.
 */
static int FindFreeMinor( CardInfo *pCard )
{
   SPINLOCK_STATE state;
   int i, j, pct;
   int minor = -1;

   // Find the minor device number
   SpinLockGet( &driverSpin, &state );

   // Find N consecutive free minor numbers for a card with
   // N ports.
   pct = pCard->portCt;
   for( i=0; i<=(MAX_MINOR-pct); i++ )
   {
      int ok = 1;

      for( j=0; (j<pct) && ok; j++ ) 
         if( cards[i+j] ) ok = 0;

      if( ok )
      {
         for( j=0; j<pct; j++ )
            cards[i+j] = pCard;
         minor = i;
         break;
      }
   }

   SpinLockPut( &driverSpin, &state );

   return minor;
}

static void ReleaseMinor( CardInfo *pCard )
{
   int i, pct;
   SPINLOCK_STATE state;

   pct = pCard->portCt;

   SpinLockGet( &driverSpin, &state );

   for( i=0; i<MAX_MINOR; i++ )
   {
      if( cards[i] == pCard )
         cards[i] = 0;
   }

   SpinLockPut( &driverSpin, &state );
}

/**
 * The following code is different for 2.4.x and 2.6.x versions of the Linux 
 * kernel.
 */

#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))
static struct class *classPtr;

/**
 * The class_create function was added in 2.6.13.  Before that I need to use
 * the 'class_simple' interface.
 */
#if( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13))
struct class *class_create(struct module *owner, char *name)
{
   struct class_simple *cs = class_simple_create( owner, name );
   return (struct class *)cs;
}

void class_destroy( struct class *cls )
{
   class_simple_destroy( (struct class_simple *)cls );
}
#endif

/**
 * Creating and destroying devices keeps changing with different kernel versions!
 */
static void *CreateDevice( dev_t num, struct device *device, int minor ) 
{
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26))
   return device_create( classPtr, 0, num, device, DEVICE_NAME"%02d", minor );
#elif( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14))
   return class_device_create( classPtr, 0, num, device, DEVICE_NAME"%02d", minor );
#elif( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,12))
   return class_device_create( classPtr, num, device, DEVICE_NAME"%02d", minor );
#else
   return class_simple_device_add( (struct class_simple *)classPtr, num, device, DEVICE_NAME"%02d", minor );
#endif
}

static void DestroyDevice( dev_t num )
{
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,26))
   device_destroy( classPtr, num );
#elif( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,12))
   class_device_destroy( classPtr, num );
#else
   class_simple_device_remove( num );
#endif
}

/**
 * Allocate a major number for this device and do anything else
 * that needs to be done during module init.
 */
static int RegisterModule( void )
{
   // Allocate a major device number for use with this driver
   dev_t dev;
   int ret = alloc_chrdev_region( &dev, 0, MAX_MINOR, DEVICE_NAME );
   if( ret < 0 )
   {
      errmsg( "Unable to allocate a major device number for driver.\n" );
      return ret;
   }

   major = MAJOR(dev);

   // Create a simple class to use to register these devices
   classPtr = class_create( THIS_MODULE, DEVICE_CLASS );
   if( IS_ERR( classPtr ) )
   {
      errmsg( "Unable to register simple driver class %s\n", DEVICE_CLASS );
      return (int)PTR_ERR( classPtr );
   }
   return 0;
}

/**
 * Undo anything we did in RegisterModule
 */
static int UnRegisterModule( void )
{
   class_destroy( classPtr );
   unregister_chrdev_region( MKDEV(major, 0), MAX_MINOR );
   return 0;
}

// Find the port associated with the file inode.
static PortInfo *FindPortInfo( struct inode *inode )
{
   return (PortInfo *)container_of( inode->i_cdev, PortInfo, os.cdev );
}

// Register the new device.
static int RegisterDevice( CardInfo *pCard, int minor )
{
   int i, ret;

   // Create a device number for each port
   for( i=0; i<pCard->portCt; i++, minor++ )
   {
      PortInfo *pPort = &pCard->port[i];

      pPort->os.num = MKDEV( major, minor );
      cdev_init( &pPort->os.cdev, &fops );
      pPort->os.cdev.owner = THIS_MODULE;

      ret = cdev_add( &pPort->os.cdev, pPort->os.num, 1 );
      if( ret )
      {
	 errmsg( "Error %d adding cdev\n", ret );
	 kobject_put( &pPort->os.cdev.kobj );
	 return ret;
      }

      debug( "Allocated char dev 0x%08x\n", pPort->os.num );
      CreateDevice( pPort->os.num, &pCard->os.dev->dev, minor );
   }

   return 0;
}

/**************************************************************
 * 
 *************************************************************/
static int UnRegisterDevice( CardInfo *pCard )
{
   int i;
   for( i=0; i<pCard->portCt; i++ )
   {
      DestroyDevice( pCard->port[i].os.num );
      cdev_del( &pCard->port[i].os.cdev );
   }
   return 0;
}

#else

// 2.4.x kernel support

/**
 * Allocate a major number for this device and do anything else
 * that needs to be done during module init.
 */
static int RegisterModule( void )
{
   int ret;

   ret = register_chrdev( 0, DEVICE_NAME, &fops );
   if( ret < 0 )
   {
      errmsg( "Error %d registering device\n", ret );
      return ret;
   }

   info( "Device major number %d was dynamically assigned\n", ret );
   major = ret;

   return 0;
}

/**
 * Undo anything we did in RegisterModule
 */
static int UnRegisterModule( void )
{
   int ret;

   ret = unregister_chrdev( major, DEVICE_NAME );
   if( ret < 0 )
      errmsg( "Error %d unregistering device %d\n", ret, major );
   return ret;
}

// Find the port associated with the file inode.
static PortInfo *FindPortInfo( struct inode *inode )
{
   int i, p;
   PortInfo *pPort = 0;
   SPINLOCK_STATE state;

   SpinLockGet( &driverSpin, &state );
   for( i=0; (i<MAX_MINOR) && !pPort; i++ )
   {
      if( !cards[i] ) continue;

      for( p=0; (p<pCard->portCt) && !pPort; p++ )
      {
         if( cards[i]->port[p].os.num == inode->i_rdev )
            pPort = &cards[i]->port[p];
      }
   }

   SpinLockPut( &driverSpin, &state );
   return pPort;
}

static int RegisterDevice( CardInfo *pCard, int minor )
{
   int i, ret;
   void *vptr;

   // Create a device number for each port
   for( i=0; i<pCard->portCt; i++, minor++ )
   {
      PortInfo *pPort = &pCard->port[i];

      pPort->os.num = MKDEV( major, minor );
      debug( "Allocated char dev 0x%08x\n", pPort->os.num );

      // TODO: I need to create a device node for this if devfs is available
   }

   return 0;
}

static int UnRegisterDevice( CardInfo *pCard )
{
   return 0;
}

#endif


MODULE_LICENSE("GPL");

