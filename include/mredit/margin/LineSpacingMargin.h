#pragma once

#include "mredit/margin/AbstractMargin.h"

namespace mredit {
namespace margin {

class LineSpacingMargin : public AbstractMargin
{
    Q_OBJECT
    
public:
    LineSpacingMargin( MarginStacker* marginStacker );
    virtual ~LineSpacingMargin();
    
	Global::Margin margin_type() const;
protected:
    virtual void paintEvent( QPaintEvent* event );

protected slots:
    virtual void updateWidthRequested();
};

} }