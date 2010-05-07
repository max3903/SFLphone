#ifndef CALL_VIEW
#define CALL_VIEW

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QItemDelegate>

#include "lib/CallModel.h"
#include "lib/sflphone_const.h"
#include "lib/callmanager_interface_singleton.h"

class CallTreeItemDelegate : public QItemDelegate
{
   public:
      CallTreeItemDelegate() { }
      QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const 
      {  
         Q_UNUSED(option)
         Q_UNUSED(index)
         return QSize(0,60); 
      }
};

class CallView : private QTreeWidget, public CallModel<InternalCallModelStruct> {
   Q_OBJECT
   public:
      CallView(ModelType type, QWidget* parent =0);
      bool selectItem(Call* item);
      Call* getCurrentItem();
      bool removeItem(Call* item);
      QWidget* getWidget();
      void setTitle(QString title);
      bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
      QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
      virtual Call* addCall(Call* call, Call* parent =0);
      virtual Call* addConference(const QString &confID);
      virtual bool conferenceChanged(const QString &confId, const QString &state);
      virtual void conferenceRemoved(const QString &confId);
      
   private:
      QTreeWidgetItem* extractItem(QString callId);
      QTreeWidgetItem* extractItem(QTreeWidgetItem* item);
      CallTreeItem* insertItem(QTreeWidgetItem* item, QTreeWidgetItem* parent=0);
      CallTreeItem* insertItem(QTreeWidgetItem* item, Call* parent);
      void clearArtefact(QTreeWidgetItem* item);

   protected:
      void dragEnterEvent(QDragEnterEvent *e) { e->accept(); }
      void dragMoveEvent(QDragMoveEvent *e) { e->accept(); }
      QHash<CallTreeItem* , InternalCallModelStruct*> privateCallList_widget;
      QHash<QTreeWidgetItem* , InternalCallModelStruct*> privateCallList_item;
      
   private slots:
      void destroyCall(Call* toDestroy);
      void itemDoubleClicked(QTreeWidgetItem* item, int column);
      void itemClicked(QTreeWidgetItem* item, int column);
      
   public slots:
      void clearHistory();

   signals:
      void itemChanged(Call*);
      
};
#endif
