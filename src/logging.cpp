/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

#include "KCL/logging.h"
#include "KCL/debug.h"

#include <QFile>
#include <QObject>
#include <QPointer>

static Logging *loggingInstance = NULL;
static bool loggingEnabled_ = true;
static QFile *logFile_ = NULL;

#ifdef KCL_WIDGETS
#include <QTextEdit>
static QPointer<QTextEdit> logWindow_ = NULL;
#endif

Logging &Logging::singleton()
{
    if (!loggingInstance)
        loggingInstance = new Logging();

    return *loggingInstance;
}

void Logging::registerHandler()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qInstallMessageHandler(customMessageHandler);
#else
    qInstallMsgHandler(customMessageHandler);
#endif

    kaInstallMessageHandler(kaCustomMessageHandler);
}

void Logging::unregisterHandler()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qInstallMessageHandler(0);
#else
    qInstallMsgHandler(0);
#endif

    kaInstallMessageHandler(NULL);
}

#ifdef KCL_WIDGETS
void Logging::enableLogWindow()
{
    if (!logWindow_)
    {
        logWindow_ = new QTextEdit();

        QFont font = logWindow_->font();
        font.setFamily("Courier");
        font.setPixelSize(10);

        logWindow_->setAttribute(Qt::WA_DeleteOnClose, true);
        logWindow_->setFont(font);
        logWindow_->setWindowTitle("Log Window");
        logWindow_->document()->setMaximumBlockCount(1000);
        logWindow_->resize(QSize(800, 500));
        logWindow_->setReadOnly(true);
        logWindow_->show();

        connect(logWindow_.data(), SIGNAL(destroyed()), this, SLOT(disableLogWindow()));

        emit logWindowEnabledChanged();
    }
}

void Logging::disableLogWindow()
{
    if (logWindow_)
    {
        QTextEdit *temp = logWindow_;
        logWindow_ = NULL;
        temp->deleteLater();

        emit logWindowEnabledChanged();
    }
}

bool Logging::isLogWindowEnabled() const
{
    return !logWindow_.isNull();
}

#else

bool Logging::isLogWindowEnabled() const
{
    return false;
}

#endif

void Logging::enableLogFile(const QString &fileName)
{
    if (!logFile_)
        logFile_ = new QFile();

    if (logFile_->isOpen())
        logFile_->close();

    logFile_->setFileName(fileName);

    if (logFile_->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered))
        logFile_->seek(logFile_->size());
}

void Logging::disableLogFile()
{
    if (logFile_)
    {
        QFile *temp = logFile_;
        logFile_ = NULL;
        temp->close();
        delete temp;
    }
}

bool Logging::isEnabled() const
{
    return (logFile_ != NULL);
}

QString Logging::logFileName() const
{
    return (logFile_ ? logFile_->fileName() : QString::null);
}

Logging::Logging()
{
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void Logging::customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msgText)
#else
void Logging::customMessageHandler(QtMsgType type, const char *msg)
#endif
{
    if (!loggingEnabled_ && type != QtFatalMsg) return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QByteArray msgArray = msgText.toLocal8Bit(); // Store transient data in local variable...
    const char *msg = msgArray.constData(); // ...or content gets popped from stack later on.
    QString message = msgText;
#else
    QString message = msg;
#endif

    if (message.length() >= 8195)
        message.truncate(8195);

    switch (type)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case QtInfoMsg:
#endif
    case QtDebugMsg:
#ifdef KCL_WIDGETS
        if (logWindow_)
        {
            message = "<span style='color: blue'>" + message + "</span>";
            logWindow_->append(message);
        }
        else
#endif
            fprintf(stderr, "%s \n", msg);

        break;
    case QtCriticalMsg:
#ifdef KCL_WIDGETS
        if (logWindow_)
        {
            message = "<span style='color: red'>" + message + "</span>";
            logWindow_->append("<b>Critical:</b> " + message);
        }
        else
#endif
            fprintf(stderr, "Critical: %s \n", msg);

        break;
    case QtWarningMsg:
#ifdef KCL_WIDGETS
        if (logWindow_)
        {
            message = "<span style='color: red'>" + message + "</span>";
            logWindow_->append("<b>Warning:</b> " + message);
        }
        else
#endif
            fprintf(stderr, "Warning: %s \n", msg);

        break;
    case QtFatalMsg:
#ifdef KCL_WIDGETS
        if (logWindow_)
            logWindow_->append("<span style='color: red'><b>Fatal: " + message + "</b></span>");
        else
#endif
            fprintf(stderr, "Fatal: %s \n", msg);

        exit(1);
    }

    fflush(stderr);

    if (logFile_)
    {
        if (logFile_->isOpen() && logFile_->isWritable())
        {
            logFile_->write(message.toUtf8() + "\n");
            logFile_->flush();
        }
    }

#ifdef KCL_WIDGETS
    if (logWindow_)
        logWindow_->show();
#endif
}

void Logging::kaCustomMessageHandler(const QString &msg)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    customMessageHandler(QtDebugMsg, QMessageLogContext(), msg);
#else
    customMessageHandler(QtDebugMsg, msg.toLocal8Bit());
#endif
}
