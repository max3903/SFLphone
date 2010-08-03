/***************************************************************************
 *   Copyright (C) 2009-2010 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/ 

#ifndef CALL_MODEL
#define CALL_MODEL

#include <QObject>
#include <QHash>
#include <QVector>
#include <QTreeWidgetItem>
#include <QDragEnterEvent>
#include <QDebug>

#include "CallTreeItem.h"
#include "Call.h"
#include "AccountList.h"
#include "dbus/metatypes.h"
#include "libsflphonekde_export.h"
#include "callmanager_interface_singleton.h"
#include "configurationmanager_interface_singleton.h"
#include "instance_interface_singleton.h"
#include "sflphone_const.h"
#include "unistd.h"

/** Note from the author: It was previously done by a QAbstractModel + QTreeView, but the sip-call use case is incompatible 
 *  with the MVC model. The MVC never got to a point were it was bug-free and the code was getting dirty. The QTreeWidget
 *  solution may be less "clean" than MVC, but is 3 time smaller and easier to improve (in fact, possible to improve).
 *  
 *  This model is the view itself (private inheritance) so drag and drop can interact directly with the model without cross
 *  layer hack. This call merge the content of 4 previous classes (CallTreeModel, CallTreeView, CallList and most of the 
 *  previous CallTreeItem).
 *  @note This model intend to be reimplemented by the view, not used alone
 *  @note Most of the member are static to preserve ressources and QObject::connect()
 */

struct InternalCallModelStruct {
   //InternalCallModelStruct* parent;
   CallTreeItem* call;
   Call* call_real;
   QTreeWidgetItem* treeItem; //For the view
   QList<InternalCallModelStruct*> children; //For the view
   bool conference;
};

struct InternalCallModelStruct;

template  <typename T>
class LIBSFLPHONEKDE_EXPORT CallModel {
   //Q_OBJECT
   public:
      enum ModelType {
         ActiveCall,
         History,
         Address
      };
      
      CallModel(ModelType type);
      virtual ~CallModel() {}

      virtual Call* addCall(Call* call, Call* parent =0);
      int size();
      Call* findCallByCallId(QString callId);
      QList<Call*> getCallList();
      
      Call* addDialingCall(const QString& peerName="", QString account="");
      Call* addIncomingCall(const QString& callId);
      Call* addRingingCall(const QString& callId);
      bool createConferenceFromCall(Call* call1, Call* call2);
      bool mergeConferences(Call* conf1, Call* conf2);
      bool addParticipant(Call* call2, Call* conference);
      bool detachParticipant(Call* call);
      virtual Call* addConference(const QString &confID);
      virtual bool conferenceChanged(const QString &confId, const QString &state);
      virtual void conferenceRemoved(const QString &confId);
      virtual bool selectItem(Call* item) { Q_UNUSED(item); return false;}
      static QString generateCallId();
      
      MapStringString getHistoryMap();

      //Account related members
      static Account* getCurrentAccount();
      static QString getCurrentAccountId();
      static AccountList* getAccountList();
      static QString getPriorAccoundId();
      static void setPriorAccountId(QString value);

      //Connection related members
      static bool init();
      
      
   protected:
      static QHash<QString, Call*> activeCalls;
      static QHash<QString, Call*> historyCalls;
      static QHash<Call*, T*> privateCallList_call;
      static QHash<QString, T*> privateCallList_callId;
      static QString currentAccountId;
      static QString priorAccountId;
      static AccountList* accountList;
      static bool callInit;
      static bool historyInit;
      virtual bool initCall();
      virtual bool initHistory();

   private:
      static bool instanceInit;
   //public slots:
      //void clearHistory();
};

//Static member
template  <typename T> QString CallModel<T>::priorAccountId;
template  <typename T> AccountList* CallModel<T>::accountList;
template  <typename T> bool CallModel<T>::instanceInit(false);
template  <typename T> bool CallModel<T>::callInit(false);
template  <typename T> bool CallModel<T>::historyInit(false);

template  <typename T> QHash<QString, Call*> CallModel<T>::activeCalls;
template  <typename T> QHash<QString, Call*> CallModel<T>::historyCalls;
template  <typename T> QHash<Call*, T*> CallModel<T>::privateCallList_call;
template  <typename T> QHash<QString, T*> CallModel<T>::privateCallList_callId;

/*****************************************************************************
 *                                                                           *
 *                               Constructor                                 *
 *                                                                           *
 ****************************************************************************/

///Retrieve current and older calls from the daemon, fill history and the calls TreeView and enable drag n' drop
template<typename T> CallModel<T>::CallModel(ModelType type)
{
   Q_UNUSED(type)
   init();
}

//Open the connection to the daemon and register this client
template<typename T> bool CallModel<T>::init() 
{
   if (!instanceInit) {
      registerCommTypes();
      InstanceInterface& instance = InstanceInterfaceSingleton::getInstance();
      instance.Register(getpid(), APP_NAME);
      
      //Setup accounts
      accountList = new AccountList(true);
   }
   instanceInit = true;
   return true;
}

//Fill the call list
//@warning This solution wont scale to multiple call or history model implementation. Some static addCall + foreach for each call would be needed if this case ever become unavoidable
template<typename T> bool CallModel<T>::initCall()
{
   if (!callInit) {
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      QStringList callList = callManager.getCallList();
      foreach (QString callId, callList) {
         Call* tmpCall = Call::buildExistingCall(callId);
         activeCalls[tmpCall->getCallId()] = tmpCall;
         addCall(tmpCall);
      }
   
      QStringList confList = callManager.getConferenceList();
      foreach (QString confId, confList) {
          addConference(confId);
      }
   }
   callInit = true;
   return true;
}

//Fill the history list
//@warning This solution wont scale to multiple call or history model implementation. Some static addCall + foreach for each call would be needed if this case ever become unavoidable
template<typename T> bool CallModel<T>::initHistory()
{
   if (!historyInit) {
      ConfigurationManagerInterface& configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
      MapStringString historyMap = configurationManager.getHistory().value();
      qDebug() << "Call History = " << historyMap;
      QMapIterator<QString, QString> i(historyMap);
      while (i.hasNext()) {
         i.next();
         uint startTimeStamp = i.key().toUInt();
         QStringList param = i.value().split("|");
         QString type2 = param[0];
         QString number = param[1];
         QString name = param[2];
         uint stopTimeStamp = param[3].toUInt();
         QString account = param[4];
         historyCalls[QString::number(startTimeStamp)] = Call::buildHistoryCall(generateCallId(), startTimeStamp, stopTimeStamp, account, name, number, type2);
         addCall(historyCalls[QString::number(startTimeStamp)]);
      }
   }
   historyInit = true;
   return true;
}


/*****************************************************************************
 *                                                                           *
 *                         Access related functions                          *
 *                                                                           *
 ****************************************************************************/

///Return the active call count
template<typename T> int CallModel<T>::size() 
{
   return activeCalls.size();
}

///Return a call corresponding to this ID or NULL
template<typename T> Call* CallModel<T>::findCallByCallId(QString callId) 
{
   return activeCalls[callId];
}

///Return the action call list
template<typename T> QList<Call*> CallModel<T>::getCallList() 
{
   QList<Call*> callList;
   foreach(Call* call, activeCalls) {
      callList.push_back(call);
   }
   return callList;
}


/*****************************************************************************
 *                                                                           *
 *                            Call related code                              *
 *                                                                           *
 ****************************************************************************/

///Add a call in the model structure, the call must exist before being added to the model
template<typename T> Call* CallModel<T>::addCall(Call* call, Call* parent) 
{
   Q_UNUSED(parent)
   InternalCallModelStruct* aNewStruct = new InternalCallModelStruct;
   aNewStruct->call_real = call;
   aNewStruct->conference = false;
   
   privateCallList_call[call] = aNewStruct;
   privateCallList_callId[call->getCallId()] = aNewStruct;

   //setCurrentItem(callItem);
   
   return call;
}

///Create a new dialing call from peer name and the account ID
template<typename T> Call* CallModel<T>::addDialingCall(const QString & peerName, QString account)
{
   Call* call = Call::buildDialingCall(generateCallId(), peerName, account);
   activeCalls[call->getCallId()] = call;
   addCall(call);
   selectItem(call);
   return call;
}

///Create a new incomming call when the daemon is being called
template<typename T> Call* CallModel<T>::addIncomingCall(const QString & callId)
{
   Call* call = Call::buildIncomingCall(callId);
   activeCalls[call->getCallId()] = call;
   addCall(call);
   selectItem(call);
   return call;
}

///Create a ringing call
template<typename T> Call* CallModel<T>::addRingingCall(const QString & callId)
{
   Call* call = Call::buildRingingCall(callId);
   activeCalls[call->getCallId()] = call;
   addCall(call);
   selectItem(call);
   return call;
}

///Generate a new random call unique identifier (callId)
template<typename T> QString CallModel<T>::generateCallId()
{
   int id = qrand();
   QString res = QString::number(id);
   return res;
}


/*****************************************************************************
 *                                                                           *
 *                         Conference related code                           *
 *                                                                           *
 ****************************************************************************/

///Add a new conference, get the call list and update the interface as needed
template<typename T> Call* CallModel<T>::addConference(const QString & confID) 
{
   qDebug() << "Notified of a new conference " << confID;
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   QStringList callList = callManager.getParticipantList(confID);
   qDebug() << "Paticiapants are:" << callList;
   
   if (!callList.size()) {
      qDebug() << "This conference (" + confID + ") contain no call";
      return 0;
   }

   if (!privateCallList_callId[callList[0]]) {
      qDebug() << "Invalid call";
      return 0;
   }
   Call* newConf =  new Call(confID, privateCallList_callId[callList[0]]->call_real->getAccountId());
   
   InternalCallModelStruct* aNewStruct = new InternalCallModelStruct;
   aNewStruct->call_real = newConf;
   aNewStruct->conference = true;
   
   QTreeWidgetItem* confItem = new QTreeWidgetItem();
   aNewStruct->treeItem = confItem;
   
   privateCallList_call[newConf] = aNewStruct;
   privateCallList_callId[newConf->getConfId()] = aNewStruct; //WARNING It may break something is it is done wrong
   
   return newConf;
}

///Join two call to create a conference, the conference will be created later (see addConference)
template<typename T> bool CallModel<T>::createConferenceFromCall(Call* call1, Call* call2) 
{
  qDebug() << "Joining call: " << call1->getCallId() << " and " << call2->getCallId();
  CallManagerInterface &callManager = CallManagerInterfaceSingleton::getInstance();
  callManager.joinParticipant(call1->getCallId(),call2->getCallId());
  return true;
}

///Add a new participant to a conference
template<typename T> bool CallModel<T>::addParticipant(Call* call2, Call* conference) 
{
   if (conference->isConference()) {
      CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
      callManager.addParticipant(call2->getCallId(), conference->getConfId());
      return true;
   }
   else {
      qDebug() << "This is not a conference";
      return false;
   }
}

///Remove a participant from a conference
template<typename T> bool CallModel<T>::detachParticipant(Call* call) 
{
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   callManager.detachParticipant(call->getCallId());
   return true;
}

///Merge two conferences
template<typename T> bool CallModel<T>::mergeConferences(Call* conf1, Call* conf2) 
{
   CallManagerInterface& callManager = CallManagerInterfaceSingleton::getInstance();
   callManager.joinConference(conf1->getConfId(),conf2->getConfId());
   return true;
}

///Executed when the daemon signal a modification in an existing conference. Update the call list and update the TreeView
template<typename T> bool CallModel<T>::conferenceChanged(const QString& confId, const QString& state) 
{
   Q_UNUSED(state)
   
   if (!privateCallList_callId[confId]) {
      qDebug() << "The conference does not exist";
      return false;
   }
   
   if (!privateCallList_callId[confId]->treeItem) {
      qDebug() << "The conference item does not exist";
      return false;
   }
   return true;
}

///Remove a conference from the model and the TreeView
template<typename T> void CallModel<T>::conferenceRemoved(const QString &confId) 
{
   qDebug() << "Ending conversation containing " << privateCallList_callId[confId]->treeItem->childCount() << " participants";
   privateCallList_call.remove(privateCallList_callId[confId]->call_real);
}


/*****************************************************************************
 *                                                                           *
 *                           History related code                            *
 *                                                                           *
 ****************************************************************************/

///Return a list of all previous calls
template<typename T> MapStringString CallModel<T>::getHistoryMap() 
{
   MapStringString toReturn;
   foreach(Call* call, historyCalls) {
      toReturn[historyCalls.key(call)] = Call::getTypeFromHistoryState(call->getHistoryState()) + "|" + call->getPeerPhoneNumber() + "|" + call->getPeerName() + "|" + call->getStopTimeStamp() + "|" + call->getAccountId();
   }
   return toReturn;
}

/*****************************************************************************
 *                                                                           *
 *                           Account related code                            *
 *                                                                           *
 ****************************************************************************/

//Return the current account id (do not put in the cpp file)
template<typename T> QString CallModel<T>::getCurrentAccountId()
{
   Account* firstRegistered = getCurrentAccount();
   if(firstRegistered == NULL) {
      return QString();
   }
   else {
      return firstRegistered->getAccountId();
   }
}


      //Return the current account
template<typename T> Account* CallModel<T>::getCurrentAccount()
{
   Account* priorAccount = accountList->getAccountById(priorAccountId);
      if(priorAccount && priorAccount->getAccountDetail(ACCOUNT_STATUS) == ACCOUNT_STATE_REGISTERED ) {
         return priorAccount;
   }
   else {
      qDebug() << "Returning the first account" << accountList->size();
      return accountList->firstRegisteredAccount();
   }
}

//Return a list of registered accounts
template<typename T> AccountList* CallModel<T>::getAccountList()
{
   return accountList;
}

//Return the previously used account ID
template<typename T> QString CallModel<T>::getPriorAccoundId() 
{
   return priorAccountId;
}

//Set the previous account used
template<typename T> void CallModel<T>::setPriorAccountId(QString value) {
   priorAccountId = value;
}

class CallModelConvenience : public CallModel<InternalCallModelStruct>
{
   public:
      CallModelConvenience(ModelType type) : CallModel<InternalCallModelStruct>(type) {}
};
#endif
