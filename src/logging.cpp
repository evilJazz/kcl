/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "KCL/logging.h"
#include "KCL/debug.h"

#include <QFile>

static Logging *loggingInstance = NULL;
static bool loggingEnabled_ = true;
static QFile *logFile_ = NULL;

#ifdef KCL_WIDGETS
#include <QTextEdit>
static QTextEdit *logWindow_ = NULL;
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

        logWindow_->setFont(font);
        logWindow_->setWindowTitle("Log Window");
        logWindow_->document()->setMaximumBlockCount(200);
        logWindow_->setReadOnly(true);
        logWindow_->show();
    }
}

void Logging::disableLogWindow()
{
    if (logWindow_)
    {
        QTextEdit *temp = logWindow_;
        logWindow_ = NULL;
        delete temp;
    }
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
    const char *msg = msgText.toLocal8Bit().constData();
    QString message = msgText;
#else
    QString message = msg;
#endif

    if (message.length() >= 8195)
        message.truncate(8195);

    switch (type)
    {
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
