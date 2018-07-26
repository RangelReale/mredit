#pragma once

#include "mredit/Global.h"

#include <QWidget>

class QTextBlock;

namespace mredit {

class Editor;

namespace margin {

class AbstractMarginPrivate;
class MarginStacker;

class AbstractMargin : public QWidget
{
    Q_OBJECT
    friend class AbstractMarginPrivate;
    friend class MarginStacker;
    friend class MarginStackerPrivate;
    
public:
    AbstractMargin( MarginStacker* marginStacker );
    virtual ~AbstractMargin();
    
	virtual Global::Margin margin_type() const = 0;
	int marginId() const;
	void setMarginId(int margin_id);
    Editor* editor() const;
    MarginStacker* stacker() const;
    
    int lineAt( const QPoint& pos ) const;
    QRect blockRect( const QTextBlock& block ) const;
    QRect lineRect( int line ) const;
    
    int firstVisibleLine( const QRect& rect = QRect() ) const;
    int lastVisibleLine( const QRect& rect = QRect() ) const;

public slots:
    void updateLineRect( int line );

protected:
    virtual bool event( QEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void enterEvent( QEvent* event );
    virtual void leaveEvent( QEvent* event );
    
    virtual void setEditor( Editor* editor );

protected slots:
    virtual void updateWidthRequested() = 0;

private:
    AbstractMarginPrivate* d;

signals:
    void mouseClicked( int line, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers );
    void mouseDoubleClicked( int line, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers );
    void mouseEntered( int line );
    void mouseLeft( int line );
    void lineCountChanged( int count );
    void fontChanged();
    void resized();
};

} }
