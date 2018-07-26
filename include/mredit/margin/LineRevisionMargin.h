#pragma once

#include "mredit/margin/AbstractMargin.h"

namespace mredit {
namespace margin {

class LineRevisionMargin : public AbstractMargin
{
    Q_OBJECT
    
public:
    LineRevisionMargin( MarginStacker* marginStacker );
    virtual ~LineRevisionMargin();

	Global::Margin margin_type() const;
protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void setEditor( Editor* editor );

protected slots:
    virtual void updateWidthRequested();
};

} }
