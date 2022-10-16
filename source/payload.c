#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <orbis/libkernel.h>
#include <orbis/libSceLibcInternal.h>

int64_t SendNotification(const char* fmt, ...);

typedef int dlsym_t(int, const char*, void*);

struct payload_args
{
    dlsym_t* dlsym;
    int *rwpipe;
    int *payloadout;
};

typeof(sceKernelSendNotificationRequest) * f_sceKernelSendNotificationRequest;

//libc
typeof(vsprintf) * f_vsprintf;
typeof(strcpy) * f_strcpy;

int payload_main(struct payload_args *args) 
{
  dlsym_t* dlsym = args->dlsym;
  
  dlsym(0x2001, "sceKernelSendNotificationRequest", &f_sceKernelSendNotificationRequest);

//  int libC = f_sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, 0, 0, 0, 0);
  dlsym(0x2, "vsprintf", &f_vsprintf);
  dlsym(0x2, "strcpy", &f_strcpy);
  
  SendNotification("Hello World!");

  return 0;
}

int64_t SendNotification(const char* fmt, ...)
{
    SceNotificationRequest buf;

    va_list args;
    va_start(args, fmt);
    f_vsprintf(buf.message, fmt, args);
    va_end(args);

    buf.type = 0;
    buf.unk3 = 0;
    buf.use_icon_image_uri = 1;
    buf.target_id = -1;
    f_strcpy(buf.uri, "cxml://psnotification/tex_icon_system");

    return f_sceKernelSendNotificationRequest(0, (SceNotificationRequest * ) & buf, sizeof(buf), 0);
}