#include "BigginPCH.h"

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Biggin.h"

int main(int, char*[]) {
	biggin::Biggin engine;
	engine.Run();
    return 0;
}