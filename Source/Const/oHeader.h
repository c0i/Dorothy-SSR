/* Copyright (c) 2016 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#ifndef __DOROTHY_CONST_OHEADER_H__
#define __DOROTHY_CONST_OHEADER_H__

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <functional>
using std::function;
#include <unordered_map>
using std::unordered_map;
#include <stack>
using std::stack;
#include <unordered_set>
using std::unordered_set;
#include <memory>
#include <sstream>
using std::ostringstream;
#include <tuple>
using std::tuple;
#include "Other/AcfDelegate.h"
using Acf::Delegate;
#include "SDL_syswm.h"
#include "SDL.h"
#include "bgfx/platform.h"
#include "bgfx/bgfx.h"
#include "bx/thread.h"
#include "silly/LifeCycledSingleton.h"
#include "silly/Slice.h"
using silly::Slice;
#include "Common/oHelper.h"
#include "Const/oDefine.h"
#include "Lua/oLuaHelper.h"
#include "Basic/oObject.h"
#include "Common/oRef.h"
#include "Common/oRefVector.h"
#include "Common/oOwn.h"
#include "Common/oOwnVector.h"
#include "Common/oWRef.h"
#include "Common/oWRefVector.h"
#include "Common/oDebug.h"
#include "Basic/oAutoreleasePool.h"
#include "Basic/oContent.h"

#endif // __DOROTHY_CONST_OHEADER_H__