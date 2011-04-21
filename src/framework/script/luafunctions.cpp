/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <prerequisites.h>
#include <script/luascript.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>

void LuaScript::registerFunctions()
{
    registerGlobalFunction("exitGame", &LuaScript::lua_exitGame);
    registerGlobalFunction("loadUI", &LuaScript::lua_loadUI);
    registerGlobalFunction("getUIRootContainer", &LuaScript::lua_getUIRootContainer);

    registerClass("UILayout");
    registerClass("UIElement",   "UILayout");
    registerClass("UIButton",    "UIElement");
    registerClass("UITextEdit",  "UIElement");
    registerClass("UICheckBox",  "UIElement");
    registerClass("UIContainer", "UIElement");
    registerClass("UIWindow",    "UIContainer");

    registerMemberFunction("UIElement", "destroy", &LuaScript::lua_UIElement_destroy);
    registerMemberFunction("UIContainer", "getChildByID", &LuaScript::lua_UIContainer_getChildByID);
    registerMemberFunction("UIButton", "setOnClick", &LuaScript::lua_UIButton_setOnClick);
}

int LuaScript::lua_exitGame()
{
    g_engine.stop();
    return 1;
}

int LuaScript::lua_loadUI()
{
    UIContainerPtr parent;
    if(getStackSize() > 1) {
        parent = boost::static_pointer_cast<UIContainer>(popClassInstance());
    } else {
        parent = UIContainer::getRootContainer();
    }
    std::string uiFile = popString();
    UIElementPtr element = UILoader::loadFile(uiFile.c_str(), parent);
    pushClassInstance(element);
    return 1;
}

int LuaScript::lua_getUIRootContainer()
{
    UIContainerPtr rootContainer = UIContainer::getRootContainer();
    pushClassInstance(rootContainer);
    return 1;
}

int LuaScript::lua_UIElement_destroy()
{
    UIElementPtr element = boost::static_pointer_cast<UIElement>(popClassInstance());
    element->destroy();
    return 1;
}

int LuaScript::lua_UIButton_setOnClick()
{
    lua_insert(L, -2);
    UIButtonPtr button = boost::static_pointer_cast<UIButton>(popClassInstance());
    if(button) {
        int funcRef = popFunction();
        button->setOnClick([this, funcRef] {
            pushFunction(funcRef);
            callFunction();
        });
    } else {
        pop();
    }
    return 1;
}

int LuaScript::lua_UIContainer_getChildByID()
{
    std::string id = popString();
    ScriptablePtr object = popClassInstance();
    if(object && strcmp("UIContainer", object->getScriptableName()) == 0) {
        UIContainerPtr container = boost::static_pointer_cast<UIContainer>(object);
        pushClassInstance(container->getChildById(id));
    } else {
        pushNil();
    }

    return 1;
}