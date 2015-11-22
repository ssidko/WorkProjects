#ifndef _MESSAGE_TYPES_H
#define _MESSAGE_TYPES_H

// Message type
typedef enum {
  kUnknownMessage,
  kIdentification,
  kNotification,
  kCommand  
} MessageType;

typedef enum {
  kMasterID = 111,
  kSlaveID  = 222, 
} IdentificationType;

typedef enum {
  kUnknownNotification,
  kGuardModeEnabled,
  kGuardModeDisabled, 
  kLine1Activated,
  kLine2Activated,
  kLine3Activated,
  kLine4Activated,
  kLine5Activated,
  kAllLinesActivated,
  kGsmEraseAllActivated,
  kKeyEraseAllActivated,
  kMasterGuardSensor1Activated,
  kSlaveGuardSensor1Activated,
  kLinkToSlaveLosted,
  kLinkToMasterLosted,
} NotificationType;

typedef enum {
  kUnknownCommand,
  kEnableGuardMode,
  kDisableGuardMode,
  kActivateLine1,
  kActivateLine2,
  kActivateLine3,
  kActivateLine4,
  kActivateLine5,
  kActivateAllLines,
} CommandType;

#endif // _MESSAGE_TYPES_H
