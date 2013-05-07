#include "logging.h"

#include <QTextEdit>
#include <QFile>

static Logging *loggingInstance = NULL;
static QTextEdit *logWindow_ = NULL;
static bool loggingEnabled_ = true;
static QFile *logFile_ = NULL;

Logging &Logging::singleton()
{
    if (!loggingInstance)
        loggingInstance = new Logging();

    return *loggingInstance;
}

void Logging::registerHandler()
{
    qInstallMsgHandler(customMessageHandler);
}

void Logging::unregisterHandler()
{
    qInstallMsgHandler(0);
}

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

void Logging::customMessageHandler(QtMsgType type, const char *msg)
{
    if (!loggingEnabled_ && type != QtFatalMsg) return;

    QString message = msg;

    if (message.isEmpty())
        return;

    if (message.length() >= 8195)
        message.truncate(8195);

    switch (type)
    {
    case QtDebugMsg:
        if (logWindow_)
        {
            message = "<span style='color: blue'>" + message + "</span>";
            logWindow_->append(message);
        }
        else
            fprintf(stderr, "%s \n", msg);

        break;
    case QtCriticalMsg:
        if (logWindow_)
        {
            message = "<span style='color: red'>" + message + "</span>";
            logWindow_->append("<b>Critical:</b> " + message);
        }
        else
            fprintf(stderr, "Critical: %s \n", msg);

        break;
    case QtWarningMsg:
        if (logWindow_)
        {
            message = "<span style='color: red'>" + message + "</span>";
            logWindow_->append("<b>Warning:</b> " + message);
        }
        else
            fprintf(stderr, "Warning: %s \n", msg);

        break;
    case QtFatalMsg:
        if (logWindow_)
            logWindow_->append("<span style='color: red'><b>Fatal: " + message + "</b></span>");
        else
            fprintf(stderr, "Fatal: %s \n", msg);

        exit(1);
    }

    fflush(stderr);

    if (logFile_)
    {
        if (logFile_->isOpen() && logFile_->isWritable())
            logFile_->write(message.toUtf8() + "\n");
    }

    if (logWindow_)
        logWindow_->show();
}
