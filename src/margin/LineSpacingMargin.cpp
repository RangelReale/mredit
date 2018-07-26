#include "mredit/margin/LineSpacingMargin.h"
#include "mredit/Editor.h"

#include <QPainter>

namespace mredit {
namespace margin {

LineSpacingMargin::LineSpacingMargin( MarginStacker* marginStacker )
    : AbstractMargin( marginStacker )
{
    updateWidthRequested();
    setMouseTracking( false );
}

LineSpacingMargin::~LineSpacingMargin()
{
}

Global::Margin LineSpacingMargin::margin_type() const
{
	return Global::Margin::SpaceMargin;
}

void LineSpacingMargin::paintEvent( QPaintEvent* event )
{
    QWidget::paintEvent( event );
    
    const Editor* editor = this->editor();
    const QColor color = editor ? editor->palette().color( editor->viewport()->backgroundRole() ) : palette().color( backgroundRole() );
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, false );
    painter.fillRect( event->rect(), color );
}

void LineSpacingMargin::updateWidthRequested()
{
    setMinimumWidth( 2 );
}

} }