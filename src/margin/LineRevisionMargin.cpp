#include "mredit/margin/LineRevisionMargin.h"
#include "mredit/Editor.h"
#include "mredit/TextDocument.h"

#include <QPainter>
#include <QTextBlock>
#include <QDebug>

namespace mredit {
namespace margin {

LineRevisionMargin::LineRevisionMargin( MarginStacker* marginStacker )
    : AbstractMargin( marginStacker )
{
    updateWidthRequested();
    setMouseTracking( false );
}

LineRevisionMargin::~LineRevisionMargin()
{
}

Global::Margin LineRevisionMargin::margin_type() const
{
	return Global::Margin::RevisionMargin;
}

void LineRevisionMargin::setEditor( Editor* editor )
{
    Editor* oldEditor = this->editor();
    
    if ( oldEditor ) {
        disconnect( oldEditor->textDocument(), SIGNAL( contentsChanged() ), this, SLOT( update() ) );
        disconnect( oldEditor->textDocument(), SIGNAL( modificationChanged( bool ) ), this, SLOT( update() ) );
    }
    
    AbstractMargin::setEditor( editor );
    
    if ( editor ) {
        connect( editor->textDocument(), SIGNAL( contentsChanged() ), this, SLOT( update() ) );
        connect( editor->textDocument(), SIGNAL( modificationChanged( bool ) ), this, SLOT( update() ) );
    }
}

void LineRevisionMargin::paintEvent( QPaintEvent* event )
{
    AbstractMargin::paintEvent( event );
    
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, false );
    painter.setBrush( Qt::NoBrush );
    
    const int firstLine = firstVisibleLine( event->rect() );
    const int lastLine = lastVisibleLine( event->rect() );
    const TextDocument* document = editor()->textDocument();
    
    for ( QTextBlock block = document->findBlockByNumber( firstLine ); block.isValid() && block.blockNumber() <= lastLine; block = block.next() ) {
        const QRect rect = blockRect( block );
        
        if ( block.revision() != document->lastUnmodifiedRevision() ) {
            if ( block.revision() < 0 ) {
                painter.setPen( QPen( QColor( Qt::darkGreen ), minimumWidth() ) );
            }
            else {
                painter.setPen( QPen( QColor( Qt::red ), minimumWidth() ) );
            }
            
            painter.drawLine( rect.topRight(), rect.bottomRight() );
        }
    }
}

void LineRevisionMargin::updateWidthRequested()
{
    setMinimumWidth( 2 );
}

} }