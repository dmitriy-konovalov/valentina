#ifndef DRW_TEXTCODEC_H
#define DRW_TEXTCODEC_H

#include <string>
#include <QtGlobal>
#include "../drw_base.h"

class QTextCodec;
class QStringList;
template <class Key, class T> class QMap;

class DRW_TextCodec
{
public:
    DRW_TextCodec();
    std::string fromUtf8(const std::string &s);
    std::string toUtf8(const std::string &s);
    int getVersion() const {return version;}
    void setVersion(const std::string &v, bool dxfFormat);
    void setVersion(DRW::Version v, bool dxfFormat);
    void setCodePage(const std::string &c, bool dxfFormat);
    std::string getCodePage() const {return cp;}

    static QMap<QString, QStringList> DXFCodePageMap();
    static QTextCodec* CodecForName(const QString &name);

private:
    std::string correctCodePage(const std::string& s);

private:
    Q_DISABLE_COPY(DRW_TextCodec)
    DRW::Version version{DRW::UNKNOWNV};
    std::string cp{};
    QTextCodec *conv{nullptr};
};

#endif // DRW_TEXTCODEC_H
