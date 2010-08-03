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

#include "CallModel.hpp"

#endif
