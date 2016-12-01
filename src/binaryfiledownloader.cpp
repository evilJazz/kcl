#include "KCL/binaryfiledownloader.h"

BinaryFileDownloader::BinaryFileDownloader(QObject *parent) :
    WebCall(parent)
{
    connect(this, SIGNAL(success(QByteArray)), this, SIGNAL(downloaded(QByteArray)));
}

BinaryFileDownloader::~BinaryFileDownloader()
{
}
