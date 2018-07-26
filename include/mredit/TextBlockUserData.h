#pragma once

#include <QTextBlockUserData>
#include <QSet>

namespace mredit {

class TextBlockUserData : public QTextBlockUserData
{
public:
    QSet<int> bookmarkgroups;
    
    TextBlockUserData();
    virtual ~TextBlockUserData();
};

}