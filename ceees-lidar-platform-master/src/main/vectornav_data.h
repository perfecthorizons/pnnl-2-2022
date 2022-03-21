#include "id.h"
#if defined(USE_TIMESTARTUP)
   static constexpr int TIMESTARTUP = __id__();
   #define TYPE_TIMESTARTUP
#else
   #define TYPE_TIMESTARTUP
#endif
#if defined(USE_TIMEGPS)
   static constexpr int TIMEGPS = __id__();
   #define TYPE_TIMEGPS
#else
   #define TYPE_TIMEGPS
#endif
#if defined(USE_TIMESYNCIN)
   static constexpr int TIMESYNCIN = __id__();
   #define TYPE_TIMESYNCIN
#else
   #define TYPE_TIMESYNCIN
#endif
#if defined(USE_YPR)
   static constexpr int YPR = __id__();
   #define TYPE_YPR
#else
   #define TYPE_YPR
#endif
#if defined(USE_QTN)
   static constexpr int QTN = __id__();
   #define TYPE_QTN
#else
   #define TYPE_QTN
#endif
#if defined(USE_ANGULARRATE)
   static constexpr int ANGULARRATE = __id__();
   #define TYPE_ANGULARRATE
#else
   #define TYPE_ANGULARRATE
#endif
#if defined(USE_POSITION)
   static constexpr int POSITION = __id__();
   #define TYPE_POSITION
#else
   #define TYPE_POSITION
#endif
#if defined(USE_VELOCITY)
   static constexpr int VELOCITY = __id__();
   #define TYPE_VELOCITY
#else
   #define TYPE_VELOCITY
#endif
#if defined(USE_ACCEL)
   static constexpr int ACCEL = __id__();
   #define TYPE_ACCEL
#else
   #define TYPE_ACCEL
#endif
#if defined(USE_IMU)
   static constexpr int IMU = __id__();
   #define TYPE_IMU
#else
   #define TYPE_IMU
#endif
#if defined(USE_MAGPRES)
   static constexpr int MAGPRES = __id__();
   #define TYPE_MAGPRES
#else
   #define TYPE_MAGPRES
#endif
#if defined(USE_DELTATHETA)
   static constexpr int DELTATHETA = __id__();
   #define TYPE_DELTATHETA
#else
   #define TYPE_DELTATHETA
#endif
#if defined(USE_INSSTATUS)
   static constexpr int INSSTATUS = __id__();
   #define TYPE_INSSTATUS
#else
   #define TYPE_INSSTATUS
#endif
#if defined(USE_SYNCINCNT)
   static constexpr int SYNCINCNT = __id__();
   #define TYPE_SYNCINCNT
#else
   #define TYPE_SYNCINCNT
#endif
#if defined(USE_TIMEGPSPPSTIMESTARTUP)
   static constexpr int TIMEGPSPPSTIMESTARTUP = __id__();
   #define TYPE_TIMEGPSPPSTIMESTARTUP
#else
   #define TYPE_TIMEGPSPPSTIMESTARTUP
#endif
#if defined(USE_GPSTOW)
   static constexpr int GPSTOW = __id__();
   #define TYPE_GPSTOW
#else
   #define TYPE_GPSTOW
#endif
#if defined(USE_GPSWEEK)
   static constexpr int GPSWEEK = __id__();
   #define TYPE_GPSWEEK
#else
   #define TYPE_GPSWEEK
#endif
#if defined(USE_TIMESYNCIN)
   static constexpr int TIMESYNCIN = __id__();
   #define TYPE_TIMESYNCIN
#else
   #define TYPE_TIMESYNCIN
#endif
#if defined(USE_TIMEGPSPPS)
   static constexpr int TIMEGPSPPS = __id__();
   #define TYPE_TIMEGPSPPS
#else
   #define TYPE_TIMEGPSPPS
#endif
#if defined(USE_TIMEUTC)
   static constexpr int TIMEUTC = __id__();
   #define TYPE_TIMEUTC
#else
   #define TYPE_TIMEUTC
#endif
#if defined(USE_SYNCINCNT)
   static constexpr int SYNCINCNT = __id__();
   #define TYPE_SYNCINCNT
#else
   #define TYPE_SYNCINCNT
#endif
#if defined(USE_IMUSTATUS)
   static constexpr int IMUSTATUS = __id__();
   #define TYPE_IMUSTATUS
#else
   #define TYPE_IMUSTATUS
#endif
#if defined(USE_UNCOMPMAG)
   static constexpr int UNCOMPMAG = __id__();
   #define TYPE_UNCOMPMAG
#else
   #define TYPE_UNCOMPMAG
#endif
#if defined(USE_UNCOMPACCEL)
   static constexpr int UNCOMPACCEL = __id__();
   #define TYPE_UNCOMPACCEL
#else
   #define TYPE_UNCOMPACCEL
#endif
#if defined(USE_UNCOMPGYRO)
   static constexpr int UNCOMPGYRO = __id__();
   #define TYPE_UNCOMPGYRO
#else
   #define TYPE_UNCOMPGYRO
#endif
#if defined(USE_TEMP)
   static constexpr int TEMP = __id__();
   #define TYPE_TEMP
#else
   #define TYPE_TEMP
#endif
#if defined(USE_PRES)
   static constexpr int PRES = __id__();
   #define TYPE_PRES
#else
   #define TYPE_PRES
#endif
#if defined(USE_DELTATHETA)
   static constexpr int DELTATHETA = __id__();
   #define TYPE_DELTATHETA
#else
   #define TYPE_DELTATHETA
#endif
#if defined(USE_DELTAVEL)
   static constexpr int DELTAVEL = __id__();
   #define TYPE_DELTAVEL
#else
   #define TYPE_DELTAVEL
#endif
#if defined(USE_MAG)
   static constexpr int MAG = __id__();
   #define TYPE_MAG
#else
   #define TYPE_MAG
#endif
#if defined(USE_GYRO)
   static constexpr int GYRO = __id__();
   #define TYPE_GYRO
#else
   #define TYPE_GYRO
#endif
#if defined(USE_SENSSAT)
   static constexpr int SENSSAT = __id__();
   #define TYPE_SENSSAT
#else
   #define TYPE_SENSSAT
#endif
#if defined(USE_RAW)
   static constexpr int RAW = __id__();
   #define TYPE_RAW
#else
   #define TYPE_RAW
#endif
#if defined(USE_UTC)
   static constexpr int UTC = __id__();
   #define TYPE_UTC
#else
   #define TYPE_UTC
#endif
#if defined(USE_TOW)
   static constexpr int TOW = __id__();
   #define TYPE_TOW
#else
   #define TYPE_TOW
#endif
#if defined(USE_WEEK)
   static constexpr int WEEK = __id__();
   #define TYPE_WEEK
#else
   #define TYPE_WEEK
#endif
#if defined(USE_NUMSATS)
   static constexpr int NUMSATS = __id__();
   #define TYPE_NUMSATS
#else
   #define TYPE_NUMSATS
#endif
#if defined(USE_FIX)
   static constexpr int FIX = __id__();
   #define TYPE_FIX
#else
   #define TYPE_FIX
#endif
#if defined(USE_POSLLA)
   static constexpr int POSLLA = __id__();
   #define TYPE_POSLLA
#else
   #define TYPE_POSLLA
#endif
#if defined(USE_POSEXEC)
   static constexpr int POSEXEC = __id__();
   #define TYPE_POSEXEC
#else
   #define TYPE_POSEXEC
#endif
#if defined(USE_VELNED)
   static constexpr int VELNED = __id__();
   #define TYPE_VELNED
#else
   #define TYPE_VELNED
#endif
#if defined(USE_VELECEF)
   static constexpr int VELECEF = __id__();
   #define TYPE_VELECEF
#else
   #define TYPE_VELECEF
#endif
#if defined(USE_POSU)
   static constexpr int POSU = __id__();
   #define TYPE_POSU
#else
   #define TYPE_POSU
#endif
#if defined(USE_VELU)
   static constexpr int VELU = __id__();
   #define TYPE_VELU
#else
   #define TYPE_VELU
#endif
#if defined(USE_TIMEU)
   static constexpr int TIMEU = __id__();
   #define TYPE_TIMEU
#else
   #define TYPE_TIMEU
#endif
#if defined(USE_SVSTAT)
   static constexpr int SVSTAT = __id__();
   #define TYPE_SVSTAT
#else
   #define TYPE_SVSTAT
#endif
#if defined(USE_VPESTATUS)
   static constexpr int VPESTATUS = __id__();
   #define TYPE_VPESTATUS
#else
   #define TYPE_VPESTATUS
#endif
#if defined(USE_QTN)
   static constexpr int QTN = __id__();
   #define TYPE_QTN
#else
   #define TYPE_QTN
#endif
#if defined(USE_DCM)
   static constexpr int DCM = __id__();
   #define TYPE_DCM
#else
   #define TYPE_DCM
#endif
#if defined(USE_MAGNED)
   static constexpr int MAGNED = __id__();
   #define TYPE_MAGNED
#else
   #define TYPE_MAGNED
#endif
#if defined(USE_ACCELNED)
   static constexpr int ACCELNED = __id__();
   #define TYPE_ACCELNED
#else
   #define TYPE_ACCELNED
#endif
#if defined(USE_LINEARACCELBODY)
   static constexpr int LINEARACCELBODY = __id__();
   #define TYPE_LINEARACCELBODY
#else
   #define TYPE_LINEARACCELBODY
#endif
#if defined(USE_LINEARACCELNED)
   static constexpr int LINEARACCELNED = __id__();
   #define TYPE_LINEARACCELNED
#else
   #define TYPE_LINEARACCELNED
#endif
#if defined(USE_YPRU)
   static constexpr int YPRU = __id__();
   #define TYPE_YPRU
#else
   #define TYPE_YPRU
#endif
#if defined(USE_YPRRATE)
   static constexpr int YPRRATE = __id__();
   #define TYPE_YPRRATE
#else
   #define TYPE_YPRRATE
#endif
#if defined(USE_STATEAHRS)
   static constexpr int STATEAHRS = __id__();
   #define TYPE_STATEAHRS
#else
   #define TYPE_STATEAHRS
#endif
#if defined(USE_COVAHRS)
   static constexpr int COVAHRS = __id__();
   #define TYPE_COVAHRS
#else
   #define TYPE_COVAHRS
#endif
#if defined(USE_INSSTATUS)
   static constexpr int INSSTATUS = __id__();
   #define TYPE_INSSTATUS
#else
   #define TYPE_INSSTATUS
#endif
#if defined(USE_POSLLA)
   static constexpr int POSLLA = __id__();
   #define TYPE_POSLLA
#else
   #define TYPE_POSLLA
#endif
#if defined(USE_POSECEF)
   static constexpr int POSECEF = __id__();
   #define TYPE_POSECEF
#else
   #define TYPE_POSECEF
#endif
#if defined(USE_VELBODY)
   static constexpr int VELBODY = __id__();
   #define TYPE_VELBODY
#else
   #define TYPE_VELBODY
#endif
#if defined(USE_VELNED)
   static constexpr int VELNED = __id__();
   #define TYPE_VELNED
#else
   #define TYPE_VELNED
#endif
#if defined(USE_VELECEF)
   static constexpr int VELECEF = __id__();
   #define TYPE_VELECEF
#else
   #define TYPE_VELECEF
#endif
#if defined(USE_MAGECEF)
   static constexpr int MAGECEF = __id__();
   #define TYPE_MAGECEF
#else
   #define TYPE_MAGECEF
#endif
#if defined(USE_ACCELECEF)
   static constexpr int ACCELECEF = __id__();
   #define TYPE_ACCELECEF
#else
   #define TYPE_ACCELECEF
#endif
#if defined(USE_LINEARACCELECEF)
   static constexpr int LINEARACCELECEF = __id__();
   #define TYPE_LINEARACCELECEF
#else
   #define TYPE_LINEARACCELECEF
#endif
#if defined(USE_POSU)
   static constexpr int POSU = __id__();
   #define TYPE_POSU
#else
   #define TYPE_POSU
#endif
#if defined(USE_VELU)
   static constexpr int VELU = __id__();
   #define TYPE_VELU
#else
   #define TYPE_VELU
#endif
#if defined(USE_STATEINS)
   static constexpr int STATEINS = __id__();
   #define TYPE_STATEINS
#else
   #define TYPE_STATEINS
#endif
#if defined(USE_COVINS)
   static constexpr int COVINS = __id__();
   #define TYPE_COVINS
#else
   #define TYPE_COVINS
#endif
#define VN_DATA_TYPE TYPE_TIMESTARTUP TYPE_TIMEGPS TYPE_TIMESYNCIN TYPE_YPR TYPE_QTN TYPE_ANGULARRATE TYPE_POSITION TYPE_VELOCITY TYPE_ACCEL TYPE_IMU TYPE_MAGPRES TYPE_DELTATHETA TYPE_INSSTATUS TYPE_SYNCINCNT TYPE_TIMEGPSPPSTIMESTARTUP TYPE_GPSTOW TYPE_GPSWEEK TYPE_TIMESYNCIN TYPE_TIMEGPSPPS TYPE_TIMEUTC TYPE_SYNCINCNT TYPE_IMUSTATUS TYPE_UNCOMPMAG TYPE_UNCOMPACCEL TYPE_UNCOMPGYRO TYPE_TEMP TYPE_PRES TYPE_DELTATHETA TYPE_DELTAVEL TYPE_MAG TYPE_GYRO TYPE_SENSSAT TYPE_RAW TYPE_UTC TYPE_TOW TYPE_WEEK TYPE_NUMSATS TYPE_FIX TYPE_POSLLA TYPE_POSEXEC TYPE_VELNED TYPE_VELECEF TYPE_POSU TYPE_VELU TYPE_TIMEU TYPE_SVSTAT TYPE_VPESTATUS TYPE_QTN TYPE_DCM TYPE_MAGNED TYPE_ACCELNED TYPE_LINEARACCELBODY TYPE_LINEARACCELNED TYPE_YPRU TYPE_YPRRATE TYPE_STATEAHRS TYPE_COVAHRS TYPE_INSSTATUS TYPE_POSLLA TYPE_POSECEF TYPE_VELBODY TYPE_VELNED TYPE_VELECEF TYPE_MAGECEF TYPE_ACCELECEF TYPE_LINEARACCELECEF TYPE_POSU TYPE_VELU TYPE_STATEINS TYPE_COVINS
