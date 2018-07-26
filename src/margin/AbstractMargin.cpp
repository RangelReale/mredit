#include "mredit/margin/AbstractMargin.h"
#include "mredit/margin/MarginStacker.h"
#include "mredit/Editor.h"
#include "mredit/TextDocument.h"
#include "mredit/PlainTextDocumentLayout.h"
#include "mredit/TextBlockUserData.h"

#include <QScrollBar>
#include <QApplication>
#include <QDebug>

namespace mredit {
namespace margin {

// AbstractMarginPrivate

class AbstractMarginPrivate {
public:
    AbstractMargin* margin;
    MarginStacker* stacker;
	int margin_id;
    int line;
    int linePressed;
    
    AbstractMarginPrivate( AbstractMargin* _margin, MarginStacker* marginStacker )
        : margin( _margin ), stacker( marginStacker ),
			margin_id(-1), line( -1 ), linePressed( -1 )
    {
        Q_ASSERT( margin );
        Q_ASSERT( marginStacker );
    }
    
    int lineAt( const QPoint& pos ) const {
        const Editor* editor = stacker->editor();
        return editor ? editor->cursorForPosition( pos ).blockNumber() : -1;
    }
    
    QRect blockRect( const QTextBlock& block ) const {
        const Editor* editor = stacker->editor();
        QRect rect;
        
        if ( editor ) {
            rect = editor->blockRect( block );
            rect.setWidth( margin->width() );
        }
        
        return rect;
    }
    
    QRect lineRect( int line ) const {
        const Editor* editor = stacker->editor();
        const TextDocument* document = editor ? editor->textDocument() : 0;
        return document ? blockRect( document->findBlockByNumber( line ) ) : QRect();
    }
};

// AbstractMargin

AbstractMargin::AbstractMargin( MarginStacker* marginStacker )
    : QWidget( 0 ),
        d( new AbstractMarginPrivate( this, marginStacker ) )
{
    Q_ASSERT( marginStacker );
    
    setMouseTracking( true );
}

AbstractMargin::~AbstractMargin()
{
    delete d;
}

int AbstractMargin::marginId() const
{
	return d->margin_id;
}

void AbstractMargin::setMarginId(int margin_id)
{
	d->margin_id = margin_id;
}

Editor* AbstractMargin::editor() const
{
    return d->stacker->editor();
}

void AbstractMargin::setEditor( Editor* editor )
{
    Editor* oldEditor = this->editor();
    
    if ( oldEditor ) {
        disconnect( oldEditor->textDocument()->layout(), SIGNAL( update( const QRectF& ) ), this, SLOT( update() ) );
        disconnect( oldEditor, SIGNAL( cursorPositionChanged() ), this, SLOT( update() ) );
        disconnect( oldEditor->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( update() ) );
        disconnect( oldEditor, SIGNAL( blockCountChanged( int ) ), this, SLOT( update() ) );
        disconnect( oldEditor, SIGNAL( blockCountChanged( int ) ), this, SIGNAL( lineCountChanged( int ) ) );
    }
    
    if ( editor ) {
        connect( editor->textDocument()->layout(), SIGNAL( update( const QRectF& ) ), this, SLOT( update() ) );
        connect( editor, SIGNAL( cursorPositionChanged() ), this, SLOT( update() ) );
        connect( editor->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( update() ) );
        connect( editor, SIGNAL( blockCountChanged( int ) ), this, SLOT( update() ) );
        connect( editor, SIGNAL( blockCountChanged( int ) ), this, SIGNAL( lineCountChanged( int ) ) );
    }
    
    updateWidthRequested();
}

MarginStacker* AbstractMargin::stacker() const
{
    return d->stacker;
}

int AbstractMargin::lineAt( const QPoint& pos ) const
{
    return d->lineAt( pos );
}

QRect AbstractMargin::blockRect( const QTextBlock& block ) const
{
    return d->blockRect( block );
}

QRect AbstractMargin::lineRect( int line ) const
{
    return d->lineRect( line );
}

int AbstractMargin::firstVisibleLine( const QRect& rect ) const
{
    const Editor* editor = this->editor();
    const QRect r = rect.isNull() ? ( editor ? editor->viewport()->rect() : rect ) : rect;
    return d->lineAt( r.topLeft() );
}

int AbstractMargin::lastVisibleLine( const QRect& rect ) const
{
    const Editor* editor = this->editor();
    const QRect r = rect.isNull() ? ( editor ? editor->viewport()->rect() : rect ) : rect;
    return d->lineAt( r.bottomLeft() );
}

void AbstractMargin::updateLineRect( int line )
{
    update( lineRect( line ) );
}

bool AbstractMargin::event( QEvent* event )
{
    const bool result = QWidget::event( event );
    
    switch ( event->type() ) {
        case QEvent::FontChange:
            emit fontChanged();
            break;
        case QEvent::Resize:
            emit resized();
            break;
        default:
            break;
    }
    
    return result;
}

void AbstractMargin::mousePressEvent( QMouseEvent* event )
{
    QWidget::mousePressEvent( event );
    d->linePressed = d->lineAt( event->pos() );
}

void AbstractMargin::mouseDoubleClickEvent( QMouseEvent* event )
{
    QWidget::mouseDoubleClickEvent( event );
    
    if ( d->line != -1 ) {
        emit mouseDoubleClicked( d->line, event->button(), event->buttons(), event->modifiers() );
    }
}

void AbstractMargin::mouseReleaseEvent( QMouseEvent* event )
{
    QWidget::mouseReleaseEvent( event );
    
    if ( d->linePressed != -1 && d->linePressed == d->lineAt( event->pos() ) ) {
        emit mouseClicked( d->line, event->button(), event->buttons(), event->modifiers() );
    }
    
    d->linePressed = -1;
}

void AbstractMargin::mouseMoveEvent( QMouseEvent* event )
{
    QWidget::mouseMoveEvent( event );
    
    const int line = d->lineAt( event->pos() );
    
    if ( line == d->line ) {
        return;
    }
    
    if ( d->line != -1 ) {
        emit mouseLeft( d->line );
    }
    
    d->line = line;
    
    if ( d->line != -1 ) {
        emit mouseEntered( d->line );
    }
}

void AbstractMargin::enterEvent( QEvent* event )
{
    QWidget::enterEvent( event );
    
    const QPoint pos = QCursor::pos();
    QMouseEvent me( QEvent::MouseMove, mapFromGlobal( pos ), pos, Qt::NoButton, QApplication::mouseButtons(), QApplication::keyboardModifiers() );
    
    mouseMoveEvent( &me );
}

void AbstractMargin::leaveEvent( QEvent* event )
{
    QWidget::leaveEvent( event );
    
    const int line = -1;
    
    if ( line == d->line ) {
        return;
    }
    
    if ( d->line != -1 ) {
        emit mouseLeft( d->line );
    }
    
    d->line = line;
}

} }