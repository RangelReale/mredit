#include "mredit/BookmarkGroup.h"

namespace mredit {

// BookmarkPrivate

class BookmarkGroupPrivate : public QObject {
    Q_OBJECT

public:
	QString title;
	QIcon icon;
	QTextBlockFormat format;

	BookmarkGroupPrivate(const QString &title, const QIcon &icon, BookmarkGroup* bookmark )
        : QObject( bookmark ),
			title(title), icon(icon),
            bookmark( bookmark )
    {
        Q_ASSERT( bookmark );
    }

private:
	BookmarkGroup * bookmark;
};

// BookmarkCollectionPrivate

class BookmarkGroupListPrivate : public QObject {
	Q_OBJECT

public:
	QMap<int, BookmarkGroup*> groups;
	int group_next_id;

	BookmarkGroupListPrivate(BookmarkGroupList* bglist)
		: QObject(bglist),
		bglist(bglist), 
		groups(), group_next_id(1)
	{
		Q_ASSERT(bglist);
	}

	int addGroup(const QString &title, const QIcon &icon)
	{
		int cur_id = group_next_id++;
		groups[cur_id] = new BookmarkGroup(title, icon, this);
		return cur_id;
	}

private:
	BookmarkGroupList *bglist;
};

// BookmarkGroup

BookmarkGroup::BookmarkGroup(const QString &title, const QIcon &icon, QObject *parent) :
	QObject(parent), d(new BookmarkGroupPrivate(title, icon, this))
{

}

BookmarkGroup::~BookmarkGroup()
{

}

const QString &BookmarkGroup::title() const
{
	return d->title;
}

void BookmarkGroup::setTitle(const QString &title)
{
	d->title = title;
}

const QIcon &BookmarkGroup::icon() const
{
	return d->icon;
}

void BookmarkGroup::setIcon(const QIcon &icon)
{
	d->icon = icon;
}

QTextBlockFormat BookmarkGroup::format() const
{
	return d->format;
}

void BookmarkGroup::setTextBlockFormat(QTextBlockFormat format)
{
	d->format = format;
}

// BookmarkCollection

BookmarkGroupList::BookmarkGroupList(QObject *parent) :
	QObject(parent), d(new BookmarkGroupListPrivate(this))
{

}

BookmarkGroupList::~BookmarkGroupList()
{

}

int BookmarkGroupList::addGroup(const QString &title, const QIcon &icon)
{
	return d->addGroup(title, icon);
}

BookmarkGroup *BookmarkGroupList::getGroup(int bmid) const
{
	if (d->groups.contains(bmid))
		return d->groups[bmid];
	return nullptr;
}

}

#include "BookmarkGroup.moc"