cmd_usr/include/linux/usb/.install := /bin/bash scripts/headers_install.sh ./usr/include/linux/usb ./include/uapi/linux/usb g_printer.h video.h ch9.h g_uvc.h audio.h tmc.h functionfs.h cdc.h charger.h cdc-wdm.h gadgetfs.h ch11.h midi.h; /bin/bash scripts/headers_install.sh ./usr/include/linux/usb ./include/generated/uapi/linux/usb ; touch usr/include/linux/usb/.install