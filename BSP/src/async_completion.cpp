#include "framework/ports.hpp"
namespace fw::bsp{namespace{AsyncCompletionHook hook=nullptr;void*context=nullptr;}void setAsyncCompletionHook(AsyncCompletionHook h,void*c)noexcept{hook=h;context=c;}void notifyAsyncCompletionFromISR()noexcept{if(hook)hook(context);}}
