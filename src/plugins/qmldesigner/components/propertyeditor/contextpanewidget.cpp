#include "contextpanewidget.h"
#include <coreplugin/icore.h>
#include <QFontComboBox>
#include <QComboBox>
#include <QSpinBox>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QGridLayout>
#include <QToolButton>
#include <QAction>
#include <qmldesignerplugin.h>
#include "colorwidget.h"
#include "contextpanetextwidget.h"
#include "easingcontextpane.h"
#include "contextpanewidgetimage.h"
#include "contextpanewidgetrectangle.h"

namespace QmlDesigner {


DragWidget::DragWidget(QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    m_oldPos = QPoint(-1, -1);
    m_pos = QPoint(-1, -1);

    m_dropShadowEffect = new QGraphicsDropShadowEffect;
    m_dropShadowEffect->setBlurRadius(6);
    m_dropShadowEffect->setOffset(2, 2);
    setGraphicsEffect(m_dropShadowEffect);
}

void DragWidget::mousePressEvent(QMouseEvent * event)
{
    if (event->button() ==  Qt::LeftButton) {
        m_oldPos = event->globalPos();
        m_opacityEffect = new QGraphicsOpacityEffect;
        setGraphicsEffect(m_opacityEffect);
        event->accept();
    }
    QFrame::mousePressEvent(event);
}

void DragWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() ==  Qt::LeftButton) {
        m_oldPos = QPoint(-1, -1);
        m_dropShadowEffect = new QGraphicsDropShadowEffect;
        m_dropShadowEffect->setBlurRadius(6);
        m_dropShadowEffect->setOffset(2, 2);
        setGraphicsEffect(m_dropShadowEffect);
    }
    QFrame::mouseReleaseEvent(event);
}

void DragWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() &&  Qt::LeftButton) {
        if (pos().x() < 10  && event->pos().x() < -20)
            return;
        if (m_oldPos != QPoint(-1, -1)) {
            QPoint diff = event->globalPos() - m_oldPos;
            QPoint newPos = pos() + diff;
            if (newPos.x() > 0 && newPos.y() > 0 && (newPos.x() + width()) < parentWidget()->width() && (newPos.y() + height()) < parentWidget()->height()) {
                if (m_secondaryTarget)
                    m_secondaryTarget->move(m_secondaryTarget->pos() + diff);
                move(newPos);
                m_pos = newPos;
            }
        } else {
            m_opacityEffect = new QGraphicsOpacityEffect;
            setGraphicsEffect(m_opacityEffect);
        }
        m_oldPos = event->globalPos();
        event->accept();
    }
}

ContextPaneWidget::ContextPaneWidget(QWidget *parent) : DragWidget(parent), m_currentWidget(0)
{
//    setFrameStyle(QFrame::NoFrame);
//    setFrameShape(QFrame::StyledPanel);
//    setFrameShadow(QFrame::Sunken);
//    m_oldPos = QPoint(-1, -1);
//    m_pos = QPoint(-1, -1);

//    m_dropShadowEffect = new QGraphicsDropShadowEffect;
//    m_dropShadowEffect->setBlurRadius(6);
//    m_dropShadowEffect->setOffset(2, 2);
//    setGraphicsEffect(m_dropShadowEffect);

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(0);
    QToolButton *toolButton = new QToolButton(this);    
    QIcon icon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    toolButton->setIcon(icon);
    toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);    
    toolButton->setFixedSize(icon.availableSizes().value(0) + QSize(4, 4));
    connect(toolButton, SIGNAL(clicked()), this, SLOT(onTogglePane()));
    layout->addWidget(toolButton, 0, 0, 1, 1);
    colorDialog();

    QWidget *fontWidget = createFontWidget();
    m_currentWidget = fontWidget;
    QWidget *imageWidget = createImageWidget();
    QWidget *rectangleWidget = createRectangleWidget();
    QWidget *easingWidget = createEasingWidget();
    layout->addWidget(fontWidget, 0, 1, 2, 1);
    layout->addWidget(easingWidget, 0, 1, 2, 1);
    layout->addWidget(imageWidget, 0, 1, 2, 1);
    layout->addWidget(rectangleWidget, 0, 1, 2, 1);

    setAutoFillBackground(true);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *resetAction = new QAction(tr("Reset position"), this);
    addAction(resetAction);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(onResetPosition()));

    QAction *disableAction = new QAction(tr("Disable permanently"), this);
    addAction(disableAction);
    connect(disableAction, SIGNAL(triggered()), this, SLOT(onDisable())); 
}

ContextPaneWidget::~ContextPaneWidget()
{
    //if the pane was never activated the widget is not in a widget tree
    if (!m_bauhausColorDialog.isNull())
        delete m_bauhausColorDialog.data();
        m_bauhausColorDialog.clear();
}

void ContextPaneWidget::activate(const QPoint &pos, const QPoint &alternative, const QPoint &alternative2)
{
    //uncheck all color buttons
    foreach (ColorButton *colorButton, findChildren<ColorButton*>()) {
            colorButton->setChecked(false);
    }
    show();
    update();
    resize(sizeHint());
    show();
    rePosition(pos, alternative, alternative2);
    raise();
}

void ContextPaneWidget::rePosition(const QPoint &position, const QPoint &alternative, const QPoint &alternative2)
{
    if ((position.x()  + width()) < parentWidget()->width())
        move(position);
    else
        move(alternative);

    if (pos().y() < 0)
        move(alternative2);
    if ((pos().y() + height()) > parentWidget()->height())
        hide();

    m_originalPos = pos();

    if (m_pos.x() > 0) {
        move(m_pos);
        show();
    }
}

void ContextPaneWidget::deactivate()
{
    hide();
    if (m_bauhausColorDialog)
        m_bauhausColorDialog->hide();
}

BauhausColorDialog *ContextPaneWidget::colorDialog()
{
    if (m_bauhausColorDialog.isNull()) {
        m_bauhausColorDialog = new BauhausColorDialog(parentWidget());
        m_bauhausColorDialog->hide();
        setSecondaryTarget(m_bauhausColorDialog.data());
    }

    return m_bauhausColorDialog.data();
}

void ContextPaneWidget::setProperties(::QmlJS::PropertyReader *propertyReader)
{
    ContextPaneTextWidget *textWidget = qobject_cast<ContextPaneTextWidget*>(m_currentWidget);
    if (textWidget)
        textWidget->setProperties(propertyReader);

    EasingContextPane *easingWidget = qobject_cast<EasingContextPane*>(m_currentWidget);
    if (easingWidget)
        easingWidget->setProperties(propertyReader);

    ContextPaneWidgetImage *imageWidget = qobject_cast<ContextPaneWidgetImage*>(m_currentWidget);
    if (imageWidget)
        imageWidget->setProperties(propertyReader);

    ContextPaneWidgetRectangle *rectangleWidget = qobject_cast<ContextPaneWidgetRectangle*>(m_currentWidget);
    if (rectangleWidget)
        rectangleWidget->setProperties(propertyReader);
}

void ContextPaneWidget::setPath(const QString &path)
{
    ContextPaneWidgetImage *imageWidget = qobject_cast<ContextPaneWidgetImage*>(m_currentWidget);
    if (imageWidget)
        imageWidget->setPath(path);

}

bool ContextPaneWidget::setType(const QString &typeName)
{
    m_imageWidget->hide();
    m_textWidget->hide();
    m_rectangleWidget->hide();
    m_easingWidget->hide();

    if (typeName.contains("Text")) {
        m_currentWidget = m_textWidget;
        m_textWidget->show();
        m_textWidget->setStyleVisible(true);
        m_textWidget->setVerticalAlignmentVisible(true);
        if (typeName.contains("TextInput")) {
            m_textWidget->setVerticalAlignmentVisible(false);
            m_textWidget->setStyleVisible(false);
        } else if (typeName.contains("TextEdit")) {
            m_textWidget->setStyleVisible(false);
        }
        resize(sizeHint());
        return true;
    }

    if (m_easingWidget->acceptsType(typeName)) {
        m_currentWidget = m_easingWidget;
        m_easingWidget->show();
        resize(sizeHint());
        return true;
    }
    if (typeName.contains("Rectangle")) {
        m_currentWidget = m_rectangleWidget;
        m_rectangleWidget->show();
        resize(sizeHint());
        return true;
    }
    if (typeName.contains("Image")) {
        m_currentWidget = m_imageWidget;
        m_imageWidget->show();
        resize(sizeHint());
        return true;
    }
    return false;
}

bool ContextPaneWidget::acceptsType(const QString &typeName)
{
    return typeName.contains("Text") || m_easingWidget->acceptsType(typeName) ||
            typeName.contains("Rectangle") || typeName.contains("Image");
}

void ContextPaneWidget::onTogglePane()
{
    if (!m_currentWidget)
        return;
    deactivate();
}

void ContextPaneWidget::onShowColorDialog(bool checked, const QPoint &p)
{    
    if (checked) {
        colorDialog()->setParent(parentWidget());
        colorDialog()->move(p);
        colorDialog()->show();
        colorDialog()->raise();
    } else {
        colorDialog()->hide();
    }
}

void ContextPaneWidget::onDisable()
{       
    DesignerSettings designerSettings = Internal::BauhausPlugin::pluginInstance()->settings();
    designerSettings.enableContextPane = false;
    Internal::BauhausPlugin::pluginInstance()->setSettings(designerSettings);
    hide();
    colorDialog()->hide();
}

void  ContextPaneWidget::onResetPosition()
{
    move(m_originalPos);
    m_pos = QPoint(-1,-1);
}

QWidget* ContextPaneWidget::createFontWidget()
{    
    m_textWidget = new ContextPaneTextWidget(this);
    connect(m_textWidget, SIGNAL(propertyChanged(QString,QVariant)), this, SIGNAL(propertyChanged(QString,QVariant)));
    connect(m_textWidget, SIGNAL(removeProperty(QString)), this, SIGNAL(removeProperty(QString)));
    connect(m_textWidget, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)), this, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)));

    return m_textWidget;
}

QWidget* ContextPaneWidget::createEasingWidget()
{
    m_easingWidget = new EasingContextPane(this);

    connect(m_easingWidget, SIGNAL(propertyChanged(QString,QVariant)), this, SIGNAL(propertyChanged(QString,QVariant)));
    connect(m_easingWidget, SIGNAL(removeProperty(QString)), this, SIGNAL(removeProperty(QString)));
    connect(m_easingWidget, SIGNAL(removeAndChangeProperty(QString,QString,QVariant,bool)), this, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)));

    return m_easingWidget;
}

QWidget *ContextPaneWidget::createImageWidget()
{
    m_imageWidget = new ContextPaneWidgetImage(this);
    connect(m_imageWidget, SIGNAL(propertyChanged(QString,QVariant)), this, SIGNAL(propertyChanged(QString,QVariant)));
    connect(m_imageWidget, SIGNAL(removeProperty(QString)), this, SIGNAL(removeProperty(QString)));
    connect(m_imageWidget, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)), this, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)));

    return m_imageWidget;
}

QWidget *ContextPaneWidget::createRectangleWidget()
{
    m_rectangleWidget = new ContextPaneWidgetRectangle(this);
    connect(m_rectangleWidget, SIGNAL(propertyChanged(QString,QVariant)), this, SIGNAL(propertyChanged(QString,QVariant)));
    connect(m_rectangleWidget, SIGNAL(removeProperty(QString)), this, SIGNAL(removeProperty(QString)));
    connect(m_rectangleWidget, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)), this, SIGNAL(removeAndChangeProperty(QString,QString,QVariant, bool)));

    return m_rectangleWidget;
}


} //QmlDesigner


