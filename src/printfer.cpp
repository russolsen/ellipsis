#include "utils.h"
#include "printfer.h"
#include "value.h"

void CPrintFer::PrintF(const char* format, ...) {
    char ch, buffer[32];
    const char *str;
    const char Percent = '%';
    CCell *v;

    va_list args;
    va_start(args, format);

    for (const char* p = format; *p != '\0'; ++p) {
        if (*p != Percent) {
            m_Device->Write(p, 1);
            continue;
        }

        ++p;
        if (*p == '\0') break;

        switch (*p) {
            case Percent:
                m_Device->Write(p, 1);
                break;
            case 'v':
		v = va_arg(args, CCell*);
		v->Print(this);
                break;
             case 'd':
                IntToString(va_arg(args, int), buffer);
                m_Device->Write(buffer, strlen(buffer));
                break;
            case 'c':
		ch = (char)va_arg(args, int);
                m_Device->Write(&ch, 1);
                break;
            case 's': 
                str = va_arg(args, const char*);
		m_Device->Write(str, strlen(str));
                break;
            default:
		m_Device->Write(&Percent, 1);
		m_Device->Write(p, 1);
                break;
        }
    }

    va_end(args);
}

