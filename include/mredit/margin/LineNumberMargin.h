#pragma once

#include "mredit/margin/AbstractMargin.h"

namespace mredit {
namespace margin {

class LineNumberMargin : public AbstractMargin
{
    Q_OBJECT
    
public:
    LineNumberMargin( MarginStacker* marginStacker );
    virtual ~LineNumberMargin();

	Global::Margin margin_type() const;
protected:
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent* event );
    virtual void paintEvent( QPaintEvent* event );

protected slots:
    virtual void updateWidthRequested();
};

} }
