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


#include <TGUI/Animation.hpp>
#include <TGUI/Widget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Animation::Type Animation::getType() const
        {
            return m_type;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Animation::finish()
        {
            if (m_finishedCallback != nullptr)
                m_finishedCallback();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Animation::Animation(Type type, Widget::Ptr widget, sf::Time duration, std::function<void()> finishedCallback) :
            m_type            {type},
            m_widget          {widget},
            m_totalDuration   {duration},
            m_finishedCallback{finishedCallback}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        MoveAnimation::MoveAnimation(Widget::Ptr widget, Vector2f start, Vector2f end, sf::Time duration, std::function<void()> finishedCallback) :
            Animation {Type::Move, widget, duration, finishedCallback},
            m_startPos{start},
            m_endPos  {end}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool MoveAnimation::update(sf::Time elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setPosition(m_startPos + ((m_elapsedTime.asSeconds() / m_totalDuration.asSeconds()) * (m_endPos - m_startPos)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void MoveAnimation::finish()
        {
            m_widget->setPosition(m_endPos);
            Animation::finish();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ResizeAnimation::ResizeAnimation(Widget::Ptr widget, Vector2f start, Vector2f end, sf::Time duration, std::function<void()> finishedCallback) :
            Animation  {Type::Resize, widget, duration, finishedCallback},
            m_startSize{start},
            m_endSize  {end}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool ResizeAnimation::update(sf::Time elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setSize(m_startSize + ((m_elapsedTime.asSeconds() / m_totalDuration.asSeconds()) * (m_endSize - m_startSize)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void ResizeAnimation::finish()
        {
            m_widget->setSize(m_endSize);
            Animation::finish();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FadeAnimation::FadeAnimation(Widget::Ptr widget, float start, float end, sf::Time duration, std::function<void()> finishedCallback) :
            Animation     {Type::Fade, widget, duration, finishedCallback},
            m_startOpacity{std::max(0.f, std::min(1.f, start))},
            m_endOpacity  {std::max(0.f, std::min(1.f, end))}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool FadeAnimation::update(sf::Time elapsedTime)
        {
            m_elapsedTime += elapsedTime;
            if (m_elapsedTime >= m_totalDuration)
            {
                finish();
                return true;
            }

            m_widget->setInheritedOpacity(m_startOpacity + ((m_elapsedTime.asSeconds() / m_totalDuration.asSeconds()) * (m_endOpacity - m_startOpacity)));
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void FadeAnimation::finish()
        {
            m_widget->setInheritedOpacity(m_endOpacity);
            Animation::finish();
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


#include <TGUI/Clipping.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    sf::View Clipping::m_originalView;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Clipping::Clipping(sf::RenderTarget& target, const sf::RenderStates& states, Vector2f topLeft, Vector2f size) :
        m_target {target},
        m_oldView{target.getView()}
    {
        Vector2f bottomRight = Vector2f(states.transform.transformPoint(topLeft + size));
        topLeft = Vector2f(states.transform.transformPoint(topLeft));

        Vector2f viewTopLeft = topLeft;
        size = bottomRight - topLeft;

        topLeft.x -= m_originalView.getCenter().x - (m_originalView.getSize().x / 2.f);
        topLeft.y -= m_originalView.getCenter().y - (m_originalView.getSize().y / 2.f);
        bottomRight.x -= m_originalView.getCenter().x - (m_originalView.getSize().x / 2.f);
        bottomRight.y -= m_originalView.getCenter().y - (m_originalView.getSize().y / 2.f);

        topLeft.x *= m_originalView.getViewport().width / m_originalView.getSize().x;
        topLeft.y *= m_originalView.getViewport().height / m_originalView.getSize().y;
        size.x *= m_originalView.getViewport().width / m_originalView.getSize().x;
        size.y *= m_originalView.getViewport().height / m_originalView.getSize().y;

        topLeft.x += m_originalView.getViewport().left;
        topLeft.y += m_originalView.getViewport().top;

        if (topLeft.x < m_oldView.getViewport().left)
        {
            size.x -= m_oldView.getViewport().left - topLeft.x;
            viewTopLeft.x += (m_oldView.getViewport().left - topLeft.x) * (m_originalView.getSize().x / m_originalView.getViewport().width);
            topLeft.x = m_oldView.getViewport().left;
        }
        if (topLeft.y < m_oldView.getViewport().top)
        {
            size.y -= m_oldView.getViewport().top - topLeft.y;
            viewTopLeft.y += (m_oldView.getViewport().top - topLeft.y) * (m_originalView.getSize().y / m_originalView.getViewport().height);
            topLeft.y = m_oldView.getViewport().top;
        }

        if (size.x > m_oldView.getViewport().left + m_oldView.getViewport().width - topLeft.x)
            size.x = m_oldView.getViewport().left + m_oldView.getViewport().width - topLeft.x;
        if (size.y > m_oldView.getViewport().top + m_oldView.getViewport().height - topLeft.y)
            size.y = m_oldView.getViewport().top + m_oldView.getViewport().height - topLeft.y;

        if ((size.x >= 0) && (size.y >= 0))
        {
            sf::View view{{std::round(viewTopLeft.x),
                           std::round(viewTopLeft.y),
                           std::round(size.x * m_originalView.getSize().x / m_originalView.getViewport().width),
                           std::round(size.y * m_originalView.getSize().y / m_originalView.getViewport().height)}};

            view.setViewport({topLeft.x, topLeft.y, size.x, size.y});
            target.setView(view);
        }
        else // The clipping area lies outside the viewport
        {
            sf::View emptyView{{0, 0, 0, 0}};
            emptyView.setViewport({0, 0, 0, 0});
            target.setView(emptyView);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Clipping::~Clipping()
    {
        m_target.setView(m_oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Clipping::setGuiView(const sf::View& view)
    {
        m_originalView = view;
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
#include <TGUI/ToolTip.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>

#include <cassert>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void getAllRenderers(std::map<RendererData*, std::vector<const Widget*>>& renderers, const Container* container)
        {
            for (const auto& child : container->getWidgets())
            {
                renderers[child->getSharedRenderer()->getData().get()].push_back(child.get());

                if (child->getToolTip())
                    renderers[child->getToolTip()->getSharedRenderer()->getData().get()].push_back(child->getToolTip().get());

                Container* childContainer = dynamic_cast<Container*>(child.get());
                if (childContainer)
                    getAllRenderers(renderers, childContainer);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveRenderer(RendererData* renderer, const std::string& name)
        {
            auto node = std::make_unique<DataIO::Node>();
            node->name = name;
            for (const auto& pair : renderer->propertyValuePairs)
            {
                // Skip "font = null"
                if (pair.first == "font" && ObjectConverter{pair.second}.getString() == "null")
                    continue;

                if (pair.second.getType() == ObjectConverter::Type::RendererData)
                {
                    std::stringstream ss{ObjectConverter{pair.second}.getString()};
                    auto rendererRootNode = DataIO::parse(ss);

                    // If there are braces around the renderer string, then the child node is the one we need
                    if (rendererRootNode->propertyValuePairs.empty() && (rendererRootNode->children.size() == 1))
                        rendererRootNode = std::move(rendererRootNode->children[0]);

                    rendererRootNode->name = pair.first;
                    node->children.push_back(std::move(rendererRootNode));
                }
                else
                    node->propertyValuePairs[pair.first] = std::make_unique<DataIO::ValueNode>(ObjectConverter{pair.second}.getString());
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container()
    {
        m_containerWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(const Container& other) :
        Widget{other}
    {
        // Copy all the widgets
        for (std::size_t i = 0; i < other.m_widgets.size(); ++i)
            add(other.m_widgets[i]->clone(), other.m_widgetNames[i]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::Container(Container&& other) :
        Widget                {std::move(other)},
        m_widgets             {std::move(other.m_widgets)},
        m_widgetNames         {std::move(other.m_widgetNames)},
        m_widgetBelowMouse    {std::move(other.m_widgetBelowMouse)},
        m_focusedWidget       {std::move(other.m_focusedWidget)},
        m_handingMouseReleased{std::move(other.m_handingMouseReleased)}
    {
        for (auto& widget : m_widgets)
            widget->setParent(this);

        other.m_widgets = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container::~Container()
    {
        for (const auto& widget : m_widgets)
        {
            if (widget->getParent() == this)
                widget->setParent(nullptr);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (const Container& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(right);

            m_widgetBelowMouse = nullptr;
            m_focusedWidget = nullptr;

            // Remove all the old widgets
            Container::removeAllWidgets();

            // Copy all the widgets
            for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
            {
                // Don't allow the 'add' function of a derived class to be called, since its members are not copied yet
                Container::add(right.m_widgets[i]->clone(), right.m_widgetNames[i]);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Container& Container::operator= (Container&& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Widget::operator=(std::move(right));
            m_widgets              = std::move(right.m_widgets);
            m_widgetNames          = std::move(right.m_widgetNames);
            m_widgetBelowMouse     = std::move(right.m_widgetBelowMouse);
            m_focusedWidget        = std::move(right.m_focusedWidget);
            m_handingMouseReleased = std::move(right.m_handingMouseReleased);

            for (auto& widget : m_widgets)
                widget->setParent(this);

            right.m_widgets = {};
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setSize(const Layout2d& size)
    {
        if (size.getValue() != m_prevSize)
        {
            Widget::setSize(size);
            m_prevInnerSize = getInnerSize();
        }
        else // Size didn't change, but also check the inner size in case the borders or padding changed
        {
            Widget::setSize(size);
            if (getInnerSize() != m_prevInnerSize)
            {
                m_prevInnerSize = getInnerSize();
                for (auto& layout : m_boundSizeLayouts)
                    layout->recalculateValue();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        assert(widgetPtr != nullptr);

        widgetPtr->setParent(this);
        m_widgets.push_back(widgetPtr);
        m_widgetNames.push_back(widgetName);

        if (m_fontCached != getGlobalFont())
            widgetPtr->setInheritedFont(m_fontCached);

        if (m_opacityCached < 1)
            widgetPtr->setInheritedOpacity(m_opacityCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::get(const sf::String& widgetName) const
    {
        for (std::size_t i = 0; i < m_widgetNames.size(); ++i)
        {
            if (m_widgetNames[i] == widgetName)
                return m_widgets[i];
        }

        for (std::size_t i = 0; i < m_widgetNames.size(); ++i)
        {
            if (m_widgets[i]->isContainer())
            {
                Widget::Ptr widget = std::static_pointer_cast<Container>(m_widgets[i])->get(widgetName);
                if (widget != nullptr)
                    return widget;
            }
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::remove(const Widget::Ptr& widget)
    {
        // Loop through every widget
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Check if the pointer matches
            if (m_widgets[i] == widget)
            {
                if (m_widgetBelowMouse == widget)
                {
                    widget->mouseNoLongerOnWidget();
                    m_widgetBelowMouse = nullptr;
                }

                if (widget == m_focusedWidget)
                {
                    m_focusedWidget = nullptr;
                    widget->setFocused(false);
                }

                // Remove the widget
                widget->setParent(nullptr);
                m_widgets.erase(m_widgets.begin() + i);
                m_widgetNames.erase(m_widgetNames.begin() + i);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::removeAllWidgets()
    {
        for (const auto& widget : m_widgets)
            widget->setParent(nullptr);

        m_widgets.clear();
        m_widgetNames.clear();

        m_widgetBelowMouse = nullptr;
        m_focusedWidget = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
            {
                m_widgetNames[i] = name;
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Container::getWidgetName(const Widget::ConstPtr& widget) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return m_widgetNames[i];
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::uncheckRadioButtons()
    {
        // Loop through all radio buttons and uncheck them
        for (auto& widget : m_widgets)
        {
            if (widget->getWidgetType() == "RadioButton")
                std::static_pointer_cast<RadioButton>(widget)->setChecked(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Container::getInnerSize() const
    {
        return getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromFile(const std::string& filename)
    {
        std::ifstream in{filename};
        if (!in.is_open())
            throw Exception{"Failed to open '" + filename + "' to load the widgets from it."};

        std::stringstream stream;
        stream << in.rdbuf();
        loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToFile(const std::string& filename)
    {
        std::stringstream stream;
        saveWidgetsToStream(stream);

        std::ofstream out{filename};
        if (!out.is_open())
            throw Exception{"Failed to open '" + filename + "' for saving the widgets to it."};

        out << stream.rdbuf();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream& stream)
    {
        auto rootNode = DataIO::parse(stream);

        removeAllWidgets(); // The existing widgets will be replaced by the ones that will be loaded

        if (rootNode->propertyValuePairs.size() != 0)
            Widget::load(rootNode, {});

        std::map<std::string, std::shared_ptr<RendererData>> availableRenderers;
        for (const auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);

            std::string objectName;
            if (nameSeparator != std::string::npos)
                objectName = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();

            if (toLower(widgetType) == "renderer")
            {
                if (!objectName.empty())
                    availableRenderers[toLower(objectName)] = RendererData::createFromDataIONode(node.get());
            }
            else // Section describes a widget
            {
                const auto& constructor = WidgetFactory::getConstructFunction(toLower(widgetType));
                if (constructor)
                {
                    Widget::Ptr widget = constructor();
                    widget->load(node, availableRenderers);
                    add(widget, objectName);
                }
                else
                    throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::loadWidgetsFromStream(std::stringstream&& stream)
    {
        loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::saveWidgetsToStream(std::stringstream& stream) const
    {
        auto node = std::make_unique<DataIO::Node>();

        std::map<RendererData*, std::vector<const Widget*>> renderers;
        getAllRenderers(renderers, this);

        unsigned int id = 0;
        SavingRenderersMap renderersMap;
        for (const auto& renderer : renderers)
        {
            // The renderer can remain inside the widget if it is not shared, so provide the node to be included inside the widget
            if (renderer.second.size() == 1)
            {
                renderersMap[renderer.second[0]] = {saveRenderer(renderer.first, "Renderer"), ""};
                continue;
            }

            // When the widget is shared, only provide the id instead of the node itself
            ++id;
            const std::string idStr = to_string(id);
            node->children.push_back(saveRenderer(renderer.first, "Renderer." + idStr));
            for (const auto& child : renderer.second)
                renderersMap[child] = std::make_pair(nullptr, idStr); // Did not compile with VS2015 Update 2 when using braces
        }

        for (const auto& child : getWidgets())
            node->children.emplace_back(child->save(renderersMap));

        DataIO::emit(node, stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToFront(const Widget::Ptr& widget)
    {
        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            // Copy the widget
            m_widgets.push_back(m_widgets[i]);
            m_widgetNames.push_back(m_widgetNames[i]);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i);
            m_widgetNames.erase(m_widgetNames.begin() + i);
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::moveWidgetToBack(const Widget::Ptr& widget)
    {
        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] != widget)
                continue;

            // Copy the widget
            const Widget::Ptr obj = m_widgets[i];
            const std::string name = m_widgetNames[i];
            m_widgets.insert(m_widgets.begin(), obj);
            m_widgetNames.insert(m_widgetNames.begin(), name);

            // Remove the old widget
            m_widgets.erase(m_widgets.begin() + i + 1);
            m_widgetNames.erase(m_widgetNames.begin() + i + 1);
            break;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusNextWidget()
    {
        // If the focused widget is a container then try to focus the next widget inside it
        if (m_focusedWidget && m_focusedWidget->isContainer())
        {
            auto focusedContainer = std::static_pointer_cast<Container>(m_focusedWidget);
            if (focusedContainer->focusNextWidget())
                return true;
        }

        // Loop all widgets behind the focused one
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();
        for (std::size_t i = focusedWidgetIndex; i < m_widgets.size(); ++i)
        {
            if (tryFocusWidget(m_widgets[i], false))
                return true;
        }

        // If we are not an isolated focus group then the focus will be given to the group behind us
        if (!m_isolatedFocus)
            return false;

        // None of the widgets behind the focused one could be focused, so loop the ones before it
        if (!m_focusedWidget)
            return false;

        // Also include the focused widget since it may be a container that didn't have its first widget focused
        for (std::size_t i = 0; i < focusedWidgetIndex; ++i)
        {
            if (tryFocusWidget(m_widgets[i], false))
                return true;
        }

        // No other widget could be focused
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::focusPreviousWidget()
    {
        // If the focused widget is a container then try to focus the previous widget inside it
        if (m_focusedWidget && m_focusedWidget->isContainer())
        {
            auto focusedContainer = std::static_pointer_cast<Container>(m_focusedWidget);
            if (focusedContainer->focusPreviousWidget())
                return true;
        }

        // Loop all widgets before the focused one
        const std::size_t focusedWidgetIndex = getFocusedWidgetIndex();
        if (focusedWidgetIndex > 0)
        {
            for (std::size_t i = focusedWidgetIndex - 1; i > 0; --i)
            {
                if (tryFocusWidget(m_widgets[i-1], true))
                    return true;
            }

            // If we are not an isolated focus group then the focus will be given to the group before us
            if (!m_isolatedFocus)
                return false;
        }

        // None of the widgets before the focused one could be focused, so loop the ones after it.
        for (std::size_t i = m_widgets.size(); i > focusedWidgetIndex; --i)
        {
            if (tryFocusWidget(m_widgets[i-1], true))
                return true;
        }

        // Also include the focused widget since it may be a container that didn't have its last widget focused.
        if (focusedWidgetIndex > 0)
        {
            if (tryFocusWidget(m_widgets[focusedWidgetIndex-1], true))
                return true;
        }

        // No other widget could be focused
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::setFocused(bool focused)
    {
        if (!focused && m_focused && m_focusedWidget)
            m_focusedWidget->setFocused(false);

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::childWidgetFocused(const Widget::Ptr& child)
    {
        if (m_focusedWidget != child)
        {
            if (m_focusedWidget)
                m_focusedWidget->setFocused(false);

            m_focusedWidget = child;
        }

        if (!isFocused())
            setFocused(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMousePressed(Vector2f pos)
    {
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseButton.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::leftMouseReleased(Vector2f pos)
    {
        sf::Event event;
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseButton.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event, but don't let it call mouseNoLongerDown on all widgets (it will be done later)
        m_handingMouseReleased = true;
        handleEvent(event);
        m_handingMouseReleased = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);

        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseMove.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::keyPressed(const sf::Event::KeyEvent& event)
    {
        sf::Event newEvent;
        newEvent.type = sf::Event::KeyPressed;
        newEvent.key = event;

        // Let the event manager handle the event
        handleEvent(newEvent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::textEntered(std::uint32_t key)
    {
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = key;

        // Let the event manager handle the event
        handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::mouseWheelScrolled(float delta, Vector2f pos)
    {
        sf::Event event;
        event.type = sf::Event::MouseWheelScrolled;
        event.mouseWheelScroll.delta = delta;
        event.mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel;
        event.mouseWheelScroll.x = static_cast<int>(pos.x - getPosition().x - getChildWidgetsOffset().x);
        event.mouseWheelScroll.y = static_cast<int>(pos.y - getPosition().y - getChildWidgetsOffset().y);

        // Let the event manager handle the event
        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
        {
            mouseLeftWidget();

            if (m_widgetBelowMouse)
            {
                m_widgetBelowMouse->mouseNoLongerOnWidget();
                m_widgetBelowMouse = nullptr;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        for (auto& widget : m_widgets)
            widget->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::askToolTip(Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset();

            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                toolTip = widget->askToolTip(mousePos);
                if (toolTip)
                    return toolTip;
            }

            if (m_toolTip)
                return getToolTip();
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::rendererChanged(const std::string& property)
    {
        Widget::rendererChanged(property);

        if (property == "opacity")
        {
            for (std::size_t i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->setInheritedOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            for (const auto& widget : m_widgets)
            {
                if (m_fontCached != getGlobalFont())
                    widget->setInheritedFont(m_fontCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Container::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        for (const auto& child : getWidgets())
            node->children.emplace_back(child->save(renderers));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        for (const auto& childNode : node->children)
        {
            const auto nameSeparator = childNode->name.find('.');
            const auto widgetType = childNode->name.substr(0, nameSeparator);

            const auto& constructor = WidgetFactory::getConstructFunction(toLower(widgetType));
            if (constructor)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
                    className = Deserializer::deserialize(ObjectConverter::Type::String, childNode->name.substr(nameSeparator + 1)).getString();

                Widget::Ptr childWidget = constructor();
                childWidget->load(childNode, renderers);
                add(childWidget, className);
            }
            else
                throw Exception{"No construct function exists for widget type '" + widgetType + "'."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // Loop through all widgets
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            // Update the elapsed time in widgets that need it
            if (m_widgets[i]->isVisible())
                m_widgets[i]->update(elapsedTime);
        }

        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::handleEvent(sf::Event& event)
    {
        // Check if a mouse button has moved
        if ((event.type == sf::Event::MouseMoved) || ((event.type == sf::Event::TouchMoved) && (event.touch.finger == 0)))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseMoved)
                mousePos = {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Loop through all widgets
            for (auto& widget : m_widgets)
            {
                // Check if the mouse went down on the widget
                if (widget->m_mouseDown)
                {
                    // Some widgets should always receive mouse move events while dragging them, even if the mouse is no longer on top of them.
                    if (widget->m_draggableWidget || widget->isContainer())
                    {
                        widget->mouseMoved(mousePos);
                        return true;
                    }
                }
            }

            // Check if the mouse is on top of a widget
            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget != nullptr)
            {
                // Send the event to the widget
                widget->mouseMoved(mousePos);
                return true;
            }

            return false;
        }

        // Check if a mouse button was pressed or a touch event occurred
        else if ((event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::TouchBegan))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseButtonPressed)
                mousePos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Check if the mouse is on top of a widget
            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                // Unfocus the previously focused widget
                if (m_focusedWidget && (m_focusedWidget != widget))
                    m_focusedWidget->setFocused(false);

                // Focus the widget unless it is a container, in which case it will get focused when the event is handled by the bottom widget
                m_focusedWidget = widget;
                if (!widget->isContainer())
                    widget->setFocused(true);

                if (((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Left))
                 || ((event.type == sf::Event::TouchBegan) && (event.touch.finger == 0)))
                {
                    widget->leftMousePressed(mousePos);
                    return true;
                }
                else if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right))
                {
                    widget->rightMousePressed(mousePos);
                    return true;
                }
            }
            else // The mouse did not went down on a widget, so unfocus the focused child widget, but keep ourselves focused
            {
                if (m_focusedWidget)
                    m_focusedWidget->setFocused(false);

                m_focusedWidget = nullptr;
                setFocused(true);
            }

            return false;
        }

        // Check if a mouse button was released
        else if (((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Left))
              || ((event.type == sf::Event::TouchEnded) && (event.touch.finger == 0)))
        {
            Vector2f mousePos;
            if (event.type == sf::Event::MouseButtonReleased)
                mousePos = {static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)};
            else
                mousePos = {static_cast<float>(event.touch.x), static_cast<float>(event.touch.y)};

            // Check if the mouse is on top of a widget
            Widget::Ptr widgetBelowMouse = mouseOnWhichWidget(mousePos);
            if (widgetBelowMouse != nullptr)
                widgetBelowMouse->leftMouseReleased(mousePos);

            // Tell all widgets that the mouse has gone up
            // But don't do this when leftMouseReleased was called on this container because
            // it will happen afterwards when mouseNoLongerDown is called on it
            if (!m_handingMouseReleased)
            {
                for (auto& widget : m_widgets)
                    widget->mouseNoLongerDown();
            }

            if (widgetBelowMouse != nullptr)
                return true;

            return false;
        }

        // Check if a key was pressed
        else if (event.type == sf::Event::KeyPressed)
        {
            // Only continue when the character was recognised
            if (event.key.code != sf::Keyboard::Unknown)
            {
                // Check if there is a focused widget
                if (m_focusedWidget && m_focusedWidget->isFocused())
                {
                #if defined(SFML_SYSTEM_ANDROID) && SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
                    // SFML versions prior to 2.5 sent Delete instead of BackSpace
                    if (event.key.code == sf::Keyboard::Delete)
                        event.key.code = sf::Keyboard::BackSpace;
                #endif

                    // Tell the widget that the key was pressed
                    m_focusedWidget->keyPressed(event.key);
                    return true;
                }
            }

            return false;
        }

        // Also check if text was entered (not a special key)
        else if (event.type == sf::Event::TextEntered)
        {
            // Check if the character that we pressed is allowed
            if ((event.text.unicode >= 32) && (event.text.unicode != 127))
            {
                // Tell the widget that the key was pressed
                if (m_focusedWidget && m_focusedWidget->isFocused())
                {
                    m_focusedWidget->textEntered(event.text.unicode);
                    return true;
                }
            }

            return false;
        }

        // Check for mouse wheel scrolling
        else if ((event.type == sf::Event::MouseWheelScrolled) && (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel))
        {
            // Send the event to the widget below the mouse
            Widget::Ptr widget = mouseOnWhichWidget({static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y)});
            if (widget != nullptr)
                return widget->mouseWheelScrolled(event.mouseWheelScroll.delta, {static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y)});

            return false;
        }
        else // Event is ignored
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Container::mouseOnWhichWidget(Vector2f mousePos)
    {
        Widget::Ptr widget = nullptr;
        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            if ((*it)->isVisible())
            {
                if ((*it)->mouseOnWidget(mousePos))
                {
                    if ((*it)->isEnabled())
                        widget = *it;

                    break;
                }
            }
        }

        // If the mouse is on a different widget, tell the old widget that the mouse has left
        if (m_widgetBelowMouse && (widget != m_widgetBelowMouse))
            m_widgetBelowMouse->mouseNoLongerOnWidget();

        m_widgetBelowMouse = widget;
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Container::drawWidgetContainer(sf::RenderTarget* target, const sf::RenderStates& states) const
    {
        // Draw all widgets when they are visible
        for (const auto& widget : m_widgets)
        {
            if (widget->isVisible())
                widget->draw(*target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Container::getFocusedWidgetIndex() const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_focusedWidget == m_widgets[i])
                return i+1;
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Container::tryFocusWidget(const tgui::Widget::Ptr &widget, bool reverseWidgetOrder)
    {
        // If you are not allowed to focus the widget, then skip it
        if (!widget->canGainFocus() || !widget->isVisible() || !widget->isEnabled())
            return false;

        if (widget->isContainer())
        {
            auto container = std::static_pointer_cast<Container>(widget);

            // Also skip isolated containers (e.g. ChildWindow)
            if (container->m_isolatedFocus)
                return false;

            // Try to focus the first focusable widget in the container
            auto oldUnfocusedWidget = container->m_focusedWidget;
            container->m_focusedWidget = nullptr;
            bool childFocused = reverseWidgetOrder ? container->focusPreviousWidget() : container->focusNextWidget();

            if (oldUnfocusedWidget && (oldUnfocusedWidget != container->m_focusedWidget))
                oldUnfocusedWidget->setFocused(false);

            if (!childFocused)
                return false;
        }

        if (m_focusedWidget == widget)
            return true;

        if (m_focusedWidget)
            m_focusedWidget->setFocused(false);

        m_focusedWidget = widget;
        m_focusedWidget->setFocused(true);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiContainer::GuiContainer()
    {
        m_type = "GuiContainer";
        m_focused = true;
        m_isolatedFocus = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setSize(const Layout2d&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::setFocused(bool focused)
    {
        Container::setFocused(focused);
        m_focused = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiContainer::mouseOnWidget(Vector2f) const
    {
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiContainer::draw(sf::RenderTarget&, sf::RenderStates) const
    {
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


#include <TGUI/Font.hpp>
#include <TGUI/Loading/Deserializer.hpp>

#include <cassert>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(std::nullptr_t)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::string& id) :
        m_font(Deserializer::deserialize(ObjectConverter::Type::Font, id).getFont()), // Did not compile with clang 3.6 when using braces
        m_id  (Deserializer::deserialize(ObjectConverter::Type::String, id).getString()) // Did not compile with clang 3.6 when using braces
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const char* id) :
        Font(std::string{id})
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::shared_ptr<sf::Font>& font) :
        m_font{font}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const sf::Font& font) :
        m_font{std::make_shared<sf::Font>(font)}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::Font(const void* data, std::size_t sizeInBytes) :
        m_font{std::make_shared<sf::Font>()}
    {
        m_font->loadFromMemory(data, sizeInBytes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font& Font::operator=(const Font& other)
    {
        if (this != &other)
        {
            // If this was the last instance that shares the global font then also destroy the global font
            if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
                setGlobalFont(nullptr);

            m_font = other.m_font;
            m_id = other.m_id;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font& Font::operator=(Font&& other)
    {
        if (this != &other)
        {
            // If this was the last instance that shares the global font then also destroy the global font
            if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
                setGlobalFont(nullptr);

            m_font = std::move(other.m_font);
            m_id = std::move(other.m_id);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::~Font()
    {
        // If this is the last instance that shares the global font then also destroy the global font
        if (m_font && (m_font == getInternalGlobalFont()) && (m_font.use_count() == 2))
            setGlobalFont(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Font::getId() const
    {
        return m_id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> Font::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::operator std::shared_ptr<sf::Font>() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font::operator bool() const
    {
        return m_font != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator==(std::nullptr_t) const
    {
        return m_font == nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator!=(std::nullptr_t) const
    {
        return m_font != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator==(const Font& right) const
    {
        return m_font == right.m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Font::operator!=(const Font& right) const
    {
        return m_font != right.m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Glyph& Font::getGlyph(std::uint32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness) const
    {
        assert(m_font != nullptr);

    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        return m_font->getGlyph(codePoint, characterSize, bold, outlineThickness);
    #else
        (void)outlineThickness;
        return m_font->getGlyph(codePoint, characterSize, bold);
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Font::getKerning(std::uint32_t first, std::uint32_t second, unsigned int characterSize) const
    {
        if (m_font)
            return m_font->getKerning(first, second, characterSize);
        else
            return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Font::getLineSpacing(unsigned int characterSize) const
    {
        if (m_font)
            return m_font->getLineSpacing(characterSize);
        else
            return 0;
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

#include <TGUI/Gui.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Clipping.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <cassert>

#ifdef SFML_SYSTEM_WINDOWS
    #ifndef NOMINMAX // MinGW already defines this which causes a warning without this check
        #define NOMINMAX
    #endif
    #define NOMB
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        m_target(nullptr)
    #else
        m_target        (nullptr),
        m_accessToWindow(false)
    #endif
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    Gui::Gui(sf::RenderWindow& window) :
        m_target        (&window),
        m_accessToWindow(true)
    {
        Clipboard::setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());

        init();
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderTarget& target) :
    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        m_target(&target)
    #else
        m_target        (&target),
        m_accessToWindow(false)
    #endif
    {
        setView(target.getDefaultView());

        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    void Gui::setTarget(sf::RenderWindow& window)
    {
        m_target = &window;

        m_accessToWindow = true;
        Clipboard::setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setTarget(sf::RenderTarget& target)
    {
    #if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        m_accessToWindow = false;
    #endif

        m_target = &target;

        setView(target.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* Gui::getTarget() const
    {
        return m_target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setView(const sf::View& view)
    {
        if ((m_view.getCenter() != view.getCenter()) || (m_view.getSize() != view.getSize()))
        {
            m_view = view;

            m_container->m_size = view.getSize();
            m_container->onSizeChange.emit(m_container.get(), m_container->getSize());

            for (auto& layout : m_container->m_boundSizeLayouts)
                layout->recalculateValue();
        }
        else // Set it anyway in case something changed that we didn't care to check
            m_view = view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Gui::getView() const
    {
        return m_view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event)
    {
        assert(m_target != nullptr);

        // Check if the event has something to do with the mouse
        if ((event.type == sf::Event::MouseMoved) || (event.type == sf::Event::TouchMoved)
         || (event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::TouchBegan)
         || (event.type == sf::Event::MouseButtonReleased) || (event.type == sf::Event::TouchEnded)
         || (event.type == sf::Event::MouseWheelScrolled))
        {
            Vector2f mouseCoords;

            switch (event.type)
            {
                case sf::Event::MouseMoved:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}, m_view);
                    event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseButtonReleased:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}, m_view);
                    event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseWheelScrolled:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, m_view);
                    event.mouseWheelScroll.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseWheelScroll.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::TouchMoved:
                case sf::Event::TouchBegan:
                case sf::Event::TouchEnded:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.touch.x, event.touch.y}, m_view);
                    event.touch.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.touch.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                default:
                    break;
            }

            // If a tooltip is visible then hide it now
            if (m_visibleToolTip != nullptr)
            {
                // Correct the position of the tool tip so that it is relative again
                m_visibleToolTip->setPosition(m_visibleToolTip->getPosition() - ToolTip::getDistanceToMouse() - m_lastMousePos);

                remove(m_visibleToolTip);
                m_visibleToolTip = nullptr;
            }

            // Reset the data for the tooltip since the mouse has moved
            m_tooltipTime = {};
            m_tooltipPossible = true;
            m_lastMousePos = mouseCoords;
        }

        // Handle tab key presses
        else if (event.type == sf::Event::KeyPressed)
        {
            if (isTabKeyUsageEnabled() && (event.key.code == sf::Keyboard::Tab))
            {
                if (event.key.shift)
                    focusPreviousWidget();
                else
                    focusNextWidget();

                return true;
            }
        }

        // Keep track of whether the window is focused or not
        else if (event.type == sf::Event::LostFocus)
        {
            m_windowFocused = false;
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            m_windowFocused = true;
        #if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
            if (m_accessToWindow)
                Clipboard::setWindowHandle(static_cast<sf::RenderWindow*>(m_target)->getSystemHandle());
        #endif
        }

        return m_container->handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::enableTabKeyUsage()
    {
        m_TabKeyUsageEnabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::disableTabKeyUsage()
    {
        m_TabKeyUsageEnabled = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::isTabKeyUsageEnabled() const
    {
        return m_TabKeyUsageEnabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw()
    {
        assert(m_target != nullptr);

        // Update the time
        if (m_windowFocused)
            updateTime(m_clock.restart());
        else
            m_clock.restart();

        // Change the view
        const sf::View oldView = m_target->getView();
        m_target->setView(m_view);
        Clipping::setGuiView(m_view);

        // Draw the widgets
        m_container->drawWidgetContainer(m_target, sf::RenderStates::Default);

        // Restore the old view
        m_target->setView(oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiContainer::Ptr Gui::getContainer() const
    {
        return m_container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setFont(const Font& font)
    {
        m_container->setInheritedFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> Gui::getFont() const
    {
        return m_container->getInheritedFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& Gui::getWidgets() const
    {
        return m_container->getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& Gui::getWidgetNames() const
    {
        return m_container->getWidgetNames();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const sf::String& widgetName) const
    {
        return m_container->get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::remove(const Widget::Ptr& widget)
    {
        return m_container->remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::removeAllWidgets()
    {
        m_container->removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        return m_container->setWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Gui::getWidgetName(const Widget::Ptr& widget) const
    {
        return m_container->getWidgetName(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusNextWidget()
    {
        m_container->focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusPreviousWidget()
    {
        m_container->focusPreviousWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusAllWidgets()
    {
        m_container->setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::uncheckRadioButtons()
    {
        m_container->uncheckRadioButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setOpacity(float opacity)
    {
        m_container->setInheritedOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Gui::getOpacity() const
    {
        return m_container->getInheritedOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromFile(const std::string& filename)
    {
        m_container->loadWidgetsFromFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToFile(const std::string& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream& stream)
    {
        m_container->loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream&& stream)
    {
        loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToStream(std::stringstream& stream) const
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_container->m_animationTimeElapsed = elapsedTime;
        m_container->update(elapsedTime);

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= ToolTip::getTimeToDisplay())
            {
                Widget::Ptr tooltip = m_container->askToolTip(m_lastMousePos);
                if (tooltip)
                {
                    m_visibleToolTip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$ToolTip#");

                    // Change the relative tool tip position in an absolute one
                    tooltip->setPosition(m_lastMousePos + ToolTip::getDistanceToMouse() + tooltip->getPosition());
                }

                m_tooltipPossible = false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::init()
    {
    #ifdef SFML_SYSTEM_WINDOWS
        unsigned int doubleClickTime = GetDoubleClickTime();
        if (doubleClickTime > 0)
            setDoubleClickTime(doubleClickTime);
    #endif
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


#include <TGUI/ObjectConverter.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ObjectConverter::getString()
    {
        assert(m_type != Type::None);

        if (m_serialized)
            return m_string;

        m_string = Serializer::serialize(ObjectConverter{*this});
        m_serialized = true;
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& ObjectConverter::getFont()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Font || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Font(m_string);
            m_type = Type::Font;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<Font>(m_value);
    #else
        return m_value.as<Font>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Color& ObjectConverter::getColor()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Color || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Color(m_string);
            m_type = Type::Color;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<Color>(m_value);
    #else
        return m_value.as<Color>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::getBool()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Bool || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Bool, m_string).getBool();
            m_type = Type::Bool;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<bool>(m_value);
    #else
        return m_value.as<bool>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ObjectConverter::getNumber()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Number || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Number, m_string).getNumber();
            m_type = Type::Number;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<float>(m_value);
    #else
        return m_value.as<float>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Outline& ObjectConverter::getOutline()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Outline || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Outline, m_string).getOutline();
            m_type = Type::Outline;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<Outline>(m_value);
    #else
        return m_value.as<Outline>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture& ObjectConverter::getTexture()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::Texture || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::Texture, m_string).getTexture();
            m_type = Type::Texture;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<Texture>(m_value);
    #else
        return m_value.as<Texture>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextStyle& ObjectConverter::getTextStyle()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::TextStyle || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::TextStyle, m_string).getTextStyle();
            m_type = Type::TextStyle;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<TextStyle>(m_value);
    #else
        return m_value.as<TextStyle>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::shared_ptr<RendererData>& ObjectConverter::getRenderer()
    {
        assert(m_type != Type::None);
        assert(m_type == Type::RendererData || m_type == Type::String);

        if (m_type == Type::String)
        {
            m_value = Deserializer::deserialize(ObjectConverter::Type::RendererData, m_string).getRenderer();
            m_type = Type::RendererData;
        }

    #ifdef TGUI_USE_CPP17
        return std::get<std::shared_ptr<RendererData>>(m_value);
    #else
        return m_value.as<std::shared_ptr<RendererData>>();
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter::Type ObjectConverter::getType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator==(const ObjectConverter& right) const
    {
        if (this == &right)
            return true;

        if (m_type != right.m_type)
            return false;

        switch (m_type)
        {
        case Type::None:
            return true;
        case Type::String:
            return m_string == right.m_string;
    #ifdef TGUI_USE_CPP17
        case Type::Bool:
            return std::get<bool>(m_value) == std::get<bool>(right.m_value);
        case Type::Font:
            return std::get<Font>(m_value) == std::get<Font>(right.m_value);
        case Type::Color:
            return std::get<Color>(m_value) == std::get<Color>(right.m_value);
        case Type::Number:
            return std::get<float>(m_value) == std::get<float>(right.m_value);
        case Type::Outline:
            return std::get<Outline>(m_value) == std::get<Outline>(right.m_value);
        case Type::Texture:
            return std::get<Texture>(m_value) == std::get<Texture>(right.m_value);
        case Type::TextStyle:
            return std::get<TextStyle>(m_value) == std::get<TextStyle>(right.m_value);
        case Type::RendererData:
            return std::get<std::shared_ptr<RendererData>>(m_value) == std::get<std::shared_ptr<RendererData>>(right.m_value);
    #else
        case Type::Bool:
            return m_value.as<bool>() == right.m_value.as<bool>();
        case Type::Font:
            return m_value.as<Font>() == right.m_value.as<Font>();
        case Type::Color:
            return m_value.as<Color>() == right.m_value.as<Color>();
        case Type::Number:
            return m_value.as<float>() == right.m_value.as<float>();
        case Type::Outline:
            return m_value.as<Outline>() == right.m_value.as<Outline>();
        case Type::Texture:
            return m_value.as<Texture>() == right.m_value.as<Texture>();
        case Type::TextStyle:
            return m_value.as<TextStyle>() == right.m_value.as<TextStyle>();
        case Type::RendererData:
            return m_value.as<std::shared_ptr<RendererData>>() == right.m_value.as<std::shared_ptr<RendererData>>();
    #endif
        default: // This case should never occur, but prevents a warning that control reaches end of non-void function
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ObjectConverter::operator!=(const ObjectConverter& right) const
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


#include <TGUI/Signal.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/SignalImpl.hpp>

#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    unsigned int lastId = 0;

    unsigned int generateUniqueId()
    {
        return ++lastId;
    }

    template <typename T>
    bool checkParamType(std::initializer_list<std::type_index>::const_iterator type)
    {
    #ifdef TGUI_UNSAFE_TYPE_INFO_COMPARISON
        return strcmp(type->name(), typeid(T).name()) == 0;
    #else
        return *type == typeid(T);
    #endif
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace internal_signal
    {
        std::deque<const void*> parameters;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal::Signal(const Signal& other) :
        m_name    {other.m_name},
        m_handlers{} // signal handlers are not copied with the widget
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Signal::operator=(const Signal& other)
    {
        if (this != &other)
        {
            m_name = other.m_name;
            m_handlers.clear(); // signal handlers are not copied with the widget
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::connect(const Delegate& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = handler;
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::connect(const DelegateEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::disconnect(unsigned int id)
    {
        return m_handlers.erase(id) == 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::disconnectAll()
    {
        m_handlers.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Signal::getWidget()
    {
        return internal_signal::dereference<Widget*>(internal_signal::parameters[0])->shared_from_this();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if (unboundParameters.size() == 0)
            return 0;
        else
            throw Exception{"Signal '" + m_name + "' could not provide data for unbound parameters."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    #define TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(TypeName, Type) \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<Type>(internal_signal::parameters[1])); }; \
        return id; \
    } \
    \
    unsigned int Signal##TypeName::connect(const Delegate##TypeName##Ex& handler) \
    { \
        const auto id = generateUniqueId(); \
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<Type>(internal_signal::parameters[1])); }; \
        return id; \
    }

    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Int, int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(UInt, unsigned int)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Bool, bool)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Float, float)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(String, const sf::String&)
    TGUI_SIGNAL_VALUE_CONNECT_DEFINITION(Vector2f, Vector2f)
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalInt::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<int>(unboundParameters.begin()))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalUInt::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<unsigned int>(unboundParameters.begin()))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalBool::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<bool>(unboundParameters.begin()))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalFloat::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<float>(unboundParameters.begin()))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalString::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && (checkParamType<std::string>(unboundParameters.begin()) || checkParamType<sf::String>(unboundParameters.begin())))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalVector2f::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && (checkParamType<Vector2f>(unboundParameters.begin()) || checkParamType<sf::Vector2f>(unboundParameters.begin())))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    unsigned int SignalRange::connect(const DelegateRange& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<float>(internal_signal::parameters[1]), internal_signal::dereference<float>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalRange::connect(const DelegateRangeEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<float>(internal_signal::parameters[1]), internal_signal::dereference<float>(internal_signal::parameters[2])); };
        return id;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalRange::emit(const Widget* widget, float start, float end)
    {
        if (m_handlers.empty())
            return false;

        internal_signal::parameters[1] = static_cast<const void*>(&start);
        internal_signal::parameters[2] = static_cast<const void*>(&end);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalRange::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 2) && checkParamType<float>(unboundParameters.begin()) && checkParamType<float>(unboundParameters.begin()+1))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    unsigned int SignalChildWindow::connect(const DelegateChildWindow& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<ChildWindow::Ptr>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::connect(const DelegateChildWindowEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<ChildWindow::Ptr>(internal_signal::parameters[1])); };
        return id;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalChildWindow::emit(ChildWindow* childWindow)
    {
        if (m_handlers.empty())
            return false;

        ChildWindow::Ptr sharedPtr = std::static_pointer_cast<ChildWindow>(childWindow->shared_from_this());
        internal_signal::parameters[1] = static_cast<const void*>(&sharedPtr);
        return Signal::emit(childWindow);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalChildWindow::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<ChildWindow::Ptr>(unboundParameters.begin()))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    unsigned int SignalItem::connect(const DelegateItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndId& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1]), internal_signal::dereference<sf::String>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::connect(const DelegateItemAndIdEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1]), internal_signal::dereference<sf::String>(internal_signal::parameters[2])); };
        return id;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItem::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && (checkParamType<std::string>(unboundParameters.begin()) || checkParamType<sf::String>(unboundParameters.begin())))
            return 1;
        else if ((unboundParameters.size() == 2)
              && (checkParamType<std::string>(unboundParameters.begin()) || checkParamType<sf::String>(unboundParameters.begin()))
              && (checkParamType<std::string>(unboundParameters.begin()+1) || checkParamType<sf::String>(unboundParameters.begin()+1)))
        {
            return 1;
        }
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    unsigned int SignalItemHierarchy::connect(const DelegateMenuItem& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItemHierarchy::connect(const DelegateMenuItemEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<sf::String>(internal_signal::parameters[1])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItemHierarchy::connect(const DelegateMenuItemFull& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler](){ handler(internal_signal::dereference<std::vector<sf::String>>(internal_signal::parameters[2])); };
        return id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItemHierarchy::connect(const DelegateMenuItemFullEx& handler)
    {
        const auto id = generateUniqueId();
        m_handlers[id] = [handler, name=m_name](){ handler(getWidget(), name, internal_signal::dereference<std::vector<sf::String>>(internal_signal::parameters[2])); };
        return id;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalItemHierarchy::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && (checkParamType<std::string>(unboundParameters.begin()) || checkParamType<sf::String>(unboundParameters.begin())))
            return 1;
        if ((unboundParameters.size() == 1) && checkParamType<std::vector<sf::String>>(unboundParameters.begin()))
            return 2;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SignalAnimation::validateTypes(std::initializer_list<std::type_index> unboundParameters) const
    {
        if ((unboundParameters.size() == 1) && checkParamType<ShowAnimationType>(unboundParameters.begin()))
            return 1;
        if ((unboundParameters.size() == 1) && checkParamType<bool>(unboundParameters.begin()))
            return 2;
        if ((unboundParameters.size() == 2) && checkParamType<ShowAnimationType>(unboundParameters.begin()) && checkParamType<bool>(unboundParameters.begin()+1))
            return 1;
        else
            return Signal::validateTypes(unboundParameters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalWidgetBase::disconnect(unsigned int id)
    {
        auto it = m_connectedSignals.find(id);
        if (it != m_connectedSignals.end())
        {
            const bool ret = getSignal(it->second).disconnect(id);
            m_connectedSignals.erase(it);
            return ret;
        }
        else // The id was not found
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll(std::string signalName)
    {
        return getSignal(toLower(std::move(signalName))).disconnectAll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SignalWidgetBase::disconnectAll()
    {
        std::set<std::string> signalNames;
        for (const auto& connection : m_connectedSignals)
            signalNames.insert(connection.second);

        for (auto& name : signalNames)
            getSignal(std::move(name)).disconnectAll();
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


#include <TGUI/Text.hpp>
#include <TGUI/Global.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::getSize() const
    {
        return m_size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setString(const sf::String& string)
    {
        m_text.setString(string);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Text::getString() const
    {
        return m_text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setCharacterSize(unsigned int size)
    {
        m_text.setCharacterSize(size);
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::getCharacterSize() const
    {
        return m_text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setColor(Color color)
    {
        m_color = color;

    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(Color::calcColorOpacity(color, m_opacity));
    #else
        m_text.setColor(Color::calcColorOpacity(color, m_opacity));
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Color Text::getColor() const
    {
        return m_color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setOpacity(float opacity)
    {
        m_opacity = opacity;

    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_text.setFillColor(Color::calcColorOpacity(m_color, opacity));
    #else
        m_text.setColor(Color::calcColorOpacity(m_color, opacity));
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getOpacity() const
    {
        return m_opacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setFont(Font font)
    {
        m_font = font;

        if (font)
            m_text.setFont(*font.getFont());
        else
        {
            // We can't keep using a pointer to the old font (it might be destroyed), but sf::Text has no function to pass an empty font
            if (m_text.getFont())
            {
                m_text = sf::Text();
                m_text.setString(getString());
                m_text.setCharacterSize(getCharacterSize());
                m_text.setStyle(getStyle());

            #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
                m_text.setFillColor(Color::calcColorOpacity(getColor(), getOpacity()));
            #else
                m_text.setColor(Color::calcColorOpacity(getColor(), getOpacity()));
            #endif
            }
        }

        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font Text::getFont() const
    {
        return m_font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::setStyle(TextStyle style)
    {
        if (style != m_text.getStyle())
        {
            m_text.setStyle(style);
            recalculateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextStyle Text::getStyle() const
    {
        return m_text.getStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Text::findCharacterPos(std::size_t index) const
    {
        return m_text.findCharacterPos(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();

        // Round the position to avoid blurry text
        const float* matrix = states.transform.getMatrix();
        states.transform = sf::Transform{matrix[0], matrix[4], std::round(matrix[12]),
                                         matrix[1], matrix[5], std::floor(matrix[13]),
                                         matrix[3], matrix[7], matrix[15]};

        target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Text::recalculateSize()
    {
        const std::shared_ptr<sf::Font> font = m_font;
        if (font == nullptr)
        {
            m_size = {0, 0};
            return;
        }

        float width = 0;
        float maxWidth = 0;
        unsigned int lines = 1;
        std::uint32_t prevChar = 0;
        const sf::String& string = m_text.getString();
        const bool bold = (m_text.getStyle() & sf::Text::Bold) != 0;
        const unsigned int textSize = m_text.getCharacterSize();
        for (std::size_t i = 0; i < string.getSize(); ++i)
        {
            const float kerning = font->getKerning(prevChar, string[i], textSize);
            if (string[i] == '\n')
            {
                maxWidth = std::max(maxWidth, width);
                width = 0;
                lines++;
            }
            else if (string[i] == '\t')
                width += (static_cast<float>(font->getGlyph(' ', textSize, bold).advance) * 4) + kerning;
            else
                width += static_cast<float>(font->getGlyph(string[i], textSize, bold).advance) + kerning;

            prevChar = string[i];
        }

        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(m_font, m_text.getCharacterSize(), m_text.getStyle());
        const float height = lines * font->getLineSpacing(m_text.getCharacterSize()) + extraVerticalSpace;
        m_size = {std::max(maxWidth, width), height};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(const Text& text)
    {
        return getExtraHorizontalPadding(text.getFont(), text.getCharacterSize(), text.getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraHorizontalPadding(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        return getLineHeight(font, characterSize, textStyle) / 10.f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getExtraVerticalPadding(unsigned int characterSize)
    {
        return std::max(1.f, std::round(characterSize / 8.f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight(const Text& text)
    {
        return getLineHeight(text.getFont(), text.getCharacterSize(), text.getStyle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::getLineHeight(Font font, unsigned int characterSize, TextStyle textStyle)
    {
        const float extraVerticalSpace = Text::calculateExtraVerticalSpace(font, characterSize, textStyle);
        return font.getLineSpacing(characterSize) + extraVerticalSpace;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Text::findBestTextSize(Font fontWrapper, float height, int fit)
    {
        const std::shared_ptr<sf::Font> font = fontWrapper.getFont();
        if (!font)
            return 0;

        if (height < 2)
            return 1;

        std::vector<unsigned int> textSizes(static_cast<std::size_t>(height));
        for (unsigned int i = 0; i < static_cast<unsigned int>(height); ++i)
            textSizes[i] = i + 1;

        const auto high = std::lower_bound(textSizes.begin(), textSizes.end(), height,
                                           [&](unsigned int charSize, float h) { return font->getLineSpacing(charSize) + Text::calculateExtraVerticalSpace(font, charSize) < h; });
        if (high == textSizes.end())
            return static_cast<unsigned int>(height);

        const float highLineSpacing = font->getLineSpacing(*high);
        if (highLineSpacing == height)
            return *high;

        const auto low = high - 1;
        const float lowLineSpacing = font->getLineSpacing(*low);

        if (fit < 0)
            return *low;
        else if (fit > 0)
            return *high;
        else
        {
            if (std::abs(height - lowLineSpacing) < std::abs(height - highLineSpacing))
                return *low;
            else
                return *high;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Text::calculateExtraVerticalSpace(Font font, unsigned int characterSize, TextStyle style)
    {
        if (font == nullptr)
            return 0;

        const bool bold = (style & sf::Text::Bold) != 0;

        // Calculate the height of the first line (char size = everything above baseline, height + top = part below baseline)
        const float lineHeight = characterSize
                                 + font.getFont()->getGlyph('g', characterSize, bold).bounds.height
                                 + font.getFont()->getGlyph('g', characterSize, bold).bounds.top;

        // Get the line spacing sfml returns
        const float lineSpacing = font.getFont()->getLineSpacing(characterSize);

        // Calculate the offset of the text
        return lineHeight - lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Text::wordWrap(float maxWidth, const sf::String& text, Font font, unsigned int textSize, bool bold, bool dropLeadingSpace)
    {
        if (font == nullptr)
            return "";

        sf::String result;
        std::size_t index = 0;
        while (index < text.getSize())
        {
            const std::size_t oldIndex = index;

            // Find out how many characters we can get on this line
            float width = 0;
            std::uint32_t prevChar = 0;
            for (std::size_t i = index; i < text.getSize(); ++i)
            {
                float charWidth;
                const std::uint32_t curChar = text[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = font.getFont()->getGlyph(' ', textSize, bold).advance * 4;
                else
                    charWidth = font.getFont()->getGlyph(curChar, textSize, bold).advance;

                const float kerning = font.getFont()->getKerning(prevChar, curChar, textSize);
                if ((maxWidth == 0) || (width + charWidth + kerning <= maxWidth))
                {
                    width += kerning + charWidth;
                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            // Every line contains at least one character
            if (index == oldIndex)
                index++;

            // Implement the word-wrap by removing the last few characters from the line
            if (text[index-1] != '\n')
            {
                const std::size_t indexWithoutWordWrap = index;
                if ((index < text.getSize()) && (!isWhitespace(text[index])))
                {
                    std::size_t wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(text[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            // If the next line starts with just a space, then the space need not be visible
            if (dropLeadingSpace)
            {
                if ((index < text.getSize()) && (text[index] == ' '))
                {
                    if ((index == 0) || (!isWhitespace(text[index-1])))
                    {
                        // But two or more spaces indicate that it is not a normal text and the spaces should not be ignored
                        if (((index + 1 < text.getSize()) && (!isWhitespace(text[index + 1]))) || (index + 1 == text.getSize()))
                            index++;
                    }
                }
            }

            result += text.substring(oldIndex, index - oldIndex);
            if ((index < text.getSize()) && (text[index-1] != '\n'))
                result += "\n";
        }

        return result;
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


#include <TGUI/Texture.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/Exception.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::list<TextureDataHolder>> TextureManager::m_imageMap;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<TextureData> TextureManager::getTexture(Texture& texture, const std::string& filename, const sf::IntRect& partRect)
    {
        // Look if we already had this image
        auto imageIt = m_imageMap.find(filename);
        if (imageIt != m_imageMap.end())
        {
            // Loop all our textures to find the one containing the image
            for (auto dataIt = imageIt->second.begin(); dataIt != imageIt->second.end(); ++dataIt)
            {
                // Only reuse the texture when the exact same part of the image is used
                if (dataIt->data->rect == partRect)
                {
                    // The texture is now used at multiple places
                    ++(dataIt->users);

                    // Let the texture alert the texture manager when it is being copied or destroyed
                    texture.setCopyCallback(&TextureManager::copyTexture);
                    texture.setDestructCallback(&TextureManager::removeTexture);
                    return dataIt->data;
                }
            }
        }
        else // The image doesn't exist yet
        {
            auto it = m_imageMap.insert({filename, {}});
            imageIt = it.first;
        }

        // Add new data to the list
        TextureDataHolder dataHolder;
        dataHolder.filename = filename;
        dataHolder.users = 1;
        dataHolder.data = std::make_shared<TextureData>();
        dataHolder.data->rect = partRect;
        imageIt->second.push_back(std::move(dataHolder));

        // Let the texture alert the texture manager when it is being copied or destroyed
        texture.setCopyCallback(&TextureManager::copyTexture);
        texture.setDestructCallback(&TextureManager::removeTexture);

        // Load the image
        auto data = imageIt->second.back().data;
        data->image = texture.getImageLoader()(filename);
        if (data->image != nullptr)
        {
            // Create a texture from the image
            bool loadFromImageSuccess;
            if (partRect == sf::IntRect{})
                loadFromImageSuccess = data->texture.loadFromImage(*data->image);
            else
                loadFromImageSuccess = data->texture.loadFromImage(*data->image, partRect);

            if (loadFromImageSuccess)
                return data;
            else
                return nullptr;
        }

        // The image could not be loaded
        m_imageMap.erase(imageIt);
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::copyTexture(std::shared_ptr<TextureData> textureDataToCopy)
    {
        // Loop all our textures to check if we already have this one
        for (auto& dataHolder : m_imageMap)
        {
            for (auto& data : dataHolder.second)
            {
                // Check if the pointer points to our texture
                if (data.data == textureDataToCopy)
                {
                    // The texture is now used at multiple places
                    ++data.users;
                    return;
                }
            }
        }

        throw Exception{"Trying to copy texture data that was not loaded by the TextureManager."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextureManager::removeTexture(std::shared_ptr<TextureData> textureDataToRemove)
    {
        // Loop all our textures to check which one it is
        for (auto imageIt = m_imageMap.begin(); imageIt != m_imageMap.end(); ++imageIt)
        {
            for (auto dataIt = imageIt->second.begin(); dataIt != imageIt->second.end(); ++dataIt)
            {
                // Check if the pointer points to our texture
                if (dataIt->data == textureDataToRemove)
                {
                    // If this was the only place where the texture is used then delete it
                    if (--(dataIt->users) == 0)
                    {
                        imageIt->second.erase(dataIt);
                        if (imageIt->second.empty())
                            m_imageMap.erase(imageIt);
                    }

                    return;
                }
            }
        }

        throw Exception{"Trying to remove a texture that was not loaded by the TextureManager."};
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
#include <TGUI/Container.hpp>
#include <TGUI/Animation.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        void addAnimation(std::vector<std::shared_ptr<priv::Animation>>& existingAnimations, std::shared_ptr<priv::Animation> newAnimation)
        {
            const auto type = newAnimation->getType();

            // If another animation is already running with the same type then instantly finish it
            unsigned int i = 0;
            while (i < existingAnimations.size())
            {
                if (existingAnimations[i]->getType() == type)
                {
                    existingAnimations[i]->finish();
                    existingAnimations.erase(existingAnimations.begin() + i);
                }
                else
                    ++i;
            }

            existingAnimations.push_back(newAnimation);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget()
    {
        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::~Widget()
    {
        // The renderer will be null when the widget was moved
        if (m_renderer)
            m_renderer->unsubscribe(this);

        for (auto& layout : m_boundPositionLayouts)
            layout->unbindWidget();

        for (auto& layout : m_boundSizeLayouts)
            layout->unbindWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(const Widget& other) :
        SignalWidgetBase               {other},
        enable_shared_from_this<Widget>{other},
        m_type                         {other.m_type},
        m_position                     {other.m_position},
        m_size                         {other.m_size},
        m_boundPositionLayouts         {},
        m_boundSizeLayouts             {},
        m_enabled                      {other.m_enabled},
        m_visible                      {other.m_visible},
        m_parent                       {nullptr},
        m_draggableWidget              {other.m_draggableWidget},
        m_containerWidget              {other.m_containerWidget},
        m_toolTip                      {other.m_toolTip ? other.m_toolTip->clone() : nullptr},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {other.m_showAnimations},
        m_fontCached                   {other.m_fontCached},
        m_opacityCached                {other.m_opacityCached}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        m_renderer->subscribe(this, m_rendererChangedCallback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Widget(Widget&& other) :
        SignalWidgetBase               {std::move(other)},
        enable_shared_from_this<Widget>{std::move(other)},
        onPositionChange               {std::move(other.onPositionChange)},
        onSizeChange                   {std::move(other.onSizeChange)},
        onFocus                        {std::move(other.onFocus)},
        onUnfocus                      {std::move(other.onUnfocus)},
        onMouseEnter                   {std::move(other.onMouseEnter)},
        onMouseLeave                   {std::move(other.onMouseLeave)},
        m_type                         {std::move(other.m_type)},
        m_position                     {std::move(other.m_position)},
        m_size                         {std::move(other.m_size)},
        m_boundPositionLayouts         {std::move(other.m_boundPositionLayouts)},
        m_boundSizeLayouts             {std::move(other.m_boundSizeLayouts)},
        m_enabled                      {std::move(other.m_enabled)},
        m_visible                      {std::move(other.m_visible)},
        m_parent                       {nullptr},
        m_mouseHover                   {std::move(other.m_mouseHover)},
        m_mouseDown                    {std::move(other.m_mouseDown)},
        m_focused                      {std::move(other.m_focused)},
        m_animationTimeElapsed         {std::move(other.m_animationTimeElapsed)},
        m_draggableWidget              {std::move(other.m_draggableWidget)},
        m_containerWidget              {std::move(other.m_containerWidget)},
        m_toolTip                      {std::move(other.m_toolTip)},
        m_renderer                     {other.m_renderer},
        m_showAnimations               {std::move(other.m_showAnimations)},
        m_fontCached                   {std::move(other.m_fontCached)},
        m_opacityCached                {std::move(other.m_opacityCached)}
    {
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        other.m_renderer->unsubscribe(&other);
        m_renderer->subscribe(this, m_rendererChangedCallback);

        other.m_renderer = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(const Widget& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);

            SignalWidgetBase::operator=(other);
            enable_shared_from_this::operator=(other);

            onPositionChange.disconnectAll();
            onSizeChange.disconnectAll();
            onFocus.disconnectAll();
            onUnfocus.disconnectAll();
            onMouseEnter.disconnectAll();
            onMouseLeave.disconnectAll();

            m_type                 = other.m_type;
            m_position             = other.m_position;
            m_size                 = other.m_size;
            m_boundPositionLayouts = {};
            m_boundSizeLayouts     = {};
            m_enabled              = other.m_enabled;
            m_visible              = other.m_visible;
            m_parent               = nullptr;
            m_mouseHover           = false;
            m_mouseDown            = false;
            m_focused              = false;
            m_animationTimeElapsed = {};
            m_draggableWidget      = other.m_draggableWidget;
            m_containerWidget      = other.m_containerWidget;
            m_toolTip              = other.m_toolTip ? other.m_toolTip->clone() : nullptr;
            m_renderer             = other.m_renderer;
            m_showAnimations       = {};
            m_fontCached           = other.m_fontCached;
            m_opacityCached        = other.m_opacityCached;

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget& Widget::operator=(Widget&& other)
    {
        if (this != &other)
        {
            m_renderer->unsubscribe(this);
            other.m_renderer->unsubscribe(&other);

            SignalWidgetBase::operator=(std::move(other));
            enable_shared_from_this::operator=(std::move(other));

            onPositionChange       = std::move(other.onPositionChange);
            onSizeChange           = std::move(other.onSizeChange);
            onFocus                = std::move(other.onFocus);
            onUnfocus              = std::move(other.onUnfocus);
            onMouseEnter           = std::move(other.onMouseEnter);
            onMouseLeave           = std::move(other.onMouseLeave);
            m_type                 = std::move(other.m_type);
            m_position             = std::move(other.m_position);
            m_size                 = std::move(other.m_size);
            m_boundPositionLayouts = std::move(other.m_boundPositionLayouts);
            m_boundSizeLayouts     = std::move(other.m_boundSizeLayouts);
            m_enabled              = std::move(other.m_enabled);
            m_visible              = std::move(other.m_visible);
            m_parent               = nullptr;
            m_mouseHover           = std::move(other.m_mouseHover);
            m_mouseDown            = std::move(other.m_mouseDown);
            m_focused              = std::move(other.m_focused);
            m_animationTimeElapsed = std::move(other.m_animationTimeElapsed);
            m_draggableWidget      = std::move(other.m_draggableWidget);
            m_containerWidget      = std::move(other.m_containerWidget);
            m_toolTip              = std::move(other.m_toolTip);
            m_renderer             = std::move(other.m_renderer);
            m_showAnimations       = std::move(other.m_showAnimations);
            m_fontCached           = std::move(other.m_fontCached);
            m_opacityCached        = std::move(other.m_opacityCached);

            m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
            m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
            m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
            m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

            m_renderer->subscribe(this, m_rendererChangedCallback);

            other.m_renderer = nullptr;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setRenderer(std::shared_ptr<RendererData> rendererData)
    {
        if (rendererData == nullptr)
            rendererData = RendererData::create();

        std::shared_ptr<RendererData> oldData = m_renderer->getData();

        // Update the data
        m_renderer->unsubscribe(this);
        m_renderer->setData(rendererData);
        m_renderer->subscribe(this, m_rendererChangedCallback);
        rendererData->shared = true;

        // Tell the widget about all the updated properties, both new ones and old ones that were now reset to their default value
        auto oldIt = oldData->propertyValuePairs.begin();
        auto newIt = rendererData->propertyValuePairs.begin();
        while (oldIt != oldData->propertyValuePairs.end() && newIt != rendererData->propertyValuePairs.end())
        {
            if (oldIt->first < newIt->first)
            {
                // Update values that no longer exist in the new renderer and are now reset to the default value
                rendererChanged(oldIt->first);
                ++oldIt;
            }
            else
            {
                // Update changed and new properties
                rendererChanged(newIt->first);

                if (newIt->first < oldIt->first)
                    ++newIt;
                else
                {
                    ++oldIt;
                    ++newIt;
                }
            }
        }
        while (oldIt != oldData->propertyValuePairs.end())
        {
            rendererChanged(oldIt->first);
            ++oldIt;
        }
        while (newIt != rendererData->propertyValuePairs.end())
        {
            rendererChanged(newIt->first);
            ++newIt;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getSharedRenderer() const
    {
        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getSharedRenderer()
    {
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetRenderer* Widget::getRenderer() const
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        // You should not be allowed to call setters on the renderer when the widget is const
        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetRenderer* Widget::getRenderer()
    {
        if (m_renderer->getData()->shared)
        {
            m_renderer->unsubscribe(this);
            m_renderer->setData(m_renderer->clone());
            m_renderer->subscribe(this, m_rendererChangedCallback);
            m_renderer->getData()->shared = false;
        }

        return m_renderer.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setPosition(const Layout2d& position)
    {
        m_position = position;
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });

        if (getPosition() != m_prevPosition)
        {
            m_prevPosition = getPosition();
            onPositionChange.emit(this, getPosition());

            for (auto& layout : m_boundPositionLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setSize(const Layout2d& size)
    {
        m_size = size;
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });

        if (getSize() != m_prevSize)
        {
            m_prevSize = getSize();
            onSizeChange.emit(this, getSize());

            for (auto& layout : m_boundSizeLayouts)
                layout->recalculateValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getFullSize() const
    {
        return getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getAbsolutePosition() const
    {
        if (m_parent)
            return m_parent->getAbsolutePosition() + m_parent->getChildWidgetsOffset() + getPosition();
        else
            return getPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Widget::getWidgetOffset() const
    {
        return Vector2f{0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::showWithEffect(ShowAnimationType type, sf::Time duration)
    {
        setVisible(true);

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), 0.f, getInheritedOpacity(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setInheritedOpacity(0);
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), getPosition() + (getSize() / 2.f), getPosition(), duration));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), Vector2f{0, 0}, getSize(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition(getPosition() + (getSize() / 2.f));
                setSize(0, 0);
                break;
            }
            case ShowAnimationType::SlideFromLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{-getFullSize().x, getPosition().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition({-getFullSize().x, getPosition().y});
                break;
            }
            case ShowAnimationType::SlideFromRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getParent()->getSize().x + getWidgetOffset().x, getPosition().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getParent()->getSize().x + getWidgetOffset().x, getPosition().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromRight) does not work before widget has a parent.");
                }

                break;
            }
            case ShowAnimationType::SlideFromTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, -getFullSize().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                setPosition({getPosition().x, -getFullSize().y});
                break;
            }
            case ShowAnimationType::SlideFromBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), Vector2f{getPosition().x, getParent()->getSize().y + getWidgetOffset().y}, getPosition(), duration, [=]{onAnimationFinished.emit(this, type, true); }));
                    setPosition({getPosition().x, getParent()->getSize().y + getWidgetOffset().y});
                }
                else
                {
                    TGUI_PRINT_WARNING("showWithEffect(SlideFromBottom) does not work before widget has a parent.");
                }

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::hideWithEffect(ShowAnimationType type, sf::Time duration)
    {
        const auto position = getPosition();
        const auto size = getSize();

        switch (type)
        {
            case ShowAnimationType::Fade:
            {
                float opacity = getInheritedOpacity();
                addAnimation(m_showAnimations, std::make_shared<priv::FadeAnimation>(shared_from_this(), getInheritedOpacity(), 0.f, duration,
                    [=](){ setVisible(false); setInheritedOpacity(opacity); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::Scale:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, position + (size / 2.f), duration, [=](){ setVisible(false); setPosition(position); setSize(size); }));
                addAnimation(m_showAnimations, std::make_shared<priv::ResizeAnimation>(shared_from_this(), size, Vector2f{0, 0}, duration,
                    [=](){ setVisible(false); setPosition(position); setSize(size); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToRight:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{getParent()->getSize().x + getWidgetOffset().x, position.y}, duration,
                        [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToRight) does not work before widget has a parent.");

                break;
            }
            case ShowAnimationType::SlideToLeft:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{-getFullSize().x, position.y}, duration,
                    [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
            case ShowAnimationType::SlideToBottom:
            {
                if (getParent())
                {
                    addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, getParent()->getSize().y + getWidgetOffset().y}, duration,
                        [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                }
                else
                    TGUI_PRINT_WARNING("hideWithEffect(SlideToBottom) does not work before widget has a parent.");

                break;
            }
            case ShowAnimationType::SlideToTop:
            {
                addAnimation(m_showAnimations, std::make_shared<priv::MoveAnimation>(shared_from_this(), position, Vector2f{position.x, -getFullSize().y}, duration,
                    [=](){ setVisible(false); setPosition(position); onAnimationFinished.emit(this, type, false); }));
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setVisible(bool visible)
    {
        m_visible = visible;

        // If the widget is hiden while still focused then it must be unfocused
        if (!visible)
            setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setEnabled(bool enabled)
    {
        m_enabled = enabled;

        if (!enabled)
        {
            m_mouseHover = false;
            m_mouseDown = false;
            setFocused(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setFocused(bool focused)
    {
        if (m_focused == focused)
            return;

        if (focused)
        {
            if (canGainFocus())
            {
                m_focused = true;
                onFocus.emit(this);

                if (m_parent)
                    m_parent->childWidgetFocused(shared_from_this());
            }
        }
        else // Unfocusing widget
        {
            m_focused = false;
            onUnfocus.emit(this);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Widget::getWidgetType() const
    {
        return m_type;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToFront()
    {
        if (m_parent)
            m_parent->moveWidgetToFront(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::moveToBack()
    {
        if (m_parent)
            m_parent->moveWidgetToBack(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedFont(const Font& font)
    {
        m_inheritedFont = font;
        rendererChanged("font");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Font& Widget::getInheritedFont() const
    {
        return m_inheritedFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setInheritedOpacity(float opacity)
    {
        m_inheritedOpacity = opacity;
        rendererChanged("opacity");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Widget::getInheritedOpacity() const
    {
        return m_inheritedOpacity;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setToolTip(Widget::Ptr toolTip)
    {
        m_toolTip = toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::getToolTip() const
    {
        return m_toolTip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::canGainFocus() const
    {
        return m_enabled && m_visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::isContainer() const
    {
        return m_containerWidget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::setParent(Container* parent)
    {
        m_parent = parent;

        // Give the layouts another chance to find widgets to which it refers
        m_position.x.connectWidget(this, true, [this]{ setPosition(getPositionLayout()); });
        m_position.y.connectWidget(this, false, [this]{ setPosition(getPositionLayout()); });
        m_size.x.connectWidget(this, true, [this]{ setSize(getSizeLayout()); });
        m_size.y.connectWidget(this, false, [this]{ setSize(getSizeLayout()); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::update(sf::Time elapsedTime)
    {
        m_animationTimeElapsed += elapsedTime;

        for (unsigned int i = 0; i < m_showAnimations.size();)
        {
            if (m_showAnimations[i]->update(elapsedTime))
                m_showAnimations.erase(m_showAnimations.begin() + i);
            else
                i++;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::leftMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMousePressed(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rightMouseReleased(Vector2f)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseMoved(Vector2f)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::keyPressed(const sf::Event::KeyEvent&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::textEntered(std::uint32_t)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Widget::mouseWheelScrolled(float, Vector2f)
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Widget::askToolTip(Vector2f mousePos)
    {
        if (m_toolTip && mouseOnWidget(mousePos))
            return getToolTip();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindPositionLayout(Layout* layout)
    {
        m_boundPositionLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::bindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.insert(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::unbindSizeLayout(Layout* layout)
    {
        m_boundSizeLayouts.erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Widget::getSignal(std::string signalName)
    {
        if (signalName == toLower(onPositionChange.getName()))
            return onPositionChange;
        else if (signalName == toLower(onSizeChange.getName()))
            return onSizeChange;
        else if (signalName == toLower(onFocus.getName()))
            return onFocus;
        else if (signalName == toLower(onUnfocus.getName()))
            return onUnfocus;
        else if (signalName == toLower(onMouseEnter.getName()))
            return onMouseEnter;
        else if (signalName == toLower(onMouseLeave.getName()))
            return onMouseLeave;
        else if (signalName == toLower(onAnimationFinished.getName()))
            return onAnimationFinished;

        throw Exception{"No signal exists with name '" + std::move(signalName) + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChanged(const std::string& property)
    {
        if (property == "opacity")
        {
            m_opacityCached = getSharedRenderer()->getOpacity() * m_inheritedOpacity;
        }
        else if (property == "font")
        {
            if (getSharedRenderer()->getFont())
                m_fontCached = getSharedRenderer()->getFont();
            else if (m_inheritedFont)
                m_fontCached = m_inheritedFont;
            else
                m_fontCached = getGlobalFont();
        }
        else if (property == "transparenttexture")
        {
            m_transparentTextureCached = getSharedRenderer()->getTransparentTexture();
        }
        else
            throw Exception{"Could not set property '" + property + "', widget of type '" + getWidgetType() + "' does not has this property."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Widget::save(SavingRenderersMap& renderers) const
    {
        sf::String widgetName;
        if (getParent())
            widgetName = getParent()->getWidgetName(shared_from_this());

        auto node = std::make_unique<DataIO::Node>();
        if (widgetName.isEmpty())
            node->name = getWidgetType();
        else
            node->name = getWidgetType() + "." + Serializer::serialize(widgetName);

        if (!isVisible())
            node->propertyValuePairs["Visible"] = std::make_unique<DataIO::ValueNode>("false");
        if (!isEnabled())
            node->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>("false");
        if (getPosition() != Vector2f{})
            node->propertyValuePairs["Position"] = std::make_unique<DataIO::ValueNode>(m_position.toString());
        if (getSize() != Vector2f{})
            node->propertyValuePairs["Size"] = std::make_unique<DataIO::ValueNode>(m_size.toString());

        if (getToolTip() != nullptr)
        {
            auto toolTipWidgetNode = getToolTip()->save(renderers);

            auto toolTipNode = std::make_unique<DataIO::Node>();
            toolTipNode->name = "ToolTip";
            toolTipNode->children.emplace_back(std::move(toolTipWidgetNode));

            toolTipNode->propertyValuePairs["TimeToDisplay"] = std::make_unique<DataIO::ValueNode>(to_string(ToolTip::getTimeToDisplay().asSeconds()));
            toolTipNode->propertyValuePairs["DistanceToMouse"] = std::make_unique<DataIO::ValueNode>("(" + to_string(ToolTip::getDistanceToMouse().x) + "," + to_string(ToolTip::getDistanceToMouse().y) + ")");

            node->children.emplace_back(std::move(toolTipNode));
        }

        if (renderers.at(this).first)
            node->children.emplace_back(std::move(renderers.at(this).first));
        else
            node->propertyValuePairs["renderer"] = std::make_unique<DataIO::ValueNode>("&" + renderers.at(this).second);

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        auto parseLayout = [](std::string str) -> Layout2d
            {
                if (str.empty())
                    throw Exception{"Failed to parse layout. String was empty."};

                // Remove the brackets around the value
                if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                    str = str.substr(1, str.length() - 2);

                if (str.empty())
                    return {0, 0};

                const auto commaPos = str.find(',');
                if (commaPos == std::string::npos)
                    throw Exception{"Failed to parse layout '" + str + "'. Expected expressions separated with a comma."};

                // Remove quotes around the values
                std::string x = trim(str.substr(0, commaPos));
                if ((x.size() >= 2) && ((x[0] == '"') && (x[x.length()-1] == '"')))
                    x = x.substr(1, x.length()-2);

                std::string y = trim(str.substr(commaPos + 1));
                if ((y.size() >= 2) && ((y[0] == '"') && (y[y.length()-1] == '"')))
                    y = y.substr(1, y.length()-2);

                return {x, y};
            };

        if (node->propertyValuePairs["visible"])
            setVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["visible"]->value).getBool());
        if (node->propertyValuePairs["enabled"])
            setEnabled(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["enabled"]->value).getBool());
        if (node->propertyValuePairs["position"])
            setPosition(parseLayout(node->propertyValuePairs["position"]->value));
        if (node->propertyValuePairs["size"])
            setSize(parseLayout(node->propertyValuePairs["size"]->value));

        if (node->propertyValuePairs["renderer"])
        {
            const sf::String value = node->propertyValuePairs["renderer"]->value;
            if (value.isEmpty() || (value[0] != '&'))
                throw Exception{"Expected reference to renderer, did not find '&' character"};

            const auto it = renderers.find(toLower(value.substring(1)));
            if (it == renderers.end())
                throw Exception{"Widget refers to renderer with name '" + value.substring(1) + "', but no such renderer was found"};

            setRenderer(it->second);
        }

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "tooltip")
            {
                for (const auto& pair : childNode->propertyValuePairs)
                {
                    if (pair.first == "timetodisplay")
                        ToolTip::setTimeToDisplay(sf::seconds(tgui::stof(pair.second->value)));
                    else if (pair.first == "distancetomouse")
                        ToolTip::setDistanceToMouse(Vector2f{pair.second->value});
                }

                if (!childNode->children.empty())
                {
                    // There can only be one child in the tool tip section
                    if (childNode->children.size() > 1)
                        throw Exception{"ToolTip section contained multiple children."};

                    const auto& toolTipWidgetNode = childNode->children[0];
                    const auto& constructor = WidgetFactory::getConstructFunction(toolTipWidgetNode->name);
                    if (constructor)
                    {
                        Widget::Ptr toolTip = constructor();
                        toolTip->load(toolTipWidgetNode, renderers);
                        setToolTip(toolTip);
                    }
                    else
                        throw Exception{"No construct function exists for widget type '" + toolTipWidgetNode->name + "'."};
                }
            }
            else if (toLower(childNode->name) == "renderer")
                setRenderer(RendererData::createFromDataIONode(childNode.get()));

            /// TODO: Signals?
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), [](const std::unique_ptr<DataIO::Node>& child){
                return (toLower(child->name) == "tooltip") || (toLower(child->name) == "renderer");
            }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseEnteredWidget()
    {
        m_mouseHover = true;
        onMouseEnter.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::mouseLeftWidget()
    {
        m_mouseHover = false;
        onMouseLeave.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::rendererChangedCallback(const std::string& property)
    {
        rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawRectangleShape(sf::RenderTarget& target,
                                    const sf::RenderStates& states,
                                    Vector2f size,
                                    Color color) const
    {
        sf::RectangleShape shape{size};

        if (m_opacityCached < 1)
            shape.setFillColor(Color::calcColorOpacity(color, m_opacityCached));
        else
            shape.setFillColor(color);

        target.draw(shape, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Widget::drawBorders(sf::RenderTarget& target,
                             const sf::RenderStates& states,
                             const Borders& borders,
                             Vector2f size,
                             Color borderColor) const
    {
        const Color color = Color::calcColorOpacity(borderColor, m_opacityCached);

        // If size is too small then draw entire size as border
        if ((size.x <= borders.getLeft() + borders.getRight()) || (size.y <= borders.getTop() + borders.getBottom()))
        {
            sf::RectangleShape border;
            border.setFillColor(color);
            border.setSize({size.x, size.y});
            target.draw(border, states);
        }
        else // Draw borders in the normal way
        {
            //////////////////////
            // 0--1/8---------6 //
            // |              | //
            // |   9------7   | //
            // |   |      |   | //
            // |   |      |   | //
            // |   3------5   | //
            // |              | //
            // 2--------------4 //
            //////////////////////
            const std::vector<sf::Vertex> vertices = {
                {{0, 0}, color},
                {{borders.getLeft(), 0}, color},
                {{0, size.y}, color},
                {{borders.getLeft(), size.y - borders.getBottom()}, color},
                {{size.x, size.y}, color},
                {{size.x - borders.getRight(), size.y - borders.getBottom()}, color},
                {{size.x, 0}, color},
                {{size.x - borders.getRight(), borders.getTop()}, color},
                {{borders.getLeft(), 0}, color},
                {{borders.getLeft(), borders.getTop()}, color}
            };

            target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
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
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/Global.hpp>
#include <cstdint>
#include <cassert>

namespace tgui
{
    namespace
    {
        unsigned char hexToDec(char c)
        {
            assert((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')  || (c >= 'a' && c <= 'f'));

            if (c == 'A' || c == 'a')
                return 10;
            else if (c == 'B' || c == 'b')
                return 11;
            else if (c == 'C' || c == 'c')
                return 12;
            else if (c == 'D' || c == 'd')
                return 13;
            else if (c == 'E' || c == 'e')
                return 14;
            else if (c == 'F' || c == 'f')
                return 15;
            else // if (c >= '0' && c <= '9')
                return c - '0';
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool readIntRect(std::string value, sf::IntRect& rect)
        {
            if (!value.empty() && (value[0] == '(') && (value[value.length()-1] == ')'))
            {
                std::vector<std::string> tokens = Deserializer::split(value.substr(1, value.size()-2), ',');
                if (tokens.size() == 4)
                {
                    rect = {tgui::stoi(tokens[0]), tgui::stoi(tokens[1]), tgui::stoi(tokens[2]), tgui::stoi(tokens[3])};
                    return true;
                }
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeBool(const std::string& value)
        {
            const std::string str = toLower(value);
            if (str == "true" || str == "yes" || str == "on" || str == "1")
                return {true};
            else if (str == "false" || str == "no" || str == "off" || str == "0")
                return {false};
            else
                throw Exception{"Failed to deserialize boolean from '" + str + "'"};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeFont(const std::string& value)
        {
            if (value == "null" || value == "nullptr")
                return Font{};

            sf::String filename = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();
            if (filename.isEmpty())
                return Font{};

            // Load the font but insert the resource path into the filename unless the filename is an absolute path
            auto font = std::make_shared<sf::Font>();
        #ifdef SFML_SYSTEM_WINDOWS
            if ((filename[0] != '/') && (filename[0] != '\\') && ((filename.getSize() <= 1) || (filename[1] != ':')))
        #else
            if (filename[0] != '/')
        #endif
                font->loadFromFile(getResourcePath() + filename);
            else
                font->loadFromFile(filename);

            return Font(font);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeColor(const std::string& value)
        {
            std::string str = trim(value);

            // Make sure that the line isn't empty
            if (!str.empty())
            {
                // Check if the color is represented by a string with its name
                const auto it = Color::colorMap.find(toLower(str));
                if (it != Color::colorMap.end())
                    return it->second;

                // The color can be represented with a hexadecimal number
                if (str[0] == '#')
                {
                    // You can only have hex characters
                    for (std::size_t i = 1; i < value.length(); ++i)
                    {
                        if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'A' && value[i] <= 'F')  || (value[i] >= 'a' && value[i] <= 'f')))
                            throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but contained an invalid character afterwards."};
                    }

                    // Parse the different types of strings (#123, #1234, #112233 and #11223344)
                    if (value.length() == 4)
                    {
                        return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                     static_cast<std::uint8_t>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                     static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[3]))};
                    }
                    else if (value.length() == 5)
                    {
                        return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[1])),
                                     static_cast<std::uint8_t>(hexToDec(value[2]) * 16 + hexToDec(value[2])),
                                     static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[3])),
                                     static_cast<std::uint8_t>(hexToDec(value[4]) * 16 + hexToDec(value[4]))};
                    }
                    else if (value.length() == 7)
                    {
                        return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                     static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                     static_cast<std::uint8_t>(hexToDec(value[5]) * 16 + hexToDec(value[6]))};
                    }
                    else if (value.length() == 9)
                    {
                        return Color{static_cast<std::uint8_t>(hexToDec(value[1]) * 16 + hexToDec(value[2])),
                                     static_cast<std::uint8_t>(hexToDec(value[3]) * 16 + hexToDec(value[4])),
                                     static_cast<std::uint8_t>(hexToDec(value[5]) * 16 + hexToDec(value[6])),
                                     static_cast<std::uint8_t>(hexToDec(value[7]) * 16 + hexToDec(value[8]))};
                    }
                    else
                        throw Exception{"Failed to deserialize color '" + value + "'. Value started but '#' but has the wrong length."};
                }

                // The string can optionally start with "rgb" or "rgba", but this is ignored
                if (str.substr(0, 4) == "rgba")
                    str.erase(0, 4);
                else if (str.substr(0, 3) == "rgb")
                    str.erase(0, 3);

                // Remove the first and last characters when they are brackets
                if ((str[0] == '(') && (str[str.length()-1] == ')'))
                    str = str.substr(1, str.length()-2);

                const std::vector<std::string> tokens = Deserializer::split(str, ',');
                if (tokens.size() == 3 || tokens.size() == 4)
                {
                    return Color{static_cast<std::uint8_t>(tgui::stoi(tokens[0])),
                                 static_cast<std::uint8_t>(tgui::stoi(tokens[1])),
                                 static_cast<std::uint8_t>(tgui::stoi(tokens[2])),
                                 static_cast<std::uint8_t>((tokens.size() == 4) ? tgui::stoi(tokens[3]) : 255)};
                }
            }

            throw Exception{"Failed to deserialize color '" + value + "'."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeString(const std::string& value)
        {
            // Only deserialize the string when it is surrounded with quotes
            if ((value.size() >= 2) && ((value[0] == '"') && (value[value.length()-1] == '"')))
            {
                std::string result = value.substr(1, value.length()-2);

                std::size_t backslashPos = 0;
                while ((backslashPos = result.find('\\', backslashPos)) < result.size()-1)
                {
                    result.erase(backslashPos, 1);

                    if (result[backslashPos] == 'n')
                        result[backslashPos] = '\n';
                    else if (result[backslashPos] == 't')
                        result[backslashPos] = '\t';
                    else if (result[backslashPos] == 'v')
                        result[backslashPos] = '\v';

                    backslashPos++;
                }

                return {sf::String{result}};
            }
            else
                return {sf::String{value}};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeNumber(const std::string& value)
        {
            return {tgui::stof(value)};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeOutline(const std::string& value)
        {
            std::string str = trim(value);

            if (str.empty())
                throw Exception{"Failed to deserialize outline '" + value + "'. String was empty."};

            // Remove the brackets around the value
            if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
                str = str.substr(1, str.length() - 2);

            if (str.empty())
                return {Outline{0}};

            const std::vector<std::string> tokens = Deserializer::split(str, ',');
            if (tokens.size() == 1)
                return {Outline{tokens[0]}};
            else if (tokens.size() == 2)
                return {Outline{tokens[0], tokens[1]}};
            else if (tokens.size() == 4)
                return {Outline{tokens[0], tokens[1], tokens[2], tokens[3]}};
            else
                throw Exception{"Failed to deserialize outline '" + value + "'. Expected numbers separated with a comma."};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTexture(const std::string& value)
        {
            if (value.empty() || (toLower(value) == "none"))
                return Texture{};

            // If there are no quotes then the value just contains a filename
            if (value[0] != '"')
            {
                // Load the texture but insert the resource path into the filename unless the filename is an absolute path
            #ifdef SFML_SYSTEM_WINDOWS
                if ((value[0] != '/') && (value[0] != '\\') && ((value.size() <= 1) || (value[1] != ':')))
            #else
                if (value[0] != '/')
            #endif
                    return Texture{getResourcePath() + value};
                else
                    return Texture{value};
            }

            std::string::const_iterator c = value.begin();
            ++c; // Skip the opening quote

            std::string filename;
            char prev = '\0';

            // Look for the end quote
            bool filenameFound = false;
            while (c != value.end())
            {
                if ((*c != '"') || (prev == '\\'))
                {
                    prev = *c;
                    filename.push_back(*c);
                    ++c;
                }
                else
                {
                    ++c;
                    filenameFound = true;
                    break;
                }
            }

            if (!filenameFound)
                throw Exception{"Failed to deserialize texture '" + value + "'. Failed to find the closing quote of the filename."};

            // There may be optional parameters
            sf::IntRect partRect;
            sf::IntRect middleRect;
            bool smooth = false;

            while (removeWhitespace(value, c))
            {
                std::string word;
                auto openingBracketPos = value.find('(', c - value.begin());
                if (openingBracketPos != std::string::npos)
                    word = value.substr(c - value.begin(), openingBracketPos - (c - value.begin()));
                else
                {
                    if (toLower(trim(value.substr(c - value.begin()))) == "smooth")
                    {
                        smooth = true;
                        break;
                    }
                    else
                        throw Exception{"Failed to deserialize texture '" + value + "'. Invalid text found behind filename."};
                }

                sf::IntRect* rect = nullptr;
                if ((word == "Part") || (word == "part"))
                {
                    rect = &partRect;
                    std::advance(c, 4);
                }
                else if ((word == "Middle") || (word == "middle"))
                {
                    rect = &middleRect;
                    std::advance(c, 6);
                }
                else
                {
                    if (word.empty())
                        throw Exception{"Failed to deserialize texture '" + value + "'. Expected 'Part' or 'Middle' in front of opening bracket."};
                    else
                        throw Exception{"Failed to deserialize texture '" + value + "'. Unexpected word '" + word + "' in front of opening bracket. Expected 'Part' or 'Middle'."};
                }

                auto closeBracketPos = value.find(')', c - value.begin());
                if (closeBracketPos != std::string::npos)
                {
                    if (!readIntRect(value.substr(c - value.begin(), closeBracketPos - (c - value.begin()) + 1), *rect))
                        throw Exception{"Failed to parse " + word + " rectangle while deserializing texture '" + value + "'."};
                }
                else
                    throw Exception{"Failed to deserialize texture '" + value + "'. Failed to find closing bracket for " + word + " rectangle."};

                std::advance(c, closeBracketPos - (c - value.begin()) + 1);
            }

            return Texture{filename, partRect, middleRect, smooth};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeTextStyle(const std::string& style)
        {
            unsigned int decodedStyle = sf::Text::Regular;
            std::vector<std::string> styles = Deserializer::split(style, '|');
            for (const auto& elem : styles)
            {
                std::string requestedStyle = toLower(elem);
                if (requestedStyle == "bold")
                    decodedStyle |= sf::Text::Bold;
                else if (requestedStyle == "italic")
                    decodedStyle |= sf::Text::Italic;
                else if (requestedStyle == "underlined")
                    decodedStyle |= sf::Text::Underlined;
                else if (requestedStyle == "strikethrough")
                    decodedStyle |= sf::Text::StrikeThrough;
            }

            return TextStyle(decodedStyle);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ObjectConverter deserializeRendererData(const std::string& renderer)
        {
            std::stringstream ss{renderer};
            auto node = DataIO::parse(ss);

            // The root node should contain exactly one child which is the node we need
            if (node->propertyValuePairs.empty() && (node->children.size() == 1))
                node = std::move(node->children[0]);

            auto rendererData = RendererData::create();
            for (const auto& pair : node->propertyValuePairs)
                rendererData->propertyValuePairs[pair.first] = ObjectConverter(pair.second->value); // Did not compile with VS2015 Update 2 when using braces

            for (const auto& child : node->children)
            {
                std::stringstream ss2;
                DataIO::emit(child, ss2);
                rendererData->propertyValuePairs[toLower(child->name)] = {sf::String{"{\n" + ss2.str() + "}"}};
            }

            return rendererData;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ObjectConverter::Type, Deserializer::DeserializeFunc> Deserializer::m_deserializers =
        {
            {ObjectConverter::Type::Bool, deserializeBool},
            {ObjectConverter::Type::Font, deserializeFont},
            {ObjectConverter::Type::Color, deserializeColor},
            {ObjectConverter::Type::String, deserializeString},
            {ObjectConverter::Type::Number, deserializeNumber},
            {ObjectConverter::Type::Outline, deserializeOutline},
            {ObjectConverter::Type::Texture, deserializeTexture},
            {ObjectConverter::Type::TextStyle, deserializeTextStyle},
            {ObjectConverter::Type::RendererData, deserializeRendererData}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter Deserializer::deserialize(ObjectConverter::Type type, const std::string& serializedString)
    {
        assert(m_deserializers.find(type) != m_deserializers.end());
        return m_deserializers[type](serializedString);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Deserializer::setFunction(ObjectConverter::Type type, const DeserializeFunc& deserializer)
    {
        m_deserializers[type] = deserializer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Deserializer::DeserializeFunc& Deserializer::getFunction(ObjectConverter::Type type)
    {
        return m_deserializers[type];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> Deserializer::split(const std::string& str, char delim)
    {
        std::vector<std::string> tokens;

        std::size_t start = 0;
        std::size_t end = 0;
        while ((end = str.find(delim, start)) != std::string::npos) {
            tokens.push_back(trim(str.substr(start, end - start)));
            start = end + 1;
        }

        tokens.push_back(trim(str.substr(start)));
        return tokens;
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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        struct DefaultTheme : public Theme
        {
            DefaultTheme()
            {
                m_renderers = {
                    {"button", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"bordercolordown", Color::Black},
                                                     {"bordercolorfocused", Color{30, 30, 180}},
                                                     {"bordercolordisabled", Color{125, 125, 125}},
                                                     {"textcolor", Color{60, 60, 60}},
                                                     {"textcolorhover", Color::Black},
                                                     {"textcolordown", Color::Black},
                                                     {"textcolordisabled", Color{125, 125, 125}},
                                                     {"backgroundcolor", Color{245, 245, 245}},
                                                     {"backgroundcolorhover", Color::White},
                                                     {"backgroundcolordown", Color{235, 235, 235}},
                                                     {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"chatbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"backgroundcolor", Color{245, 245, 245}}})},
                    {"checkbox", RendererData::create({{"borders", Borders{1}},
                                                       {"bordercolor", Color{60, 60, 60}},
                                                       {"bordercolorhover", Color::Black},
                                                       {"bordercolorfocused", Color{30, 30, 180}},
                                                       {"bordercolordisabled", Color{125, 125, 125}},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"textcolordisabled", Color{125, 125, 125}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"backgroundcolordisabled", Color{230, 230, 230}},
                                                       {"checkcolor", Color{60, 60, 60}},
                                                       {"checkcolorhover", Color::Black},
                                                       {"checkcolordisabled", Color{125, 125, 125}},
                                                       {"textdistanceratio", 0.2f}})},
                    {"childwindow", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"titlecolor", Color{60, 60, 60}},
                                                          {"titlebarcolor", Color::White},
                                                          {"backgroundcolor", Color{230, 230, 230}},
                                                          {"distancetoside", 3.f},
                                                          {"borderbelowtitlebar", 1.f},
                                                          {"paddingbetweenbuttons", 1.f},
                                                          {"showtextontitlebuttons", true}})},
                    {"childwindowbutton", RendererData::create({{"borders", Borders{1}},
                                                                {"bordercolor", Color{60, 60, 60}},
                                                                {"bordercolorhover", Color::Black},
                                                                {"bordercolordown", Color::Black},
                                                                {"textcolor", Color{60, 60, 60}},
                                                                {"textcolorhover", Color::Black},
                                                                {"textcolordown", Color::Black},
                                                                {"backgroundcolor", Color{245, 245, 245}},
                                                                {"backgroundcolorhover", Color::White},
                                                                {"backgroundcolordown", Color{235, 235, 235}}})},
                    {"combobox", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"arrowcolor", Color{60, 60, 60}},
                                                       {"arrowcolorhover", Color::Black},
                                                       {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                       {"arrowbackgroundcolorhover", Color::White}})},
                    {"editbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color{60, 60, 60}},
                                                      {"bordercolorhover", Color::Black},
                                                      {"bordercolordisabled", Color{125, 125, 125}},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolordisabled", Color{125, 125, 125}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"defaulttextcolor", Color{160, 160, 160}},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"group", RendererData::create()},
                    {"horizontallayout", RendererData::create()},
                    {"horizontalwrap", RendererData::create()},
                    {"knob", RendererData::create({{"borders", Borders{2}},
                                                   {"imagerotation", 0.f},
                                                   {"bordercolor", Color::Black},
                                                   {"thumbcolor", Color::Black},
                                                   {"backgroundcolor", Color::White}})},
                    {"label", RendererData::create({{"borders", Borders{}},
                                                    {"bordercolor", Color{60, 60, 60}},
                                                    {"textcolor", Color{60, 60, 60}},
                                                    {"backgroundcolor", Color::Transparent}})},
                    {"listbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolorhover", Color::Black},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                      {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"menubar", RendererData::create({{"textcolor", Color{60, 60, 60}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                      {"textcolordisabled", Color{125, 125, 125}}})},
                    {"messagebox", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"titlecolor", Color{60, 60, 60}},
                                                         {"titlebarcolor", Color::White},
                                                         {"backgroundcolor", Color{230, 230, 230}},
                                                         {"distancetoside", 3.f},
                                                         {"paddingbetweenbuttons", 1.f},
                                                         {"textcolor", Color::Black}})},
                    {"panel", RendererData::create({{"bordercolor", Color::Black},
                                                    {"backgroundcolor", Color::White}})},
                    {"picture", RendererData::create()},
                    {"progressbar", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorfilled", Color::White},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"fillcolor", Color{0, 110, 255}}})},
                    {"radiobutton", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"bordercolorfocused", Color{30, 30, 180}},
                                                          {"bordercolordisabled", Color{125, 125, 125}},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorhover", Color::Black},
                                                          {"textcolordisabled", Color{125, 125, 125}},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"backgroundcolorhover", Color::White},
                                                          {"backgroundcolordisabled", Color{230, 230, 230}},
                                                          {"checkcolor", Color{60, 60, 60}},
                                                          {"checkcolorhover", Color::Black},
                                                          {"checkcolordisabled", Color{125, 125, 125}},
                                                          {"textdistanceratio", 0.2f}})},
                    {"rangeslider", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"trackcolor", Color{245, 245, 245}},
                                                          {"trackcolorhover", Color{255, 255, 255}},
                                                          {"thumbcolor", Color{245, 245, 245}},
                                                          {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"scrollablepanel", RendererData::create({{"bordercolor", Color::Black},
                                                              {"backgroundcolor", Color::White}})},
                    {"scrollbar", RendererData::create({{"trackcolor", Color{245, 245, 245}},
                                                        {"thumbcolor", Color{220, 220, 220}},
                                                        {"thumbcolorhover", Color{230, 230, 230}},
                                                        {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                        {"arrowbackgroundcolorhover", Color{255, 255, 255}},
                                                        {"arrowcolor", Color{60, 60, 60}},
                                                        {"arrowcolorhover", Color{0, 0, 0}}})},
                    {"slider", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"trackcolor", Color{245, 245, 245}},
                                                     {"trackcolorhover", Color{255, 255, 255}},
                                                     {"thumbcolor", Color{245, 245, 245}},
                                                     {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"spinbutton", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"backgroundcolor", Color{245, 245, 245}},
                                                         {"backgroundcolorhover", Color::White},
                                                         {"arrowcolor", Color{60, 60, 60}},
                                                         {"arrowcolorhover", Color::Black},
                                                         {"borderbetweenarrows", 2.f}})},
                    {"tabs", RendererData::create({{"borders", Borders{1}},
                                                   {"bordercolor", Color::Black},
                                                   {"textcolor", Color{60, 60, 60}},
                                                   {"textcolorhover", Color::Black},
                                                   {"textcolordisabled", Color{125, 125, 125}},
                                                   {"selectedtextcolor", Color::White},
                                                   {"backgroundcolor", Color{245, 245, 245}},
                                                   {"backgroundcolorhover", Color::White},
                                                   {"backgroundcolordisabled", Color{230, 230, 230}},
                                                   {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                   {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"textbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"backgroundcolor", Color::White}})},
                    {"treeview", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"selectedtextcolor", Color::White},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                       {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"verticallayout", RendererData::create({})}
               };
            }
        };



        DefaultTheme defaultTheme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::m_defaultTheme = &defaultTheme;
    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const std::string& primary) :
        m_primary(primary)
    {
        if (!primary.empty())
            m_themeLoader->preload(primary);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::~Theme()
    {
        if (m_defaultTheme == this)
            setDefault(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::load(const std::string& primary)
    {
        m_primary = primary;
        m_themeLoader->preload(primary);

        // Update the existing renderers
        for (auto& pair : m_renderers)
        {
            auto& renderer = pair.second;
            const auto oldData = renderer;

            if (!m_themeLoader->canLoad(m_primary, pair.first))
                continue;

            auto& properties = m_themeLoader->load(m_primary, pair.first);

            // Update the property-value pairs of the renderer
            renderer->propertyValuePairs = std::map<std::string, ObjectConverter>{};
            for (const auto& property : properties)
                renderer->propertyValuePairs[property.first] = ObjectConverter(property.second);

            // Tell the widgets that were using this renderer about all the updated properties, both new ones and old ones that were now reset to their default value
            auto oldIt = oldData->propertyValuePairs.begin();
            auto newIt = renderer->propertyValuePairs.begin();
            while (oldIt != oldData->propertyValuePairs.end() && newIt != renderer->propertyValuePairs.end())
            {
                if (oldIt->first < newIt->first)
                {
                    // Update values that no longer exist in the new renderer and are now reset to the default value
                    for (const auto& observer : renderer->observers)
                        observer.second(oldIt->first);

                    ++oldIt;
                }
                else
                {
                    // Update changed and new properties
                    for (const auto& observer : renderer->observers)
                        observer.second(newIt->first);

                    if (newIt->first < oldIt->first)
                        ++newIt;
                    else
                    {
                        ++oldIt;
                        ++newIt;
                    }
                }
            }
            while (oldIt != oldData->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(oldIt->first);

                ++oldIt;
            }
            while (newIt != renderer->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(newIt->first);

                ++newIt;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRenderer(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();
        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRendererNoThrow(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();

        if (!m_themeLoader->canLoad(m_primary, lowercaseSecondary))
            return nullptr;

        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const std::string& id, std::shared_ptr<RendererData> renderer)
    {
        m_renderers[toLower(id)] = renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const std::string& id)
    {
        auto it = m_renderers.find(toLower(id));
        if (it != m_renderers.end())
        {
            m_renderers.erase(it);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Theme::getPrimary() const
    {
        return m_primary;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setThemeLoader(std::shared_ptr<BaseThemeLoader> themeLoader)
    {
        m_themeLoader = themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BaseThemeLoader> Theme::getThemeLoader()
    {
        return m_themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(Theme* theme)
    {
        if (theme)
            m_defaultTheme = theme;
        else
        {
            defaultTheme = {};
            m_defaultTheme = &defaultTheme;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::getDefault()
    {
        return m_defaultTheme;
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


#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/Widgets/BitmapButton.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/HorizontalWrap.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/RangeSlider.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::function<Widget::Ptr()>> WidgetFactory::m_constructFunctions =
    {
        {"bitmapbutton", std::make_shared<BitmapButton>},
        {"button", std::make_shared<Button>},
        {"canvas", std::make_shared<Canvas>},
        {"chatbox", std::make_shared<ChatBox>},
        {"checkbox", std::make_shared<CheckBox>},
        {"childwindow", std::make_shared<ChildWindow>},
        {"clickablewidget", std::make_shared<ClickableWidget>},
        {"combobox", std::make_shared<ComboBox>},
        {"editbox", std::make_shared<EditBox>},
        {"grid", std::make_shared<Grid>},
        {"group", std::make_shared<Group>},
        {"horizontallayout", std::make_shared<HorizontalLayout>},
        {"horizontalwrap", std::make_shared<HorizontalWrap>},
        {"knob", std::make_shared<Knob>},
        {"label", std::make_shared<Label>},
        {"listbox", std::make_shared<ListBox>},
        {"menubar", std::make_shared<MenuBar>},
        {"messagebox", std::make_shared<MessageBox>},
        {"panel", std::make_shared<Panel>},
        {"picture", std::make_shared<Picture>},
        {"progressbar", std::make_shared<ProgressBar>},
        {"radiobutton", std::make_shared<RadioButton>},
        {"radiobuttongroup", std::make_shared<RadioButtonGroup>},
        {"rangeslider", std::make_shared<RangeSlider>},
        {"scrollablepanel", std::make_shared<ScrollablePanel>},
        {"scrollbar", std::make_shared<Scrollbar>},
        {"slider", std::make_shared<Slider>},
        {"spinbutton", std::make_shared<SpinButton>},
        {"tabs", std::make_shared<Tabs>},
        {"textbox", std::make_shared<TextBox>},
        {"treeview", std::make_shared<TreeView>},
        {"verticallayout", std::make_shared<VerticalLayout>}
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetFactory::setConstructFunction(const std::string& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructFunctions[toLower(type)] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::function<Widget::Ptr()>& WidgetFactory::getConstructFunction(const std::string& type)
    {
        return m_constructFunctions[toLower(type)];
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


#include <TGUI/Renderers/ButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleHover, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDown, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDisabled, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleFocused, {})
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


#include <TGUI/Renderers/ChildWindowRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ChildWindowRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, TitleBarColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, TitleColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ChildWindowRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ChildWindowRenderer, TextureTitleBar)

    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, CloseButton, "childwindowbutton")
    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, MaximizeButton, "childwindowbutton")
    TGUI_RENDERER_PROPERTY_RENDERER(ChildWindowRenderer, MinimizeButton, "childwindowbutton")

    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, BorderBelowTitleBar, 0)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, DistanceToSide, 3)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, PaddingBetweenButtons, 1)
    TGUI_RENDERER_PROPERTY_NUMBER(ChildWindowRenderer, MinimumResizableBorderWidth, 5)

    TGUI_RENDERER_PROPERTY_BOOL(ChildWindowRenderer, ShowTextOnTitleButtons, false)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ChildWindowRenderer::getTitleBarHeight() const
    {
        auto it = m_data->propertyValuePairs.find("titlebarheight");
        if (it != m_data->propertyValuePairs.end())
            return it->second.getNumber();
        else
        {
            it = m_data->propertyValuePairs.find("texturetitlebar");
            if (it != m_data->propertyValuePairs.end() && it->second.getTexture().getData())
                return it->second.getTexture().getImageSize().y;
            else
                return 20;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarHeight(float number)
    {
        setProperty("titlebarheight", ObjectConverter{number});
    }
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


#include <TGUI/Renderers/EditBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_NUMBER(EditBoxRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, DefaultTextColor, Color(160, 160, 160))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, DefaultTextStyle, sf::Text::Italic)
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


#include <TGUI/Renderers/KnobRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(KnobRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, ThumbColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(KnobRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(KnobRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(KnobRenderer, TextureForeground)

    TGUI_RENDERER_PROPERTY_NUMBER(KnobRenderer, ImageRotation, 0)
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


#include <TGUI/Renderers/ListBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ListBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ListBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, SelectedTextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListBoxRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ListBoxRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ListBoxRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ListBoxRenderer, SelectedTextStyle, {})

    TGUI_RENDERER_PROPERTY_RENDERER(ListBoxRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(ListBoxRenderer, ScrollbarWidth, 0)
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


#include <TGUI/Renderers/MessageBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_COLOR(MessageBoxRenderer, TextColor, Color::Black)

    TGUI_RENDERER_PROPERTY_RENDERER(MessageBoxRenderer, Button, "button")
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


#include <TGUI/Renderers/PictureRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_TEXTURE(PictureRenderer, Texture)
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


#include <TGUI/Renderers/RadioButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(RadioButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_NUMBER(RadioButtonRenderer, TextDistanceRatio, 0.2f)

    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColorDisabled, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUnchecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedFocused)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureChecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(RadioButtonRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(RadioButtonRenderer, TextStyleChecked, {})
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


#include <TGUI/Renderers/ScrollablePanelRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_RENDERER(ScrollablePanelRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(ScrollablePanelRenderer, ScrollbarWidth, 0)
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


#include <TGUI/Renderers/SliderRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(SliderRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, TrackColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, TrackColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, ThumbColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, ThumbColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(SliderRenderer, BorderColorHover, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(SliderRenderer, TextureTrack)
    TGUI_RENDERER_PROPERTY_TEXTURE(SliderRenderer, TextureTrackHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(SliderRenderer, TextureThumb)
    TGUI_RENDERER_PROPERTY_TEXTURE(SliderRenderer, TextureThumbHover)
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


#include <TGUI/Renderers/TabsRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TabsRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedTextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureTab)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureTabHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureSelectedTab)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureSelectedTabHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureDisabledTab)

    TGUI_RENDERER_PROPERTY_NUMBER(TabsRenderer, DistanceToSide, 0)
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


#include <TGUI/Renderers/TreeViewRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TreeViewRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(TreeViewRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BorderColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedTextColorHover, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureBranchExpanded)
    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureBranchCollapsed)
    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureLeaf)

    TGUI_RENDERER_PROPERTY_RENDERER(TreeViewRenderer, Scrollbar, "scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(TreeViewRenderer, ScrollbarWidth, 0)
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


#include <TGUI/Widgets/BitmapButton.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::BitmapButton()
    {
        m_type = "BitmapButton";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::Ptr BitmapButton::create()
    {
        return std::make_shared<BitmapButton>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::Ptr BitmapButton::copy(BitmapButton::ConstPtr button)
    {
        if (button)
            return std::static_pointer_cast<BitmapButton>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setSize(const Layout2d& size)
    {
        Button::setSize(size);

        recalculateGlyphSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setText(const sf::String& text)
    {
        m_string = text;
        m_text.setString(text);
        m_text.setCharacterSize(m_textSize);

        if (m_autoSize)
            updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setImage(const Texture& image)
    {
        m_glyphTexture = image;
        m_glyphSprite.setTexture(m_glyphTexture);
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& BitmapButton::getImage() const
    {
        return m_glyphTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setImageScaling(float relativeHeight)
    {
        m_relativeGlyphHeight = relativeHeight;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BitmapButton::getImageScaling() const
    {
        return m_relativeGlyphHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> BitmapButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Button::save(renderers);
        node->propertyValuePairs["Image"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_glyphTexture));
        node->propertyValuePairs["ImageScaling"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_relativeGlyphHeight));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Button::load(node, renderers);

        if (node->propertyValuePairs["image"])
            setImage(Deserializer::deserialize(ObjectConverter::Type::Texture, node->propertyValuePairs["image"]->value).getTexture());
        if (node->propertyValuePairs["imagescaling"])
            setImageScaling(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs["imagescaling"]->value).getNumber());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!m_glyphSprite.isSet())
        {
            Button::draw(target, states);
            return;
        }

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
            else if (m_mouseHover)
            {
                if (m_mouseDown && m_spriteDown.isSet())
                    m_spriteDown.draw(target, states);
                else if (m_spriteHover.isSet())
                    m_spriteHover.draw(target, states);
                else
                    m_sprite.draw(target, states);
            }
            else
                m_sprite.draw(target, states);

            // When the button is focused then draw an extra image
            if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
        }
        else // There is no background texture
        {
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());
        }

        Clipping clipping(target, states, {}, getInnerSize());
        if (m_text.getString().isEmpty())
        {
            states.transform.translate({(getInnerSize().x - m_glyphSprite.getSize().x) / 2.f, (getInnerSize().y - m_glyphSprite.getSize().y) / 2.f});
            m_glyphSprite.draw(target, states);
        }
        else // There is some text next to the glyph
        {
            const float distanceBetweenTextAndImage = m_text.getSize().y / 5.f;
            const float width = m_glyphSprite.getSize().x + distanceBetweenTextAndImage + m_text.getSize().x;
            states.transform.translate({(getInnerSize().x - width) / 2.f, (getInnerSize().y - m_glyphSprite.getSize().y) / 2.f});
            m_glyphSprite.draw(target, states);
            states.transform.translate({m_glyphSprite.getSize().x + distanceBetweenTextAndImage, (m_glyphSprite.getSize().y - m_text.getSize().y) / 2.f});
            m_text.draw(target, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::updateSize()
    {
        if (!m_glyphSprite.isSet())
        {
            Button::updateSize();
            return;
        }

        if (m_autoSize)
        {
            Widget::setSize({getSize().x, m_text.getSize().y * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});

            recalculateGlyphSize();

            if (m_text.getString().isEmpty())
            {
                Widget::setSize({m_glyphSprite.getSize().x + (getInnerSize().y - m_glyphSprite.getSize().y) + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                                 getSize().y});
            }
            else
            {
                const float spaceAroundImageAndText = m_text.getSize().y;
                const float distanceBetweenTextAndImage = m_text.getSize().y / 5.f;
                Widget::setSize({m_glyphSprite.getSize().x + distanceBetweenTextAndImage + m_text.getSize().x
                                + spaceAroundImageAndText + m_bordersCached.getLeft() + m_bordersCached.getRight(), getSize().y});
            }
        }
        else
            recalculateGlyphSize();

        m_bordersCached.updateParentSize(getSize());

        // Reset the texture sizes
        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDown.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::recalculateGlyphSize()
    {
        if (!m_glyphSprite.isSet())
            return;

        if (m_relativeGlyphHeight == 0)
            m_glyphSprite.setSize(m_glyphTexture.getImageSize());
        else
            m_glyphSprite.setSize({m_relativeGlyphHeight * getInnerSize().y,
                                   (m_relativeGlyphHeight * getInnerSize().y) / m_glyphTexture.getImageSize().y * m_glyphTexture.getImageSize().x});
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


#include <TGUI/Widgets/BoxLayoutRatios.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BoxLayoutRatios::BoxLayoutRatios(const Layout2d& size) :
        BoxLayout{size}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, 1, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::add(const Widget::Ptr& widget, float ratio, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, ratio, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insert(std::size_t index, const Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(index, widget, 1, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insert(std::size_t index, const Widget::Ptr& widget, float ratio, const sf::String& widgetName)
    {
        if (index < m_ratios.size())
            m_ratios.insert(m_ratios.begin() + index, ratio);
        else
            m_ratios.push_back(ratio);

        BoxLayout::insert(index, widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::remove(std::size_t index)
    {
        if (index < m_ratios.size())
            m_ratios.erase(m_ratios.begin() + index);

        return BoxLayout::remove(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::removeAllWidgets()
    {
        BoxLayout::removeAllWidgets();
        m_ratios.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::addSpace(float ratio)
    {
        insertSpace(m_widgets.size(), ratio);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::insertSpace(std::size_t index, float ratio)
    {
        insert(index, Group::create(), ratio, "#TGUI_INTERNAL$HorizontalLayoutSpace#");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::setRatio(Widget::Ptr widget, float ratio)
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return setRatio(i, ratio);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BoxLayoutRatios::setRatio(std::size_t index, float ratio)
    {
        if (index >= m_ratios.size())
            return false;

        m_ratios[index] = ratio;
        updateWidgets();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRatios::getRatio(Widget::Ptr widget) const
    {
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (m_widgets[i] == widget)
                return getRatio(i);
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BoxLayoutRatios::getRatio(std::size_t index) const
    {
        if (index >= m_ratios.size())
            return 0;

        return m_ratios[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> BoxLayoutRatios::save(SavingRenderersMap& renderers) const
    {
        auto node = BoxLayout::save(renderers);

        if (m_widgets.size() > 0)
        {
            std::string ratioList = "[" + Serializer::serialize(m_ratios[0]);
            for (std::size_t i = 1; i < m_widgets.size(); ++i)
                ratioList += ", " + Serializer::serialize(m_ratios[i]);

            ratioList += "]";
            node->propertyValuePairs["Ratios"] = std::make_unique<DataIO::ValueNode>(ratioList);
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BoxLayoutRatios::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        BoxLayout::load(node, renderers);

        if (node->propertyValuePairs["ratios"])
        {
            if (!node->propertyValuePairs["ratios"]->listNode)
                throw Exception{"Failed to parse 'Ratios' property, expected a list as value"};

            if (node->propertyValuePairs["ratios"]->valueList.size() != getWidgets().size())
                throw Exception{"Amounts of values for 'Ratios' differs from the amount in child widgets"};

            for (std::size_t i = 0; i < node->propertyValuePairs["ratios"]->valueList.size(); ++i)
                setRatio(i, Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs["ratios"]->valueList[i]).getNumber());
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


#include <TGUI/Widgets/Canvas.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const Layout2d& size)
    {
        m_type = "Canvas";

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(const Canvas& other) :
        ClickableWidget{other}
    {
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Canvas(Canvas&& other) :
        ClickableWidget{std::move(other)}
    {
        // sf::RenderTexture does not support move yet
        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (const Canvas& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas& Canvas::operator= (Canvas&& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(std::move(right));

            // sf::RenderTexture does not support move yet
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::create(Layout2d size)
    {
        return std::make_shared<Canvas>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Canvas::Ptr Canvas::copy(Canvas::ConstPtr canvas)
    {
        if (canvas)
            return std::static_pointer_cast<Canvas>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (getSize() != Vector2f{})
        {
            m_renderTexture.create(static_cast<unsigned int>(getSize().x), static_cast<unsigned int>(getSize().y));
            m_sprite.setTexture(m_renderTexture.getTexture(), true);

            m_renderTexture.clear();
            m_renderTexture.display();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::clear(Color color)
    {
        m_renderTexture.clear(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
    {
        m_renderTexture.draw(drawable, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states)
    {
        m_renderTexture.draw(vertices, vertexCount, type, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::display()
    {
        m_renderTexture.display();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::rendererChanged(const std::string& property)
    {
        Widget::rendererChanged(property);

        if (property == "opacity")
            m_sprite.setColor(Color::calcColorOpacity(Color::White, getSharedRenderer()->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        target.draw(m_sprite, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Canvas::canGainFocus() const
    {
        return false;
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


#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::CheckBox()
    {
        m_type = "CheckBox";

        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize({Text::getLineHeight(m_text) + m_bordersCached.getLeft() + m_bordersCached.getRight(),
                 Text::getLineHeight(m_text) + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::create(sf::String text)
    {
        auto checkBox = std::make_shared<CheckBox>();

        if (!text.isEmpty())
            checkBox->setText(text);

        return checkBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CheckBox::Ptr CheckBox::copy(CheckBox::ConstPtr checkbox)
    {
        if (checkbox)
            return std::static_pointer_cast<CheckBox>(checkbox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CheckBox::getFullSize() const
    {
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            Vector2f sizeDiff = m_spriteChecked.getSize() - m_spriteUnchecked.getSize();
            if (getText().isEmpty())
                return getSize() + Vector2f{std::max(0.f, sizeDiff.x - m_bordersCached.getRight()), std::max(0.f, sizeDiff.y - m_bordersCached.getTop())};
            else
                return getSize() + Vector2f{(getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(0.f, std::max((m_text.getSize().y - getSize().y) / 2, sizeDiff.y - m_bordersCached.getTop()))};
        }
        else
        {
            if (getText().isEmpty())
                return getSize();
            else
                return {getSize().x + (getSize().x * m_textDistanceRatioCached) + m_text.getSize().x, std::max(getSize().y, m_text.getSize().y)};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f CheckBox::getWidgetOffset() const
    {
        float yOffset = 0;
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet()
         && (m_textureUncheckedCached.getImageSize() != m_textureCheckedCached.getImageSize()))
        {
            const float sizeDiff = m_spriteChecked.getSize().y - m_spriteUnchecked.getSize().y;
            if (sizeDiff > m_bordersCached.getTop())
                yOffset = sizeDiff - m_bordersCached.getTop();
        }

        if (getText().isEmpty() || (getSize().y >= m_text.getSize().y))
            return {0, -yOffset};
        else
            return {0, -std::max(yOffset, (m_text.getSize().y - getSize().y) / 2)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::setChecked(bool checked)
    {
        if (checked && !m_checked)
        {
            m_checked = true;

            updateTextColor();
            if (m_textStyleCheckedCached.isSet())
                m_text.setStyle(m_textStyleCheckedCached);
            else
                m_text.setStyle(m_textStyleCached);

            onCheck.emit(this, true);
        }
        else
            RadioButton::setChecked(checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        // Check or uncheck when we clicked on the checkbox (not just mouse release)
        if (mouseDown)
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check or uncheck the checkbox if the space key or the return key was pressed
        if ((event.code == sf::Keyboard::Space) || (event.code == sf::Keyboard::Return))
            setChecked(!m_checked);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::rendererChanged(const std::string& property)
    {
        if (property == "textureunchecked")
            m_textureUncheckedCached = getSharedRenderer()->getTextureUnchecked();
        else if (property == "texturechecked")
            m_textureCheckedCached = getSharedRenderer()->getTextureChecked();

        RadioButton::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::updateTextureSizes()
    {
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            m_spriteUnchecked.setSize(getInnerSize());
            m_spriteChecked.setSize(
                {getInnerSize().x + ((m_textureCheckedCached.getImageSize().x - m_textureUncheckedCached.getImageSize().x) * (getInnerSize().x / m_textureUncheckedCached.getImageSize().x)),
                 getInnerSize().y + ((m_textureCheckedCached.getImageSize().y - m_textureUncheckedCached.getImageSize().y) * (getInnerSize().y / m_textureUncheckedCached.getImageSize().y))}
            );

            m_spriteUncheckedHover.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedHover.setSize(m_spriteChecked.getSize());

            m_spriteUncheckedDisabled.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedDisabled.setSize(m_spriteChecked.getSize());

            m_spriteUncheckedFocused.setSize(m_spriteUnchecked.getSize());
            m_spriteCheckedFocused.setSize(m_spriteChecked.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CheckBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
            drawBorders(target, states, m_bordersCached, getSize(), getCurrentBorderColor());

        states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        if (m_spriteUnchecked.isSet() && m_spriteChecked.isSet())
        {
            if (m_checked)
            {
                const Sprite* checkedSprite;
                if (!m_enabled && m_spriteCheckedDisabled.isSet())
                    checkedSprite = &m_spriteCheckedDisabled;
                else if (m_mouseHover && m_spriteCheckedHover.isSet())
                    checkedSprite = &m_spriteCheckedHover;
                else if (m_focused && m_spriteCheckedFocused.isSet())
                    checkedSprite = &m_spriteCheckedFocused;
                else
                    checkedSprite = &m_spriteChecked;

                // The image may need to be shifted when the check leaves the box
                if (getInnerSize().y != checkedSprite->getSize().y)
                {
                    float diff = getInnerSize().y - checkedSprite->getSize().y;

                    states.transform.translate({0, diff});
                    checkedSprite->draw(target, states);
                    states.transform.translate({0, -diff});
                }
                else // Draw the checked texture normally
                    checkedSprite->draw(target, states);
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
            drawRectangleShape(target, states, getInnerSize(), getCurrentBackgroundColor());

            if (m_checked)
            {
                const float pi = 3.14159265358979f;
                const Color& checkColor = getCurrentCheckColor();
                const Vector2f size = getInnerSize();
                const float lineThickness = std::min(size.x, size.y) / 5;
                const Vector2f leftPoint = {0.14f * size.x, 0.4f * size.y};
                const Vector2f middlePoint = {0.44f * size.x, 0.7f * size.y};
                const Vector2f rightPoint = {0.86f * size.x, 0.28f * size.y};
                const float x = (lineThickness / 2.f) * std::cos(pi / 4.f);
                const float y = (lineThickness / 2.f) * std::sin(pi / 4.f);
                const std::vector<sf::Vertex> vertices = {
                    {{leftPoint.x - x, leftPoint.y + y}, checkColor},
                    {{leftPoint.x + x, leftPoint.y - y}, checkColor},
                    {{middlePoint.x, middlePoint.y + 2*y}, checkColor},
                    {{middlePoint.x, middlePoint.y - 2*y}, checkColor},
                    {{rightPoint.x + x, rightPoint.y + y}, checkColor},
                    {{rightPoint.x - x, rightPoint.y - y}, checkColor}
                };

                target.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
            }
        }
        states.transform.translate({-m_bordersCached.getLeft(), -m_bordersCached.getTop()});

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


#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget()
    {
        m_type = "ClickableWidget";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::Ptr ClickableWidget::create(Layout2d size)
    {
        auto widget = std::make_shared<ClickableWidget>();
        widget->setSize(size);
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::Ptr ClickableWidget::copy(ClickableWidget::ConstPtr widget)
    {
        if (widget)
            return std::static_pointer_cast<ClickableWidget>(widget->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true; /// TODO: Is there any widget for which this can't be in Widget base class?
        onMousePress.emit(this, pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMouseReleased(Vector2f pos)
    {
        onMouseRelease.emit(this, pos - getPosition());

        if (m_mouseDown)
            onClick.emit(this, pos - getPosition());

        m_mouseDown = false; /// TODO: Is there any widget for which this can't be in Widget base class?
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::draw(sf::RenderTarget&, sf::RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ClickableWidget::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onClick.getName()))
            return onClick;
        else
            return Widget::getSignal(std::move(signalName));
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
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/Clipping.hpp>

/// TODO: Where m_selStart and m_selEnd are compared, use std::min and std::max and merge the if and else bodies

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_USE_CPP17
    const std::string EditBox::Validator::All   = ".*";
    const std::string EditBox::Validator::Int   = "[+-]?[0-9]*";
    const std::string EditBox::Validator::UInt  = "[0-9]*";
    const std::string EditBox::Validator::Float = "[+-]?[0-9]*\\.?[0-9]*";
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox()
    {
        m_type = "EditBox";
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection.setFont(m_fontCached);
        m_textAfterSelection.setFont(m_fontCached);
        m_textFull.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<EditBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_textFull) * 10,
                 Text::getLineHeight(m_textFull) * 1.25f + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::create()
    {
        return std::make_shared<EditBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::copy(EditBox::ConstPtr editBox)
    {
        if (editBox)
            return std::static_pointer_cast<EditBox>(editBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getSharedRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBox::getSharedRenderer() const
    {
        return aurora::downcast<const EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBox::getRenderer() const
    {
        return aurora::downcast<const EditBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});

        // Recalculate the position of the texts
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            m_textFull.setCharacterSize(Text::findBestTextSize(m_fontCached, (getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) * 0.8f));
            m_textBeforeSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textAfterSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_defaultText.setCharacterSize(m_textFull.getCharacterSize());
        }
        else // When the text has a fixed size
        {
            m_textFull.setCharacterSize(m_textSize);
            m_textBeforeSelection.setCharacterSize(m_textSize);
            m_textSelection.setCharacterSize(m_textSize);
            m_textAfterSelection.setCharacterSize(m_textSize);
            m_defaultText.setCharacterSize(m_textSize);
        }

        // Change the text if allowed
        if (m_regexString == ".*")
            m_text = text;
        else if (std::regex_match(text.toAnsiString(), m_regex))
            m_text = text.toAnsiString(); // Unicode is not supported when using regex because it can't be checked
        else // Clear the text
            m_text = "";

        // Remove all the excess characters if there is a character limit
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
            m_text.erase(m_maxChars, sf::String::InvalidPos);

        // Set the displayed text
        if (m_passwordChar != '\0')
        {
            sf::String displayedText = m_text;
            std::fill(displayedText.begin(), displayedText.end(), m_passwordChar);

            m_textFull.setString(displayedText);
        }
        else
            m_textFull.setString(m_text);

        // Set the texts
        m_textBeforeSelection.setString(m_textFull.getString());
        m_textSelection.setString("");
        m_textAfterSelection.setString("");

        if (!m_fontCached)
            return;

        // Check if there is a text width limit
        const float width = getVisibleEditBoxWidth();
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (!m_textFull.getString().isEmpty() && (getFullTextWidth() > width))
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());
        }
        else // There is no text cropping
        {
            // If the text can be moved to the right then do so
            const float textWidth = getFullTextWidth();
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }

        // Set the caret behind the last character
        setCaretPosition(m_textFull.getString().getSize());

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const sf::String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText(std::size_t start, std::size_t length)
    {
        m_selStart = start;
        m_selEnd = std::min(m_text.getSize(), start + length);
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getSelectedText() const
    {
        return m_text.substring(std::min(m_selStart, m_selEnd), std::max(m_selStart, m_selEnd));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to re-position the text
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getTextSize() const
    {
        return m_textFull.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char passwordChar)
    {
        // Change the password character
        m_passwordChar = passwordChar;

        // Recalculate the text position
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char EditBox::getPasswordCharacter() const
    {
        return m_passwordChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_textFull.getString().getSize() > m_maxChars))
        {
            sf::String displayedText = m_textFull.getString();

            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            displayedText.erase(m_maxChars, sf::String::InvalidPos);

            // If we passed here then the text has changed.
            m_textBeforeSelection.setString(displayedText);
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
            m_textFull.setString(displayedText);

            // Set the caret behind the last character
            setCaretPosition(displayedText.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setAlignment(Alignment alignment)
    {
        m_textAlignment = alignment;
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Alignment EditBox::getAlignment() const
    {
        return m_textAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::limitTextWidth(bool limitWidth)
    {
        m_limitTextWidth = limitWidth;

        if (!m_fontCached)
            return;

        // Check if the width is being limited
        if (m_limitTextWidth)
        {
            // Delete the last characters when the text no longer fits inside the edit box
            const float width = getVisibleEditBoxWidth();
            while (!m_textFull.getString().isEmpty() && (getFullTextWidth() > width))
            {
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());

            // There is no clipping
            m_textCropPosition = 0;

            // If the caret was behind the limit, then set it at the end
            if (m_selEnd > m_textFull.getString().getSize())
                setCaretPosition(m_selEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isTextWidthLimited() const
    {
        return m_limitTextWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Set the caret to the correct position
        m_selStart = charactersBeforeCaret;
        m_selEnd = charactersBeforeCaret;
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::getCaretPosition() const
    {
        return m_selEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setInputValidator(const std::string& regex)
    {
        m_regexString = regex;
        m_regex = m_regexString;

        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getInputValidator() const
    {
        return m_regexString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setFocused(bool focused)
    {
        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selChars)
                setCaretPosition(m_selEnd);
        }

    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(focused);
    #endif

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_sprite.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        // Find the caret position
        const float positionX = pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft();

        std::size_t caretPosition = findCaretPosition(positionX);
        const float textOffset = Text::getExtraHorizontalPadding(m_textFull);

        // When clicking on the left of the first character, move the caret to the left
        if ((positionX < textOffset) && (caretPosition > 0))
            --caretPosition;

        // When clicking on the right of the right character, move the caret to the right
        else if ((positionX > getVisibleEditBoxWidth() - textOffset) && (caretPosition < m_textFull.getString().getSize()))
            ++caretPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the caret at the end of the text
            setCaretPosition(m_textFull.getString().getSize());

            // Select the whole text
            m_selStart = 0;
            m_selEnd = m_text.getSize();
            updateSelection();
        }
        else // No double clicking
        {
            // Set the new caret
            setCaretPosition(caretPosition);

            // If the next click comes soon enough then it will be a double click
            m_possibleDoubleClick = true;
        }

        // Set the mouse down flag
        m_mouseDown = true;
        onMousePress.emit(this, pos);

        // The caret should be visible
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_mouseDown)
        {
            const auto oldSelEnd = m_selEnd;

            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft());
            }
            else // Scrolling is enabled
            {
                const float width = getVisibleEditBoxWidth();
                const float textOffset = Text::getExtraHorizontalPadding(m_textFull);

                // Check if the mouse is on the left of the text
                if (pos.x < m_bordersCached.getLeft() + m_paddingCached.getLeft() + textOffset)
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition > m_textFull.getCharacterSize() / 10)
                            m_textCropPosition -= static_cast<unsigned int>(m_textFull.getCharacterSize() / 10.f);
                        else
                            m_textCropPosition = 0;
                    }
                    else
                    {
                        if (m_textCropPosition)
                            m_textCropPosition -= 1;
                    }
                }
                // Check if the mouse is on the right of the text AND there is a possibility to scroll
                else if ((pos.x > m_bordersCached.getLeft() + m_paddingCached.getLeft() + width - textOffset) && (getFullTextWidth() > width))
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        const float pixelsToMove = m_textFull.getCharacterSize() / 10.f;
                        if (m_textCropPosition + width + pixelsToMove < getFullTextWidth())
                            m_textCropPosition += static_cast<unsigned int>(pixelsToMove);
                        else
                            m_textCropPosition = static_cast<unsigned int>(getFullTextWidth() - width);
                    }
                    else
                    {
                        if (m_textCropPosition + width < getFullTextWidth())
                            m_textCropPosition += 1;
                    }
                }

                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft());
            }

            if (m_selEnd != oldSelEnd)
                updateSelection();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if one of the correct keys was pressed
        switch (event.code)
        {
            case sf::Keyboard::Left:
            {
                if (event.control)
                {
                    // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
                    bool done = false;
                    bool skippedWhitespace = false;
                    for (std::size_t i = m_selEnd; i > 0; --i)
                    {
                        if (skippedWhitespace)
                        {
                            if (isWhitespace(m_text[i-1]))
                            {
                                m_selEnd = i;
                                done = true;
                                break;
                            }
                        }
                        else
                        {
                            if (!isWhitespace(m_text[i-1]))
                                skippedWhitespace = true;
                        }
                    }

                    if (!done && skippedWhitespace)
                        m_selEnd = 0;
                }
                else // Control key is not being pressed
                {
                    // If text is selected then move to the cursor to the left side of the selected text
                    if ((m_selChars > 0) && !event.shift)
                    {
                        m_selEnd = std::min(m_selStart, m_selEnd);
                    }
                    else if (m_selEnd > 0)
                        m_selEnd--;
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
                break;
            }
            case sf::Keyboard::Right:
            {
                if (event.control)
                {
                    // Move to the end of the word (or to the end of the next word when already at the end)
                    bool done = false;
                    bool skippedWhitespace = false;
                    for (std::size_t i = m_selEnd; i < m_text.getSize(); ++i)
                    {
                        if (skippedWhitespace)
                        {
                            if (isWhitespace(m_text[i]))
                            {
                                m_selEnd = i;
                                done = true;
                                break;
                            }
                        }
                        else
                        {
                            if (!isWhitespace(m_text[i]))
                                skippedWhitespace = true;
                        }
                    }

                    if (!done && skippedWhitespace)
                        m_selEnd = m_text.getSize();
                }
                else // Control key is not being pressed
                {
                    // If text is selected then move to the cursor to the right side of the selected text
                    if ((m_selChars > 0) && !event.shift)
                    {
                        m_selEnd = std::max(m_selStart, m_selEnd);
                    }
                    else if (m_selEnd < m_text.getSize())
                        m_selEnd++;
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
                break;
            }
            case sf::Keyboard::Home:
            {
                // Set the caret to the beginning of the text
                m_selEnd = 0;
                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
                break;
            }
            case sf::Keyboard::End:
            {
                // Set the caret behind the text
                m_selEnd = m_text.getSize();
                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
                break;
            }
            case sf::Keyboard::Return:
            {
                onReturnKeyPress.emit(this, m_text);
                break;
            }
            case sf::Keyboard::BackSpace:
            {
                if (m_readOnly)
                    return;

                // Make sure that we did not select any characters
                if (m_selChars == 0)
                {
                    // We can't delete any characters when you are at the beginning of the string
                    if (m_selEnd == 0)
                        return;

                    // Erase the character
                    sf::String displayedString = m_textFull.getString();
                    displayedString.erase(m_selEnd-1, 1);
                    m_textFull.setString(displayedString);
                    m_text.erase(m_selEnd-1, 1);

                    // Set the caret back on the correct position
                    setCaretPosition(m_selEnd - 1);

                    const float width = getVisibleEditBoxWidth();

                    // If the text can be moved to the right then do so
                    const float textWidth = getFullTextWidth();
                    if (textWidth > width)
                    {
                        if (textWidth - m_textCropPosition < width)
                            m_textCropPosition = static_cast<unsigned int>(textWidth - width);
                    }
                    else
                        m_textCropPosition = 0;
                }
                else // When you did select some characters, delete them
                    deleteSelectedCharacters();

                onTextChange.emit(this, m_text);
                break;
            }
            case sf::Keyboard::Delete:
            {
                if (m_readOnly)
                    return;

                // Make sure that no text is selected
                if (m_selChars == 0)
                {
                    // When the caret is at the end of the line then you can't delete anything
                    if (m_selEnd == m_text.getSize())
                        return;

                    // Erase the character
                    sf::String displayedString = m_textFull.getString();
                    displayedString.erase(m_selEnd, 1);
                    m_textFull.setString(displayedString);
                    m_text.erase(m_selEnd, 1);

                    // Set the caret back on the correct position
                    setCaretPosition(m_selEnd);

                    // If the text can be moved to the right then do so
                    const float width = getVisibleEditBoxWidth();
                    const float textWidth = getFullTextWidth();
                    if (textWidth > width)
                    {
                        if (textWidth - m_textCropPosition < width)
                            m_textCropPosition = static_cast<unsigned int>(textWidth - width);
                    }
                    else
                        m_textCropPosition = 0;
                }
                else // You did select some characters, delete them
                    deleteSelectedCharacters();

                onTextChange.emit(this, m_text);
                break;
            }
            case sf::Keyboard::C:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                    Clipboard::set(m_textSelection.getString());

                break;
            }
            case sf::Keyboard::V:
            {
                if (m_readOnly)
                    return;

                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    const auto clipboardContents = Clipboard::get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        const std::size_t oldCaretPos = m_selEnd;

                        if (m_text.getSize() > m_selEnd)
                            setText(m_text.toWideString().substr(0, m_selEnd) + Clipboard::get() + m_text.toWideString().substr(m_selEnd, m_text.getSize() - m_selEnd));
                        else
                            setText(m_text + clipboardContents);

                        setCaretPosition(oldCaretPos + clipboardContents.getSize());
                    }
                }

                break;
            }
            case sf::Keyboard::X:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    Clipboard::set(m_textSelection.getString());

                    if (m_readOnly)
                        return;

                    deleteSelectedCharacters();

                    onTextChange.emit(this, m_text);
                }

                break;
            }
            case sf::Keyboard::A:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                    selectText();

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

    void EditBox::textEntered(std::uint32_t key)
    {
        if (m_readOnly)
            return;

        // Only add the character when the regex matches
        if (m_regexString != ".*")
        {
            sf::String text = m_text;

            if (m_selChars == 0)
                text.insert(m_selEnd, key);
            else
            {
                const std::size_t pos = std::min(m_selStart, m_selEnd);
                text.erase(pos, m_selChars);
                text.insert(pos, key);
            }

            // The character has to match the regex
            if (!std::regex_match(text.toAnsiString(), m_regex))
                return;
        }

        // If there are selected characters then delete them first
        if (m_selChars > 0)
            deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        // Insert our character
        m_text.insert(m_selEnd, key);

        // Change the displayed text
        sf::String displayedText = m_textFull.getString();
        if (m_passwordChar != '\0')
            displayedText.insert(m_selEnd, m_passwordChar);
        else
            displayedText.insert(m_selEnd, key);

        m_textFull.setString(displayedText);

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (getFullTextWidth() > getVisibleEditBoxWidth())
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                displayedText.erase(m_selEnd, 1);
                m_textFull.setString(displayedText);
                return;
            }
        }

        // Move our caret forward
        setCaretPosition(m_selEnd + 1);

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& EditBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTextChange.getName()))
            return onTextChange;
        else if (signalName == toLower(onReturnKeyPress.getName()))
            return onReturnKeyPress;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());

            setText(m_text);

            m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if (property == "caretwidth")
        {
            m_caret.setPosition({m_caret.getPosition().x + ((m_caret.getSize().x - getSharedRenderer()->getCaretWidth()) / 2.0f), m_caret.getPosition().y});
            m_caret.setSize({getSharedRenderer()->getCaretWidth(), getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if ((property == "textcolor") || (property == "textcolordisabled") || (property == "textcolorfocused"))
        {
            updateTextColor();
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == "defaulttextcolor")
        {
            m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getSharedRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getSharedRenderer()->getTextureHover());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getSharedRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getSharedRenderer()->getTextureFocused());
        }
        else if (property == "textstyle")
        {
            const TextStyle style = getSharedRenderer()->getTextStyle();
            m_textBeforeSelection.setStyle(style);
            m_textAfterSelection.setStyle(style);
            m_textSelection.setStyle(style);
            m_textFull.setStyle(style);
        }
        else if (property == "defaulttextstyle")
        {
            m_defaultText.setStyle(getSharedRenderer()->getDefaultTextStyle());
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
        else if (property == "backgroundcolorfocused")
        {
            m_backgroundColorFocusedCached = getSharedRenderer()->getBackgroundColorFocused();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == "caretcolorhover")
        {
            m_caretColorHoverCached = getSharedRenderer()->getCaretColorHover();
        }
        else if (property == "caretcolorfocused")
        {
            m_caretColorFocusedCached = getSharedRenderer()->getCaretColorFocused();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection.setOpacity(m_opacityCached);
            m_textSelection.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection.setFont(m_fontCached);
            m_textAfterSelection.setFont(m_fontCached);
            m_textFull.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);

            // Recalculate the text size and position
            setText(m_text);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> EditBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getAlignment() != EditBox::Alignment::Left)
        {
            if (getAlignment() == EditBox::Alignment::Center)
                node->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Center");
            else
                node->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Right");
        }

        if (getInputValidator() != ".*")
        {
            if (getInputValidator() == EditBox::Validator::Int)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("Int");
            else if (getInputValidator() == EditBox::Validator::UInt)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("UInt");
            else if (getInputValidator() == EditBox::Validator::Float)
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>("Float");
            else
                node->propertyValuePairs["InputValidator"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(sf::String{getInputValidator()}));
        }

        if (!m_text.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_text));
        if (!getDefaultText().isEmpty())
            node->propertyValuePairs["DefaultText"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getDefaultText()));
        if (getPasswordCharacter() != '\0')
            node->propertyValuePairs["PasswordCharacter"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(sf::String(getPasswordCharacter())));
        if (getMaximumCharacters() != 0)
            node->propertyValuePairs["MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(to_string(getMaximumCharacters()));
        if (isTextWidthLimited())
            node->propertyValuePairs["TextWidthLimited"] = std::make_unique<DataIO::ValueNode>("true");

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["defaulttext"])
            setDefaultText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["defaulttext"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["textwidthlimited"])
            limitTextWidth(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["textwidthlimited"]->value).getBool());
        if (node->propertyValuePairs["passwordcharacter"])
        {
            const std::string pass = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["passwordcharacter"]->value).getString();
            if (!pass.empty())
                setPasswordCharacter(pass[0]);
        }
        if (node->propertyValuePairs["alignment"])
        {
            if (toLower(node->propertyValuePairs["alignment"]->value) == "left")
                setAlignment(EditBox::Alignment::Left);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "center")
                setAlignment(EditBox::Alignment::Center);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "right")
                setAlignment(EditBox::Alignment::Right);
            else
                throw Exception{"Failed to parse Alignment property. Only the values Left, Center and Right are correct."};
        }
        if (node->propertyValuePairs["inputvalidator"])
        {
            if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "int")
                setInputValidator(EditBox::Validator::Int);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "uint")
                setInputValidator(EditBox::Validator::UInt);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "float")
                setInputValidator(EditBox::Validator::Float);
            else
                setInputValidator(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["inputvalidator"]->value).getString());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getFullTextWidth() const
    {
        return m_textFull.getSize().x + (2 * Text::getExtraHorizontalPadding(m_textFull));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth() const
    {
        return std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // Take the part outside the edit box into account when the text does not fit inside it
        posX += m_textCropPosition;

        if (m_textAlignment == Alignment::Left)
            posX -= Text::getExtraHorizontalPadding(m_textFull);
        else
        {
            // If the text is centered or aligned to the right then the position has to be corrected when the edit box is not entirely full
            const float editBoxWidth = getVisibleEditBoxWidth();
            const float textWidth = getFullTextWidth();
            if (textWidth < editBoxWidth)
            {
                // Set the number of pixels to move
                if (m_textAlignment == Alignment::Center)
                    posX -= (editBoxWidth - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    posX -= editBoxWidth - textWidth;
            }
        }

        float width = 0;
        std::uint32_t prevChar = 0;
        const unsigned int textSize = getTextSize();
        const bool bold = (m_textFull.getStyle() & sf::Text::Bold) != 0;

        std::size_t index;
        for (index = 0; index < m_text.getSize(); ++index)
        {
            float charWidth;
            std::uint32_t curChar = m_text[index];
            if (curChar == '\n')
            {
                // This should not happen as edit box is for single line text, but lets try the next line anyway since we haven't found the position yet
                width = 0;
                prevChar = 0;
                continue;
            }
            else if (curChar == '\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', textSize, bold).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, textSize, bold).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, textSize);
            if (width + charWidth < posX)
                width += charWidth + kerning;
            else
            {
                // If the mouse is on the second halve of the character then the caret should be on the right of it
                if (width + charWidth - posX < charWidth / 2.f)
                    index++;

                break;
            }

            prevChar = curChar;
        }

        return index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteSelectedCharacters()
    {
        // Nothing to delete when no text was selected
        if (m_selChars == 0)
            return;

        const std::size_t pos = std::min(m_selStart, m_selEnd);

        // Erase the characters
        sf::String displayedString = m_textFull.getString();
        displayedString.erase(pos, m_selChars);
        m_textFull.setString(displayedString);
        m_text.erase(pos, m_selChars);

        // Set the caret back on the correct position
        setCaretPosition(pos);

        // If the text can be moved to the right then do so
        const float width = getVisibleEditBoxWidth();
        const float textWidth = getFullTextWidth();
        if (textWidth > width)
        {
            if (textWidth - m_textCropPosition < width)
                m_textCropPosition = static_cast<unsigned int>(textWidth - width);
        }
        else
            m_textCropPosition = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::recalculateTextPositions()
    {
        const float textOffset = Text::getExtraHorizontalPadding(m_textFull);
        float textX = m_paddingCached.getLeft() - m_textCropPosition + textOffset;
        const float textY = m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) - m_textFull.getSize().y) / 2.f);

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            const float textWidth = m_textFull.getString().isEmpty() ? (m_defaultText.getSize().x + 2 * textOffset) : getFullTextWidth();

            // Check if a layout would make sense
            if (textWidth < getVisibleEditBoxWidth())
            {
                // Put the text on the correct position
                if (m_textAlignment == Alignment::Center)
                    textX += (getVisibleEditBoxWidth() - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    textX += getVisibleEditBoxWidth() - textWidth;
            }
        }

        float caretLeft = textX;

        // Set the text before the selection on the correct position
        m_textBeforeSelection.setPosition(textX, textY);
        m_defaultText.setPosition(textX, textY);

        // Check if there is a selection
        if (m_selChars != 0)
        {
            // Watch out for the kerning
            if (m_textBeforeSelection.getString().getSize() > 0)
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            textX += m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize({m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x,
                                              getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()});
            m_selectedTextBackground.setPosition({textX, m_paddingCached.getTop()});

            // Set the text selected text on the correct position
            m_textSelection.setPosition(textX, textY);

            // Watch out for kerning
            if (m_textFull.getString().getSize() > m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize())
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x;
            m_textAfterSelection.setPosition(textX, textY);
        }

        // Set the position of the caret
        caretLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_caret.getSize().x * 0.5f);
        m_caret.setPosition({caretLeft, m_paddingCached.getTop()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateSelection()
    {
        // Check if we are selecting text from left to right
        if (m_selEnd > m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selEnd - m_selStart;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selStart));
            m_textSelection.setString(m_textFull.getString().substring(m_selStart, m_selChars));
            m_textAfterSelection.setString(m_textFull.getString().substring(m_selEnd));
        }
        else if (m_selEnd < m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selStart - m_selEnd;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selEnd));
            m_textSelection.setString(m_textFull.getString().substring(m_selEnd, m_selChars));
            m_textAfterSelection.setString(m_textFull.getString().substring(m_selStart));
        }
        else
        {
            // Update the number of characters that are selected
            m_selChars = 0;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString());
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
        }

        if (!m_fontCached)
            return;

        // Check if scrolling is enabled
        if (!m_limitTextWidth)
        {
            // Find out the position of the caret
            const float caretPosition = m_textFull.findCharacterPos(m_selEnd).x;

            // If the caret is too far on the right then adjust the cropping
            if (m_textCropPosition + getVisibleEditBoxWidth() - (2 * Text::getExtraHorizontalPadding(m_textFull)) < caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition - getVisibleEditBoxWidth() + (2 * Text::getExtraHorizontalPadding(m_textFull)));

            // If the caret is too far on the left then adjust the cropping
            if (m_textCropPosition > caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition);
        }

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateTextColor()
    {
        if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorDisabled());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorDisabled());
        }
        else if (m_focused && getSharedRenderer()->getTextColorFocused().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorFocused());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorFocused());
        }
        else
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::update(sf::Time elapsedTime)
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

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorDisabledCached);
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else if (m_focused && m_borderColorFocusedCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorFocusedCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else if (m_mouseHover && m_spriteHover.isSet())
                m_spriteHover.draw(target, states);
            else if (m_focused && m_spriteFocused.isSet())
                m_spriteFocused.draw(target, states);
            else
                m_sprite.draw(target, states);
        }
        else // There is no background texture
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorDisabledCached);
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorHoverCached);
            else if (m_focused && m_backgroundColorFocusedCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorFocusedCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);
        }

        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            if ((m_textBeforeSelection.getString() != "") || (m_textSelection.getString() != ""))
            {
                m_textBeforeSelection.draw(target, states);

                if (m_textSelection.getString() != "")
                {
                    states.transform.translate(m_selectedTextBackground.getPosition());
                    drawRectangleShape(target, states, m_selectedTextBackground.getSize(), m_selectedTextBackgroundColorCached);
                    states.transform.translate(-m_selectedTextBackground.getPosition());

                    m_textSelection.draw(target, states);
                    m_textAfterSelection.draw(target, states);
                }
            }
            else if (m_defaultText.getString() != "")
            {
                m_defaultText.draw(target, states);
            }
        }

        // Draw the caret
        states.transform.translate(m_caret.getPosition());
        if (m_enabled && m_focused && m_caretVisible)
        {
            if (m_mouseHover && m_caretColorHoverCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorHoverCached);
            else if (m_focused && m_caretColorFocusedCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorFocusedCached);
            else
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorCached);
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


#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/SignalImpl.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid()
    {
        m_type = "Grid";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& gridToCopy) :
        Container {gridToCopy},
        m_autoSize{gridToCopy.m_autoSize}
    {
        for (std::size_t row = 0; row < gridToCopy.m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < gridToCopy.m_gridWidgets[row].size(); ++col)
            {
                // Find the widget that belongs in this square
                for (std::size_t i = 0; i < gridToCopy.m_widgets.size(); ++i)
                {
                    // If a widget matches then add it to the grid
                    if (gridToCopy.m_widgets[i] == gridToCopy.m_gridWidgets[row][col])
                        addWidget(m_widgets[i], row, col, gridToCopy.m_objPadding[row][col], gridToCopy.m_objAlignment[row][col]);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(Grid&& gridToMove) :
        Container           {std::move(gridToMove)},
        m_autoSize          {std::move(gridToMove.m_autoSize)},
        m_gridWidgets       {std::move(gridToMove.m_gridWidgets)},
        m_objPadding        {std::move(gridToMove.m_objPadding)},
        m_objAlignment      {std::move(gridToMove.m_objAlignment)},
        m_rowHeight         {std::move(gridToMove.m_rowHeight)},
        m_columnWidth       {std::move(gridToMove.m_columnWidth)},
        m_connectedCallbacks{}
    {
        for (auto& widget : m_widgets)
        {
            widget->disconnect(gridToMove.m_connectedCallbacks[widget]);
            m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (const Grid& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Container::operator=(right);
            m_autoSize = right.m_autoSize;
            m_connectedCallbacks.clear();

            for (std::size_t row = 0; row < right.m_gridWidgets.size(); ++row)
            {
                for (std::size_t col = 0; col < right.m_gridWidgets[row].size(); ++col)
                {
                    // Find the widget that belongs in this square
                    for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
                    {
                        // If a widget matches then add it to the grid
                        if (right.m_widgets[i] == right.m_gridWidgets[row][col])
                            addWidget(m_widgets[i], row, col, right.m_objPadding[row][col], right.m_objAlignment[row][col]);
                    }
                }
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (Grid&& right)
    {
        if (this != &right)
        {
            Container::operator=(std::move(right));
            m_autoSize           = std::move(right.m_autoSize);
            m_gridWidgets        = std::move(right.m_gridWidgets);
            m_objPadding         = std::move(right.m_objPadding);
            m_objAlignment       = std::move(right.m_objAlignment);
            m_rowHeight          = std::move(right.m_rowHeight);
            m_columnWidth        = std::move(right.m_columnWidth);
            m_connectedCallbacks = std::move(right.m_connectedCallbacks);

            for (auto& widget : m_widgets)
            {
                widget->disconnect(right.m_connectedCallbacks[widget]);
                m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Ptr Grid::create()
    {
        return std::make_shared<Grid>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Ptr Grid::copy(Grid::ConstPtr grid)
    {
        if (grid)
            return std::static_pointer_cast<Grid>(grid->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setSize(const Layout2d& size)
    {
        Container::setSize(size);

        m_autoSize = false;

        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            updatePositionsOfAllWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::remove(const Widget::Ptr& widget)
    {
        const auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
        {
            widget->disconnect(callbackIt->second);
            m_connectedCallbacks.erase(callbackIt);
        }

        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                {
                    // Remove the widget from the grid
                    m_gridWidgets[row].erase(m_gridWidgets[row].begin() + col);
                    m_objPadding[row].erase(m_objPadding[row].begin() + col);
                    m_objAlignment[row].erase(m_objAlignment[row].begin() + col);

                    // Check if this is the last column
                    if (m_columnWidth.size() == m_gridWidgets[row].size() + 1)
                    {
                        // Check if there is another row with this many columns
                        bool rowFound = false;
                        for (std::size_t i = 0; i < m_gridWidgets.size(); ++i)
                        {
                            if (m_gridWidgets[i].size() >= m_columnWidth.size())
                            {
                                rowFound = true;
                                break;
                            }
                        }

                        // Erase the last column if no other row is using it
                        if (!rowFound)
                            m_columnWidth.erase(m_columnWidth.end()-1);
                    }

                    // If the row is empty then remove it as well
                    if (m_gridWidgets[row].empty())
                    {
                        m_gridWidgets.erase(m_gridWidgets.begin() + row);
                        m_objPadding.erase(m_objPadding.begin() + row);
                        m_objAlignment.erase(m_objAlignment.begin() + row);
                        m_rowHeight.erase(m_rowHeight.begin() + row);
                    }

                    // Update the positions of all remaining widgets
                    updatePositionsOfAllWidgets();
                }
            }
        }

        return Container::remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::removeAllWidgets()
    {
        Container::removeAllWidgets();

        m_gridWidgets.clear();
        m_objPadding.clear();
        m_objAlignment.clear();

        m_rowHeight.clear();
        m_columnWidth.clear();

        m_connectedCallbacks.clear();

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addWidget(const Widget::Ptr& widget, std::size_t row, std::size_t col, const Padding& padding, Alignment alignment)
    {
        // If the widget hasn't already been added then add it now
        if (std::find(getWidgets().begin(), getWidgets().end(), widget) == getWidgets().end())
            add(widget);

        // Create the row if it did not exist yet
        if (m_gridWidgets.size() < row + 1)
        {
            m_gridWidgets.resize(row + 1);
            m_objPadding.resize(row + 1);
            m_objAlignment.resize(row + 1);
        }

        // Create the column if it did not exist yet
        if (m_gridWidgets[row].size() < col + 1)
        {
            m_gridWidgets[row].resize(col + 1, nullptr);
            m_objPadding[row].resize(col + 1);
            m_objAlignment[row].resize(col + 1);
        }

        // If this is a new row then reserve some space for it
        if (m_rowHeight.size() < row + 1)
            m_rowHeight.resize(row + 1, 0);

        // If this is the first row to have so many columns then reserve some space for it
        if (m_columnWidth.size() < col + 1)
            m_columnWidth.resize(col + 1, 0);

        // Add the widget to the grid
        m_gridWidgets[row][col] = widget;
        m_objPadding[row][col] = padding;
        m_objAlignment[row][col] = alignment;

        // Update the widgets
        updateWidgets();

        // Automatically update the widgets when their size changes
        m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Grid::getWidget(std::size_t row, std::size_t col) const
    {
        if ((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size()))
            return m_gridWidgets[row][col];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<Widget::Ptr, std::pair<std::size_t, std::size_t>> Grid::getWidgetLocations() const
    {
        std::map<Widget::Ptr, std::pair<std::size_t, std::size_t>> widgetsMap;

        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col])
                    widgetsMap[m_gridWidgets[row][col]] = {row, col};
            }
        }

        return widgetsMap;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetPadding(const Widget::Ptr& widget, const Padding& padding)
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    setWidgetPadding(row, col, padding);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetPadding(std::size_t row, std::size_t col, const Padding& padding)
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
        {
            // Change padding of the widget
            m_objPadding[row][col] = padding;

            // Update all widgets
            updateWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding Grid::getWidgetPadding(const Widget::Ptr& widget) const
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    return getWidgetPadding(row, col);
            }
        }

        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding Grid::getWidgetPadding(std::size_t row, std::size_t col) const
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
            return m_objPadding[row][col];
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetAlignment(const Widget::Ptr& widget, Alignment alignment)
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    setWidgetAlignment(row, col, alignment);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetAlignment(std::size_t row, std::size_t col, Alignment alignment)
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
        {
            m_objAlignment[row][col] = alignment;
            updatePositionsOfAllWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Alignment Grid::getWidgetAlignment(const Widget::Ptr& widget) const
    {
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    return getWidgetAlignment(row, col);
            }
        }

        return Alignment::Center;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Alignment Grid::getWidgetAlignment(std::size_t row, std::size_t col) const
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
            return m_objAlignment[row][col];
        else
            return Alignment::Center;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::vector<Widget::Ptr>>& Grid::getGridWidgets() const
    {
        return m_gridWidgets;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Grid::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        const auto& children = getWidgets();
        auto widgetsMap = getWidgetLocations();
        if (!widgetsMap.empty())
        {
            auto alignmentToString = [](Grid::Alignment alignment) -> std::string {
                switch (alignment)
                {
                case Grid::Alignment::Center:
                    return "Center";
                case Grid::Alignment::UpperLeft:
                    return "UpperLeft";
                case Grid::Alignment::Up:
                    return "Up";
                case Grid::Alignment::UpperRight:
                    return "UpperRight";
                case Grid::Alignment::Right:
                    return "Right";
                case Grid::Alignment::BottomRight:
                    return "BottomRight";
                case Grid::Alignment::Bottom:
                    return "Bottom";
                case Grid::Alignment::BottomLeft:
                    return "BottomLeft";
                case Grid::Alignment::Left:
                    return "Left";
                default:
                    throw Exception{"Invalid grid alignment encountered."};
                }
            };

            auto getWidgetsInGridString = [&](const Widget::Ptr& w) -> std::string {
                auto it = widgetsMap.find(w);
                if (it != widgetsMap.end())
                {
                    const auto row = it->second.first;
                    const auto col = it->second.second;
                    return "\"(" + to_string(row)
                         + ", " + to_string(col)
                         + ", " + getWidgetPadding(row, col).toString()
                         + ", " + alignmentToString(getWidgetAlignment(row, col))
                         + ")\"";
                }
                else
                    return "\"()\"";
            };

            std::string str = "[" + getWidgetsInGridString(children[0]);

            for (std::size_t i = 1; i < children.size(); ++i)
                str += ", " + getWidgetsInGridString(children[i]);

            str += "]";
            node->propertyValuePairs["GridWidgets"] = std::make_unique<DataIO::ValueNode>(str);
        }

        if (m_autoSize)
            node->propertyValuePairs.erase("Size");

        node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_autoSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs["autosize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autosize"]->value).getBool());

        if (node->propertyValuePairs["gridwidgets"])
        {
            if (!node->propertyValuePairs["gridwidgets"]->listNode)
                throw Exception{"Failed to parse 'GridWidgets' property, expected a list as value"};

            const auto& elements = node->propertyValuePairs["gridwidgets"]->valueList;
            if (elements.size() != getWidgets().size())
                throw Exception{"Failed to parse 'GridWidgets' property, the amount of items has to match with the amount of child widgets"};

            for (unsigned int i = 0; i < elements.size(); ++i)
            {
                std::string str = elements[i].toAnsiString();

                // Remove quotes
                if ((str.size() >= 2) && (str[0] == '"') && (str[str.size()-1] == '"'))
                    str = str.substr(1, str.size()-2);

                // Remove brackets
                if ((str.size() >= 2) && (str[0] == '(') && (str[str.size()-1] == ')'))
                    str = str.substr(1, str.size()-2);

                // Ignore empty values (which are widgets that have not been given a location in the grid)
                if (str.empty())
                    continue;

                int row;
                int col;
                Padding padding;
                auto alignment = Grid::Alignment::Center;

                std::size_t index = 0;
                std::size_t pos = str.find(',');
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after row."};

                row = tgui::stoi(str.substr(index, pos - index));
                index = pos + 1;

                pos = str.find(',', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after column."};

                col = tgui::stoi(str.substr(index, pos - index));
                index = pos + 1;

                if (row < 0 || col < 0)
                    throw Exception{"Failed to parse 'GridWidgets' property, row and column have to be positive integers"};

                pos = str.find('(', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing opening bracket for padding."};

                index = pos;
                pos = str.find(')', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing closing bracket for padding."};

                padding = Deserializer::deserialize(ObjectConverter::Type::Outline, str.substr(index, pos+1 - index)).getOutline();
                index = pos + 1;

                pos = str.find(',', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after padding."};

                std::string alignmentStr = toLower(trim(str.substr(pos + 1)));
                if (alignmentStr == "center")
                    alignment = Grid::Alignment::Center;
                else if (alignmentStr == "upperleft")
                    alignment = Grid::Alignment::UpperLeft;
                else if (alignmentStr == "up")
                    alignment = Grid::Alignment::Up;
                else if (alignmentStr == "upperright")
                    alignment = Grid::Alignment::UpperRight;
                else if (alignmentStr == "right")
                    alignment = Grid::Alignment::Right;
                else if (alignmentStr == "bottomright")
                    alignment = Grid::Alignment::BottomRight;
                else if (alignmentStr == "bottom")
                    alignment = Grid::Alignment::Bottom;
                else if (alignmentStr == "bottomleft")
                    alignment = Grid::Alignment::BottomLeft;
                else if (alignmentStr == "left")
                    alignment = Grid::Alignment::Left;
                else
                    throw Exception{"Failed to parse 'GridWidgets' property. Invalid alignment '" + alignmentStr + "'."};

                addWidget(getWidgets()[i], static_cast<std::size_t>(row), static_cast<std::size_t>(col), padding, alignment);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Grid::getMinimumSize() const
    {
        // Calculate the required space to have all widgets in the grid.
        Vector2f minSize;

        // Loop through all rows to find the minimum height required by the grid
        for (float rowHeight : m_rowHeight)
            minSize.y += rowHeight;

        // Loop through all columns to find the minimum width required by the grid
        for (float columnWidth : m_columnWidth)
            minSize.x += columnWidth;

        return minSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllWidgets()
    {
        Vector2f extraOffset;
        if (!m_autoSize && m_gridWidgets.size() > 1)
        {
            const Vector2f minimumSize = getMinimumSize();
            if (getSize().x > minimumSize.x)
                extraOffset.x = (getSize().x - minimumSize.x) / (m_gridWidgets.size() - 1);
            if (getSize().y > minimumSize.y)
                extraOffset.y = (getSize().y - minimumSize.y) / (m_gridWidgets.size() - 1);
        }

        Vector2f position;
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            const Vector2f previousPosition = position;

            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                {
                    position.x += m_columnWidth[col] + extraOffset.x;
                    continue;
                }

                Vector2f cellPosition = position + (extraOffset / 2.f);
                switch (m_objAlignment[row][col])
                {
                case Alignment::UpperLeft:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::Up:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::UpperRight:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::Right:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::BottomRight:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Bottom:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::BottomLeft:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Left:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::Center:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;
                }

                m_gridWidgets[row][col]->setPosition(cellPosition);
                position.x += m_columnWidth[col] + extraOffset.x;
            }

            // Move to the next row
            position = previousPosition;
            position.y += m_rowHeight[row] + extraOffset.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateWidgets()
    {
        // Reset the column widths
        for (float& width : m_columnWidth)
            width = 0;

        // Loop through all widgets
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            // Reset the row height
            m_rowHeight[row] = 0;

            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                    continue;

                // Remember the biggest column width
                if (m_columnWidth[col] < m_gridWidgets[row][col]->getFullSize().x + m_objPadding[row][col].getLeft() + m_objPadding[row][col].getRight())
                    m_columnWidth[col] = m_gridWidgets[row][col]->getFullSize().x + m_objPadding[row][col].getLeft() + m_objPadding[row][col].getRight();

                // Remember the biggest row height
                if (m_rowHeight[row] < m_gridWidgets[row][col]->getFullSize().y + m_objPadding[row][col].getTop() + m_objPadding[row][col].getBottom())
                    m_rowHeight[row] = m_gridWidgets[row][col]->getFullSize().y + m_objPadding[row][col].getTop() + m_objPadding[row][col].getBottom();
            }
        }

        if (m_autoSize)
        {
            sf::Vector2f size;
            for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
            {
                float rowWidth = 0;
                for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
                    rowWidth += m_columnWidth[col];

                size.x = std::max(size.x, rowWidth);
                size.y += m_rowHeight[row];
            }

            Container::setSize(size);
        }

        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw all widgets
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() != nullptr)
                {
                    if (m_gridWidgets[row][col]->isVisible())
                        m_gridWidgets[row][col]->draw(target, states);
                }
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


#include <TGUI/Widgets/HorizontalWrap.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::HorizontalWrap(const Layout2d& size) :
        BoxLayout{size}
    {
        m_type = "HorizontalWrap";

        m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::Ptr HorizontalWrap::create(const Layout2d& size)
    {
        return std::make_shared<HorizontalWrap>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::Ptr HorizontalWrap::copy(HorizontalWrap::ConstPtr layout)
    {
        if (layout)
            return std::static_pointer_cast<HorizontalWrap>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void HorizontalWrap::updateWidgets()
    {
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                          getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        float currentHorizontalOffset = 0;
        float currentVerticalOffset = 0;
        float lineHeight = 0;
        for (const auto& widget : m_widgets)
        {
            const auto size = widget->getSize();

            if (currentHorizontalOffset + size.x > contentSize.x)
            {
                currentVerticalOffset += lineHeight + m_spaceBetweenWidgetsCached;
                currentHorizontalOffset = 0;
                lineHeight = 0;
            }

            widget->setPosition({currentHorizontalOffset, currentVerticalOffset});

            currentHorizontalOffset += size.x + m_spaceBetweenWidgetsCached;

            if (lineHeight < size.y)
                lineHeight = size.y;
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


#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

#ifdef TGUI_USE_CPP17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Label()
    {
        m_type = "Label";

        m_renderer = aurora::makeCopied<LabelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::create(sf::String text)
    {
        auto label = std::make_shared<Label>();

        if (!text.isEmpty())
            label->setText(text);

        return label;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::Ptr Label::copy(Label::ConstPtr label)
    {
        if (label)
            return std::static_pointer_cast<Label>(label->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LabelRenderer* Label::getSharedRenderer()
    {
        return aurora::downcast<LabelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const LabelRenderer* Label::getSharedRenderer() const
    {
        return aurora::downcast<const LabelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LabelRenderer* Label::getRenderer()
    {
        return aurora::downcast<LabelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const LabelRenderer* Label::getRenderer() const
    {
        return aurora::downcast<const LabelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        // You are no longer auto-sizing
        m_autoSize = false;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setText(const sf::String& string)
    {
        m_string = string;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& Label::getText() const
    {
        return m_string;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setTextSize(unsigned int size)
    {
        if (size != m_textSize)
        {
            m_textSize = size;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Label::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setHorizontalAlignment(HorizontalAlignment alignment)
    {
        m_horizontalAlignment = alignment;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::HorizontalAlignment Label::getHorizontalAlignment() const
    {
        return m_horizontalAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setVerticalAlignment(VerticalAlignment alignment)
    {
        m_verticalAlignment = alignment;
        rearrangeText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Label::VerticalAlignment Label::getVerticalAlignment() const
    {
        return m_verticalAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setMaximumTextWidth(float maximumWidth)
    {
        if (m_maximumTextWidth != maximumWidth)
        {
            m_maximumTextWidth = maximumWidth;
            rearrangeText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Label::getMaximumTextWidth() const
    {
        if (m_autoSize)
            return m_maximumTextWidth;
        else
            return getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::setParent(Container* parent)
    {
        const bool autoSize = getAutoSize();
        Widget::setParent(parent);
        setAutoSize(autoSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Label::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                onDoubleClick.emit(this, m_string);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Label::getSignal(std::string signalName)
    {
        if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            m_bordersCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());
            rearrangeText();
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();
            rearrangeText();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            for (auto& line : m_lines)
                line.setColor(m_textColorCached);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);
            rearrangeText();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            for (auto& line : m_lines)
                line.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Label::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_horizontalAlignment == Label::HorizontalAlignment::Center)
            node->propertyValuePairs["HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_horizontalAlignment == Label::HorizontalAlignment::Right)
            node->propertyValuePairs["HorizontalAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (m_verticalAlignment == Label::VerticalAlignment::Center)
            node->propertyValuePairs["VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_verticalAlignment == Label::VerticalAlignment::Bottom)
            node->propertyValuePairs["VerticalAlignment"] = std::make_unique<DataIO::ValueNode>("Bottom");

        if (!m_string.isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_string));
        if (m_maximumTextWidth > 0)
            node->propertyValuePairs["MaximumTextWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximumTextWidth));
        if (m_autoSize)
            node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>("true");
        if (m_ignoringMouseEvents)
            node->propertyValuePairs["IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["horizontalalignment"])
        {
            std::string alignment = toLower(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["horizontalalignment"]->value).getString());
            if (alignment == "right")
                setHorizontalAlignment(Label::HorizontalAlignment::Right);
            else if (alignment == "center")
                setHorizontalAlignment(Label::HorizontalAlignment::Center);
            else if (alignment != "left")
                throw Exception{"Failed to parse HorizontalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["verticalalignment"])
        {
            std::string alignment = toLower(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["verticalalignment"]->value).getString());
            if (alignment == "bottom")
                setVerticalAlignment(Label::VerticalAlignment::Bottom);
            else if (alignment == "center")
                setVerticalAlignment(Label::VerticalAlignment::Center);
            else if (alignment != "top")
                throw Exception{"Failed to parse VerticalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumtextwidth"])
            setMaximumTextWidth(tgui::stof(node->propertyValuePairs["maximumtextwidth"]->value));
        if (node->propertyValuePairs["autosize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autosize"]->value).getBool());

        if (node->propertyValuePairs["ignoremouseevents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["ignoremouseevents"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::rearrangeText()
    {
        m_lines.clear();

        if (m_fontCached == nullptr)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize, m_textStyleCached);

        // Find the maximum width of one line
        float maxWidth;
        if (m_autoSize)
            maxWidth = std::max(0.f, m_maximumTextWidth - 2*textOffset);
        else
        {
            maxWidth = getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2*textOffset;
            if (maxWidth <= 0)
                return;
        }

        // Fit the text in the available space
        const sf::String string = Text::wordWrap(maxWidth, m_string, m_fontCached, m_textSize, m_textStyleCached & sf::Text::Bold);

        // Split the string in multiple lines
        float width = 0;
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != sf::String::InvalidPos)
        {
            newLinePos = string.find('\n', searchPosStart);

            m_lines.emplace_back();
            m_lines.back().setCharacterSize(getTextSize());
            m_lines.back().setFont(m_fontCached);
            m_lines.back().setStyle(m_textStyleCached);
            m_lines.back().setColor(m_textColorCached);
            m_lines.back().setOpacity(m_opacityCached);

            if (newLinePos != sf::String::InvalidPos)
                m_lines.back().setString(string.substring(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.back().setString(string.substring(searchPosStart));

            if (m_lines.back().getSize().x > width)
                width = m_lines.back().getSize().x;

            searchPosStart = newLinePos + 1;
        }

        const Outline outline = {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                                 m_paddingCached.getTop() + m_bordersCached.getTop(),
                                 m_paddingCached.getRight() + m_bordersCached.getRight(),
                                 m_paddingCached.getBottom() + m_bordersCached.getBottom()};

        // Update the size of the label
        if (m_autoSize)
        {
            Widget::setSize({std::max(width, maxWidth) + outline.getLeft() + outline.getRight() + 2*textOffset,
                            (std::max<std::size_t>(m_lines.size(), 1) * m_fontCached.getLineSpacing(m_textSize))
                             + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize, m_textStyleCached)
                             + Text::getExtraVerticalPadding(m_textSize) + outline.getTop() + outline.getBottom()});

            m_bordersCached.updateParentSize(getSize());
            m_paddingCached.updateParentSize(getSize());
        }

        // Update the line positions
        {
            if ((getSize().x <= outline.getLeft() + outline.getRight()) || (getSize().y <= outline.getTop() + outline.getBottom()))
                return;

            Vector2f pos{m_paddingCached.getLeft() + textOffset, m_paddingCached.getTop()};

            if (m_verticalAlignment != VerticalAlignment::Top)
            {
                const float totalHeight = getSize().y - outline.getTop() - outline.getBottom();
                const float totalTextHeight = m_lines.size() * m_fontCached.getLineSpacing(m_textSize);

                if (m_verticalAlignment == VerticalAlignment::Center)
                    pos.y += (totalHeight - totalTextHeight) / 2.f;
                else if (m_verticalAlignment == VerticalAlignment::Bottom)
                    pos.y += totalHeight - totalTextHeight;
            }

            if (m_horizontalAlignment == HorizontalAlignment::Left)
            {
                const float lineSpacing = m_fontCached.getLineSpacing(m_textSize);
                for (auto& line : m_lines)
                {
                    line.setPosition(pos.x, pos.y);
                    pos.y += lineSpacing;
                }
            }
            else // Center or Right alignment
            {
                const float totalWidth = getSize().x - outline.getLeft() - outline.getRight() - 2*textOffset;

                for (auto& line : m_lines)
                {
                    std::size_t lastChar = line.getString().getSize();
                    while (lastChar > 0 && isWhitespace(line.getString()[lastChar-1]))
                        lastChar--;

                    const float textWidth = line.findCharacterPos(lastChar).x;

                    if (m_horizontalAlignment == HorizontalAlignment::Center)
                        line.setPosition(pos.x + ((totalWidth - textWidth) / 2.f), pos.y);
                    else // if (m_horizontalAlignment == HorizontalAlignment::Right)
                        line.setPosition(pos.x + totalWidth - textWidth, pos.y);

                    pos.y += m_fontCached.getLineSpacing(m_textSize);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(std::round(getPosition().x), std::round(getPosition().y));

        Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                              getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        if (m_backgroundColorCached.isSet() && (m_backgroundColorCached != Color::Transparent))
            drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        // Apply clipping when needed
    #ifdef TGUI_USE_CPP17
        std::optional<Clipping> clipping;
    #else
        std::unique_ptr<Clipping> clipping;
    #endif
        if (!m_autoSize)
        {
            innerSize.x -= m_paddingCached.getLeft() + m_paddingCached.getRight();
            innerSize.y -= m_paddingCached.getTop() + m_paddingCached.getBottom();

        #ifdef TGUI_USE_CPP17
            clipping.emplace(target, states, Vector2f{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize);
        #else
            clipping = std::make_unique<Clipping>(target, states, Vector2f{m_paddingCached.getLeft(), m_paddingCached.getTop()}, innerSize);
        #endif
        }

        // Draw the text
        for (const auto& line : m_lines)
            line.draw(target, states);
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


#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Clipping.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        bool removeMenuImpl(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<MenuBar::Menu>& menus)
        {
            for (auto it = menus.begin(); it != menus.end(); ++it)
            {
                if (it->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    menus.erase(it);
                    return true;
                }
                else
                {
                    // Return false if some menu in the hierarchy couldn't be found
                    if (!removeMenuImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, it->menuItems))
                        return false;

                    // If parents don't have to be removed as well then we are done
                    if (!removeParentsWhenEmpty)
                        return true;

                    // Also delete the parent if empty
                    if (it->menuItems.empty())
                        menus.erase(it);

                    return true;
                }
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void setTextSizeImpl(std::vector<MenuBar::Menu>& menus, unsigned int textSize)
        {
            for (auto& menu : menus)
            {
                menu.text.setCharacterSize(textSize);
                if (!menu.menuItems.empty())
                    setTextSizeImpl(menu.menuItems, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> getAllMenusImpl(const std::vector<MenuBar::Menu>& menus)
        {
            std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> menuElements;

            for (const auto& menu : menus)
            {
                menuElements.emplace_back(std::make_unique<MenuBar::GetAllMenusElement>());
                menuElements.back()->text = menu.text.getString();
                menuElements.back()->enabled = menu.enabled;
                if (!menu.menuItems.empty())
                    menuElements.back()->menuItems = getAllMenusImpl(menu.menuItems);
            }

            return menuElements;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<MenuBar::GetMenuListElement> getMenuListImpl(const std::vector<MenuBar::Menu>& menus)
        {
            std::vector<MenuBar::GetMenuListElement> menuElements;

            for (const auto& menu : menus)
            {
                menuElements.emplace_back();
                menuElements.back().text = menu.text.getString();
                menuElements.back().enabled = menu.enabled;
                if (!menu.menuItems.empty())
                    menuElements.back().menuItems = getMenuListImpl(menu.menuItems);
            }

            return menuElements;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveMenus(std::unique_ptr<DataIO::Node>& parentNode, const std::vector<MenuBar::Menu>& menus)
        {
            for (const auto& menu : menus)
            {
                auto menuNode = std::make_unique<DataIO::Node>();
                menuNode->name = "Menu";

                menuNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.text.getString()));
                if (!menu.enabled)
                    menuNode->propertyValuePairs["Enabled"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.enabled));

                if (!menu.menuItems.empty())
                {
                    // Save as nested 'Menu' sections only when needed, use the more compact string list when just storing the menu items
                    bool recursionNeeded = false;
                    for (const auto& menuItem : menu.menuItems)
                    {
                        if (!menuItem.enabled || !menuItem.menuItems.empty())
                        {
                            recursionNeeded = true;
                            break;
                        }
                    }

                    if (recursionNeeded)
                        saveMenus(menuNode, menu.menuItems);
                    else
                    {
                        std::string itemList = "[" + Serializer::serialize(menu.menuItems[0].text.getString());
                        for (std::size_t i = 1; i < menu.menuItems.size(); ++i)
                            itemList += ", " + Serializer::serialize(menu.menuItems[i].text.getString());
                        itemList += "]";

                        menuNode->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                    }
                }

                parentNode->children.push_back(std::move(menuNode));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_type = "MenuBar";
        m_distanceToSideCached = Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f;

        m_renderer = aurora::makeCopied<MenuBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setMinimumSubMenuWidth((Text::getLineHeight(m_fontCached, m_textSize) * 4) + (2 * m_distanceToSideCached));
        setSize({"100%", Text::getLineHeight(m_fontCached, m_textSize) * 1.25f});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create()
    {
        return std::make_shared<MenuBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::copy(MenuBar::ConstPtr menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<MenuBar>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getSharedRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getSharedRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);

        if (!enabled)
        {
            closeMenu();
            updateTextColors(m_menus, m_visibleMenu);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        createMenu(m_menus, text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& text)
    {
        if (!m_menus.empty())
            return addMenuItem(m_menus.back().text.getString(), text);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        return addMenuItem({menu, text}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const std::vector<sf::String>& hierarchy, bool createParents)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, createParents);
        if (!menu)
            return false;

        createMenu(menu->menuItems, hierarchy.back());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const sf::String& menu)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            m_menus.erase(m_menus.begin() + i);

            // The menu was removed, so it can't remain open
            if (m_visibleMenu == static_cast<int>(i))
                m_visibleMenu = -1;

            return true;
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const sf::String& menu, const sf::String& menuItem)
    {
        return removeMenuItem({menu, menuItem}, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty)
    {
        if (hierarchy.size() < 2)
            return false;

        return removeMenuImpl(hierarchy, removeParentsWhenEmpty, 0, m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuEnabled(const sf::String& menu, bool enabled)
    {
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            if (m_menus[i].text.getString() != menu)
                continue;

            if (!enabled && (m_visibleMenu == static_cast<int>(i)))
                closeMenu();

            m_menus[i].enabled = enabled;
            updateMenuTextColor(m_menus[i], (m_visibleMenu == static_cast<int>(i)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuEnabled(const sf::String& menuText) const
    {
        for (auto& menu : m_menus)
        {
            if (menu.text.getString() == menuText)
                return menu.enabled;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const sf::String& menuText, const sf::String& menuItemText, bool enabled)
    {
        return setMenuItemEnabled({menuText, menuItemText}, enabled);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::setMenuItemEnabled(const std::vector<sf::String>& hierarchy, bool enabled)
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menu = findMenu(hierarchy, 0, m_menus, false);
        if (!menu)
            return false;

        for (unsigned int j = 0; j < menu->menuItems.size(); ++j)
        {
            auto& menuItem = menu->menuItems[j];
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            if (!enabled && (menu->selectedMenuItem == static_cast<int>(j)))
                menu->selectedMenuItem = -1;

            menuItem.enabled = enabled;
            updateMenuTextColor(menuItem, (menu->selectedMenuItem == static_cast<int>(j)));
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const sf::String& menuText, const sf::String& menuItemText) const
    {
        return getMenuItemEnabled({menuText, menuItemText});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getMenuItemEnabled(const std::vector<sf::String>& hierarchy) const
    {
        if (hierarchy.size() < 2)
            return false;

        auto* menuItem = findMenuItem(hierarchy);
        if (!menuItem)
            return false;

        return menuItem->enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;
        setTextSizeImpl(m_menus, size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setInvertedMenuDirection(bool invertDirection)
    {
        m_invertedMenuDirection = invertDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getInvertedMenuDirection() const
    {
        return m_invertedMenuDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    std::vector<std::pair<sf::String, std::vector<sf::String>>> MenuBar::getMenus() const
    {
        std::vector<std::pair<sf::String, std::vector<sf::String>>> menus;

        for (const auto& menu : m_menus)
        {
            std::vector<sf::String> items;
            for (const auto& item : menu.menuItems)
                items.push_back(item.text.getString());

            menus.emplace_back(menu.text.getString(), std::move(items));
        }

        return menus;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::unique_ptr<MenuBar::GetAllMenusElement>> MenuBar::getAllMenus() const
    {
        return getAllMenusImpl(m_menus);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<MenuBar::GetMenuListElement> MenuBar::getMenuList() const
    {
        return getMenuListImpl(m_menus);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeMenu()
    {
        if (m_visibleMenu != -1)
            closeSubMenus(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
            return true;
        else if (m_visibleMenu != -1)
        {
            Vector2f menuPos = getPosition();
            for (int i = 0; i < m_visibleMenu; ++i)
                menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            if (m_invertedMenuDirection)
                menuPos.y -= getSize().y * m_menus[m_visibleMenu].menuItems.size();
            else
                menuPos.y += getSize().y;

            // The menu is moved to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
            {
                menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            return isMouseOnTopOfMenu(menuPos, pos, openSubMenuToRight, m_menus[m_visibleMenu], menuWidth);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(Vector2f pos)
    {
        // Check if a menu should be opened or closed
        pos -= getPosition();
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (std::size_t i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x >= menuWidth)
                    continue;

                // Close the menu when it was already open
                if (m_visibleMenu == static_cast<int>(i))
                    closeMenu();

                // If this menu can be opened then do so
                else if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                {
                    updateMenuTextColor(m_menus[i], true);
                    m_visibleMenu = static_cast<int>(i);
                }

                break;
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(Vector2f pos)
    {
        if (!m_mouseDown || (m_visibleMenu == -1))
            return;

        pos -= getPosition();

        // Check if the mouse is on top of one of the menus
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return;

        auto* menu = &m_menus[m_visibleMenu];
        std::vector<sf::String> hierarchy;
        hierarchy.push_back(m_menus[m_visibleMenu].text.getString());
        while (menu->selectedMenuItem != -1)
        {
            auto& menuItem = menu->menuItems[menu->selectedMenuItem];
            hierarchy.push_back(menuItem.text.getString());
            if (menuItem.menuItems.empty())
            {
                onMenuItemClick.emit(this, menuItem.text.getString(), hierarchy);
                closeMenu();
                break;
            }

            menu = &menuItem;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Don't open a menu without having clicked first
        if (m_visibleMenu == -1)
            return;

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (FloatRect{getPosition(), getSize()}.contains(pos))
        {
            pos -= getPosition();

            // Loop through the menus to check if the mouse is on top of them
            bool handled = false;
            float menuWidth = 0;
            for (std::size_t i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x >= menuWidth)
                    continue;

                // Check if the menu is already open
                if (m_visibleMenu == static_cast<int>(i))
                {
                    // If one of the menu items is selected then unselect it
                    if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    {
                        updateMenuTextColor(m_menus[i].menuItems[m_menus[m_visibleMenu].selectedMenuItem], false);
                        m_menus[m_visibleMenu].selectedMenuItem = -1;
                    }
                }
                else // The menu isn't open yet
                {
                    // If there is another menu open then close it first
                    closeMenu();

                    // If this menu can be opened then do so
                    if (m_menus[i].enabled && !m_menus[i].menuItems.empty())
                    {
                        updateMenuTextColor(m_menus[i], true);
                        m_visibleMenu = static_cast<int>(i);
                    }
                }

                handled = true;
                break;
            }

            // The mouse is to the right of all menus, deselect the selected item of the deepest submenu
            if (!handled)
                deselectBottomItem();
        }
        else // The mouse is on top of the open menu
        {
            Vector2f menuPos = getPosition();
            for (int i = 0; i < m_visibleMenu; ++i)
                menuPos.x += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            if (m_invertedMenuDirection)
                menuPos.y -= getSize().y * m_menus[m_visibleMenu].menuItems.size();
            else
                menuPos.y += getSize().y;

            // The menu is moved to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (menuPos.x + menuWidth > getParent()->getInnerSize().x))
            {
                menuPos.x = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            Menu* menuBelowMouse;
            int menuItemIndexBelowMouse;
            if (findMenuItemBelowMouse(menuPos, pos, openSubMenuToRight, m_menus[m_visibleMenu], menuWidth, &menuBelowMouse, &menuItemIndexBelowMouse))
            {
                // Check if the mouse is on a different item than before
                auto& menu = *menuBelowMouse;
                if (menuItemIndexBelowMouse != menu.selectedMenuItem)
                {
                    // If another of the menu items is selected then unselect it
                    if (menu.selectedMenuItem != -1)
                        closeSubMenus(menu.menuItems, menu.selectedMenuItem);

                    // Mark the item below the mouse as selected
                    if (menu.menuItems[menuItemIndexBelowMouse].enabled)
                    {
                        updateMenuTextColor(menu.menuItems[menuItemIndexBelowMouse], true);
                        menu.selectedMenuItem = menuItemIndexBelowMouse;
                    }
                }
                else // We already selected this item
                {
                    // If the selected item has a submenu then unselect its item
                    if (menu.menuItems[menuItemIndexBelowMouse].selectedMenuItem != -1)
                        closeSubMenus(menu.menuItems[menuItemIndexBelowMouse].menuItems, menu.menuItems[menuItemIndexBelowMouse].selectedMenuItem);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        // Close the open menu, but not when it just opened because of this mouse click
        if (!m_mouseDown)
            closeMenu();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Deselect the selected item of the deepest submenu
        if (m_visibleMenu != -1)
            deselectBottomItem();

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MenuBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMenuItemClick.getName()))
            return onMenuItemClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "textcolordisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors(m_menus, m_visibleMenu);
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "textureitembackground")
        {
            m_spriteItemBackground.setTexture(getSharedRenderer()->getTextureItemBackground());
        }
        else if (property == "textureselecteditembackground")
        {
            m_spriteSelectedItemBackground.setTexture(getSharedRenderer()->getTextureSelectedItemBackground());
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);
            updateTextOpacity(m_menus);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);
            updateTextFont(m_menus);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MenuBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        saveMenus(node, m_menus);

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["MinimumSubMenuWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimumSubMenuWidth));
        if (m_invertedMenuDirection)
            node->propertyValuePairs["InvertedMenuDirection"] = std::make_unique<DataIO::ValueNode>("true");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["minimumsubmenuwidth"])
            setMinimumSubMenuWidth(tgui::stof(node->propertyValuePairs["minimumsubmenuwidth"]->value));
        if (node->propertyValuePairs["invertedmenudirection"])
            setInvertedMenuDirection(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, node->propertyValuePairs["invertedmenudirection"]->value).getBool());

        loadMenus(node, m_menus);

        // Remove the 'menu' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "menu"; }), node->children.end());

        // Update the text colors to properly display disabled menus
        updateTextColors(m_menus, m_visibleMenu);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        if (m_menus.empty())
            return;

        Sprite backgroundSprite = m_spriteItemBackground;
        drawMenusOnBar(target, states, backgroundSprite);

        // Draw the menu if one is opened
        if (m_visibleMenu >= 0)
        {
            // Find the position of the menu
            float leftOffset = 0;
            for (int i = 0; i < m_visibleMenu; ++i)
                leftOffset += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

            // Move the menu to the left if it otherwise falls off the screen
            bool openSubMenuToRight = true;
            const float menuWidth = calculateMenuWidth(m_menus[m_visibleMenu]);
            if (getParent() && (getPosition().x + leftOffset + menuWidth > getParent()->getInnerSize().x))
            {
                leftOffset = std::max(0.f, getParent()->getInnerSize().x - menuWidth);
                openSubMenuToRight = false;
            }

            if (m_invertedMenuDirection)
                states.transform.translate({leftOffset, -getSize().y * m_menus[m_visibleMenu].menuItems.size()});
            else
                states.transform.translate({leftOffset, getSize().y});

            drawMenu(target, states, m_menus[m_visibleMenu], menuWidth, backgroundSprite, getPosition().x + leftOffset, openSubMenuToRight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::createMenu(std::vector<Menu>& menus, const sf::String& text)
    {
        Menu newMenu;
        newMenu.text.setFont(m_fontCached);
        newMenu.text.setColor(m_textColorCached);
        newMenu.text.setOpacity(m_opacityCached);
        newMenu.text.setCharacterSize(m_textSize);
        newMenu.text.setString(text);
        menus.push_back(std::move(newMenu));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Menu* MenuBar::findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<Menu>& menus, bool createParents)
    {
        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems, createParents);
        }

        if (createParents)
        {
            createMenu(menus, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return &menus.back();
            else
                return findMenu(hierarchy, parentIndex + 1, menus.back().menuItems, createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBar::Menu* MenuBar::findMenu(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, const std::vector<Menu>& menus) const
    {
        for (auto& menu : menus)
        {
            if (menu.text.getString() != hierarchy[parentIndex])
                continue;

            if (parentIndex + 2 == hierarchy.size())
                return &menu;
            else
                return findMenu(hierarchy, parentIndex + 1, menu.menuItems);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBar::Menu* MenuBar::findMenuItem(const std::vector<sf::String>& hierarchy) const
    {
        if (hierarchy.size() < 2)
            return nullptr;

        const auto* menu = findMenu(hierarchy, 0, m_menus);
        if (!menu)
            return nullptr;

        for (auto& menuItem : menu->menuItems)
        {
            if (menuItem.text.getString() != hierarchy.back())
                continue;

            return &menuItem;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::loadMenus(const std::unique_ptr<DataIO::Node>& node, std::vector<Menu>& menus)
    {
        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "menu")
                continue;

            // Every menu node should either have a Name (TGUI 0.8.0) or a Text (TGUI >= 0.8.1) property
        #ifdef TGUI_REMOVE_DEPRECATED_CODE
            if (!childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Menu' property, expected a nested 'Text' propery"};

            const sf::String menuText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString();
            createMenu(menus, menuText);
        #else
            if (!childNode->propertyValuePairs["name"] && !childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Menu' property, expected a nested 'Text' propery"};

            const sf::String menuText = Deserializer::deserialize(ObjectConverter::Type::String,
                childNode->propertyValuePairs[childNode->propertyValuePairs["text"] ? "text" : "name"]->value).getString();
            createMenu(menus, menuText);
        #endif

            if (childNode->propertyValuePairs["enabled"])
                menus.back().enabled = Deserializer::deserialize(ObjectConverter::Type::Bool, childNode->propertyValuePairs["enabled"]->value).getBool();

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadMenus(childNode, menus.back().menuItems);

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs["items"])
            {
                if (!childNode->propertyValuePairs["items"]->listNode)
                    throw Exception{"Failed to parse 'Items' property inside 'Menu' property, expected a list as value"};

            #ifndef TGUI_REMOVE_DEPRECATED_CODE
                // The ItemsEnabled property existed in TGUI 0.8.0 but was replaced with nested Menu sections in TGUI 0.8.1
                if (childNode->propertyValuePairs["itemsenabled"])
                {
                    if (!childNode->propertyValuePairs["itemsenabled"]->listNode)
                        throw Exception{"Failed to parse 'ItemsEnabled' property inside 'Menu' property, expected a list as value"};
                    if (childNode->propertyValuePairs["items"]->valueList.size() != childNode->propertyValuePairs["itemsenabled"]->valueList.size())
                        throw Exception{"Failed to parse 'ItemsEnabled' property inside 'Menu' property, length differs from 'Items' propery"};
                }
            #endif

                for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    const sf::String menuItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString();
                    createMenu(menus.back().menuItems, menuItemText);

                #ifndef TGUI_REMOVE_DEPRECATED_CODE
                    if (childNode->propertyValuePairs["itemsenabled"])
                        menus.back().menuItems.back().enabled = Deserializer::deserialize(ObjectConverter::Type::Bool, childNode->propertyValuePairs["itemsenabled"]->valueList[i]).getBool();
                #endif
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeSubMenus(std::vector<Menu>& menus, int& selectedMenu)
    {
        if (menus[selectedMenu].selectedMenuItem != -1)
            closeSubMenus(menus[selectedMenu].menuItems, menus[selectedMenu].selectedMenuItem);

        updateMenuTextColor(menus[selectedMenu], false);
        selectedMenu = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::deselectBottomItem()
    {
        auto* menu = &m_menus[m_visibleMenu];
        while (menu->selectedMenuItem != -1)
        {
            auto& menuItem = menu->menuItems[menu->selectedMenuItem];
            if (menuItem.menuItems.empty())
            {
                closeSubMenus(menu->menuItems, menu->selectedMenuItem);
                break;
            }

            menu = &menuItem;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateMenuTextColor(Menu& menu, bool selected)
    {
        if ((!m_enabled || !menu.enabled) && m_textColorDisabledCached.isSet())
            menu.text.setColor(m_textColorDisabledCached);
        else if (selected && m_selectedTextColorCached.isSet())
            menu.text.setColor(m_selectedTextColorCached);
        else
            menu.text.setColor(m_textColorCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextColors(std::vector<Menu>& menus, int selectedMenu)
    {
        for (std::size_t i = 0; i < menus.size(); ++i)
        {
            updateMenuTextColor(menus[i], (selectedMenu == static_cast<int>(i)));
            updateTextColors(menus[i].menuItems, menus[i].selectedMenuItem);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextOpacity(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setOpacity(m_opacityCached);
            updateTextOpacity(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::updateTextFont(std::vector<Menu>& menus)
    {
        for (auto& menu : menus)
        {
            menu.text.setFont(m_fontCached);
            updateTextFont(menu.menuItems);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::calculateMenuWidth(const Menu& menu) const
    {
        float maxWidth = m_minimumSubMenuWidth;
        for (const auto& item : menu.menuItems)
        {
            float width = item.text.getSize().x + (2.f * m_distanceToSideCached);

            // Reserve space for an arrow if there are submenus
            if (!item.menuItems.empty())
                width += (getSize().y / 4.f) + m_distanceToSideCached;

            if (width > maxWidth)
                maxWidth = width;
        }

        return maxWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f MenuBar::calculateSubmenuOffset(const Menu& menu, float globalLeftPos, float menuWidth, float subMenuWidth, bool& openSubMenuToRight) const
    {
        float leftOffset;
        if (openSubMenuToRight)
        {
            leftOffset = menuWidth;
            if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
            {
                if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                {
                    leftOffset = -subMenuWidth;
                    openSubMenuToRight = false;
                }
            }
        }
        else // Submenu opens to the left side
        {
            leftOffset = -subMenuWidth;
            if (getParent() && (globalLeftPos < subMenuWidth))
            {
                if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                {
                    leftOffset = menuWidth;
                    openSubMenuToRight = true;
                }
            }
        }

        float topOffset = getSize().y * menu.selectedMenuItem;
        if (m_invertedMenuDirection)
            topOffset -= getSize().y * (menu.menuItems[menu.selectedMenuItem].menuItems.size() - 1);

        return {leftOffset, topOffset};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::isMouseOnTopOfMenu(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, const Menu& menu, float menuWidth) const
    {
        // Check if the mouse is on top of the menu
        if (FloatRect{menuPos.x, menuPos.y, menuWidth, menu.menuItems.size() * getSize().y}.contains(mousePos))
            return true;

        // Check if the mouse is on one of the submenus
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (isMouseOnTopOfMenu(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[menu.selectedMenuItem], subMenuWidth))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::findMenuItemBelowMouse(Vector2f menuPos, Vector2f mousePos, bool openSubMenuToRight, Menu& menu, float menuWidth, Menu** resultMenu, int* resultSelectedMenuItem)
    {
        // Loop over the open submenus and make sure to handle them first as menus can overlap
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            const Vector2f offset = calculateSubmenuOffset(menu, menuPos.x, menuWidth, subMenuWidth, openSubMenuToRight);
            if (findMenuItemBelowMouse(menuPos + offset, mousePos, openSubMenuToRight, menu.menuItems[menu.selectedMenuItem], subMenuWidth, resultMenu, resultSelectedMenuItem))
                return true;
        }

        // Check if the mouse is on top of the menu
        if (FloatRect{menuPos.x, menuPos.y, menuWidth, menu.menuItems.size() * getSize().y}.contains(mousePos))
        {
            *resultMenu = &menu;
            *resultSelectedMenuItem = static_cast<int>((mousePos.y - menuPos.y) / getSize().y);
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawMenusOnBar(sf::RenderTarget& target, sf::RenderStates states, Sprite& backgroundSprite) const
    {
        sf::Transform oldTransform = states.transform;

        // Draw the backgrounds
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            const bool isMenuOpen = (m_visibleMenu == static_cast<int>(i));
            const float width = m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            if (backgroundSprite.isSet())
            {
                if (isMenuOpen && m_spriteSelectedItemBackground.isSet())
                {
                    Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                    selectedBackgroundSprite.setSize({width, getSize().y});
                    selectedBackgroundSprite.draw(target, states);
                }
                else // Not selected or no different texture for selected menu
                {
                    backgroundSprite.setSize({width, getSize().y});
                    backgroundSprite.draw(target, states);
                }
            }
            else // No textures where loaded
            {
                if (isMenuOpen && m_selectedBackgroundColorCached.isSet())
                    drawRectangleShape(target, states, {width, getSize().y}, m_selectedBackgroundColorCached);
            }

            states.transform.translate({width, 0});
        }

        states.transform = oldTransform;

        // Draw the texts
        const float textHeight = m_menus[0].text.getSize().y;
        states.transform.translate({m_distanceToSideCached, (getSize().y - textHeight) / 2.f});
        for (std::size_t i = 0; i < m_menus.size(); ++i)
        {
            m_menus[i].text.draw(target, states);

            const float width = m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
            states.transform.translate({width, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::drawMenu(sf::RenderTarget& target, sf::RenderStates states, const Menu& menu, float menuWidth, Sprite& backgroundSprite, float globalLeftPos, bool openSubMenuToRight) const
    {
        if (menu.menuItems.empty())
            return;

        sf::Transform oldTransform = states.transform;

        // Draw the backgrounds
        if ((menu.selectedMenuItem == -1) && !backgroundSprite.isSet() && !m_selectedBackgroundColorCached.isSet())
        {
            drawRectangleShape(target, states, {menuWidth, getSize().y * menu.menuItems.size()}, m_backgroundColorCached);
        }
        else // We can't draw the entire menu with a singe draw call
        {
            for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
            {
                const bool isMenuItemSelected = (menu.selectedMenuItem == static_cast<int>(j));
                if (backgroundSprite.isSet())
                {
                    if (isMenuItemSelected && m_spriteSelectedItemBackground.isSet())
                    {
                        Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                        selectedBackgroundSprite.setSize({menuWidth, getSize().y});
                        selectedBackgroundSprite.draw(target, states);
                    }
                    else // Not selected or no different texture for selected menu
                    {
                        backgroundSprite.setSize({menuWidth, getSize().y});
                        backgroundSprite.draw(target, states);
                    }
                }
                else // No textures where loaded
                {
                    if (isMenuItemSelected && m_selectedBackgroundColorCached.isSet())
                        drawRectangleShape(target, states, {menuWidth, getSize().y}, m_selectedBackgroundColorCached);
                    else
                        drawRectangleShape(target, states, {menuWidth, getSize().y}, m_backgroundColorCached);
                }

                states.transform.translate({0, getSize().y});
            }

            states.transform = oldTransform;
        }

        // Draw the texts (and arrows when there are submenus)
        states.transform.translate({m_distanceToSideCached, (getSize().y - menu.text.getSize().y) / 2.f});
        for (std::size_t j = 0; j < menu.menuItems.size(); ++j)
        {
            menu.menuItems[j].text.draw(target, states);

            // Draw an arrow next to the text if there is a submenu
            if (!menu.menuItems[j].menuItems.empty())
            {
                sf::Transform textTransform = states.transform;
                const float arrowHeight = getSize().y / 2.f;
                const float arrowWidth = arrowHeight / 2.f;
                states.transform.translate({menuWidth - 2*m_distanceToSideCached - arrowWidth, // 2x m_distanceToSideCached because we already translated once
                                            (menu.menuItems[j].text.getSize().y - arrowHeight) / 2.f});

                sf::ConvexShape arrow{3};
                arrow.setPoint(0, {0, 0});
                arrow.setPoint(1, {arrowWidth, arrowHeight / 2.f});
                arrow.setPoint(2, {0, arrowHeight});

                if ((!m_enabled || !menu.menuItems[j].enabled) && m_textColorDisabledCached.isSet())
                    arrow.setFillColor(Color::calcColorOpacity(m_textColorDisabledCached, m_opacityCached));
                else if ((menu.selectedMenuItem == static_cast<int>(j)) && m_selectedTextColorCached.isSet())
                    arrow.setFillColor(Color::calcColorOpacity(m_selectedTextColorCached, m_opacityCached));
                else
                    arrow.setFillColor(Color::calcColorOpacity(m_textColorCached, m_opacityCached));

                target.draw(arrow, states);
                states.transform = textTransform;
            }

            states.transform.translate({0, getSize().y});
        }

        // Draw the submenu if one is opened
        if ((menu.selectedMenuItem >= 0) && !menu.menuItems[menu.selectedMenuItem].menuItems.empty())
        {
            states.transform = oldTransform;

            // If there isn't enough room on the right side then open the menu to the left if it has more room
            const float subMenuWidth = calculateMenuWidth(menu.menuItems[menu.selectedMenuItem]);
            float leftOffset;
            if (openSubMenuToRight)
            {
                leftOffset = menuWidth;
                if (getParent() && (globalLeftPos + leftOffset + subMenuWidth > getParent()->getInnerSize().x))
                {
                    if (globalLeftPos + leftOffset + subMenuWidth - getParent()->getInnerSize().x < globalLeftPos)
                    {
                        leftOffset = -subMenuWidth;
                        openSubMenuToRight = false;
                    }
                }
            }
            else // Submenu opens to the left side
            {
                leftOffset = -subMenuWidth;
                if (getParent() && (globalLeftPos < subMenuWidth))
                {
                    if (getParent()->getInnerSize().x - menuWidth - globalLeftPos > globalLeftPos)
                    {
                        leftOffset = menuWidth;
                        openSubMenuToRight = true;
                    }
                }
            }

            float topOffset = getSize().y * menu.selectedMenuItem;
            if (m_invertedMenuDirection)
                topOffset -= getSize().y * (menu.menuItems[menu.selectedMenuItem].menuItems.size() - 1);

            states.transform.translate({leftOffset, topOffset});
            drawMenu(target, states, menu.menuItems[menu.selectedMenuItem], subMenuWidth, backgroundSprite, globalLeftPos + leftOffset, openSubMenuToRight);
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


#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Layout2d& size)
    {
        m_type = "Panel";
        m_isolatedFocus = true;

        m_renderer = aurora::makeCopied<PanelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::create(Layout2d size)
    {
        return std::make_shared<Panel>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::copy(Panel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<Panel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getSharedRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelRenderer* Panel::getSharedRenderer() const
    {
        return aurora::downcast<const PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelRenderer* Panel::getRenderer() const
    {
        return aurora::downcast<const PanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(const Layout2d& size)
    {
        m_bordersCached.updateParentSize(size.getValue());

        Group::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_paddingCached.getTop() - m_paddingCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                m_paddingCached.getTop() + m_bordersCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        onMousePress.emit(this, pos - getPosition());

        Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(Vector2f pos)
    {
        onMouseRelease.emit(this, pos - getPosition());

        if (m_mouseDown)
            onClick.emit(this, pos - getPosition());

        m_mouseDown = false;

        Container::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Panel::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onClick.getName()))
            return onClick;
        else
            return Group::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
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
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        const Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                      innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        // Draw the child widgets
        const Clipping clipping{target, states, {}, contentSize};
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


#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::ProgressBar()
    {
        m_type = "ProgressBar";
        m_textBack.setFont(m_fontCached);
        m_textFront.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<ProgressBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_textBack) * 15,
                 Text::getLineHeight(m_textBack) * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom()});

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::create()
    {
        return std::make_shared<ProgressBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::Ptr ProgressBar::copy(ProgressBar::ConstPtr progressBar)
    {
        if (progressBar)
            return std::static_pointer_cast<ProgressBar>(progressBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getSharedRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ProgressBarRenderer* ProgressBar::getSharedRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBarRenderer* ProgressBar::getRenderer()
    {
        return aurora::downcast<ProgressBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ProgressBarRenderer* ProgressBar::getRenderer() const
    {
        return aurora::downcast<const ProgressBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Recalculate the size of the front image
        recalculateFillSize();

        // Recalculate the text size
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            m_minimum = m_maximum;

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setValue(unsigned int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            onValueChange.emit(this, m_value);

            if (m_value == m_maximum)
                onFull.emit(this);

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateFillSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_value < m_maximum)
            setValue(m_value + 1);

        // return the new value
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setText(const sf::String& text)
    {
        // Set the new text
        m_textBack.setString(text);
        m_textFront.setString(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            unsigned int textSize;
            if (m_spriteFill.isSet())
                textSize = Text::findBestTextSize(m_fontCached, m_spriteFill.getSize().y * 0.8f);
            else
                textSize = Text::findBestTextSize(m_fontCached, getInnerSize().y * 0.8f);

            m_textBack.setCharacterSize(textSize);

            // Make the text smaller when it's too width
            if (m_textBack.getSize().x > (getInnerSize().x * 0.85f))
                m_textBack.setCharacterSize(static_cast<unsigned int>(textSize * ((getInnerSize().x * 0.85f) / m_textBack.getSize().x)));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_textBack.setCharacterSize(m_textSize);
        }

        m_textFront.setCharacterSize(m_textBack.getCharacterSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& ProgressBar::getText() const
    {
        return m_textBack.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ProgressBar::getTextSize() const
    {
        return m_textBack.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::setFillDirection(FillDirection direction)
    {
        m_fillDirection = direction;
        recalculateFillSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ProgressBar::FillDirection ProgressBar::getFillDirection() const
    {
        return m_fillDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ProgressBar::mouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::mouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ProgressBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else if (signalName == toLower(onFull.getName()))
            return onFull;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if ((property == "textcolor") || (property == "textcolorfilled"))
        {
            m_textBack.setColor(getSharedRenderer()->getTextColor());

            if (getSharedRenderer()->getTextColorFilled().isSet())
                m_textFront.setColor(getSharedRenderer()->getTextColorFilled());
            else
                m_textFront.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "texturefill")
        {
            m_spriteFill.setTexture(getSharedRenderer()->getTextureFill());
            recalculateFillSize();
        }
        else if (property == "textstyle")
        {
            m_textBack.setStyle(getSharedRenderer()->getTextStyle());
            m_textFront.setStyle(getSharedRenderer()->getTextStyle());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "fillcolor")
        {
            m_fillColorCached = getSharedRenderer()->getFillColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_spriteFill.setOpacity(m_opacityCached);

            m_textBack.setOpacity(m_opacityCached);
            m_textFront.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBack.setFont(m_fontCached);
            m_textFront.setFont(m_fontCached);
            setText(getText());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ProgressBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));

        if (m_fillDirection != ProgressBar::FillDirection::LeftToRight)
        {
            if (m_fillDirection == ProgressBar::FillDirection::RightToLeft)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("RightToLeft");
            else if (m_fillDirection == ProgressBar::FillDirection::TopToBottom)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("TopToBottom");
            else if (m_fillDirection == ProgressBar::FillDirection::BottomToTop)
                node->propertyValuePairs["FillDirection"] = std::make_unique<DataIO::ValueNode>("BottomToTop");
        }

        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["minimum"])
            setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

        if (node->propertyValuePairs["filldirection"])
        {
            std::string requestedStyle = toLower(trim(node->propertyValuePairs["filldirection"]->value));
            if (requestedStyle == "lefttoright")
                setFillDirection(ProgressBar::FillDirection::LeftToRight);
            else if (requestedStyle == "righttoleft")
                setFillDirection(ProgressBar::FillDirection::RightToLeft);
            else if (requestedStyle == "toptobottom")
                setFillDirection(ProgressBar::FillDirection::TopToBottom);
            else if (requestedStyle == "bottomtotop")
                setFillDirection(ProgressBar::FillDirection::BottomToTop);
            else
                throw Exception{"Failed to parse FillDirection property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::recalculateFillSize()
    {
        Vector2f size;
        if (m_spriteFill.isSet())
        {
            const Vector2f frontSize = getFrontImageSize();
            m_spriteFill.setSize(frontSize);
            size = frontSize;
        }
        else
            size = getInnerSize();

        switch (getFillDirection())
        {
            case FillDirection::LeftToRight:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_backRect = {m_frontRect.width, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::RightToLeft:
                m_frontRect =  {0, 0, size.x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), size.y};
                m_frontRect.left = size.x - m_frontRect.width;
                m_backRect = {0, 0, size.x - m_frontRect.width, size.y};
                break;

            case FillDirection::TopToBottom:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_backRect = {0, m_frontRect.height, size.x, size.y - m_frontRect.height};
                break;

            case FillDirection::BottomToTop:
                m_frontRect =  {0, 0, size.x, size.y * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum))};
                m_frontRect.top = size.y - m_frontRect.height;
                m_backRect = {0, 0, size.x, size.y - m_frontRect.height};
                break;
        }

        if (m_spriteFill.isSet())
            m_spriteFill.setVisibleRect(m_frontRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ProgressBar::getFrontImageSize() const
    {
        if (m_spriteBackground.isSet())
        {
            switch (m_spriteBackground.getScalingType())
            {
            case Sprite::ScalingType::Normal:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Horizontal:
                return {getInnerSize().x - ((m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x) * (getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y)),
                        m_spriteFill.getTexture().getImageSize().y * getInnerSize().y / m_spriteBackground.getTexture().getImageSize().y};

            case Sprite::ScalingType::Vertical:
                return {m_spriteFill.getTexture().getImageSize().x * getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x,
                        getInnerSize().y - ((m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y) * (getInnerSize().x / m_spriteBackground.getTexture().getImageSize().x))};

            case Sprite::ScalingType::NineSlice:
                return {getInnerSize().x - (m_spriteBackground.getTexture().getImageSize().x - m_spriteFill.getTexture().getImageSize().x),
                        getInnerSize().y - (m_spriteBackground.getTexture().getImageSize().y - m_spriteFill.getTexture().getImageSize().y)};
            }
        }

        return getInnerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

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
        {
            Vector2f positionOffset = {m_backRect.left, m_backRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_backRect.width, m_backRect.height}, m_backgroundColorCached);
            states.transform.translate(-positionOffset);
        }

        // Draw the filled area
        Vector2f imageShift;
        if (m_spriteFill.isSet())
        {
            if (m_spriteBackground.isSet() && (m_spriteBackground.getSize() != m_spriteFill.getSize()))
            {
                imageShift = (m_spriteBackground.getSize() - m_spriteFill.getSize()) / 2.f;

                states.transform.translate(imageShift);
                m_spriteFill.draw(target, states);
                states.transform.translate(-imageShift);
            }
            else
                m_spriteFill.draw(target, states);
        }
        else // Using colors instead of a texture
        {
            Vector2f positionOffset = {m_frontRect.left, m_frontRect.top};

            states.transform.translate(positionOffset);
            drawRectangleShape(target, states, {m_frontRect.width, m_frontRect.height}, m_fillColorCached);
            states.transform.translate(-positionOffset);
        }

        // Draw the text
        if (m_textBack.getString() != "")
        {
            Vector2f textTranslation = (getInnerSize() - m_textBack.getSize()) / 2.f;

            if (m_textBack.getColor() == m_textFront.getColor())
            {
                states.transform.translate(textTranslation);
                m_textBack.draw(target, states);
                states.transform.translate(-textTranslation);
            }
            else
            {
                // Draw the text on top of the unfilled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_backRect.left, m_backRect.top}, {m_backRect.width, m_backRect.height}};

                    states.transform.translate(textTranslation);
                    m_textBack.draw(target, states);
                    states.transform.translate(-textTranslation);
                }

                // Draw the text on top of the filled part
                {
                    Clipping clipping{target, states, imageShift + Vector2f{m_frontRect.left, m_frontRect.top}, {m_frontRect.width, m_frontRect.height}};

                    states.transform.translate(textTranslation);
                    m_textFront.draw(target, states);
                    states.transform.translate(-textTranslation);
                }
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


#include <TGUI/Widgets/RadioButtonGroup.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::RadioButtonGroup()
    {
        m_type = "RadioButtonGroup";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::create()
    {
        return std::make_shared<RadioButtonGroup>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RadioButtonGroup::Ptr RadioButtonGroup::copy(RadioButtonGroup::ConstPtr group)
    {
        if (group)
            return std::static_pointer_cast<RadioButtonGroup>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void RadioButtonGroup::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
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


#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Vector2f.hpp>
#include <TGUI/Clipping.hpp>
#include <TGUI/SignalImpl.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const Layout2d& size, Vector2f contentSize) :
        Panel{size}
    {
        m_type = "ScrollablePanel";

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        m_renderer = aurora::makeCopied<ScrollablePanelRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setContentSize(contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const ScrollablePanel& other) :
        Panel                      {other},
        m_contentSize              {other.m_contentSize},
        m_mostBottomRightPosition  {other.m_mostBottomRightPosition},
        m_verticalScrollbar        {other.m_verticalScrollbar},
        m_horizontalScrollbar      {other.m_horizontalScrollbar},
        m_verticalScrollbarPolicy  {other.m_verticalScrollbarPolicy},
        m_horizontalScrollbarPolicy{other.m_horizontalScrollbarPolicy},
        m_connectedCallbacks       {}
    {
        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(ScrollablePanel&& other) :
        Panel                      {std::move(other)},
        m_contentSize              {std::move(other.m_contentSize)},
        m_mostBottomRightPosition  {std::move(other.m_mostBottomRightPosition)},
        m_verticalScrollbar        {std::move(other.m_verticalScrollbar)},
        m_horizontalScrollbar      {std::move(other.m_horizontalScrollbar)},
        m_verticalScrollbarPolicy  {std::move(other.m_verticalScrollbarPolicy)},
        m_horizontalScrollbarPolicy{std::move(other.m_horizontalScrollbarPolicy)},
        m_connectedCallbacks       {std::move(other.m_connectedCallbacks)}
    {
        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (const ScrollablePanel& other)
    {
        if (this != &other)
        {
            Panel::operator=(other);
            m_contentSize               = other.m_contentSize;
            m_mostBottomRightPosition   = other.m_mostBottomRightPosition;
            m_verticalScrollbar         = other.m_verticalScrollbar;
            m_horizontalScrollbar       = other.m_horizontalScrollbar;
            m_verticalScrollbarPolicy   = other.m_verticalScrollbarPolicy;
            m_horizontalScrollbarPolicy = other.m_horizontalScrollbarPolicy;

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel& ScrollablePanel::operator= (ScrollablePanel&& other)
    {
        if (this != &other)
        {
            Panel::operator=(std::move(other));
            m_contentSize               = std::move(other.m_contentSize);
            m_mostBottomRightPosition   = std::move(other.m_mostBottomRightPosition);
            m_verticalScrollbar         = std::move(other.m_verticalScrollbar);
            m_horizontalScrollbar       = std::move(other.m_horizontalScrollbar);
            m_verticalScrollbarPolicy   = std::move(other.m_verticalScrollbarPolicy);
            m_horizontalScrollbarPolicy = std::move(other.m_horizontalScrollbarPolicy);

            disconnectAllChildWidgets();

            if (m_contentSize == Vector2f{0, 0})
            {
                for (auto& widget : m_widgets)
                    connectPositionAndSize(widget);
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::create(Layout2d size, Vector2f contentSize)
    {
        return std::make_shared<ScrollablePanel>(size, contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::copy(ScrollablePanel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<ScrollablePanel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getSharedRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanelRenderer* ScrollablePanel::getRenderer()
    {
        return aurora::downcast<ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ScrollablePanelRenderer* ScrollablePanel::getRenderer() const
    {
        return aurora::downcast<const ScrollablePanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getAbsolutePosition() const
    {
        return Panel::getAbsolutePosition() - getContentOffset();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::add(const Widget::Ptr& widget, const sf::String& widgetName)
    {
        Panel::add(widget, widgetName);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;

            updateScrollbars();

            connectPositionAndSize(widget);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::remove(const Widget::Ptr& widget)
    {
        const auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
        {
            widget->disconnect(callbackIt->second-1);
            widget->disconnect(callbackIt->second);
            m_connectedCallbacks.erase(callbackIt);
        }

        const bool ret = Panel::remove(widget);

        if (m_contentSize == Vector2f{0, 0})
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if ((bottomRight.x == m_mostBottomRightPosition.x) || (bottomRight.y == m_mostBottomRightPosition.y))
            {
                recalculateMostBottomRightPosition();
                updateScrollbars();
            }
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::removeAllWidgets()
    {
        disconnectAllChildWidgets();

        Panel::removeAllWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();
            updateScrollbars();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setContentSize(Vector2f size)
    {
        m_contentSize = size;

        disconnectAllChildWidgets();

        if (m_contentSize == Vector2f{0, 0})
        {
            recalculateMostBottomRightPosition();

            // Automatically recalculate the bottom right position when the position or size of a widget changes
            for (auto& widget : m_widgets)
                connectPositionAndSize(widget);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentSize() const
    {
        if (m_contentSize != Vector2f{0, 0})
            return m_contentSize;
        else if (m_widgets.empty())
            return getInnerSize();
        else
            return m_mostBottomRightPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ScrollablePanel::getContentOffset() const
    {
        return {static_cast<float>(m_horizontalScrollbar->getValue()), static_cast<float>(m_verticalScrollbar->getValue())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setScrollbarWidth(float width)
    {
        m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
        m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ScrollablePanel::getScrollbarWidth() const
    {
        return m_verticalScrollbar->getSize().x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setVerticalScrollbarPolicy(ScrollbarPolicy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == ScrollbarPolicy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == ScrollbarPolicy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // ScrollbarPolicy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollbarPolicy ScrollablePanel::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setHorizontalScrollbarPolicy(ScrollbarPolicy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == ScrollbarPolicy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == ScrollbarPolicy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // ScrollbarPolicy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollbarPolicy ScrollablePanel::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMousePressed(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMousePressed(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMousePressed({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                     pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseReleased(Vector2f pos)
    {
        if (m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar->leftMouseReleased(pos - getPosition());
        else if (m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        else if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMouseReleased({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                      pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseMoved(Vector2f pos)
    {
        // Check if the mouse event should go to the scrollbar
        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_verticalScrollbar->mouseMoved(pos - getPosition());
        }
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseMoved(pos - getPosition());
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
            {
                Panel::mouseMoved({pos.x + static_cast<float>(m_horizontalScrollbar->getValue()),
                                   pos.y + static_cast<float>(m_verticalScrollbar->getValue())});
            }

            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::mouseWheelScrolled(float delta, Vector2f pos)
    {
        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));

        sf::Vector2f innerSize = getInnerSize();
        if (verticalScrollbarVisible)
            innerSize.x -= m_verticalScrollbar->getSize().x;
        if (horizontalScrollbarVisible)
            innerSize.y -= m_horizontalScrollbar->getSize().y;

        if (FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, innerSize.x, innerSize.y}.contains(pos))
        {
            if (Container::mouseWheelScrolled(delta, pos + getContentOffset()))
                return true; // A child widget swallowed the event
        }

        if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }

        return true; // We swallowed the event
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerOnWidget()
    {
        Panel::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerDown()
    {
        Panel::mouseNoLongerDown();
        m_verticalScrollbar->mouseNoLongerDown();
        m_horizontalScrollbar->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ScrollablePanel::askToolTip(Vector2f mousePos)
    {
        if (mouseOnWidget(mousePos))
        {
            Widget::Ptr toolTip = nullptr;

            mousePos -= getPosition() + getChildWidgetsOffset() - getContentOffset();

            Widget::Ptr widget = mouseOnWhichWidget(mousePos);
            if (widget)
            {
                toolTip = widget->askToolTip(mousePos);
                if (toolTip)
                    return toolTip;
            }

            if (m_toolTip)
                return getToolTip();
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const auto oldStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        if (m_verticalScrollbar->isVisible() && (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()))
            contentSize.x -= m_verticalScrollbar->getSize().x;
        if (m_horizontalScrollbar->isVisible() && (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()))
            contentSize.y -= m_horizontalScrollbar->getSize().y;

        // If the content size is manually specified and smaller than the panel itself, then use it for clipping
        if ((m_contentSize.x > 0) && (contentSize.x > m_contentSize.x))
            contentSize.x = m_contentSize.x;
        if ((m_contentSize.y > 0) && (contentSize.y > m_contentSize.y))
            contentSize.y = m_contentSize.y;

        // Draw the child widgets
        {
            const Clipping clipping{target, states, {}, contentSize};

            states.transform.translate(-static_cast<float>(m_horizontalScrollbar->getValue()),
                                       -static_cast<float>(m_verticalScrollbar->getValue()));

            drawWidgetContainer(&target, states);
        }

        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, oldStates);

        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, oldStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::rendererChanged(const std::string& property)
    {
        if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            updateScrollbars();
        }
        else
            Panel::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ScrollablePanel::save(SavingRenderersMap& renderers) const
    {
        auto node = Panel::save(renderers);

        if (m_verticalScrollbarPolicy != ScrollbarPolicy::Automatic)
        {
            if (m_verticalScrollbarPolicy == ScrollbarPolicy::Always)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == ScrollbarPolicy::Never)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != ScrollbarPolicy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == ScrollbarPolicy::Always)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == ScrollbarPolicy::Never)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs["ContentSize"] = std::make_unique<DataIO::ValueNode>("(" + to_string(m_contentSize.x) + ", " + to_string(m_contentSize.y) + ")");
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Panel::load(node, renderers);

        if (node->propertyValuePairs["contentsize"])
            setContentSize(Vector2f{node->propertyValuePairs["contentsize"]->value});

        if (node->propertyValuePairs["verticalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["verticalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Automatic);
            else if (policy == "always")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Always);
            else if (policy == "never")
                setVerticalScrollbarPolicy(ScrollbarPolicy::Never);
            else
                throw Exception{"Failed to parse VerticalScrollbarPolicy property, found unknown value."};
        }

        if (node->propertyValuePairs["horizontalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["horizontalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Automatic);
            else if (policy == "always")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Always);
            else if (policy == "never")
                setHorizontalScrollbarPolicy(ScrollbarPolicy::Never);
            else
                throw Exception{"Failed to parse HorizontalScrollbarPolicy property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::updateScrollbars()
    {
        const Vector2f scrollbarSpace = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                         getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        const Vector2f visibleSize = getInnerSize();
        m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.x));
        m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(visibleSize.y));

        const Vector2f contentSize = getContentSize();
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.x));
        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(contentSize.y));

        const bool horizontalScrollbarVisible = m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize()));
        if (horizontalScrollbarVisible)
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(m_verticalScrollbar->getViewportSize() - m_horizontalScrollbar->getSize().y));

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
            }
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }
        else
        {
            m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y);

            const bool verticalScrollbarVisible = m_verticalScrollbar->isVisible() && (!m_verticalScrollbar->getAutoHide() || (m_verticalScrollbar->getMaximum() > m_verticalScrollbar->getViewportSize()));
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar->setSize(scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y);
                m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(m_horizontalScrollbar->getViewportSize() - m_verticalScrollbar->getSize().x));

                if (m_horizontalScrollbar->isVisible() && (!m_horizontalScrollbar->getAutoHide() || (m_horizontalScrollbar->getMaximum() > m_horizontalScrollbar->getViewportSize())))
                    m_verticalScrollbar->setSize(m_verticalScrollbar->getSize().x, scrollbarSpace.y - m_horizontalScrollbar->getSize().y);
            }
            else
                m_horizontalScrollbar->setSize(scrollbarSpace.x, m_horizontalScrollbar->getSize().y);
        }

        m_verticalScrollbar->setPosition(m_bordersCached.getLeft() + scrollbarSpace.x - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), m_bordersCached.getTop() + scrollbarSpace.y - m_horizontalScrollbar->getSize().y);

        const float verticalSpeed = 40.f * (static_cast<float>(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize()) / m_verticalScrollbar->getViewportSize());
        m_verticalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(verticalSpeed))));

        const float horizontalSpeed = 40.f * (static_cast<float>(m_horizontalScrollbar->getMaximum() - m_horizontalScrollbar->getViewportSize()) / m_horizontalScrollbar->getViewportSize());
        m_horizontalScrollbar->setScrollAmount(static_cast<unsigned int>(std::ceil(std::sqrt(horizontalSpeed))));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::recalculateMostBottomRightPosition()
    {
        m_mostBottomRightPosition = {0, 0};

        for (const auto& widget : m_widgets)
        {
            const Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::connectPositionAndSize(Widget::Ptr widget)
    {
        m_connectedCallbacks[widget] = widget->connect({"PositionChanged", "SizeChanged"}, [this](){ recalculateMostBottomRightPosition(); updateScrollbars(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::disconnectAllChildWidgets()
    {
        for (const auto& pair : m_connectedCallbacks)
        {
            pair.first->disconnect(pair.second-1);
            pair.first->disconnect(pair.second);
        }

        m_connectedCallbacks.clear();
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


#include <TGUI/Widgets/Slider.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Slider()
    {
        m_type = "Slider";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<SliderRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setSize(200, 16);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::create(float minimum, float maximum)
    {
        auto slider = std::make_shared<Slider>();

        slider->setMinimum(minimum);
        slider->setMaximum(maximum);

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Slider::Ptr Slider::copy(Slider::ConstPtr slider)
    {
        if (slider)
            return std::static_pointer_cast<Slider>(slider->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getSharedRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* Slider::getSharedRenderer() const
    {
        return aurora::downcast<const SliderRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SliderRenderer* Slider::getRenderer()
    {
        return aurora::downcast<SliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SliderRenderer* Slider::getRenderer() const
    {
        return aurora::downcast<const SliderRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setSize(const Layout2d& size)
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

            m_thumb.width = scaleFactor * m_spriteThumb.getTexture().getImageSize().x;
            m_thumb.height = scaleFactor * m_spriteThumb.getTexture().getImageSize().y;

            m_spriteThumb.setSize({m_thumb.width, m_thumb.height});
            m_spriteThumbHover.setSize({m_thumb.width, m_thumb.height});

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
                m_thumb.width = getSize().x * 1.6f;
                m_thumb.height = m_thumb.width / 2.0f;
            }
            else
            {
                m_thumb.height = getSize().y * 1.6f;
                m_thumb.width = m_thumb.height / 2.0f;
            }
        }

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getFullSize() const
    {
        if (m_verticalScroll)
            return {std::max(getSize().x, m_thumb.width), getSize().y + m_thumb.height};
        else
            return {getSize().x + m_thumb.width, std::max(getSize().y, m_thumb.height)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getWidgetOffset() const
    {
        if (m_verticalScroll)
            return {std::min(0.f, (getSize().x - m_thumb.width) / 2.f), -m_thumb.height / 2.f};
        else
            return {-m_thumb.width / 2.f, std::min(0.f, (getSize().y - m_thumb.height) / 2.f)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMinimum(float minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The maximum can't be below the minimum
        if (m_maximum < m_minimum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMinimum() const
    {
        return m_minimum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setMaximum(float maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The minimum can't be below the maximum
        if (m_minimum > m_maximum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);

        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getMaximum() const
    {
        return m_maximum;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setValue(float value)
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

            onValueChange.emit(this, m_value);

            updateThumbPosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getValue() const
    {
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setStep(float step)
    {
        m_step = step;

        // Reset the value in case it does not match the step
        setValue(m_value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Slider::getStep() const
    {
        return m_step;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::setInvertedDirection(bool invertedDirection)
    {
        m_invertedDirection = invertedDirection;
        updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::getInvertedDirection() const
    {
        return m_invertedDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the thumb
        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
            return true;

        // Check if the mouse is on top of the track
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMousePressed(Vector2f pos)
    {
        m_mouseDown = true;

        if (FloatRect(m_thumb.left, m_thumb.top, m_thumb.width, m_thumb.height).contains(pos))
        {
            m_mouseDownOnThumb = true;
            m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
            m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
        }
        else // The mouse is not on top of the thumb
            m_mouseDownOnThumb = false;

        // Refresh the value
        mouseMoved(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::leftMouseReleased(Vector2f)
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if (!m_mouseDown)
            return;

        // Check in which direction the slider goes
        if (m_verticalScroll)
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = pos.x - m_thumb.left;
                m_mouseDownOnThumbPos.y = m_thumb.height / 2.0f;
            }

            float value = m_maximum - (((pos.y + (m_thumb.height / 2.0f) - m_mouseDownOnThumbPos.y) / getSize().y) * (m_maximum - m_minimum));
            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbTop = pos.y - m_mouseDownOnThumbPos.y;
            if ((thumbTop + (m_thumb.height / 2.0f) > 0) && (thumbTop + (m_thumb.height / 2.0f) < getSize().y))
                m_thumb.top = thumbTop;
            else
            {
                m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_value)) - (m_thumb.height / 2.0f);
                if (m_invertedDirection)
                    m_thumb.top = getSize().y - m_thumb.top - m_thumb.height;
            }
        }
        else // the slider lies horizontal
        {
            // Check if the click occurred on the track
            if (!m_mouseDownOnThumb)
            {
                m_mouseDownOnThumb = true;
                m_mouseDownOnThumbPos.x = m_thumb.width / 2.0f;
                m_mouseDownOnThumbPos.y = pos.y - m_thumb.top;
            }

            float value = (((pos.x + (m_thumb.width / 2.0f) - m_mouseDownOnThumbPos.x) / getSize().x) * (m_maximum - m_minimum)) + m_minimum;
            if (m_invertedDirection)
                value = m_maximum - (value - m_minimum);

            setValue(value);

            // Set the thumb position for smooth scrolling
            const float thumbLeft = pos.x - m_mouseDownOnThumbPos.x;
            if ((thumbLeft + (m_thumb.width / 2.0f) > 0) && (thumbLeft + (m_thumb.width / 2.0f) < getSize().x))
                m_thumb.left = thumbLeft;
            else
            {
                m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);
                if (m_invertedDirection)
                    m_thumb.left = getSize().x - m_thumb.left - m_thumb.width;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Slider::mouseWheelScrolled(float delta, Vector2f)
    {
        if (m_invertedDirection)
            delta = -delta;

        if (m_step == 0)
            setValue(m_value + delta);
        else
        {
            if (std::abs(delta) <= 1)
            {
                if (delta > 0)
                    setValue(m_value + m_step);
                else if (delta < 0)
                    setValue(m_value - m_step);
            }
            else
                setValue(m_value + std::round(delta) * m_step);
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::mouseNoLongerDown()
    {
        // The thumb might have been dragged between two values
        if (m_mouseDown)
            updateThumbPosition();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Slider::getSignal(std::string signalName)
    {
        if (signalName == toLower(onValueChange.getName()))
            return onValueChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::rendererChanged(const std::string& property)
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

    std::unique_ptr<DataIO::Node> Slider::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs["Minimum"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimum));
        node->propertyValuePairs["Maximum"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximum));
        node->propertyValuePairs["Value"] = std::make_unique<DataIO::ValueNode>(to_string(m_value));
        node->propertyValuePairs["Step"] = std::make_unique<DataIO::ValueNode>(to_string(m_step));
        node->propertyValuePairs["InvertedDirection"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_invertedDirection));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
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
        if (node->propertyValuePairs["inverteddirection"])
            setInvertedDirection(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["inverteddirection"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Slider::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::updateThumbPosition()
    {
        if (m_verticalScroll)
        {
            m_thumb.left = m_bordersCached.getLeft() + (getInnerSize().x - m_thumb.width) / 2.0f;
            m_thumb.top = (getSize().y / (m_maximum - m_minimum) * (m_maximum - m_value)) - (m_thumb.height / 2.0f);

            if (m_invertedDirection)
                m_thumb.top = getSize().y - m_thumb.top - m_thumb.height;
        }
        else
        {
            m_thumb.left = (getSize().x / (m_maximum - m_minimum) * (m_value - m_minimum)) - (m_thumb.width / 2.0f);
            m_thumb.top = m_bordersCached.getTop() + (getInnerSize().y - m_thumb.height) / 2.0f;

            if (m_invertedDirection)
                m_thumb.left = getSize().x - m_thumb.left - m_thumb.width;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw the borders around the track
        if (m_bordersCached != Borders{0})
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
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
        }

        states.transform.translate({-m_bordersCached.getLeft() + m_thumb.left, -m_bordersCached.getTop() + m_thumb.top});

        // Draw the borders around the thumb when using colors
        if ((m_bordersCached != Borders{0}) && !(m_spriteTrack.isSet() && m_spriteThumb.isSet()))
        {
            if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, {m_thumb.width, m_thumb.height}, m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, {m_thumb.width, m_thumb.height}, m_borderColorCached);

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
            const Vector2f thumbInnerSize = {m_thumb.width - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                             m_thumb.height - m_bordersCached.getTop() - m_bordersCached.getBottom()};

            if (m_mouseHover && m_thumbColorHoverCached.isSet())
                drawRectangleShape(target, states, thumbInnerSize, m_thumbColorHoverCached);
            else
                drawRectangleShape(target, states, thumbInnerSize, m_thumbColorCached);
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


#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Clipping.hpp>

#ifdef TGUI_USE_CPP17
    #include <optional>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs()
    {
        m_type = "Tabs";
        m_distanceToSideCached = Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f;

        m_renderer = aurora::makeCopied<TabsRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setTabHeight(Text::getLineHeight(m_fontCached, m_textSize) * 1.25f + m_bordersCached.getTop() + m_bordersCached.getBottom());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::create()
    {
        return std::make_shared<Tabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::copy(Tabs::ConstPtr tabs)
    {
        if (tabs)
            return std::static_pointer_cast<Tabs>(tabs->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getSharedRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getSharedRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Tabs is no longer auto-sizing
        m_autoSize = false;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);

        if (!enabled)
            deselect();

        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabs.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabs.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::insert(std::size_t index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabs.size())
            index = m_tabs.size();

        // Create the new tab
        Tab newTab;
        newTab.visible = true;
        newTab.enabled = true;
        newTab.width = 0;
        newTab.text.setFont(m_fontCached);
        newTab.text.setColor(m_textColorCached);
        newTab.text.setOpacity(m_opacityCached);
        newTab.text.setCharacterSize(getTextSize());
        newTab.text.setString(text);

        m_tabs.insert(m_tabs.begin() + index, std::move(newTab));
        recalculateTabsWidth();

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getText(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return "";
        else
            return m_tabs[index].text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::changeText(std::size_t index, const sf::String& text)
    {
        if (index >= m_tabs.size())
            return false;

        m_tabs[index].text.setString(text);
        recalculateTabsWidth();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::select(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return select(i);
        }

        deselect();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::select(std::size_t index)
    {
        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return true;

        // If the index is too high or if the tab is invisible or disabled then we can't select it
        if ((index >= m_tabs.size()) || !m_enabled || !m_tabs[index].visible || !m_tabs[index].enabled)
        {
            deselect();
            return false;
        }

        if (m_selectedTab >= 0)
            m_tabs[m_selectedTab].text.setColor(m_textColorCached);

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabs[m_selectedTab].text.setColor(m_selectedTextColorCached);

        // Send the callback
        onTabSelect.emit(this, m_tabs[index].text.getString());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::deselect()
    {
        if (m_selectedTab >= 0)
        {
            m_tabs[m_selectedTab].text.setColor(m_textColorCached);
            m_selectedTab = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabs.size() - 1)
            return false;

        // Remove the tab
        m_tabs.erase(m_tabs.begin() + index);

        // Check if the selected tab should be updated
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::removeAll()
    {
        m_tabs.clear();
        m_selectedTab = -1;
        m_hoveringTab = -1;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabs[m_selectedTab].text.getString();
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tabs::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabVisible(std::size_t index, bool visible)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].visible = visible;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabVisible(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabEnabled(std::size_t index, bool enabled)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].enabled = enabled;
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabEnabled(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTextSize(unsigned int size)
    {
        if ((size == 0) || (m_requestedTextSize != size))
        {
            m_requestedTextSize = size;

            if (size == 0)
                m_textSize = Text::findBestTextSize(m_fontCached, (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()) * 0.8f);
            else
                m_textSize = size;

            for (auto& tab : m_tabs)
                tab.text.setCharacterSize(m_textSize);

            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tabs::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        Widget::setSize({getSizeLayout().x, height});
        m_bordersCached.updateParentSize(getSize());

        // Recalculate the size when the text is auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMaximumTabWidth() const
    {
        return m_maximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMinimumTabWidth(float minimumWidth)
    {
        m_minimumTabWidth = minimumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMinimumTabWidth() const
    {
        return m_minimumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::getTabsCount() const
    {
        return m_tabs.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        float width = m_bordersCached.getLeft() / 2.f;
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.x < width)
            {
                select(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);

        pos -= getPosition();
        m_hoveringTab = -1;
        float width = m_bordersCached.getLeft() / 2.f;

        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse is on top of this tab then remember it
            if (pos.x < width)
            {
                if (m_tabs[i].enabled)
                    m_hoveringTab = i;

                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();

        m_hoveringTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::recalculateTabsWidth()
    {
        unsigned int visibleTabs = 0;
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].visible)
                visibleTabs++;
        }

        if (m_autoSize)
        {
            if (m_tabs.empty())
                Widget::setSize({m_bordersCached.getLeft() + m_bordersCached.getRight(), getSizeLayout().y});
            else
            {
                // First calculate the width of the tabs as if there aren't any borders
                float totalWidth = 0;
                for (unsigned int i = 0; i < m_tabs.size(); ++i)
                {
                    if (!m_tabs[i].visible)
                        continue;

                    m_tabs[i].width = m_tabs[i].text.getSize().x + std::max(m_minimumTabWidth, 2 * m_distanceToSideCached);
                    if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < m_tabs[i].width))
                        m_tabs[i].width = m_maximumTabWidth;

                    totalWidth += m_tabs[i].width;
                }

                // Now add the borders to the tabs
                totalWidth += (visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f);

                Widget::setSize({totalWidth, getSizeLayout().y});
            }
        }
        else // A size was provided
        {
            const float tabWidth = (getSize().x - ((visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f))) / visibleTabs;
            for (unsigned int i = 0; i < m_tabs.size(); ++i)
                m_tabs[i].width = tabWidth;
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Tabs::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTabSelect.getName()))
            return onTabSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            recalculateTabsWidth();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors();
        }
        else if (property == "textcolordisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors();
        }
        else if (property == "texturetab")
        {
            m_spriteTab.setTexture(getSharedRenderer()->getTextureTab());
        }
        else if (property == "texturetabhover")
        {
            m_spriteTabHover.setTexture(getSharedRenderer()->getTextureTabHover());
        }
        else if (property == "textureselectedtab")
        {
            m_spriteSelectedTab.setTexture(getSharedRenderer()->getTextureSelectedTab());
        }
        else if (property == "textureselectedtabhover")
        {
            m_spriteSelectedTabHover.setTexture(getSharedRenderer()->getTextureSelectedTabHover());
        }
        else if (property == "texturedisabledtab")
        {
            m_spriteDisabledTab.setTexture(getSharedRenderer()->getTextureDisabledTab());
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            recalculateTabsWidth();
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
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTab.setOpacity(m_opacityCached);
            m_spriteTabHover.setOpacity(m_opacityCached);
            m_spriteSelectedTab.setOpacity(m_opacityCached);
            m_spriteSelectedTabHover.setOpacity(m_opacityCached);
            m_spriteDisabledTab.setOpacity(m_opacityCached);

            for (auto& tab : m_tabs)
                tab.text.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& tab : m_tabs)
                tab.text.setFont(m_fontCached);

            // Recalculate the size when the text is auto sizing
            if (m_requestedTextSize == 0)
                setTextSize(0);
            else
                recalculateTabsWidth();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Tabs::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_tabs.size() > 0)
        {
            bool allTabsVisible = true;
            bool allTabsEnabled = true;
            std::string tabList = "[" + Serializer::serialize(getText(0));
            std::string tabVisibleList = "[" + Serializer::serialize(getTabVisible(0));
            std::string tabEnabledList = "[" + Serializer::serialize(getTabEnabled(0));
            for (std::size_t i = 1; i < m_tabs.size(); ++i)
            {
                tabList += ", " + Serializer::serialize(getText(i));
                tabVisibleList += ", " + Serializer::serialize(getTabVisible(i));
                tabEnabledList += ", " + Serializer::serialize(getTabEnabled(i));

                if (!getTabVisible(i))
                    allTabsVisible = false;
                if (!getTabEnabled(i))
                    allTabsEnabled = false;
            }

            tabList += "]";
            tabVisibleList += "]";
            tabEnabledList += "]";

            node->propertyValuePairs["Tabs"] = std::make_unique<DataIO::ValueNode>(tabList);
            if (!allTabsVisible)
                node->propertyValuePairs["TabsVisible"] = std::make_unique<DataIO::ValueNode>(tabVisibleList);
            if (!allTabsEnabled)
                node->propertyValuePairs["TabsEnabled"] = std::make_unique<DataIO::ValueNode>(tabEnabledList);
        }

        if (getSelectedIndex() >= 0)
            node->propertyValuePairs["Selected"] = std::make_unique<DataIO::ValueNode>(to_string(getSelectedIndex()));

        if (m_maximumTabWidth > 0)
            node->propertyValuePairs["MaximumTabWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_maximumTabWidth));

        if (m_autoSize)
        {
            node->propertyValuePairs.erase("Size");
            node->propertyValuePairs["TabHeight"] = std::make_unique<DataIO::ValueNode>(to_string(getSize().y));
        }

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_autoSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["tabs"])
        {
            if (!node->propertyValuePairs["tabs"]->listNode)
                throw Exception{"Failed to parse 'Tabs' property, expected a list as value"};

            for (const auto& tabText : node->propertyValuePairs["tabs"]->valueList)
                add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString());
        }

        if (node->propertyValuePairs["tabsvisible"])
        {
            if (!node->propertyValuePairs["tabsvisible"]->listNode)
                throw Exception{"Failed to parse 'TabsVisible' property, expected a list as value"};

            const auto& values = node->propertyValuePairs["tabsvisible"]->valueList;
            for (unsigned int i = 0; i < values.size(); ++i)
                setTabVisible(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs["tabsenabled"])
        {
            if (!node->propertyValuePairs["tabsenabled"]->listNode)
                throw Exception{"Failed to parse 'TabsEnabled' property, expected a list as value"};

            const auto& values = node->propertyValuePairs["tabsenabled"]->valueList;
            for (unsigned int i = 0; i < values.size(); ++i)
                setTabEnabled(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs["maximumtabwidth"])
            setMaximumTabWidth(tgui::stof(node->propertyValuePairs["maximumtabwidth"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["tabheight"])
            setTabHeight(tgui::stof(node->propertyValuePairs["tabheight"]->value));
        if (node->propertyValuePairs["selected"])
            select(tgui::stoi(node->propertyValuePairs["selected"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::updateTextColors()
    {
        for (auto& tab : m_tabs)
        {
            if ((!m_enabled || !tab.enabled) && m_textColorDisabledCached.isSet())
                tab.text.setColor(m_textColorDisabledCached);
            else
                tab.text.setColor(m_textColorCached);
        }

        if (m_selectedTab >= 0)
        {
            if ((m_selectedTab == m_hoveringTab) && m_selectedTextColorHoverCached.isSet())
                m_tabs[m_selectedTab].text.setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_tabs[m_selectedTab].text.setColor(m_selectedTextColorCached);
        }

        if ((m_hoveringTab >= 0) && (m_selectedTab != m_hoveringTab))
        {
            if (m_textColorHoverCached.isSet())
                m_tabs[m_hoveringTab].text.setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        states.transform.translate({});

        // Draw the borders around the tabs
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const float usableHeight = getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom();
        for (unsigned int i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            sf::RenderStates textStates = states;

            // Draw the background of the tab
            if (m_spriteTab.isSet() && m_spriteSelectedTab.isSet())
            {
                Sprite spriteTab;
                if ((!m_enabled || !m_tabs[i].enabled) && m_spriteDisabledTab.isSet())
                    spriteTab = m_spriteDisabledTab;
                else if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_spriteSelectedTabHover.isSet())
                        spriteTab = m_spriteSelectedTabHover;
                    else
                        spriteTab = m_spriteSelectedTab;
                }
                else if ((m_hoveringTab == static_cast<int>(i)) && m_spriteTabHover.isSet())
                    spriteTab = m_spriteTabHover;
                else
                    spriteTab = m_spriteTab;

                spriteTab.setSize({m_tabs[i].width, usableHeight});
                spriteTab.draw(target, states);
            }
            else // No texture was loaded
            {
                if ((!m_enabled || !m_tabs[i].enabled) && m_backgroundColorDisabledCached.isSet())
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorDisabledCached);
                else if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBackgroundColorHoverCached.isSet())
                        drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_selectedBackgroundColorHoverCached);
                    else
                        drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_selectedBackgroundColorCached);
                }
                else if ((m_hoveringTab == static_cast<int>(i)) && m_backgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, {m_tabs[i].width, usableHeight}, m_backgroundColorCached);
            }

            // Draw the borders between the tabs
            states.transform.translate({m_tabs[i].width, 0});
            if ((m_bordersCached != Borders{0}) && (i < m_tabs.size() - 1))
            {
                drawRectangleShape(target, states, {(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, usableHeight}, m_borderColorCached);
                states.transform.translate({(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, 0});
            }

            // Apply clipping if required for the text in this tab
            const float usableWidth = m_tabs[i].width - (2 * m_distanceToSideCached);
        #ifdef TGUI_USE_CPP17
            std::optional<Clipping> clipping;
            if (m_tabs[i].text.getSize().x > usableWidth)
                clipping.emplace(target, textStates, Vector2f{m_distanceToSideCached, 0}, Vector2f{usableWidth, usableHeight});
        #else
            std::unique_ptr<Clipping> clipping;
            if (m_tabs[i].text.getSize().x > usableWidth)
                clipping = std::make_unique<Clipping>(target, textStates, Vector2f{m_distanceToSideCached, 0}, Vector2f{usableWidth, usableHeight});
        #endif

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabs[i].text.getSize().x) / 2.f), ((usableHeight - m_tabs[i].text.getSize().y) / 2.f)});
            m_tabs[i].text.draw(target, textStates);
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


#include <TGUI/Widgets/TreeView.hpp>
#include <TGUI/Clipping.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        void setTextSizeImpl(std::vector<std::shared_ptr<TreeView::Node>>& nodes, unsigned int textSize)
        {
            for (auto& node : nodes)
            {
                node->text.setCharacterSize(textSize);
                if (!node->nodes.empty())
                    setTextSizeImpl(node->nodes, textSize);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<TreeView::Node> cloneNode(const std::shared_ptr<TreeView::Node>& oldNode, TreeView::Node* parent)
        {
            auto newNode = std::make_shared<TreeView::Node>();
            newNode->text = oldNode->text;
            newNode->depth = oldNode->depth;
            newNode->expanded = oldNode->expanded;
            newNode->parent = parent;

            for (const auto& oldChild : oldNode->nodes)
                newNode->nodes.push_back(cloneNode(oldChild, newNode.get()));

            return newNode;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void cloneVisibleNodeList(const std::vector<std::shared_ptr<TreeView::Node>>& oldNodes,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newNodes,
                                  const std::vector<std::shared_ptr<TreeView::Node>>& oldList,
                                  std::vector<std::shared_ptr<TreeView::Node>>& newList)
        {
            assert(oldNodes.size() == newNodes.size());
            assert(oldList.size() == newList.size());

            for (unsigned int i = 0; i < oldNodes.size(); ++i)
            {
                for (unsigned int j = 0; j < oldList.size(); ++j)
                {
                    if (oldNodes[i] == oldList[j])
                        newList[j] = newNodes[i];
                }

                cloneVisibleNodeList(oldNodes[i]->nodes, newNodes[i]->nodes, oldList, newList);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void expandOrCollapseAll(std::vector<std::shared_ptr<TreeView::Node>>& nodes, bool expand)
        {
            for (auto& node : nodes)
            {
                if (!node->nodes.empty())
                {
                    node->expanded = expand;
                    expandOrCollapseAll(node->nodes, expand);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool removeItemImpl(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty, unsigned int parentIndex, std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            for (auto it = nodes.begin(); it != nodes.end(); ++it)
            {
                if ((*it)->text.getString() != hierarchy[parentIndex])
                    continue;

                if (parentIndex + 1 == hierarchy.size())
                {
                    nodes.erase(it);
                    return true;
                }
                else
                {
                    // Return false if some menu in the hierarchy couldn't be found
                    if (!removeItemImpl(hierarchy, removeParentsWhenEmpty, parentIndex + 1, (*it)->nodes))
                        return false;

                    // If parents don't have to be removed as well then we are done
                    if (!removeParentsWhenEmpty)
                        return true;

                    // Also delete the parent if empty
                    if ((*it)->nodes.empty())
                        nodes.erase(it);

                    return true;
                }
            }

            // The hierarchy doesn't exist
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<TreeView::ConstNode> convertNodesToConstNodes(const std::vector<std::shared_ptr<TreeView::Node>>& nodes)
        {
            std::vector<TreeView::ConstNode> constNodes;
            for (const auto& node : nodes)
            {
                TreeView::ConstNode constNode;
                constNode.expanded = node->expanded;
                constNode.text = node->text.getString();
                constNode.nodes = convertNodesToConstNodes(node->nodes);
                constNodes.push_back(std::move(constNode));
            }
            return constNodes;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        TreeView::Node* findNode(const std::vector<std::shared_ptr<TreeView::Node>>& nodes, const std::vector<sf::String>& hierarchy, unsigned int parentIndex)
        {
            for (auto& node : nodes)
            {
                if (node->text.getString() != hierarchy[parentIndex])
                    continue;
                else if (parentIndex + 1 == hierarchy.size())
                    return node.get();
                else
                    return findNode(node->nodes, hierarchy, parentIndex + 1);
            }

            return nullptr;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void saveItems(std::unique_ptr<DataIO::Node>& parentNode, const std::vector<std::shared_ptr<TreeView::Node>>& items)
        {
            for (const auto& item : items)
            {
                auto itemNode = std::make_unique<DataIO::Node>();
                itemNode->name = "Item";

                itemNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(item->text.getString()));

                if (!item->nodes.empty())
                {
                    // Save as nested 'Item' sections only when needed, use the more compact string list when just storing the leaf items
                    bool recursionNeeded = false;
                    for (const auto& childItem : item->nodes)
                    {
                        if (!childItem->nodes.empty())
                        {
                            recursionNeeded = true;
                            break;
                        }
                    }

                    if (recursionNeeded)
                        saveItems(itemNode, item->nodes);
                    else
                    {
                        std::string itemList = "[" + Serializer::serialize(item->nodes[0]->text.getString());
                        for (std::size_t i = 1; i < item->nodes.size(); ++i)
                            itemList += ", " + Serializer::serialize(item->nodes[i]->text.getString());
                        itemList += "]";

                        itemNode->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
                    }
                }

                parentNode->children.push_back(std::move(itemNode));
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView()
    {
        m_type = "TreeView";
        m_draggableWidget = true;

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        m_renderer = aurora::makeCopied<TreeViewRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 1.25f));
        setSize({Text::getLineHeight(m_fontCached, m_textSize, m_textStyleCached) * 10,
                 (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::TreeView(const TreeView& other) :
        Widget                              {other},
        onItemSelect                        {other.onItemSelect},
        onDoubleClick                       {other.onDoubleClick},
        onExpand                            {other.onExpand},
        onCollapse                          {other.onCollapse},
        m_selectedItem                      {other.m_selectedItem},
        m_hoveredItem                       {other.m_hoveredItem},
        m_itemHeight                        {other.m_itemHeight},
        m_requestedTextSize                 {other.m_requestedTextSize},
        m_textSize                          {other.m_textSize},
        m_maxRight                          {other.m_maxRight},
        m_iconBounds                        {other.m_iconBounds},
        m_verticalScrollbar                 {other.m_verticalScrollbar},
        m_horizontalScrollbar               {other.m_horizontalScrollbar},
        m_possibleDoubleClick               {other.m_possibleDoubleClick},
        m_doubleClickNodeIndex              {other.m_doubleClickNodeIndex},
        m_spriteBranchExpanded              {other.m_spriteBranchExpanded},
        m_spriteBranchCollapsed             {other.m_spriteBranchCollapsed},
        m_spriteLeaf                        {other.m_spriteLeaf},
        m_bordersCached                     {other.m_bordersCached},
        m_paddingCached                     {other.m_paddingCached},
        m_borderColorCached                 {other.m_borderColorCached},
        m_backgroundColorCached             {other.m_backgroundColorCached},
        m_textColorCached                   {other.m_textColorCached},
        m_textColorHoverCached              {other.m_textColorHoverCached},
        m_selectedTextColorCached           {other.m_selectedTextColorCached},
        m_selectedTextColorHoverCached      {other.m_selectedTextColorHoverCached},
        m_selectedBackgroundColorCached     {other.m_selectedBackgroundColorCached},
        m_selectedBackgroundColorHoverCached{other.m_selectedBackgroundColorHoverCached},
        m_backgroundColorHoverCached        {other.m_backgroundColorHoverCached},
        m_textStyleCached                   {other.m_textStyleCached}
    {
        for (const auto& node : other.m_nodes)
            m_nodes.push_back(cloneNode(node, nullptr));

        m_visibleNodes.resize(other.m_visibleNodes.size(), nullptr);
        cloneVisibleNodeList(other.m_nodes, m_nodes, other.m_visibleNodes, m_visibleNodes);

        assert(std::count(m_visibleNodes.begin(), m_visibleNodes.end(), nullptr) == 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView& TreeView::operator= (const TreeView& other)
    {
        if (this != &other)
        {
            TreeView temp(other);
            Widget::operator=(temp);

            std::swap(onItemSelect,                         temp.onItemSelect);
            std::swap(onDoubleClick,                        temp.onDoubleClick);
            std::swap(onExpand,                             temp.onExpand);
            std::swap(onCollapse,                           temp.onCollapse);
            std::swap(m_nodes,                              temp.m_nodes);
            std::swap(m_visibleNodes,                       temp.m_visibleNodes);
            std::swap(m_selectedItem,                       temp.m_selectedItem);
            std::swap(m_hoveredItem,                        temp.m_hoveredItem);
            std::swap(m_itemHeight,                         temp.m_itemHeight);
            std::swap(m_requestedTextSize,                  temp.m_requestedTextSize);
            std::swap(m_textSize,                           temp.m_textSize);
            std::swap(m_maxRight,                           temp.m_maxRight);
            std::swap(m_iconBounds,                         temp.m_iconBounds);
            std::swap(m_verticalScrollbar,                  temp.m_verticalScrollbar);
            std::swap(m_horizontalScrollbar,                temp.m_horizontalScrollbar);
            std::swap(m_possibleDoubleClick,                temp.m_possibleDoubleClick);
            std::swap(m_doubleClickNodeIndex,               temp.m_doubleClickNodeIndex);
            std::swap(m_spriteBranchExpanded,               temp.m_spriteBranchExpanded);
            std::swap(m_spriteBranchCollapsed,              temp.m_spriteBranchCollapsed);
            std::swap(m_spriteLeaf,                         temp.m_spriteLeaf);
            std::swap(m_bordersCached,                      temp.m_bordersCached);
            std::swap(m_paddingCached,                      temp.m_paddingCached);
            std::swap(m_borderColorCached,                  temp.m_borderColorCached);
            std::swap(m_backgroundColorCached,              temp.m_backgroundColorCached);
            std::swap(m_textColorCached,                    temp.m_textColorCached);
            std::swap(m_textColorHoverCached,               temp.m_textColorHoverCached);
            std::swap(m_selectedTextColorCached,            temp.m_selectedTextColorCached);
            std::swap(m_selectedTextColorHoverCached,       temp.m_selectedTextColorHoverCached);
            std::swap(m_selectedBackgroundColorCached,      temp.m_selectedBackgroundColorCached);
            std::swap(m_selectedBackgroundColorHoverCached, temp.m_selectedBackgroundColorHoverCached);
            std::swap(m_backgroundColorHoverCached,         temp.m_backgroundColorHoverCached);
            std::swap(m_textStyleCached,                    temp.m_textStyleCached);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::create()
    {
        return std::make_shared<TreeView>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Ptr TreeView::copy(TreeView::ConstPtr treeView)
    {
        if (treeView)
            return std::static_pointer_cast<TreeView>(treeView->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getSharedRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TreeViewRenderer* TreeView::getSharedRenderer() const
    {
        return aurora::downcast<const TreeViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeViewRenderer* TreeView::getRenderer()
    {
        return aurora::downcast<TreeViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TreeViewRenderer* TreeView::getRenderer() const
    {
        return aurora::downcast<const TreeViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (m_spriteBranchCollapsed.isSet() || m_spriteBranchExpanded.isSet() || m_spriteLeaf.isSet())
        {
            m_iconBounds =
                {
                    std::max(std::max(m_spriteBranchCollapsed.getSize().x, m_spriteBranchExpanded.getSize().x), m_spriteLeaf.getSize().x),
                    std::max(std::max(m_spriteBranchCollapsed.getSize().y, m_spriteBranchExpanded.getSize().y), m_spriteLeaf.getSize().y)
                };
        }
        else
        {
            const float wantedIconSize = std::round(m_itemHeight / 2.f);
            const float lineThickness = std::round(m_itemHeight / 10.f);
            const float iconSize = (std::floor((wantedIconSize - lineThickness) / 2.f) * 2.f) + lineThickness; // "+" sign should be symmetric
            m_iconBounds = {iconSize, iconSize};
        }

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::addItem(const std::vector<sf::String>& hierarchy, bool createParents)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findParentNode(hierarchy, 0, m_nodes, nullptr, createParents);
            if (!node)
                return false;

            createNode(node->nodes, node, hierarchy.back());
        }
        else // Root node
            createNode(m_nodes, nullptr, hierarchy.back());

        markNodesDirty();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expand(const std::vector<sf::String>& hierarchy)
    {
        expandOrCollapse(hierarchy, true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::expandAll()
    {
        expandOrCollapseAll(m_nodes, true);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapse(const std::vector<sf::String>& hierarchy)
    {
        expandOrCollapse(hierarchy, false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::collapseAll()
    {
        expandOrCollapseAll(m_nodes, false);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::removeItem(const std::vector<sf::String>& hierarchy, bool removeParentsWhenEmpty)
    {
        const bool ret = removeItemImpl(hierarchy, removeParentsWhenEmpty, 0, m_nodes);
        markNodesDirty();
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::removeAllItems()
    {
        m_nodes.clear();
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> TreeView::getSelectedItem() const
    {
        std::vector<sf::String> hierarchy;

        if (m_selectedItem == -1)
            return hierarchy;

        const auto* node = m_visibleNodes[m_selectedItem].get();
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        return hierarchy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::toggleNodeInternal(std::size_t index)
    {
        if (index >= m_visibleNodes.size())
            return;

        if (m_visibleNodes[index]->nodes.empty())
            return;

        std::vector<sf::String> hierarchy;
        auto* node = m_visibleNodes[index].get();
        while (node)
        {
            hierarchy.insert(hierarchy.begin(), node->text.getString());
            node = node->parent;
        }

        m_visibleNodes[index]->expanded = !m_visibleNodes[index]->expanded;
        if (m_visibleNodes[index]->expanded)
            onExpand.emit(this, hierarchy.back(), hierarchy);
        else
            onCollapse.emit(this, hierarchy.back(), hierarchy);

        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TreeView::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<TreeView::ConstNode> TreeView::getNodes() const
    {
        return convertNodesToConstNodes(m_nodes);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setItemHeight(unsigned int itemHeight)
    {
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
            setTextSize(0);

        m_verticalScrollbar->setScrollAmount(m_itemHeight);
        m_horizontalScrollbar->setScrollAmount(m_itemHeight);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        setTextSizeImpl(m_nodes, textSize);
        markNodesDirty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos))
            m_verticalScrollbar->leftMousePressed(pos);
        else if (m_horizontalScrollbar->mouseOnWidget(pos))
            m_horizontalScrollbar->leftMousePressed(pos);
        else
        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();
                int selectedItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (selectedItem >= static_cast<int>(m_visibleNodes.size()))
                    selectedItem = -1;

                updateSelectedItem(selectedItem);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();
        auto childPos = pos;
        if (m_mouseDown && !m_verticalScrollbar->isMouseDown() && !m_horizontalScrollbar->isMouseDown())
        {
            m_mouseDown = false;

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            bool iconPressed = false;
            int selectedIndex = -1;
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                selectedIndex = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if ((selectedIndex >= 0) && (selectedIndex == m_selectedItem))
                {
                    // Expand or colapse the node when clicking the icon
                    const float iconPadding = (m_iconBounds.x / 4.f);
                    const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * m_visibleNodes[selectedIndex]->depth);
                    if (FloatRect{iconOffset + m_bordersCached.getLeft() + m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(),
                                  (selectedIndex * m_itemHeight) + m_bordersCached.getTop() + m_paddingCached.getTop() - m_verticalScrollbar->getValue(),
                                  m_iconBounds.x,
                                  m_iconBounds.y}.contains(pos))
                    {
                        toggleNodeInternal(selectedIndex);
                        m_possibleDoubleClick = false;
                        iconPressed = true;
                    }
                }
            }

            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if ((selectedIndex >= 0) && (selectedIndex == m_doubleClickNodeIndex))
                {
                    toggleNodeInternal(selectedIndex);

                    // Send double click if this was a leaf node
                    if (m_visibleNodes[selectedIndex]->nodes.empty())
                    {
                        std::vector<sf::String> hierarchy;
                        auto* node = m_visibleNodes[selectedIndex].get();
                        while (node)
                        {
                            hierarchy.insert(hierarchy.begin(), node->text.getString());
                            node = node->parent;
                        }

                        onDoubleClick.emit(this, hierarchy.back(), hierarchy);
                    }
                }
            }
            else if (!iconPressed)
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
                m_doubleClickNodeIndex = selectedIndex;
            }
        }

        m_verticalScrollbar->leftMouseReleased(childPos);
        m_horizontalScrollbar->leftMouseReleased(childPos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos))
            m_verticalScrollbar->mouseMoved(pos);
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos))
            m_horizontalScrollbar->mouseMoved(pos);
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();

            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveredItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (hoveredItem >= static_cast<int>(m_visibleNodes.size()))
                    hoveredItem = -1;

                updateHoveredItem(hoveredItem);
            }
            else // Mouse is on top of padding or borders
                updateHoveredItem(-1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
            return true;
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();

        m_hoveredItem = -1;
        m_possibleDoubleClick = false;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_verticalScrollbar->mouseNoLongerDown();
        m_horizontalScrollbar->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TreeView::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else if (signalName == toLower(onExpand.getName()))
            return onExpand;
        else if (signalName == toLower(onCollapse.getName()))
            return onCollapse;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::rendererChanged(const std::string& property)
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
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "texturebranchexpanded")
        {
            m_spriteBranchExpanded.setTexture(getSharedRenderer()->getTextureBranchExpanded());
            markNodesDirty();
        }
        else if (property == "texturebranchcollapsed")
        {
            m_spriteBranchCollapsed.setTexture(getSharedRenderer()->getTextureBranchCollapsed());
            markNodesDirty();
        }
        else if (property == "textureleaf")
        {
            m_spriteLeaf.setTexture(getSharedRenderer()->getTextureLeaf());
            markNodesDirty();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }
        else if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            markNodesDirty();
        }
        else
        {
            Widget::rendererChanged(property);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TreeView::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        saveItems(node, m_nodes);
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["itemheight"])
            setItemHeight(tgui::stoi(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

        loadItems(node, m_nodes, nullptr);

        // Remove the 'Item' nodes as they have been processed
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(),
            [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "item"; }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::loadItems(const std::unique_ptr<DataIO::Node>& node, std::vector<std::shared_ptr<Node>>& items, Node* parent)
    {
        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "item")
                continue;

            if (!childNode->propertyValuePairs["text"])
                throw Exception{"Failed to parse 'Item' property, expected a nested 'Text' propery"};

            const sf::String itemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString();
            createNode(items, parent, itemText);

            // Recursively handle the menu nodes
            if (!childNode->children.empty())
                loadItems(childNode, items.back()->nodes, items.back().get());

            // Menu items can also be stored in an string array in the 'Items' property instead of as a nested Menu section
            if (childNode->propertyValuePairs["items"])
            {
                if (!childNode->propertyValuePairs["items"]->listNode)
                    throw Exception{"Failed to parse 'Items' property inside 'Item' property, expected a list as value"};

                for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    const sf::String subItemText = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString();
                    createNode(items.back()->nodes, items.back().get(), subItemText);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TreeView::updateVisibleNodes(std::vector<std::shared_ptr<Node>>& nodes, Node* selectedNode, float textPadding, unsigned int pos)
    {
        for (auto& node : nodes)
        {
            m_visibleNodes.push_back(node);
            if (selectedNode == node.get())
                m_selectedItem = pos;

            const float iconPadding = (m_iconBounds.x / 4.f);
            const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * node->depth);
            node->text.setPosition(iconOffset + m_iconBounds.x + iconPadding + textPadding,
                                   (pos * m_itemHeight) + ((m_itemHeight - node->text.getSize().y) / 2.f));

            const float right = node->text.getPosition().x + node->text.getSize().x + m_paddingCached.getRight();
            if (right > m_maxRight)
                m_maxRight = right;

            pos++;
            if (node->expanded && !node->nodes.empty())
                pos = updateVisibleNodes(node->nodes, selectedNode, textPadding, pos);
        }

        return pos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::markNodesDirty()
    {
        Node* selectedNode = nullptr;
        if (m_selectedItem >= 0 && static_cast<std::size_t>(m_selectedItem) < m_visibleNodes.size())
            selectedNode = m_visibleNodes[m_selectedItem].get();

        int oldHoveredItem = m_hoveredItem;

        m_maxRight = 0;
        m_hoveredItem = -1;
        m_selectedItem = -1;
        m_visibleNodes.clear();
        updateVisibleNodes(m_nodes, selectedNode, Text::getExtraHorizontalPadding(m_fontCached, m_textSize), 0);

        if (oldHoveredItem >= 0)
        {
            if (static_cast<std::size_t>(oldHoveredItem) < m_visibleNodes.size())
                m_hoveredItem = oldHoveredItem;

            updateTextColors(m_nodes);
            updateSelectedAndHoveringItemColors();
        }

        m_verticalScrollbar->setMaximum(m_itemHeight * m_visibleNodes.size());
        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_maxRight));

        if ((m_maxRight + m_verticalScrollbar->getSize().x) > (getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()))
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y - m_horizontalScrollbar->getSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_horizontalScrollbar->getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x - m_verticalScrollbar->getSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_verticalScrollbar->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({std::max(0.f, getInnerSize().x), m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }

        // If the horizontal scrollbar is shown then add a little padding at the end of the longest line
        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_horizontalScrollbar->getMaximum() + (m_iconBounds.x / 4.f)));

        m_verticalScrollbar->setPosition(getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        sf::RenderStates statesForScrollbars = states;

        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                maxItemWidth -= m_verticalScrollbar->getSize().x;

            const Clipping clipping
            {
                target, states,
                {m_paddingCached.getLeft(), m_paddingCached.getTop()},
                {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}
            };

            int firstNode = 0;
            int lastNode = m_visibleNodes.size();
            if (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum())
            {
                firstNode = static_cast<int>(m_verticalScrollbar->getValue() / m_itemHeight);
                lastNode = static_cast<int>((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) / m_itemHeight);

                // Show another item when the scrollbar is standing between two items
                if ((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) % m_itemHeight != 0)
                    ++lastNode;
            }

            states.transform.translate({m_paddingCached.getLeft() - m_horizontalScrollbar->getValue(), m_paddingCached.getTop() - m_verticalScrollbar->getValue()});

            // Draw the background of the selected item
            if ((m_selectedItem >= firstNode) && (m_selectedItem < lastNode))
            {
                states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_selectedItem * static_cast<float>(m_itemHeight)});

                const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveredItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorCached);

                states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_selectedItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveredItem >= firstNode) && (m_hoveredItem < lastNode) && (m_hoveredItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({static_cast<float>(m_horizontalScrollbar->getValue()), m_hoveredItem * static_cast<float>(m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -m_hoveredItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the icons
            for (int i = firstNode; i < lastNode; ++i)
            {
                auto statesForIcon = states;
                const float iconPadding = (m_iconBounds.x / 4.f);
                const float iconOffset = iconPadding + ((m_iconBounds.x + iconPadding) * m_visibleNodes[i]->depth);
                statesForIcon.transform.translate(iconOffset, std::round((i * m_itemHeight) + ((m_itemHeight - m_iconBounds.y) / 2.f)));

                // Draw an icon for the leaf node if a texture is set
                if (m_visibleNodes[i]->nodes.empty())
                {
                    if (m_spriteLeaf.isSet())
                        m_spriteLeaf.draw(target, statesForIcon);
                }
                else // Branch node
                {
                    if (m_spriteLeaf.isSet() || m_spriteBranchExpanded.isSet() || m_spriteBranchCollapsed.isSet())
                    {
                        const Sprite* iconSprite = nullptr;
                        if (m_visibleNodes[i]->expanded)
                        {
                            if (m_spriteBranchExpanded.isSet())
                                iconSprite = &m_spriteBranchExpanded;
                            else if (m_spriteBranchCollapsed.isSet())
                                iconSprite = &m_spriteBranchCollapsed;
                            else
                                iconSprite = &m_spriteLeaf;
                        }
                        else // Collapsed node
                        {
                            if (m_spriteBranchCollapsed.isSet())
                                iconSprite = &m_spriteBranchCollapsed;
                            else if (m_spriteBranchExpanded.isSet())
                                iconSprite = &m_spriteBranchExpanded;
                            else
                                iconSprite = &m_spriteLeaf;
                        }

                        iconSprite->draw(target, statesForIcon);
                    }
                    else // No textures are used
                    {
                        const Color& iconColor = (i == m_hoveredItem) ? m_textColorHoverCached : m_textColorCached;
                        const float thickness = std::max(1.f, std::round(m_itemHeight / 10.f));
                        if (m_visibleNodes[i]->expanded)
                        {
                            // Draw "-"
                            statesForIcon.transform.translate(0, (m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {m_iconBounds.x, thickness}, iconColor);
                        }
                        else // Collapsed node
                        {
                            // Draw "+"
                            statesForIcon.transform.translate(0, (m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {m_iconBounds.x, thickness}, iconColor);
                            statesForIcon.transform.translate((m_iconBounds.x - thickness) / 2.f, -(m_iconBounds.y - thickness) / 2.f);
                            drawRectangleShape(target, statesForIcon, {thickness, m_iconBounds.y}, iconColor);
                        }
                    }
                }
            }

            // Draw the texts
            for (int i = firstNode; i < lastNode; ++i)
                m_visibleNodes[i]->text.draw(target, states);
        }

        m_horizontalScrollbar->draw(target, statesForScrollbars);
        m_verticalScrollbar->draw(target, statesForScrollbars);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::createNode(std::vector<std::shared_ptr<Node>>& nodes, Node* parent, const sf::String& text)
    {
        auto newNode = std::make_shared<Node>();
        newNode->text.setFont(m_fontCached);
        newNode->text.setColor(m_textColorCached);
        newNode->text.setOpacity(m_opacityCached);
        newNode->text.setCharacterSize(m_textSize);
        newNode->text.setString(text);
        newNode->expanded = true;
        newNode->parent = parent;

        if (parent)
            newNode->depth = parent->depth + 1;
        else
            newNode->depth = 0;

        nodes.push_back(std::move(newNode));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TreeView::expandOrCollapse(const std::vector<sf::String>& hierarchy, bool expand)
    {
        if (hierarchy.empty())
            return false;

        if (hierarchy.size() >= 2)
        {
            auto* node = findNode(m_nodes, hierarchy, 0);
            if (!node)
                return false;

            node->expanded = expand;
        }
        else // Root node
        {
            for (auto& node : m_nodes)
            {
                if (node->text.getString() == hierarchy.back())
                {
                    node->expanded = expand;
                    break;
                }
            }
        }

        markNodesDirty();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateTextColors(std::vector<std::shared_ptr<Node>>& nodes)
    {
        for (auto& node : nodes)
        {
            node->text.setColor(m_textColorCached);
            updateTextColors(node->nodes);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedAndHoveringItemColors()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_selectedTextColorCached);
        }

        if ((m_hoveredItem >= 0) && (m_selectedItem != m_hoveredItem))
        {
            if (m_textColorHoverCached.isSet())
                m_visibleNodes[m_hoveredItem]->text.setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateHoveredItem(int item)
    {
        if (m_hoveredItem == item)
            return;

        if (m_hoveredItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorCached.isSet())
                m_visibleNodes[m_hoveredItem]->text.setColor(m_selectedTextColorCached);
            else
                m_visibleNodes[m_hoveredItem]->text.setColor(m_textColorCached);
        }

        m_hoveredItem = item;
        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TreeView::updateSelectedItem(int item)
    {
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_textColorHoverCached.isSet())
                m_visibleNodes[m_selectedItem]->text.setColor(m_textColorHoverCached);
            else
                m_visibleNodes[m_selectedItem]->text.setColor(m_textColorCached);
        }

        m_selectedItem = item;
        if (m_selectedItem >= 0)
        {
            std::vector<sf::String> hierarchy;
            auto* node = m_visibleNodes[m_selectedItem].get();
            while (node)
            {
                hierarchy.insert(hierarchy.begin(), node->text.getString());
                node = node->parent;
            }

            onItemSelect.emit(this, hierarchy.back(), hierarchy);
        }
        else
            onItemSelect.emit(this, "", {});

        updateSelectedAndHoveringItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TreeView::Node* TreeView::findParentNode(const std::vector<sf::String>& hierarchy, unsigned int parentIndex, std::vector<std::shared_ptr<Node>>& nodes, Node* parent, bool createParents)
    {
        for (auto& node : nodes)
        {
            if (node->text.getString() != hierarchy[parentIndex])
                continue;
            else if (parentIndex + 2 == hierarchy.size())
                return node.get();
            else
                return findParentNode(hierarchy, parentIndex + 1, node->nodes, node.get(), createParents);
        }

        if (createParents)
        {
            createNode(nodes, parent, hierarchy[parentIndex]);
            if (parentIndex + 2 == hierarchy.size())
                return nodes.back().get();
            else
                return findParentNode(hierarchy, parentIndex + 1, nodes.back()->nodes, nodes.back().get(), createParents);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

