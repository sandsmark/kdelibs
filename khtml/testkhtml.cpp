// programm to test the new khtml implementation

#define WIDGET
#define BROWSER

#include "khtmltokenizer.h"
#include "khtmldecoder.h"
#include "kapp.h"
#include "qfile.h"
#include "html_document.h"
#include "khtmltokenizer.h"
#include "khtml.h"
// to be able to delete a static protected member pointer in kbrowser...
// just for memory debugging
#define protected public
#include "kbrowser.h"
#undef protected
#include "qpushbutton.h"
#include "khtmldata.h"
#include "khtmlfont.h"
#include "khtmlio.h"
#include "kjs.h"
#include <qcursor.h>

#ifdef BROWSER
class TestBrowser : public KBrowser {
public:
  TestBrowser() { kjs = new KJSWorld(this); }
  ~TestBrowser() { delete kjs; }
  virtual void executeScript(const QString &c) { kjs->evaluate(c); }
private:
  KJSWorld *kjs;
};
#endif

int main(int argc, char *argv[])
{
    char data[1024];

    KApplication a(argc, argv);

#ifdef BROWSER
    TestBrowser *doc = new TestBrowser;
    doc->resize(800,500);

    a.setTopWidget(doc);
    doc->openURL(argv[1]);
    a.setTopWidget(doc);
    doc->show();
    doc->setURLCursor(QCursor(PointingHandCursor));
#else
#ifdef WIDGET
    KHTMLWidget *doc = new KHTMLWidget;
    doc->resize(500,300);

    a.setTopWidget(doc);
    doc->begin();
#else
    HTMLDocument doc;
    doc->open();
    //   KHTMLParser p(0,0);
    //   HTMLTokenizer doc(0);
    //   doc.begin();
#endif
    QFile file(argv[1]);
    file.open(IO_ReadOnly);


    QString decoded;

    printf("start reading\n");
    while(file.readLine(data, 1023) != -1)
    {
	//printf("passing %s\n", decoded.ascii());
	//printf("passing %s\n", data);
	doc->write(data);
    }

#ifdef WIDGET
    doc->end();
    doc->show();
#else
    doc->close();
#endif
#endif
    printf("finished!\n");

    QPushButton *p = new QPushButton(0, 0);
    QWidget::connect(p, SIGNAL(pressed()), &a, SLOT(quit()));
    p->show();
    a.exec();

    delete p;
    delete doc;

    if(pSettings) delete pSettings;
    if(pFontManager) delete pFontManager;
    KHTMLCache::clear();
    if(KBrowser::lstViews) delete KBrowser::lstViews;
}




