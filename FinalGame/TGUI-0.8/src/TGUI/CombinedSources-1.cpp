/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//                         Koushtav Chakrabarty (koushtav@fleptic.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Clipboard.hpp>

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
    #include <SFML/Window/Clipboard.hpp>
#else
    #ifdef SFML_SYSTEM_WINDOWS
        #define NOMB
        #define NOMINMAX
        #define VC_EXTRALEAN
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    sf::String Clipboard::m_contents;
    sf::WindowHandle Clipboard::m_windowHandle = sf::WindowHandle();
    bool Clipboard::m_isWindowHandleSet = false;
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Clipboard::get()
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        return sf::Clipboard::getString();
#else
    #ifdef SFML_SYSTEM_WINDOWS
        if (m_isWindowHandleSet)
        {
            if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(m_windowHandle))
            {
                HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
                if (hGlobal != NULL)
                {
                    const char* lpszData = static_cast<const char*>(GlobalLock(hGlobal));
                    if (lpszData != NULL)
                    {
                        m_contents = lpszData;

                        GlobalUnlock(hGlobal);
                    }
                }

                CloseClipboard();
            }
        }
    #endif

        return m_contents;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Clipboard::set(const sf::String& contents)
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        sf::Clipboard::setString(contents);
#else
        m_contents = contents;

    #ifdef SFML_SYSTEM_WINDOWS
        if (m_isWindowHandleSet)
        {
            if (OpenClipboard(m_windowHandle))
            {
                EmptyClipboard();

                HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, m_contents.getSize() + 1);
                if (hGlobal != NULL)
                {
                    char* pchData = static_cast<char*>(GlobalLock(hGlobal));
                    if (pchData != NULL)
                    {
                        memcpy(pchData, m_contents.toAnsiString().c_str(), m_contents.getSize() + 1);
                        SetClipboardData(CF_TEXT, hGlobal);

                        GlobalUnlock(hGlobal);
                    }

                    GlobalFree(hGlobal);
                }

                CloseClipboard();
            }
        }
    #endif
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    void Clipboard::setWindowHandle(const sf::WindowHandle& windowHandle)
    {
        m_windowHandle = windowHandle;
        m_isWindowHandleSet = true;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Color.hpp>
#include <TGUI/Loading/Deserializer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#ifndef TGUI_USE_CPP17
    const Color Color::Black      {  0,   0,   0};
    const Color Color::White      {255, 255, 255};
    const Color Color::Red        {255,   0,   0};
    const Color Color::Green      {  0, 255,   0};
    const Color Color::Blue       {  0,   0, 255};
    const Color Color::Yellow     {255, 255,   0};
    const Color Color::Magenta    {255,   0, 255};
    const Color Color::Cyan       {  0, 255, 255};
    const Color Color::Transparent{  0,   0,   0,   0};

    const std::map<std::string, Color> Color::colorMap =
    {
        {"black", Color::Black},
        {"white", Color::White},
        {"red", Color::Red},
        {"yellow", Color::Yellow},
        {"green", Color::Green},
        {"cyan", Color::Cyan},
        {"blue", Color::Blue},
        {"magenta", Color::Magenta},
        {"transparent", Color::Transparent}
    };
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::Color(const std::string& string) :
        m_isSet{true},
        m_color(Deserializer::deserialize(ObjectConverter::Type::Color, string).getColor()) // Did not compile with clang 3.6 when using braces
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::Color(const char* string) :
        Color{std::string{string}}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::isSet() const
    {
        return m_isSet;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color::operator sf::Color() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getRed() const
    {
        return m_color.r;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getGreen() const
    {
        return m_color.g;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getBlue() const
    {
        return m_color.b;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint8_t Color::getAlpha() const
    {
        return m_color.a;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator==(const Color& right) const
    {
        return (m_isSet == right.m_isSet) && (m_color == right.m_color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator!=(const Color& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator==(const sf::Color& right) const
    {
        return m_isSet && (m_color == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Color::operator!=(const sf::Color& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/CustomWidgetForBindings.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CustomWidgetForBindings::Ptr CustomWidgetForBindings::create()
    {
        return std::make_shared<CustomWidgetForBindings>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);
        implPositionChanged(position.getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setSize(const Layout2d& size)
    {
        Widget::setSize(size);
        implSizeChanged(size.getValue());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getFullSize() const
    {
        return implGetFullSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getAbsolutePosition() const
    {
        return implGetAbsolutePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CustomWidgetForBindings::getWidgetOffset() const
    {
        return implGetWidgetOffset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setVisible(bool visible)
    {
        Widget::setVisible(visible);
        implVisibleChanged(visible);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        implEnableChanged(enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::setFocused(bool focused)
    {
        Widget::setFocused(focused);
        implFocusChanged(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::canGainFocus() const
    {
        return implCanGainFocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);
        implUpdateFunction(elapsedTime);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::mouseOnWidget(Vector2f pos) const
    {
        return implMouseOnWidget(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);
        implLeftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::leftMouseReleased(Vector2f pos)
    {
        Widget::leftMouseReleased(pos);
        implLeftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rightMousePressed(Vector2f pos)
    {
        Widget::rightMousePressed(pos);
        implRightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rightMouseReleased(Vector2f pos)
    {
        Widget::rightMouseReleased(pos);
        implRightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);
        implMouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::keyPressed(const sf::Event::KeyEvent& event)
    {
        Widget::keyPressed(event);
        implKeyPressed(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::textEntered(std::uint32_t key)
    {
        Widget::textEntered(key);
        implTextEntered(key);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool CustomWidgetForBindings::mouseWheelScrolled(float delta, Vector2f pos)
    {
        return implMouseWheelScrolled(delta, pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        implMouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        implMouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::rendererChanged(const std::string& property)
    {
        if (!implRendererChanged(property))
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        implMouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        implMouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CustomWidgetForBindings::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        implDrawFunction(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Global.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/DefaultFont.hpp>
#include <functional>
#include <sstream>
#include <locale>
#include <cctype> // isspace
#include <cmath> // abs

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        unsigned int globalTextSize = 13;
        unsigned int globalDoubleClickTime = 500;
        std::string globalResourcePath = "";
        std::shared_ptr<sf::Font> globalFont = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setGlobalTextSize(unsigned int textSize)
    {
        globalTextSize = textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getGlobalTextSize()
    {
        return globalTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setGlobalFont(const Font& font)
    {
        globalFont = font.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font getGlobalFont()
    {
        if (!globalFont)
        {
            globalFont = std::make_shared<sf::Font>();
            globalFont->loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes));
        }

        return globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<sf::Font>& getInternalGlobalFont()
    {
        return globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setDoubleClickTime(unsigned int milliseconds)
    {
        globalDoubleClickTime = milliseconds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int getDoubleClickTime()
    {
        return globalDoubleClickTime;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const std::string& path)
    {
        globalResourcePath = path;

        if (!globalResourcePath.empty())
        {
            if (globalResourcePath[globalResourcePath.length()-1] != '/')
                globalResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& getResourcePath()
    {
        return globalResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool compareFloats(float x, float y)
    {
        return (std::abs(x - y) < 0.0000001f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(std::uint32_t character)
    {
        if (character == ' ' || character == '\t' || character == '\r' || character == '\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int stoi(const std::string& value)
    {
        int result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float stof(const std::string& value)
    {
        float result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractBoolFromString(const std::string& property, const std::string& value)
    {
        if ((value == "true") || (value == "True") || (value == "TRUE") || (value == "1"))
            return true;
        else if ((value == "false") || (value == "False") || (value == "FALSE") || (value == "0"))
            return false;
        else
            throw Exception{"Failed to parse boolean value of property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool removeWhitespace(const std::string& line, std::string::const_iterator& c)
    {
        while (c != line.end())
        {
            if ((*c == ' ') || (*c == '\t') || (*c == '\r'))
                ++c;
            else
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toLower(std::string str)
    {
        for (auto& c : str)
            c = static_cast<char>(std::tolower(c));

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string trim(std::string str)
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isspace(c); }));
        str.erase(std::find_if(str.rbegin(), str.rend(), [](int c) { return !std::isspace(c); }).base(), str.end());
        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Layout.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Gui.hpp>
#include <TGUI/to_string.hpp>
#include <SFML/System/Err.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(std::string expression)
    {
        // Empty strings have value 0 (although this might indicate a mistake in the expression, it is valid for unary minus)
        expression = trim(expression);
        if (expression.empty())
            return;

        auto searchPos = expression.find_first_of("+-*/()");

        // Extract the value from the string when there are no more operators
        if (searchPos == std::string::npos)
        {
            // Convert percentages to references to the parent widget
            if (expression.back() == '%')
            {
                // We don't know if we have to bind the width or height, so bind "size" and let the connectWidget function figure it out later
                if (expression == "100%")
                {
                    m_boundString = "&.innersize";
                    m_operation = Operation::BindingString;
                }
                else // value is a fraction of parent size
                {
                    *this = Layout{Layout::Operation::Multiplies,
                                   std::make_unique<Layout>(tgui::stof(expression.substr(0, expression.length()-1)) / 100.f),
                                   std::make_unique<Layout>("&.innersize")};
                }
            }
            else
            {
                // The expression might reference to a widget instead of being a constant
                expression = toLower(expression);
                if ((expression.substr(expression.size()-1) == "x")
                 || (expression.substr(expression.size()-1) == "y")
                 || (expression.substr(expression.size()-1) == "w") // width
                 || (expression.substr(expression.size()-1) == "h") // height
                 || (expression.size() >= 2 && expression.substr(expression.size()-2) == "iw") // width inside the container
                 || (expression.size() >= 2 && expression.substr(expression.size()-2) == "ih") // height inside the container
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "left")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "top")
                 || (expression.size() >= 5 && expression.substr(expression.size()-5) == "width")
                 || (expression.size() >= 6 && expression.substr(expression.size()-6) == "height")
                 || (expression.size() >= 4 && expression.substr(expression.size()-4) == "size")
                 || (expression.size() >= 3 && expression.substr(expression.size()-3) == "pos")
                 || (expression.size() >= 8 && expression.substr(expression.size()-8) == "position")
                 || (expression.size() >= 9 && expression.substr(expression.size()-9) == "innersize")
                 || (expression.size() >= 10 && expression.substr(expression.size()-10) == "innerwidth")
                 || (expression.size() >= 11 && expression.substr(expression.size()-11) == "innerheight"))
                {
                    // We can't search for the referenced widget yet as no widget is connected to the widget yet, so store the string for future parsing
                    m_boundString = expression;
                    m_operation = Operation::BindingString;
                }
                else if (expression.size() >= 5 && expression.substr(expression.size()-5) == "right")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "left"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-5) + "width")};
                }
                else if (expression.size() >= 6 && expression.substr(expression.size()-6) == "bottom")
                {
                    *this = Layout{Operation::Plus,
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "top"),
                                   std::make_unique<Layout>(expression.substr(0, expression.size()-6) + "height")};
                }
                else // Constant value
                    m_value = tgui::stof(expression);
            }

            return;
        }

        // The string contains an expression, so split it up in multiple layouts
        std::list<Layout> operands;
        std::vector<Operation> operators;
        decltype(searchPos) prevSearchPos = 0;
        while (searchPos != std::string::npos)
        {
            switch (expression[searchPos])
            {
            case '+':
                operators.push_back(Operation::Plus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '-':
                operators.push_back(Operation::Minus);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '*':
                operators.push_back(Operation::Multiplies);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '/':
                operators.push_back(Operation::Divides);
                operands.emplace_back(expression.substr(prevSearchPos, searchPos - prevSearchPos));
                break;
            case '(':
            {
                // Find corresponding closing bracket
                unsigned int bracketCount = 0;
                auto bracketPos = expression.find_first_of("()", searchPos + 1);
                while (bracketPos != std::string::npos)
                {
                    if (expression[bracketPos] == '(')
                        bracketCount++;
                    else if (bracketCount > 0)
                        bracketCount--;
                    else
                    {
                        // If the entire layout was in brackets then remove these brackets
                        if ((searchPos == 0) && (bracketPos == expression.size()-1))
                        {
                            *this = Layout{expression.substr(1, expression.size()-2)};
                            return;
                        }
                        else // The brackets form a sub-expression
                            searchPos = bracketPos;

                        break;
                    }

                    bracketPos = expression.find_first_of("()", bracketPos + 1);
                }

                if (bracketPos == std::string::npos)
                {
                    TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                    return;
                }
                else
                {
                    // Search for the next operator, starting from the closing bracket, but keeping prevSearchPos before the opening bracket
                    searchPos = expression.find_first_of("+-*/()", searchPos + 1);
                    continue;
                }
            }
            case ')':
                TGUI_PRINT_WARNING("bracket mismatch while parsing layout string '" << expression << "'.");
                return;
            };

            prevSearchPos = searchPos + 1;
            searchPos = expression.find_first_of("+-*/()", searchPos + 1);
        }

        operands.emplace_back(expression.substr(prevSearchPos));

        // First perform all * and / operations
        auto operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Multiplies) || (operators[i] == Operation::Divides))
            {
                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                (*operandIt) = Layout{operators[i],
                                      std::make_unique<Layout>(*operandIt),
                                      std::make_unique<Layout>(*nextOperandIt)};

                operands.erase(nextOperandIt);
            }
            else
                ++operandIt;
        }

        // Now perform all + and - operations
        operandIt = operands.begin();
        for (std::size_t i = 0; i < operators.size(); ++i)
        {
            if ((operators[i] == Operation::Plus) || (operators[i] == Operation::Minus))
            {
                assert(operandIt != operands.end());

                auto nextOperandIt = operandIt;
                std::advance(nextOperandIt, 1);

                assert(nextOperandIt != operands.end());

                (*operandIt) = Layout{operators[i],
                                      std::make_unique<Layout>(*operandIt),
                                      std::make_unique<Layout>(*nextOperandIt)};

                operands.erase(nextOperandIt);
            }
        }

        assert(operands.size() == 1);
        *this = operands.front();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, Widget* boundWidget) :
        m_operation  {operation},
        m_boundWidget{boundWidget}
    {
        assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
               || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
               || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));
        assert(m_boundWidget != nullptr);

        if (m_operation == Operation::BindingLeft)
            m_value = m_boundWidget->getPosition().x;
        else if (m_operation == Operation::BindingTop)
            m_value = m_boundWidget->getPosition().y;
        else if (m_operation == Operation::BindingWidth)
            m_value = m_boundWidget->getSize().x;
        else if (m_operation == Operation::BindingHeight)
            m_value = m_boundWidget->getSize().y;
        else if (m_operation == Operation::BindingInnerWidth)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().x;
        }
        else if (m_operation == Operation::BindingInnerHeight)
        {
            const auto* boundContainer = dynamic_cast<Container*>(boundWidget);
            if (boundContainer)
                m_value = boundContainer->getInnerSize().y;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Operation operation, std::unique_ptr<Layout> leftOperand, std::unique_ptr<Layout> rightOperand) :
        m_operation   {operation},
        m_leftOperand {std::move(leftOperand)},
        m_rightOperand{std::move(rightOperand)}
    {
        assert(m_leftOperand != nullptr);
        assert(m_rightOperand != nullptr);
        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(const Layout& other) :
        m_value          {other.m_value},
        m_parent         {other.m_parent},
        m_operation      {other.m_operation},
        m_leftOperand    {other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr},
        m_rightOperand   {other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {other.m_boundString}
    {
        // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
        if (!m_boundString.empty())
            m_boundWidget = nullptr;

        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::Layout(Layout&& other) :
        m_value          {std::move(other.m_value)},
        m_parent         {std::move(other.m_parent)},
        m_operation      {other.m_operation},
        m_leftOperand    {std::move(other.m_leftOperand)},
        m_rightOperand   {std::move(other.m_rightOperand)},
        m_boundWidget    {other.m_boundWidget},
        m_boundString    {std::move(other.m_boundString)}
    {
        resetPointers();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(const Layout& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = other.m_value;
            m_parent          = other.m_parent;
            m_operation       = other.m_operation;
            m_leftOperand     = other.m_leftOperand ? std::make_unique<Layout>(*other.m_leftOperand) : nullptr;
            m_rightOperand    = other.m_rightOperand ? std::make_unique<Layout>(*other.m_rightOperand) : nullptr;
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = other.m_boundString;

            // Disconnect the bound widget if a string was used, the same name may apply to a different widget now
            if (!m_boundString.empty())
                m_boundWidget = nullptr;

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout& Layout::operator=(Layout&& other)
    {
        if (this != &other)
        {
            unbindLayout();

            m_value           = std::move(other.m_value);
            m_parent          = std::move(other.m_parent);
            m_operation       = other.m_operation;
            m_leftOperand     = std::move(other.m_leftOperand);
            m_rightOperand    = std::move(other.m_rightOperand);
            m_boundWidget     = other.m_boundWidget;
            m_boundString     = std::move(other.m_boundString);

            resetPointers();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::~Layout()
    {
        unbindLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Layout::toString() const
    {
        if (m_operation == Operation::Value)
        {
            return to_string(m_value);
        }
        else if ((m_operation == Operation::Plus) || (m_operation == Operation::Minus) || (m_operation == Operation::Multiplies) || (m_operation == Operation::Divides))
        {
            char operatorChar;
            if (m_operation == Operation::Plus)
                operatorChar = '+';
            else if (m_operation == Operation::Minus)
                operatorChar = '-';
            else if (m_operation == Operation::Multiplies)
                operatorChar = '*';
            else // if (m_operation == Operation::Divides)
                operatorChar = '/';

            if ((m_leftOperand->m_leftOperand) && (m_rightOperand->m_leftOperand))
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else if (m_leftOperand->m_leftOperand)
                return "(" + m_leftOperand->toString() + ") " + operatorChar + " " + m_rightOperand->toString();
            else if (m_rightOperand->m_leftOperand)
                return m_leftOperand->toString() + " " + operatorChar + " (" + m_rightOperand->toString() + ")";
            else
                return m_leftOperand->toString() + " " + operatorChar + " " + m_rightOperand->toString();
        }
        else
        {
            // Hopefully the expression is stored in the bound string, otherwise (i.e. when bind functions were used) it is infeasible to turn it into a string
            return m_boundString;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindLayout()
    {
        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->unbindPositionLayout(this);
            else
                m_boundWidget->unbindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::resetPointers()
    {
        if (m_leftOperand != nullptr)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->m_parent = this;
            m_rightOperand->m_parent = this;
        }

        if (m_boundWidget)
        {
            assert((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop)
                   || (m_operation == Operation::BindingWidth) || (m_operation == Operation::BindingHeight)
                   || (m_operation == Operation::BindingInnerWidth) || (m_operation == Operation::BindingInnerHeight));

            if ((m_operation == Operation::BindingLeft) || (m_operation == Operation::BindingTop))
                m_boundWidget->bindPositionLayout(this);
            else
                m_boundWidget->bindSizeLayout(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::connectWidget(Widget* widget, bool xAxis, std::function<void()> valueChangedCallbackHandler)
    {
        const float oldValue = m_value;

        // No callbacks must be made while parsing, a single callback will be made when done if needed
        m_connectedWidgetCallback = nullptr;

        parseBindingStringRecursive(widget, xAxis);

        // Restore the callback function
        m_connectedWidgetCallback = valueChangedCallbackHandler;

        if (m_value != oldValue)
        {
            if (m_connectedWidgetCallback)
                m_connectedWidgetCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::unbindWidget()
    {
        m_boundWidget = nullptr;

        if (!m_boundString.empty())
            m_operation = Operation::BindingString;
        else
        {
            m_value = 0;
            m_operation = Operation::Value;
        }

        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::recalculateValue()
    {
        const float oldValue = m_value;

        switch (m_operation)
        {
            case Operation::Value:
                break;
            case Operation::Plus:
                m_value = m_leftOperand->getValue() + m_rightOperand->getValue();
                break;
            case Operation::Minus:
                m_value = m_leftOperand->getValue() - m_rightOperand->getValue();
                break;
            case Operation::Multiplies:
                m_value = m_leftOperand->getValue() * m_rightOperand->getValue();
                break;
            case Operation::Divides:
                if (m_rightOperand->getValue() != 0)
                    m_value = m_leftOperand->getValue() / m_rightOperand->getValue();
                else
                    m_value = 0;
                break;
            case Operation::BindingLeft:
                m_value = m_boundWidget->getPosition().x;
                break;
            case Operation::BindingTop:
                m_value = m_boundWidget->getPosition().y;
                break;
            case Operation::BindingWidth:
                m_value = m_boundWidget->getSize().x;
                break;
            case Operation::BindingHeight:
                m_value = m_boundWidget->getSize().y;
                break;
            case Operation::BindingInnerWidth:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().x;
                break;
            }
            case Operation::BindingInnerHeight:
            {
                const auto* boundContainer = dynamic_cast<Container*>(m_boundWidget);
                if (boundContainer)
                    m_value = boundContainer->getInnerSize().y;
                break;
            }
            case Operation::BindingString:
                // The string should have already been parsed by now.
                // Passing here either means something is wrong with the string or the layout was not connected to a widget with a parent yet.
                break;
        };

        if (m_value != oldValue)
        {
            if (m_parent)
                m_parent->recalculateValue();
            else
            {
                // The topmost layout must tell the connected widget about the new value
                if (m_connectedWidgetCallback)
                    m_connectedWidgetCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingStringRecursive(Widget* widget, bool xAxis)
    {
        if (m_leftOperand)
        {
            assert(m_rightOperand != nullptr);

            m_leftOperand->parseBindingStringRecursive(widget, xAxis);
            m_rightOperand->parseBindingStringRecursive(widget, xAxis);
        }

        // Parse the string binding even when the referred widget was already found. The widget may be added to a different parent
        if (!m_boundString.empty())
            parseBindingString(m_boundString, widget, xAxis);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::parseBindingString(const std::string& expression, Widget* widget, bool xAxis)
    {
        if (expression == "x" || expression == "left")
        {
            m_operation = Operation::BindingLeft;
            m_boundWidget = widget;
        }
        else if (expression == "y" || expression == "top")
        {
            m_operation = Operation::BindingTop;
            m_boundWidget = widget;
        }
        else if (expression == "w" || expression == "width")
        {
            m_operation = Operation::BindingWidth;
            m_boundWidget = widget;
        }
        else if (expression == "h" || expression == "height")
        {
            m_operation = Operation::BindingHeight;
            m_boundWidget = widget;
        }
        else if (expression == "iw" || expression == "innerwidth")
        {
            m_operation = Operation::BindingInnerWidth;
            m_boundWidget = widget;
        }
        else if (expression == "ih" || expression == "innerheight")
        {
            m_operation = Operation::BindingInnerHeight;
            m_boundWidget = widget;
        }
        else if (expression == "size")
        {
            if (xAxis)
                return parseBindingString("width", widget, xAxis);
            else
                return parseBindingString("height", widget, xAxis);
        }
        else if (expression == "innersize")
        {
            if (xAxis)
                return parseBindingString("innerwidth", widget, xAxis);
            else
                return parseBindingString("innerheight", widget, xAxis);
        }
        else if ((expression == "pos") || (expression == "position"))
        {
            if (xAxis)
                return parseBindingString("x", widget, xAxis);
            else
                return parseBindingString("y", widget, xAxis);
        }
        else
        {
            const auto dotPos = expression.find('.');
            if (dotPos != std::string::npos)
            {
                const std::string widgetName = expression.substr(0, dotPos);
                if (widgetName == "parent" || widgetName == "&")
                {
                    if (widget->getParent())
                        return parseBindingString(expression.substr(dotPos+1), widget->getParent(), xAxis);
                }
                else if (!widgetName.empty())
                {
                    // If the widget is a container, search in its children first
                    Container* container = dynamic_cast<Container*>(widget);
                    if (container != nullptr)
                    {
                        const auto& widgets = container->getWidgets();
                        const auto& widgetNames = container->getWidgetNames();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgetNames[i]) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }

                    // If the widget has a parent, look for a sibling
                    if (widget->getParent())
                    {
                        const auto& widgets = widget->getParent()->getWidgets();
                        const auto& widgetNames = widget->getParent()->getWidgetNames();
                        for (std::size_t i = 0; i < widgets.size(); ++i)
                        {
                            if (toLower(widgetNames[i]) == widgetName)
                                return parseBindingString(expression.substr(dotPos+1), widgets[i].get(), xAxis);
                        }
                    }
                }
            }

            // The referred widget was not found or there was something wrong with the string
            return;
        }

        resetPointers();
        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator+(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Plus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator-(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Minus, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator*(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Multiplies, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout operator/(Layout left, Layout right)
    {
        return Layout{Layout::Operation::Divides, std::make_unique<Layout>(std::move(left)), std::make_unique<Layout>(std::move(right))};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d right)
    {
        return Layout2d{-std::move(right.x), -std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator+(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) + std::move(right.x), std::move(left.y) + std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator-(Layout2d left, Layout2d right)
    {
        return Layout2d{std::move(left.x) - std::move(right.x), std::move(left.y) - std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) * right, std::move(left.y) * right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator*(const Layout& left, Layout2d right)
    {
        return Layout2d{left * std::move(right.x), left * std::move(right.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout2d operator/(Layout2d left, const Layout& right)
    {
        return Layout2d{std::move(left.x) / right, std::move(left.y) / right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    inline namespace bind_functions
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindLeft(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingLeft, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindTop(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingTop, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingWidth, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::BindingHeight, widget.get()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindRight(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingLeft, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingWidth, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindBottom(Widget::Ptr widget)
        {
            return Layout{Layout::Operation::Plus,
                          std::make_unique<Layout>(Layout::Operation::BindingTop, widget.get()),
                          std::make_unique<Layout>(Layout::Operation::BindingHeight, widget.get())};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindPosition(Widget::Ptr widget)
        {
            return {bindLeft(widget), bindTop(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Widget::Ptr widget)
        {
            return {bindWidth(widget), bindHeight(widget)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindWidth(Gui& gui)
        {
            return bindWidth(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout bindHeight(Gui& gui)
        {
            return bindHeight(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Layout2d bindSize(Gui& gui)
        {
            return bindSize(gui.getContainer());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Sprite.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Clipping.hpp>

#include <cassert>

#ifdef TGUI_USE_CPP17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    void Sprite::setTexture(const Texture& texture)
    {
        m_texture = texture;

        if (isSet())
        {
            if (getSize() == Vector2f{})
                setSize(texture.getImageSize());
            else
                updateVertices();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& Sprite::getTexture() const
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Sprite::getTexture()
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isSet() const
    {
        return m_texture.getData() != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setSize(const Vector2f& size)
    {
        m_size.x = std::max(size.x, 0.f);
        m_size.y = std::max(size.y, 0.f);

        if (isSet())
            updateVertices();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Sprite::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setColor(const Color& color)
    {
        m_vertexColor = color;

        const sf::Color vertexColor = Color::calcColorOpacity(m_vertexColor, m_opacity);
        for (auto& vertex : m_vertices)
            vertex.color = vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Sprite::getColor() const
    {
        return m_vertexColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setOpacity(float opacity)
    {
        m_opacity = opacity;
        setColor(getColor());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Sprite::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::setVisibleRect(const FloatRect& visibleRect)
    {
        m_visibleRect = visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect Sprite::getVisibleRect() const
    {
        return m_visibleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Sprite::isTransparentPixel(Vector2f pos) const
    {
        if (!isSet() || !m_texture.getData()->image || (m_size.x == 0) || (m_size.y == 0))
            return true;

        if (getRotation() != 0)
        {
            Vector2f offset = {getTransform().transformRect(FloatRect({}, getSize())).left,
                               getTransform().transformRect(FloatRect({}, getSize())).top};

            pos = getInverseTransform().transformPoint(pos) + getInverseTransform().transformPoint(offset);

            // Watch out for rounding errors
            const float epsilon = 0.00001f;
            if ((pos.x < 0) && (pos.x > -epsilon))
                pos.x = 0;
            if ((pos.y < 0) && (pos.y > -epsilon))
                pos.y = 0;
            if ((pos.x >= getSize().x) && (pos.x < getSize().x + epsilon))
                pos.x = getSize().x;
            if ((pos.y >= getSize().y) && (pos.y < getSize().y + epsilon))
                pos.y = getSize().y;
        }
        else // There is no rotation
            pos -= getPosition();

        if ((pos.x < 0) || (pos.y < 0) || (pos.x >= getSize().x) || (pos.y >= getSize().y))
            return true;

        // Find out on which pixel the mouse is standing
        sf::Vector2u pixel;
        sf::IntRect middleRect = m_texture.getMiddleRect();
        const sf::Texture& texture = m_texture.getData()->texture;
        switch (m_scalingType)
        {
            case ScalingType::Normal:
            {
                pixel.x = static_cast<unsigned int>(pos.x / m_size.x * texture.getSize().x);
                pixel.y = static_cast<unsigned int>(pos.y / m_size.y * texture.getSize().y);
                break;
            }
            case ScalingType::Horizontal:
            {
                if (pos.x >= m_size.x - (texture.getSize().x - middleRect.left - middleRect.width) * (m_size.y / texture.getSize().y))
                {
                    float xDiff = (pos.x - (m_size.x - (texture.getSize().x - middleRect.left - middleRect.width) * (m_size.y / texture.getSize().y)));
                    pixel.x = static_cast<unsigned int>(middleRect.left + middleRect.width + (xDiff / m_size.y * texture.getSize().y));
                }
                else if (pos.x >= middleRect.left * (m_size.y / texture.getSize().y))
                {
                    float xDiff = pos.x - (middleRect.left * (m_size.y / texture.getSize().y));
                    pixel.x = static_cast<unsigned int>(middleRect.left + (xDiff / (m_size.x - ((texture.getSize().x - middleRect.width) * (m_size.y / texture.getSize().y))) * middleRect.width));
                }
                else // Mouse on the left part
                {
                    pixel.x = static_cast<unsigned int>(pos.x / m_size.y * texture.getSize().y);
                }

                pixel.y = static_cast<unsigned int>(pos.y / m_size.y * texture.getSize().y);
                break;
            }
            case ScalingType::Vertical:
            {
                if (pos.y >= m_size.y - (texture.getSize().y - middleRect.top - middleRect.height) * (m_size.x / texture.getSize().x))
                {
                    float yDiff = (pos.y - (m_size.y - (texture.getSize().y - middleRect.top - middleRect.height) * (m_size.x / texture.getSize().x)));
                    pixel.y = static_cast<unsigned int>(middleRect.top + middleRect.height + (yDiff / m_size.x * texture.getSize().x));
                }
                else if (pos.y >= middleRect.top * (m_size.x / texture.getSize().x))
                {
                    float yDiff = pos.y - (middleRect.top * (m_size.x / texture.getSize().x));
                    pixel.y = static_cast<unsigned int>(middleRect.top + (yDiff / (m_size.y - ((texture.getSize().y - middleRect.height) * (m_size.x / texture.getSize().x))) * middleRect.height));
                }
                else // Mouse on the top part
                {
                    pixel.y = static_cast<unsigned int>(pos.y / m_size.x * texture.getSize().x);
                }

                pixel.x = static_cast<unsigned int>(pos.x / m_size.x * texture.getSize().x);
                break;
            }
            case ScalingType::NineSlice:
            {
                if (pos.x < middleRect.left)
                    pixel.x = static_cast<unsigned int>(pos.x);
                else if (pos.x >= m_size.x - (texture.getSize().x - middleRect.width - middleRect.left))
                    pixel.x = static_cast<unsigned int>(pos.x - m_size.x + texture.getSize().x);
                else
                {
                    float xDiff = (pos.x - middleRect.left) / (m_size.x - (texture.getSize().x - middleRect.width)) * middleRect.width;
                    pixel.x = static_cast<unsigned int>(middleRect.left + xDiff);
                }

                if (pos.y < middleRect.top)
                    pixel.y = static_cast<unsigned int>(pos.y);
                else if (pos.y >= m_size.y - (texture.getSize().y - middleRect.height - middleRect.top))
                    pixel.y = static_cast<unsigned int>(pos.y - m_size.y + texture.getSize().y);
                else
                {
                    float yDiff = (pos.y - middleRect.top) / (m_size.y - (texture.getSize().y - middleRect.height)) * middleRect.height;
                    pixel.y = static_cast<unsigned int>(middleRect.top + yDiff);
                }

                break;
            }
        };

        return m_texture.isTransparentPixel(pixel);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::ScalingType Sprite::getScalingType() const
    {
        return m_scalingType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::updateVertices()
    {
        // Figure out how the image is scaled best
        Vector2f textureSize{m_texture.getImageSize()};
        FloatRect middleRect{sf::FloatRect{m_texture.getMiddleRect()}};
        if (middleRect == FloatRect(0, 0, textureSize.x, textureSize.y))
        {
            m_scalingType = ScalingType::Normal;
        }
        else if (middleRect.height == textureSize.y)
        {
            if (m_size.x >= (textureSize.x - middleRect.width) * (m_size.y / textureSize.y))
                m_scalingType = ScalingType::Horizontal;
            else
                m_scalingType = ScalingType::Normal;
        }
        else if (middleRect.width == textureSize.x)
        {
            if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                m_scalingType = ScalingType::Vertical;
            else
                m_scalingType = ScalingType::Normal;
        }
        else
        {
            if (m_size.x >= textureSize.x - middleRect.width)
            {
                if (m_size.y >= textureSize.y - middleRect.height)
                    m_scalingType = ScalingType::NineSlice;
                else
                    m_scalingType = ScalingType::Horizontal;
            }
            else if (m_size.y >= (textureSize.y - middleRect.height) * (m_size.x / textureSize.x))
                m_scalingType = ScalingType::Vertical;
            else
                m_scalingType = ScalingType::Normal;
        }

        // Calculate the vertices based on the way we are scaling
        switch (m_scalingType)
        {
        case ScalingType::Normal:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            ///////////
            m_vertices.resize(4);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[3] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Horizontal:
            ///////////////////////
            // 0---2-------4---6 //
            // |   |       |   | //
            // 1---3-------5---7 //
            ///////////////////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[2] = {{middleRect.left * (m_size.y / textureSize.y), 0}, m_vertexColor, {middleRect.left, 0}};
            m_vertices[3] = {{middleRect.left * (m_size.y / textureSize.y), m_size.y}, m_vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[4] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), 0}, m_vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[5] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width) * (m_size.y / textureSize.y), m_size.y}, m_vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[6] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[7] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::Vertical:
            ///////////
            // 0---1 //
            // |   | //
            // 2---3 //
            // |   | //
            // |   | //
            // |   | //
            // 4---5 //
            // |   | //
            // 6---7-//
            ///////////
            m_vertices.resize(8);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[2] = {{0, middleRect.top * (m_size.x / textureSize.x)}, m_vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{m_size.x, middleRect.top * (m_size.x / textureSize.x)}, m_vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, m_vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height) * (m_size.x / textureSize.x)}, m_vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;

        case ScalingType::NineSlice:
            //////////////////////////////////
            // 0---1/13-----------14-----15 //
            // |    |              |     |  //
            // 2---3/11----------12/16---17 //
            // |    |              |     |  //
            // |    |              |     |  //
            // |    |              |     |  //
            // 4---5/9-----------10/18---19 //
            // |    |              |     |  //
            // 6----7-------------8/20---21 //
            //////////////////////////////////
            m_vertices.resize(22);
            m_vertices[0] = {{0, 0}, m_vertexColor, {0, 0}};
            m_vertices[1] = {{middleRect.left, 0}, m_vertexColor, {middleRect.left, 0}};
            m_vertices[2] = {{0, middleRect.top}, m_vertexColor, {0, middleRect.top}};
            m_vertices[3] = {{middleRect.left, middleRect.top}, m_vertexColor, {middleRect.left, middleRect.top}};
            m_vertices[4] = {{0, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {0, middleRect.top + middleRect.height}};
            m_vertices[5] = {{middleRect.left, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {middleRect.left, middleRect.top + middleRect.height}};
            m_vertices[6] = {{0, m_size.y}, m_vertexColor, {0, textureSize.y}};
            m_vertices[7] = {{middleRect.left, m_size.y}, m_vertexColor, {middleRect.left, textureSize.y}};
            m_vertices[8] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y}, m_vertexColor, {middleRect.left + middleRect.width, textureSize.y}};
            m_vertices[9] = m_vertices[5];
            m_vertices[10] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {middleRect.left + middleRect.width, middleRect.top + middleRect.height}};
            m_vertices[11] = m_vertices[3];
            m_vertices[12] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), middleRect.top}, m_vertexColor, {middleRect.left + middleRect.width, middleRect.top}};
            m_vertices[13] = m_vertices[1];
            m_vertices[14] = {{m_size.x - (textureSize.x - middleRect.left - middleRect.width), 0}, m_vertexColor, {middleRect.left + middleRect.width, 0}};
            m_vertices[15] = {{m_size.x, 0}, m_vertexColor, {textureSize.x, 0}};
            m_vertices[16] = m_vertices[12];
            m_vertices[17] = {{m_size.x, middleRect.top}, m_vertexColor, {textureSize.x, middleRect.top}};
            m_vertices[18] = m_vertices[10];
            m_vertices[19] = {{m_size.x, m_size.y - (textureSize.y - middleRect.top - middleRect.height)}, m_vertexColor, {textureSize.x, middleRect.top + middleRect.height}};
            m_vertices[20] = m_vertices[8];
            m_vertices[21] = {{m_size.x, m_size.y}, m_vertexColor, {textureSize.x, textureSize.y}};
            break;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
        if (getRotation() != 0)
        {
            Vector2f pos = {getTransform().transformRect(FloatRect({}, getSize())).left,
                            getTransform().transformRect(FloatRect({}, getSize())).top};

            states.transform.translate(getPosition() - pos);
        }

        states.transform *= getTransform();

        if (isSet())
        {
            // Apply clipping when needed
        #ifdef TGUI_USE_CPP17
            std::optional<Clipping> clipping;
            if (m_visibleRect != FloatRect{})
                clipping.emplace(target, states, Vector2f{m_visibleRect.left, m_visibleRect.top}, Vector2f{m_visibleRect.width, m_visibleRect.height});
        #else
            std::unique_ptr<Clipping> clipping;
            if (m_visibleRect != FloatRect{0, 0, 0, 0})
                clipping = std::make_unique<Clipping>(target, states, Vector2f{m_visibleRect.left, m_visibleRect.top}, Vector2f{m_visibleRect.width, m_visibleRect.height});
        #endif

            states.shader = m_texture.getData()->shader;
            states.texture = &m_texture.getData()->texture;
            target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Loading/Deserializer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle::TextStyle() :
        m_isSet{false},
        m_style{sf::Text::Regular}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle::TextStyle(unsigned int style) :
        m_isSet{true},
        m_style{style}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle::TextStyle(const std::string& string) :
        m_isSet{true},
        m_style{Deserializer::deserialize(ObjectConverter::Type::TextStyle, string).getTextStyle()}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle::TextStyle(const char* string) :
        TextStyle(std::string{string})
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextStyle::isSet() const
    {
        return m_isSet;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle::operator unsigned int() const
    {
        return m_style;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Global.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/TextureManager.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Texture::TextureLoaderFunc Texture::m_textureLoader = &TextureManager::getTexture;
    Texture::ImageLoaderFunc Texture::m_imageLoader = [](const sf::String& filename) -> std::unique_ptr<sf::Image>
        {
            auto image = std::make_unique<sf::Image>();
            if (image->loadFromFile(filename))
                return image;
            else
                return nullptr;
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool smooth)
    {
        load(id, partRect, middlePart, smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middlePart)
    {
        load(texture, partRect, middlePart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(const Texture& other) :
        m_data            {other.m_data},
        m_middleRect      {other.m_middleRect},
        m_id              {other.m_id},
        m_copyCallback    {other.m_copyCallback},
        m_destructCallback{other.m_destructCallback}
    {
        if (getData() && (m_copyCallback != nullptr))
            m_copyCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::Texture(Texture&& other) :
        m_data            {std::move(other.m_data)},
        m_middleRect      {std::move(other.m_middleRect)},
        m_id              {std::move(other.m_id)},
        m_copyCallback    {std::move(other.m_copyCallback)},
        m_destructCallback{std::move(other.m_destructCallback)}
    {
        other.m_data = nullptr;
        other.m_copyCallback = nullptr;
        other.m_destructCallback = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture::~Texture()
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(const Texture& other)
    {
        if (this != &other)
        {
            Texture temp{other};

            std::swap(m_data,             temp.m_data);
            std::swap(m_middleRect,       temp.m_middleRect);
            std::swap(m_id,               temp.m_id);
            std::swap(m_copyCallback,     temp.m_copyCallback);
            std::swap(m_destructCallback, temp.m_destructCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& Texture::operator=(Texture&& other)
    {
        if (this != &other)
        {
            m_data             = std::move(other.m_data);
            m_middleRect       = std::move(other.m_middleRect);
            m_id               = std::move(other.m_id);
            m_copyCallback     = std::move(other.m_copyCallback);
            m_destructCallback = std::move(other.m_destructCallback);

            other.m_data = nullptr;
            other.m_copyCallback = nullptr;
            other.m_destructCallback = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::String& id, const sf::IntRect& partRect, const sf::IntRect& middleRect, bool smooth)
    {
        if (id.isEmpty())
        {
            *this = Texture{};
            return;
        }

        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = nullptr;

        std::shared_ptr<TextureData> data;
    #ifdef SFML_SYSTEM_WINDOWS
        if ((id[0] != '/') && (id[0] != '\\') && ((id.getSize() <= 1) || (id[1] != ':')))
    #else
        if (id[0] != '/')
    #endif
        {
            data = m_textureLoader(*this, getResourcePath() + id, partRect);
            if (!data)
                throw Exception{"Failed to load '" + getResourcePath() + id + "'"};
        }
        else
        {
            data = m_textureLoader(*this, id, partRect);
            if (!data)
                throw Exception{"Failed to load '" + id + "'"};
        }

        m_id = id;
        setTextureData(data, middleRect);

        if (smooth)
            setSmooth(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::load(const sf::Texture& texture, const sf::IntRect& partRect, const sf::IntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = nullptr;
        auto data = std::make_shared<TextureData>();
        if (partRect == sf::IntRect{})
            data->texture = texture;
        else
            data->texture.loadFromImage(texture.copyToImage(), partRect);

        m_id = "";
        setTextureData(data, middleRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Texture::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> Texture::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Texture::getImageSize() const
    {
        if (m_data)
            return sf::Vector2f{m_data->texture.getSize()};
        else
            return {0,0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setSmooth(bool smooth)
    {
        if (m_data)
            m_data->texture.setSmooth(smooth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isSmooth() const
    {
        if (m_data)
            return m_data->texture.isSmooth();
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setShader(sf::Shader* shader)
    {
        if (m_data)
            m_data->shader = shader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Shader* Texture::getShader() const
    {
        if (m_data)
            return m_data->shader;
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::IntRect Texture::getMiddleRect() const
    {
        return m_middleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::isTransparentPixel(sf::Vector2u pixel) const
    {
        if (!m_data || !m_data->image)
            return false;

        assert(pixel.x < m_data->texture.getSize().x && pixel.y < m_data->texture.getSize().y);

        if (m_data->image->getPixel(pixel.x + m_data->rect.left, pixel.y + m_data->rect.top).a == 0)
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setCopyCallback(const CallbackFunc& func)
    {
        m_copyCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setDestructCallback(const CallbackFunc& func)
    {
        m_destructCallback = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator==(const Texture& right) const
    {
        if (m_id.isEmpty() && right.m_id.isEmpty())
            return (m_data == right.m_data) && (m_middleRect == right.m_middleRect);
        else
            return (m_middleRect == right.m_middleRect) && (m_id == right.m_id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Texture::operator!=(const Texture& right) const
    {
        return !(*this == right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setImageLoader(const ImageLoaderFunc& func)
    {
        assert(func != nullptr);
        m_imageLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::ImageLoaderFunc& Texture::getImageLoader()
    {
        return m_imageLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureLoader(const TextureLoaderFunc& func)
    {
        assert(func != nullptr);
        m_textureLoader = func;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture::TextureLoaderFunc& Texture::getTextureLoader()
    {
        return m_textureLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Texture::setTextureData(std::shared_ptr<TextureData> data, const sf::IntRect& middleRect)
    {
        if (getData() && (m_destructCallback != nullptr))
            m_destructCallback(getData());

        m_data = data;

        if (middleRect == sf::IntRect{})
            m_middleRect = {0, 0, static_cast<int>(m_data->texture.getSize().x), static_cast<int>(m_data->texture.getSize().y)};
        else
            m_middleRect = middleRect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/ToolTip.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Time ToolTip::m_timeToDisplay = sf::milliseconds(500);
    Vector2f ToolTip::m_distanceToMouse = {5, 20};

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ToolTip::setTimeToDisplay(const sf::Time& timeToDisplay)
    {
        m_timeToDisplay = timeToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Time ToolTip::getTimeToDisplay()
    {
        return m_timeToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ToolTip::setDistanceToMouse(const Vector2f& distance)
    {
        m_distanceToMouse = distance;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ToolTip::getDistanceToMouse()
    {
        return m_distanceToMouse;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Global.hpp>
#include <TGUI/to_string.hpp>

#include <cctype>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define REMOVE_WHITESPACE_AND_COMMENTS(ReturnErrorOnEOF) \
    for (;;) \
    { \
        stream >> std::ws; \
        if (stream.peek() == EOF) \
            break; \
        \
        if (stream.peek() == '/') \
        { \
            char c; \
            stream.read(&c, 1); \
            if (stream.peek() == '/') \
            { \
                while (stream.peek() != EOF) \
                { \
                    stream.read(&c, 1); \
                    if (c == '\n') \
                        break; \
                } \
            } \
            else if (stream.peek() == '*') \
            { \
                while (stream.peek() != EOF) \
                { \
                    stream.read(&c, 1); \
                    if (stream.peek() == '*') \
                    { \
                        stream.read(&c, 1); \
                        if (stream.peek() == '/') \
                        { \
                            stream.read(&c, 1); \
                            break; \
                        } \
                    } \
                } \
                continue; \
            } \
            else \
                return "Unexpected '/' found."; \
            \
            continue; \
        } \
        break; \
    } \
    \
    if (stream.peek() == EOF) \
    { \
        if (ReturnErrorOnEOF) \
            return "Unexpected EOF while parsing."; \
        else \
            return ""; \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        // Forward declare one of the functions to solve circular dependency
        std::string parseSection(std::stringstream& stream, const std::unique_ptr<DataIO::Node>& node, const std::string& sectionName);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string readWord(std::stringstream& stream)
        {
            std::string word = "";
            while (stream.peek() != EOF)
            {
                char c = static_cast<char>(stream.peek());
                if (c == '\r')
                {
                    stream.read(&c, 1);
                    return word;
                }
                else if (!::isspace(c) && (c != '=') && (c != ';') && (c != '{') && (c != '}'))
                {
                    stream.read(&c, 1);

                    if ((c == '/') && (stream.peek() == '/'))
                    {
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            if (c == '\n')
                            {
                                assert(!word.empty()); // No known case in which you can pass here with an empty word
                                return word;
                            }
                        }
                    }
                    else if ((c == '/') && (stream.peek() == '*'))
                    {
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            if (c == '*')
                            {
                                if (stream.peek() == '/')
                                {
                                    stream.read(&c, 1);
                                    break;
                                }
                            }
                        }
                    }
                    else if (c == '"')
                    {
                        word.push_back(c);
                        bool backslash = false;
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            word.push_back(c);

                            if (c == '"' && !backslash)
                                break;

                            if (c == '\\' && !backslash)
                                backslash = true;
                            else
                                backslash = false;
                        }
                    }
                    else
                        word.push_back(c);
                }
                else
                    return word;
            }

            return "";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string readLine(std::stringstream& stream)
        {
            std::string line;
            bool whitespaceFound = false;
            while (stream.peek() != EOF)
            {
                char c = static_cast<char>(stream.peek());

                if (stream.peek() == '/')
                {
                    stream.read(&c, 1);
                    if (stream.peek() == '/')
                    {
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            if (c == '\n')
                                break;
                        }
                    }
                    else if (stream.peek() == '*')
                    {
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            if (stream.peek() == '*')
                            {
                                stream.read(&c, 1);
                                if (stream.peek() == '/')
                                {
                                    stream.read(&c, 1);
                                    break;
                                }
                            }
                        }
                        continue;
                    }
                    else
                        return "";

                    continue;
                }

                if (c == '"')
                {
                    stream.read(&c, 1);
                    line.push_back(c);

                    bool backslash = false;
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        line.push_back(c);

                        if (c == '"' && !backslash)
                            break;

                        if (c == '\\' && !backslash)
                            backslash = true;
                        else
                            backslash = false;
                    }

                    if (stream.peek() == EOF)
                        return "";

                    c = static_cast<char>(stream.peek());
                }

                if ((c == '=') || (c == '{'))
                    return "";
                else if ((c == ';') || (c == '}'))
                {
                    // Remove trailing whitespace before returning the line
                    line.erase(line.find_last_not_of(" \n\r\t")+1);
                    return line;
                }
                else if (::isspace(c))
                {
                    stream.read(&c, 1);
                    if (!whitespaceFound)
                    {
                        whitespaceFound = true;
                        line.push_back(' ');
                    }
                }
                else
                {
                    whitespaceFound = false;
                    line.push_back(c);
                    stream.read(&c, 1);
                }
            }

            return "";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string parseKeyValue(std::stringstream& stream, std::unique_ptr<DataIO::Node>& node, const std::string& key)
        {
            // Read the assignment symbol from the stream and remove the whitespace behind it
            char chr;
            stream.read(&chr, 1);

            REMOVE_WHITESPACE_AND_COMMENTS(true)

            // Check for subsection as value
            if (stream.peek() == '{')
                return parseSection(stream, node, key);

            // Read the value
            std::string line = trim(readLine(stream));
            if (!line.empty())
            {
                // Remove the ';' if it is there
                if (stream.peek() == ';')
                    stream.read(&chr, 1);

                // Create a value node to store the value
                auto valueNode = std::make_unique<DataIO::ValueNode>();
                valueNode->value = line;

                // It might be a list node
                if ((line.size() >= 2) && (line[0] == '[') && (line.back() == ']'))
                {
                    valueNode->listNode = true;
                    if (line.size() >= 3)
                    {
                        valueNode->valueList.push_back("");

                        std::size_t i = 1;
                        while (i < line.size()-1)
                        {
                            if (line[i] == ',')
                            {
                                i++;
                                valueNode->valueList.back() = trim(valueNode->valueList.back());
                                valueNode->valueList.push_back("");
                            }
                            else if (line[i] == '"')
                            {
                                valueNode->valueList.back().insert(valueNode->valueList.back().getSize(), line[i]);
                                i++;

                                bool backslash = false;
                                while (i < line.size()-1)
                                {
                                    valueNode->valueList.back().insert(valueNode->valueList.back().getSize(), line[i]);

                                    if (line[i] == '"' && !backslash)
                                    {
                                        i++;
                                        break;
                                    }

                                    if (line[i] == '\\' && !backslash)
                                        backslash = true;
                                    else
                                        backslash = false;

                                    i++;
                                }
                            }
                            else
                            {
                                valueNode->valueList.back().insert(valueNode->valueList.back().getSize(), line[i]);
                                i++;
                            }
                        }

                        valueNode->valueList.back() = trim(valueNode->valueList.back());
                    }
                }

                node->propertyValuePairs[toLower(key)] = std::move(valueNode);
                return "";
            }
            else
            {
                if (stream.peek() == EOF)
                    return "Found EOF while trying to read a value.";
                else
                {
                    chr = static_cast<char>(stream.peek());
                    if (chr == '=')
                        return "Found '=' while trying to read a value.";
                    else if (chr == '{')
                        return "Found '{' while trying to read a value.";
                    else
                        return "Found empty value.";
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string parseSection(std::stringstream& stream, const std::unique_ptr<DataIO::Node>& node, const std::string& sectionName)
        {
            // Create a new node for this section
            auto sectionNode = std::make_unique<DataIO::Node>();
            sectionNode->parent = node.get();
            sectionNode->name = sectionName;

            // Read the brace from the stream
            char chr;
            stream.read(&chr, 1);

            while (stream.peek() != EOF)
            {
                REMOVE_WHITESPACE_AND_COMMENTS(true)

                std::string word = readWord(stream);
                if (word == "")
                {
                    if (stream.peek() == EOF)
                        return "Found EOF while trying to read property or nested section name.";
                    else if (stream.peek() == '}')
                    {
                        node->children.push_back(std::move(sectionNode));

                        stream.read(&chr, 1);

                        // Ignore semicolon behind closing brace
                        REMOVE_WHITESPACE_AND_COMMENTS(false)
                        if (stream.peek() == ';')
                            stream.read(&chr, 1);

                        REMOVE_WHITESPACE_AND_COMMENTS(false)
                        return "";
                    }
                    else if (stream.peek() != '{')
                        return "Expected property or nested section name, found '" + std::string(1, static_cast<char>(stream.peek())) + "' instead.";
                }

                REMOVE_WHITESPACE_AND_COMMENTS(true)
                if (stream.peek() == '{')
                {
                    std::string error = parseSection(stream, sectionNode, word);
                    if (!error.empty())
                        return error;
                }
                else if (stream.peek() == '=')
                {
                    std::string error = parseKeyValue(stream, sectionNode, word);
                    if (!error.empty())
                        return error;
                }
                else
                    return "Expected '{' or '=', found '" + std::string(1, static_cast<char>(stream.peek())) + "' instead.";
            }

            return "Found EOF while reading section.";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string parseRootSection(std::stringstream& stream, std::unique_ptr<DataIO::Node>& root)
        {
            REMOVE_WHITESPACE_AND_COMMENTS(false)

            std::string word = readWord(stream);
            if (word == "")
            {
                REMOVE_WHITESPACE_AND_COMMENTS(true)
                if (stream.peek() != '{')
                    return "Expected section name, found '" + std::string(1, static_cast<char>(stream.peek())) + "' instead.";
            }

            REMOVE_WHITESPACE_AND_COMMENTS(true)
            if (stream.peek() == '{')
                return parseSection(stream, root, word);
            else if (stream.peek() == '=')
                return parseKeyValue(stream, root, word);
            else
                return "Expected '{' or '=', found '" + std::string(1, static_cast<char>(stream.peek())) + "' instead.";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::string> convertNodesToLines(const std::unique_ptr<DataIO::Node>& node)
        {
            std::vector<std::string> output;
            if (node->name.empty())
                output.emplace_back("{");
            else
                output.emplace_back(node->name + " {");

            if (!node->propertyValuePairs.empty())
            {
                for (const auto& pair : node->propertyValuePairs)
                    output.emplace_back("    " + pair.first + " = " + pair.second->value + ";");
            }

            if (node->propertyValuePairs.size() > 0 && node->children.size() > 0)
                output.emplace_back("");

            if (!node->children.empty())
            {
                for (std::size_t i = 0; i < node->children.size(); ++i)
                {
                    for (const auto& line : convertNodesToLines(node->children[i]))
                        output.emplace_back("    " + line);

                    if (i < node->children.size() - 1)
                        output.emplace_back("");
                }
            }

            output.emplace_back("}");
            return output;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> DataIO::parse(std::stringstream& stream)
    {
        auto root = std::make_unique<Node>();

        std::string error;
        while (stream.peek() != EOF)
        {
            error = parseRootSection(stream, root);
            if (!error.empty())
            {
                if (stream.tellg() != std::stringstream::pos_type(-1))
                {
                    std::string str = stream.str();
                    auto position = static_cast<std::iterator_traits<std::string::const_iterator>::difference_type>(stream.tellg());
                    std::size_t lineNumber = std::count(str.begin(), str.begin() + position, '\n') + 1;
                    throw Exception{"Error while parsing input at line " + to_string(lineNumber) + ". " + error};
                }
                else
                    throw Exception{"Error while parsing input. " + error};
            }
        }

        return root;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DataIO::emit(const std::unique_ptr<Node>& rootNode, std::stringstream& stream)
    {
        for (const auto& pair : rootNode->propertyValuePairs)
            stream << pair.first << " = " << pair.second->value.toAnsiString() << ";" << std::endl;

        if (rootNode->propertyValuePairs.size() > 0 && rootNode->children.size() > 0)
            stream << std::endl;

        std::vector<std::string> output;
        for (std::size_t i = 0; i < rootNode->children.size(); ++i)
        {
            for (const auto& line : convertNodesToLines(rootNode->children[i]))
                output.emplace_back(std::move(line));

            if (i < rootNode->children.size()-1)
                output.emplace_back("");
        }

        for (const auto& line : output)
            stream << line << std::endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/to_string.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        char decToSingleHex(unsigned char c)
        {
            assert(c < 16);

            if (c == 10)
                return 'A';
            else if (c == 11)
                return 'B';
            else if (c == 12)
                return 'C';
            else if (c == 13)
                return 'D';
            else if (c == 14)
                return 'E';
            else if (c == 15)
                return 'F';
            else
                return static_cast<char>(c + '0');
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string decToHex(unsigned char c)
        {
            return {decToSingleHex(c / 16), decToSingleHex(c % 16)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeEmptyObject(ObjectConverter&&)
        {
            throw Exception{"Can't serialize empty object"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeBool(ObjectConverter&& value)
        {
            if (value.getBool())
                return "true";
            else
                return "false";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeFont(ObjectConverter&& value)
        {
            if (value.getFont() && !value.getFont().getId().empty())
                return Serializer::serialize({sf::String{value.getFont().getId()}});
            else
                return "null";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeColor(ObjectConverter&& value)
        {
            Color color = value.getColor();

            // Check if the color can be represented by a string with its name
            for (const auto& pair : Color::colorMap)
            {
                if (color == pair.second)
                    return pair.first;
            }

            // Return the color by its rgb value
            return "#" + decToHex(color.getRed()) + decToHex(color.getGreen()) + decToHex(color.getBlue()) + (color.getAlpha() < 255 ? decToHex(color.getAlpha()) : "");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeString(ObjectConverter&& value)
        {
            std::string result = value.getString();

            auto replace = [&](char from, char to)
                {
                    std::size_t pos = 0;
                    while ((pos = result.find(from, pos)) != std::string::npos)
                    {
                        result[pos] = to;
                        result.insert(pos, 1, '\\');
                        pos += 2;
                    }
                };

            replace('\\', '\\');
            replace('\"', '\"');
            replace('\v', 'v');
            replace('\t', 't');
            replace('\n', 'n');
            replace('\0', '0');

            return "\"" + result + "\"";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeNumber(ObjectConverter&& value)
        {
            return to_string(value.getNumber());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeOutline(ObjectConverter&& value)
        {
            return value.getOutline().toString();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeTexture(ObjectConverter&& value)
        {
            Texture texture = value.getTexture();
            if (texture.getId().isEmpty())
                return "None";

            std::string result = "\"" + texture.getId() + "\"";

            if (texture.getData()->rect != sf::IntRect{})
            {
                result += " Part(" + to_string(texture.getData()->rect.left) + ", " + to_string(texture.getData()->rect.top)
                            + ", " + to_string(texture.getData()->rect.width) + ", " + to_string(texture.getData()->rect.height) + ")";
            }

            if (texture.getMiddleRect() != sf::IntRect{0, 0, static_cast<int>(texture.getData()->texture.getSize().x), static_cast<int>(texture.getData()->texture.getSize().y)})
            {
                result += " Middle(" + to_string(texture.getMiddleRect().left) + ", " + to_string(texture.getMiddleRect().top)
                              + ", " + to_string(texture.getMiddleRect().width) + ", " + to_string(texture.getMiddleRect().height) + ")";
            }

            if (texture.isSmooth())
                result += " Smooth";

            return result;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeTextStyle(ObjectConverter&& value)
        {
            const unsigned int style = value.getTextStyle();

            if (style == sf::Text::Regular)
                return "Regular";

            std::string encodedStyle;
            if (style & sf::Text::Bold)
                encodedStyle += " | Bold";
            if (style & sf::Text::Italic)
                encodedStyle += " | Italic";
            if (style & sf::Text::Underlined)
                encodedStyle += " | Underlined";
            if (style & sf::Text::StrikeThrough)
                encodedStyle += " | StrikeThrough";

            if (!encodedStyle.empty())
                return encodedStyle.substr(3);
            else // Something is wrong with the style parameter
                return "Regular";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string serializeRendererData(ObjectConverter&& value)
        {
            auto node = std::make_unique<DataIO::Node>();
            for (const auto& pair : value.getRenderer()->propertyValuePairs)
            {
                sf::String strValue;
                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString()};
                    node->children.push_back(DataIO::parse(ss));
                    node->children.back()->name = pair.first;
                }
                else
                {
                    strValue = ObjectConverter{pair.second}.getString();
                    node->propertyValuePairs[pair.first] = std::make_unique<DataIO::ValueNode>(strValue);
                }
            }

            std::stringstream ss;
            DataIO::emit(node, ss);
            return ss.str();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Serializer::SerializeFunc> Serializer::m_serializers =
        {
            {ObjectConverter::Type::None, serializeEmptyObject},
            {ObjectConverter::Type::Bool, serializeBool},
            {ObjectConverter::Type::Font, serializeFont},
            {ObjectConverter::Type::Color, serializeColor},
            {ObjectConverter::Type::String, serializeString},
            {ObjectConverter::Type::Number, serializeNumber},
            {ObjectConverter::Type::Outline, serializeOutline},
            {ObjectConverter::Type::Texture, serializeTexture},
            {ObjectConverter::Type::TextStyle, serializeTextStyle},
            {ObjectConverter::Type::RendererData, serializeRendererData}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Serializer::serialize(ObjectConverter&& object)
    {
        return m_serializers[object.getType()](std::move(object));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Serializer::setFunction(ObjectConverter::Type type, const SerializeFunc& serializer)
    {
        m_serializers[type] = serializer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Serializer::SerializeFunc& Serializer::getFunction(ObjectConverter::Type type)
    {
        return m_serializers[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/ThemeLoader.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Global.hpp>

#include <cassert>
#include <sstream>
#include <fstream>
#include <set>

#ifdef SFML_SYSTEM_ANDROID
    #include <SFML/System/NativeActivity.hpp>
    #include <android/asset_manager_jni.h>
    #include <android/asset_manager.h>
    #include <android/native_activity.h>
    #include <android/configuration.h>
#endif

// Ignore warning "C4503: decorated name length exceeded, name was truncated" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4503)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::map<std::string, std::map<sf::String, sf::String>>> DefaultThemeLoader::m_propertiesCache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        // Turns texture and font filenames into paths relative to the theme file
        void injectThemePath(std::set<const DataIO::Node*>& handledSections, const std::unique_ptr<DataIO::Node>& node, const std::string& path)
        {
            for (const auto& pair : node->propertyValuePairs)
            {
                if (((pair.first.size() >= 7) && (toLower(pair.first.substr(0, 7)) == "texture")) || (pair.first == "font"))
                {
                    if (pair.second->value.isEmpty() || (pair.second->value == "null") || (pair.second->value == "nullptr"))
                        continue;

                    // Insert the path into the filename unless the filename is already an absolute path
                    if (pair.second->value[0] != '"')
                    {
                    #ifdef SFML_SYSTEM_WINDOWS
                        if ((pair.second->value[0] != '/') && (pair.second->value[0] != '\\') && ((pair.second->value.getSize() <= 1) || (pair.second->value[1] != ':')))
                    #else
                        if (pair.second->value[0] != '/')
                    #endif
                            pair.second->value = path + pair.second->value;
                    }
                    else // The filename is between quotes
                    {
                        if (pair.second->value.getSize() <= 1)
                            continue;

                    #ifdef SFML_SYSTEM_WINDOWS
                        if ((pair.second->value[1] != '/') && (pair.second->value[1] != '\\') && ((pair.second->value.getSize() <= 2) || (pair.second->value[2] != ':')))
                    #else
                        if (pair.second->value[1] != '/')
                    #endif
                            pair.second->value = '"' + path + pair.second->value.substring(1);
                    }
                }
            }

            for (const auto& child : node->children)
            {
                if (handledSections.find(child.get()) == handledSections.end())
                {
                    handledSections.insert(child.get());
                    injectThemePath(handledSections, child, path);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void resolveReferences(std::map<std::string, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>>& sections, const std::unique_ptr<DataIO::Node>& node)
        {
            for (const auto& pair : node->propertyValuePairs)
            {
                // Check if this property is a reference to another section
                if (!pair.second->value.isEmpty() && (pair.second->value[0] == '&'))
                {
                    std::string name = toLower(Deserializer::deserialize(ObjectConverter::Type::String, pair.second->value.substring(1)).getString());

                    auto sectionsIt = sections.find(name);
                    if (sectionsIt == sections.end())
                        throw Exception{"Undefined reference to '" + name + "' encountered."};

                    // Resolve references recursively
                    resolveReferences(sections, sectionsIt->second);

                    // Make a copy of the section
                    std::stringstream ss;
                    DataIO::emit(sectionsIt->second, ss);
                    pair.second->value = "{\n" + ss.str() + "}";
                }
            }

            for (const auto& child : node->children)
                resolveReferences(sections, child);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseThemeLoader::preload(const std::string&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::flushCache(const std::string& filename)
    {
        if (filename != "")
        {
            auto propertiesCacheIt = m_propertiesCache.find(filename);
            if (propertiesCacheIt != m_propertiesCache.end())
                m_propertiesCache.erase(propertiesCacheIt);
        }
        else
        {
            m_propertiesCache.clear();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::preload(const std::string& filename)
    {
        if (filename == "")
            return;

        // Load the file when not already in cache
        if (m_propertiesCache.find(filename) == m_propertiesCache.end())
        {
            std::string resourcePath;
            auto slashPos = filename.find_last_of("/\\");
            if (slashPos != std::string::npos)
                resourcePath = filename.substr(0, slashPos+1);

            std::stringstream fileContents;
            readFile(filename, fileContents);

            std::unique_ptr<DataIO::Node> root = DataIO::parse(fileContents);

            if (root->propertyValuePairs.size() != 0)
                throw Exception{"Unexpected result while loading theme file '" + filename + "'. Root property-value pair found."};

            // Turn texture and font filenames into paths relative to the theme file
            if (!resourcePath.empty())
            {
                std::set<const DataIO::Node*> handledSections;
                injectThemePath(handledSections, root, resourcePath);
            }

            // Get a list of section names and map them to their nodes (needed for resolving references)
            std::map<std::string, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>> sections;
            for (const auto& child : root->children)
            {
                std::string name = toLower(Deserializer::deserialize(ObjectConverter::Type::String, child->name).getString());
                sections.emplace(name, std::cref(child));
            }

            // Resolve references to sections
            resolveReferences(sections, root);

            // Cache all propery value pairs
            for (const auto& section : sections)
            {
                const auto& child = section.second;
                const std::string& name = section.first;
                for (const auto& pair : child.get()->propertyValuePairs)
                    m_propertiesCache[filename][name][toLower(pair.first)] = pair.second->value;

                for (const auto& nestedProperty : child.get()->children)
                {
                    std::stringstream ss;
                    DataIO::emit(nestedProperty, ss);
                    m_propertiesCache[filename][name][toLower(nestedProperty->name)] = "{\n" + ss.str() + "}";
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<sf::String, sf::String>& DefaultThemeLoader::load(const std::string& filename, const std::string& section)
    {
        preload(filename);

        const std::string lowercaseClassName = toLower(section);

        // An empty filename is not considered an error and will result in an empty property list
        if (filename.empty())
            return m_propertiesCache[""][lowercaseClassName];

        if (m_propertiesCache[filename].find(lowercaseClassName) == m_propertiesCache[filename].end())
            throw Exception{"No section '" + section + "' was found in file '" + filename + "'."};

        return m_propertiesCache[filename][lowercaseClassName];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool DefaultThemeLoader::canLoad(const std::string& filename, const std::string& section)
    {
        if (filename.empty())
            return true;
        else
            return m_propertiesCache[filename].find(toLower(section)) != m_propertiesCache[filename].end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::readFile(const std::string& filename, std::stringstream& contents) const
    {
        if (filename.empty())
            return;

        std::string fullFilename;
    #ifdef SFML_SYSTEM_WINDOWS
        if ((filename[0] != '/') && (filename[0] != '\\') && ((filename.size() <= 1) || (filename[1] != ':')))
    #else
        if (filename[0] != '/')
    #endif
            fullFilename = getResourcePath() + filename;
        else
            fullFilename = filename;

    #ifdef SFML_SYSTEM_ANDROID
        // If the file does not start with a slash then load it from the assets
        if (!fullFilename.empty() && (fullFilename[0] != '/'))
        {
            ANativeActivity* activity = sf::getNativeActivity();

            JNIEnv* env = 0;
            activity->vm->AttachCurrentThread(&env, NULL);
            jclass clazz = env->GetObjectClass(activity->clazz);

            jmethodID methodID = env->GetMethodID(clazz, "getAssets", "()Landroid/content/res/AssetManager;");
            jobject assetManagerObject = env->CallObjectMethod(activity->clazz, methodID);
            jobject globalAssetManagerRef = env->NewGlobalRef(assetManagerObject);
            AAssetManager* assetManager = AAssetManager_fromJava(env, globalAssetManagerRef);
            assert(assetManager);

            AAsset* asset = AAssetManager_open(assetManager, fullFilename.c_str(), AASSET_MODE_UNKNOWN);
            if (!asset)
                throw Exception{ "Failed to open theme file '" + fullFilename + "' from assets." };

            off_t assetLength = AAsset_getLength(asset);

            char* buffer = new char[assetLength + 1];
            AAsset_read(asset, buffer, assetLength);
            buffer[assetLength] = 0;

            contents << buffer;

            AAsset_close(asset);
            delete[] buffer;

            activity->vm->DetachCurrentThread();
        }
        else
    #endif
        {
            std::ifstream file{fullFilename};
            if (!file.is_open())
                throw Exception{"Failed to open theme file '" + fullFilename + "'."};

            contents << file.rdbuf();
            file.close();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/BoxLayoutRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRenderer::setSpaceBetweenWidgets(float distance)
    {
        setProperty("spacebetweenwidgets", ObjectConverter{distance});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRenderer::getSpaceBetweenWidgets() const
    {
        auto it = m_data->propertyValuePairs.find("spacebetweenwidgets");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getNumber();
        else
        {
            it = m_data->propertyValuePairs.find("padding");
            if (it != m_data->propertyValuePairs.end())
            {
                const Padding padding = it->second.getOutline();
                return std::max(std::min(padding.getLeft(), padding.getRight()), std::min(padding.getTop(), padding.getBottom()));
            }
            else
                return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/ChatBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ChatBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ChatBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ChatBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ChatBoxRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ChatBoxRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_RENDERER(ChatBoxRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(ChatBoxRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/ComboBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrow)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowHover)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ComboBoxRenderer, TextStyle, sf::Text::Regular)

    TGUI_RENDERER_PROPERTY_RENDERER(ComboBoxRenderer, ListBox, "listbox")
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/GroupRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(GroupRenderer, Padding)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/LabelRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(LabelRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(LabelRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BackgroundColor, Color::Transparent)
    TGUI_RENDERER_PROPERTY_COLOR(LabelRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(LabelRenderer, TextStyle, sf::Text::Regular)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/MenuBarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(MenuBarRenderer, TextColorDisabled, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureItemBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(MenuBarRenderer, TextureSelectedItemBackground)

    TGUI_RENDERER_PROPERTY_NUMBER(MenuBarRenderer, DistanceToSide, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/PanelRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(PanelRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(PanelRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(PanelRenderer, BackgroundColor, Color::White)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/ProgressBarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ProgressBarRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, TextColorFilled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, FillColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(ProgressBarRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ProgressBarRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ProgressBarRenderer, TextureFill)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ProgressBarRenderer, TextStyle, sf::Text::Regular)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/RangeSliderRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(RangeSliderRenderer, SelectedTrackColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(RangeSliderRenderer, SelectedTrackColorHover, {})
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/ScrollbarRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, TrackColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, TrackColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ThumbColor, Color(128, 128, 128))
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ThumbColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowBackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ScrollbarRenderer, ArrowColorHover, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureTrack)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureTrackHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureThumb)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureThumbHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowUp)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowUpHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ScrollbarRenderer, TextureArrowDownHover)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/SpinButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(SpinButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_NUMBER(SpinButtonRenderer, BorderBetweenArrows, 0)

    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, ArrowColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SpinButtonRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowUp)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowUpHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(SpinButtonRenderer, TextureArrowDownHover)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/TextBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TextBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(TextBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TextBoxRenderer, CaretColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(TextBoxRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_NUMBER(TextBoxRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_RENDERER(TextBoxRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(TextBoxRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_BOOL(WidgetRenderer, TransparentTexture, false)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_GET_NUMBER(WidgetRenderer, Opacity, 1)

    void WidgetRenderer::setOpacity(float opacity)
    {
        if (opacity < 0)
            opacity = 0;
        else if (opacity > 1)
            opacity = 1;

        setProperty("opacity", ObjectConverter{opacity});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setFont(Font font)
    {
        setProperty("font", font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font WidgetRenderer::getFont() const
    {
        auto it = m_data->propertyValuePairs.find("font");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getFont();
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setProperty(const std::string& property, ObjectConverter&& value)
    {
        std::string lowercaseProperty = toLower(property);

        if (m_data->propertyValuePairs[lowercaseProperty] != value)
        {
            m_data->propertyValuePairs[lowercaseProperty] = value;

            for (const auto& observer : m_data->observers)
                observer.second(lowercaseProperty);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter WidgetRenderer::getProperty(const std::string& property) const
    {
        auto it = m_data->propertyValuePairs.find(toLower(property));
        if (it != m_data->propertyValuePairs.end())
            return it->second;
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<std::string, ObjectConverter>& WidgetRenderer::getPropertyValuePairs() const
    {
        return m_data->propertyValuePairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::subscribe(const void* id, const std::function<void(const std::string& property)>& function)
    {
        m_data->observers[id] = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::unsubscribe(const void* id)
    {
        m_data->observers.erase(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetRenderer::setData(const std::shared_ptr<RendererData>& data)
    {
        m_data = data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::getData() const
    {
        return m_data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> WidgetRenderer::clone() const
    {
        auto data = std::make_shared<RendererData>(*m_data);
        data->observers = {};
        return data;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/BoxLayout.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayout::BoxLayout(const Layout2d& size) :
        Group{size}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRenderer* BoxLayout::getSharedRenderer()
    {
        return aurora::downcast<BoxLayoutRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const BoxLayoutRenderer* BoxLayout::getSharedRenderer() const
    {
        return aurora::downcast<const BoxLayoutRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRenderer* BoxLayout::getRenderer()
    {
        return aurora::downcast<BoxLayoutRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const BoxLayoutRenderer* BoxLayout::getRenderer() const
    {
        return aurora::downcast<const BoxLayoutRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::insert(std::size_t index, const Widget::Ptr& widget, const sf::String& widgetName)
    {
        // Move the widget to the right position
        if (index < m_widgets.size())
        {
            Group::add(widget, widgetName);

            m_widgets.pop_back();
            m_widgetNames.pop_back();

            m_widgets.insert(m_widgets.begin() + index, widget);
            m_widgetNames.insert(m_widgetNames.begin() + index, widgetName);
        }
        else // Just add the widget to the back
            Group::add(widget, widgetName);

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(const Widget::Ptr& widget)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayout::remove(std::size_t index)
    {
        if (index >= m_widgets.size())
            return false;

        Group::remove(m_widgets[index]);

        updateWidgets();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BoxLayout::get(std::size_t index) const
    {
        if (index < m_widgets.size())
            return m_widgets[index];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayout::rendererChanged(const std::string& property)
    {
        if (property == "spacebetweenwidgets")
        {
            m_spaceBetweenWidgetsCached = getSharedRenderer()->getSpaceBetweenWidgets();
            updateWidgets();
        }
        else if (property == "padding")
        {
            Group::rendererChanged(property);

            // Update the space between widgets as the padding is used when no space was explicitly set
            m_spaceBetweenWidgetsCached = getSharedRenderer()->getSpaceBetweenWidgets();
            updateWidgets();
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button()
    {
        m_type = "Button";
        m_text.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::create(const sf::String& text)
    {
        auto button = std::make_shared<Button>();

        if (!text.isEmpty())
            button->setText(text);

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Ptr Button::copy(Button::ConstPtr button)
    {
        if (button)
            return std::static_pointer_cast<Button>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* Button::getSharedRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ButtonRenderer* Button::getSharedRenderer() const
    {
        return aurora::downcast<const ButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ButtonRenderer* Button::getRenderer()
    {
        return aurora::downcast<ButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ButtonRenderer* Button::getRenderer() const
    {
        return aurora::downcast<const ButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_autoSize = false;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        m_string = text;
        m_text.setString(text);

        // Set the text size when the text has a fixed size
        if (m_textSize != 0)
        {
            m_text.setCharacterSize(m_textSize);
            updateSize();
        }

        if ((getInnerSize().x < 0) || (getInnerSize().y < 0))
            return;

        // Draw the text normally unless the height is more than double of the width
        if (getInnerSize().y <= getInnerSize().x * 2)
        {
            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                const unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too width
                if (m_text.getSize().x > (getInnerSize().x * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_text.getSize().x)));
            }
        }
        else // Place the text vertically
        {
            // The text is vertical
            if (!m_string.isEmpty())
            {
                m_text.setString(m_string[0]);

                for (unsigned int i = 1; i < m_string.getSize(); ++i)
                    m_text.setString(m_text.getString() + "\n" + m_string[i]);
            }

            // Auto size the text when necessary
            if (m_textSize == 0)
            {
                unsigned int textSize = Text::findBestTextSize(m_fontCached, getInnerSize().x * 0.8f);
                m_text.setCharacterSize(textSize);

                // Make the text smaller when it's too high
                if (m_text.getSize().y > (getInnerSize().y * 0.85f))
                    m_text.setCharacterSize(static_cast<unsigned int>(textSize * getInnerSize().y * 0.85f / m_text.getSize().y));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Button::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;

            // Call setText to reposition the text
            setText(getText());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_sprite.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown)
            onPress.emit(this, m_text.getString());

        ClickableWidget::leftMouseReleased(pos);

        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            onPress.emit(this, m_text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        updateTextColorAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Button::getSignal(std::string signalName)
    {
        if (signalName == toLower(onPress.getName()))
            return onPress;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordown") || (property == "textcolordisabled") || (property == "textcolorfocused")
              || (property == "textstyle") || (property == "textstylehover") || (property == "textstyledown") || (property == "textstyledisabled") || (property == "textstylefocused"))
        {
            updateTextColorAndStyle();
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getSharedRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getSharedRenderer()->getTextureHover());
        }
        else if (property == "texturedown")
        {
            m_spriteDown.setTexture(getSharedRenderer()->getTextureDown());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getSharedRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getSharedRenderer()->getTextureFocused());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordown")
        {
            m_borderColorDownCached = getSharedRenderer()->getBorderColorDown();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == "bordercolorfocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordown")
        {
            m_backgroundColorDownCached = getSharedRenderer()->getBackgroundColorDown();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorfocused")
        {
            m_backgroundColorFocusedCached = getSharedRenderer()->getBackgroundColorFocused();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDown.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Button::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!m_string.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_textSize > 0)
            node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Button::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Button::getCurrentBackgroundColor() const
    {
        if (!m_enabled && m_backgroundColorDisabledCached.isSet())
            return m_backgroundColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_backgroundColorDownCached.isSet())
            return m_backgroundColorDownCached;
        else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
            return m_backgroundColorHoverCached;
        else if (m_focused && m_backgroundColorFocusedCached.isSet())
            return m_backgroundColorFocusedCached;
        else
            return m_backgroundColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& Button::getCurrentBorderColor() const
    {
        if (!m_enabled && m_borderColorDisabledCached.isSet())
            return m_borderColorDisabledCached;
        else if (m_mouseHover && m_mouseDown && m_borderColorDownCached.isSet())
            return m_borderColorDownCached;
        else if (m_mouseHover && m_borderColorHoverCached.isSet())
            return m_borderColorHoverCached;
        else if (m_focused && m_borderColorFocusedCached.isSet())
            return m_borderColorFocusedCached;
        else
            return m_borderColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateTextColorAndStyle()
    {
        if (!m_enabled && getSharedRenderer()->getTextStyleDisabled().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleDisabled());
        else if (m_mouseHover && m_mouseDown && getSharedRenderer()->getTextStyleDown().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleDown());
        else if (m_mouseHover && getSharedRenderer()->getTextStyleHover().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleHover());
        else if (m_focused && getSharedRenderer()->getTextStyleFocused().isSet())
            m_text.setStyle(getSharedRenderer()->getTextStyleFocused());
        else
            m_text.setStyle(getSharedRenderer()->getTextStyle());

        if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorDisabled());
        else if (m_mouseHover && m_mouseDown && getSharedRenderer()->getTextColorDown().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorDown());
        else if (m_mouseHover && getSharedRenderer()->getTextColorHover().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorHover());
        else if (m_focused && getSharedRenderer()->getTextColorFocused().isSet())
            m_text.setColor(getSharedRenderer()->getTextColorFocused());
        else
            m_text.setColor(getSharedRenderer()->getTextColor());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::updateSize()
    {
        if (m_autoSize)
        {
            const float spaceAroundText = m_text.getSize().y;
            Widget::setSize({m_text.getSize().x + spaceAroundText + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                             m_text.getSize().y * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }

        m_bordersCached.updateParentSize(getSize());

        // Reset the texture sizes
        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDown.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Recalculate the text size when auto sizing
        if (m_textSize == 0)
            setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), getCurrentBorderColor());
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Check if there is a background texture
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else if (m_mouseHover && m_mouseDown && m_spriteDown.isSet())
                m_spriteDown.draw(target, states);
            else if (m_mouseHover && m_spriteHover.isSet())
                m_spriteHover.draw(target, states);
            else if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
            else
                m_sprite.draw(target, states);
        }
        else // There is no background texture
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());
        }

        // If the button has a text then also draw the text
        states.transform.translate({(getInnerSize().x - m_text.getSize().x) / 2.f, (getInnerSize().y - m_text.getSize().y) / 2.f});
        m_text.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox()
    {
        m_type = "ChatBox";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ChatBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_fontCached, m_textSize) * 18,
                 Text::getLineHeight(m_fontCached, m_textSize) * 8
                 + Text::getExtraVerticalPadding(m_textSize)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom()
                 + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::create()
    {
        return std::make_shared<ChatBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::Ptr ChatBox::copy(ChatBox::ConstPtr chatBox)
    {
        if (chatBox)
            return std::static_pointer_cast<ChatBox>(chatBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBoxRenderer* ChatBox::getSharedRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getSharedRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBoxRenderer* ChatBox::getRenderer()
    {
        return aurora::downcast<ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChatBoxRenderer* ChatBox::getRenderer() const
    {
        return aurora::downcast<const ChatBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getInnerSize());

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
        m_scroll->setSize({m_scroll->getSize().x, getInnerSize().y});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text)
    {
        addLine(text, m_textColor, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, Color color)
    {
        addLine(text, color, m_textStyle);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, Color color, TextStyle style)
    {
        // Remove the oldest line if you exceed the maximum
        if ((m_maxLines > 0) && (m_maxLines == m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                removeLine(0);
            else
                removeLine(m_maxLines-1);
        }

        Line line;
        line.string = text;
        line.text.setColor(color);
        line.text.setStyle(style);
        line.text.setOpacity(m_opacityCached);
        line.text.setCharacterSize(m_textSize);
        line.text.setString(text);
        line.text.setFont(m_fontCached);

        recalculateLineText(line);

        if (m_newLinesBelowOthers)
            m_lines.push_back(std::move(line));
        else
            m_lines.push_front(std::move(line));

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ChatBox::getLine(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].string;
        }
        else // Index too high
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color ChatBox::getLineColor(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
        {
            return m_lines[lineIndex].text.getColor();
        }
        else // Index too high
            return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle ChatBox::getLineTextStyle(std::size_t lineIndex) const
    {
        if (lineIndex < m_lines.size())
            return m_lines[lineIndex].text.getStyle();
        else // Index too high
            return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::removeLine(std::size_t lineIndex)
    {
        if (lineIndex < m_lines.size())
        {
            m_lines.erase(m_lines.begin() + lineIndex);

            recalculateFullTextHeight();
            return true;
        }
        else // Index too high
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeAllLines()
    {
        m_lines.clear();

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineAmount()
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLineLimit(std::size_t maxLines)
    {
        m_maxLines = maxLines;

        // Remove the oldest lines if there are too many lines
        if ((m_maxLines > 0) && (m_maxLines < m_lines.size()))
        {
            if (m_newLinesBelowOthers)
                m_lines.erase(m_lines.begin(), m_lines.begin() + m_lines.size() - m_maxLines);
            else
                m_lines.erase(m_lines.begin() + m_maxLines, m_lines.end());

            recalculateFullTextHeight();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ChatBox::getLineLimit()
    {
        return m_maxLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        m_textSize = size;
        m_scroll->setScrollAmount(size);

        for (auto& line : m_lines)
            line.text.setCharacterSize(size);

        recalculateAllLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextColor(Color color)
    {
        m_textColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ChatBox::getTextColor() const
    {
        return m_textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextStyle(TextStyle style)
    {
        m_textStyle = style;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle ChatBox::getTextStyle() const
    {
        return m_textStyle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setLinesStartFromTop(bool startFromTop)
    {
        if (m_linesStartFromTop != startFromTop)
            m_linesStartFromTop = startFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getLinesStartFromTop() const
    {
        return m_linesStartFromTop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setNewLinesBelowOthers(bool newLinesBelowOthers)
    {
        m_newLinesBelowOthers = newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getNewLinesBelowOthers() const
    {
        return m_newLinesBelowOthers;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(Vector2f pos)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // Pass the event to the scrollbar
        if (m_scroll->mouseOnWidget(pos - getPosition()))
            m_scroll->leftMousePressed(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(Vector2f pos)
    {
        if (m_scroll->isMouseDown())
            m_scroll->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Pass the event to the scrollbar when the mouse is on top of it or when we are dragging its thumb
        if (((m_scroll->isMouseDown()) && (m_scroll->isMouseDownOnThumb())) || m_scroll->mouseOnWidget(pos - getPosition()))
            m_scroll->mouseMoved(pos - getPosition());
        else
            m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_scroll->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_scroll->getViewportSize() < m_scroll->getMaximum())
        {
            m_scroll->mouseWheelScrolled(delta, pos - getPosition());
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateLineText(Line& line)
    {
        line.text.setString("");

        // Find the maximum width of one line
        const float maxWidth = getInnerSize().x - m_scroll->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (maxWidth < 0)
            return;

        line.text.setString(Text::wordWrap(maxWidth, line.string, m_fontCached, line.text.getCharacterSize(), false));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateAllLines()
    {
        for (auto& line : m_lines)
            recalculateLineText(line);

        recalculateFullTextHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::recalculateFullTextHeight()
    {
        m_fullTextHeight = 0;
        for (const auto& line : m_lines)
            m_fullTextHeight += line.text.getSize().y;

        // Update the maximum of the scrollbar
        const unsigned int oldMaximum = m_scroll->getMaximum();
        m_scroll->setMaximum(static_cast<unsigned int>(m_fullTextHeight + Text::getExtraVerticalPadding(m_textSize)));

        // Scroll down to the last item when there is a scrollbar and it is at the bottom
        if (m_newLinesBelowOthers)
        {
            if (((oldMaximum >= m_scroll->getViewportSize()) && (m_scroll->getValue() == oldMaximum - m_scroll->getViewportSize()))
             || ((oldMaximum <= m_scroll->getViewportSize()) && (m_scroll->getMaximum() > m_scroll->getViewportSize())))
            {
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scroll->setInheritedOpacity(m_opacityCached);

            for (auto& line : m_lines)
                line.text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& line : m_lines)
                line.text.setFont(m_fontCached);

            recalculateAllLines();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChatBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["TextColor"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textColor));

        if (m_textStyle != sf::Text::Style::Regular)
            node->propertyValuePairs["TextStyle"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_textStyle));

        if (m_maxLines > 0)
            node->propertyValuePairs["LineLimit"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxLines));

        if (m_linesStartFromTop)
            node->propertyValuePairs["LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["LinesStartFromTop"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_newLinesBelowOthers)
            node->propertyValuePairs["NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("true");
        else
            node->propertyValuePairs["NewLinesBelowOthers"] = std::make_unique<DataIO::ValueNode>("false");

        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            auto lineNode = std::make_unique<DataIO::Node>();
            lineNode->name = "Line";

            lineNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getLine(i)));

            const Color lineTextColor = getLineColor(i);
            if (lineTextColor != m_textColor)
                lineNode->propertyValuePairs["Color"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

            const TextStyle lineTextStyle = getLineTextStyle(i);
            if (lineTextStyle != m_textStyle)
                lineNode->propertyValuePairs["Style"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextStyle));

            node->children.push_back(std::move(lineNode));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["textcolor"])
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, node->propertyValuePairs["textcolor"]->value).getColor());
        if (node->propertyValuePairs["textstyle"])
            setTextStyle(Deserializer::deserialize(ObjectConverter::Type::TextStyle, node->propertyValuePairs["textstyle"]->value).getTextStyle());
        if (node->propertyValuePairs["linelimit"])
            setLineLimit(tgui::stoi(node->propertyValuePairs["linelimit"]->value));

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "line")
            {
                Color lineTextColor = getTextColor();
                if (childNode->propertyValuePairs["color"])
                    lineTextColor = Deserializer::deserialize(ObjectConverter::Type::Color, childNode->propertyValuePairs["color"]->value).getColor();

                TextStyle lineTextStyle = getTextStyle();
                if (childNode->propertyValuePairs["style"])
                    lineTextStyle = Deserializer::deserialize(ObjectConverter::Type::TextStyle, childNode->propertyValuePairs["style"]->value).getTextStyle();

                if (childNode->propertyValuePairs["text"])
                    addLine(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString(), lineTextColor, lineTextStyle);
            }
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
                                        [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "line"; }), node->children.end());

        if (node->propertyValuePairs["linesstartfromtop"])
            setLinesStartFromTop(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["linesstartfromtop"]->value).getBool());

        // This has to be parsed after the lines have been added
        if (node->propertyValuePairs["newlinesbelowothers"])
            setNewLinesBelowOthers(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["newlinesbelowothers"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChatBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const sf::RenderStates scrollbarStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the scrollbar
        m_scroll->draw(target, scrollbarStates);

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        const Clipping clipping{target, states, {}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - m_scroll->getSize().x,
                                                     getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

        states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSize), -static_cast<float>(m_scroll->getValue())});

        // Put the lines at the bottom of the chat box if needed
        if (!m_linesStartFromTop && (m_fullTextHeight + Text::getExtraVerticalPadding(m_textSize) < getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()))
            states.transform.translate(0, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_fullTextHeight - Text::getExtraVerticalPadding(m_textSize));

        for (const auto& line : m_lines)
        {
            line.text.draw(target, states);
            states.transform.translate(0, line.text.getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        float clamp(float value, float lower, float upper)
        {
            if (value < lower)
                return lower;

            if (value > upper)
                return upper;

            return value;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow(const sf::String& title, unsigned int titleButtons)
    {
        m_type = "ChildWindow";
        m_isolatedFocus = true;
        m_titleText.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ChildWindowRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitle(title);
        setTitleTextSize(getGlobalTextSize());
        m_titleBarHeightCached = m_titleText.getSize().y * 1.25f;
        setTitleButtons(titleButtons);
        setSize({400, 300});

        m_maximizeButton->connect("pressed", [this](){ onMaximize.emit(this); });
        m_minimizeButton->connect("pressed", [this](){ onMinimize.emit(this); });
        m_closeButton->connect("pressed", [this](){
                if (!onClose.emit(this))
                    destroy();
            });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create(const sf::String& title, unsigned int titleButtons)
    {
        return std::make_shared<ChildWindow>(title, titleButtons);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::copy(ChildWindow::ConstPtr childWindow)
    {
        if (childWindow)
            return std::static_pointer_cast<ChildWindow>(childWindow->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindowRenderer* ChildWindow::getSharedRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getSharedRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindowRenderer* ChildWindow::getRenderer()
    {
        return aurora::downcast<ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ChildWindowRenderer* ChildWindow::getRenderer() const
    {
        return aurora::downcast<const ChildWindowRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(const Layout2d& position)
    {
        float x = position.getValue().x;
        float y = position.getValue().y;

        if (m_keepInParent && m_parent && (m_parent->getSize().x > 0) && (m_parent->getSize().y > 0)
         && ((y < 0) || (y > m_parent->getSize().y - getFullSize().y) || (x < 0) || (x > m_parent->getSize().x - getFullSize().x)))
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - getFullSize().y)
                y = m_parent->getSize().y - getFullSize().y;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getFullSize().x)
                x = m_parent->getSize().x - getFullSize().x;

            Container::setPosition({x, y});
        }
        else
            Container::setPosition(position);

        // Calculate the distance from the right side that the buttons will need
        float buttonOffsetX = 0;
        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
        }

        if (buttonOffsetX > 0)
            buttonOffsetX += m_distanceToSideCached;

        if (m_titleAlignment == TitleAlignment::Left)
        {
            m_titleText.setPosition(m_distanceToSideCached,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else if (m_titleAlignment == TitleAlignment::Center)
        {
            m_titleText.setPosition(m_distanceToSideCached + ((getSize().x - (2 * m_distanceToSideCached) - buttonOffsetX - m_titleText.getSize().x) / 2.0f),
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }
        else // if (m_titleAlignment == TitleAlignment::Right)
        {
            m_titleText.setPosition(getSize().x - m_distanceToSideCached - buttonOffsetX - m_titleText.getSize().x,
                                    (m_titleBarHeightCached - m_titleText.getSize().y) / 2.0f);
        }

        buttonOffsetX = m_distanceToSideCached;
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                button->setPosition(m_bordersCached.getLeft() + getSize().x - buttonOffsetX - button->getSize().x,
                                    m_bordersCached.getTop() + (m_titleBarHeightCached - button->getSize().y) / 2.f);

                buttonOffsetX += button->getSize().x + m_paddingBetweenButtonsCached;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                getSize().y + m_bordersCached.getTop() + m_bordersCached.getBottom() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMaximumSize(Vector2f size)
    {
        if ((size.x < getSize().x) || (size.y < getSize().y))
        {
            // The window is currently larger than the new maximum size, lets downsize
            setSize(std::min(size.x, getSize().x), std::min(size.y, getSize().y));
        }

        m_maximumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMaximumSize() const
    {
        return m_maximumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setMinimumSize(Vector2f size)
    {
        if ((size.x > getSize().x) || (size.y > getSize().y))
        {
            // The window is currently smaller than the new minimum size, lets upsize
            setSize(std::max(size.x, getSize().x), std::max(size.y, getSize().y));
        }

        m_minimumSize = size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getMinimumSize() const
    {
        return m_minimumSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setString(title);

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ChildWindow::getTitle() const
    {
        return m_titleText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleTextSize(unsigned int size)
    {
        m_titleTextSize = size;

        if (m_titleTextSize)
            m_titleText.setCharacterSize(m_titleTextSize);
        else
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleTextSize() const
    {
        return m_titleTextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::TitleAlignment ChildWindow::getTitleAlignment() const
    {
        return m_titleAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleButtons(unsigned int buttons)
    {
        m_titleButtons = buttons;

        if (m_titleButtons & TitleButton::Close)
        {
            m_closeButton->setVisible(true);
            m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
            m_closeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_closeButton->setText(L"\u2715");
            else
                m_closeButton->setText("");
        }
        else
            m_closeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Maximize)
        {
            m_maximizeButton->setVisible(true);
            m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
            m_maximizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_maximizeButton->setText(L"\u2610");
            else
                m_maximizeButton->setText("");
        }
        else
            m_maximizeButton->setVisible(false);

        if (m_titleButtons & TitleButton::Minimize)
        {
            m_minimizeButton->setVisible(true);
            m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
            m_minimizeButton->setInheritedOpacity(m_opacityCached);

            if (m_showTextOnTitleButtonsCached)
                m_minimizeButton->setText(L"\u2043");
            else
                m_minimizeButton->setText("");
        }
        else
            m_minimizeButton->setVisible(false);

        updateTitleBarHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChildWindow::getTitleButtons() const
    {
        return m_titleButtons;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        if (m_parent)
            m_parent->remove(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setResizable(bool resizable)
    {
        m_resizable = resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isResizable() const
    {
        return m_resizable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPositionLocked(bool positionLocked)
    {
        m_positionLocked = positionLocked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isPositionLocked() const
    {
        return m_positionLocked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setKeepInParent(bool enabled)
    {
        m_keepInParent = enabled;

        if (enabled)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ChildWindow::getChildWidgetsOffset() const
    {
        return {m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached + m_borderBelowTitleBarCached};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setParent(Container* parent)
    {
        Container::setParent(parent);
        if (m_keepInParent)
            setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(Vector2f pos) const
    {
        FloatRect region{getPosition(), getFullSize()};

        // Expand the region if the child window is resizable (to make the borders easier to click on)
        if (m_resizable)
        {
            region.left -= std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getLeft());
            region.top -= std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getTop());
            region.width += std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getLeft())
                            + std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getRight());
            region.height += std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getTop())
                             + std::max(0.f, m_MinimumResizableBorderWidthCached - m_bordersCached.getBottom());
        }

        if (region.contains(pos))
        {
            // If the mouse enters the border or title bar then then none of the widgets can still be under the mouse
            if (m_widgetBelowMouse && !FloatRect{getPosition() + getChildWidgetsOffset(), getSize()}.contains(pos))
                m_widgetBelowMouse->mouseNoLongerOnWidget();

            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        // Move the child window to the front
        moveToFront();

        onMousePress.emit(this);

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMousePressed(pos + getPosition());
        }
        else if (!FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, getSize().y + m_titleBarHeightCached + m_borderBelowTitleBarCached}.contains(pos))
        {
            // Mouse is on top of the borders
            if (m_resizable)
            {
                // Check on which border the mouse is standing
                m_resizeDirection = ResizeNone;
                if (pos.x < m_bordersCached.getLeft())
                    m_resizeDirection |= ResizeLeft;
                if (pos.y < m_bordersCached.getTop())
                    m_resizeDirection |= ResizeTop;
                if (pos.x >= getFullSize().x - m_bordersCached.getRight())
                    m_resizeDirection |= ResizeRight;
                if (pos.y >= getFullSize().y - m_bordersCached.getBottom())
                    m_resizeDirection |= ResizeBottom;
            }

            m_draggingPosition = pos;
        }
        else if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
        {
            // Send the mouse press event to the title buttons
            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible() && button->mouseOnWidget(pos))
                {
                    button->leftMousePressed(pos);
                    return;
                }
            }

            // The mouse went down on the title bar
            if (!m_positionLocked)
            {
                m_mouseDownOnTitleBar = true;
                m_draggingPosition = pos;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDownOnTitleBar = false;
        m_resizeDirection = ResizeNone;

        if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
        {
            // Propagate the event to the child widgets
            Container::leftMouseReleased(pos + getPosition());
        }
        else
        {
            // Tell the widgets that the mouse was released
            for (auto& widget : m_widgets)
                widget->mouseNoLongerDown();

            // Check if the mouse is on top of the title bar
            if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
            {
                // Send the mouse release event to the title buttons
                for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                {
                    if (button->isVisible() && button->mouseOnWidget(pos))
                    {
                        button->leftMouseReleased(pos);
                        break;
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window, but don't allow the dragging position to leave the screen
            sf::Vector2f newPosition;
            if (getPosition().x + pos.x <= 0)
                newPosition.x = -m_draggingPosition.x + 1;
            else if (m_parent && getPosition().x + pos.x >= m_parent->getSize().x)
                newPosition.x = m_parent->getSize().x - m_draggingPosition.x - 1;
            else
                newPosition.x = getPosition().x + (pos.x - m_draggingPosition.x);

            if (getPosition().y + pos.y <= 0)
                newPosition.y = -m_draggingPosition.y + 1;
            else if (m_parent && getPosition().y + pos.y >= m_parent->getSize().y)
                newPosition.y = m_parent->getSize().y - m_draggingPosition.y - 1;
            else
                newPosition.y = getPosition().y + (pos.y - m_draggingPosition.y);

            setPosition(newPosition);
        }

        // Check if you are resizing the window
        else if (m_mouseDown && m_resizeDirection != ResizeNone)
        {
            float minimumWidth = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    minimumWidth += (minimumWidth > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }
            minimumWidth += 2 * m_distanceToSideCached;

            if ((m_resizeDirection & ResizeLeft) != 0)
            {
                const float diff = clamp(m_draggingPosition.x - pos.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setPosition(getPosition().x - diff, getPosition().y);
                setSize(getSize().x + diff, getSize().y);
            }
            else if ((m_resizeDirection & ResizeRight) != 0)
            {
                const float diff = clamp(pos.x - m_draggingPosition.x, std::max(minimumWidth, m_minimumSize.x) - getSize().x, m_maximumSize.x - getSize().x);
                setSize(getSize().x + diff, getSize().y);
                m_draggingPosition.x += diff;
            }

            if ((m_resizeDirection & ResizeTop) != 0)
            {
                const float diff = clamp(m_draggingPosition.y - pos.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setPosition(getPosition().x, getPosition().y - diff);
                setSize(getSize().x, getSize().y + diff);
            }
            else if ((m_resizeDirection & ResizeBottom) != 0)
            {
                const float diff = clamp(pos.y - m_draggingPosition.y, m_minimumSize.y - getSize().y, m_maximumSize.y - getSize().y);
                setSize(getSize().x, getSize().y + diff);
                m_draggingPosition.y += diff;
            }
        }
        else // Not dragging child window
        {
            if (FloatRect{getChildWidgetsOffset(), getSize()}.contains(pos))
            {
                // Propagate the event to the child widgets
                Container::mouseMoved(pos + getPosition());
            }
            else
            {
                if (!m_mouseHover)
                    mouseEnteredWidget();

                // Check if the mouse is on top of the title bar
                if (FloatRect{m_bordersCached.getLeft(), m_bordersCached.getTop(), getSize().x, m_titleBarHeightCached}.contains(pos))
                {
                    // Send the hover event to the buttons inside the title bar
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                        {
                            if (button->mouseOnWidget(pos))
                                button->mouseMoved(pos);
                            else
                                button->mouseNoLongerOnWidget();
                        }
                    }
                }
                else // When the mouse is not on the title bar, the mouse can't be on the buttons inside it
                {
                    for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
                    {
                        if (button->isVisible())
                            button->mouseNoLongerOnWidget();
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerOnWidget()
    {
        Container::mouseNoLongerOnWidget();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();

        for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->mouseNoLongerDown();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::updateTitleBarHeight()
    {
        m_spriteTitleBar.setSize({getSize().x, m_titleBarHeightCached});

        // Set the size of the buttons in the title bar
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
            {
                if (m_spriteTitleBar.isSet() && (button->getSharedRenderer()->getTexture().getData() != nullptr))
                {
                    button->setSize(button->getSharedRenderer()->getTexture().getImageSize().x * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y),
                                    button->getSharedRenderer()->getTexture().getImageSize().y * (m_titleBarHeightCached / m_spriteTitleBar.getTexture().getImageSize().y));
                }
                else
                    button->setSize({m_titleBarHeightCached * 0.8f, m_titleBarHeightCached * 0.8f});

                const Borders& buttonBorders = button->getSharedRenderer()->getBorders();
                button->setTextSize(Text::findBestTextSize(m_titleText.getFont(), (button->getSize().y - buttonBorders.getTop() - buttonBorders.getBottom()) * 0.8f));
            }
        }

        if (m_titleTextSize == 0)
            m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, m_titleBarHeightCached * 0.8f));

        // Reposition the images and text
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ChildWindow::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onClose.getName()))
            return onClose;
        else if (signalName == toLower(onMinimize.getName()))
            return onMinimize;
        else if (signalName == toLower(onMaximize.getName()))
            return onMaximize;
        else
            return Container::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "titlecolor")
        {
            m_titleText.setColor(getSharedRenderer()->getTitleColor());
        }
        else if (property == "texturetitlebar")
        {
            m_spriteTitleBar.setTexture(getSharedRenderer()->getTextureTitleBar());

            // If the title bar height is determined by the texture then update it (note that getTitleBarHeight has a non-trivial implementation)
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            if (m_titleBarHeightCached == m_spriteTitleBar.getTexture().getImageSize().y)
                updateTitleBarHeight();
        }
        else if (property == "titlebarheight")
        {
            m_titleBarHeightCached = getSharedRenderer()->getTitleBarHeight();
            updateTitleBarHeight();
        }
        else if (property == "borderbelowtitlebar")
        {
            m_borderBelowTitleBarCached = getSharedRenderer()->getBorderBelowTitleBar();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            setPosition(m_position);
        }
        else if (property == "paddingbetweenbuttons")
        {
            m_paddingBetweenButtonsCached = getSharedRenderer()->getPaddingBetweenButtons();
            setPosition(m_position);
        }
        else if (property == "minimumresizableborderwidth")
        {
            m_MinimumResizableBorderWidthCached = getSharedRenderer()->getMinimumResizableBorderWidth();
        }
        else if (property == "showtextontitlebuttons")
        {
            m_showTextOnTitleButtonsCached = getSharedRenderer()->getShowTextOnTitleButtons();
            setTitleButtons(m_titleButtons);
        }
        else if (property == "closebutton")
        {
            if (m_closeButton->isVisible())
            {
                m_closeButton->setRenderer(getSharedRenderer()->getCloseButton());
                m_closeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "maximizebutton")
        {
            if (m_maximizeButton->isVisible())
            {
                m_maximizeButton->setRenderer(getSharedRenderer()->getMaximizeButton());
                m_maximizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "minimizebutton")
        {
            if (m_minimizeButton->isVisible())
            {
                m_minimizeButton->setRenderer(getSharedRenderer()->getMinimizeButton());
                m_minimizeButton->setInheritedOpacity(m_opacityCached);
            }

            updateTitleBarHeight();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "titlebarcolor")
        {
            m_titleBarColorCached = getSharedRenderer()->getTitleBarColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedOpacity(m_opacityCached);
            }

            m_titleText.setOpacity(m_opacityCached);
            m_spriteTitleBar.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Container::rendererChanged(property);

            for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    button->setInheritedFont(m_fontCached);
            }

            m_titleText.setFont(m_fontCached);
            if (m_titleTextSize == 0)
                m_titleText.setCharacterSize(Text::findBestTextSize(m_fontCached, getSharedRenderer()->getTitleBarHeight() * 0.8f));

            setPosition(m_position);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ChildWindow::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        if (m_titleAlignment == ChildWindow::TitleAlignment::Left)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_titleAlignment == ChildWindow::TitleAlignment::Center)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_titleAlignment == ChildWindow::TitleAlignment::Right)
            node->propertyValuePairs["TitleAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (getTitle().getSize() > 0)
            node->propertyValuePairs["Title"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getTitle()));

        if (m_keepInParent)
            node->propertyValuePairs["KeepInParent"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_resizable)
            node->propertyValuePairs["Resizable"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_positionLocked)
            node->propertyValuePairs["PositionLocked"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_minimumSize != Vector2f{})
            node->propertyValuePairs["MinimumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_minimumSize.x) + ", " + to_string(m_minimumSize.y) + ")");
        if (m_maximumSize != Vector2f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
            node->propertyValuePairs["MaximumSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_maximumSize.x) + ", " + to_string(m_maximumSize.y) + ")");

        std::string serializedTitleButtons;
        if (m_titleButtons & ChildWindow::TitleButton::Minimize)
            serializedTitleButtons += " | Minimize";
        if (m_titleButtons & ChildWindow::TitleButton::Maximize)
            serializedTitleButtons += " | Maximize";
        if (m_titleButtons & ChildWindow::TitleButton::Close)
            serializedTitleButtons += " | Close";

        if (!serializedTitleButtons.empty())
            serializedTitleButtons.erase(0, 3);
        else
            serializedTitleButtons = "None";

        node->propertyValuePairs["TitleButtons"] = std::make_unique<DataIO::ValueNode>(serializedTitleButtons);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs["titlealignment"])
        {
            if (toLower(node->propertyValuePairs["titlealignment"]->value) == "left")
                setTitleAlignment(ChildWindow::TitleAlignment::Left);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "center")
                setTitleAlignment(ChildWindow::TitleAlignment::Center);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "right")
                setTitleAlignment(ChildWindow::TitleAlignment::Right);
            else
                throw Exception{"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["titlebuttons"])
        {
            int decodedTitleButtons = ChildWindow::TitleButton::None;
            std::vector<std::string> titleButtons = Deserializer::split(node->propertyValuePairs["titlebuttons"]->value, '|');
            for (const auto& elem : titleButtons)
            {
                std::string requestedTitleButton = toLower(trim(elem));
                if (requestedTitleButton == "close")
                    decodedTitleButtons |= sf::Text::Bold;
                else if (requestedTitleButton == "maximize")
                    decodedTitleButtons |= sf::Text::Italic;
                else if (requestedTitleButton == "minimize")
                    decodedTitleButtons |= sf::Text::Underlined;
            }

            setTitleButtons(decodedTitleButtons);
        }

        if (node->propertyValuePairs["title"])
            setTitle(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["title"]->value).getString());

        if (node->propertyValuePairs["keepinparent"])
            setKeepInParent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["keepinparent"]->value).getBool());

        if (node->propertyValuePairs["resizable"])
            setResizable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["resizable"]->value).getBool());
        if (node->propertyValuePairs["positionlocked"])
            setPositionLocked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["positionlocked"]->value).getBool());

        if (node->propertyValuePairs["minimumsize"])
            setMinimumSize(Vector2f{node->propertyValuePairs["minimumsize"]->value});

        if (node->propertyValuePairs["maximumsize"])
            setMaximumSize(Vector2f{node->propertyValuePairs["maximumsize"]->value});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getFullSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the title bar
        if (m_spriteTitleBar.isSet())
            m_spriteTitleBar.draw(target, states);
        else
            drawRectangleShape(target, states, {getSize().x, m_titleBarHeightCached}, m_titleBarColorCached);

        // Draw the text in the title bar (after setting the clipping area)
        {
            float buttonOffsetX = 0;
            for (const auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
            {
                if (button->isVisible())
                    buttonOffsetX += (buttonOffsetX > 0 ? m_paddingBetweenButtonsCached : 0) + button->getSize().x;
            }

            if (buttonOffsetX > 0)
                buttonOffsetX += m_distanceToSideCached;

            const float clippingLeft = m_distanceToSideCached;
            const float clippingRight = getSize().x - m_distanceToSideCached - buttonOffsetX;

            const Clipping clipping{target, states, {clippingLeft, 0}, {clippingRight - clippingLeft, m_titleBarHeightCached}};

            m_titleText.draw(target, states);
        }

        // Draw the buttons
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});
        for (auto& button : {m_closeButton.get(), m_maximizeButton.get(), m_minimizeButton.get()})
        {
            if (button->isVisible())
                button->draw(target, states);
        }

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop() + m_titleBarHeightCached});

        // Draw the border below the title bar
        if (m_borderBelowTitleBarCached > 0)
        {
            drawRectangleShape(target, states, {getSize().x, m_borderBelowTitleBarCached}, m_borderColorCached);
            states.transform.translate({0, m_borderBelowTitleBarCached});
        }

        // Draw the background
        if (m_backgroundColorCached != Color::Transparent)
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        // Draw the widgets in the child window
        const Clipping clipping{target, states, {}, {getSize()}};
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_type = "ComboBox";
        m_draggableWidget = true;
        m_text.setFont(m_fontCached);

        initListBox();

        m_renderer = aurora::makeCopied<ComboBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_text) * 10,
                 Text::getLineHeight(m_text) * 1.25f + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& other) :
        Widget                           {other},
        onItemSelect                     {other.onItemSelect},
        m_nrOfItemsToDisplay             {other.m_nrOfItemsToDisplay},
        m_listBox                        {ListBox::copy(other.m_listBox)},
        m_text                           {other.m_text},
        m_expandDirection                {other.m_expandDirection},
        m_spriteBackground               {other.m_spriteBackground},
        m_spriteArrow                    {other.m_spriteArrow},
        m_spriteArrowHover               {other.m_spriteArrowHover},
        m_bordersCached                  {other.m_bordersCached},
        m_paddingCached                  {other.m_paddingCached},
        m_borderColorCached              {other.m_borderColorCached},
        m_backgroundColorCached          {other.m_backgroundColorCached},
        m_arrowColorCached               {other.m_arrowColorCached},
        m_arrowColorHoverCached          {other.m_arrowColorHoverCached},
        m_arrowBackgroundColorCached     {other.m_arrowBackgroundColorCached},
        m_arrowBackgroundColorHoverCached{other.m_arrowBackgroundColorHoverCached}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(ComboBox&& other) :
        Widget                           {std::move(other)},
        onItemSelect                     {std::move(other.onItemSelect)},
        m_nrOfItemsToDisplay             {std::move(other.m_nrOfItemsToDisplay)},
        m_listBox                        {std::move(other.m_listBox)},
        m_text                           {std::move(other.m_text)},
        m_expandDirection                {std::move(other.m_expandDirection)},
        m_spriteBackground               {std::move(other.m_spriteBackground)},
        m_spriteArrow                    {std::move(other.m_spriteArrow)},
        m_spriteArrowHover               {std::move(other.m_spriteArrowHover)},
        m_bordersCached                  {std::move(other.m_bordersCached)},
        m_paddingCached                  {std::move(other.m_paddingCached)},
        m_borderColorCached              {std::move(other.m_borderColorCached)},
        m_backgroundColorCached          {std::move(other.m_backgroundColorCached)},
        m_arrowColorCached               {std::move(other.m_arrowColorCached)},
        m_arrowColorHoverCached          {std::move(other.m_arrowColorHoverCached)},
        m_arrowBackgroundColorCached     {std::move(other.m_arrowBackgroundColorCached)},
        m_arrowBackgroundColorHoverCached{std::move(other.m_arrowBackgroundColorHoverCached)}
    {
        initListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& other)
    {
        if (this != &other)
        {
            ComboBox temp{other};
            Widget::operator=(other);

            std::swap(onItemSelect,                      temp.onItemSelect);
            std::swap(m_nrOfItemsToDisplay,              temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,                         temp.m_listBox);
            std::swap(m_text,                            temp.m_text);
            std::swap(m_expandDirection,                 temp.m_expandDirection);
            std::swap(m_spriteBackground,                temp.m_spriteBackground);
            std::swap(m_spriteArrow,                     temp.m_spriteArrow);
            std::swap(m_spriteArrowHover,                temp.m_spriteArrowHover);
            std::swap(m_bordersCached,                   temp.m_bordersCached);
            std::swap(m_paddingCached,                   temp.m_paddingCached);
            std::swap(m_borderColorCached,               temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,           temp.m_backgroundColorCached);
            std::swap(m_arrowColorCached,                temp.m_arrowColorCached);
            std::swap(m_arrowColorHoverCached,           temp.m_arrowColorHoverCached);
            std::swap(m_arrowBackgroundColorCached,      temp.m_arrowBackgroundColorCached);
            std::swap(m_arrowBackgroundColorHoverCached, temp.m_arrowBackgroundColorHoverCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (ComboBox&& other)
    {
        if (this != &other)
        {
            Widget::operator=(std::move(other));
            onItemSelect                      = std::move(other.onItemSelect);
            m_nrOfItemsToDisplay              = std::move(other.m_nrOfItemsToDisplay);
            m_listBox                         = std::move(other.m_listBox);
            m_text                            = std::move(other.m_text);
            m_expandDirection                 = std::move(other.m_expandDirection);
            m_spriteBackground                = std::move(other.m_spriteBackground);
            m_spriteArrow                     = std::move(other.m_spriteArrow);
            m_spriteArrowHover                = std::move(other.m_spriteArrowHover);
            m_bordersCached                   = std::move(other.m_bordersCached);
            m_paddingCached                   = std::move(other.m_paddingCached);
            m_borderColorCached               = std::move(other.m_borderColorCached);
            m_backgroundColorCached           = std::move(other.m_backgroundColorCached);
            m_arrowColorCached                = std::move(other.m_arrowColorCached);
            m_arrowColorHoverCached           = std::move(other.m_arrowColorHoverCached);
            m_arrowBackgroundColorCached      = std::move(other.m_arrowBackgroundColorCached);
            m_arrowBackgroundColorHoverCached = std::move(other.m_arrowBackgroundColorHoverCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create()
    {
        return std::make_shared<ComboBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::copy(ComboBox::ConstPtr comboBox)
    {
        if (comboBox)
            return std::static_pointer_cast<ComboBox>(comboBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getSharedRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ComboBoxRenderer* ComboBox::getSharedRenderer() const
    {
        return aurora::downcast<const ComboBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBoxRenderer* ComboBox::getRenderer()
    {
        return aurora::downcast<ComboBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ComboBoxRenderer* ComboBox::getRenderer() const
    {
        return aurora::downcast<const ComboBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        const float height = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();

        if (height > 0)
        {
            m_listBox->setItemHeight(static_cast<unsigned int>(height));
            updateListBoxHeight();
        }

        if (m_spriteArrow.isSet())
        {
            m_spriteArrow.setSize({m_spriteArrow.getTexture().getImageSize().x * (height / m_spriteArrow.getTexture().getImageSize().y), height});
            m_spriteArrowHover.setSize(m_spriteArrow.getSize());
        }

        m_text.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(std::size_t nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        if (m_nrOfItemsToDisplay < m_listBox->getItemCount())
            updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemCount() const
    {
        return m_listBox->getItemCount();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getItemsToDisplay() const
    {
        return m_nrOfItemsToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        const bool ret = m_listBox->addItem(item, id);
        updateListBoxHeight();
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        const bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        const bool ret = m_listBox->setSelectedItemById(id);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemByIndex(std::size_t index)
    {
        const bool ret = m_listBox->setSelectedItemByIndex(index);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setString("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        // Implemented like removeItemByIndex instead of like removeItemById because m_listBox->getItems() may be slow

        const bool ret = m_listBox->removeItem(itemName);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        const auto& ids = m_listBox->getItemIds();
        for (std::size_t i = 0; i < ids.size(); ++i)
        {
            if (ids[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemByIndex(std::size_t index)
    {
        const bool ret = m_listBox->removeItemByIndex(index);

        m_text.setString(m_listBox->getSelectedItem());

        // Shrink the list size
        if ((m_nrOfItemsToDisplay == 0) || (m_listBox->getItemCount() < m_nrOfItemsToDisplay))
            updateListBoxHeight();

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setString("");
        m_listBox->removeAllItems();

        updateListBoxHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getItemById(const sf::String& id) const
    {
        return m_listBox->getItemById(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItem() const
    {
        return m_listBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItemId() const
    {
        return m_listBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_listBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        const bool ret = m_listBox->changeItemByIndex(index, newValue);
        m_text.setString(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ComboBox::getItems() const
    {
        return m_listBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ComboBox::getItemIds() const
    {
        return m_listBox->getItemIds();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(std::size_t maximumItems)
    {
        m_listBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ComboBox::getMaximumItems() const
    {
        return m_listBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextSize(unsigned int textSize)
    {
        m_listBox->setTextSize(textSize);
        m_text.setCharacterSize(m_listBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getTextSize() const
    {
        return m_listBox->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setExpandDirection(ExpandDirection direction)
    {
        m_expandDirection = direction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ExpandDirection ComboBox::getExpandDirection() const
    {
        return m_expandDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::contains(const sf::String& item) const
    {
        const auto& items = getItems();
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::containsId(const sf::String& id) const
    {
        const auto& ids = getItemIds();
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setParent(Container* parent)
    {
        hideListBox();
        Widget::setParent(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(Vector2f)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Reselect the selected item to make sure it is always among the visible items when the list opens
            if (m_listBox->getSelectedItemIndex() >= 0)
                m_listBox->setSelectedItemByIndex(m_listBox->getSelectedItemIndex());
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseWheelScrolled(float delta, Vector2f)
    {
        // Only act to scrolling when the list is not being shown
        if (m_listBox->isVisible())
            return false;

        // Check if you are scrolling down
        if (delta < 0)
        {
            // Select the next item
            if (static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1) < m_listBox->getItemCount())
            {
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() + 1));
                m_text.setString(m_listBox->getSelectedItem());
            }
        }
        else // You are scrolling up
        {
            // Select the previous item
            if (m_listBox->getSelectedItemIndex() > 0)
            {
                m_listBox->setSelectedItemByIndex(static_cast<std::size_t>(m_listBox->getSelectedItemIndex() - 1));
                m_text.setString(m_listBox->getSelectedItem());
            }
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ComboBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_text.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "textstyle")
        {
            m_text.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "texturearrow")
        {
            m_spriteArrow.setTexture(getSharedRenderer()->getTextureArrow());
            setSize(m_size);
        }
        else if (property == "texturearrowhover")
        {
            m_spriteArrowHover.setTexture(getSharedRenderer()->getTextureArrowHover());
        }
        else if (property == "listbox")
        {
            m_listBox->setRenderer(getSharedRenderer()->getListBox());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "arrowbackgroundcolor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == "arrowbackgroundcolorhover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteArrow.setOpacity(m_opacityCached);
            m_spriteArrowHover.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            m_listBox->setInheritedFont(m_fontCached);

            setSize(m_size);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ComboBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getItemCount() > 0)
        {
            const auto& items = getItems();
            const auto& ids = getItemIds();

            bool itemIdsUsed = false;
            std::string itemList = "[" + Serializer::serialize(items[0]);
            std::string itemIdList = "[" + Serializer::serialize(ids[0]);
            for (std::size_t i = 1; i < items.size(); ++i)
            {
                itemList += ", " + Serializer::serialize(items[i]);
                itemIdList += ", " + Serializer::serialize(ids[i]);

                if (!ids[i].isEmpty())
                    itemIdsUsed = true;
            }
            itemList += "]";
            itemIdList += "]";

            node->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            if (itemIdsUsed)
                node->propertyValuePairs["ItemIds"] = std::make_unique<DataIO::ValueNode>(itemIdList);
        }

        node->propertyValuePairs["ItemsToDisplay"] = std::make_unique<DataIO::ValueNode>(to_string(getItemsToDisplay()));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(getTextSize()));
        node->propertyValuePairs["MaximumItems"] = std::make_unique<DataIO::ValueNode>(to_string(getMaximumItems()));

        if (getExpandDirection() != ComboBox::ExpandDirection::Down)
            node->propertyValuePairs["ExpandDirection"] = std::make_unique<DataIO::ValueNode>("Up");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["items"])
        {
            if (!node->propertyValuePairs["items"]->listNode)
                throw Exception{"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs["itemids"])
            {
                if (!node->propertyValuePairs["itemids"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs["items"]->valueList.size() != node->propertyValuePairs["itemids"]->valueList.size())
                    throw Exception{"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["items"]->valueList[i]).getString(),
                            Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["itemids"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (const auto& item : node->propertyValuePairs["items"]->valueList)
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs["itemids"])
                throw Exception{"Found 'ItemIds' property while there is no 'Items' property"};
        }

        if (node->propertyValuePairs["itemstodisplay"])
            setItemsToDisplay(tgui::stoi(node->propertyValuePairs["itemstodisplay"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumitems"])
            setMaximumItems(tgui::stoi(node->propertyValuePairs["maximumitems"]->value));

        if (node->propertyValuePairs["expanddirection"])
        {
            if (toLower(node->propertyValuePairs["expanddirection"]->value) == "up")
                setExpandDirection(ComboBox::ExpandDirection::Up);
            else if (toLower(node->propertyValuePairs["expanddirection"]->value) == "down")
                setExpandDirection(ComboBox::ExpandDirection::Down);
            else
                throw Exception{"Failed to parse ExpandDirection property. Only the values Up and Down are correct."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ComboBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::updateListBoxHeight()
    {
        const Borders borders = m_listBox->getSharedRenderer()->getBorders();
        const Padding padding = m_listBox->getSharedRenderer()->getPadding();

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * (std::min<std::size_t>(m_nrOfItemsToDisplay, std::max<std::size_t>(m_listBox->getItemCount(), 1))))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});
        else
            m_listBox->setSize({getSize().x, (m_listBox->getItemHeight() * std::max<std::size_t>(m_listBox->getItemCount(), 1))
                                             + borders.getTop() + borders.getBottom() + padding.getTop() + padding.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_listBox->isVisible() && getParent())
        {
            m_listBox->setVisible(true);

            // Find the GuiContainer that contains the combo box
            Container* container = getParent();
            while (container->getParent() != nullptr)
                container = container->getParent();

            if (m_expandDirection == ExpandDirection::Down)
                m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y + getSize().y - m_bordersCached.getBottom()});
            else // if (m_expandDirection == ExpandDirection::Up)
                m_listBox->setPosition({getAbsolutePosition().x, getAbsolutePosition().y - m_listBox->getSize().y + m_bordersCached.getTop()});

            container->add(m_listBox, "#TGUI_INTERNAL$ComboBoxListBox#");
            m_listBox->setFocused(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        // If the list was open then close it now
        if (m_listBox->isVisible())
        {
            m_listBox->setVisible(false);
            m_listBox->mouseNoLongerOnWidget();

            // Find the GuiContainer in order to remove the ListBox from it
            Widget* container = this;
            while (container->getParent() != nullptr)
                container = container->getParent();

            if (container != this)
                static_cast<Container*>(container)->remove(m_listBox);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initListBox()
    {
        m_listBox->setVisible(false);

        m_listBox->connect("ItemSelected", [this](){
                                                m_text.setString(m_listBox->getSelectedItem());
                                                onItemSelect.emit(this, m_listBox->getSelectedItem(), m_listBox->getSelectedItemId());
                                            });

        m_listBox->connect("Unfocused", [this](){
                                                    if (!m_mouseHover)
                                                        hideListBox();
                                                });

        m_listBox->connect("MouseReleased", [this](){ hideListBox(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        sf::RenderStates statesForText = states;

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Check if we have a texture for the arrow
        float arrowSize;
        if (m_spriteArrow.isSet())
        {
            arrowSize = m_spriteArrow.getSize().x;
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (m_mouseHover && m_spriteArrowHover.isSet())
                m_spriteArrowHover.draw(target, states);
            else
                m_spriteArrow.draw(target, states);
        }
        else // There are no textures for the arrow
        {
            arrowSize = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            states.transform.translate({getInnerSize().x - m_paddingCached.getRight() - arrowSize, m_paddingCached.getTop()});

            if (m_mouseHover && m_arrowBackgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorHoverCached);
            else
                drawRectangleShape(target, states, {arrowSize, arrowSize}, m_arrowBackgroundColorCached);

            sf::ConvexShape arrow{3};
            arrow.setPoint(0, {arrowSize / 5, arrowSize / 5});
            arrow.setPoint(1, {arrowSize / 2, arrowSize * 4/5});
            arrow.setPoint(2, {arrowSize * 4/5, arrowSize / 5});

            if (m_mouseHover && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(m_arrowColorHoverCached);
            else
                arrow.setFillColor(m_arrowColorCached);

            target.draw(arrow, states);
        }

        // Draw the selected item
        if (!m_text.getString().isEmpty())
        {
            const Clipping clipping{target, statesForText, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - arrowSize, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            statesForText.transform.translate(m_paddingCached.getLeft() + Text::getExtraHorizontalPadding(m_text),
                                              m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) - m_text.getSize().y) / 2.0f));
            m_text.draw(target, statesForText);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Layout2d& size)
    {
        m_type = "Group";

        m_renderer = aurora::makeCopied<GroupRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::create(const Layout2d& size)
    {
        return std::make_shared<Group>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::copy(Group::ConstPtr group)
    {
        if (group)
            return std::static_pointer_cast<Group>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getSharedRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const GroupRenderer* Group::getSharedRenderer() const
    {
        return aurora::downcast<const GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const GroupRenderer* Group::getRenderer() const
    {
        return aurora::downcast<const GroupRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::setSize(const Layout2d& size)
    {
        m_paddingCached.updateParentSize(size.getValue());

        Container::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getInnerSize() const
    {
        return {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft(), m_paddingCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Group::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            const Vector2f offset = getChildWidgetsOffset();
            for (const auto& widget : m_widgets)
            {
                if (widget->isVisible())
                {
                    if (widget->mouseOnWidget(pos - offset))
                        return true;
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;
        Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::rendererChanged(const std::string& property)
    {
        if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition().x + m_paddingCached.getLeft(), getPosition().y + m_paddingCached.getTop());

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        const Vector2f innerSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                        getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};
        const Clipping clipping{target, states, {}, innerSize};

        // Draw the child widgets
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::HorizontalLayout(const Layout2d& size) :
        BoxLayoutRatios{size}
    {
        m_type = "HorizontalLayout";

        m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::create(const Layout2d& size)
    {
        return std::make_shared<HorizontalLayout>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalLayout::Ptr HorizontalLayout::copy(HorizontalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<HorizontalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void HorizontalLayout::updateWidgets()
    {
        const float totalSpaceBetweenWidgets = (m_spaceBetweenWidgetsCached * m_widgets.size()) - m_spaceBetweenWidgetsCached;
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                          getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        const float totalRatio = std::accumulate(m_ratios.begin(), m_ratios.end(), 0.f);

        float currentOffset = 0;
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            auto& widget = m_widgets[i];
            const float width = (contentSize.x - totalSpaceBetweenWidgets) * (m_ratios[i] / totalRatio);

            widget->setSize({width, contentSize.y});
            widget->setPosition({currentOffset, 0});

            // Correct the size for widgets that are bigger than what you set (e.g. have borders around it or a text next to them)
            if (widget->getFullSize() != widget->getSize())
            {
                const Vector2f newSize = widget->getSize() - (widget->getFullSize() - widget->getSize());
                if (newSize.x > 0 && newSize.y > 0)
                {
                    widget->setSize(newSize);
                    widget->setPosition(widget->getPosition() - widget->getWidgetOffset());
                }
            }

            currentOffset += width + m_spaceBetweenWidgetsCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Knob.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    const float pi = 3.14159265358979f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Knob()
    {
        m_type = "Knob";
        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<KnobRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(140, 140);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::create()
    {
        return std::make_shared<Knob>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Knob::Ptr Knob::copy(Knob::ConstPtr knob)
    {
        if (knob)
            return std::static_pointer_cast<Knob>(knob->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KnobRenderer* Knob::getSharedRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getSharedRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    KnobRenderer* Knob::getRenderer()
    {
        return aurora::downcast<KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const KnobRenderer* Knob::getRenderer() const
    {
        return aurora::downcast<const KnobRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
        {
            m_spriteBackground.setSize(getInnerSize());
            m_spriteForeground.setSize({m_spriteForeground.getTexture().getImageSize().x / m_spriteBackground.getTexture().getImageSize().x * getInnerSize().x,
                                        m_spriteForeground.getTexture().getImageSize().y / m_spriteBackground.getTexture().getImageSize().y * getInnerSize().y});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setStartRotation(float startRotation)
    {
        while (startRotation >= 360)
            startRotation -= 360;
        while (startRotation < 0)
            startRotation += 360;

        m_startRotation = startRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Knob::getStartRotation() const
    {
        return m_startRotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setEndRotation(float endRotation)
    {
        while (endRotation >= 360)
            endRotation -= 360;
        while (endRotation < 0)
            endRotation += 360;

        m_endRotation = endRotation;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Knob::getEndRotation() const
    {
        return m_endRotation;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMinimum(int minimum)
    {
        if (m_minimum != minimum)
        {
            // Set the new minimum
            m_minimum = minimum;

            // The maximum can't be below the minimum
            if (m_maximum < m_minimum)
                m_maximum = m_minimum;

            // When the value is below the minimum then adjust it
            if (m_value < m_minimum)
                setValue(m_minimum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setMaximum(int maximum)
    {
        if (m_maximum != maximum)
        {
            // Set the new maximum
            if (maximum > 0)
                m_maximum = maximum;
            else
                m_maximum = 1;

            // The minimum can't be below the maximum
            if (m_minimum > m_maximum)
                m_minimum = m_maximum;

            // When the value is above the maximum then adjust it
            if (m_value > m_maximum)
                setValue(m_maximum);

            // The knob might have to point in a different direction even though it has the same value
            recalculateRotation();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setValue(int value)
    {
        if (m_value != value)
        {
            // Set the new value
            m_value = value;

            // When the value is below the minimum or above the maximum then adjust it
            if (m_value < m_minimum)
                m_value = m_minimum;
            else if (m_value > m_maximum)
                m_value = m_maximum;

            // The knob might have to point in a different direction
            recalculateRotation();

            onValueChange.emit(this, m_value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Knob::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::setClockwiseTurning(bool clockwise)
    {
        m_clockwiseTurning = clockwise;

        // The knob might have to point in a different direction even though it has the same value
        recalculateRotation();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::getClockwiseTurning() const
    {
        return m_clockwiseTurning;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Knob::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the widget
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (m_spriteBackground.isSet() && m_spriteForeground.isSet())
            {
                // Only return true when the pixel under the mouse isn't transparent
                if (!m_spriteBackground.isTransparentPixel(pos - m_bordersCached.getOffset()))
                    return true;
            }
            else // There is no texture, the widget has a circle shape
            {
                const Vector2f centerPoint = getSize() / 2.f;
                const float distance = std::sqrt(std::pow(centerPoint.x - pos.x, 2.f) + std::pow(centerPoint.y - pos.y, 2.f));
                return (distance <= std::min(getSize().x, getSize().y));
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMousePressed(Vector2f pos)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // Change the value of the knob depending on where you clicked
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::leftMouseReleased(Vector2f)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        const Vector2f centerPosition = getSize() / 2.0f;

        // Check if the mouse button is down
        if (m_mouseDown)
        {
            // Find out the direction that the knob should now point
            if (compareFloats(pos.x, centerPosition.x))
            {
                if (pos.y > centerPosition.y)
                    m_angle = 270;
                else if (pos.y < centerPosition.y)
                    m_angle = 90;
            }
            else
            {
                m_angle = std::atan2(centerPosition.y - pos.y, pos.x - centerPosition.x) * 180.0f / pi;
                if (m_angle < 0)
                    m_angle += 360;
            }

            // The angle might lie on a part where it isn't allowed
            if (m_angle > m_startRotation)
            {
                if ((m_angle < m_endRotation) && (m_clockwiseTurning))
                {
                    if ((m_angle - m_startRotation) <= (m_endRotation - m_angle))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
                else if (m_angle > m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_angle - m_startRotation, 360 - m_angle + m_startRotation) <= std::min(m_angle - m_endRotation, 360 - m_angle + m_endRotation))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
            }
            else if (m_angle < m_startRotation)
            {
                if (m_angle < m_endRotation)
                {
                    if (((m_startRotation > m_endRotation) && (m_clockwiseTurning))
                     || ((m_startRotation < m_endRotation) && (!m_clockwiseTurning)))
                    {
                        if (std::min(m_startRotation - m_angle, 360 - m_startRotation + m_angle) <= std::min(m_endRotation - m_angle, 360 -m_endRotation + m_angle))
                            m_angle = m_startRotation;
                        else
                            m_angle = m_endRotation;
                    }
                }
                else if ((m_angle > m_endRotation) && (!m_clockwiseTurning))
                {
                    if ((m_startRotation - m_angle) <= (m_angle - m_endRotation))
                        m_angle = m_startRotation;
                    else
                        m_angle = m_endRotation;
                }
            }

            // Calculate the difference in degrees between the start and end rotation
            float allowedAngle = 0;
            if (compareFloats(m_startRotation, m_endRotation))
                allowedAngle = 360;
            else
            {
                if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
                 || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
                {
                    allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
                }
                else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                      || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
                {
                    allowedAngle = std::abs(m_endRotation - m_startRotation);
                }
            }

            // Calculate the right value
            if (m_clockwiseTurning)
            {
                if (m_angle < m_startRotation)
                    setValue(static_cast<int>(((m_startRotation - m_angle) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    if (compareFloats(m_angle, m_startRotation))
                        setValue(m_minimum);
                    else
                        setValue(static_cast<int>((((360.0f - m_angle) + m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
            else // counter-clockwise
            {
                if (m_angle >= m_startRotation)
                    setValue(static_cast<int>(((m_angle - m_startRotation) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                else
                {
                    setValue(static_cast<int>(((m_angle + (360.0f - m_startRotation)) / allowedAngle * (m_maximum - m_minimum)) + m_minimum));
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::recalculateRotation()
    {
        // Calculate the difference in degrees between the start and end rotation
        float allowedAngle = 0;
        if (compareFloats(m_startRotation, m_endRotation))
            allowedAngle = 360;
        else
        {
            if (((m_endRotation > m_startRotation) && (m_clockwiseTurning))
             || ((m_endRotation < m_startRotation) && (!m_clockwiseTurning)))
            {
                allowedAngle = 360 - std::abs(m_endRotation - m_startRotation);
            }
            else if (((m_endRotation > m_startRotation) && (!m_clockwiseTurning))
                  || ((m_endRotation < m_startRotation) && (m_clockwiseTurning)))
            {
                allowedAngle = std::abs(m_endRotation - m_startRotation);
            }
        }

        // Calculate the angle for the direction of the knob
        if (m_clockwiseTurning)
        {
            if (m_value == m_minimum)
                m_angle = m_startRotation;
            else
                m_angle = m_startRotation - (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle);
        }
        else // counter-clockwise
        {
            m_angle = (((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)) * allowedAngle) + m_startRotation;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Knob::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
            setSize(m_size);
        }
        else if (property == "textureforeground")
        {
            m_spriteForeground.setTexture(getSharedRenderer()->getTextureForeground());
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "imagerotation")
        {
            m_imageRotationCached = getSharedRenderer()->getImageRotation();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteForeground.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Knob::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["ClockwiseTurning"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_clockwiseTurning));
        node->propertyValuePairs["StartRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_startRotation));
        node->propertyValuePairs["EndRotation"] = std::make_unique<DataIO::ValueNode>(to_string(m_endRotation));
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["startrotation"])
            setStartRotation(tgui::stof(node->propertyValuePairs["startrotation"]->value));
        if (node->propertyValuePairs["endrotation"])
            setEndRotation(tgui::stof(node->propertyValuePairs["endrotation"]->value));
        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["clockwiseturning"])
            setClockwiseTurning(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["clockwiseturning"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Knob::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Knob::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const float size = std::min(getInnerSize().x, getInnerSize().y);

        // Draw the borders
        const float borderThickness = std::min({m_bordersCached.getLeft(), m_bordersCached.getTop(), m_bordersCached.getRight(), m_bordersCached.getBottom()});
        if (borderThickness > 0)
        {
            states.transform.translate({borderThickness, borderThickness});

            sf::CircleShape bordersShape{size / 2};
            bordersShape.setFillColor(Color::Transparent);
            bordersShape.setOutlineColor(Color::calcColorOpacity(m_borderColorCached, m_opacityCached));
            bordersShape.setOutlineThickness(borderThickness);
            target.draw(bordersShape, states);
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
        {
            sf::CircleShape background{size / 2};
            background.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));
            target.draw(background, states);
        }

        // Draw the foreground
        if (m_spriteForeground.isSet())
        {
            states.transform.translate((getInnerSize() - m_spriteForeground.getSize()) / 2.f);

            // Give the image the correct rotation
            if (m_imageRotationCached > m_angle)
                states.transform.rotate(m_imageRotationCached - m_angle, (m_spriteForeground.getSize() / 2.f));
            else
                states.transform.rotate(360 - m_angle + m_imageRotationCached, (m_spriteForeground.getSize() / 2.f));

            m_spriteForeground.draw(target, states);
        }
        else
        {
            sf::CircleShape thumb{size / 10.0f};
            thumb.setFillColor(Color::calcColorOpacity(m_thumbColorCached, m_opacityCached));
            thumb.setPosition({(size / 2.0f) - thumb.getRadius() + (std::cos(m_angle / 180 * pi) * (size / 2) * 3/5),
                               (size / 2.0f) - thumb.getRadius() + (-std::sin(m_angle / 180 * pi) * (size / 2) * 3/5)});
            target.draw(thumb, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_type = "ListBox";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ListBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 1.25f));
        setSize({Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 10,
                 (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create()
    {
        return std::make_shared<ListBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::copy(ListBox::ConstPtr listBox)
    {
        if (listBox)
            return std::static_pointer_cast<ListBox>(listBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getSharedRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListBoxRenderer* ListBox::getSharedRenderer() const
    {
        return aurora::downcast<const ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListBoxRenderer* ListBox::getRenderer() const
    {
        return aurora::downcast<const ListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        for (std::size_t i = 0; i < m_items.size(); ++i)
            m_items[i].setPosition({0, (i * m_itemHeight) + ((m_itemHeight - m_items[i].getSize().y) / 2.0f)});

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        m_scroll->setSize({m_scroll->getSize().x, std::max(0.f, getInnerSize().y)});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::addItem(const sf::String& itemName, const sf::String& id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_maxItems == 0) || (m_items.size() < m_maxItems))
        {
            m_scroll->setMaximum(static_cast<unsigned int>((m_items.size() + 1) * m_itemHeight));

            // Scroll down when auto-scrolling is enabled
            if (m_autoScroll && (m_scroll->getViewportSize() < m_scroll->getMaximum()))
                m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());

            // Create the new item
            Text newItem;
            newItem.setFont(m_fontCached);
            newItem.setColor(m_textColorCached);
            newItem.setOpacity(m_opacityCached);
            newItem.setStyle(m_textStyleCached);
            newItem.setCharacterSize(m_textSize);
            newItem.setString(itemName);
            newItem.setPosition({0, (m_items.size() * m_itemHeight) + ((m_itemHeight - newItem.getSize().y) / 2.0f)});

            // Add the new item to the list
            m_items.push_back(std::move(newItem));
            m_itemIds.push_back(id);
            return true;
        }
        else // The item limit was reached
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
        {
            deselectItem();
            return false;
        }

        updateSelectedItem(static_cast<int>(index));

        // Move the scrollbar
        if (m_selectedItem * getItemHeight() < m_scroll->getValue())
            m_scroll->setValue(m_selectedItem * getItemHeight());
        else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getViewportSize())
            m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getViewportSize());

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        // Keep it simple and forget hover when an item is removed
        updateHoveringItem(-1);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            updateSelectedItem(-1);
        else if (m_selectedItem > static_cast<int>(index))
        {
            // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
            m_selectedItem = m_selectedItem - 1;
        }

        // Remove the item
        m_items.erase(m_items.begin() + index);
        m_itemIds.erase(m_itemIds.begin() + index);

        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Unselect any selected item
        updateSelectedItem(-1);
        updateHoveringItem(-1);

        // Clear the list, remove all items
        m_items.clear();
        m_itemIds.clear();

        m_scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemById(const sf::String& id) const
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return m_items[i].getString();
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItem() const
    {
        return (m_selectedItem >= 0) ? m_items[m_selectedItem].getString() : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItemId() const
    {
        return (m_selectedItem >= 0) ? m_itemIds[m_selectedItem] : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == originalValue)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        if (index >= m_items.size())
            return false;

        m_items[index].setString(newValue);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListBox::getItems() const
    {
        std::vector<sf::String> items;
        for (const auto& item : m_items)
            items.push_back(item.getString());

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ListBox::getItemIds() const
    {
        return m_itemIds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
        {
            m_textSize = Text::findBestTextSize(m_fontCached, itemHeight * 0.8f);
            for (auto& item : m_items)
                item.setCharacterSize(m_textSize);
        }

        m_scroll->setScrollAmount(m_itemHeight);
        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        for (auto& item : m_items)
            item.setCharacterSize(m_textSize);

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(std::size_t maximumItems)
    {
        // Set the new limit
        m_maxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_maxItems > 0) && (m_maxItems < m_items.size()))
        {
            // Remove the items that passed the limitation
            m_items.erase(m_items.begin() + m_maxItems, m_items.end());
            m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());

            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            setPosition(m_position);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getMaximumItems() const
    {
        return m_maxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setAutoScroll(bool autoScroll)
    {
        m_autoScroll = autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::getAutoScroll() const
    {
        return m_autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::contains(const sf::String& item) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [item](const Text& text){ return text.getString() == item; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::containsId(const sf::String& id) const
    {
        return std::find(m_itemIds.begin(), m_itemIds.end(), id) != m_itemIds.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        if (m_scroll->mouseOnWidget(pos))
        {
            m_scroll->leftMousePressed(pos);
        }
        else
        {
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                if (m_hoveringItem >= 0)
                    onMousePress.emit(this, m_items[m_hoveringItem].getString(), m_itemIds[m_hoveringItem]);

                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    updateSelectedItem(m_hoveringItem);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown && !m_scroll->isMouseDown())
        {
            if (m_selectedItem >= 0)
                onMouseRelease.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    onDoubleClick.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }

        m_scroll->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        updateHoveringItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_scroll->isMouseDown() && m_scroll->isMouseDownOnThumb()) || m_scroll->mouseOnWidget(pos))
        {
            m_scroll->mouseMoved(pos);
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_scroll->mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovering
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                          m_bordersCached.getTop() + m_paddingCached.getTop(), getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                // If the mouse is held down then select the item below the mouse
                if (m_mouseDown && !m_scroll->isMouseDown())
                {
                    if (m_selectedItem != m_hoveringItem)
                    {
                        m_possibleDoubleClick = false;

                        updateSelectedItem(m_hoveringItem);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_scroll->isShown())
        {
            m_scroll->mouseWheelScrolled(delta, pos - getPosition());

            // Update on which item the mouse is hovering
            mouseMoved(pos);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();

        updateHoveringItem(-1);

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_scroll->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            for (auto& item : m_items)
                item.setStyle(m_textStyleCached);

            if ((m_selectedItem >= 0) && m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }
        else if (property == "selectedtextstyle")
        {
            m_selectedTextStyleCached = getSharedRenderer()->getSelectedTextStyle();

            if (m_selectedItem >= 0)
            {
                if (m_selectedTextStyleCached.isSet())
                    m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
                else
                    m_items[m_selectedItem].setStyle(m_textStyleCached);
            }
        }
        else if (property == "scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_scroll->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            for (auto& item : m_items)
                item.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& item : m_items)
                item.setFont(m_fontCached);

            // Recalculate the text size with the new font
            if (m_requestedTextSize == 0)
            {
                m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                    item.setCharacterSize(m_textSize);
            }

            setPosition(m_position);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ListBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getItemCount() > 0)
        {
            auto items = getItems();
            auto& ids = getItemIds();

            bool itemIdsUsed = false;
            std::string itemList = "[" + Serializer::serialize(items[0]);
            std::string itemIdList = "[" + Serializer::serialize(ids[0]);
            for (std::size_t i = 1; i < items.size(); ++i)
            {
                itemList += ", " + Serializer::serialize(items[i]);
                itemIdList += ", " + Serializer::serialize(ids[i]);

                if (!ids[i].isEmpty())
                    itemIdsUsed = true;
            }
            itemList += "]";
            itemIdList += "]";

            node->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            if (itemIdsUsed)
                node->propertyValuePairs["ItemIds"] = std::make_unique<DataIO::ValueNode>(itemIdList);
        }

        if (!m_autoScroll)
            node->propertyValuePairs["AutoScroll"] = std::make_unique<DataIO::ValueNode>("false");

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
        node->propertyValuePairs["MaximumItems"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxItems));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["items"])
        {
            if (!node->propertyValuePairs["items"]->listNode)
                throw Exception{"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs["itemids"])
            {
                if (!node->propertyValuePairs["itemids"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs["items"]->valueList.size() != node->propertyValuePairs["itemids"]->valueList.size())
                    throw Exception{"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["items"]->valueList[i]).getString(),
                            Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["itemids"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (const auto& item : node->propertyValuePairs["items"]->valueList)
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs["itemids"])
                throw Exception{"Found 'ItemIds' property while there is no 'Items' property"};
        }

        if (node->propertyValuePairs["autoscroll"])
            setAutoScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autoscroll"]->value).getBool());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["itemheight"])
            setItemHeight(tgui::stoi(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["maximumitems"])
            setMaximumItems(tgui::stoi(node->propertyValuePairs["maximumitems"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedAndHoveringItemColorsAndStyle()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorHoverCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorCached);

            if (m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            if (m_textColorHoverCached.isSet())
                m_items[m_hoveringItem].setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColorsAndStyle()
    {
        for (auto& item : m_items)
        {
            item.setColor(m_textColorCached);
            item.setStyle(m_textStyleCached);
        }

        updateSelectedAndHoveringItemColorsAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateHoveringItem(int item)
    {
        if (m_hoveringItem != item)
        {
            if (m_hoveringItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorCached.isSet())
                    m_items[m_hoveringItem].setColor(m_selectedTextColorCached);
                else
                    m_items[m_hoveringItem].setColor(m_textColorCached);
            }

            m_hoveringItem = item;

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedItem(int item)
    {
        if (m_selectedItem != item)
        {
            if (m_selectedItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_textColorHoverCached.isSet())
                    m_items[m_selectedItem].setColor(m_textColorHoverCached);
                else
                    m_items[m_selectedItem].setColor(m_textColorCached);

                m_items[m_selectedItem].setStyle(m_textStyleCached);
            }

            m_selectedItem = item;
            if (m_selectedItem >= 0)
                onItemSelect.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            else
                onItemSelect.emit(this, "", "");

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the items and their selected/hover backgrounds
        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_scroll->isShown())
                maxItemWidth -= m_scroll->getSize().x;
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            // Find out which items are visible
            std::size_t firstItem = 0;
            std::size_t lastItem = m_items.size();
            if (m_scroll->getViewportSize() < m_scroll->getMaximum())
            {
                firstItem = m_scroll->getValue() / m_itemHeight;
                lastItem = (m_scroll->getValue() + m_scroll->getViewportSize()) / m_itemHeight;

                // Show another item when the scrollbar is standing between two items
                if ((m_scroll->getValue() + m_scroll->getViewportSize()) % m_itemHeight != 0)
                    ++lastItem;
            }

            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop() - m_scroll->getValue()});

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, m_selectedItem * static_cast<float>(m_itemHeight)});

                const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveringItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorCached);

                states.transform.translate({0, -m_selectedItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, m_hoveringItem * static_cast<float>(m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({0, -m_hoveringItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the items
            states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSize, m_textStyleCached), 0});
            for (std::size_t i = firstItem; i < lastItem; ++i)
                m_items[i].draw(target, states);
        }

        // Draw the scrollbar
        m_scroll->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_type = "MessageBox";

        m_renderer = aurora::makeCopied<MessageBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTitleButtons(ChildWindow::TitleButton::None);
        setTextSize(getGlobalTextSize());

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSize);

        setSize({400, 150});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& other) :
        ChildWindow      {other},
        onButtonPress    {other.onButtonPress},
        m_loadedThemeFile{other.m_loadedThemeFile},
        m_buttonClassName{other.m_buttonClassName},
        m_textSize       {other.m_textSize}
    {
        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(MessageBox&& other) :
        ChildWindow      {std::move(other)},
        onButtonPress    {std::move(other.onButtonPress)},
        m_loadedThemeFile{std::move(other.m_loadedThemeFile)},
        m_buttonClassName{std::move(other.m_buttonClassName)},
        m_buttons        {std::move(other.m_buttons)},
        m_label          {std::move(other.m_label)},
        m_textSize       {std::move(other.m_textSize)}
    {
        for (auto& button : m_buttons)
        {
            button->disconnectAll("Pressed");
            button->connect("Pressed", [=]() { onButtonPress.emit(this, button->getText()); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& other)
    {
        if (this != &other)
        {
            MessageBox temp(other);
            ChildWindow::operator=(temp);

            std::swap(onButtonPress,     temp.onButtonPress);
            std::swap(m_loadedThemeFile, temp.m_loadedThemeFile);
            std::swap(m_buttonClassName, temp.m_buttonClassName);
            std::swap(m_buttons,         temp.m_buttons);
            std::swap(m_label,           temp.m_label);
            std::swap(m_textSize,        temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (MessageBox&& other)
    {
        if (this != &other)
        {
            ChildWindow::operator=(std::move(other));

            onButtonPress     = std::move(other.onButtonPress);
            m_loadedThemeFile = std::move(other.m_loadedThemeFile);
            m_buttonClassName = std::move(other.m_buttonClassName);
            m_buttons         = std::move(other.m_buttons);
            m_label           = std::move(other.m_label);
            m_textSize        = std::move(other.m_textSize);

            for (auto& button : m_buttons)
            {
                button->disconnectAll("Pressed");
                button->connect("Pressed", [=]() { onButtonPress.emit(this, button->getText()); });
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create(sf::String title, sf::String text, std::vector<sf::String> buttons)
    {
        auto messageBox = std::make_shared<MessageBox>();
        messageBox->setTitle(title);
        messageBox->setText(text);
        for (auto& buttonText : buttons)
            messageBox->addButton(std::move(buttonText));

        return messageBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::copy(MessageBox::ConstPtr messageBox)
    {
        if (messageBox)
            return std::static_pointer_cast<MessageBox>(messageBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getSharedRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MessageBoxRenderer* MessageBox::getSharedRenderer() const
    {
        return aurora::downcast<const MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MessageBoxRenderer* MessageBox::getRenderer() const
    {
        return aurora::downcast<const MessageBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const sf::String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& MessageBox::getText() const
    {
        return m_label->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_textSize = size;

        m_label->setTextSize(size);

        for (auto& button : m_buttons)
            button->setTextSize(m_textSize);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MessageBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        auto button = Button::create(caption);
        button->setRenderer(getSharedRenderer()->getButton());
        button->setTextSize(m_textSize);
        button->connect("Pressed", [=]() { onButtonPress.emit(this, caption); });

        add(button, "#TGUI_INTERNAL$MessageBoxButton:" + caption + "#");
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> MessageBox::getButtons() const
    {
        std::vector<sf::String> buttonTexts;
        for (auto& button : m_buttons)
            buttonTexts.emplace_back(button->getText());

        return buttonTexts;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        float buttonWidth = 120;
        float buttonHeight = 24;

        // Calculate the button size
        if (m_fontCached)
        {
            buttonWidth = 4.0f * Text::getLineHeight(m_fontCached, m_textSize);
            buttonHeight = Text::getLineHeight(m_fontCached, m_textSize) * 1.25f;

            for (const auto& button : m_buttons)
            {
                const float width = sf::Text(button->getText(), *m_fontCached.getFont(), m_textSize).getLocalBounds().width;
                if (buttonWidth < width * 10.0f / 9.0f)
                    buttonWidth = width * 10.0f / 9.0f;
            }
        }

        // Calculate the space needed for the buttons
        const float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (auto& button : m_buttons)
        {
            button->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += button->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        Vector2f size = {2*distance + m_label->getSize().x, 3*distance + m_label->getSize().y + buttonHeight};

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > size.x)
            size.x = buttonsAreaWidth;

        // Set the size of the window
        setSize(size);

        // Set the text on the correct position
        m_label->setPosition({distance, distance});

        // Set the buttons on the correct position
        float leftPosition = 0;
        const float topPosition = 2*distance + m_label->getSize().y;
        for (auto& button : m_buttons)
        {
            leftPosition += distance + ((size.x - buttonsAreaWidth) / (m_buttons.size()+1));
            button->setPosition({leftPosition, topPosition});
            leftPosition += button->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MessageBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onButtonPress.getName()))
            return onButtonPress;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_label->getRenderer()->setTextColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "button")
        {
            const auto& renderer = getSharedRenderer()->getButton();
            for (auto& button : m_buttons)
                button->setRenderer(renderer);
        }
        else if (property == "font")
        {
            ChildWindow::rendererChanged(property);

            m_label->setInheritedFont(m_fontCached);

            for (auto& button : m_buttons)
                button->setInheritedFont(m_fontCached);

            rearrange();
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MessageBox::save(SavingRenderersMap& renderers) const
    {
        auto node = ChildWindow::save(renderers);
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        // Label and buttons are saved indirectly by saving the child window
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        // Remove the label that the MessageBox constructor creates because it will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::identifyLabelAndButtons()
    {
        m_label = get<Label>("#TGUI_INTERNAL$MessageBoxText#");

        for (unsigned int i = 0; i < m_widgets.size(); ++i)
        {
            if ((m_widgetNames[i].getSize() >= 32) && (m_widgetNames[i].substring(0, 32) == "#TGUI_INTERNAL$MessageBoxButton:"))
            {
                auto button = std::dynamic_pointer_cast<Button>(m_widgets[i]);

                button->disconnectAll("Pressed");
                button->connect("Pressed", [=]() { onButtonPress.emit(this, button->getText()); });
                m_buttons.push_back(button);
            }
        }

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Picture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture()
    {
        m_type = "Picture";

        m_renderer = aurora::makeCopied<PictureRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const Texture& texture, bool transparentTexture) :
        Picture{}
    {
        getRenderer()->setTexture(texture);
        getRenderer()->setTransparentTexture(transparentTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::create(const Texture& texture, bool fullyClickable)
    {
        return std::make_shared<Picture>(texture, fullyClickable);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::copy(Picture::ConstPtr picture)
    {
        if (picture)
            return std::static_pointer_cast<Picture>(picture->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PictureRenderer* Picture::getSharedRenderer()
    {
        return aurora::downcast<PictureRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PictureRenderer* Picture::getSharedRenderer() const
    {
        return aurora::downcast<const PictureRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PictureRenderer* Picture::getRenderer()
    {
        return aurora::downcast<PictureRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PictureRenderer* Picture::getRenderer() const
    {
        return aurora::downcast<const PictureRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_sprite.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the picture
        if (!m_ignoringMouseEvents && (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos)))
        {
            if (!m_transparentTextureCached || !m_sprite.isTransparentPixel(pos))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                onDoubleClick.emit(this, pos - getPosition());
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Picture::getSignal(std::string signalName)
    {
        if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::rendererChanged(const std::string& property)
    {
        if (property == "texture")
        {
            const auto& texture = getSharedRenderer()->getTexture();

            if (!m_sprite.isSet() && (getSize() == Vector2f{0,0}))
                setSize(texture.getImageSize());

            m_sprite.setTexture(texture);
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);
            m_sprite.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Picture::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_ignoringMouseEvents)
            node->propertyValuePairs["IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["ignoremouseevents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["ignoremouseevents"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        m_sprite.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Container.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::RadioButton()
    {
        m_type = "RadioButton";
        m_text.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<RadioButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_text) + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                 Text::getLineHeight(m_text) + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::create()
    {
        return std::make_shared<RadioButton>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButton::Ptr RadioButton::copy(RadioButton::ConstPtr radioButton)
    {
        if (radioButton)
            return std::static_pointer_cast<RadioButton>(radioButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonRenderer* RadioButton::getSharedRenderer()
    {
        return aurora::downcast<RadioButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RadioButtonRenderer* RadioButton::getSharedRenderer() const
    {
        return aurora::downcast<const RadioButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonRenderer* RadioButton::getRenderer()
    {
        return aurora::downcast<RadioButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RadioButtonRenderer* RadioButton::getRenderer() const
    {
        return aurora::downcast<const RadioButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        // If the text is auto sized then recalculate the size
        if (m_textSize == 0)
            setText(getText());

        updateTextureSizes();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getFullSize() const
    {
        if (getText().isEmpty())
            return getSize();
        else
            return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getWidgetOffset() const
    {
        if (getText().isEmpty() || (getSize().y >= m_text.getSize().y))
            return {0, 0};
        else
            return {0, -(m_text.getSize().y - getSize().y) / 2};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setChecked(bool checked)
    {
        if (m_checked == checked)
            return;

        if (checked)
        {
            // Tell our parent that all the radio buttons should be unchecked
            if (m_parent)
                m_parent->uncheckRadioButtons();

            m_checked = true;
            onCheck.emit(this, true);
        }
        else
        {
            m_checked = false;
            onUncheck.emit(this, false);
        }

        updateTextColor();
        if (m_checked && m_textStyleCheckedCached.isSet())
            m_text.setStyle(m_textStyleCheckedCached);
        else
            m_text.setStyle(m_textStyleCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setString(text);

        // Set the text size
        if (m_textSize == 0)
            m_text.setCharacterSize(Text::findBestTextSize(m_fontCached, getSize().y * 0.8f));
        else
            m_text.setCharacterSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& RadioButton::getText() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextSize(unsigned int size)
    {
        m_textSize = size;
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int RadioButton::getTextSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::setTextClickable(bool acceptTextClick)
    {
        m_allowTextClick = acceptTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::isTextClickable() const
    {
        return m_allowTextClick;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RadioButton::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (m_allowTextClick && !getText().isEmpty())
        {
            // Check if the mouse is on top of the image or the small gap between image and text
            if (FloatRect{0, 0, getSize().x + getSize().x * m_textDistanceRatioCached, getSize().y}.contains(pos))
                return true;

            // Check if the mouse is on top of the text
            if (FloatRect{0, 0, m_text.getSize().x, m_text.getSize().y}.contains(pos.x - (getSize().x + (getSize().x * m_textDistanceRatioCached)),
                                                                                 pos.y - ((getSize().y - m_text.getSize().y) / 2.0f)))
                return true;
        }
        else // You are not allowed to click on the text
        {
            // Check if the mouse is on top of the image
            if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            {
                if (!m_transparentTextureCached || !m_spriteUnchecked.isSet() || !m_spriteChecked.isSet()
                 || !m_spriteUnchecked.isTransparentPixel(pos - m_bordersCached.getOffset()))
                    return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        // Check the radio button if we clicked on the radio button (not just mouse release)
        if (mouseDown)
            setChecked(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::keyPressed(const sf::Event::KeyEvent& event)
    {
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            setChecked(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseEnteredWidget()
    {
        Widget::mouseEnteredWidget();
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::mouseLeftWidget()
    {
        Widget::mouseLeftWidget();
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& RadioButton::getSignal(std::string signalName)
    {
        if (signalName == toLower(onCheck.getName()))
            return onCheck;
        else if (signalName == toLower(onUncheck.getName()))
            return onUncheck;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            updateTextureSizes();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "textcolordisabled")
              || (property == "textcolorchecked") || (property == "textcolorcheckedhover") || (property == "textcolorcheckeddisabled"))
        {
            updateTextColor();
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textstylechecked")
        {
            m_textStyleCheckedCached = getSharedRenderer()->getTextStyleChecked();

            if (m_checked && m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);
        }
        else if (property == "textureunchecked")
        {
            m_spriteUnchecked.setTexture(getSharedRenderer()->getTextureUnchecked());
            updateTextureSizes();
        }
        else if (property == "texturechecked")
        {
            m_spriteChecked.setTexture(getSharedRenderer()->getTextureChecked());
            updateTextureSizes();
        }
        else if (property == "textureuncheckedhover")
        {
            m_spriteUncheckedHover.setTexture(getSharedRenderer()->getTextureUncheckedHover());
        }
        else if (property == "texturecheckedhover")
        {
            m_spriteCheckedHover.setTexture(getSharedRenderer()->getTextureCheckedHover());
        }
        else if (property == "textureuncheckeddisabled")
        {
            m_spriteUncheckedDisabled.setTexture(getSharedRenderer()->getTextureUncheckedDisabled());
        }
        else if (property == "texturecheckeddisabled")
        {
            m_spriteCheckedDisabled.setTexture(getSharedRenderer()->getTextureCheckedDisabled());
        }
        else if (property == "textureuncheckedfocused")
        {
            m_spriteUncheckedFocused.setTexture(getSharedRenderer()->getTextureUncheckedFocused());
        }
        else if (property == "texturecheckedfocused")
        {
            m_spriteCheckedFocused.setTexture(getSharedRenderer()->getTextureCheckedFocused());
        }
        else if (property == "checkcolor")
        {
            m_checkColorCached = getSharedRenderer()->getCheckColor();
        }
        else if (property == "checkcolorhover")
        {
            m_checkColorHoverCached = getSharedRenderer()->getCheckColorHover();
        }
        else if (property == "checkcolordisabled")
        {
            m_checkColorDisabledCached = getSharedRenderer()->getCheckColorDisabled();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == "bordercolorfocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == "bordercolorchecked")
        {
            m_borderColorCheckedCached = getSharedRenderer()->getBorderColorChecked();
        }
        else if (property == "bordercolorcheckedhover")
        {
            m_borderColorCheckedHoverCached = getSharedRenderer()->getBorderColorCheckedHover();
        }
        else if (property == "bordercolorcheckeddisabled")
        {
            m_borderColorCheckedDisabledCached = getSharedRenderer()->getBorderColorCheckedDisabled();
        }
        else if (property == "bordercolorcheckedfocused")
        {
            m_borderColorCheckedFocusedCached = getSharedRenderer()->getBorderColorCheckedFocused();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "backgroundcolorchecked")
        {
            m_backgroundColorCheckedCached = getSharedRenderer()->getBackgroundColorChecked();
        }
        else if (property == "backgroundcolorcheckedhover")
        {
            m_backgroundColorCheckedHoverCached = getSharedRenderer()->getBackgroundColorCheckedHover();
        }
        else if (property == "backgroundcolorcheckeddisabled")
        {
            m_backgroundColorCheckedDisabledCached = getSharedRenderer()->getBackgroundColorCheckedDisabled();
        }
        else if (property == "textdistanceratio")
        {
            m_textDistanceRatioCached = getSharedRenderer()->getTextDistanceRatio();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteUnchecked.setOpacity(m_opacityCached);
            m_spriteChecked.setOpacity(m_opacityCached);
            m_spriteUncheckedHover.setOpacity(m_opacityCached);
            m_spriteCheckedHover.setOpacity(m_opacityCached);
            m_spriteUncheckedDisabled.setOpacity(m_opacityCached);
            m_spriteCheckedDisabled.setOpacity(m_opacityCached);
            m_spriteUncheckedFocused.setOpacity(m_opacityCached);
            m_spriteCheckedFocused.setOpacity(m_opacityCached);

            m_text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_text.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> RadioButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));
        if (m_checked)
            node->propertyValuePairs["Checked"] = std::make_unique<DataIO::ValueNode>("true");
        if (!isTextClickable())
            node->propertyValuePairs["TextClickable"] = std::make_unique<DataIO::ValueNode>("false");

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["textclickable"])
            setTextClickable(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["textclickable"]->value).getBool());
        if (node->propertyValuePairs["checked"])
            setChecked(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["checked"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RadioButton::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentCheckColor() const
    {
        if (!m_enabled && m_checkColorDisabledCached.isSet())
            return m_checkColorDisabledCached;
        else if (m_mouseHover && m_checkColorHoverCached.isSet())
            return m_checkColorHoverCached;
        else
            return m_checkColorCached;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentBackgroundColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && m_backgroundColorCheckedDisabledCached.isSet())
                return m_backgroundColorCheckedDisabledCached;
            else if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                return m_backgroundColorDisabledCached;
            else if (m_mouseHover)
            {
                if (m_backgroundColorCheckedHoverCached.isSet())
                    return m_backgroundColorCheckedHoverCached;
                else if (m_backgroundColorCheckedCached.isSet())
                    return m_backgroundColorCheckedCached;
                else if (m_backgroundColorHoverCached.isSet())
                    return m_backgroundColorHoverCached;
                else
                    return m_backgroundColorCached;
            }
            else
            {
                if (m_backgroundColorCheckedCached.isSet())
                    return m_backgroundColorCheckedCached;
                else
                    return m_backgroundColorCached;
            }
        }
        else
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                return m_backgroundColorDisabledCached;
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                return m_backgroundColorHoverCached;
            else
                return m_backgroundColorCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& RadioButton::getCurrentBorderColor() const
    {
        if (m_checked)
        {
            if (!m_enabled && m_borderColorCheckedDisabledCached.isSet())
                return m_borderColorCheckedDisabledCached;
            else if (!m_enabled && m_borderColorDisabledCached.isSet())
                return m_borderColorDisabledCached;
            else if (m_mouseHover)
            {
                if (m_borderColorCheckedHoverCached.isSet())
                    return m_borderColorCheckedHoverCached;
                else if (m_borderColorCheckedCached.isSet())
                    return m_borderColorCheckedCached;
                else if (m_focused && m_borderColorCheckedFocusedCached.isSet())
                    return m_borderColorCheckedFocusedCached;
                else if (m_borderColorHoverCached.isSet())
                    return m_borderColorHoverCached;
                else if (m_focused && m_borderColorFocusedCached.isSet())
                    return m_borderColorFocusedCached;
                else
                    return m_borderColorCached;
            }
            else
            {
                if (m_focused && m_borderColorCheckedFocusedCached.isSet())
                    return m_borderColorCheckedFocusedCached;
                else if (m_borderColorCheckedCached.isSet())
                    return m_borderColorCheckedCached;
                else if (m_focused && m_borderColorFocusedCached.isSet())
                    return m_borderColorFocusedCached;
                else
                    return m_borderColorCached;
            }
        }
        else
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                return m_borderColorDisabledCached;
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                return m_borderColorHoverCached;
            else if (m_focused && m_borderColorFocusedCached.isSet())
                return m_borderColorFocusedCached;
            else
                return m_borderColorCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextureSizes()
    {
        m_spriteUnchecked.setSize(getInnerSize());
        m_spriteChecked.setSize(getInnerSize());
        m_spriteUncheckedHover.setSize(getInnerSize());
        m_spriteCheckedHover.setSize(getInnerSize());
        m_spriteUncheckedDisabled.setSize(getInnerSize());
        m_spriteCheckedDisabled.setSize(getInnerSize());
        m_spriteUncheckedFocused.setSize(getInnerSize());
        m_spriteCheckedFocused.setSize(getInnerSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::updateTextColor()
    {
        if (m_checked)
        {
            if (!m_enabled && getSharedRenderer()->getTextColorCheckedDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorCheckedDisabled());
            else if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorDisabled());
            else if (m_mouseHover)
            {
                if (getSharedRenderer()->getTextColorCheckedHover().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorCheckedHover());
                else if (getSharedRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorChecked());
                else if (getSharedRenderer()->getTextColorHover().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorHover());
                else
                    m_text.setColor(getSharedRenderer()->getTextColor());
            }
            else
            {
                if (getSharedRenderer()->getTextColorChecked().isSet())
                    m_text.setColor(getSharedRenderer()->getTextColorChecked());
                else
                    m_text.setColor(getSharedRenderer()->getTextColor());
            }
        }
        else
        {
            if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorDisabled());
            else if (m_mouseHover && getSharedRenderer()->getTextColorHover().isSet())
                m_text.setColor(getSharedRenderer()->getTextColorHover());
            else
                m_text.setColor(getSharedRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        const float innerRadius = std::min(getInnerSize().x, getInnerSize().y) / 2;
        if (m_bordersCached != Borders{0})
        {
            sf::CircleShape circle{innerRadius + m_bordersCached.getLeft()};
            circle.setOutlineThickness(-m_bordersCached.getLeft());
            circle.setFillColor(Color::Transparent);
            circle.setOutlineColor(Color::calcColorOpacity(getCurrentBorderColor(), m_opacityCached));
            target.draw(circle, states);
        }

        // Draw the box
        states.transform.translate(m_bordersCached.getOffset());
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            if (m_checked)
            {
                if (!m_enabled && m_spriteCheckedDisabled.isSet())
                    m_spriteCheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteCheckedHover.isSet())
                    m_spriteCheckedHover.draw(target, states);
                else if (m_focused && m_spriteCheckedFocused.isSet())
                    m_spriteCheckedFocused.draw(target, states);
                else
                    m_spriteChecked.draw(target, states);
            }
            else
            {
                if (!m_enabled && m_spriteUncheckedDisabled.isSet())
                    m_spriteUncheckedDisabled.draw(target, states);
                else if (m_mouseHover && m_spriteUncheckedHover.isSet())
                    m_spriteUncheckedHover.draw(target, states);
                else if (m_focused && m_spriteUncheckedFocused.isSet())
                    m_spriteUncheckedFocused.draw(target, states);
                else
                    m_spriteUnchecked.draw(target, states);
            }
        }
        else // There are no images
        {
            sf::CircleShape circle{innerRadius};
            circle.setFillColor(Color::calcColorOpacity(getCurrentBackgroundColor(), m_opacityCached));
            target.draw(circle, states);

            // Draw the check if the radio button is checked
            if (m_checked)
            {
                sf::CircleShape checkShape{innerRadius * 0.4f};
                checkShape.setFillColor(Color::calcColorOpacity(getCurrentCheckColor(), m_opacityCached));
                checkShape.setPosition({innerRadius - checkShape.getRadius(), innerRadius - checkShape.getRadius()});
                target.draw(checkShape, states);
            }
        }
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getLeft()});

        if (!getText().isEmpty())
        {
            states.transform.translate({(1 + m_textDistanceRatioCached) * getSize().x, (getSize().y - m_text.getSize().y) / 2.0f});
            m_text.draw(target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/RangeSlider.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::RangeSlider()
    {
        m_type = "RangeSlider";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<RangeSliderRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::Ptr RangeSlider::create(float minimum, float maximum)
    {
        auto slider = std::make_shared<RangeSlider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSlider::Ptr RangeSlider::copy(RangeSlider::ConstPtr slider)
    {
        if (slider)
            return std::static_pointer_cast<RangeSlider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSliderRenderer* RangeSlider::getSharedRenderer()
    {
        return aurora::downcast<RangeSliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RangeSliderRenderer* RangeSlider::getSharedRenderer() const
    {
        return aurora::downcast<const RangeSliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RangeSliderRenderer* RangeSlider::getRenderer()
    {
        return aurora::downcast<RangeSliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const RangeSliderRenderer* RangeSlider::getRenderer() const
    {
        return aurora::downcast<const RangeSliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        if (getSize().x < getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            float scaleFactor;
            if (m_verticalImage == m_verticalScroll)
            {
                m_spriteTrack.setSize(getInnerSize());
                m_spriteTrackHover.setSize(getInnerSize());

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().x;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().y;
            }
            else // The image is rotated
            {
                m_spriteTrack.setSize({getInnerSize().y, getInnerSize().x});
                m_spriteTrackHover.setSize({getInnerSize().y, getInnerSize().x});

                if (m_verticalScroll)
                    scaleFactor = getInnerSize().x / m_spriteTrack.getTexture().getImageSize().y;
                else
                    scaleFactor = getInnerSize().y / m_spriteTrack.getTexture().getImageSize().x;
            }

            m_thumbs.first.width = scaleFactor * m_spriteThumb.getTexture().getImageSize().x;
            m_thumbs.first.height = scaleFactor * m_spriteThumb.getTexture().getImageSize().y;

            m_spriteThumb.setSize({m_thumbs.first.width, m_thumbs.first.height});
            m_spriteThumbHover.setSize({m_thumbs.first.width, m_thumbs.first.height});

            // Apply the rotation now that the size has been set
            if (m_verticalScroll != m_verticalImage)
            {
                m_spriteTrack.setRotation(-90);
                m_spriteTrackHover.setRotation(-90);
                m_spriteThumb.setRotation(-90);
                m_spriteThumbHover.setRotation(-90);
            }
            else
            {
                m_spriteTrack.setRotation(0);
                m_spriteTrackHover.setRotation(0);
                m_spriteThumb.setRotation(0);
                m_spriteThumbHover.setRotation(0);
            }
        }
        else // There are no textures
        {
            if (m_verticalScroll)
            {
                m_thumbs.first.width = getSize().x * 1.6f;
                m_thumbs.first.height = m_thumbs.first.width / 2.0f;
            }
            else
            {
                m_thumbs.first.height = getSize().y * 1.6f;
                m_thumbs.first.width = m_thumbs.first.height / 2.0f;
            }
        }

        m_thumbs.second.width = m_thumbs.first.width;
        m_thumbs.second.height = m_thumbs.first.height;

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getFullSize() const
    {
        if (m_verticalScroll)
            return {std::max(getSize().x, m_thumbs.first.width), getSize().y + m_thumbs.first.height};
        else
            return {getSize().x + m_thumbs.first.width, std::max(getSize().y, m_thumbs.first.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getWidgetOffset() const
    {
        if (m_verticalScroll)
            return {std::min(0.f, getSize().x - m_thumbs.first.width), -m_thumbs.first.height / 2.f};
        else
            return {-m_thumbs.first.width / 2.f, std::min(0.f, getSize().y - m_thumbs.first.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setMinimum(float minimum)
    {
        const auto oldMinimum = m_minimum;

        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the selection start equaled the minimum, move it as well, otherwise set it again to make sure it is still within minimum and maximum
        if (oldMinimum == m_selectionStart)
            setSelectionStart(m_minimum);
        else
            setSelectionStart(m_selectionStart);

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setMaximum(float maximum)
    {
        const auto oldMaximum = m_maximum;

        // Set the new maximum
        m_maximum = maximum;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the selection end equaled the maximum, move it as well, otherwise set it again to make sure it is still within minimum and maximum
        if (oldMaximum == m_selectionStart)
            setSelectionEnd(m_maximum);
        else
            setSelectionEnd(m_selectionEnd);

        updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSelectionStart(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_selectionStart != value)
        {
            m_selectionStart = value;

            // Update the selection end when the selection start passed it
            if (m_selectionEnd < value)
                m_selectionEnd = value;

            onRangeChange.emit(this, m_selectionStart, m_selectionEnd);

            updateThumbPositions();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getSelectionStart() const
    {
        return m_selectionStart;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setSelectionEnd(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_selectionEnd != value)
        {
            m_selectionEnd = value;

            // Update the selection start when the selection end passed it
            if (m_selectionStart > value)
                m_selectionStart = value;

            onRangeChange.emit(this, m_selectionStart, m_selectionEnd);

            updateThumbPositions();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getSelectionEnd() const
    {
        return m_selectionEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::setStep(float step)
    {
        m_step = step;

        // Reset the values in case it does not match the step
        setSelectionStart(m_selectionStart);
        setSelectionEnd(m_selectionEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float RangeSlider::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool RangeSlider::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the thumbs
        if (FloatRect(m_thumbs.second.left, m_thumbs.second.top, m_thumbs.second.width, m_thumbs.second.height).contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteThumb.isTransparentPixel(pos - m_thumbs.first.getPosition()))
                return true;
        }
        if (FloatRect(m_thumbs.first.left, m_thumbs.first.top, m_thumbs.first.width, m_thumbs.first.height).contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteThumb.isTransparentPixel(pos - m_thumbs.second.getPosition()))
                return true;
        }

        // Check if the mouse is on top of the track
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteTrack.isTransparentPixel(pos - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        // Refresh the value
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::leftMouseReleased(Vector2f)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse button is down
        if (m_mouseDown && (m_mouseDownOnThumb != 0))
        {
            // Check in which direction the slider goes
            if (m_verticalScroll)
            {
                if (m_mouseDownOnThumb == 1)
                {
                    setSelectionStart(m_maximum - (((pos.y + (m_thumbs.first.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum)));

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop + (m_thumbs.first.height / 2.0f) > 0) && (thumbTop + (m_thumbs.first.height / 2.0f) < getSize().y))
                        m_thumbs.first.top = thumbTop;
                    else
                        m_thumbs.first.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_selectionStart)) - (m_thumbs.first.height / 2.0f);
                }
                else // if (m_mouseDownOnThumb == 2)
                {
                    setSelectionEnd(m_maximum - (((pos.y + (m_thumbs.second.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum)));

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop + (m_thumbs.second.height / 2.0f) > 0) && (thumbTop + (m_thumbs.second.height / 2.0f) < getSize().y))
                        m_thumbs.second.top = thumbTop;
                    else
                        m_thumbs.second.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_selectionEnd)) - (m_thumbs.second.height / 2.0f);
                }
            }
            else // the slider lies horizontal
            {
                if (m_mouseDownOnThumb == 1)
                {
                    setSelectionStart((((pos.x + (m_thumbs.first.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft + (m_thumbs.first.width / 2.0f) > 0) && (thumbLeft + (m_thumbs.first.width / 2.0f) < getSize().x))
                        m_thumbs.first.left = thumbLeft;
                    else
                        m_thumbs.first.left = (getSize().x / (m_maximum - m_minimum) * (m_selectionStart - m_minimum)) - (m_thumbs.first.width / 2.0f);
                }
                else // if (m_mouseDownOnThumb == 2)
                {
                    setSelectionEnd((((pos.x + (m_thumbs.second.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft + (m_thumbs.second.width / 2.0f) > 0) && (thumbLeft + (m_thumbs.second.width / 2.0f) < getSize().x))
                        m_thumbs.second.left = thumbLeft;
                    else
                        m_thumbs.second.left = (getSize().x / (m_maximum - m_minimum) * (m_selectionEnd - m_minimum)) - (m_thumbs.second.width / 2.0f);
                }
            }
        }
        else // Normal mouse move
        {
            // Set some variables so that when the mouse goes down we know whether it is on the track or not
            if (FloatRect(m_thumbs.second.left, m_thumbs.second.top, m_thumbs.second.width, m_thumbs.second.height).contains(pos))
            {
                m_mouseDownOnThumb = 2;
                m_mouseDownOnThumbPos.x = pos.x - m_thumbs.second.left;
                m_mouseDownOnThumbPos.y = pos.y - m_thumbs.second.top;
            }
            else if (FloatRect(m_thumbs.first.left, m_thumbs.first.top, m_thumbs.first.width, m_thumbs.first.height).contains(pos))
            {
                m_mouseDownOnThumb = 1;
                m_mouseDownOnThumbPos.x = pos.x - m_thumbs.first.left;
                m_mouseDownOnThumbPos.y = pos.y - m_thumbs.first.top;
            }
            else // The mouse is not on top of the thumb
                m_mouseDownOnThumb = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPositions();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& RangeSlider::getSignal(std::string signalName)
    {
        if (signalName == toLower(onRangeChange.getName()))
            return onRangeChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "texturetrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());

            if (m_spriteTrack.getTexture().getImageSize().x < m_spriteTrack.getTexture().getImageSize().y)
                m_verticalImage = true;
            else
                m_verticalImage = false;

            setSize(m_size);
        }
        else if (property == "texturetrackhover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == "texturethumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            setSize(m_size);
        }
        else if (property == "texturethumbhover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
        }
        else if (property == "trackcolor")
        {
            m_trackColorCached = getSharedRenderer()->getTrackColor();
        }
        else if (property == "trackcolorhover")
        {
            m_trackColorHoverCached = getSharedRenderer()->getTrackColorHover();
        }
        else if (property == "selectedtrackcolor")
        {
            m_selectedTrackColorCached = getSharedRenderer()->getSelectedTrackColor();
        }
        else if (property == "selectedtrackcolorhover")
        {
            m_selectedTrackColorHoverCached = getSharedRenderer()->getSelectedTrackColorHover();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "thumbcolorhover")
        {
            m_thumbColorHoverCached = getSharedRenderer()->getThumbColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTrack.setOpacity(m_opacityCached);
            m_spriteTrackHover.setOpacity(m_opacityCached);
            m_spriteThumb.setOpacity(m_opacityCached);
            m_spriteThumbHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> RangeSlider::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["SelectionStart"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectionStart));
        node->propertyValuePairs["SelectionEnd"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectionEnd));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(to_string(m_step));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stof(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stof(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["selectionstart"])
            setSelectionStart(tgui::stof(node->propertyValuePairs["selectionstart"]->value));
        if (node->propertyValuePairs["selectionend"])
            setSelectionEnd(tgui::stof(node->propertyValuePairs["selectionend"]->value));
        if (node->propertyValuePairs["step"])
            setStep(tgui::stof(node->propertyValuePairs["step"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f RangeSlider::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::updateThumbPositions()
    {
        const Vector2f innerSize = getInnerSize();

        if (m_verticalScroll)
        {
            m_thumbs.first.left = m_bordersCached.getLeft() + (innerSize.x - m_thumbs.first.width) / 2.0f;
            m_thumbs.first.top = (innerSize.y / (m_maximum - m_minimum) * (m_maximum - m_selectionStart)) - (m_thumbs.first.height / 2.0f);

            m_thumbs.second.left = m_bordersCached.getLeft() + (innerSize.x - m_thumbs.second.width) / 2.0f;
            m_thumbs.second.top = (innerSize.y / (m_maximum - m_minimum) * (m_maximum - m_selectionEnd)) - (m_thumbs.second.height / 2.0f);
        }
        else // horizontal
        {
            m_thumbs.first.left = (innerSize.x / (m_maximum - m_minimum) * (m_selectionStart - m_minimum)) - (m_thumbs.first.width / 2.0f);
            m_thumbs.first.top = m_bordersCached.getTop() + (innerSize.y - m_thumbs.first.height) / 2.0f;

            m_thumbs.second.left = (innerSize.x / (m_maximum - m_minimum) * (m_selectionEnd - m_minimum)) - (m_thumbs.second.width / 2.0f);
            m_thumbs.second.top = m_bordersCached.getTop() + (innerSize.y - m_thumbs.second.height) / 2.0f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RangeSlider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders around the track
        if (m_bordersCached != Borders{0})
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the track
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
        {
            if (m_mouseHover && m_spriteTrackHover.isSet())
                m_spriteTrackHover.draw(target, states);
            else
                m_spriteTrack.draw(target, states);
        }
        else // There are no textures
        {
            if (m_mouseHover && m_trackColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_trackColorHoverCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_trackColorCached);

            if (m_selectedTrackColorCached.isSet())
            {
                sf::RenderStates selectedTrackStates = states;
                Vector2f size;

                if (m_verticalScroll)
                {
                    selectedTrackStates.transform.translate({0, m_thumbs.first.top + m_thumbs.first.height / 2.f});
                    size = {getInnerSize().x, m_thumbs.second.top - m_thumbs.first.top};
                }
                else
                {
                    selectedTrackStates.transform.translate({m_thumbs.first.left + m_thumbs.first.width / 2.f, 0});
                    size = {m_thumbs.second.left - m_thumbs.first.left, getInnerSize().y};
                }

                if (m_mouseHover && m_selectedTrackColorHoverCached.isSet())
                    drawRectangleShape(target, selectedTrackStates, size, m_selectedTrackColorHoverCached);
                else
                    drawRectangleShape(target, selectedTrackStates, size, m_selectedTrackColorCached);
            }
        }

        const auto oldStates = states;

        // Draw the first thumb
        {
            states.transform.translate({-m_bordersCached.getLeft() + m_thumbs.first.left, -m_bordersCached.getTop() + m_thumbs.first.top});

            // Draw the borders around the thumb when using colors
            if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
            {
                if (m_mouseHover && m_borderColorHoverCached.isSet())
                    drawBorders(target, states, m_bordersCached, {m_thumbs.first.width, m_thumbs.first.height}, m_borderColorHoverCached);
                else
                    drawBorders(target, states, m_bordersCached, {m_thumbs.first.width, m_thumbs.first.height}, m_borderColorCached);

                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the thumb
            if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
            {
                if (m_mouseHover && m_spriteThumbHover.isSet())
                    m_spriteThumbHover.draw(target, states);
                else
                    m_spriteThumb.draw(target, states);
            }
            else // There are no textures
            {
                const Vector2f thumbInnerSize = {m_thumbs.first.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                                     m_thumbs.first.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

                if (m_mouseHover && m_thumbColorHoverCached.isSet())
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
                else
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
            }
        }

        states = oldStates;

        // Draw the second thumb
        {
            states.transform.translate({-m_bordersCached.getLeft() + m_thumbs.second.left, -m_bordersCached.getTop() + m_thumbs.second.top});

            // Draw the borders around the thumb when using colors
            if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
            {
                if (m_mouseHover && m_borderColorHoverCached.isSet())
                    drawBorders(target, states, m_bordersCached, {m_thumbs.second.width, m_thumbs.second.height}, m_borderColorHoverCached);
                else
                    drawBorders(target, states, m_bordersCached, {m_thumbs.second.width, m_thumbs.second.height}, m_borderColorCached);

                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the thumb
            if (m_spriteTrack.isSet() && m_spriteThumb.isSet())
            {
                if (m_mouseHover && m_spriteThumbHover.isSet())
                    m_spriteThumbHover.draw(target, states);
                else
                    m_spriteThumb.draw(target, states);
            }
            else // There are no textures
            {
                const Vector2f thumbInnerSize = {m_thumbs.second.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                                     m_thumbs.second.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

                if (m_mouseHover && m_thumbColorHoverCached.isSet())
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
                else
                    drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Scrollbar.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Scrollbar()
    {
        m_type = "Scrollbar";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ScrollbarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(getDefaultWidth(), 160);
        m_sizeSet = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::create()
    {
        return std::make_shared<Scrollbar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr Scrollbar::copy(Scrollbar::ConstPtr scrollbar)
    {
        if (scrollbar)
            return std::static_pointer_cast<Scrollbar>(scrollbar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollbarRenderer* Scrollbar::getSharedRenderer()
    {
        return aurora::downcast<ScrollbarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollbarRenderer* Scrollbar::getSharedRenderer() const
    {
        return aurora::downcast<const ScrollbarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollbarRenderer* Scrollbar::getRenderer()
    {
        return aurora::downcast<ScrollbarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollbarRenderer* Scrollbar::getRenderer() const
    {
        return aurora::downcast<const ScrollbarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_sizeSet = true;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        if (maximum > 0)
            m_maximum = maximum;
        else
            m_maximum = 1;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_viewportSize)
            setValue(0);
        else if (m_value > m_maximum - m_viewportSize)
            setValue(m_maximum - m_viewportSize);

        // Recalculate the size and position of the thumb image
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setValue(unsigned int value)
    {
        // When the value is above the maximum then adjust it
        if (m_maximum < m_viewportSize)
            value = 0;
        else if (value > m_maximum - m_viewportSize)
            value = m_maximum - m_viewportSize;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

            // Recalculate the size and position of the thumb image
            updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setViewportSize(unsigned int viewportSize)
    {
        // Set the new value
        m_viewportSize = viewportSize;

        // When the value is above the maximum then adjust it
        if (m_maximum < m_viewportSize)
            setValue(0);
        else if (m_value > m_maximum - m_viewportSize)
            setValue(m_maximum - m_viewportSize);

        // Recalculate the size and position of the thumb image
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getViewportSize() const
    {
        return m_viewportSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setScrollAmount(unsigned int scrollAmount)
    {
        m_scrollAmount = scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Scrollbar::getScrollAmount() const
    {
        return m_scrollAmount;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::setAutoHide(bool autoHide)
    {
        m_autoHide = autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::getAutoHide() const
    {
        return m_autoHide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Scrollbar::getDefaultWidth()
    {
        if (m_spriteTrack.isSet())
            return m_spriteTrack.getTexture().getImageSize().x;
        else
            return 16;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseOnWidget(Vector2f pos) const
    {
        // Don't make any calculations when no scrollbar is needed
        if (m_autoHide && (m_maximum <= m_viewportSize))
            return false;

        pos -= getPosition();
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached)
                return true;

            if (!m_spriteArrowUp.isTransparentPixel(pos))
                return true;
            if (!m_spriteArrowDown.isTransparentPixel(pos - m_arrowDown.getPosition()))
                return true;
            if (!m_spriteTrack.isTransparentPixel(pos - m_track.getPosition()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;
        m_mouseDownOnArrow = false;

        if (m_verticalScroll)
        {
            // Check if the arrows are drawn at full size
            if (getSize().y > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if ((pos.y < m_arrowUp.height) || (pos.y >= getSize().y - m_arrowUp.height))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }
        else
        {
            // Check if the arrows are drawn at full size
            if (getSize().x > m_arrowUp.height + m_arrowDown.height)
            {
                // Check if you clicked on one of the arrows
                if ((pos.x < m_arrowUp.height) || (pos.x >= getSize().x - m_arrowUp.height))
                    m_mouseDownOnArrow = true;
            }
            else // The arrows are not drawn at full size (there is no track)
                m_mouseDownOnArrow = true;
        }

        // Check if the mouse is on top of the thumb
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
        {
            m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
            m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;

            m_mouseDownOnThumb = true;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the scrollbar value
        if (!m_mouseDownOnArrow)
            mouseMoved(pos + getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::leftMouseReleased(Vector2f pos)
    {
        // Check if one of the arrows was clicked
        if (m_mouseDown && m_mouseDownOnArrow)
        {
            // Only continue when the calculations can be made
            if (m_maximum > m_viewportSize)
            {
                bool valueDown = false;
                bool valueUp = false;

                pos -= getPosition();

                // Check in which direction the scrollbar lies
                if (m_verticalScroll)
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().y > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (pos.y < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (pos.y >= getSize().y - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (pos.y < getSize().y * 0.5f)
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }
                else // the scrollbar lies horizontal
                {
                    // Check if the arrows are drawn at full size
                    if (getSize().x > m_arrowUp.height + m_arrowDown.height)
                    {
                        // Check if you clicked on the top arrow
                        if (pos.x < m_arrowUp.height)
                            valueDown = true;

                        // Check if you clicked the down arrow
                        else if (pos.x >= getSize().x - m_arrowUp.height)
                            valueUp = true;
                    }
                    else // The arrows are not drawn at full size
                    {
                        // Check on which arrow you clicked
                        if (pos.x < getSize().x * 0.5f)
                            valueDown = true;
                        else // You clicked on the bottom arrow
                            valueUp = true;
                    }
                }

                if (valueDown)
                {
                    if (m_value > m_scrollAmount)
                    {
                        if (m_value % m_scrollAmount)
                            setValue(m_value - (m_value % m_scrollAmount));
                        else
                            setValue(m_value - m_scrollAmount);
                    }
                    else
                        setValue(0);
                }
                else if (valueUp)
                {
                    if (m_value + m_scrollAmount < m_maximum - m_viewportSize + 1)
                    {
                        if (m_value % m_scrollAmount)
                            setValue(m_value + (m_scrollAmount - (m_value % m_scrollAmount)));
                        else
                            setValue(m_value + m_scrollAmount);
                    }
                    else
                        setValue(m_maximum - m_viewportSize);
                }
            }
        }

        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse button went down on top of the track (or thumb)
        if (m_mouseDown && !m_mouseDownOnArrow)
        {
            // Don't continue if the calculations can't be made
            if (!m_autoHide && (m_maximum <= m_viewportSize))
                return;

            // Check in which direction the scrollbar lies
            if (m_verticalScroll)
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if ((pos.y - m_mouseDownOnThumbPos.y - m_arrowUp.height) > 0)
                    {
                        // Calculate the new value
                        const unsigned int value = static_cast<unsigned int>((((pos.y - m_mouseDownOnThumbPos.y - m_arrowUp.height)
                            / (getSize().y - m_arrowUp.height - m_arrowDown.height - m_thumb.height)) * (m_maximum - m_viewportSize)) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_viewportSize))
                            setValue(value);
                        else
                            setValue(m_maximum - m_viewportSize);
                    }
                    else // The mouse was above the scrollbar
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
                    if ((thumbTop - m_arrowUp.height > 0) && (thumbTop + m_thumb.height + m_arrowDown.height < getSize().y))
                        m_thumb.top = thumbTop;
                    else // Prevent the thumb from going outside the scrollbar
                        m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_viewportSize));
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (pos.y >= m_arrowUp.height)
                    {
                        // Make sure that you did not click on the down arrow
                        if (pos.y < getSize().y - m_arrowUp.height)
                        {
                            // Calculate the exact position (a number between 0 and maximum), as if the top of the thumb will be where you clicked
                            const float scaleFactor = (m_maximum - m_viewportSize) / (getSize().y - m_arrowUp.height - m_arrowDown.height - m_thumb.height);
                            const float value = (pos.y - m_arrowUp.height) * scaleFactor;

                            // Check if you clicked above the thumb
                            if (value <= m_value)
                            {
                                // Try to place the thumb on 2/3 of the clicked position
                                const float subtractValue = (m_thumb.height / 3.0f) * scaleFactor;
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred below the thumb
                            {
                                // Try to place the thumb on 2/3 of the clicked position
                                const float subtractValue = (m_thumb.height * 2.0f / 3.0f) * scaleFactor;
                                if (value <= (m_maximum - m_viewportSize + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_viewportSize);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
            else // the scrollbar lies horizontal
            {
                // Check if the thumb is being dragged
                if (m_mouseDownOnThumb)
                {
                    // Set the new value
                    if (pos.x - m_mouseDownOnThumbPos.x - m_arrowUp.width > 0)
                    {
                        // Calculate the new value
                        const unsigned int value = static_cast<unsigned int>((((pos.x - m_mouseDownOnThumbPos.x - m_arrowUp.width)
                            / (getSize().x - m_arrowUp.width - m_arrowDown.width - m_thumb.width)) * (m_maximum - m_viewportSize)) + 0.5f);

                        // If the value isn't too high then change it
                        if (value <= (m_maximum - m_viewportSize))
                            setValue(value);
                        else
                            setValue(m_maximum - m_viewportSize);
                    }
                    else // The mouse was to the left of the thumb
                        setValue(0);

                    // Set the thumb position for smooth scrolling
                    const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
                    if ((thumbLeft - m_arrowUp.width > 0) && (thumbLeft + m_thumb.width + m_arrowDown.width < getSize().x))
                        m_thumb.left = thumbLeft;
                    else // Prevent the thumb from going outside the scrollbar
                        m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_viewportSize));
                }
                else // The click occurred on the track
                {
                    // If the position is positive then calculate the correct value
                    if (pos.x >= m_arrowUp.width)
                    {
                        // Make sure that you did not click on the down arrow
                        if (pos.x < getSize().x - m_arrowUp.width)
                        {
                            // Calculate the exact position (a number between 0 and maximum), as if the left of the thumb will be where you clicked
                            const float scaleFactor = (m_maximum - m_viewportSize) / (getSize().x - m_arrowUp.width - m_arrowDown.width - m_thumb.width);
                            const float value = (pos.x - m_arrowUp.width) * scaleFactor;

                            // Check if you clicked to the left of the thumb
                            if (value <= m_value)
                            {
                                const float subtractValue = (m_thumb.width / 3.0f) * scaleFactor;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value >= subtractValue)
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(0);
                            }
                            else // The click occurred to the right of the thumb
                            {
                                const float subtractValue = (m_thumb.width * 2.0f / 3.0f) * scaleFactor;

                                // Try to place the thumb on 2/3 of the clicked position
                                if (value <= (m_maximum - m_viewportSize + subtractValue))
                                    setValue(static_cast<unsigned int>(value - subtractValue + 0.5f));
                                else
                                    setValue(m_maximum - m_viewportSize);
                            }
                        }
                    }

                    m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                    m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
                    m_mouseDownOnThumb = true;
                }
            }
        }

        if (FloatRect{m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height}.contains(pos))
            m_mouseHoverOverPart = Part::Thumb;
        else if (FloatRect{m_track.left, m_track.top, m_track.width, m_track.height}.contains(pos))
            m_mouseHoverOverPart = Part::Track;
        else if (FloatRect{m_arrowUp.left, m_arrowUp.top, m_arrowUp.width, m_arrowUp.height}.contains(pos))
            m_mouseHoverOverPart = Part::ArrowUp;
        else if (FloatRect{m_arrowDown.left, m_arrowDown.top, m_arrowDown.width, m_arrowDown.height}.contains(pos))
            m_mouseHoverOverPart = Part::ArrowDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Scrollbar::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (static_cast<int>(m_value) - static_cast<int>(delta * m_scrollAmount) < 0)
            setValue(0);
        else
            setValue(static_cast<unsigned int>(m_value - (delta * m_scrollAmount)));

        // Update over which part the mouse is hovering
        mouseMoved(pos - getPosition());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::updateSize()
    {
        if (getSize().x <= getSize().y)
            m_verticalScroll = true;
        else
            m_verticalScroll = false;

        bool textured = false;
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet() && m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
            textured = true;

        if (m_verticalScroll)
        {
            m_arrowUp.width = getSize().x;
            m_arrowDown.width = getSize().x;

            if (textured)
            {
                m_arrowUp.height = getSize().x * m_spriteArrowUp.getTexture().getImageSize().x / m_spriteArrowUp.getTexture().getImageSize().y;
                m_arrowDown.height = getSize().x * m_spriteArrowDown.getTexture().getImageSize().x / m_spriteArrowDown.getTexture().getImageSize().y;
            }
            else
            {
                m_arrowUp.height = m_arrowUp.width;
                m_arrowDown.height = m_arrowUp.width;
            }

            m_track.width = getSize().x;
            m_track.height = std::max(0.f, getSize().y - m_arrowUp.height - m_arrowDown.height);

            m_thumb.width = getSize().x;
            if (m_maximum > m_viewportSize)
            {
                m_thumb.height = m_track.height * m_viewportSize / m_maximum;

                // Don't allow the thumb to become smaller than the scrollbar width, unless there isn't enough room for it
                if (m_thumb.height < getSize().x)
                {
                    if (getSize().x < m_track.height)
                        m_thumb.height = getSize().x;
                    else // Track is too small to contain minimum the thumb size, so instead we use 3/4th of track as size
                        m_thumb.height = m_track.height * 0.75f;
                }
            }
            else
                m_thumb.height = m_track.height;
        }
        else // The scrollbar lies horizontally
        {
            m_arrowUp.height = getSize().y;
            m_arrowDown.height = getSize().y;

            if (textured)
            {
                m_arrowUp.width = getSize().y * m_spriteArrowUp.getTexture().getImageSize().x / m_spriteArrowUp.getTexture().getImageSize().y;
                m_arrowDown.width = getSize().y * m_spriteArrowDown.getTexture().getImageSize().x / m_spriteArrowDown.getTexture().getImageSize().y;
            }
            else
            {
                m_arrowUp.width = m_arrowUp.height;
                m_arrowDown.width = m_arrowUp.height;
            }

            m_track.width = std::max(0.f, getSize().x - m_arrowUp.height - m_arrowDown.height);
            m_track.height = getSize().y;

            m_thumb.height = getSize().y;
            if (m_maximum > m_viewportSize)
            {
                m_thumb.width = m_track.width * m_viewportSize / m_maximum;

                // Don't allow the thumb to become smaller than the scrollbar width, unless there isn't enough room for it
                if (m_thumb.width < getSize().y)
                {
                    if (getSize().y < m_track.width)
                        m_thumb.width = getSize().y;
                    else // Track is too small to contain minimum the thumb size, so instead we use 3/4th of track as size
                        m_thumb.width = m_track.width * 0.75f;
                }
            }
            else
                m_thumb.width = m_track.width;
        }

        if (textured)
        {
            m_spriteArrowUp.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowUpHover.setSize({m_arrowUp.width, m_arrowUp.height});
            m_spriteArrowDown.setSize({m_arrowDown.width, m_arrowDown.height});
            m_spriteArrowDownHover.setSize({m_arrowDown.width, m_arrowDown.height});

            if (m_verticalScroll == m_verticalImage)
            {
                m_spriteTrack.setSize({m_track.width, m_track.height});
                m_spriteTrackHover.setSize({m_track.width, m_track.height});
                m_spriteThumb.setSize({m_thumb.width, m_thumb.height});
                m_spriteThumbHover.setSize({m_thumb.width, m_thumb.height});

                m_spriteTrack.setRotation(0);
                m_spriteTrackHover.setRotation(0);
                m_spriteThumb.setRotation(0);
                m_spriteThumbHover.setRotation(0);
            }
            else
            {
                m_spriteTrack.setSize({m_track.height, m_track.width});
                m_spriteTrackHover.setSize({m_track.height, m_track.width});
                m_spriteThumb.setSize({m_thumb.height, m_thumb.width});
                m_spriteThumbHover.setSize({m_thumb.height, m_thumb.width});

                m_spriteTrack.setRotation(-90);
                m_spriteTrackHover.setRotation(-90);
                m_spriteThumb.setRotation(-90);
                m_spriteThumbHover.setRotation(-90);
            }

            // Set the rotation or the arrows now that the size has been set
            if (m_verticalScroll)
            {
                m_spriteArrowUp.setRotation(0);
                m_spriteArrowUpHover.setRotation(0);
                m_spriteArrowDown.setRotation(0);
                m_spriteArrowDownHover.setRotation(0);
            }
            else
            {
                m_spriteArrowUp.setRotation(-90);
                m_spriteArrowUpHover.setRotation(-90);
                m_spriteArrowDown.setRotation(-90);
                m_spriteArrowDownHover.setRotation(-90);
            }
        }

        // Recalculate the position of the track, thumb and arrows
        if (m_verticalScroll)
        {
            m_arrowDown.left = 0;
            m_arrowDown.top = getSize().y - m_arrowDown.height;

            m_track.left = 0;
            m_track.top = m_arrowUp.height;
        }
        else
        {
            m_arrowDown.left = getSize().x - m_arrowDown.width;
            m_arrowDown.top = 0;

            m_track.left = m_arrowUp.width;
            m_track.top = 0;
        }

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Scrollbar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::rendererChanged(const std::string& property)
    {
        if (property == "texturetrack")
        {
            m_spriteTrack.setTexture(getSharedRenderer()->getTextureTrack());
            if (m_sizeSet)
                updateSize();
            else
            {
                if (m_verticalScroll)
                    setSize({getDefaultWidth(), getSize().y});
                else
                    setSize({getSize().x, getDefaultWidth()});

                m_sizeSet = false;
            }
        }
        else if (property == "texturetrackhover")
        {
            m_spriteTrackHover.setTexture(getSharedRenderer()->getTextureTrackHover());
        }
        else if (property == "texturethumb")
        {
            m_spriteThumb.setTexture(getSharedRenderer()->getTextureThumb());
            updateSize();
        }
        else if (property == "texturethumbhover")
        {
            m_spriteThumbHover.setTexture(getSharedRenderer()->getTextureThumbHover());
        }
        else if (property == "texturearrowup")
        {
            m_spriteArrowUp.setTexture(getSharedRenderer()->getTextureArrowUp());
            updateSize();
        }
        else if (property == "texturearrowuphover")
        {
            m_spriteArrowUpHover.setTexture(getSharedRenderer()->getTextureArrowUpHover());
        }
        else if (property == "texturearrowdown")
        {
            m_spriteArrowDown.setTexture(getSharedRenderer()->getTextureArrowDown());
            updateSize();
        }
        else if (property == "texturearrowdownhover")
        {
            m_spriteArrowDownHover.setTexture(getSharedRenderer()->getTextureArrowDownHover());
        }
        else if (property == "trackcolor")
        {
            m_trackColorCached = getSharedRenderer()->getTrackColor();
        }
        else if (property == "trackcolorhover")
        {
            m_trackColorHoverCached = getSharedRenderer()->getTrackColorHover();
        }
        else if (property == "thumbcolor")
        {
            m_thumbColorCached = getSharedRenderer()->getThumbColor();
        }
        else if (property == "thumbcolorhover")
        {
            m_thumbColorHoverCached = getSharedRenderer()->getThumbColorHover();
        }
        else if (property == "arrowbackgroundcolor")
        {
            m_arrowBackgroundColorCached = getSharedRenderer()->getArrowBackgroundColor();
        }
        else if (property == "arrowbackgroundcolorhover")
        {
            m_arrowBackgroundColorHoverCached = getSharedRenderer()->getArrowBackgroundColorHover();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTrack.setOpacity(m_opacityCached);
            m_spriteTrackHover.setOpacity(m_opacityCached);
            m_spriteThumb.setOpacity(m_opacityCached);
            m_spriteThumbHover.setOpacity(m_opacityCached);
            m_spriteArrowUp.setOpacity(m_opacityCached);
            m_spriteArrowUpHover.setOpacity(m_opacityCached);
            m_spriteArrowDown.setOpacity(m_opacityCached);
            m_spriteArrowDownHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Scrollbar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["AutoHide"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_autoHide));
        node->propertyValuePairs["ViewportSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_viewportSize));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["ScrollAmount"] = std::make_unique<DataIO::ValueNode>(to_string(m_scrollAmount));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["viewportsize"])
            setViewportSize(tgui::stoi(node->propertyValuePairs["viewportsize"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["scrollamount"])
            setScrollAmount(tgui::stoi(node->propertyValuePairs["scrollamount"]->value));
        if (node->propertyValuePairs["autohide"])
            setAutoHide(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autohide"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = 0;
            m_thumb.top = m_track.top + ((m_track.height - m_thumb.height) * m_value / (m_maximum - m_viewportSize));
        }
        else
        {
            m_thumb.left = m_track.left + ((m_track.width - m_thumb.width) * m_value / (m_maximum - m_viewportSize));
            m_thumb.top = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Scrollbar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw the scrollbar when it is not needed
        if (m_autoHide && (m_maximum <= m_viewportSize))
            return;

        states.transform.translate(getPosition());

        bool textured = false;
        if (m_spriteTrack.isSet() && m_spriteThumb.isSet() && m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
            textured = true;

        // Draw arrow up/left
        if (textured)
        {
            if (m_mouseHover && m_spriteArrowUpHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp))
                m_spriteArrowUpHover.draw(target, states);
            else
                m_spriteArrowUp.draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && m_arrowBackgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, {m_arrowUp.width, m_arrowUp.height}, m_arrowBackgroundColorHoverCached);
            else
                drawRectangleShape(target, states, {m_arrowUp.width, m_arrowUp.height}, m_arrowBackgroundColorCached);

            sf::ConvexShape arrow{3};
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowUp) && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                arrow.setPoint(0, {m_arrowUp.width / 5, m_arrowUp.height * 4/5});
                arrow.setPoint(1, {m_arrowUp.width / 2, m_arrowUp.height / 5});
                arrow.setPoint(2, {m_arrowUp.width * 4/5, m_arrowUp.height * 4/5});
            }
            else
            {
                arrow.setPoint(0, {m_arrowUp.width * 4/5, m_arrowUp.height / 5});
                arrow.setPoint(1, {m_arrowUp.width / 5, m_arrowUp.height / 2});
                arrow.setPoint(2, {m_arrowUp.width * 4/5, m_arrowUp.height * 4/5});
            }

            target.draw(arrow, states);
        }

        // Draw the track
        states.transform.translate(m_track.getPosition());
        if (textured)
        {
            if (m_mouseHover && m_spriteTrackHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::Track))
                m_spriteTrackHover.draw(target, states);
            else
                m_spriteTrack.draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Track) && m_trackColorHoverCached.isSet())
                drawRectangleShape(target, states, {m_track.width, m_track.height}, m_trackColorHoverCached);
            else
                drawRectangleShape(target, states, {m_track.width, m_track.height}, m_trackColorCached);
        }
        states.transform.translate(-m_track.getPosition());

        // Draw the thumb
        states.transform.translate(m_thumb.getPosition());
        if (textured)
        {
            if (m_mouseHover && m_spriteThumbHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::Thumb))
                m_spriteThumbHover.draw(target, states);
            else
                m_spriteThumb.draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::Thumb) && m_thumbColorHoverCached.isSet())
                drawRectangleShape(target, states, {m_thumb.width, m_thumb.height}, m_thumbColorHoverCached);
            else
                drawRectangleShape(target, states, {m_thumb.width, m_thumb.height}, m_thumbColorCached);
        }
        states.transform.translate(-m_thumb.getPosition());

        // Draw arrow down/right
        states.transform.translate(m_arrowDown.getPosition());
        if (textured)
        {
            if (m_mouseHover && m_spriteArrowDownHover.isSet() && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown))
                m_spriteArrowDownHover.draw(target, states);
            else
                m_spriteArrowDown.draw(target, states);
        }
        else
        {
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && m_arrowBackgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, {m_arrowDown.width, m_arrowDown.height}, m_arrowBackgroundColorHoverCached);
            else
                drawRectangleShape(target, states, {m_arrowDown.width, m_arrowDown.height}, m_arrowBackgroundColorCached);

            sf::ConvexShape arrow{3};
            if (m_mouseHover && (m_mouseHoverOverPart == Scrollbar::Part::ArrowDown) && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            if (m_verticalScroll)
            {
                arrow.setPoint(0, {m_arrowDown.width / 5, m_arrowDown.height / 5});
                arrow.setPoint(1, {m_arrowDown.width / 2, m_arrowDown.height * 4/5});
                arrow.setPoint(2, {m_arrowDown.width * 4/5, m_arrowDown.height / 5});
            }
            else // Spin button lies horizontal
            {
                arrow.setPoint(0, {m_arrowDown.width / 5, m_arrowDown.height / 5});
                arrow.setPoint(1, {m_arrowDown.width * 4/5, m_arrowDown.height / 2});
                arrow.setPoint(2, {m_arrowDown.width / 5, m_arrowDown.height * 4/5});
            }

            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isMouseDown() const
    {
        return m_mouseDown;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isMouseDownOnThumb() const
    {
        return m_mouseDownOnThumb;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollbarChildWidget::isShown() const
    {
        return m_visible && (!m_autoHide || (m_maximum > m_viewportSize));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Widgets/SpinButton.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton()
    {
        m_type = "SpinButton";

        m_renderer = aurora::makeCopied<SpinButtonRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(20, 42);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::create(float minimum, float maximum)
    {
        auto spinButton = std::make_shared<SpinButton>();

        spinButton->setMinimum(minimum);
        spinButton->setMaximum(maximum);

        return spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::copy(SpinButton::ConstPtr spinButton)
    {
        if (spinButton)
            return std::static_pointer_cast<SpinButton>(spinButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinButton::getSharedRenderer()
    {
        return aurora::downcast<SpinButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SpinButtonRenderer* SpinButton::getSharedRenderer() const
    {
        return aurora::downcast<const SpinButtonRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButtonRenderer* SpinButton::getRenderer()
    {
        return aurora::downcast<SpinButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SpinButtonRenderer* SpinButton::getRenderer() const
    {
        return aurora::downcast<const SpinButtonRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_verticalScroll = (getSize().y >= getSize().x);
        if (m_verticalScroll)
        {
            m_spriteArrowUp.setRotation(0);
            m_spriteArrowUpHover.setRotation(0);
            m_spriteArrowDown.setRotation(0);
            m_spriteArrowDownHover.setRotation(0);
        }
        else
        {
            m_spriteArrowUp.setRotation(-90);
            m_spriteArrowUpHover.setRotation(-90);
            m_spriteArrowDown.setRotation(-90);
            m_spriteArrowDownHover.setRotation(-90);
        }

        const Vector2f arrowSize = getArrowSize();
        m_spriteArrowUp.setSize(arrowSize);
        m_spriteArrowUpHover.setSize(arrowSize);
        m_spriteArrowDown.setSize(arrowSize);
        m_spriteArrowDownHover.setSize(arrowSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(float minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            setMaximum(m_minimum);

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(float maximum)
    {
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(float value)
    {
        // Round to nearest allowed value
        if (m_step != 0)
           value = m_minimum + (std::round((value - m_minimum) / m_step) * m_step);

        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;
            onValueChange.emit(this, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setStep(float step)
    {
        m_step = step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float SpinButton::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseDownOnTopArrow = true;
            else
                m_mouseDownOnTopArrow = false;
        }
        else
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseDownOnTopArrow = false;
            else
                m_mouseDownOnTopArrow = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(Vector2f pos)
    {
        // Check if the mouse went down on the spin button
        if (m_mouseDown)
        {
            // Check if the arrow went down on the top/right arrow
            if (m_mouseDownOnTopArrow)
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (!FloatRect{getPosition().x, getPosition().y, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Increment the value
                    if (m_value < m_maximum)
                        setValue(m_value + m_step);
                    else
                        return;
                }
                else
                    return;
            }
            else // The mouse went down on the bottom/left arrow
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (!FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.f}.contains(pos)))
                 || (!m_verticalScroll && (FloatRect{getPosition().x, getPosition().y, getSize().x / 2.f, getSize().y}.contains(pos))))
                {
                    // Decrement the value
                    if (m_value > m_minimum)
                        setValue(m_value - m_step);
                    else
                        return;
                }
                else
                    return;
            }
        }

        ClickableWidget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(Vector2f pos)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.0f}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }
        else
        {
            if (FloatRect{getPosition().x, getPosition().y, getSize().x / 2.0f, getSize().y}.contains(pos))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }

        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& SpinButton::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "borderbetweenarrows")
        {
            m_borderBetweenArrowsCached = getSharedRenderer()->getBorderBetweenArrows();
            setSize(m_size);
        }
        else if (property == "texturearrowup")
        {
            m_spriteArrowUp.setTexture(getSharedRenderer()->getTextureArrowUp());
        }
        else if (property == "texturearrowuphover")
        {
            m_spriteArrowUpHover.setTexture(getSharedRenderer()->getTextureArrowUpHover());
        }
        else if (property == "texturearrowdown")
        {
            m_spriteArrowDown.setTexture(getSharedRenderer()->getTextureArrowDown());
        }
        else if (property == "texturearrowdownhover")
        {
            m_spriteArrowDownHover.setTexture(getSharedRenderer()->getTextureArrowDownHover());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "arrowcolor")
        {
            m_arrowColorCached = getSharedRenderer()->getArrowColor();
        }
        else if (property == "arrowcolorhover")
        {
            m_arrowColorHoverCached = getSharedRenderer()->getArrowColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteArrowUp.setOpacity(m_opacityCached);
            m_spriteArrowUpHover.setOpacity(m_opacityCached);
            m_spriteArrowDown.setOpacity(m_opacityCached);
            m_spriteArrowDownHover.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> SpinButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(to_string(m_step));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stof(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stof(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stof(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["step"])
            setStep(tgui::stof(node->propertyValuePairs["step"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SpinButton::getArrowSize() const
    {
        if (m_verticalScroll)
            return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                    (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_borderBetweenArrowsCached) / 2.0f};
        else
            return {getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom(),
                    (getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_borderBetweenArrowsCached) / 2.0f};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const Vector2f arrowSize = getArrowSize();

        // Draw the top/left arrow
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && m_mouseHoverOnTopArrow && m_spriteArrowUpHover.isSet())
                m_spriteArrowUpHover.draw(target, states);
            else
                m_spriteArrowUp.draw(target, states);
        }
        else
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y / 5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorHoverCached, m_opacityCached));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));

            if (m_mouseHover && m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }

        // Draw the space between the arrows (if there is space)
        if (m_verticalScroll)
        {
            states.transform.translate({0, arrowSize.y});

            if (m_borderBetweenArrowsCached > 0)
            {
                drawRectangleShape(target, states, {arrowSize.x, m_borderBetweenArrowsCached}, m_borderColorCached);
                states.transform.translate({0, m_borderBetweenArrowsCached});
            }
        }
        else // Horizontal orientation
        {
            states.transform.translate({arrowSize.y, 0});

            if (m_borderBetweenArrowsCached > 0)
            {
                drawRectangleShape(target, states, {m_borderBetweenArrowsCached, arrowSize.x}, m_borderColorCached);
                states.transform.translate({m_borderBetweenArrowsCached, 0});
            }
        }

        // Draw the bottom/right arrow
        if (m_spriteArrowUp.isSet() && m_spriteArrowDown.isSet())
        {
            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_spriteArrowDownHover.isSet())
                m_spriteArrowDownHover.draw(target, states);
            else
                m_spriteArrowDown.draw(target, states);
        }
        else // There are no images
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;

            if (m_verticalScroll)
            {
                arrowBack.setSize(arrowSize);

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x / 2, arrowBack.getSize().y * 4/5});
                arrow.setPoint(2, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 5});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({arrowSize.y, arrowSize.x});

                arrow.setPoint(0, {arrowBack.getSize().x / 5, arrowBack.getSize().y / 5});
                arrow.setPoint(1, {arrowBack.getSize().x * 4/5, arrowBack.getSize().y / 2});
                arrow.setPoint(2, {arrowBack.getSize().x / 5, arrowBack.getSize().y * 4/5});
            }

            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_backgroundColorHoverCached.isSet())
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorHoverCached, m_opacityCached));
            else
                arrowBack.setFillColor(Color::calcColorOpacity(m_backgroundColorCached, m_opacityCached));

            if (m_mouseHover && !m_mouseHoverOnTopArrow && m_arrowColorHoverCached.isSet())
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorHoverCached, m_opacityCached));
            else
                arrow.setFillColor(Color::calcColorOpacity(m_arrowColorCached, m_opacityCached));

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Clipboard.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox()
    {
        m_type = "TextBox";
        m_draggableWidget = true;
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection1.setFont(m_fontCached);
        m_textSelection2.setFont(m_fontCached);
        m_textAfterSelection1.setFont(m_fontCached);
        m_textAfterSelection2.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<TextBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_fontCached, m_textSize) * 18,
                 10 * m_fontCached.getLineSpacing(m_textSize) + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize) + Text::getExtraVerticalPadding(m_textSize)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::create()
    {
        return std::make_shared<TextBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::copy(TextBox::ConstPtr textBox)
    {
        if (textBox)
            return std::static_pointer_cast<TextBox>(textBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getSharedRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getSharedRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // If there is a scrollbar then reinitialize it
        if (isVerticalScrollbarPresent())
        {
            m_verticalScroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
            m_verticalScroll->setSize({m_verticalScroll->getSize().x, getInnerSize().y});
        }

        // The size of the text box has changed, update the text
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String& text)
    {
        // Remove all the excess characters when a character limit is set
        if ((m_maxChars > 0) && (text.getSize() > m_maxChars))
            m_text = text.substring(0, m_maxChars);
        else
            m_text = text;

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const sf::String& text)
    {
        setText(m_text + text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& TextBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String TextBox::getSelectedText() const
    {
        auto pos = findTextSelectionPositions();
        if (pos.first > pos.second)
            std::swap(pos.first, pos.second);

        return m_text.substring(pos.first, pos.second - pos.first);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;
        if (m_textSize < 1)
            m_textSize = 1;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSize);
        m_textSelection1.setCharacterSize(m_textSize);
        m_textSelection2.setCharacterSize(m_textSize);
        m_textAfterSelection1.setCharacterSize(m_textSize);
        m_textAfterSelection2.setCharacterSize(m_textSize);

        // Calculate the height of one line
        m_lineHeight = static_cast<unsigned int>(m_fontCached.getLineSpacing(m_textSize));

        m_verticalScroll->setScrollAmount(m_lineHeight);

        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(std::size_t maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setVerticalScrollbarPresent(bool present)
    {
        if (present)
        {
            m_verticalScroll->setVisible(true);
            setSize(m_size);
        }
        else
        {
            m_verticalScroll->setVisible(false);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::isVerticalScrollbarPresent() const
    {
        return m_verticalScroll->isVisible();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Find the line and position on that line on which the caret is located
        std::size_t count = 0;
        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            if (count + m_lines[i].getSize() < charactersBeforeCaret)
            {
                count += m_lines[i].getSize();
                if ((count < m_text.getSize()) && (m_text[count] == '\n'))
                    count += 1;
            }
            else
            {
                m_selStart.y = i;
                m_selStart.x = charactersBeforeCaret - count;

                m_selEnd = m_selStart;
                updateSelectionTexts();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getCaretPosition() const
    {
        return findTextSelectionPositions().second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getLinesCount() const
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setFocused(bool focused)
    {
        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selStart != m_selEnd)
            {
                m_selStart = m_selEnd;
                updateSelectionTexts();
            }
        }

    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(focused);
    #endif

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        // Set the mouse down flag
        m_mouseDown = true;

        // If there is a scrollbar then pass the event
        if ((m_verticalScroll->isShown()) && (m_verticalScroll->mouseOnWidget(pos)))
        {
            m_verticalScroll->leftMousePressed(pos);
            recalculateVisibleLines();
        }
        else // The click occurred on the text box
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            const auto caretPosition = findCaretPosition(pos);

            // Check if this is a double click
            if ((m_possibleDoubleClick) && (m_selStart == m_selEnd) && (caretPosition == m_selEnd))
            {
                // The next click is going to be a normal one again
                m_possibleDoubleClick = false;

                // If the click was to the right of the end of line then make sure to select the word on the left
                if (m_lines[m_selStart.y].getSize() > 1 && (m_selStart.x == (m_lines[m_selStart.y].getSize()-1) || m_selStart.x == m_lines[m_selStart.y].getSize()))
                {
                    m_selStart.x--;
                    m_selEnd.x = m_selStart.x;
                }

                bool selectingWhitespace;
                if (isWhitespace(m_lines[m_selStart.y][m_selStart.x]))
                    selectingWhitespace = true;
                else
                    selectingWhitespace = false;

                // Move start pointer to the beginning of the word/whitespace
                for (std::size_t i = m_selStart.x; i > 0; --i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selStart.y][i-1]))
                    {
                        m_selStart.x = i;
                        break;
                    }
                    else
                        m_selStart.x = 0;
                }

                // Move end pointer to the end of the word/whitespace
                for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].getSize(); ++i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selEnd.y][i]))
                    {
                        m_selEnd.x = i;
                        break;
                    }
                    else
                        m_selEnd.x = m_lines[m_selEnd.y].getSize();
                }
            }
            else // No double clicking
            {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                    m_selStart = caretPosition;

                m_selEnd = caretPosition;

                // If the next click comes soon enough then it will be a double click
                m_possibleDoubleClick = true;
            }

            // Update the texts
            updateSelectionTexts();

            // The caret should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(Vector2f pos)
    {
        // If there is a scrollbar then pass it the event
        if (m_verticalScroll->isShown())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_verticalScroll->isMouseDown())
            {
                m_verticalScroll->leftMouseReleased(pos - getPosition());
                recalculateVisibleLines();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the scrollbar
        if (m_verticalScroll->isShown() && ((m_verticalScroll->isMouseDown() && m_verticalScroll->isMouseDownOnThumb()) || m_verticalScroll->mouseOnWidget(pos)))
        {
            m_verticalScroll->mouseMoved(pos);
            recalculateVisibleLines();
        }

        // If the mouse is held down then you are selecting text
        else if (m_mouseDown)
        {
            const sf::Vector2<std::size_t> caretPosition = findCaretPosition(pos);
            if (caretPosition != m_selEnd)
            {
                m_selEnd = caretPosition;
                updateSelectionTexts();
            }

            // Check if the caret is located above or below the view
            if (m_verticalScroll->isShown())
            {
                if (m_selEnd.y <= m_topLine)
                    m_verticalScroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                    m_verticalScroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_verticalScroll->getViewportSize()));

                recalculateVisibleLines();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_verticalScroll->isShown())
            m_verticalScroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        if (m_verticalScroll->isShown())
            m_verticalScroll->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        switch (event.code)
        {
            case sf::Keyboard::Up:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y - (m_lineHeight / 2.f) - m_verticalScroll->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Down:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y + (m_lineHeight * 1.5f) - m_verticalScroll->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Left:
            {
                if (event.control)
                {
                    // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
                    bool skippedWhitespace = false;
                    bool done = false;
                    for (std::size_t j = m_selEnd.y + 1; j > 0; --j)
                    {
                        for (std::size_t i = m_selEnd.x; i > 0; --i)
                        {
                            if (skippedWhitespace)
                            {
                                if (isWhitespace(m_lines[m_selEnd.y][i-1]))
                                {
                                    m_selEnd.x = i;
                                    done = true;
                                    break;
                                }
                            }
                            else
                            {
                                if (!isWhitespace(m_lines[m_selEnd.y][i-1]))
                                    skippedWhitespace = true;
                            }
                        }

                        if (!done)
                        {
                            if (!skippedWhitespace)
                            {
                                if (m_selEnd.y > 0)
                                {
                                    m_selEnd.y--;
                                    m_selEnd.x = m_lines[m_selEnd.y].getSize();
                                }
                            }
                            else
                            {
                                m_selEnd.x = 0;
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y < m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x < m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else if (m_selEnd.x > 0)
                        m_selEnd.x--;
                    else
                    {
                        // You are at the left side of a line so move up
                        if (m_selEnd.y > 0)
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].getSize();
                        }
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Right:
            {
                if (event.control)
                {
                    // Move to the end of the word (or to the end of the next word when already at the end)
                    bool skippedWhitespace = false;
                    bool done = false;
                    for (std::size_t j = m_selEnd.y; j < m_lines.size(); ++j)
                    {
                        for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].getSize(); ++i)
                        {
                            if (skippedWhitespace)
                            {
                                if (isWhitespace(m_lines[m_selEnd.y][i]))
                                {
                                    m_selEnd.x = i;
                                    done = true;
                                    break;
                                }
                            }
                            else
                            {
                                if (!isWhitespace(m_lines[m_selEnd.y][i]))
                                    skippedWhitespace = true;
                            }
                        }

                        if (!done)
                        {
                            if (!skippedWhitespace)
                            {
                                if (m_selEnd.y + 1 < m_lines.size())
                                {
                                    m_selEnd.y++;
                                    m_selEnd.x = 0;
                                }
                            }
                            else
                            {
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else
                    {
                        // Move to the next line if you are at the end of the line
                        if (m_selEnd.x == m_lines[m_selEnd.y].getSize())
                        {
                            if (m_selEnd.y + 1 < m_lines.size())
                            {
                                m_selEnd.y++;
                                m_selEnd.x = 0;
                            }
                        }
                        else
                            m_selEnd.x++;
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Home:
            {
                if (event.control)
                    m_selEnd = {0, 0};
                else
                    m_selEnd.x = 0;

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::End:
            {
                if (event.control)
                    m_selEnd = {m_lines[m_lines.size()-1].getSize(), m_lines.size()-1};
                else
                    m_selEnd.x = m_lines[m_selEnd.y].getSize();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageUp:
            {
                // Move to the top line when not there already
                if (m_selEnd.y != m_topLine)
                    m_selEnd.y = m_topLine;
                else
                {
                    // Scroll up when we already where at the top line
                    const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
                    if (m_topLine < visibleLines - 1)
                        m_selEnd.y = 0;
                    else
                        m_selEnd.y = m_topLine - visibleLines + 1;
                }

                m_selEnd.x = 0;

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageDown:
            {
                // Move to the bottom line when not there already
                if (m_topLine + m_visibleLines > m_lines.size())
                    m_selEnd.y = m_lines.size() - 1;
                else if (m_selEnd.y != m_topLine + m_visibleLines - 1)
                    m_selEnd.y = m_topLine + m_visibleLines - 1;
                else
                {
                    // Scroll down when we already where at the bottom line
                    const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
                    if (m_selEnd.y + visibleLines >= m_lines.size() + 2)
                        m_selEnd.y = m_lines.size() - 1;
                    else
                        m_selEnd.y = m_selEnd.y + visibleLines - 2;
                }

                m_selEnd.x = m_lines[m_selEnd.y].getSize();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Return:
            {
                textEntered('\n');
                break;
            }

            case sf::Keyboard::BackSpace:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    const std::size_t pos = findTextSelectionPositions().second;
                    if (pos > 0)
                    {
                        if (m_selEnd.x > 0)
                        {
                            // There is a specific case that we have to watch out for. When we are removing the last character on
                            // a line which was placed there by word wrap and a newline follows this character then the caret
                            // has to be placed at the line above (before the newline) instead of at the same line (after the newline)
                            if ((m_lines[m_selEnd.y].getSize() == 1) && (pos > 1) && (pos < m_text.getSize()) && (m_text[pos-2] != '\n') && (m_text[pos] == '\n') && (m_selEnd.y > 0))
                            {
                                m_selEnd.y--;
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();
                            }
                            else // Just remove the character normally
                                --m_selEnd.x;
                        }
                        else // At the beginning of the line
                        {
                            if (m_selEnd.y > 0)
                            {
                                --m_selEnd.y;
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();

                                if ((m_text[pos-1] != '\n') && m_selEnd.x > 0)
                                    --m_selEnd.x;
                            }
                        }

                        m_selStart = m_selEnd;

                        m_text.erase(pos - 1, 1);
                        rearrangeText(true);
                    }
                }
                else // When you did select some characters then delete them
                    deleteSelectedCharacters();

                // The caret should be visible again
                m_caretVisible = true;
                m_animationTimeElapsed = {};

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::Delete:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    m_text.erase(findTextSelectionPositions().second, 1);
                    rearrangeText(true);
                }
                else // You did select some characters, so remove them
                    deleteSelectedCharacters();

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::A:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    m_selStart = {0, 0};
                    m_selEnd = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                    updateSelectionTexts();
                }

                break;
            }

            case sf::Keyboard::C:
            {
                auto selectionPositions = findTextSelectionPositions();
                if (selectionPositions.first > selectionPositions.second)
                    std::swap(selectionPositions.first, selectionPositions.second);

                if (event.control && !event.alt && !event.shift && !event.system)
                    Clipboard::set(m_text.substring(selectionPositions.first, selectionPositions.second - selectionPositions.first));

                break;
            }

            case sf::Keyboard::X:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    auto selectionPositions = findTextSelectionPositions();
                    if (selectionPositions.first > selectionPositions.second)
                        std::swap(selectionPositions.first, selectionPositions.second);

                    Clipboard::set(m_text.substring(selectionPositions.first, selectionPositions.second - selectionPositions.first));
                    deleteSelectedCharacters();
                }

                break;
            }

            case sf::Keyboard::V:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    const sf::String clipboardContents = Clipboard::get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selStart != m_selEnd) || (clipboardContents != ""))
                    {
                        deleteSelectedCharacters();

                        m_text.insert(findTextSelectionPositions().first, clipboardContents);
                        m_lines[m_selStart.y].insert(m_selStart.x, clipboardContents);

                        m_selStart.x += clipboardContents.getSize();
                        m_selEnd = m_selStart;
                        rearrangeText(true);

                        onTextChange.emit(this, m_text);
                    }
                }

                break;
            }

            default:
                break;
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(std::uint32_t key)
    {
        if (m_readOnly)
            return;

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        auto insert = [=]()
        {
            deleteSelectedCharacters();

            const std::size_t caretPosition = findTextSelectionPositions().first;

            m_text.insert(caretPosition, key);
            m_lines[m_selEnd.y].insert(m_selEnd.x, key);

            // Increment the caret position, unless you type a newline at the start of a line while that line only existed due to word wrapping
            if ((key != '\n') || (m_selEnd.x > 0) || (m_selEnd.y == 0) || m_lines[m_selEnd.y-1].isEmpty() || (m_text[caretPosition-1] == '\n'))
            {
                m_selStart.x++;
                m_selEnd.x++;
            }

            rearrangeText(true);
        };

        // If there is a scrollbar then inserting can't go wrong
        if (isVerticalScrollbarPresent())
        {
            insert();
        }
        else // There is no scrollbar, the text may not fit
        {
            // Store the data so that it can be reverted
            sf::String oldText = m_text;
            const auto oldSelStart = m_selStart;
            const auto oldSelEnd = m_selEnd;

            // Try to insert the character
            insert();

            // Undo the insert if the text does not fit
            if (m_lines.size() > getInnerSize().y / m_lineHeight)
            {
                m_text = oldText;
                m_selStart = oldSelStart;
                m_selEnd = oldSelEnd;

                rearrangeText(true);
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_verticalScroll->isShown())
        {
            m_verticalScroll->mouseWheelScrolled(delta, pos - getPosition());
            recalculateVisibleLines();
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2<std::size_t> TextBox::findCaretPosition(Vector2f position) const
    {
        position.x -= m_bordersCached.getLeft() + m_paddingCached.getLeft();
        position.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find on which line the mouse is
        std::size_t lineNumber;
        if (m_verticalScroll->isShown())
        {
            if (position.y + m_verticalScroll->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor((position.y + m_verticalScroll->getValue()) / m_lineHeight));
        }
        else
        {
            if (position.y < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor(position.y / m_lineHeight));
        }

        // Check if you clicked behind everything
        if (lineNumber + 1 > m_lines.size())
            return sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find between which character the mouse is standing
        float width = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        std::uint32_t prevChar = 0;
        for (std::size_t i = 0; i < m_lines[lineNumber].getSize(); ++i)
        {
            float charWidth;
            const std::uint32_t curChar = m_lines[lineNumber][i];
            //if (curChar == '\n')
            //    return sf::Vector2f<std::size_t>(m_lines[lineNumber].getSize() - 1, lineNumber); // TextBox strips newlines but this code is kept for when this function is generalized
            //else
            if (curChar == '\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, getTextSize(), false).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, getTextSize());
            if (width + charWidth + kerning <= position.x)
                width += charWidth + kerning;
            else
            {
                if (position.x < width + kerning + (charWidth / 2.0f))
                    return {i, lineNumber};
                else
                    return {i + 1, lineNumber};
            }

            prevChar = curChar;
        }

        // You clicked behind the last character
        return sf::Vector2<std::size_t>(m_lines[lineNumber].getSize(), lineNumber);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<std::size_t, std::size_t> TextBox::findTextSelectionPositions() const
    {
        // This function is used to count the amount of characters spread over several lines
        auto findIndex = [this](std::size_t line)
        {
            std::size_t counter = 0;
            for (std::size_t i = 0; i < line; ++i)
            {
                counter += m_lines[i].getSize();
                if ((counter < m_text.getSize()) && (m_text[counter] == '\n'))
                    counter += 1;
            }

            return counter;
        };

        return {findIndex(m_selStart.y) + m_selStart.x, findIndex(m_selEnd.y) + m_selEnd.x};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        if (m_selStart != m_selEnd)
        {
            auto textSelectionPositions = findTextSelectionPositions();

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
            {
                m_text.erase(textSelectionPositions.second, textSelectionPositions.first - textSelectionPositions.second);
                m_selStart = m_selEnd;
            }
            else
            {
                m_text.erase(textSelectionPositions.first, textSelectionPositions.second - textSelectionPositions.first);
                m_selEnd = m_selStart;
            }

            rearrangeText(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rearrangeText(bool keepSelection)
    {
        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return;

        // Find the maximum width of one line
        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2 * textOffset;
        if (m_verticalScroll->isShown())
            maxLineWidth -= m_verticalScroll->getSize().x;

        // Don't do anything when there is no room for the text
        if (maxLineWidth <= 0)
            return;

        // Store the current selection position when we are keeping the selection
        std::pair<std::size_t, std::size_t> textSelectionPositions;
        if (keepSelection)
            textSelectionPositions = findTextSelectionPositions();

        // Fit the text in the available space
        const sf::String string = Text::wordWrap(maxLineWidth, m_text, m_fontCached, m_textSize, false, false);

        // Split the string in multiple lines
        m_lines.clear();
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != sf::String::InvalidPos)
        {
            newLinePos = string.find('\n', searchPosStart);

            if (newLinePos != sf::String::InvalidPos)
                m_lines.push_back(string.substring(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.push_back(string.substring(searchPosStart));

            searchPosStart = newLinePos + 1;
        }

        // Check if we should try to keep our selection
        if (keepSelection)
        {
            std::size_t index = 0;
            sf::Vector2<std::size_t> newSelStart;
            sf::Vector2<std::size_t> newSelEnd;
            bool newSelStartFound = false;
            bool newSelEndFound = false;

            // Look for the new locations of our selection
            for (std::size_t i = 0; i < m_lines.size(); ++i)
            {
                index += m_lines[i].getSize();

                if (!newSelStartFound && (index >= textSelectionPositions.first))
                {
                    newSelStart = sf::Vector2<std::size_t>(m_lines[i].getSize() - (index - textSelectionPositions.first), i);

                    newSelStartFound = true;
                    if (newSelEndFound)
                        break;
                }

                if (!newSelEndFound && (index >= textSelectionPositions.second))
                {
                    newSelEnd = sf::Vector2<std::size_t>(m_lines[i].getSize() - (index - textSelectionPositions.second), i);

                    newSelEndFound = true;
                    if (newSelStartFound)
                        break;
                }

                // Skip newlines in the text
                if (m_text[index] == '\n')
                    ++index;
            }

            // Keep the selection when possible
            if (newSelStartFound && newSelEndFound)
            {
                m_selStart = newSelStart;
                m_selEnd = newSelEnd;
            }
            else // The text has changed too much, the selection can't be kept
            {
                m_selStart = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                m_selEnd = m_selStart;
            }
        }
        else // Set the caret at the back of the text
        {
            m_selStart = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
            m_selEnd = m_selStart;
        }

        // Tell the scrollbar how many pixels the text contains
        const bool scrollbarShown = m_verticalScroll->isShown();

        m_verticalScroll->setMaximum(static_cast<unsigned int>(m_lines.size() * m_lineHeight
                                                              + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                              + Text::getExtraVerticalPadding(m_textSize)));

        // We may have to recalculate what we just calculated if the scrollbar just appeared or disappeared
        if (scrollbarShown != m_verticalScroll->isShown())
        {
            rearrangeText(true);
            return;
        }

        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateSelectionTexts()
    {
        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selStart == m_selEnd)
        {
            sf::String displayedText;
            for (std::size_t i = 0; i < m_lines.size(); ++i)
                displayedText += m_lines[i] + "\n";

            m_textBeforeSelection.setString(displayedText);
            m_textSelection1.setString("");
            m_textSelection2.setString("");
            m_textAfterSelection1.setString("");
            m_textAfterSelection2.setString("");
        }
        else // Some text is selected
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            // Set the text before the selection
            if (selectionStart.y > 0)
            {
                sf::String string;
                for (std::size_t i = 0; i < selectionStart.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionStart.y].substring(0, selectionStart.x);
                m_textBeforeSelection.setString(string);
            }
            else
                m_textBeforeSelection.setString(m_lines[0].substring(0, selectionStart.x));

            // Set the selected text
            if (m_selStart.y == m_selEnd.y)
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, selectionEnd.x - selectionStart.x));
                m_textSelection2.setString("");
            }
            else
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, m_lines[selectionStart.y].getSize() - selectionStart.x));

                sf::String string;
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionEnd.y].substring(0, selectionEnd.x);

                m_textSelection2.setString(string);
            }

            // Set the text after the selection
            {
                m_textAfterSelection1.setString(m_lines[selectionEnd.y].substring(selectionEnd.x, m_lines[selectionEnd.y].getSize() - selectionEnd.x));

                sf::String string;
                for (std::size_t i = selectionEnd.y + 1; i < m_lines.size(); ++i)
                    string += m_lines[i] + "\n";

                m_textAfterSelection2.setString(string);
            }
        }

        // Check if the caret is located above or below the view
        if (isVerticalScrollbarPresent())
        {
            if (m_selEnd.y <= m_topLine)
                m_verticalScroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_verticalScroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight)
                                                                    + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                                    + Text::getExtraVerticalPadding(m_textSize)
                                                                    - m_verticalScroll->getViewportSize()));
        }

        recalculatePositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TextBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // Only show/hide the caret every half second
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            // Reset the elapsed time
            m_animationTimeElapsed = {};

            // Switch the value of the visible flag
            m_caretVisible = !m_caretVisible;

            // Too slow for double clicking
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculatePositions()
    {
        if (!m_fontCached)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        sf::Text tempText{"", *m_fontCached.getFont(), getTextSize()};

        // Position the caret
        {
            tempText.setString(m_lines[m_selEnd.y].substring(0, m_selEnd.x));

            float kerning = 0;
            if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].getSize()))
                kerning = m_fontCached.getKerning(m_lines[m_selEnd.y][m_selEnd.x-1], m_lines[m_selEnd.y][m_selEnd.x], m_textSize);

            m_caretPosition = {textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerning, static_cast<float>(m_selEnd.y * m_lineHeight)};
        }

        // Calculate the position of the text objects
        m_selectionRects.clear();
        m_textBeforeSelection.setPosition({textOffset, 0});
        if (m_selStart != m_selEnd)
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            float kerningSelectionStart = 0;
            if ((selectionStart.x > 0) && (selectionStart.x < m_lines[selectionStart.y].getSize()))
                kerningSelectionStart = m_fontCached.getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSize);

            float kerningSelectionEnd = 0;
            if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].getSize()))
                kerningSelectionEnd = m_fontCached.getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSize);

            if (selectionStart.x > 0)
            {
                m_textSelection1.setPosition({textOffset + m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x + kerningSelectionStart,
                                              m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});
            }
            else
                m_textSelection1.setPosition({textOffset, m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});

            m_textSelection2.setPosition({textOffset, static_cast<float>((selectionStart.y + 1) * m_lineHeight)});

            if (!m_textSelection2.getString().isEmpty() || (selectionEnd.x == 0))
            {
                m_textAfterSelection1.setPosition({textOffset + m_textSelection2.findCharacterPos(m_textSelection2.getString().getSize()).x + kerningSelectionEnd,
                                                   m_textSelection2.getPosition().y + ((selectionEnd.y - selectionStart.y - 1) * m_lineHeight)});
            }
            else
                m_textAfterSelection1.setPosition({m_textSelection1.getPosition().x + m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x + kerningSelectionEnd,
                                                   m_textSelection1.getPosition().y});

            m_textAfterSelection2.setPosition({textOffset, static_cast<float>((selectionEnd.y + 1) * m_lineHeight)});

            // Recalculate the selection rectangles
            {
                m_selectionRects.push_back({m_textSelection1.getPosition().x, static_cast<float>(selectionStart.y * m_lineHeight), 0, static_cast<float>(m_lineHeight)});

                if (!m_lines[selectionStart.y].isEmpty())
                {
                    m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x;

                    // There is kerning when the selection is on just this line
                    if (selectionStart.y == selectionEnd.y)
                        m_selectionRects.back().width += kerningSelectionEnd;
                }

                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                {
                    m_selectionRects.back().width += textOffset;
                    m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(i * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});

                    if (!m_lines[i].isEmpty())
                    {
                        tempText.setString(m_lines[i]);
                        m_selectionRects.back().width += tempText.findCharacterPos(tempText.getString().getSize()).x;
                    }
                }

                if (selectionStart.y != selectionEnd.y)
                {
                    m_selectionRects.back().width += textOffset;

                    if (m_textSelection2.getString() != "")
                    {
                        tempText.setString(m_lines[selectionEnd.y].substring(0, selectionEnd.x));
                        m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(selectionEnd.y * m_lineHeight),
                                                    textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerningSelectionEnd, static_cast<float>(m_lineHeight)});
                    }
                    else
                        m_selectionRects.push_back({0, static_cast<float>(selectionEnd.y * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});
                }
            }
        }

        recalculateVisibleLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculateVisibleLines()
    {
        m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight), m_lines.size());

        // Store which area is visible
        if (m_verticalScroll->isShown())
        {
            m_verticalScroll->setPosition({getSize().x - m_bordersCached.getRight() - m_verticalScroll->getSize().x, m_bordersCached.getTop()});

            m_topLine = m_verticalScroll->getValue() / m_lineHeight;

            // The scrollbar may be standing between lines in which case one more line is visible
            if (((static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) % m_lineHeight) != 0) || ((m_verticalScroll->getValue() % m_lineHeight) != 0))
                m_visibleLines++;
        }
        else // There is no scrollbar
        {
            m_topLine = 0;
            m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight), m_lines.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TextBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTextChange.getName()))
            return onTextChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection1.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection2.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection1.setColor(getSharedRenderer()->getSelectedTextColor());
            m_textSelection2.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_verticalScroll->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScroll->getDefaultWidth();
            m_verticalScroll->setSize({width, m_verticalScroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == "caretwidth")
        {
            m_caretWidthCached = getSharedRenderer()->getCaretWidth();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_verticalScroll->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection1.setOpacity(m_opacityCached);
            m_textAfterSelection2.setOpacity(m_opacityCached);
            m_textSelection1.setOpacity(m_opacityCached);
            m_textSelection2.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection1.setFont(m_fontCached);
            m_textSelection2.setFont(m_fontCached);
            m_textAfterSelection1.setFont(m_fontCached);
            m_textAfterSelection2.setFont(m_fontCached);
            setTextSize(getTextSize());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TextBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxChars));

        if (m_readOnly)
            node->propertyValuePairs["ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");

        if (!isVerticalScrollbarPresent())
            node->propertyValuePairs["VerticalScrollbarPresent"] = std::make_unique<DataIO::ValueNode>("false");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["readonly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["readonly"]->value).getBool());
        if (node->propertyValuePairs["verticalscrollbarpresent"])
            setVerticalScrollbarPresent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["verticalscrollbarpresent"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the contents of the text box
        {
            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

            float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScroll->isShown())
                maxLineWidth -= m_verticalScroll->getSize().x;

            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            const Clipping clipping{target, states, {}, {maxLineWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            // Move the text according to the vertical scrollar
            states.transform.translate({0, -static_cast<float>(m_verticalScroll->getValue())});

            // Draw the background of the selected text
            for (const auto& selectionRect : m_selectionRects)
            {
                states.transform.translate({selectionRect.left, selectionRect.top});
                drawRectangleShape(target, states, {selectionRect.width, selectionRect.height + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)}, m_selectedTextBackgroundColorCached);
                states.transform.translate({-selectionRect.left, -selectionRect.top});
            }

            // Draw the text
            m_textBeforeSelection.draw(target, states);
            if (m_selStart != m_selEnd)
            {
                m_textSelection1.draw(target, states);
                m_textSelection2.draw(target, states);
                m_textAfterSelection1.draw(target, states);
                m_textAfterSelection2.draw(target, states);
            }

            // Only draw the caret when needed
            if (m_focused && m_caretVisible && (m_caretWidthCached > 0))
            {
                const float caretHeight = m_lineHeight + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize);
                states.transform.translate({std::ceil(m_caretPosition.x - (m_caretWidthCached / 2.f)), m_caretPosition.y});
                drawRectangleShape(target, states, {m_caretWidthCached, caretHeight}, m_caretColorCached);
            }
        }

        // Draw the scrollbar if needed
        if (m_verticalScroll->isShown())
            m_verticalScroll->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/VerticalLayout.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::VerticalLayout(const Layout2d& size) :
        BoxLayoutRatios{size}
    {
        m_type = "VerticalLayout";

        m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::Ptr VerticalLayout::create(const Layout2d& size)
    {
        return std::make_shared<VerticalLayout>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalLayout::Ptr VerticalLayout::copy(VerticalLayout::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<VerticalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalLayout::updateWidgets()
    {
        const float totalSpaceBetweenWidgets = (m_spaceBetweenWidgetsCached * m_widgets.size()) - m_spaceBetweenWidgetsCached;
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                          getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        const float totalRatio = std::accumulate(m_ratios.begin(), m_ratios.end(), 0.f);

        float currentOffset = 0;
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            auto& widget = m_widgets[i];
            const float height = (contentSize.y - totalSpaceBetweenWidgets) * (m_ratios[i] / totalRatio);

            widget->setSize({contentSize.x, height});
            widget->setPosition({0, currentOffset});

            // Correct the size for widgets that are bigger than what you set (e.g. have borders around it or a text next to them)
            if (widget->getFullSize() != widget->getSize())
            {
                const Vector2f newSize = widget->getSize() - (widget->getFullSize() - widget->getSize());
                if (newSize.x > 0 && newSize.y > 0)
                {
                    widget->setSize(newSize);
                    widget->setPosition(widget->getPosition() - widget->getWidgetOffset());
                }
            }

            currentOffset += height + m_spaceBetweenWidgetsCached;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
