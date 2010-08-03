#include "CallView.h"

///Retrieve current and older calls from the daemon, fill history and the calls TreeView and enable drag n' drop
CallView::CallView(ModelType type, QWidget* parent) : QTreeWidget(parent), CallModel<InternalCallModelStruct>(type)
{
   if (type == ActiveCall)
      initCall();
   else if (type == History)
      initHistory();
   
   //Widget part
   setAcceptDrops(true);
   setDragEnabled(true);
   CallTreeItemDelegate *delegate = new CallTreeItemDelegate();
   setItemDelegate(delegate); 

   //Events
   connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
   connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));
}


/*****************************************************************************
 *                                                                           *
 *                        Drag and drop related code                         *
 *                                                                           *
 ****************************************************************************/

///Action performed when an item is dropped on the TreeView
bool CallView::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) 
{
   Q_UNUSED(index)
   Q_UNUSED(action)
   
   QByteArray encodedData = data->data(MIME_CALLID);
   
   if (!QString(encodedData).isEmpty()) {
   clearArtefact(privateCallList_callId[encodedData]->treeItem);
   
      if (!parent) {
         qDebug() << "Call dropped on empty space";
         if (privateCallList_callId[encodedData]->treeItem->parent()) {
            qDebug() << "Detaching participant";
            detachParticipant(privateCallList_callId[encodedData]->call_real);
         }
         else
            qDebug() << "The call is not in a conversation (doing nothing)";
         return true;
      }
      
      if (privateCallList_item[parent]->call_real->getCallId() == QString(encodedData)) {
         qDebug() << "Call dropped on itself (doing nothing)";
         return true;
      }
      
      if ((parent->childCount()) && (privateCallList_callId[encodedData]->treeItem->childCount())) {
         qDebug() << "Merging two conferences";
         mergeConferences(privateCallList_item[parent]->call_real,privateCallList_callId[encodedData]->call_real);
         return true;
      }
      else if ((parent->parent()) || (parent->childCount())) {
         qDebug() << "Call dropped on a conference";
         
         if ((privateCallList_callId[encodedData]->treeItem->childCount()) && (!parent->childCount())) {
            qDebug() << "Conference dropped on a call (doing nothing)";
            return true;
         }
         
         QTreeWidgetItem* call1 = privateCallList_callId[encodedData]->treeItem;
         QTreeWidgetItem* call2 = (parent->parent())?parent->parent():parent;
         
         if (call1->parent()) {
            qDebug() << "Call 1 is part of a conference";
            if (call1->parent() == call2) {
               qDebug() << "Call dropped on it's own conference (doing nothing)";
               return true;
            }
            else if (privateCallList_item[call1]->treeItem->childCount()) {
               qDebug() << "Merging two conferences";
               mergeConferences(privateCallList_item[call1]->call_real,privateCallList_item[call2]->call_real);
            }
            else if (call1->parent()) {
               qDebug() << "Moving call from a conference to an other";
               detachParticipant(privateCallList_callId[encodedData]->call_real);
            }
         }
         qDebug() << "Adding participant";
         addParticipant(privateCallList_item[call1]->call_real,privateCallList_item[call2]->call_real);
         return true;
      }
      

      
      qDebug() << "Call dropped on another call";
      createConferenceFromCall(privateCallList_callId[encodedData]->call_real,privateCallList_item[parent]->call_real);
      return true;
   }
   
   return false;
}

///Encode data to be tranported during the drag n' drop operation
QMimeData* CallView::mimeData(const QList<QTreeWidgetItem *> items) const 
{   
   QMimeData *mimeData = new QMimeData();
   
   //Call ID for internal call merging and spliting
   if (privateCallList_item[items[0]]->call_real->isConference())
      mimeData->setData(MIME_CALLID, privateCallList_item[items[0]]->call_real->getConfId().toAscii());
   else
      mimeData->setData(MIME_CALLID, privateCallList_item[items[0]]->call_real->getCallId().toAscii());
   
   //Plain text for other applications
   mimeData->setData(MIME_PLAIN_TEXT, QString(privateCallList_item[items[0]]->call_real->getPeerName()+"\n"+privateCallList_item[items[0]]->call_real->getPeerPhoneNumber()).toAscii());
   
   //TODO Comment this line if you don't want to see ugly artefact, but the caller details will not be visible while dragged
   items[0]->setText(0, privateCallList_item[items[0]]->call_real->getPeerName() + "\n" + privateCallList_item[items[0]]->call_real->getPeerPhoneNumber());
   return mimeData;
}


/*****************************************************************************
 *                                                                           *
 *                            Call related code                              *
 *                                                                           *
 ****************************************************************************/

///Add a call in the model structure, the call must exist before being added to the model
Call* CallView::addCall(Call* call, Call* parent) 
{
   InternalCallModelStruct* aNewStruct = privateCallList_call[CallModel<InternalCallModelStruct>::addCall(call, parent)];
   
   QTreeWidgetItem* callItem = new QTreeWidgetItem();
   aNewStruct->treeItem = callItem;
   
   privateCallList_item[callItem] = aNewStruct;
   
   aNewStruct->call = insertItem(callItem,parent);
   privateCallList_widget[aNewStruct->call] = aNewStruct;
   
   setCurrentItem(callItem);
   
   connect(call, SIGNAL(isOver(Call*)), this, SLOT(destroyCall(Call*)));
   return call;
}

/*****************************************************************************
 *                                                                           *
 *                            View related code                              *
 *                                                                           *
 ****************************************************************************/

///Set the TreeView header text
void CallView::setTitle(QString title) 
{
   headerItem()->setText(0,title);
}

///Select an item in the TreeView
bool CallView::selectItem(Call* item) 
{
   if (privateCallList_call[item]->treeItem) {
      setCurrentItem(privateCallList_call[item]->treeItem);
      return true;
   }
   else
      return false;
}

///Return the current item
Call* CallView::getCurrentItem() 
{
   if (currentItem() && privateCallList_item[QTreeWidget::currentItem()])
      return privateCallList_item[QTreeWidget::currentItem()]->call_real;
   else
      return false;
}

///Remove a TreeView item and delete it
bool CallView::removeItem(Call* item) 
{
   if (indexOfTopLevelItem(privateCallList_call[item]->treeItem) != -1) {//TODO To remove once safe
     removeItemWidget(privateCallList_call[item]->treeItem,0);
     return true;
   }
   else
      return false;
}

///Return the TreeView, this
QWidget* CallView::getWidget() 
{
   return this;
}

///Convenience wrapper around extractItem(QTreeWidgetItem*)
QTreeWidgetItem* CallView::extractItem(QString callId) 
{
   QTreeWidgetItem* currentItem = privateCallList_callId[callId]->treeItem;
   return extractItem(currentItem);
}

///Extract an item from the TreeView and return it, the item is -not- deleted
QTreeWidgetItem* CallView::extractItem(QTreeWidgetItem* item) 
{
   QTreeWidgetItem* parentItem = item->parent();
   
   if (parentItem) {
      if ((indexOfTopLevelItem(parentItem) == -1 ) || (parentItem->indexOfChild(item) == -1)) {
         qDebug() << "The conversation does not exist";
         return 0;
      }
      
      QTreeWidgetItem* toReturn = parentItem->takeChild(parentItem->indexOfChild(item));

      return toReturn;
   }
   else
      return takeTopLevelItem(indexOfTopLevelItem(item));
}

///Convenience wrapper around insertItem(QTreeWidgetItem*, QTreeWidgetItem*)
CallTreeItem* CallView::insertItem(QTreeWidgetItem* item, Call* parent) 
{
   return insertItem(item,(parent)?privateCallList_call[parent]->treeItem:0);
}

///Insert a TreeView item in the TreeView as child of parent or as a top level item, also restore the item Widget
CallTreeItem* CallView::insertItem(QTreeWidgetItem* item, QTreeWidgetItem* parent) 
{
   if (!item) {
      qDebug() << "This is not a valid call";
      return 0;
   }
   
   if (!parent)
      insertTopLevelItem(0,item);
   else
      parent->addChild(item);
   
   privateCallList_widget.remove(privateCallList_item[item]->call);
   privateCallList_item[item]->call = new CallTreeItem();
   privateCallList_item[item]->call->setCall(privateCallList_item[item]->call_real);
   privateCallList_widget[privateCallList_item[item]->call] = privateCallList_item[item];
   
   setItemWidget(item,0,privateCallList_item[item]->call);
   
   expandAll();
   return privateCallList_item[item]->call;
}

///Remove a call from the interface
void CallView::destroyCall(Call* toDestroy) 
{
   if (privateCallList_call[toDestroy]->treeItem == currentItem())
      setCurrentItem(0);
   
   if (!privateCallList_call[toDestroy])
       qDebug() << "Call not found";
   else if (indexOfTopLevelItem(privateCallList_call[toDestroy]->treeItem) != -1)
      takeTopLevelItem(indexOfTopLevelItem(privateCallList_call[toDestroy]->treeItem));
   else if (privateCallList_call[toDestroy]->treeItem->parent()) //May crash here
      privateCallList_call[toDestroy]->treeItem->parent()->removeChild(privateCallList_call[toDestroy]->treeItem);
   else
      qDebug() << "Call not found";
}

/// @todo Remove the text partially covering the TreeView item widget when it is being dragged, a beter implementation is needed
void CallView::clearArtefact(QTreeWidgetItem* item) 
{
   item->setText(0,"");
}


/*****************************************************************************
 *                                                                           *
 *                           Event related code                              *
 *                                                                           *
 ****************************************************************************/

void CallView::itemDoubleClicked(QTreeWidgetItem* item, int column) {
   Q_UNUSED(column)
   qDebug() << "Item doubleclicked";
   switch(privateCallList_item[item]->call_real->getState()) {
      case CALL_STATE_HOLD:
         privateCallList_item[item]->call_real->actionPerformed(CALL_ACTION_HOLD);
         break;
      case CALL_STATE_DIALING:
         privateCallList_item[item]->call_real->actionPerformed(CALL_ACTION_ACCEPT);
         break;
      default:
         qDebug() << "Double clicked an item with no action on double click.";
    }
}

void CallView::itemClicked(QTreeWidgetItem* item, int column) {
   Q_UNUSED(column)
   emit itemChanged(privateCallList_item[item]->call_real);
   qDebug() << "Item clicked";
}


/*****************************************************************************
 *                                                                           *
 *                         Conference related code                           *
 *                                                                           *
 ****************************************************************************/

///Add a new conference, get the call list and update the interface as needed
Call* CallView::addConference(const QString & confID) 
{
   qDebug() << "Conference created";
   Call* newConf =  CallModel<InternalCallModelStruct>::addConference(confID);

   InternalCallModelStruct* aNewStruct = privateCallList_callId[confID];
   
   if (!aNewStruct) {
      qDebug() << "Conference failed";
      return 0;
   }
   
   QTreeWidgetItem* confItem = new QTreeWidgetItem();
   aNewStruct->treeItem = confItem;
   
   privateCallList_item[confItem] = aNewStruct;
   
   aNewStruct->call = insertItem(confItem,(QTreeWidgetItem*)0);
   privateCallList_widget[aNewStruct->call] = aNewStruct;
   
   setCurrentItem(confItem);

   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   QStringList callList = callManager.getParticipantList(confID);
   
   foreach (QString callId, callList) {
     insertItem(extractItem(privateCallList_callId[callId]->treeItem),confItem);
   }
   
   Q_ASSERT_X(confItem->childCount() == 0, "add conference","Conference created, but without any participants");
   return newConf;
}

///Executed when the daemon signal a modification in an existing conference. Update the call list and update the TreeView
bool CallView::conferenceChanged(const QString &confId, const QString &state) 
{
   qDebug() << "Conference changed";
   if (!CallModel<InternalCallModelStruct>::conferenceChanged(confId, state))
     return false;

   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   QStringList callList = callManager.getParticipantList(confId);

   QList<QTreeWidgetItem*> buffer;
   foreach (QString callId, callList) {
      if (privateCallList_callId[callId]) {
         QTreeWidgetItem* item3 = extractItem(privateCallList_callId[callId]->treeItem);
         insertItem(item3, privateCallList_callId[confId]->treeItem);
         buffer << privateCallList_callId[callId]->treeItem;
      }
      else
         qDebug() << "Call " << callId << " does not exist";
   }

   for (int j =0; j < privateCallList_callId[confId]->treeItem->childCount();j++) {
      if (buffer.indexOf(privateCallList_callId[confId]->treeItem->child(j)) == -1)
         insertItem(extractItem(privateCallList_callId[confId]->treeItem->child(j)));
   }
   
   Q_ASSERT_X(privateCallList_callId[confId]->treeItem->childCount() == 0,"changind conference","A conference can't have no participants");
   
   return true;
}

///Remove a conference from the model and the TreeView
void CallView::conferenceRemoved(const QString &confId) 
{
   CallModel<InternalCallModelStruct>::conferenceRemoved(confId);
   while (privateCallList_callId[confId]->treeItem->childCount()) {
      insertItem(extractItem(privateCallList_callId[confId]->treeItem->child(0)));
   }
   privateCallList_widget.remove(privateCallList_callId[confId]->call);
   privateCallList_item.remove(privateCallList_callId[confId]->treeItem);
   takeTopLevelItem(indexOfTopLevelItem(privateCallList_callId[confId]->treeItem));
   delete privateCallList_callId[confId]->treeItem;
   privateCallList_callId.remove(confId);
   qDebug() << "Conference removed";
}

///Clear the list of old calls //TODO Clear them from the daemon
void CallView::clearHistory()
{
   historyCalls.clear();
}
