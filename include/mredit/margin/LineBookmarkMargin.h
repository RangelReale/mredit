#pragma once

#include "mredit/margin/AbstractMargin.h"

namespace mredit {
namespace margin {

class LineBookmarkMarginPrivate;

class LineBookmarkMargin : public AbstractMargin
{
    Q_OBJECT
    friend class LineBookmarkMarginPrivate;
    
public:
    LineBookmarkMargin( MarginStacker* marginStacker );
    virtual ~LineBookmarkMargin();

	Global::Margin margin_type() const;

	void clearBookmarkGroups();
	void addBookmarkGroup(int bmgroup);

protected:
    virtual void paintEvent( QPaintEvent* event );

protected slots:
    virtual void updateWidthRequested();

private:
    LineBookmarkMarginPrivate* d;
};

} }
