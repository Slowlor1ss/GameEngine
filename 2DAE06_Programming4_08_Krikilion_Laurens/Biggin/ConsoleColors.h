#pragma once

//https://wiki.bash-hackers.org/scripting/terminalcodes

#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#ifdef UNICODE

#define RST    std::wstring{L"\x1B[0m"}
#define KRED   std::wstring{L"\x1B[31m"}
#define KGRN   std::wstring{L"\x1B[32m"}
#define KYEL   std::wstring{L"\x1B[33m"}
#define KBLU   std::wstring{L"\x1B[34m"}
#define KMAG   std::wstring{L"\x1B[35m"}
#define KCYN   std::wstring{L"\x1B[36m"}
#define KWHT   std::wstring{L"\x1B[37m"}

#define BOLD(x)  std::wstring{L"\x1B[1m"} + x + RST
#define UNDL(x)  std::wstring{L"\x1B[4m"} + x + RST

#else

#define RST    std::string{"\x1B[0m"} //reset
#define KRED   std::string{"\x1B[31m"}
#define KGRN   std::string{"\x1B[32m"}
#define KYEL   std::string{"\x1B[33m"}
#define KBLU   std::string{"\x1B[34m"}
#define KMAG   std::string{"\x1B[35m"}
#define KCYN   std::string{"\x1B[36m"}
#define KWHT   std::string{"\x1B[37m"}
#define KLGRY   std::string{"\x1b[97m"}


#define BOLD(x)  std::string{"\x1B[1m"} + x + RST
#define UNDL(x)  std::string{"\x1B[4m"} + x + RST

#endif  // UNICODE

#ifdef _DEBUG

#define FRED(x) KRED + x + RST
#define FGRN(x) KGRN + x + RST
#define FYEL(x) KYEL + x + RST
#define FBLU(x) KBLU + x + RST
#define FMAG(x) KMAG + x + RST
#define FCYN(x) KCYN + x + RST
#define FWHT(x) KWHT + x + RST
#define FLGRY(x) KLGRY + x + RST

#else
//colors dont work in release
#define FRED(x) x
#define FGRN(x) x
#define FYEL(x) x
#define FBLU(x) x
#define FMAG(x) x
#define FCYN(x) x
#define FWHT(x) x
#define FLGRY(x) x

#endif  // _DEBUG

#endif  // _COLORS_
